#pragma once

# include "Main.hpp"
# include "ReversiEngine.hpp"
# include "ReversiAgents/Agent.hpp"

void genAgents(Array<std::shared_ptr<ReversiAgent>>& agents);

class Game : public MyApp::Scene
{
private:
	struct playerInfo
	{
		ListBoxState type;
		bool active = true;
	};

	struct Statistics
	{
		int32 games = 0, p1Wins = 0, p2Wins = 0, draws = 0;
		void reset() { games = p1Wins = p2Wins = draws = 0; }
	};

	const Array<String> PlayerTypes = {
		U"Human",
		U"Random",
		U"Greedy",
		U"MinMax",
		U"AlphaBeta",
	};

	const int32 boardW = AppData::Width / 2 - 20;
	const int32 boardH = AppData::Width / 2 - 20;
	const int32 boardSize = Min(boardW, boardH);
	const double cellSize = boardSize / 8.0;
	const Vec2 boardCenter{ AppData::Width / 4, AppData::Height / 2 };
	const int32 UIW = AppData::Width / 4 - 15;

	playerInfo p1Info, p2Info;
	Array<std::shared_ptr<ReversiAgent>> p1agents, p2agents;

	Reversi::ReversiEngine engine;
	Array<int32> boardState, legals, subjectiveState;
	bool isFirstFrame;
	AsyncTask<Point> playTask;

	Statistics gameStats;
	bool runningStats;

public:
	Game(const InitData& init);
	~Game();
	void update() override;
	void draw() const override;
	void reset();
	void updatePlayers();
	void updateUIs();
	void updateStats();
};
