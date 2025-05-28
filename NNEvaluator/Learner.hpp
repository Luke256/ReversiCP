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
		using ReversiSummary = std::tuple<uint64_t, uint64_t, bool>;
		std::vector<EvalPattern> patterns = {
		};

		NNCpp::Modules::SimpleNet<float, NNCpp::Modules::ReLU<float>> integrate;

		std::vector<ReversiSummary> stateBuffer;
		NNCpp::Modules::MSELoss<float>loss;
		NNCpp::Optim::SGD<float> optim;

		float forward(const ReversiSummary& engine)
		{
			std::vector<float>pScores(patterns.size());
			for (const auto& pattern : patterns)
			{

			}
		}

		float backward(CMat::Matrix<float>& input)
		{

		}

	public:
		Learner(): integrate(static_cast<uint32_t>(patterns.size()), 32, 1)
		{
			NNCpp::Utils::ParamInfoList<float> p;
			for (auto& pattern : patterns)
			{
				auto param = pattern.net.parameters();
				p.insert(p.end(), param.begin(), param.end());
			}
			optim = NNCpp::Optim::SGD<float>(p, 0.0001f);
		}

		int32_t eval(const Reversi::ReversiEngine& engine)
		{
			stateBuffer.push_back(engine.getTupleState());
			float scoref = forward(stateBuffer.back());
			return static_cast<int32_t>(scoref * 64);
		}

		/// @brief 今まで評価した盤面のスコアがscoreであるとして学習を行う
		/// @param score 盤面スコア
		void step(uint32_t score)
		{
			const auto t = CMat::Matrix<float>{ {1, 1}, score / 64.0f };
			for (const ReversiSummary& state : stateBuffer)
			{
				auto y = CMat::Matrix<float>{ {1, 1}, forward(state) };
				float l;
				loss.forward(y, t, l);

				optim.zeroGrad();
				loss.backward(y);
				backward(y);
				optim.step();
			}
			stateBuffer.clear();
		}
	};
}
