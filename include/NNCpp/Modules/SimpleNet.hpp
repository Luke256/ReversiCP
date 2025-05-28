# pragma once

# include "Dense.hpp"

namespace NNCpp::Modules
{
	template<class _Dty, class HiddenActivation>
	class SimpleNet
	{
	private:
		Dense<_Dty> d1, d2;
		HiddenActivation activator;
	public:
		SimpleNet(uint32_t inputSize, uint32_t hiddenSize, uint32_t outputSize):
			d1(inputSize, hiddenSize), d2(hiddenSize, outputSize),
			activator()
		{}

		void forward(const CMat::Matrix<_Dty>& input, CMat::Matrix<_Dty>& output)
		{
			d1.forward(input, output);
			activator.forward(output, output);
			d2.forward(output, output);
		}

		void backward(const CMat::Matrix<_Dty>& input, CMat::Matrix<_Dty>& output)
		{
			d2.backward(input, output);
			activator.backward(output, output);
			d1.backward(output, output);
		}

		NNCpp::Utils::ParamInfoList<_Dty> parameters() 
		{
			NNCpp::Utils::ParamInfoList<_Dty>res = d1.parameters();
			NNCpp::Utils::ParamInfoList<_Dty>p2 = d2.parameters();
			res.insert(res.end(), p2.begin(), p2.end());
			return res;
		}
	};
}
