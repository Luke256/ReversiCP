# pragma once

# include "Matrix.hpp"

namespace CMat
{
	template<class _Dty>
	_Dty sum(const Matrix<_Dty>& x)
	{
		_Dty res = 0;
		const _Dty* ptr = x.data();

		uint32_t i, j;

		for (i = 0; i < x.shape.rows; ++i)
		{
			for (j = 0; j < x.shape.cols; ++j)
			{
				res += *ptr++;
			}
		}
		return res;
	}

	template<class _Dty>
	Matrix<_Dty> sumRows(const Matrix<_Dty>& x)
	{
		Matrix<_Dty>res(MatShape{ 1, x.shape.cols });
		const _Dty* ptr = x.data();
		_Dty* target_ptr;

		uint32_t i, j;

		for (i = 0; i < x.shape.rows; ++i)
		{
			target_ptr = res.data();
			for (j = 0; j < x.shape.cols; ++j)
			{
				*target_ptr++ += *ptr++;
			}
		}
		return res;
	}

	template<class _Dty>
	inline Matrix<_Dty> pow(const Matrix<_Dty>& a, const _Dty exponent)
	{
		auto res = a;
		_Dty* target = res.data();
		const _Dty* ptr = a.data();
		const _Dty* end = ptr + a.size();
		while (ptr != end)
		{
			*target++ = std::pow(*ptr++, exponent);
		}
		return res;
	}
}
