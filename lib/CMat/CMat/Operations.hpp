# pragma once

# include "Matrix.hpp"

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
	CMat<_Dty>& operator+(CMat<_Dty> a, const CMat<_Dty>& b) { return a += b; }
	template<class _Dty>
	CMat<_Dty>& operator-(CMat<_Dty> a, const CMat<_Dty>& b) { return a -= b; }
	template<class _Dty>
	CMat<_Dty>& operator*(CMat<_Dty> a, const CMat<_Dty>& b) { return a *= b; }
	template<class _Dty>
	CMat<_Dty>& operator/(CMat<_Dty> a, const CMat<_Dty>& b) { return a /= b; }
}
