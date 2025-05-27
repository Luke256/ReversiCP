# pragma once

# include <cmath>

# include "../Utils/ParamInfo.hpp"

namespace NNCpp::Modules
{
	template<class _Dty>
	class Dense
	{
	private:
		CMat::Matrix<_Dty> w, b, last_input, gw, gb;
	public:
		Dense(uint32_t input_size, uint32_t output_size) :
			w(CMat::Random::norm<_Dty>(CMat::MatShape{ input_size, output_size }, 0.0, 0.01)),
			b(CMat::zeros<_Dty>(CMat::MatShape{ 1, output_size })),
			gw(CMat::zeros<_Dty>(CMat::MatShape{ input_size, output_size })),
			gb(CMat::zeros<_Dty>(CMat::MatShape{ 1, output_size }))
		{
		}


		void forward(const CMat::Matrix<_Dty>& input, CMat::Matrix<_Dty>& output)
		{
			last_input = input;
			output = CMat::matmul(input, w);
			output += b;
		}

		void backward(const CMat::Matrix<_Dty>& input, CMat::Matrix<_Dty>& output)
		{
			gb += CMat::sumRows(input);
			gw += CMat::matmul(last_input.transposed(), input);
			output = CMat::matmul(output, w.transposed());
		}

		NNCpp::Utils::ParamInfoList<_Dty> parameters()
		{
			return {
				{w.data(), gw.data(), w.size()},
				{b.data(), gb.data(), b.size()}
			};
		}

		void print()
		{
			Console << w;
		}
	};
}
