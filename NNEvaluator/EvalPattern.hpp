# pragma once

# include <bit>
# include <vector>
# include <tuple>
# include <cmath>

# include "../include/NNCpp/NNCpp.hpp"
# include "../include/CMat/CMat.hpp"
# include "../ReversiEngine.hpp"
# include "../Utils.hpp"

namespace NNEvaluator
{
	struct TupleHash {
		std::size_t operator()(const std::tuple<uint64_t, uint64_t>& t) const noexcept {
			uint64_t a, b;
			std::tie(a, b) = t;

			// 高速で分布の良いハッシュ関数（SplitMix64ベース）
			auto hash64 = [](uint64_t x) {
				x += 0x9e3779b97f4a7c15;
				x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9;
				x = (x ^ (x >> 27)) * 0x94d049bb133111eb;
				return x ^ (x >> 31);
				};

			uint64_t h1 = hash64(a);
			uint64_t h2 = hash64(b);

			// ハッシュ値を混合する（rotate xor加算など）
			uint64_t combined = h1;
			combined ^= (h2 << 1) | (h2 >> 63);

			return static_cast<std::size_t>(combined);
		}
	};

	template<typename T>
	inline T fast_pow_int(T base, unsigned int exponent) {
		T result = 1;
		while (exponent > 0) {
			if (exponent & 1) {  // 奇数の場合
				result *= base;
			}
			base *= base;
			exponent >>= 1;  // 2で割る
		}
		return result;
	}

	struct EvalPattern
	{
		uint64_t mask, maskSize;
		NNCpp::Modules::SimpleNet<float, NNCpp::Modules::ReLU<float>>net;
		std::unordered_map<std::tuple<uint64_t, uint64_t>, float, NNEvaluator::TupleHash> evals;

		EvalPattern(uint64_t mask_) : mask(mask_), maskSize(std::popcount(mask_)), net(std::popcount(mask_), 16, 1)
		{}

		void eval(const Reversi::ReversiEngine& engine, float *res)
		{
			for (int i = 0; i < 1; i++, mask = BitBordUtils::rotateClockwise90(mask))
			{
				auto tpl = maskState2Tuple(engine.getTupleState());
				if (not evals.contains(tpl))
				{
					auto mat = tuple2Matrix(tpl);
					net.forward(mat, mat);
					auto v = *mat.data();
					evals[tpl] = v;
				}
				res[i] = evals[tpl];
			}
		}

		void preCalc()
		{
			evals.clear();
			return;
		}

		void forward(const std::tuple<uint64_t, uint64_t, bool>& state, CMat::Matrix<float>& output)
		{
			net.forward(maskState2Matrix(state), output);
		}

		CMat::Matrix<float> maskState2Matrix(const std::tuple<uint64_t, uint64_t, bool>& state)
		{
			CMat::Matrix<float>res{ CMat::MatShape{4, static_cast<uint32_t>(std::popcount(mask))} };
			float* ptr = res.data();
			for (int i = 0; i < 4; i++, mask = BitBordUtils::rotateClockwise90(mask))
			{
				uint64_t pointer = 0x8000000000000000;
				for (; pointer; pointer >>= 1)
				{
					if ((mask & pointer) == 0) continue;
					if (std::get<0>(state) & pointer) *ptr = 1.0f;
					else if (std::get<1>(state) & pointer) *ptr = -1.0f;
					ptr++;
				}
			}
			return res;
		}

		std::tuple<uint64_t, uint64_t> maskState2Tuple(const std::tuple<uint64_t, uint64_t, bool>& state) const
		{
			std::tuple<uint64_t, uint64_t> res;
			uint64_t ptr = (1ull << (maskSize - 1));
			uint64_t pointer = 0x8000000000000000;
			for (; pointer; pointer >>= 1)
			{
				if ((mask & pointer) == 0) continue;
				if (std::get<0>(state) & pointer) std::get<0>(res) |= ptr;
				else if (std::get<1>(state) & pointer) std::get<1>(res) |= ptr;
				ptr >>= 1;
			}
			return res;
		}

		CMat::Matrix<float> tuple2Matrix(const std::tuple<uint64_t, uint64_t>& state)
		{
			CMat::Matrix<float>res{ CMat::MatShape{1, static_cast<uint32_t>(maskSize)} };
			float* ptr = res.data();
			uint64_t pointer = (1ull << (maskSize - 1));
			for (; pointer; pointer >>= 1, ptr++)
			{
				if (std::get<0>(state) & pointer) *ptr = 1.0f;
				else if (std::get<1>(state) & pointer) *ptr = -1.0f;
			}
			return res;
		}
	};
}
