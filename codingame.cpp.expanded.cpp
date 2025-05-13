# include <iostream>
# include <string>
# include <vector>
# include <algorithm>
# include <assert.h>
# include <bit>
# include <bitset>
# include <unordered_map>

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

namespace Reversi
{

	uint64_t ReversiEngine::pos2bit(uint32_t x, uint32_t y) const
	{
		uint64_t mask = 0x8000000000000000;
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

	uint64_t ReversiEngine::getLegals(bool inverseTurn) const
	{
		const uint64_t& playerBoard = (m_blackTurn ^ inverseTurn) ? m_blacks : m_whites;
		const uint64_t& oppBoard = (m_blackTurn ^ inverseTurn) ? m_whites : m_blacks;
		const uint64_t hMask = 0x7e7e7e7e7e7e7e7e & oppBoard;
		const uint64_t vMask = 0x00FFFFFFFFFFFF00 & oppBoard;
		const uint64_t edgeMask = 0x007e7e7e7e7e7e00 & oppBoard;
		const uint64_t blank = ~(m_blacks | m_whites);

		uint64_t tmp = 0, legals = 0;

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

	bool ReversiEngine::place(uint32_t x, uint32_t y)
	{
		uint64_t& playerBoard = m_blackTurn ? m_blacks : m_whites;
		uint64_t& oppBoard = m_blackTurn ? m_whites : m_blacks;
		uint64_t b = pos2bit(x, y);
		if ((getLegals() & b) == 0) return false;
		uint64_t rev = 0;
		for (int dir = 0; dir < 8; ++dir)
		{
			uint64_t rev_ = 0;
			uint64_t mask = transfer(b, dir);
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

	uint64_t ReversiEngine::transfer(uint64_t put, uint32_t dir) const
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

	void ReversiEngine::getBoard(std::vector<int32_t>& board) const
	{
		uint64_t mask = 0x8000000000000000;
		uint32_t cnt = 0;
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

	void ReversiEngine::setState(uint64_t blacks, uint64_t whites, bool blackTurn)
	{
		m_blacks = blacks;
		m_whites = whites;
		m_blackTurn = blackTurn;
	}

	void ReversiEngine::swapBW()
	{
		std::swap(m_blacks, m_whites);
		m_blackTurn = !m_blackTurn;
	}

	bool ReversiEngine::isBlackTurn() const
	{
		return m_blackTurn;
	}

	int32_t ReversiEngine::getNBlacks() const
	{
		return std::popcount(m_blacks);
	}

	int32_t ReversiEngine::getNWhites() const
	{
		return std::popcount(m_whites);
	}

	uint64_t ReversiEngine::getBlacks() const
	{
		return m_blacks;
	}

	uint64_t ReversiEngine::getWhites() const
	{
		return m_whites;
	}

	bool ReversiEngine::isFinished()
	{
		if (getLegals() != 0) return false;
		if (getLegals(true) != 0) return false;
		return true;
	}

	void bit2boad(const uint64_t& bit, std::vector<int32_t>& board)
	{
		uint64_t mask = 0x8000000000000000;
		uint32_t cnt = 0;
		while (cnt < 64)
		{
			if (mask & bit) board[cnt++] = 1;
			else board[cnt++] = 0;
			mask >>= 1;
		}
	}
}

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

AlphaBetaAgent::AlphaBetaAgent()
{

}

AlphaBetaAgent::Pos AlphaBetaAgent::play(const Reversi::ReversiEngine& engine)
{
	callCnt = 0;
	Reversi::ReversiEngine env = engine;
	if (not env.isBlackTurn()) env.swapBW(); // 黒を扱いたい
	const uint64_t prevBlacks = env.getBlacks(), prevWhites = env.getWhites();
	const int32_t SEARCH_DEPTH = 6;

	int32_t best = -1, score, alpha = -inf, beta = inf, depth;
	std::vector<LegalState> legals;

	for (depth = 0; depth < SEARCH_DEPTH; depth++)
	{
		if (isAborted()) break;
		alpha = -inf, beta = inf;

		getSortedLegals(env, legals);

		for (auto [v, idx] : legals)
		{
			env.place(idx & 7, idx >> 3);
			score = -negaAlpha(env, depth + 1, false, -beta, -alpha);

			if (alpha < score)
			{
				alpha = score;
				best = idx;
			}

			env.setState(prevBlacks, prevWhites, true);
		}
		transTable.swap(transTablePrev);
		transTable.clear();
	}
	return { best & 7, best >> 3 };
}

void AlphaBetaAgent::reset_child()
{
}

int32_t AlphaBetaAgent::negaAlpha(Reversi::ReversiEngine& engine, int32_t depth, bool passed, int32_t alpha, int32_t beta)
{
	callCnt++;
	if (depth == 0) return eval(engine);
	if (transTable.contains(engine.getTupleState())) return transTable[engine.getTupleState()];

	const uint64_t prevBlacks = engine.getBlacks(), prevWhites = engine.getWhites();
	const bool prevBlackTurn = engine.isBlackTurn();
	int32_t maxScore = -inf, g = 0;

	std::vector<LegalState> legals;
	getSortedLegals(engine, legals);

	for (auto [v, idx] : legals)
	{
		engine.place(idx & 7, idx >> 3);
		g = -negaAlpha(engine, depth - 1, false, -beta, -alpha);
		if (g >= beta) return g;
		alpha = std::max(alpha, g);
		maxScore = std::max(maxScore, g);
		engine.setState(prevBlacks, prevWhites, prevBlackTurn);
	}

	if (maxScore != -inf) return transTable[engine.getTupleState()] = maxScore; // 操作をした

	if (passed) return transTable[engine.getTupleState()] = eval(engine); // パスの連続

	// 初回のパス
	engine.pass();
	maxScore = -negaAlpha(engine, depth - 1, true, -beta, -alpha);
	engine.pass();
	return transTable[engine.getTupleState()] = maxScore;
}

inline int32_t AlphaBetaAgent::eval(const Reversi::ReversiEngine& engine) const
{
	uint64_t black = engine.getBlacks(), white = engine.getWhites();
	int32_t score = 0;
	score += rowValues[(0 << 8) + ((black & 0xFF00000000000000) >> 56)];
	score += rowValues[(1 << 8) + ((black & 0x00FF000000000000) >> 48)];
	score += rowValues[(2 << 8) + ((black & 0x0000FF0000000000) >> 40)];
	score += rowValues[(3 << 8) + ((black & 0x000000FF00000000) >> 32)];
	score += rowValues[(4 << 8) + ((black & 0x00000000FF000000) >> 24)];
	score += rowValues[(5 << 8) + ((black & 0x0000000000FF0000) >> 16)];
	score += rowValues[(6 << 8) + ((black & 0x000000000000FF00) >> 8)];
	score += rowValues[(7 << 8) + ((black & 0x00000000000000FF))];
	score -= rowValues[(0 << 8) + ((white & 0xFF00000000000000) >> 56)];
	score -= rowValues[(1 << 8) + ((white & 0x00FF000000000000) >> 48)];
	score -= rowValues[(2 << 8) + ((white & 0x0000FF0000000000) >> 40)];
	score -= rowValues[(3 << 8) + ((white & 0x000000FF00000000) >> 32)];
	score -= rowValues[(4 << 8) + ((white & 0x00000000FF000000) >> 24)];
	score -= rowValues[(5 << 8) + ((white & 0x0000000000FF0000) >> 16)];
	score -= rowValues[(6 << 8) + ((white & 0x000000000000FF00) >> 8)];
	score -= rowValues[(7 << 8) + ((white & 0x00000000000000FF))];

	if (not engine.isBlackTurn()) score = -score;
	if (score > 0) // 四捨五入のため
		score += 128;
	else
		score -= 128;
	score /= 256; // 生の評価値は最終石差の256倍なので、256で割る
	if (score > 64) score = 64; // 評価値を[-64, 64] に収める
	else if (score < -64) score = -64;

	score += std::popcount(engine.getLegals());

	return score;
}
