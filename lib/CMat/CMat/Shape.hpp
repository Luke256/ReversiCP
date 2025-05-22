# pragma once

namespace CMat
{
	class MatShape
	{
	public:
		uint32_t rows, cols;

		MatShape(): rows(0), cols(0) {}
		MatShape(uint32_t r, uint32_t c): rows(r), cols(c) {}
	};
};
