#pragma once

# include "Main.hpp"
# include "ReversiEngine.hpp"

using _PlayFunc = std::function<Optional<Point>(const Reversi::ReversiEngine&, bool)>;

Optional<Point> updateHumanPlayer(const Reversi::ReversiEngine&, bool);
Optional<Point> updateRandomPlayer(const Reversi::ReversiEngine& engine, bool);

template<class AgentType>
_PlayFunc getAgentPlayer()
{
	auto agent_ptr = std::make_shared<AgentType>();
	AsyncTask<Point> task;
	auto F = [agent_ptr, task](const Reversi::ReversiEngine& engine, bool firstFime)
		{
			if (firstFime)
			{
				agent_ptr->reset();
				task = Async(agent_ptr->play, engine);
			}

			if (task.isReady())
			{
				return task.get();
			}
			return none;
		};

	return F;
}

class Game : public MyApp::Scene
{
private:
	struct playerInfo
	{
		ListBoxState type;
		bool active = true;
	};

	const Array<String> PlayerTypes = {
		U"Human", U"Random"
	};
	const Array<_PlayFunc> PlayerFunctions = {
		updateHumanPlayer,
		updateRandomPlayer,
	};
	const int32 boardW = AppData::Width / 2 - 20;
	const int32 boardH = AppData::Width / 2 - 20;
	const int32 boardSize = Min(boardW, boardH);
	const double cellSize = boardSize / 8.0;
	const Vec2 boardCenter{ AppData::Width / 4, AppData::Height / 2 };
	const int32 UIW = AppData::Width / 4 - 15;

	playerInfo p1Info, p2Info;

	Reversi::ReversiEngine engine;
	Array<int32> boardState, legals, subjectiveState;
	bool isFirstFrame;

public:
	Game(const InitData& init);
	void update() override;
	void draw() const override;
	void reset();
	void updatePlayers();
};
