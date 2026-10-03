#pragma once

#include <cmath>
#include <cstdint>

namespace Random {

	class Mersenne final {

	private:

		std::float_t const randmaxf = 1.f / 4294967295.f;
		// Random seed found by rolling a d20
		uint32_t seed{ 11 };

	public:

		Mersenne() = delete;

		Mersenne( std::uint32_t const& seed )
			: seed( seed ) {};

		std::float_t operator()() { return next() * randmaxf; };

	private:

		inline std::uint32_t next() {
			std::uint32_t x = seed = ( 1812433253U * ( seed ^ ( seed >> 30 ) ) + 1 ) & 0xFFFFFFFFU;
			x ^= ( x >> 11 );
			x ^= ( x << 7 ) & 0x9D2C5680U;
			x ^= ( x << 15 ) & 0xEFC60000U;
			return x ^= ( x >> 18 );
		};

	};

};
