# pragma once

namespace NNCpp::Modules
{
	template<class _Dty>
	class ReLU
	{
	private:
		CMat::Matrix<_Dty> mask;
	public:
		ReLU(){}

		void forward(const CMat::Matrix<_Dty>& input, CMat::Matrix<_Dty>& output)
		{
			output = input;
			mask = CMat::Matrix<_Dty>(input.shape);
			for (auto itr = output.begin(), itrMask = mask.begin(); itr != output.end(); itr++, itrMask++)
			{
				if (*itr <= 0) *itr = 0;
				else *itrMask = 1;
			}
		}

		void backward(const CMat::Matrix<_Dty>& input, CMat::Matrix<_Dty>& output)
		{
			output = input * mask;
		}
	};
}
