# pragma once

# include "Matrix.hpp"

namespace CMat
{
	template<class _Dty>
	Matrix<_Dty> sumRows(const Matrix<_Dty>& x)
	{
		Matrix<_Dty>res(MatShape{ 1, x.shape.cols });
		auto* ptr = x.data();
		auto* target_ptr;

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
}
