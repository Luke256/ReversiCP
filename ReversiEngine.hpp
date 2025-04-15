#pragma once

namespace Reversi
{


	class ReversiEngine
	{
	private:
		uint64 m_blacks, m_whites;
		bool m_blackTurn;

		/// @brief 二次元座標をビットに変換します
		/// @param x 左から何番目か
		/// @param y 上から何番目か
		/// @return ビッドボードでのマスク
		uint64 pos2bit(uint32 x, uint32 y) const;

		uint64 transfer(uint64 put, uint32 dir) const;

	public:
		ReversiEngine();

		void reset();

		uint64 getLegals(bool inverseTurn = false) const;

		bool place(uint32 x, uint32 y);

		void getBoard(Array<int32>& board) const;

		void pass();

		bool isBlackTurn() const;

		int32 getNBlacks() const;

		int32 getNWhites() const;

		bool isFinished();
	};

	void bit2boad(const uint64& bit, Array<int32>& board);

};
