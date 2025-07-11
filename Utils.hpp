# pragma once

namespace BitBordUtils
{
	// Delta Swap
	inline uint64_t delta_swap(uint64_t x, uint64_t mask, int delta) {
		uint64_t t = (x ^ (x >> delta)) & mask;
		return x ^ t ^ (t << delta);
	}

	// 水平反転
	inline uint64_t flipHorizontal(uint64_t x) {
		x = delta_swap(x, 0x5555555555555555, 1);
		x = delta_swap(x, 0x3333333333333333, 2);
		return delta_swap(x, 0x0F0F0F0F0F0F0F0F, 4);
	}

	// A1-H8反転
	inline uint64_t flipDiagonalA1H8(uint64_t x) {
		x = delta_swap(x, 0x00AA00AA00AA00AA, 7);
		x = delta_swap(x, 0x0000CCCC0000CCCC, 14);
		return delta_swap(x, 0x00000000F0F0F0F0, 28);
	}

	// 時計回りに90度回転
	inline uint64_t rotateClockwise90(uint64_t x) {
		return flipHorizontal(flipDiagonalA1H8(x));
	}
}
