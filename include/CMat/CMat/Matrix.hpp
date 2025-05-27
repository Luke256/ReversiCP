# pragma once

# include <vector>
# include <initializer_list>
# include <cstdint>
# include <stdexcept>
# include <immintrin.h>
# include "Shape.hpp"

namespace CMat
{

	template<class _Dty>
	class Matrix
	{
	private:
		std::vector<_Dty> m_data;
		MatShape m_strides;
	public:
		MatShape shape;

		Matrix() : shape(0, 0), m_strides(0, 0) {}
		Matrix(const std::initializer_list<_Dty>& init) : m_data(init.begin(), init.end()), shape(1, init.size()), m_strides(init.size(), 1) {}

		Matrix(const std::initializer_list<std::initializer_list<_Dty>>& init)
		{
			shape.rows = static_cast<uint32_t>(init.size());
			for (const auto& i : init)
			{
				if (shape.cols == 0)
				{
					shape.cols = static_cast<uint32_t>(i.size());
				}
				else if (shape.cols != static_cast<uint32_t>(i.size()))
				{
					throw std::invalid_argument("All rows of the initializer list needs to have the same number of elements.");
				}
				m_data.insert(m_data.end(), i.begin(), i.end());
			}
		}

		Matrix(const MatShape& shape, _Dty x = 0) : m_data(shape.cols* shape.rows, x), shape(shape) {}

		Matrix(const MatShape& shape, _Dty* x) : m_data(x, x + shape.cols* shape.rows), shape(shape) {}

		_Dty* data() { return m_data.data(); }
		const _Dty* data() const { return m_data.data(); }

		auto begin() { return m_data.begin(); }
		const auto begin() const { return m_data.begin(); }
		auto begin(uint32_t row) { return m_data.begin() + (shape.cols * row); }
		const auto begin(uint32_t row) const { return m_data.begin() + (shape.cols * row); }
		auto cbegin() { return m_data.cbegin(); }
		const auto cbegin() const { return m_data.cbegin(); }
		auto cbegin(uint32_t row) { return m_data.cbegin() + (shape.cols * row); }
		const auto cbegin(uint32_t row) const { return m_data.cbegin() + (shape.cols * row); }

		auto end() { return m_data.end(); }
		const auto end() const { return m_data.end(); }
		auto end(uint32_t row) { return begin(row) + shape.cols; }
		const auto end(uint32_t row) const { return begin(row) + shape.cols; }
		auto cend() { return m_data.cend(); }
		const auto cend() const { return m_data.cend(); }
		auto cend(uint32_t row) { return cbegin(row) + shape.cols; }
		const auto cend(uint32_t row) const { return cbegin(row) + shape.cols; }
		
		size_t size() const { return m_data.size(); }

		inline Matrix& transpose()
		{
			constexpr bool is_float = std::is_same_v<_Dty, float>;
			constexpr bool is_double = std::is_same_v<_Dty, double>;

			std::vector<_Dty>res(shape.cols * shape.rows);
			uint32_t i, j, k;

			for (i = 0; i < shape.rows; ++i)
			{
				j = 0;
				if constexpr (is_float)
				{
					for (; j + 8 <= shape.cols; j += 8)
					{
						__m256 vec = _mm256_loadu_ps(&m_data[i * shape.cols + j]);
						for (k = 0; k < 8; ++k)
						{
							res[(j + k) * shape.rows + i] = ((float*)&vec)[k];
						}
					}
				}
				else if constexpr (is_double)
				{
					for (; j + 4 <= shape.cols; j += 4)
					{
						__m256 vec = _mm256_loadu_pd(&m_data[i * shape.cols + j]);
						for (k = 0; k < 4; ++k)
						{
							res[(j + k) * shape.rows + i] = ((float*)&vec)[k];
						}
					}
				}

				for (; j < shape.cols; ++j)
				{
					res[j * shape.rows + i] = m_data[i * shape.cols + j];
				}
			}

			m_data.swap(res);
			std::swap(shape.cols, shape.rows);
			return *this;
		}

		inline Matrix transposed() const
		{
			return Matrix(*this).transpose();
		}

#ifdef SIV3D_INCLUDED

		friend void Formatter(FormatData& formatData, const Matrix& value)
		{
			formatData.string += U"{ ";
			for (uint32_t i : step(value.shape.rows))
			{
				if (i == 0) formatData.string += U"{";
				else formatData.string += U"  {";
				for (uint32_t j : step(value.shape.cols))
				{
					if (j == 0) formatData.string += U" {}"_fmt(value.m_data[i * value.shape.cols + j]);
					else formatData.string += U", {}"_fmt(value.m_data[i * value.shape.cols + j]);
				}

				if (i == value.shape.rows - 1) formatData.string += U" }";
				else formatData.string += U" }\n";
			}
			formatData.string += U" }";
		}

#endif
	};

	template<class _Dty>
	Matrix<_Dty> zeros(MatShape shape)
	{
		return Matrix<_Dty>(shape);
	}
	template<class _Dty>
	Matrix<_Dty> zerosLike(const Matrix<_Dty>& x)
	{
		return Matrix<_Dty>(x.shape);
	}

	template<class _Dty>
	Matrix<_Dty> ones(MatShape shape)
	{
		return Matrix<_Dty>(shape, 1);
	}
	template<class _Dty>
	Matrix<_Dty> onesLike(const Matrix<_Dty>& x)
	{
		return Matrix<_Dty>(x.shape, 1);
	}
};
