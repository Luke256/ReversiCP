# pragma once

namespace NNCpp::Modules
{
	template<class _Dty>
	class Sigmoid
	{
	private:
		CMat::Matrix<_Dty> lastInput, lastOutput;
	public:
		Sigmoid(){}

		void forward(const CMat::Matrix<_Dty>& input, CMat::Matrix<_Dty>& output)
		{
			lastInput = input;

			output = lastOutput = CMat::pow(CMat::exp(input * -1.0f) + 1.0f, -1.0f);
		}

		void backward(const CMat::Matrix<_Dty>& input, CMat::Matrix<_Dty>& output)
		{
			output = input * lastOutput * lastOutput * CMat::exp(lastInput * -1.0f);
		}
	};
}
