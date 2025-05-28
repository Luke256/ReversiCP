# pragma once

# include "../ReversiEngine.hpp"
# include "../include/CMat/CMat.hpp"
# include "../include/NNCpp/NNCpp.hpp"

# include "EvalPattern.hpp"

namespace NNEvaluator
{
	// 学習・出力機能を持った盤面評価器
	class Learner
	{
	private:
		std::vector<EvalPattern> patterns = {
		};
	public:
	};
}
