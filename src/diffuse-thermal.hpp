// Copyright (c) 2026 Thomas Klietsch, all rights reserved.
//
// Licensed under the GNU Lesser General Public License, version 3.0 or later
//
// This program is free software: you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation, either version 3 of
// the License, or ( at your option ) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General
// Public License along with this program.If not, see < https://www.gnu.org/licenses/>.

#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <omp.h>

#include "./component.hpp"

// Diffusive thermal instability, using shaped areas
namespace DiffuseThermal {

	// Define the abstract class
	class Polymorphic {

	public:

		virtual ~Polymorphic() {};

		// Mandatory interface
		virtual Component process( Component const& ) const = 0;
	};

	// Derived classes:

	class Circle final : public DiffuseThermal::Polymorphic {

	protected:

		// Largest radius
		std::uint8_t size;

		// Change per iteration
		std::float_t const delta{ 0.25 };

		// Radius squared
		std::uint16_t radius_inner2;
		std::uint16_t radius_outer2;

		// Ratio between area normalised shapes, with optional skew multiplier
		std::float_t const scalar;

	public:

		Circle() = delete;

		Circle(
			std::uint8_t const inner_radius,
			std::uint8_t const outer_radius,
			std::float_t const skew = 1.f
		)
			: size( std::max( inner_radius, outer_radius ) )
			, radius_inner2( inner_radius * inner_radius )
			, radius_outer2( outer_radius * outer_radius )
			// Constants for area calculation cancels out (i.e. pi)
			, scalar( ( skew * outer_radius * outer_radius ) / ( inner_radius * inner_radius ) ) {};

		~Circle() {};

		Component process(
			Component const& source
		) const override {
			Component destination( source );

			// Lambda function
			// Calculate area sums
			auto Sigma = [&]( std::uint16_t const x, std::uint16_t y ) -> std::array<std::float_t, 2>
			{
				std::float_t sum_inner{ 0 };
				std::float_t sum_outer{ 0 };
				for ( std::int16_t dx = -size; dx <= size; ++dx )
					for ( std::int16_t dy = -size; dy <= size; ++dy ) {
						std::uint16_t const radius = dx * dx + dy * dy;
						std::float_t const value = source.data[( x + dx + source.width ) % source.width][( y + dy + source.height ) % source.height];
						if ( radius <= radius_inner2 )
							sum_inner += value;
						if ( radius <= radius_outer2 )
							sum_outer += value;
					}
				return { sum_inner * scalar, sum_outer };
			};

			for ( std::uint16_t x{ 0 }; x < source.width; ++x )
#pragma omp parallel for
				for ( std::uint16_t y = 0; y < source.height; ++y ) {
					auto const sums = Sigma( x, y );
					destination.data[x][y] = std::clamp(
						sums[0] > sums[1]
							? source.data[x][y] + delta
							: source.data[x][y] - delta,
						0.f, 1.f
					);
				}

			return destination;
		};
	};

	class Diamond final : public DiffuseThermal::Polymorphic {

	protected:

		// Largest half width/height
		std::uint8_t size;

		// Change per iteration
		std::float_t const delta{ 0.25f };

		// Half width/height
		std::uint8_t size_inner;
		std::uint8_t size_outer;

		// Ratio between area normalised shapes, with optional skew multiplier
		std::float_t scalar{ 1 };

	public:

		Diamond() = delete;

		Diamond(
			std::uint8_t const inner_size,
			std::uint8_t const outer_size,
			std::float_t const skew = 1.f
		) {
			size_inner = std::ceil( inner_size / 2 );
			size_outer = std::ceil( outer_size / 2 );
			size = std::max( size_inner, size_outer );
			scalar = ( skew * size_outer * size_outer ) / ( size_inner * size_inner );
		};

		~Diamond() {};

		Component process( Component const& source ) const override {
			Component destination( source );

			// Lambda function
			// Calculate area sums
			auto Sigma = [&]( std::uint16_t const x, std::uint16_t y ) -> std::array<std::float_t, 2>
			{
				std::float_t sum_inner{ 0 };
				std::float_t sum_outer{ 0 };
				for ( std::int16_t dx = -size; dx <= size; ++dx )
					for ( std::int16_t dy = -size; dy <= size; ++dy ) {
						std::uint16_t const max_span = std::abs( dx ) + std::abs( dy );
						std::float_t const value = source.data[( x + dx + source.width ) % source.width][( y + dy + source.height ) % source.height];
						if ( max_span <= size_inner )
							sum_inner += value;
						if ( max_span <= size_outer )
							sum_outer += value;
					}
				return { sum_inner * scalar, sum_outer };
			};

			for ( std::uint16_t x{ 0 }; x < source.width; ++x )
#pragma omp parallel for
				for ( std::uint16_t y = 0; y < source.height; ++y ) {
					auto const sums = Sigma( x, y );
					destination.data[x][y] = std::clamp(
						sums[0] > sums[1]
							? source.data[x][y] + delta
							: source.data[x][y] - delta,
						0.f, 1.f
					);
				}

			return destination;
		};
	};

	class Square final : public DiffuseThermal::Polymorphic {

	protected:

		// Largest half width/height
		std::uint8_t size;

		// Change per iteration
		std::float_t const delta{ 0.25f };

		// Half width/height
		std::uint8_t size_inner;
		std::uint8_t size_outer;

		// Ratio between area normalised shapes, with optional skew multiplier
		std::float_t scalar{ 1 };

	public:

		Square() = delete;

		Square(
			std::uint8_t const inner_size,
			std::uint8_t const outer_size,
			std::float_t const skew = 1.f
		) {
			size_inner = std::ceil( inner_size / 2 );
			size_outer = std::ceil( outer_size / 2 );
			size = std::max( size_inner, size_outer );
			scalar = ( skew * size_outer * size_outer ) / ( size_inner * size_inner );
		};

		~Square() {};

		Component process( Component const& source ) const override {
			Component destination( source );

			// Lambda function
			// Calculate area sums
			auto Sigma = [&]( std::uint16_t const x, std::uint16_t y ) -> std::array<std::float_t, 2>
			{
				std::float_t sum_inner{ 0 };
				std::float_t sum_outer{ 0 };
				for ( std::int16_t dx = -size; dx <= size; ++dx )
					for ( std::int16_t dy = -size; dy <= size; ++dy ) {
						std::uint16_t const max_span = std::max( std::abs( dx ), std::abs( dy ) );
						std::float_t const value = source.data[( x + dx + source.width ) % source.width][( y + dy + source.height ) % source.height];
						if ( max_span <= size_inner )
							sum_inner += value;
						if ( max_span <= size_outer )
							sum_outer += value;
					}
				return { sum_inner * scalar, sum_outer };
			};

			for ( std::uint16_t x{ 0 }; x < source.width; ++x )
#pragma omp parallel for
				for ( std::uint16_t y = 0; y < source.height; ++y ) {
					auto const sums = Sigma( x, y );
					destination.data[x][y] = std::clamp(
						sums[0] > sums[1]
							? source.data[x][y] + delta
							: source.data[x][y] - delta,
						0.f, 1.f
					);
				}

			return destination;
		};
	};
};
