# pragma once

# include "../CMat/CMat.hpp"

namespace NNCpp::Modules
{
	template<class _Dty>
	class MSELoss
	{
	private:
		CMat::Matrix<_Dty> lastGrad;

	public:
		void forward(const CMat::Matrix<_Dty>& input, const CMat::Matrix<_Dty>& target, _Dty& output)
		{
			lastGrad = input - target;
			const size_t n = input.shape.rows;
			output = CMat::sum(CMat::pow<_Dty>(lastGrad, 2)) / n;
		}

		void backward(CMat::Matrix<_Dty>& output)
		{
			output = lastGrad;
		}
	};
}
