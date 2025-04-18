# pragma once
# include "ReversiEngine.hpp"

class ReversiAgent
{
private:
	bool m_abort;
public:
	ReversiAgent(): m_abort(false)
	{}
	virtual Point play(const Reversi::ReversiEngine &engine) = 0;
	virtual void reset_child() = 0;
	void reset()
	{
		m_abort = false;
		reset_child();
	}

	void abort()
	{
		m_abort = true;
	}
protected:
	bool isAborted() { return m_abort; }
};
