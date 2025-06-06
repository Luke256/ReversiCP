# include "LearnerAgent.hpp"
# include "../NNEvaluator/Learner.hpp"

NNEvaluator::Learner LearnerAgent::learner;

LearnerAgent::LearnerAgent()
{

}

LearnerAgent::Pos LearnerAgent::play(const Reversi::ReversiEngine& engine)
{
	callCnt = 0;
	Reversi::ReversiEngine env = engine, bestEnv;
	isBlack = env.isBlackTurn();

	env.swapBW(true); // 相手の盤面を自分のものとしてみる
	learner.addTarget(env.getTupleState(), not isBlack); // 行動前の盤面を保存
	if (isBlack)
	{
		env.swapBW(true); // 黒を扱いたい。自分が元々黒なら反転の必要はないので戻す。
	}
	const uint64_t prevBlacks = env.getBlacks(), prevWhites = env.getWhites();
	const int32_t SEARCH_DEPTH = 3;

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
				bestEnv = env;
				alpha = score;
				best = idx;
			}

			env.setState(prevBlacks, prevWhites, true);
		}
		transTable.swap(transTablePrev);
		transTable.clear();
	}
	learner.addTarget(bestEnv.getTupleState(), isBlack); // 行動後の盤面を保存
	if (env.isBlackTurn()) return { best & 7, best >> 3 };
	else return { 7 - (best & 7), best >> 3 };
}

void LearnerAgent::reset_child()
{
}

void LearnerAgent::reviewGame(const Reversi::ReversiEngine& engine)
{
	Reversi::ReversiEngine env = engine;
	assert(env.isBlackTurn() == isBlack);
	if (not isBlack) env.swapBW(); // 黒を扱いたい
	learner.step(env.getNBlacks() - env.getNWhites(), isBlack, false); // 自盤面の学習
	learner.step(env.getNWhites() - env.getNBlacks(), not isBlack); // 相手盤面の学習
}

int32_t LearnerAgent::negaAlpha(Reversi::ReversiEngine& engine, int32_t depth, bool passed, int32_t alpha, int32_t beta)
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

inline int32_t LearnerAgent::eval(const Reversi::ReversiEngine& engine)
{
	return learner.eval(engine);
}
