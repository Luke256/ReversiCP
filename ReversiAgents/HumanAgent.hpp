# pragma once
# include "Agent.hpp"
# include "../Main.hpp"

class HumanAgent : public ReversiAgent
{
private:
	Optional<Pos> playStep()
	{
		const int32_t boardW = AppData::Width / 2 - 20;
		const int32_t boardH = AppData::Width / 2 - 20;
		constexpr int32_t boardSize = Min(boardW, boardH);
		const double cellSize = boardSize / 8.0;
		const Vec2 boardCenter{ AppData::Width / 4, AppData::Height / 2 };
		for (int32_t i : step(64))
		{
			Vec2 pos{ boardCenter.x - boardSize / 2 + cellSize * (i % 8), boardCenter.y - boardSize / 2 + cellSize * (i / 8) };
			RectF region{ pos, cellSize };

			if (region.leftClicked())
			{
				return std::make_pair(i % 8, i / 8);
			}
		}
		return none;
	}

public:
	Pos play(const Reversi::ReversiEngine&) override
	{
		Optional<Pos>pos;
		while(not pos and not isAborted())
		{
			pos = playStep();
			System::Sleep(10);
		}
		if (pos) return *pos;
		else return { 0,0 };
	}

	void reset_child() override {}
};
