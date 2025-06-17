# pragma once

# include "Dense.hpp"

namespace NNCpp::Modules
{
	template<class _Dty, class HiddenActivation>
	class SimpleNet
	{
	private:
		Dense<_Dty> d1, d2, d3;
		HiddenActivation a1, a2;
	public:
		SimpleNet(uint32_t inputSize, uint32_t hiddenSize, uint32_t outputSize):
			d1(inputSize, hiddenSize), d2(hiddenSize, hiddenSize), d3(hiddenSize, outputSize),
			a1(), a2()
		{}

		void forward(const CMat::Matrix<_Dty>& input, CMat::Matrix<_Dty>& output)
		{
			d1.forward(input, output);
			a1.forward(output, output);
			d2.forward(output, output);
			a2.forward(output, output);
			d3.forward(output, output);
		}

		void backward(const CMat::Matrix<_Dty>& input, CMat::Matrix<_Dty>& output)
		{
			d3.backward(input, output);
			a2.backward(output, output);
			d2.backward(output, output);
			a1.backward(output, output);
			d1.backward(output, output);
		}

		NNCpp::Utils::ParamInfoList<_Dty> parameters() 
		{
			NNCpp::Utils::ParamInfoList<_Dty>res = d1.parameters();
			NNCpp::Utils::ParamInfoList<_Dty>p2 = d2.parameters();
			NNCpp::Utils::ParamInfoList<_Dty>p3 = d3.parameters();
			res.insert(res.end(), p2.begin(), p2.end());
			res.insert(res.end(), p3.begin(), p3.end());
			return res;
		}
	};
}
