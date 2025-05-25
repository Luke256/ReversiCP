# pragma once

# include "Core.hpp"

namespace NNCpp
{

	template<class _Dty>
	class Dense : public Module
	{
	private:
		CMat::Matrix<_Dty> w, b, gw, gb, last_input;
	public:
		Dense(uint32_t input_size, uint32_t output_size):
			w(CMat::Random::norm(CMat::MatShape{ input_size, output_size })),
			b(CMat::Random::norm(CMat::MatShape{1, output_size}))
		{}


		void forward(const CMat::Matrix& input, CMat::Matrix& output) override
		{
			last_input = input;
			output = CMat::matmul(input, w);
			output += b;
		}

		void backward(const CMat::Matrix& input, CMat::Matrix& output) override
		{
			gb = CMat::sumRows(input);
			gw = CMat::matmul(last_input.transpose(), input);
			output = CMat::matmul(output, w.transposed());
		}
	};
}
