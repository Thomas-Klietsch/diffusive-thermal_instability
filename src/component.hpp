#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <valarray>
#include <vector>

class Component final {

public:

	std::uint16_t width{ 0 };
	std::uint16_t height{ 0 };
	std::vector<std::vector<std::float_t>> data{ 0 };

public:

	Component() = delete;

	Component(
		std::uint16_t const a_width,
		std::uint16_t const a_height
	) {
		if ( !a_width || !a_height )
			return;
		width = a_width;
		height = a_height;
		data = std::vector<std::vector<std::float_t>>( width, std::vector<std::float_t>( height, 0.f ) );
	};

	Component(
		Component const& canvas
	) {
		if ( !canvas.width || !canvas.height )
			return;
		width = canvas.width;
		height = canvas.height;
		data = std::vector<std::vector<std::float_t>>( width, std::vector<std::float_t>( height, 0.f ) );
	};

	// Maps a data cell to RGB colour, in the range [0;1].
	// Returns empty valarray if x or y is outside valid area.
	std::valarray<std::float_t> pixel(
		std::uint16_t const x,
		std::uint16_t const y
	) const {
		if ( x >= width || y >= height )
			return {};
		return { data[x][y], data[x][y], data[x][y] };
	};

	bool SaveTGA(
		std::string const& filename
	) {
		if ( !width || !height || filename.size() == 0 ) {
			std::cout << "Error! Empty canvas and/or filename.\n";
			return false;
		};

		std::ofstream file( filename + ".tga", std::ios::binary );
		if ( !file ) {
			std::cout << "Could not open file \"" + filename + ".tga\" to save image.\n";
			return false;
		};

		std::vector<std::uint8_t> header( 32, 0 ); // header (18B) + ID field (14B)
		header[0] = 14;
		header[2] = 2; // Uncompressed true colour
		header[12] = static_cast<std::uint8_t>( width % 256 );
		header[13] = static_cast<std::uint8_t>( width / 256 );
		header[14] = static_cast<std::uint8_t>( height % 256 );
		header[15] = static_cast<std::uint8_t>( height / 256 );
		header[16] = 24; // Bits per pixel
		header[17] = 0b100000; // Screen origin (bit 5), 0 lower left, 1 upper left (bit 0-7)

		std::vector<std::uint8_t> image( width * height * 3 );
		std::float_t const gamma = 1.f / 2.2f;

		for ( std::int32_t y{ 0 }; y < height; ++y )
			for ( std::int32_t x{ 0 }; x < width; ++x ) {
				std::uint32_t const index = x + y * width;
				auto colour = pixel( x, y );
				// Slight blur, to soften hard edges
				colour = colour * 0.9f + 0.1f * 0.25f * ( pixel( std::max( 0, x - 1 ), y ) + pixel( std::min( x + 1, width - 1 ), y ) + pixel( x, std::max( 0, y - 1 ) ) + pixel( x, std::min( y + 1, height - 1 ) ) );
				if ( colour.size() == 3 ) {
					// TGA uses BGR colour ordering
					image[index * 3 + 2] = std::clamp<std::uint8_t>( std::pow( std::clamp( colour[0], 0.f, 1.f ), gamma ) * 255, 0, 255 );
					image[index * 3 + 1] = std::clamp<std::uint8_t>( std::pow( std::clamp( colour[1], 0.f, 1.f ), gamma ) * 255, 0, 255 );
					image[index * 3 + 0] = std::clamp<std::uint8_t>( std::pow( std::clamp( colour[2], 0.f, 1.f ), gamma ) * 255, 0, 255 );
				}
			}

		file.write( reinterpret_cast<char*>( &header[0] ), header.size() );
		file.write( reinterpret_cast<char*>( &image[0] ), image.size() );
		file.close();
		return true;
	};
};
