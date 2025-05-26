# pragma once

# include <vector>

# include "../CMat/CMat.hpp"

namespace NNCpp::Utils
{
	template<class _Dty>
	struct ParamInfo
	{
		_Dty *data, *grad;
		size_t size;

		ParamInfo(_Dty* data_, _Dty* grad_, size_t size_): data(data_), grad(grad_), size(size_)
		{}
	};

	template<class _Dty>
	using ParamInfoList = std::vector<ParamInfo<_Dty>>;
}
