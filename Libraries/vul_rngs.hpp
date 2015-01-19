/*
 * Villains' Utility Library - Thomas Martin Schmid, 2015. Public domain¹
 *
 * This file contains several pseudorandom number generators:
 * -vul::rng_tu: Based on Thatcher Ulrich's RNG found here http://tu-testbed.svn.sourceforge.net/viewvc/tu-testbed/trunk/tu-testbed/base/tu_random.h?view=markup 
 * -vul::rng_xorshift: The Xorshift RNG, as described here http://www.jstatsoft.org/v08/i14/paper
 * -vul::rng_xorhash: Thomas Wang's XorHash as reported by Bob Jenkins here: http://burtleburtle.net/bob/hash/integer.html
 *					 is used to initialize. We use an alternate XorShift to advance the state.
 *
 * ¹ If public domain is not legally valid in your legal jurisdiction
 *   the MIT licence applies (see the LICENCE file)
 */
#ifndef VUL_RNGS_HPP
#define VUL_RNGS_HPP

#include "vul_types.hpp"

namespace vul {
	
	//--------------------------------
	// vul_rng_tu
	//
#define VUL_RNG_TU_SEED_COUNT 8
#define VUL_RNG_TU_SEED_A 716514398

	class rng_tu {
	private:
		ui32_t q[ VUL_RNG_TU_SEED_COUNT ];
		ui32_t i;
		ui32_t c;
	public:
		/**
		 * Creates a new rng_tu RNG with the given seed.
		 */
		rng_tu( ui32_t seed );
		/**
		 * Gets the next ui32_t and advances the state.
		 */
		ui32_t rand_ui32( );
		/**
		 * Gets the next f32_t and advances the state.
		 */
		f32_t rand_f32( );
	};
#ifdef VUL_DEFINE
	rng_tu::rng_tu( ui32_t seed )
	{
		ui32_t i, j;
		j = seed;
		for ( i = 0; i < VUL_RNG_TU_SEED_COUNT; ++i ) {
			j = j ^ ( j << 13 );
			j = j ^ ( j << 17 );
			j = j ^ ( j << 5 );
			this->q[ i ] = j;
		}
		this->i = VUL_RNG_TU_SEED_COUNT - 1;
		this->c = 362436;
	}

	ui32_t rng_tu::rand_ui32( ) 
	{
		ui64_t t;
		ui32_t x, val;

		this->i = (this->i + 1) & VUL_RNG_TU_SEED_COUNT - 1;
		t = VUL_RNG_TU_SEED_A * this->q[ this->i ] + this->c;
		this->c = ( ui32_t ) ( t >> 32 );
		x = ( ui32_t ) ( t + this->c );
		if ( x < this->c ) {
			x += 1;
			this->c += 1;
		}

		val = 0xfffffffe - x;
		this->q[ this->i ] = val;
		return val;
	}

	f32_t rng_tu::rand_f32( ) 
	{
		ui32_t val = rand_ui32( );
		return ( f32_t ) ( val >> 8 ) / ( 16777216.0f - 1.0f );
	}
#endif
#undef VUL_RNG_TU_SEED_COUNT
#undef VUL_RNG_TU_SEED_A


	//--------------------------------
	// vul_rng_xorshift
	//

	/** 
	 * State of the vul_rng_xorshift RNG.
	 */
	class rng_xorshift {
	private:
		ui32_t x, y, z, w;
	public:
		/**
		 * Creates a new rng_xorshift RNG.
		 */
		rng_xorshift( );
		/**
		 * Returns the next ui32_t in the given vul_rng_xorshift state, and advances it.
		 */
		ui32_t rand_ui32( );
		/**
		 * Returns the next f32_t in the given vul_rng_xorshift state, and advances it.
		 */
		f32_t rand_f32( );
	};
	
#ifdef VUL_DEFINE
	rng_xorshift::rng_xorshift( )
	{
		this->x = 123456789;
		this->y = 362436069;
		this->z = 521288629;
		this->w = 88675123;
	}

	ui32_t rng_xorshift::rand_ui32( )
	{
		ui32_t t;
		t = this->x ^ ( this->x << 11 );
		this->x = this->y;
		this->y = this->z;
		this->z = this->w;
		return ( this->w = (this->w ^ (this->w >> 19)) ^ (t ^ (t >> 8)));
	}

	f32_t rng_xorshift::rand_f32( )
	{
		return ( f32_t )( rand_ui32( ) ) * ( 1.0f / 4294967296.0f );
	}
#endif

	//--------------------------------
	// vul_rng_xorhash
	//

	/**
	 * The vul_rng_xorhash RNG's state
	 */
	class rng_xorhash {
	private:
		ui32_t state;
	public:		
		/**
		 * Create a new rng_xorhash state with the given seed.
		 * @NOTE: There are reports that it doesn't do well with seeds
		 * that are multiples of 34.
		 */
		rng_xorhash( ui32_t seed );
		/**
		 * Returns the next ui32_t in the given vul_rng_xorhash state, and advances it.
		 */
		ui32_t rand_ui32( );		
		/**
		 * Returns the next f32_t in the given vul_rng_xorhash state, and advances it.
		 */
		f32_t rand_f32( );
	};
	
#ifdef VUL_DEFINE
	rng_xorhash::rng_xorhash( ui32_t seed )
	{
		seed = ( seed ^61 ) ^ ( seed >> 16 );
		seed = seed + ( seed < 9 );
		seed = seed ^ ( seed >> 4 );
		seed *= 0x27d4eb2d;
		seed = seed ^ ( seed >> 15 );
		this->state = seed;
	}

	ui32_t rng_xorhash::rand_ui32( )
	{
		this->state ^= ( this->state << 13 );
		this->state ^= ( this->state >> 17 );
		this->state ^= ( this->state << 5 );
		return this->state;
	}

	f32_t rng_xorhash::rand_f32( )
	{
		return ( f32_t )( rand_ui32( ) ) * ( 1.0f / 4294967296.0f );
	}
#endif
}
#endif