#line 1 "codingame.cpp"
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <assert.h>
#include <bit>
#line 1 "ReversiAgents/AlphaBetaAgent.hpp"
# pragma once

#line 1 "ReversiAgents/Agent.hpp"
# pragma once
#line 1 "ReversiEngine.hpp"
#pragma once

namespace Reversi
{
	struct TupleHash {
		std::size_t operator()(const std::tuple<uint64_t, uint64_t, bool>& t) const noexcept {
			uint64_t a, b;
			bool c;
			std::tie(a, b, c) = t;

			// 高速で分布の良いハッシュ関数（SplitMix64ベース）
			auto hash64 = [](uint64_t x) {
				x += 0x9e3779b97f4a7c15;
				x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9;
				x = (x ^ (x >> 27)) * 0x94d049bb133111eb;
				return x ^ (x >> 31);
				};

			uint64_t h1 = hash64(a);
			uint64_t h2 = hash64(b);
			uint64_t h3 = hash64(static_cast<uint64_t>(c));

			// ハッシュ値を混合する（rotate xor加算など）
			uint64_t combined = h1;
			combined ^= (h2 << 1) | (h2 >> 63);
			combined ^= (h3 << 2) | (h3 >> 62);

			return static_cast<std::size_t>(combined);
		}
	};

	class ReversiEngine
	{
	private:
		uint64_t m_blacks, m_whites;
		bool m_blackTurn;

		/// @brief 二次元座標をビットに変換します
		/// @param x 左から何番目か
		/// @param y 上から何番目か
		/// @return ビッドボードでのマスク
		uint64_t pos2bit(uint32_t x, uint32_t y) const;

		uint64_t transfer(uint64_t put, uint32_t dir) const;

	public:
		ReversiEngine();

		void reset();

		uint64_t getLegals(bool inverseTurn = false) const;

		bool place(uint32_t x, uint32_t y);

		void getBoard(std::vector<int32_t>& board) const;

		void pass();

		void setState(uint64_t blacks, uint64_t whites, bool blackTurn);

		void swapBW();

		bool isBlackTurn() const;

		int32_t getNBlacks() const;

		int32_t getNWhites() const;

		uint64_t getBlacks() const;

		uint64_t getWhites() const;

		bool isFinished();

		inline std::tuple<uint64_t, uint64_t, bool> getTupleState() const
		{
			return { m_blacks, m_whites, m_blackTurn };
		}
	};

	void bit2boad(const uint64_t& bit, std::vector<int32_t>& board);

};
#line 3 "ReversiAgents/Agent.hpp"

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
#line 4 "ReversiAgents/AlphaBetaAgent.hpp"
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

	std::vector<int32_t> _initRowValues()
	{
		//Grid<int32_t>res{ 8, (1 << 8) };
		std::vector<int32_t> res(1 << 11);
		int32_t i, bit, j;
		for (i = 0; i < 8; i++)
		{
			for (bit = 0; bit < (1 << 8); bit++)
			{
				for (j = 0; i < 8; j++)
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
#line 8 "codingame.cpp"

using namespace std;

int main()
{
	int id; // id of your player.
	cin >> id; cin.ignore();
	int board_size;
	cin >> board_size; cin.ignore();

	assert(board_size == 8);

	AlphaBetaAgent agent;
	Reversi::ReversiEngine engine;

	// game loop
	while (1) {
		uint64_t blacks = 0, whites = 0, mask = 0x8000000000000000;
		for (int i = 0; i < board_size; i++) {
			string line; // rows from top to bottom (viewer perspective).
			cin >> line; cin.ignore();
			for (char c : line)
			{
				if (c == '0') blacks |= mask;
				if (c == '1') whites |= mask;
				mask >>= 1;
			}
		}
		int action_count; // number of legal actions for this turn.
		cin >> action_count; cin.ignore();
		for (int i = 0; i < action_count; i++) {
			string action; // the action
			cin >> action; cin.ignore();
		}

		engine.setState(blacks, whites, id == 0);
		auto action = agent.play(engine);

		cout << char('a' + action.first) << char('1' + action.second) << endl;
	}
}
