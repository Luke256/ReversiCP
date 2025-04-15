# include "ReversiEngine.hpp"
# include <bitset>

namespace Reversi
{

	uint64 ReversiEngine::pos2bit(uint32 x, uint32 y) const
	{
		uint64 mask = 0x8000000000000000;
		mask >>= x;
		mask >>= (y << 3);
		return mask;
	}

	ReversiEngine::ReversiEngine() :
		m_blacks(0), m_whites(0), m_blackTurn(true)
	{
	}

	void ReversiEngine::reset()
	{
		m_blacks = pos2bit(4, 3) | pos2bit(3, 4);
		m_whites = pos2bit(3, 3) | pos2bit(4, 4);
		m_blackTurn = true;
	}

	uint64 ReversiEngine::getLegals(bool inverseTurn) const
	{
		const uint64& playerBoard = (m_blackTurn ^ inverseTurn) ? m_blacks : m_whites;
		const uint64& oppBoard = (m_blackTurn ^ inverseTurn) ? m_whites : m_blacks;
		const uint64 hMask = 0x7e7e7e7e7e7e7e7e & oppBoard;
		const uint64 vMask = 0x00FFFFFFFFFFFF00 & oppBoard;
		const uint64 edgeMask = 0x007e7e7e7e7e7e00 & oppBoard;
		const uint64 blank = ~(m_blacks | m_whites);

		uint64 tmp = 0, legals = 0;

		tmp = hMask & (playerBoard << 1);
		tmp |= hMask & (tmp << 1);
		tmp |= hMask & (tmp << 1);
		tmp |= hMask & (tmp << 1);
		tmp |= hMask & (tmp << 1);
		tmp |= hMask & (tmp << 1);
		legals |= blank & (tmp << 1);
		
		tmp = hMask & (playerBoard >> 1);
		tmp |= hMask & (tmp >> 1);
		tmp |= hMask & (tmp >> 1);
		tmp |= hMask & (tmp >> 1);
		tmp |= hMask & (tmp >> 1);
		tmp |= hMask & (tmp >> 1);
		legals |= blank & (tmp >> 1);
		
		tmp = vMask & (playerBoard >> 8);
		tmp |= vMask & (tmp >> 8);
		tmp |= vMask & (tmp >> 8);
		tmp |= vMask & (tmp >> 8);
		tmp |= vMask & (tmp >> 8);
		tmp |= vMask & (tmp >> 8);
		legals |= blank & (tmp >> 8);

		tmp = vMask & (playerBoard << 8);
		tmp |= vMask & (tmp << 8);
		tmp |= vMask & (tmp << 8);
		tmp |= vMask & (tmp << 8);
		tmp |= vMask & (tmp << 8);
		tmp |= vMask & (tmp << 8);
		legals |= blank & (tmp << 8);

		tmp = edgeMask & (playerBoard << 7);
		tmp |= edgeMask & (tmp << 7);
		tmp |= edgeMask & (tmp << 7);
		tmp |= edgeMask & (tmp << 7);
		tmp |= edgeMask & (tmp << 7);
		tmp |= edgeMask & (tmp << 7);
		legals |= blank & (tmp << 7);

		tmp = edgeMask & (playerBoard << 9);
		tmp |= edgeMask & (tmp << 9);
		tmp |= edgeMask & (tmp << 9);
		tmp |= edgeMask & (tmp << 9);
		tmp |= edgeMask & (tmp << 9);
		tmp |= edgeMask & (tmp << 9);
		legals |= blank & (tmp << 9);

		tmp = edgeMask & (playerBoard >> 7);
		tmp |= edgeMask & (tmp >> 7);
		tmp |= edgeMask & (tmp >> 7);
		tmp |= edgeMask & (tmp >> 7);
		tmp |= edgeMask & (tmp >> 7);
		tmp |= edgeMask & (tmp >> 7);
		legals |= blank & (tmp >> 7);

		tmp = edgeMask & (playerBoard >> 9);
		tmp |= edgeMask & (tmp >> 9);
		tmp |= edgeMask & (tmp >> 9);
		tmp |= edgeMask & (tmp >> 9);
		tmp |= edgeMask & (tmp >> 9);
		tmp |= edgeMask & (tmp >> 9);
		legals |= blank & (tmp >> 9);

		return legals;
	}

	bool ReversiEngine::place(uint32 x, uint32 y)
	{
		uint64& playerBoard = m_blackTurn ? m_blacks : m_whites;
		uint64& oppBoard = m_blackTurn ? m_whites : m_blacks;
		uint64 b = pos2bit(x, y);
		if ((getLegals() & b) == 0) return false;
		uint64 rev = 0;
		for (int dir = 0; dir < 8; ++dir)
		{
			uint64 rev_ = 0;
			uint64 mask = transfer(b, dir);
			while (mask != 0 && ((mask & oppBoard) != 0))
			{
				rev_ |= mask;
				mask = transfer(mask, dir);
			}
			if ((mask & playerBoard) != 0)
			{
				rev |= rev_;
			}
		}

		playerBoard ^= b | rev;
		oppBoard ^= rev;
		m_blackTurn = !m_blackTurn;

		return true;
	}

	uint64 ReversiEngine::transfer(uint64 put, uint32 dir) const
	{
		switch (dir) {
		case 0: //上
			return (put << 8) & 0xffffffffffffff00;
		case 1: //右上
			return (put << 7) & 0x7f7f7f7f7f7f7f00;
		case 2: //右
			return (put >> 1) & 0x7f7f7f7f7f7f7f7f;
		case 3: //右下
			return (put >> 9) & 0x007f7f7f7f7f7f7f;
		case 4: //下
			return (put >> 8) & 0x00ffffffffffffff;
		case 5: //左下
			return (put >> 7) & 0x00fefefefefefefe;
		case 6: //左
			return (put << 1) & 0xfefefefefefefefe;
		case 7: //左上
			return (put << 9) & 0xfefefefefefefe00;
		default:
			return 0;
		}
	}

	void ReversiEngine::getBoard(Array<int32>& board) const
	{
		uint64 mask = 0x8000000000000000;
		uint32 cnt = 0;
		while (cnt < 64)
		{
			if (mask & m_blacks) board[cnt++] = 1;
			else if (mask & m_whites) board[cnt++] = -1;
			else board[cnt++] = 0;
			mask >>= 1;
		}
	}

	void ReversiEngine::pass()
	{
		m_blackTurn = !m_blackTurn;
	}

	bool ReversiEngine::isBlackTurn() const
	{
		return m_blackTurn;
	}

	int32 ReversiEngine::getNBlacks() const
	{
		return std::popcount(m_blacks);
	}

	int32 ReversiEngine::getNWhites() const
	{
		return std::popcount(m_whites);
	}

	bool ReversiEngine::isFinished()
	{
		if (getLegals() != 0) return false;
		if (getLegals(true) != 0) return false;
		return true;
	}

	void bit2boad(const uint64& bit, Array<int32>& board)
	{
		uint64 mask = 0x8000000000000000;
		uint32 cnt = 0;
		while (cnt < 64)
		{
			if (mask & bit) board[cnt++] = 1;
			else board[cnt++] = 0;
			mask >>= 1;
		}
	}
}
