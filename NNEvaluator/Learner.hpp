# pragma once

# include <bit>
# include <tuple>
# include <string>

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
			{0x8040201008040201}, // 左上から右下 8
			{0x0102040810204080}, // 右上から左下 8
			{0xf0e0c08000000000}, // 左上周辺 10
			{0x0f07030100000000}, // 右上周辺 10
			{0x00000000080c0e0f}, // 左下周辺 10
			{0x000000000103070f}, // 右下周辺 10
			{0x0010307000000000}, // 左上中心 6
			{0x00080c0e00000000}, // 右上中心 6
			{0x0000000070301000}, // 左下中心 6
			{0x000000000e0c0800}, // 右下中心 6
		};


		NNCpp::Modules::SimpleNet<float, NNCpp::Modules::ReLU<float>> integrate;

		std::vector<ReversiSummary> stateBufferB, stateBufferW;
		NNCpp::Modules::MSELoss<float>loss;
		NNCpp::Optim::Adam<float> optim;

		// No Cache
		float forward(const ReversiSummary& state)
		{
			CMat::Matrix<float>pScores(CMat::MatShape{ 1, static_cast<uint32_t>(patterns.size()) });
			float* targetPtr = pScores.data();
			CMat::Matrix<float> tmp;
			for (auto& pattern : patterns)
			{
				pattern.forward(state, tmp);
				*targetPtr++ = *tmp.data();
			}

			integrate.forward(pScores, tmp);
			return *tmp.data();
		}

		void backward(CMat::Matrix<float>& input)
		{
			CMat::Matrix<float>pin;
			integrate.backward(input, input);
			float* ptr = input.data();
			for (auto& pattern : patterns)
			{
				pin = CMat::Matrix<float>({ 1, 1 }, *ptr++);
				pattern.net.backward(pin, pin);
			}
		}

		char base64EncodeChar(uint64_t x)
		{
			if (x < 26) return static_cast<char>('A' + x);
			if (x < 52) return static_cast<char>('a' + x - 26);
			if (x < 62) return static_cast<char>('0' + x - 52);
			if (x == 62) return '+';
			if (x == 63) return '/';
		}

		uint32_t base64DecodeChar(char x)
		{
			if (x == '+') return 62;
			if (x == '/') return 63;
			if (x < ':') return 52 + x - '0'; // 0-9
			if (x > '`') return 26 + x - 'a'; // a-z
			return x - 'A'; // A-Z
		}

		std::string dumpVector(const float* data, const size_t size)
		{
			std::string result(size * 32 / 6 + 1, '=');
			// 右から左に並べるイメージで
			uint64_t buffer = 0;
			int resIdx = 0, bufferSize = 0;
			auto endPtr = data + size;
			while (bufferSize >= 6 or data != endPtr)
			{
				if (bufferSize < 6)
				{
					uint64_t intValue = static_cast<uint64_t>(*reinterpret_cast<const uint32_t*>(data++));
					buffer |= (intValue << bufferSize);
					bufferSize += 32;
				}
				result[resIdx++] = base64EncodeChar(buffer & 0x3F);
				buffer >>= 6;
				bufferSize -= 6;
			}

			if (bufferSize > 0)
			{
				result[resIdx++] = base64EncodeChar(buffer & 0x3F);
			}

			return result;
		}


	public:
		Learner(): integrate(static_cast<uint32_t>(patterns.size()), 16, 1)
		{
			auto p = parameters();
			optim = NNCpp::Optim::Adam<float>(p, 0.01f);

			preCalsPatterns();
		}

		inline NNCpp::Utils::ParamInfoList<float> parameters()
		{
			NNCpp::Utils::ParamInfoList<float> p;
			for (auto& pattern : patterns)
			{
				auto param = pattern.net.parameters();
				p.insert(p.end(), param.begin(), param.end());
			}
			auto i_param = integrate.parameters();
			p.insert(p.end(), i_param.begin(), i_param.end());
			return p;
		}

		int32_t eval(const Reversi::ReversiEngine& engine)
		{
			CMat::Matrix<float>pScores(CMat::MatShape{ 1, static_cast<uint32_t>(patterns.size()) });
			float* targetPtr = pScores.data();
			CMat::Matrix<float> tmp;
			for (auto& pattern : patterns)
			{
				*targetPtr++ = pattern.eval(engine);
			}

			integrate.forward(pScores, tmp);
			//assert(*tmp.data() == forward(engine.getTupleState()));
			return static_cast<int32_t>(*tmp.data() * 128 - 64);
		}

		void addTarget(const ReversiSummary& state, bool isBlack)
		{
			auto& stateBuffer = isBlack ? stateBufferB : stateBufferW;
			stateBuffer.push_back(state);
		}

		void clearTarget()
		{
			stateBufferB.clear();
			stateBufferW.clear();
		}

		/// @brief 今まで登録した盤面のスコアがscoreであるとして学習を行う
		/// @param score 盤面スコア
		/// @param requireRecalc 前計算をもう一度行うか
		void step(int32_t score, bool isBlack, bool requireRecalc = true)
		{
			auto& stateBuffer = isBlack ? stateBufferB : stateBufferW;
			const auto t = CMat::Matrix<float>{ {1, 1}, score / 128.0f + 0.5f };
			auto cnt = stateBuffer.size();
			for (const ReversiSummary& state : stateBuffer)
			{
				--cnt;

				//for (int32_t i = 0; i < (cnt and stateBuffer.size() > 1 ? 1 : 1000); ++i)
				for (int32_t i = 0; i < 1; ++i)
				{
					auto y = CMat::Matrix<float>{ {1, 1}, forward(state) };
					float l;
					loss.forward(y, t, l);

					if (i == 0) Console << U"y: " << *y.data() << U"\tt: " << *t.data() << U"\tloss:" << l;

					optim.zeroGrad();
					loss.backward(y);
					backward(y);
					optim.step();
					if (l < 0.00001 and cnt == 0 and stateBuffer.size() > 1)
					{
						Console << U"Optimized for {} times"_fmt(i + 1);
						break;
					}
				}
			}
			stateBuffer.clear();
			if (requireRecalc) preCalsPatterns();
			auto d = dump();
			String s;
			for (auto c : d) s << c;
			Console << s;
		}

		void preCalsPatterns()
		{
			// 遅いので実際はテーブルのクリアだけ
			for (auto& pattern : patterns)
			{
				//auto start = std::chrono::high_resolution_clock::now();
				pattern.preCalc();
				//auto end = std::chrono::high_resolution_clock::now();
				//auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
				//int a = 10;
			}
		}

		/// @brief パラメータをbase64形式-likeの文字列にします
		/// @return 変換後の文字列
		std::string dump()
		{
			auto params = parameters();
			std::string result = "";
			for (const auto& param : params)
			{
				result += dumpVector(param.data, param.size);
				result += '|';
			}

			return result;
		}

		void load(std::string str)
		{
			auto params = parameters();
			auto pItr = params.begin();
			auto dPtr = pItr->data;
			//auto dEnd = pItr->data + pItr->size;

			uint64_t buffer = 0;
			int bufferSize = 0, cnt = 0;
			for (char c : str)
			{
				if (c == '|')
				{
					cnt = 0;
					buffer = 0;
					bufferSize = 0;
					++pItr;
					if (pItr == params.end()) break;
					dPtr = pItr->data;
					continue;
				}
				//if (cnt == pItr->size) continue;

				uint64_t value = base64DecodeChar(c);
				buffer |= (value << bufferSize);
				bufferSize += 6;

				while (bufferSize >= 32)
				{
					uint32_t intValue = static_cast<uint32_t>(buffer & 0xFFFFFFFF);
					*dPtr++ = *reinterpret_cast<float*>(&intValue);
					buffer >>= 32;
					bufferSize -= 32;
					cnt++;
				}
			}
		}
	};
}
