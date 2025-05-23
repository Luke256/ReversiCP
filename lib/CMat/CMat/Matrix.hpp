# pragma once

# include <vector>
# include <initializer_list>
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

		_Dty* data() { return m_data.data(); }
		const _Dty* data() const { return m_data.data(); }
		size_t size() { return m_data.size(); }

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
