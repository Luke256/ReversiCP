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
	class CMat
	{
	private:
		std::vector<_Dty> m_data;
		MatShape m_strides;
	public:
		MatShape shape;

		CMat() : shape(0, 0), m_strides(0, 0) {}
		CMat(const std::initializer_list<_Dty>& init) : m_data(init.begin(), init.end()), shape(1, init.size()), m_strides(init.size(), 1) {}

		CMat(const std::initializer_list<std::initializer_list<_Dty>>& init)
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

		CMat(const MatShape& shape): m_data(shape.cols * shape.rows), shape(shape) {}

		_Dty* data() { return m_data.data(); }
		const _Dty* data() const { return m_data.data(); }
		size_t size() { return m_data.size(); }


		inline CMat& transpose()
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

		inline CMat transposed() const
		{
			return CMat(*this).transpose();
		}

#ifdef SIV3D_INCLUDED

		friend void Formatter(FormatData& formatData, const CMat& value)
		{
			formatData.string += U"{ ";
			for (uint32_t i : step(value.shape.rows))
			{
				if (i == 0) formatData.string += U"{";
				else formatData.string += U"  {";
				for (uint32_t j : step(value.shape.cols))
				{
					if (j == 0) formatData.string += U" {}"_fmt(value.m_data[i * value.shape.rows + j]);
					else formatData.string += U", {}"_fmt(value.m_data[i * value.shape.rows + j]);
				}

				if (i == value.shape.rows - 1) formatData.string += U" }";
				else formatData.string += U" }\n";
			}
			formatData.string += U" }";
		}

#endif
	};
};
