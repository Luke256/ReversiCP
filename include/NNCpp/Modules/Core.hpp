# pragma once

# include "../CMat/CMat.hpp"

namespace NNCpp
{
	template<class _Dty>
	class Module
	{
	public:
		virtual void forward(const CMat::Matrix& input, CMat::Matrix& output) = 0;
		virtual void backward(const CMat::Matrix& input, CMat::Matrix& output) = 0;

		void forward(CMat::Matrix& input)
		{
			forward(input, input);
		}

		void backward(CMat::Matrix& input)
		{
			backward(input, input);
		}
	};
};
