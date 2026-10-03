#### Introduction

The algorithm use (shaped) areas to simulate the diffusion coefficients of two properties in one media. This is memory bandwidth and cache heavy.

This method was described by Yakov Borisovich Zeldovich, and similar to the Turing mechanism.

Examples of the effect of a shape on generated patterns. Circle, diamond and square.

![Circle](/image/pattern-circle-1.000.png)
![Diamond](/image/pattern-diamond.png)
![Square](/image/pattern-square.png)

Rewrite of some old C code (~15 years ago). I was still using Fortran/Pascal habits, like declaring all variables at the start of the scope. That is some ugly code \*tries to wash eyes\*.

#### Usage

Note that setting the (optional) skew value too high/low can result in a pure white/black image.

Generate a 400x400 image, from random noise, using circle pattern, for 100 iterations. Circles have radius of 10 (inner) and 20 (outer).

	Component canvas( 400, 400 );
	Random::Mersenne rnd( 985456376 );
	for ( std::uint16_t y{ 0 }; y < canvas.height; ++y )
		for ( std::uint16_t x{ 0 }; x < canvas.width; ++x )
			canvas.data[x][y] = rnd();
	DiffuseThermal::Circle circle( 10, 20 );
	for ( std::uint32_t i{ 1 }; i <= 100; ++i ) {
		auto scratch_circle = circle.process( canvas );
		std::swap( canvas, scratch_circle );
	}
	canvas.SaveTGA( "pattern" );

Outut:

![Circle](/image/pattern-circle-1.000.png)


#### Requirement

C++17

#### Additional examples

Circle patterns with different skew values; 0.9, 1, 1.05

![Circle skew 0.90](/image/pattern-circle-0.900.png)
![Circle skew 1.00](/image/pattern-circle-1.000.png)
![Circle skew 1.05](/image/pattern-circle-1.050.png)

Circle pattern, with skew value (1.075) near the limit of pure white result (for this setup). Iterations of 100 and 400.

![Circle iterations 100](/image/pattern-circle-1.075-100.png)
![Circle iterations 400](/image/pattern-circle-1.075-400.png)

Average of circle and diamond patterns.

![Average](/image/pattern-average.png)
