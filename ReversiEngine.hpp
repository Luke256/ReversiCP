#pragma once

namespace Reversi
{
	struct TupleHash {
		std::size_t operator()(const std::tuple<uint64_t, uint64_t, bool>& t) const noexcept {
			uint64_t a, b;
			bool c;
			std::tie(a, b, c) = t;

			// 高速で分布の良いハッシュ関数（SplitMix64ベース）
			auto hash64 = [](uint64_t x) {
				x += 0x9e3779b97f4a7c15;
				x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9;
				x = (x ^ (x >> 27)) * 0x94d049bb133111eb;
				return x ^ (x >> 31);
				};

			uint64_t h1 = hash64(a);
			uint64_t h2 = hash64(b);
			uint64_t h3 = hash64(static_cast<uint64_t>(c));

			// ハッシュ値を混合する（rotate xor加算など）
			uint64_t combined = h1;
			combined ^= (h2 << 1) | (h2 >> 63);
			combined ^= (h3 << 2) | (h3 >> 62);

			return static_cast<std::size_t>(combined);
		}
	};

	class ReversiEngine
	{
	private:
		uint64_t m_blacks, m_whites;
		bool m_blackTurn;

		/// @brief 二次元座標をビットに変換します
		/// @param x 左から何番目か
		/// @param y 上から何番目か
		/// @return ビッドボードでのマスク
		uint64_t pos2bit(uint32_t x, uint32_t y) const;

		uint64_t transfer(uint64_t put, uint32_t dir) const;

	public:
		ReversiEngine();

		void reset();

		uint64_t getLegals(bool inverseTurn = false) const;

		bool place(uint32_t x, uint32_t y);

		void getBoard(std::vector<int32_t>& board) const;

		void pass();

		void setState(uint64_t blacks, uint64_t whites, bool blackTurn);

		void swapBW();

		bool isBlackTurn() const;

		int32_t getNBlacks() const;

		int32_t getNWhites() const;

		uint64_t getBlacks() const;

		uint64_t getWhites() const;

		bool isFinished();

		inline std::tuple<uint64_t, uint64_t, bool> getTupleState() const
		{
			return { m_blacks, m_whites, m_blackTurn };
		}
	};

	void bit2boad(const uint64_t& bit, std::vector<int32_t>& board);

};
