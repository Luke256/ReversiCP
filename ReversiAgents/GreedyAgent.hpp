# pragma once

# include "Agent.hpp"

class GreedyAgent : public ReversiAgent
{
public:
	GreedyAgent();
	Pos play(const Reversi::ReversiEngine& engine) override;
	void reset_child() override;
private:
	const Grid<int32>valPerCell = {
		{ 2714, 147, 69, -18, -18, 69, 147, 2714 },
		{ 147, -577, -186, -153, -153, -186, -577, 147 },
		{ 69, -186, -379, -122, -122, -379, -186, 69 },
		{ -18, -153, -122, -169, -169, -122, -153, -18 },
		{ -18, -153, -122, -169, -169, -122, -153, -18 },
		{ 69, -186, -379, -122, -122, -379, -186, 69 },
		{ 147, -577, -186, -153, -153, -186, -577, 147 },
		{ 2714, 147, 69, -18, -18, 69, 147, 2714 },
	};

	const Grid<int32>rowValues = _initRowValues();

	constexpr Grid<int32> _initRowValues()
	{
		Grid<int32>res{ 8, (1 << 8) };
		for (int32 i : step(8))
		{
			for (int32 bit : step(1 << 8))
			{
				for (int32 j : step(8))
				{
					if (bit & (1 << (7 - j)))
					{
						res[i][bit] += valPerCell[i][j];
					}
				}
			}
		}
		return res;
	}

	inline int32 eval(const Reversi::ReversiEngine& engine) const;
};
