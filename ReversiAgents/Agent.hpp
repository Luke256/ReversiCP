# pragma once
# include "../ReversiEngine.hpp"

class ReversiAgent
{
private:
	bool m_abort;
public:
	using Pos = std::pair<int32_t, int32_t>;

	ReversiAgent(): m_abort(false)
	{
	}
	virtual Pos play(const Reversi::ReversiEngine &engine) = 0;
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
	const int32_t inf = 1000000;
	bool isAborted() { return m_abort; }
};
