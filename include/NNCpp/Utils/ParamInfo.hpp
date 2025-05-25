# pragma once

# include <vector>

namespace NNCpp::Utils
{

	template<class _Dty>
	struct ParamInfo
	{
		_Dty* data;
		uint32_t size;
	};

	template<class _Dty>
	using ParamInfoList = std::vector<ParamInfo<_Dty>>;
}
