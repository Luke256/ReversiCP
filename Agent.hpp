# pragma once
# include "ReversiEngine.hpp"

class ReversiAgent
{
	virtual Point play(const Reversi::ReversiEngine &engine) = 0;
	virtual void reset() = 0;
};
