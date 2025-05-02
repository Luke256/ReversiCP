#include "AlphaBetaAgent.hpp"

AlphaBetaAgent::AlphaBetaAgent()
{

}

Point AlphaBetaAgent::play(const Reversi::ReversiEngine& engine)
{
	callCnt = 0;
	Reversi::ReversiEngine env = engine;
	if (not env.isBlackTurn()) env.swapBW(); // 黒を扱いたい
	const uint64 legals = env.getLegals();
	const uint64 prevBlacks = env.getBlacks(), prevWhites = env.getWhites();

	uint64 mask = 0x8000000000000000;
	int32 best = -1, score, alpha = -inf, beta = inf;
	for (int32 i : step(64))
	{
		if (not (legals & mask))
		{
			mask >>= 1;
			continue;
		}
		env.place(i % 8, i / 8);
		score = -negaAlpha(env, 6, false, -beta, -alpha);

		if (alpha < score)
		{
			alpha = score;
			best = i;
		}

		env.setState(prevBlacks, prevWhites, true);
		mask >>= 1;
	}
	Console << U"AlphaBeta: " << callCnt << U" calls.";
	return Point{ best % 8, best / 8 };
}

void AlphaBetaAgent::reset_child()
{
}

int32 AlphaBetaAgent::negaAlpha(Reversi::ReversiEngine& engine, int32 depth, bool passed, int32 alpha, int32 beta)
{
	callCnt++;
	if (depth == 0) return eval(engine);
	if (transTable.contains(engine.getTupleState())) return transTable[engine.getTupleState()];

	const uint64 legals = engine.getLegals(), prevBlacks = engine.getBlacks(), prevWhites = engine.getWhites();
	uint64 mask = 0x8000000000000000;
	const bool prevBlackTurn = engine.isBlackTurn();
	int32 maxScore = -inf, g = 0;
	for (int32 i : step(64))
	{
		if (not (legals & mask))
		{
			mask >>= 1;
			continue;
		}
		engine.place(i & 7, i >> 3);
		g = -negaAlpha(engine, depth - 1, false, -beta, -alpha);
		if (g >= beta) return g;
		alpha = Max(alpha, g);
		maxScore = Max(maxScore, g);
		engine.setState(prevBlacks, prevWhites, prevBlackTurn);
		mask >>= 1;
	}

	if (maxScore != -inf) return transTable[engine.getTupleState()] = maxScore; // 操作をした

	if (passed) return transTable[engine.getTupleState()] = eval(engine); // パスの連続

	// 初回のパス
	engine.pass();
	maxScore = -negaAlpha(engine, depth - 1, true, -beta, -alpha);
	engine.pass();
	return transTable[engine.getTupleState()] = maxScore;
}

inline int32 AlphaBetaAgent::eval(const Reversi::ReversiEngine& engine) const
{
	uint64 black = engine.getBlacks(), white = engine.getWhites();
	int32 score = 0;
	score += rowValues[0][(black & 0xFF00000000000000) >> 56];
	score += rowValues[1][(black & 0x00FF000000000000) >> 48];
	score += rowValues[2][(black & 0x0000FF0000000000) >> 40];
	score += rowValues[3][(black & 0x000000FF00000000) >> 32];
	score += rowValues[4][(black & 0x00000000FF000000) >> 24];
	score += rowValues[5][(black & 0x0000000000FF0000) >> 16];
	score += rowValues[6][(black & 0x000000000000FF00) >> 8];
	score += rowValues[7][(black & 0x00000000000000FF)];
	score -= rowValues[0][(white & 0xFF00000000000000) >> 56];
	score -= rowValues[1][(white & 0x00FF000000000000) >> 48];
	score -= rowValues[2][(white & 0x0000FF0000000000) >> 40];
	score -= rowValues[3][(white & 0x000000FF00000000) >> 32];
	score -= rowValues[4][(white & 0x00000000FF000000) >> 24];
	score -= rowValues[5][(white & 0x0000000000FF0000) >> 16];
	score -= rowValues[6][(white & 0x000000000000FF00) >> 8];
	score -= rowValues[7][(white & 0x00000000000000FF)];

	if (not engine.isBlackTurn()) score = -score;
	if (score > 0) // 四捨五入のため
		score += 128;
	else
		score -= 128;
	score /= 256; // 生の評価値は最終石差の256倍なので、256で割る
	if (score > 64) score = 64; // 評価値を[-64, 64] に収める
	else if (score < -64) score = -64;
	return score;
}
