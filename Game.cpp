# include "Game.hpp"

# include "ReversiAgents/RandomAgent.hpp"
# include "ReversiAgents/HumanAgent.hpp"
# include "ReversiAgents/GreedyAgent.hpp"
# include "ReversiAgents/MinMaxAgent.hpp"
# include "ReversiAgents/AlphaBetaAgent.hpp"

void genAgents(Array<std::shared_ptr<ReversiAgent>>& agents)
{
	agents << std::make_shared<HumanAgent>();
	agents << std::make_shared<RandomAgent>();
	agents << std::make_shared<GreedyAgent>();
	agents << std::make_shared<MinMaxAgent>();
	agents << std::make_shared<AlphaBetaAgent>();
}


Game::Game(const InitData& init):
	IScene(init), boardState(64), legals(64), runningStats(false)
{
	p1Info.type = ListBoxState{ PlayerTypes };
	p2Info.type = ListBoxState{ PlayerTypes };
	p1Info.type.selectedItemIndex = 0;
	p2Info.type.selectedItemIndex = 0;

	genAgents(p1agents);
	genAgents(p2agents);

	reset();
}

Game::~Game()
{
	if (playTask.isValid())
	{
		p1agents[*p1Info.type.selectedItemIndex]->abort();
		p2agents[*p2Info.type.selectedItemIndex]->abort();
		playTask.wait();
	}
}

void Game::update()
{
	// players
	updatePlayers();

	// UI
	updateUIs();

	// Stats
	if (runningStats)
	{
		updateStats();
	}
}

void Game::draw() const
{
	RectF{ Arg::center(boardCenter), (double)boardSize }.draw(Palette::Lightgreen);

	for (int32 i : step(64))
	{
		Vec2 pos{ boardCenter.x - boardSize / 2 + cellSize * (i % 8), boardCenter.y - boardSize / 2 + cellSize * (i / 8) };
		if (legals[i]) RectF{ pos, cellSize }.draw(Palette::White.withA(120));
		if (boardState[i] == 1) Circle{ pos + Vec2{ cellSize, cellSize } / 2, cellSize * 0.4 }.draw(Palette::Black);
		if (boardState[i] == -1) Circle{ pos + Vec2{ cellSize, cellSize } / 2, cellSize * 0.4 }.draw(Palette::White);
		RectF{ pos, cellSize }.drawFrame();
	}

	Circle{ AppData::Width / 2 + 10 + UIW / 2, AppData::Height / 2 - 50, 48 }.draw(Palette::Black).drawFrame(0, 3 * engine.isBlackTurn(), Palette::Orange);
	Circle{ AppData::Width * 3 / 4 + 5 + UIW / 2, AppData::Height / 2 - 50, 48 }.draw(Palette::White).drawFrame(0, 3 * (not engine.isBlackTurn()), Palette::Orange);
	FontAsset(U"bold")(U"{}"_fmt(engine.getNBlacks())).drawAt(64, { AppData::Width / 2 + 10 + UIW / 2, AppData::Height / 2 - 50 }, Palette::White);
	FontAsset(U"bold")(U"{}"_fmt(engine.getNWhites())).drawAt(64, { AppData::Width * 3 / 4 + 5 + UIW / 2, AppData::Height / 2 - 50 }, Palette::Black);

	{
		const auto t = Transformer2D(Mat3x2::Translate(AppData::Width / 2, 120));
		const int32 width = AppData::Width / 2;
		FontAsset(U"bold")(U"対局数: {}"_fmt(gameStats.games)).drawAt(width / 2, 30, ColorF{ 0.1 });
		FontAsset(U"bold")(U"黒: {}"_fmt(gameStats.p1Wins)).drawAt(width / 4, 100, ColorF{ 0.1 });
		FontAsset(U"bold")(U"分: {}"_fmt(gameStats.draws)).drawAt(width / 2, 100, ColorF{ 0.1 });
		FontAsset(U"bold")(U"白: {}"_fmt(gameStats.p2Wins)).drawAt(width * 3 / 4, 100, ColorF{ 0.1 });
	}
}

