# pragma once

#include <immintrin.h>

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
	CMat<_Dty> matmul(const CMat<_Dty>& a, const CMat<_Dty>& b) {
		if (a.shape.cols != b.shape.rows)
			throw std::invalid_argument("Number of cols for a and number of rows for b doesn't match.");

		const CMat<_Dty> bt = b.transposed();
		CMat<_Dty> c(MatShape{ a.shape.rows, b.shape.cols });

		const _Dty* a_ptr = a.data();
		_Dty* c_ptr = c.data();

		constexpr bool is_float = std::is_same_v<_Dty, float>;
		constexpr bool is_double = std::is_same_v<_Dty, double>;

		const _Dty* bt_ptr;

		for (uint32_t i = 0; i < c.shape.rows; ++i) {
			bt_ptr = bt.data();
			for (uint32_t j = 0; j < c.shape.cols; ++j) {
				_Dty sum = 0;
				uint32_t k = 0;

				if constexpr (is_float) {
					__m256 acc = _mm256_setzero_ps();
					for (; k + 8 <= a.shape.cols; k += 8) {
						__m256 va = _mm256_loadu_ps(a_ptr + k);
						__m256 vb = _mm256_loadu_ps(bt_ptr + k);
						acc = _mm256_fmadd_ps(va, vb, acc);
					}
					alignas(32) float temp[8];
					_mm256_store_ps(temp, acc);
					for (int l = 0; l < 8; ++l) sum += temp[l];
				}
				else if constexpr (is_double) {
					__m256d acc = _mm256_setzero_pd();
					for (; k + 4 <= a.shape.cols; k += 4) {
						__m256d va = _mm256_loadu_pd(a_ptr + k);
						__m256d vb = _mm256_loadu_pd(bt_ptr + k);
						acc = _mm256_fmadd_pd(va, vb, acc);
					}
					alignas(32) double temp[4];
					_mm256_store_pd(temp, acc);
					for (int l = 0; l < 4; ++l) sum += temp[l];
				}

				for (; k < a.shape.cols; ++k) {
					sum += a_ptr[k] * bt_ptr[k];
				}

				*c_ptr++ = sum;
				bt_ptr += a.shape.cols;
			}
			a_ptr += a.shape.cols;
		}
		return c;
	}
}
