#pragma once

# include "Main.hpp"
# include "ReversiEngine.hpp"
# include "AgentRandom.hpp"

void genAgents(Array<std::shared_ptr<ReversiAgent>>& agents)
{
	agents << std::make_shared<RandomAgent>();
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
		//U"Human",
		U"Random",
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

public:
	Game(const InitData& init);
	void update() override;
	void draw() const override;
	void reset();
	void updatePlayers();
};