void Game::reset()
{
	engine.reset();
	engine.getBoard(boardState);
	Reversi::bit2boad(engine.getLegals(), legals);
	subjectiveState = boardState;
	isFirstFrame = true;
}

void Game::updatePlayers()
{
	const playerInfo& player = engine.isBlackTurn() ? p1Info : p2Info;
	if (engine.isFinished()) return;
	if (not player.active) return;

	if (isFirstFrame)
	{
		auto& agents_ptr = engine.isBlackTurn() ? p1agents : p2agents;
		if (playTask.isValid())
		{
			for (auto& agent : p1agents) agent->abort();
			for (auto& agent : p2agents) agent->abort();
			playTask.wait();
			playTask.get();
		}
		auto F = [&]() -> Point
			{
				return agents_ptr[*player.type.selectedItemIndex]->play(engine);
			};
		agents_ptr[*player.type.selectedItemIndex]->reset();
		playTask = Async(F);
		isFirstFrame = false;
	}

	if (not playTask.isReady()) return;

	isFirstFrame = true;

	Point pos = playTask.get();

	bool executed = engine.place(pos.x, pos.y);
	if (not executed) return;

	if (engine.getLegals() == 0ll)
	{
		engine.pass();
	}

	engine.getBoard(boardState);
	Reversi::bit2boad(engine.getLegals(), legals);
	subjectiveState = boardState;
	if (not engine.isBlackTurn())
	{
		for (auto i : step(64)) subjectiveState[i] *= -1;
	}
}

void Game::updateUIs()
{

	if (SimpleGUI::ListBox(p1Info.type, { AppData::Width / 2 + 10, AppData::Height / 2 + 10 }, UIW, AppData::Height / 2 - 70))
	{
		p1Info.active = p1Info.type.selectedItemIndex == 0;
		if (engine.isBlackTurn() and playTask.isValid())
		{
			for (auto& agent : p1agents) agent->abort();
			playTask.wait();
			playTask.get();
		}
		isFirstFrame = true;
	}
	if (SimpleGUI::ListBox(p2Info.type, { AppData::Width * 3 / 4 + 5, AppData::Height / 2 + 10 }, UIW, AppData::Height / 2 - 70))
	{
		p2Info.active = p2Info.type.selectedItemIndex == 0;
		if (not engine.isBlackTurn() and playTask.isValid())
		{
			for (auto& agent : p2agents) agent->abort();
			playTask.wait();
			playTask.get();
		}
		isFirstFrame = true;
	}

	if (SimpleGUI::Button(p1Info.active ? U"\U000F03E4" : U"\U000F040A", { AppData::Width / 2 + 10, AppData::Height - 50 }, UIW))
	{
		p1Info.active = !p1Info.active;
		if (engine.isBlackTurn())
		{
			isFirstFrame = true;
		}
	}
	if (SimpleGUI::Button(p2Info.active ? U"\U000F03E4" : U"\U000F040A", { AppData::Width * 3 / 4 + 5, AppData::Height - 50 }, UIW))
	{
		p2Info.active = !p2Info.active;
		if (not engine.isBlackTurn())
		{
			isFirstFrame = true;
		}
	}
	if (SimpleGUI::Button(U"Reset", { AppData::Width / 2 + 10,10 }, UIW * 2 + 10, not runningStats))
	{
		reset();
	}
	if (SimpleGUI::Button(runningStats ? U"Stop Stats" : U"Run Stats", { AppData::Width / 2 + 10, 60 }, UIW))
	{
		runningStats = !runningStats;
		if (runningStats)
		{
			reset();
			p1Info.active = true;
			p2Info.active = true;
		}
		else
		{
			p1Info.active = false;
			p2Info.active = false;
		}
	}
	if (SimpleGUI::Button(U"Reset Stats", { AppData::Width * 3 / 4 + 5, 60 }, UIW))
	{
		gameStats.reset();
	}
}

void Game::updateStats()
{
	if (engine.isFinished())
	{
		const int32 blacks = engine.getNBlacks();
		const int32 whites = engine.getNWhites();

		gameStats.games++;
		if (blacks > whites) gameStats.p1Wins++;
		if (blacks < whites) gameStats.p2Wins++;
		if (blacks == whites) gameStats.draws++;

		reset();
	}
}
