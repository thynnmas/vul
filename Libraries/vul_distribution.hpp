/*
 * Villains' Utility Library - Thomas Martin Schmid, 2014. Public domain¹
 *
 * This file contains several number distribution functions.
 * -vul_halton_par: Halton series of 2 given numbers. Distributes values 
 *					uniformely in the range [0, 1]
 * 
 * ¹ If public domain is not legally valid in your legal jurisdiction
 *   the MIT licence applies (see the LICENCE file)
 */
#ifndef VUL_DISTRIBUTIONS_HPP
#define VUL_DISTRIBUTIONS_HPP

#include "vul_types.hpp"

namespace vul {

	/**
	 * Halton pair class
	 */
	class halton_pair {
	private:
		f32_t value_a, value_b;
		f32_t inv_base_a, inv_base_b;

	public:
		/**
		 * Creates a new halton pair. Uses Thomas Wang's integer hash to 
		 * initialize values from the seed, where we use the integer hash on the seed
		 * to find two new seeds sa, sb. We then initialize value_a = 1-(sa/max)
		 * and value_b = [ 1-(sb/max) ]^2
		 */
		halton_pair( ui32_t base1, ui32_t base2, ui32_t seed );
		/**
		 * Advances the state of the pair.
		 */
		void next( );
		/**
		 * Fetches two values from the pair.
		 */
		void fetch( f32_t *v1, f32_t *v2 );
	};

#ifdef VUL_DEFINE
	halton_pair::halton_pair( ui32_t base1, ui32_t base2, ui32_t seed )
	{
		seed = (seed ^ 61) ^ (seed >> 16);
		seed *= 9;
		seed = seed ^ (seed >> 4);
		seed *= 0x27d4eb2d;
		seed = seed ^ (seed >> 15);

		this->value_a = 1.f - ( ( f32_t )seed / std::numeric_limits< f32_t >::max( ) );

		seed ^= ( seed << 13 );
		seed ^= ( seed >> 17 );
		seed ^= ( seed << 5 );

		this->value_b = 1.f - ( ( f32_t )seed / std::numeric_limits< f32_t >::max( ) );
		this->value_b *= this->value_b;
		this->inv_base_a = 1.f / base1;
		this->inv_base_b = 1.f / base2;
	}
	
	void halton_pair::next( )
	{	
		f32_t r = 1.f - ( this->value_a - 0.0000001f );
		if ( this->inv_base_a < r ) {
			this->value_a += this->inv_base_a;
		} else {
			f32_t h = this->inv_base_a, h2;
			do{
				h2 = h; 
				h *= this->inv_base_a;
			} while ( h >= r );
			this->value_a += h2 + h - 1.f;
		}
		r = 1.f - this->value_a - 0.0000001f;
		if ( this->inv_base_a < r ) {
			this->value_b += this->inv_base_a;
		} else {
			f32_t h = this->inv_base_a, h2;
			do{
				h2 = h; 
				h *= this->inv_base_a;
			} while ( h >= r );
			this->value_b += h2 + h - 1.f;
		}
	}

	void halton_pair::fetch( f32_t *v1, f32_t *v2 )
	{
		*v1 = this->value_a;
		*v2 = this->value_b;
	}
#endif
}

#endif