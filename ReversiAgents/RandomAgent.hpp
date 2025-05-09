#pragma once
# include "Agent.hpp"

class RandomAgent : public ReversiAgent
{
public:
	RandomAgent() {}
	Pos play(const Reversi::ReversiEngine& engine) override
	{
		uint64 uintlegals = engine.getLegals();
		Array<int32> legals;
		uint64 mask = 0x8000000000000000;
		for (int32 i : step(64))
		{
			if (uintlegals & mask)
			{
				legals << i;
			}
			mask >>= 1;
		}

		int32 p = Sample(legals);
		return { p % 8, p / 8 };
	}
	void reset_child() override {}
};
