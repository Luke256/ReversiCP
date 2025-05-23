# pragma once

# include "Matrix.hpp"
# include "Shape.hpp"

namespace CMat
{
	template<class _Dty>
	CMat<_Dty>& operator+=(CMat<_Dty>& a, const CMat<_Dty>& b)
	{
		if (a.shape != b.shape) throw std::invalid_argument("Two shape of operand for operator + doesn't match.");

		_Dty* p = a.data();
		const _Dty* q = b.data();
		const _Dty* end = p + a.size();

		while (p != end)
		{
			*p++ += *q++;
		}
		return a;
	}

	template<class _Dty>
	CMat<_Dty>& operator-=(CMat<_Dty>& a, const CMat<_Dty>& b)
	{
		if (a.shape != b.shape) throw std::invalid_argument("Two shape of operand for operator - doesn't match.");

		_Dty* p = a.data();
		const _Dty* q = b.data();
		const _Dty* end = p + a.size();

		while (p != end)
		{
			*p++ -= *q++;
		}
		return a;
	}

	template<class _Dty>
	CMat<_Dty>& operator*=(CMat<_Dty>& a, const CMat<_Dty>& b)
	{
		if (a.shape != b.shape) throw std::invalid_argument("Two shape of operand for operator * doesn't match.");

		_Dty* p = a.data();
		const _Dty* q = b.data();
		const _Dty* end = p + a.size();

		while (p != end)
		{
			*p++ *= *q++;
		}
		return a;
	}

	template<class _Dty>
	CMat<_Dty>& operator/=(CMat<_Dty>& a, const CMat<_Dty>& b)
	{
		if (a.shape != b.shape) throw std::invalid_argument("Two shape of operand for operator / doesn't match.");

		_Dty* p = a.data();
		const _Dty* q = b.data();
		const _Dty* end = p + a.size();

		while (p != end)
		{
			*p++ /= *q++;
		}
		return a;
	}

	template<class _Dty>
	CMat<_Dty> operator+(CMat<_Dty> a, const CMat<_Dty>& b) { return a += b; }
	template<class _Dty>
	CMat<_Dty> operator-(CMat<_Dty> a, const CMat<_Dty>& b) { return a -= b; }
	template<class _Dty>
	CMat<_Dty> operator*(CMat<_Dty> a, const CMat<_Dty>& b) { return a *= b; }
	template<class _Dty>
	CMat<_Dty> operator/(CMat<_Dty> a, const CMat<_Dty>& b) { return a /= b; }

	template<class _Dty>
	CMat<_Dty> matmul(const CMat<_Dty>& a, const CMat<_Dty>& b)
	{
		if (a.shape.cols != b.shape.rows) throw std::invalid_argument("Number of cols for a and number of rows for b doesn't match.");

		const CMat<_Dty>bt = b.transposed();
		CMat<_Dty> c(MatShape{ a.shape.rows, b.shape.cols });
		_Dty* c_ptr = c.data();
		
		const _Dty* a_ptr = a.data();
		const _Dty* bt_ptr;
		for (uint32_t i = 0; i < c.shape.rows; ++i)
		{
			bt_ptr = bt.data();
			for (uint32_t j = 0; j < c.shape.cols; ++j)
			{
				for (uint32_t k = 0; k < a.shape.cols; ++k)
				{
					*c_ptr += a_ptr[k] * bt_ptr[k];
				}
				c_ptr++;
				bt_ptr = bt_ptr + a.shape.cols;
			}
			a_ptr = a_ptr + a.shape.cols;
		}

		return c;
	}
}
