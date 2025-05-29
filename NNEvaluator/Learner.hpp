# pragma once

# include <bit>
# include <tuple>

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
			{0xff00000000000000},
			{0x00ff000000000000},
			{0x0000ff0000000000},
			{0x000000ff00000000},
			{0x00000000ff000000},
			{0x0000000000ff0000},
			{0x000000000000ff00},
			{0x00000000000000ff},
		};

		NNCpp::Modules::SimpleNet<float, NNCpp::Modules::ReLU<float>> integrate;
		NNCpp::Modules::Sigmoid<float> sigmoid;

		std::vector<ReversiSummary> stateBuffer;
		NNCpp::Modules::MSELoss<float>loss;
		NNCpp::Optim::SGD<float> optim;

		float forward(const ReversiSummary& state)
		{
			CMat::Matrix<float>pScores(CMat::MatShape{ 1, static_cast<uint32_t>(patterns.size()) });
			float* targetPtr = pScores.data();
			CMat::Matrix<float> tmp;
			for (auto& pattern : patterns)
			{
				pattern.net.forward(maskState2Matrix(state, pattern.mask), tmp);
				*targetPtr++ = *tmp.data();
			}

			integrate.forward(pScores, tmp);
			sigmoid.forward(tmp, tmp);
			return *tmp.data();
		}

		void backward(CMat::Matrix<float>& input)
		{
			CMat::Matrix<float>tmp, pin(CMat::MatShape{1, 1});
			sigmoid.backward(input, tmp);
			integrate.backward(tmp, tmp);
			float* ptr = tmp.data();
			for (auto& pattern : patterns)
			{
				*pin.data() = *ptr;
				pattern.net.backward(pin, pin);
			}
		}

		CMat::Matrix<float> maskState2Matrix(const ReversiSummary& state, const uint64_t mask)
		{
			CMat::Matrix<float>res{ CMat::MatShape{1, static_cast<uint32_t>(std::popcount(mask))}};
			float* ptr = res.data();
			uint64_t pointer = 0x8000000000000000;
			for (; pointer; pointer >>= 1)
			{
				if ((mask & pointer) == 0) continue;
				if (std::get<0>(state) & pointer) *ptr = 1.0f;
				else if (std::get<1>(state) & pointer) *ptr = -1.0f;
				ptr++;
			}
			return res;
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
