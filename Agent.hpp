# pragma once
# include "ReversiEngine.hpp"

class ReversiAgent
{
public:
	virtual Point play(const Reversi::ReversiEngine &engine) = 0;
	virtual void reset() = 0;
};
