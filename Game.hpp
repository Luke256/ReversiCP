#pragma once

# include "Main.hpp"
# include "ReversiEngine.hpp"

Optional<Point> updateHumanPlayer(const Array<int32>&, const Array<int32>&, bool);
Optional<Point> updateRandomPlayer(const Array<int32>&, const Array<int32>& legals, bool);

class Game : public MyApp::Scene
{
private:
	using _PlayFunc = std::function<Optional<Point>(const Array<int32>&, const Array<int32>& , bool)>;
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
