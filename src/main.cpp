#include <cstdint>
#include <iostream>
#include <memory>
#include <omp.h>

#include "./component.hpp"
#include "./mersenne.hpp"
#include "./diffuse-thermal.hpp"

int main(
	[[maybe_unused]] int argc,
	[[maybe_unused]] char* argv[]
) {
	std::cout << "Programme start." << std::endl;

	omp_set_num_threads( omp_get_max_threads() );

	Component canvas( 400, 400 );

	// Generate initial state
	Random::Mersenne rnd( 985456376 );
	for ( std::uint16_t y{ 0 }; y < canvas.height; ++y )
		for ( std::uint16_t x{ 0 }; x < canvas.width; ++x )
			canvas.data[x][y] = rnd();

	// Direct construction
	DiffuseThermal::Circle circle( 10, 20 );

	// Smart pointer
	// std::unique_ptr<DiffuseThermal::Polymorphic> p_pattern = std::make_unique<DiffuseThermal::Diamond>( 20, 40 );
	// std::unique_ptr<DiffuseThermal::Polymorphic> p_pattern = std::make_unique<DiffuseThermal::Square>( 20, 40 );

	std::uint32_t const max_iteration{ 100 };
	std::cout << "\rIteration: " << 0 << '/' << max_iteration << std::flush;
	// Process
	for ( std::uint32_t i{ 1 }; i <= max_iteration; ++i ) {
		// Direct, swap
		auto scratch_circle = circle.process( canvas );
		std::swap( canvas, scratch_circle );

		// Smart pointer, swap
		// auto scratch_smart = p_pattern->process( canvas );
		// std::swap( canvas, scratch_smart );

		// Average of circle and smart (no swap)
		// for ( std::uint16_t y{ 0 }; y < canvas.height; ++y )
		// 	for ( std::uint16_t x{ 0 }; x < canvas.width; ++x )
		// 		canvas.data[x][y] = ( scratch_circle.data[x][y] + scratch_smart.data[x][y] ) / 2;

		std::cout << "\rIteration: " << i << '/' << max_iteration << std::flush;
	}

	canvas.SaveTGA( "pattern" );

	std::cout << "\nProgramme end." << std::endl;

	return EXIT_SUCCESS;
};
