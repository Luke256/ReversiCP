# pragma once

# include <algorithm>

# include "Matrix.hpp"
# include "Shape.hpp"

namespace CMat
{
	template<class _Dty, class Function, class... AdditionalOptions>
	void __applyOperation(Matrix<_Dty>& a, const Matrix<_Dty>& b, const Function& f, AdditionalOptions ...options)
	{
		if (a.shape == b.shape)
		{
			std::transform(a.cbegin(), a.cend(),
				b.cbegin(),
				a.begin(),
				[&f, &options...](const auto& inValue1, const auto& inValue2) -> _Dty {
					return f(inValue1, inValue2, std::forward<AdditionalOptions>(options)...);
				});
		}
		else if (a.shape.cols == b.shape.cols and b.shape.rows == 1)
		{
			for (uint32 row = 0; row < a.shape.rows; ++row)
			{
				std::transform(
					a.cbegin(row),
					a.cend(row),
					b.cbegin(),
					a.begin(row),
					[&f, &options...](const auto& inValue1, const auto& inValue2) -> _Dty {
						return f(inValue1, inValue2, std::forward<AdditionalOptions>(options)...);
					});
			}
		}
		else if (a.shape.rows == b.shape.rows and b.shape.cols == 1)
		{
			a.transpose();
			__applyOperation(a, b.transposed(), f, std::forward<AdditionalOptions>(options)...);
			a.transpose();
		}
		else
		{
			throw std::invalid_argument("operands could not be operated together");
		}
	}

	template<class _Dty>
	Matrix<_Dty>& operator+=(Matrix<_Dty>& a, const Matrix<_Dty>& b)
	{
		__applyOperation(a, b, std::plus<_Dty>());
		return a;
	}

	template<class _Dty>
	Matrix<_Dty>& operator-=(Matrix<_Dty>& a, const Matrix<_Dty>& b)
	{
		__applyOperation(a, b, std::minus<_Dty>());
		return a;
	}

	template<class _Dty>
	Matrix<_Dty>& operator*=(Matrix<_Dty>& a, const Matrix<_Dty>& b)
	{
		__applyOperation(a, b, std::multiplies<_Dty>());
		return a;
	}

	template<class _Dty>
	Matrix<_Dty>& operator/=(Matrix<_Dty>& a, const Matrix<_Dty>& b)
	{
		__applyOperation(a, b, std::divides<_Dty>());
		return a;
	}

	template<class _Dty>
	Matrix<_Dty> operator+(Matrix<_Dty> a, const Matrix<_Dty>& b) { return a += b; }
	template<class _Dty>
	Matrix<_Dty> operator-(Matrix<_Dty> a, const Matrix<_Dty>& b) { return a -= b; }
	template<class _Dty>
	Matrix<_Dty> operator*(Matrix<_Dty> a, const Matrix<_Dty>& b) { return a *= b; }
	template<class _Dty>
	Matrix<_Dty> operator/(Matrix<_Dty> a, const Matrix<_Dty>& b) { return a /= b; }

	template<class _Dty>
	inline Matrix<_Dty> matmul(const Matrix<_Dty>& a, const Matrix<_Dty>& b) {
		if (a.shape.cols != b.shape.rows)
			throw std::invalid_argument("Number of cols for a and number of rows for b doesn't match.");

		constexpr int BLOCK_SIZE = 128; // キャッシュサイズに応じて調整
		const Matrix<_Dty> bt = b.transposed(); // 転置で列アクセス高速化
		Matrix<_Dty> c(MatShape{ a.shape.rows, b.shape.cols });

		const _Dty* a_data = a.data();
		const _Dty* bt_data = bt.data();
		_Dty* c_data = c.data();

		constexpr bool is_float = std::is_same_v<_Dty, float>;
		constexpr bool is_double = std::is_same_v<_Dty, double>;

		uint32_t M = a.shape.rows;
		uint32_t N = b.shape.cols;
		uint32_t K = a.shape.cols;

		for (uint32_t ii = 0; ii < M; ii += BLOCK_SIZE) {
			for (uint32_t jj = 0; jj < N; jj += BLOCK_SIZE) {
				for (uint32_t kk = 0; kk < K; kk += BLOCK_SIZE) {

					uint32_t i_max = std::min(ii + BLOCK_SIZE, M);
					uint32_t j_max = std::min(jj + BLOCK_SIZE, N);
					uint32_t k_max = std::min(kk + BLOCK_SIZE, K);

					for (uint32_t i = ii; i < i_max; ++i) {
						const _Dty* a_row = a_data + i * K;
						for (uint32_t j = jj; j < j_max; ++j) {
							const _Dty* bt_row = bt_data + j * K;
							_Dty sum = 0;
							uint32_t k = kk;

							if constexpr (is_float) {
								__m256 acc = _mm256_setzero_ps();
								for (; k + 8 <= k_max; k += 8) {
									__m256 va = _mm256_loadu_ps(a_row + k);
									__m256 vb = _mm256_loadu_ps(bt_row + k);
									acc = _mm256_fmadd_ps(va, vb, acc);
								}
								alignas(32) float temp[8];
								_mm256_store_ps(temp, acc);
								for (int l = 0; l < 8; ++l) sum += temp[l];
							}
							else if constexpr (is_double) {
								__m256d acc = _mm256_setzero_pd();
								for (; k + 4 <= k_max; k += 4) {
									__m256d va = _mm256_loadu_pd(a_row + k);
									__m256d vb = _mm256_loadu_pd(bt_row + k);
									acc = _mm256_fmadd_pd(va, vb, acc);
								}
								alignas(32) double temp[4];
								_mm256_store_pd(temp, acc);
								for (int l = 0; l < 4; ++l) sum += temp[l];
							}

							for (; k < k_max; ++k) {
								sum += a_row[k] * bt_row[k];
							}

							c_data[i * N + j] += sum;
						}
					}
				}
			}
		}
		return c;
	}

}
