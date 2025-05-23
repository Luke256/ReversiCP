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
		std::vector<_Dty> data;
		MatShape shape, strides;
	public:
		CMat() : shape(0, 0), strides(0, 0) {}
		CMat(const std::initializer_list<_Dty>& init) : data(init.begin(), init.end()), shape(1, init.size()), strides(init.size(), 1) {}

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
				data.insert(data.end(), i.begin(), i.end());
			}
		}

		CMat& operator+=(const CMat& a)
		{
			if (shape != a.shape) throw std::invalid_argument("Two shape of data for operand + doesn't match.");

			_Dty* p = data.data();
			const _Dty* q = a.data.data();
			const _Dty* end = p + data.size();

			while (p != end)
			{
				*p++ += *q++;
			}
			return *this;
		}

		CMat& operator-=(const CMat& a)
		{
			if (shape != a.shape) throw std::invalid_argument("Two shape of data for operand + doesn't match.");

			_Dty* p = data.data();
			const _Dty* q = a.data.data();
			const _Dty* end = p + data.size();

			while (p != end)
			{
				*p++ -= *q++;
			}
			return *this;
		}

		CMat& operator*=(const CMat& a)
		{
			if (shape != a.shape) throw std::invalid_argument("Two shape of data for operand + doesn't match.");

			_Dty* p = data.data();
			const _Dty* q = a.data.data();
			const _Dty* end = p + data.size();

			while (p != end)
			{
				*p++ *= *q++;
			}
			return *this;
		}

		CMat& operator/=(const CMat& a)
		{
			if (shape != a.shape) throw std::invalid_argument("Two shape of data for operand + doesn't match.");

			_Dty* p = data.data();
			const _Dty* q = a.data.data();
			const _Dty* end = p + data.size();

			while (p != end)
			{
				*p++ /= *q++;
			}
			return *this;
		}

		CMat operator+(const CMat& a) { return CMat(*this) += a; }
		CMat operator-(const CMat& a) { return CMat(*this) -= a; }
		CMat operator*(const CMat& a) { return CMat(*this) *= a; }
		CMat operator/(const CMat& a) { return CMat(*this) /= a; }

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
					if (j == 0) formatData.string += U" {}"_fmt(value.data[i * value.shape.rows + j]);
					else formatData.string += U", {}"_fmt(value.data[i * value.shape.rows + j]);
				}

				if (i == value.shape.rows - 1) formatData.string += U" }";
				else formatData.string += U" }\n";
			}
			formatData.string += U" }";
		}

#endif
	};

};
