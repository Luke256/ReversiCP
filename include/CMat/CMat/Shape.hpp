# pragma once

namespace CMat
{
	class MatShape
	{
	public:
		uint32_t rows, cols;

		MatShape(): rows(0), cols(0) {}
		MatShape(uint32_t r, uint32_t c): rows(r), cols(c) {}

		bool operator==(const MatShape& a) { return rows == a.rows and cols == a.cols; }
		bool operator!=(const MatShape& a) { return rows != a.rows or cols != a.cols; }

#ifdef SIV3D_INCLUDED

		friend void Formatter(FormatData& formatData, const MatShape& value)
		{
			formatData.string += U"({}, {})"_fmt(value.rows, value.cols);
		}

#endif
	};
};
