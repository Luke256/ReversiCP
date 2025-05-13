# pragma once

# include "Agent.hpp"
# include <unordered_map>

class AlphaBetaAgent : public ReversiAgent
{
public:
	AlphaBetaAgent();
	Pos play(const Reversi::ReversiEngine& engine) override;
	void reset_child() override;
private:
	struct LegalState
	{
		int32_t score, idx;
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

	const std::vector<int32_t>valPerCell = {
		2714, 147, 69, -18, -18, 69, 147, 2714,
		147, -577, -186, -153, -153, -186, -577, 147,
		69, -186, -379, -122, -122, -379, -186, 69,
		-18, -153, -122, -169, -169, -122, -153, -18,
		-18, -153, -122, -169, -169, -122, -153, -18,
		69, -186, -379, -122, -122, -379, -186, 69,
		147, -577, -186, -153, -153, -186, -577, 147,
		2714, 147, 69, -18, -18, 69, 147, 2714,
	};

	const std::vector<int32_t>rowValues = _initRowValues();
	const int32_t MAX_CALL_CNT = 100000;

	std::vector<int32_t> _initRowValues()
	{
		//Grid<int32_t>res{ 8, (1 << 8) };
		std::vector<int32_t> res(1 << 11);
		int32_t i, bit, j;
		for (i = 0; i < 8; i++)
		{
			for (bit = 0; bit < (1 << 8); bit++)
			{
				for (j = 0; j < 8; j++)
				{
					if (bit & (1 << (7 - j)))
					{
						res[(i << 8) + bit] += valPerCell[(i << 3) + j];
					}
				}
			}
		}
		return res;
	}

	int32_t negaAlpha(Reversi::ReversiEngine& engine, int32_t depth, bool passed, int32_t alpha, int32_t beta);

	inline int32_t eval(const Reversi::ReversiEngine& engine) const;


	/// @brief 合法手をざっとした評価の高い順に並べて返します
	/// @param engine リバーシエンジン
	/// @param legalList 適当なリスト (スコア, 手)
	inline void getSortedLegals(Reversi::ReversiEngine& engine, std::vector<LegalState>& legalList)
	{
		const uint64_t legals = engine.getLegals();
		const uint64_t prevBlacks = engine.getBlacks(), prevWhites = engine.getWhites();
		const bool prevTurn = engine.isBlackTurn();

		legalList.assign(std::popcount(legals), { 0,0 });

		uint64_t mask = 0x8000000000000000;
		int32_t idx = 0, i;
		for (i = 0; i < 64; i++)
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

		std::sort(legalList.rbegin(), legalList.rend());
	}

	int32_t callCnt;
	std::unordered_map<std::tuple<uint64_t, uint64_t, bool>, int32_t, Reversi::TupleHash> transTable, transTablePrev;
	//HashTable<std::tuple<uint64_t, uint64_t, bool>, int32_t> transTable, transTablePrev;
};
