#include "Game.hpp"

Game::Game(const InitData& init):
	IScene(init), boardState(64), legals(64)
{
	p1Info.type = ListBoxState{ PlayerTypes };
	p2Info.type = ListBoxState{ PlayerTypes };
	p1Info.type.selectedItemIndex = 0;
	p2Info.type.selectedItemIndex = 0;

	reset();
}

void Game::update()
{
	// players
	updatePlayers();

	// UI

	if (SimpleGUI::ListBox(p1Info.type, { AppData::Width / 2 + 10, AppData::Height / 2 + 10 }, UIW, AppData::Height / 2 - 70))
	{
		p1Info.active = false;
	}
	if (SimpleGUI::ListBox(p2Info.type, { AppData::Width * 3 / 4 + 5, AppData::Height / 2 + 10 }, UIW, AppData::Height / 2 - 70))
	{
		p2Info.active = false;
	}

	if (SimpleGUI::Button(p1Info.active ? U"\U000F03E4" : U"\U000F040A", { AppData::Width / 2 + 10, AppData::Height - 50 }, UIW))
	{
		p1Info.active = !p1Info.active;
	}
	if (SimpleGUI::Button(p2Info.active ? U"\U000F03E4" : U"\U000F040A", { AppData::Width * 3 / 4 + 5, AppData::Height - 50 }, UIW))
	{
		p2Info.active = !p2Info.active;
	}
	if (SimpleGUI::Button(U"Reset", {AppData::Width / 2 + 10,10}, UIW * 2 + 10))
	{
		reset();
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

	Circle{ AppData::Width / 2 + 10 + UIW / 2, AppData::Height / 2 - 50, 48 }.draw(Palette::Black);
	Circle{ AppData::Width * 3 / 4 + 5 + UIW / 2, AppData::Height / 2 - 50, 48 }.draw(Palette::White);
	FontAsset(U"bold")(U"{}"_fmt(engine.getNBlacks())).drawAt(64, { AppData::Width / 2 + 10 + UIW / 2, AppData::Height / 2 - 50 }, Palette::White);
	FontAsset(U"bold")(U"{}"_fmt(engine.getNWhites())).drawAt(64, { AppData::Width * 3 / 4 + 5 + UIW / 2, AppData::Height / 2 - 50 }, Palette::Black);
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

	auto pos = PlayerFunctions[*player.type.selectedItemIndex](subjectiveState, legals, isFirstFrame);
	isFirstFrame = false;
	if (not pos) return;

	bool executed = engine.place(pos->x, pos->y);
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
	isFirstFrame = true;
}


Optional<Point> updateHumanPlayer(const Array<int32>&, const Array<int32>&, bool)
{
	const int32 boardW = AppData::Width / 2 - 20;
	const int32 boardH = AppData::Width / 2 - 20;
	constexpr int32 boardSize = Min(boardW, boardH);
	const double cellSize = boardSize / 8.0;
	const Vec2 boardCenter{ AppData::Width / 4, AppData::Height / 2 };
	for (int32 i : step(64))
	{
		Vec2 pos{ boardCenter.x - boardSize / 2 + cellSize * (i % 8), boardCenter.y - boardSize / 2 + cellSize * (i / 8) };
		RectF region{ pos, cellSize };

		if (region.leftClicked())
		{
			return Point{ i % 8, i / 8 };
		}
	}
	return none;
}

Optional<Point> updateRandomPlayer(const Array<int32>&, const Array<int32>& legals, bool)
{
	Array<Point>legalPosList;
	for (int32 i : step(64))
	{
		if (legals[i] == 0) continue;
		legalPosList << Point{ i % 8, i / 8 };
	}
	return Sample(legalPosList);
}
