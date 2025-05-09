# pragma once

# include "Agent.hpp"

class AlphaBetaAgent : public ReversiAgent
{
public:
	AlphaBetaAgent();
	Point play(const Reversi::ReversiEngine& engine) override;
	void reset_child() override;
private:
	struct LegalState
	{
		int32 score, idx;
		inline bool operator<(const LegalState& a)
		{
			if (score != a.score) return score < a.score;
			return idx < a.idx;
		}

		inline bool operator>(const LegalState& a)
		{
			if (score != a.score) return score > a.score;
			return idx > a.idx;
		}
	};

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

	int32 negaAlpha(Reversi::ReversiEngine& engine, int32 depth, bool passed, int32 alpha, int32 beta);

	inline int32 eval(const Reversi::ReversiEngine& engine) const;


	/// @brief 合法手をざっとした評価の高い順に並べて返します
	/// @param engine リバーシエンジン
	/// @param legalList 適当なリスト (スコア, 手)
	inline void getSortedLegals(Reversi::ReversiEngine& engine, Array<LegalState>& legalList)
	{
		const uint64 legals = engine.getLegals();
		const uint64 prevBlacks = engine.getBlacks(), prevWhites = engine.getWhites();
		const bool prevTurn = engine.isBlackTurn();

		legalList.assign(std::popcount(legals), { 0,0 });

		uint64 mask = 0x8000000000000000;
		int32 idx = 0;
		for (int32 i : step(64))
		{
			if (not (legals & mask))
			{
				mask >>= 1;
				continue;
			}

			engine.place(i & 7, i >> 3);

			if (transTablePrev.contains(engine.getTupleState()))
			{
				legalList[idx++] = { 1000 - transTablePrev[engine.getTupleState()], i };
			}
			else
			{
				legalList[idx++] = { -eval(engine), i };
			}

			engine.setState(prevBlacks, prevWhites, prevTurn);
			mask >>= 1;
		}

		legalList.rsort();
	}

	int32 callCnt;
	HashTable<std::tuple<uint64, uint64, bool>, int32> transTable, transTablePrev;
};
