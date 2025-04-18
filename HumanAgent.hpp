# pragma once
# include "Agent.hpp"
# include "Main.hpp"

class HumanAgent : public ReversiAgent
{
private:
	Optional<Point> playStep()
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

public:
	Point play(const Reversi::ReversiEngine&) override
	{
		Optional<Point>pos;
		while(not pos and not isAborted())
		{
			pos = playStep();
			System::Sleep(10);
		}
		if (pos) return *pos;
		else return Point{ 0,0 };
	}

	void reset_child() override {}
};
