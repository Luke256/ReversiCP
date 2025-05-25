# pragma once

# include <vector>

# include "../CMat/CMat.hpp"
# include "../Utils/ParamInfo.hpp"

namespace NNCpp::Modules
{
	template<class _Dty>
	class Module
	{
	public:
		virtual void forward(const CMat::Matrix<_Dty>& input, CMat::Matrix<_Dty>& output) = 0;
		virtual void backward(const CMat::Matrix<_Dty>& input, CMat::Matrix<_Dty>& output) = 0;
		NNCpp::Utils::ParamInfoList<_Dty> parameters() {}

		void forward(CMat::Matrix<_Dty>& input)
		{
			forward(input, input);
		}

		void backward(CMat::Matrix<_Dty>& input)
		{
			backward(input, input);
		}
	};
};
