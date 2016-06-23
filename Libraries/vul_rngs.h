/*
 * Villains' Utility Library - Thomas Martin Schmid, 2016. Public domain?
 *
 * This file contains several pseudorandom number generators:
 * -vul_rng_tu: Based on Thatcher Ulrich's RNG found here http://tu-testbed.svn.sourceforge.net/viewvc/tu-testbed/trunk/tu-testbed/base/tu_random.h?view=markup 
 * -vul_rng_xorshift: The Xorshift RNG, as described here http://www.jstatsoft.org/v08/i14/paper
 * -vul_rng_xorhash: Thomas Wang's XorHash as reported by Bob Jenkins here: http://burtleburtle.net/bob/hash/integer.html
 *					 is used to initialize. We use an alternate XorShift to advance the state.
 * -vul_rng_mt19937: Mersenne twister 19937.
 * -vul_rng_pcg32: See http://www.pcg-random.org/ @NOTE(thynn): The core of this is Apache 2.0 licenced, see the comment on the definition of vul_rng_pcg32_nex_unsigned)
 *
 * ? If public domain is not legally valid in your legal jurisdiction
 *   the MIT licence applies (see the LICENCE file)
 */
#ifndef VUL_RNGS_H
#define VUL_RNGS_H

#include "vul_types.h"

#include <stdlib.h>
#ifndef VUL_OSX
#include <malloc.h>
#endif
#include <assert.h>
#include <math.h> // For ldexp

/**
 * If defined, the functions are defined and not just declared. Only do this in _one_ c/cpp file!
 */
//#define VUL_DEFINE

//--------------------------------
// vul_rng_tu
//

#define VUL_RNG_TU_SEED_COUNT 8
#define VUL_RNG_TU_SEED_A 716514398
/**
 * The state of the vul_rng_tu RNG.
 */
typedef struct vul_rng_tu {
	u32 q[ VUL_RNG_TU_SEED_COUNT ];
	u32 i;
	u32 c;
} vul_rng_tu;

#ifdef _cplusplus
extern "C" {
#endif
/**
 * Creates a new vul_rng_tu RNG with the given seed.
 */
vul_rng_tu *vul_rng_tu_create( u32 seed );
/**
 * Destroys a vul_rnd_tu state.
 */
void vul_rng_tu_destroy( vul_rng_tu *r );
/**
 * Gets the next unsigned integer from the given vul_rng_tu state, and advances it.
 */
u32 vul_rng_tu_next_unsigned( vul_rng_tu *r );
/**
 * Gets the next float in range [0,1) from the given vul_rng_tu state, and advacnes it.
 * The float has been rounded down to the neares multiple of 1/2^32
 */
f32 vul_rng_tu_next_float( vul_rng_tu *r );
#ifdef _cplusplus
}
#endif

//--------------------------------
// vul_rng_xorshift
//

/** 
 * State of the vul_rng_xorshift RNG.
 */
typedef struct vul_rng_xorshift {
	u32 x, y, z, w;
} vul_rng_xorshift;

#ifdef _cplusplus
extern "C" {
#endif
/**
 * Creates a new vul_rng_xorshift RNG.
 */
vul_rng_xorshift *vul_rng_xorshift_create( );
/**
 * Returns the next u32 in the given vul_rng_xorshift state, and advances it.
 */
u32 vul_rng_xorshift_next_unsigned( vul_rng_xorshift *r );
/**
 * Returns the next f32 in range [0,1) in the given vul_rng_xorshift state, and advances it.
 * The float has been rounded down to the neares multiple of 1/2^32
 */
f32 vul_rng_xorshift_next_float( vul_rng_xorshift *r );
#ifdef _cplusplus
}
#endif

//--------------------------------
// vul_rng_xorhash
//

/**
 * The vul_rng_xorhash RNG's state
 */
typedef struct vul_rng_xorhash {
	u32 s;
} vul_rng_xorhash;

#ifdef _cplusplus
extern "C" {
#endif
/**
 * Create a new vul_rng_xorhash state with the given seed.
 * @NOTE: There are reports that it doesn't do well with seeds
 * that are multiples of 34.
 */
vul_rng_xorhash *vul_rng_xorhash_create( u32 seed );
/**
 * Destroy a vul_rng_xorhash RNG state
 */
void vul_rng_xorhash_destroy( vul_rng_xorhash *r );
/**
 * Returns the next u32 in the given vul_rng_xorhash state, and advances it.
 */
u32 vul_rng_xorhash_next_unsigned( vul_rng_xorhash *r );
/**
 * Returns the next f32 in range [0,1) in the given vul_rng_xorhash state, and advances it.
 */
f32 vul_rng_xorhash_next_float( vul_rng_xorhash *r );
#ifdef _cplusplus
}
#endif

//--------------------------------
// vul_rng_mt19937
//

/**
 * The vul_rng_mt19937 RNG's state
 */
typedef struct vul_rng_mt19937 {
	s32 state[ 624 ];
	u32 index;
} vul_rng_mt19937;

#ifdef _cplusplus
extern "C" {
#endif
/**
 * Create a new vul_rng_mt19937 state with the given seed.
 * @NOTE: There are reports that it doesn't do well with seeds
 * that are multiples of 34.
 */
vul_rng_mt19937 *vul_rng_mt19937_create( u32 seed );
/**
 * Generate a new set of untampered numbers
 */
void vul_rng_mt19937_generate( vul_rng_mt19937 * r );
/**
 * Destroy a vul_rng_mt19937 RNG state
 */
void vul_rng_mt19937_destroy( vul_rng_mt19937 *r );
/**
 * Returns the next u32 in the given vul_rng_mt19937 state, and advances it.
 */
u32 vul_rng_mt19937_next_unsigned( vul_rng_mt19937 *r );
/**
 * Returns the next f32 in range [0,1) in the given vul_rng_mt19937 state, and advances it.
 * The float has been rounded down to the neares multiple of 1/2^32
 */
f32 vul_rng_mt19937_next_float( vul_rng_mt19937 *r );
#ifdef _cplusplus
}
#endif

//-------------------------------
// vul_rng_pcg32
//

/**
 * The vul_rng_pcg32 RNG's state
 */
typedef struct vul_rng_pcg32 {
	u64 state;
	u64 inc;
} vul_rng_pcg32;

#ifdef _cplusplus
extern "C" {
#endif

/**
 * Create a new vul_rng_pcg32 state with the given seed and initial
 * sequence.
 * initstate specifies where in the 2^64 period we are. The initseq 
 * specified which stream we will use.
 */
vul_rng_pcg32 *vul_rng_pcg32_create( u64 initstate, u64 initseq );
/**
 * Destroy the state 
 */
void vul_rng_pcg32_destroy( vul_rng_pcg32 *r );
/**
 * Returns the next u32 in the given state.
 */
u32 vul_rng_pcg32_next_unsigned( vul_rng_pcg32 *r );
/**
 * Returns the next f32 in range [0,1) in the given state.
 * The float has been rounded down to the neares multiple of 1/2^32
 */
f32 vul_rng_pcg32_next_float( vul_rng_pcg32 *r );

#ifdef _cplusplus
}
#endif
#endif

#ifdef VUL_DEFINE

#ifdef _cplusplus
extern "C" {
#endif

vul_rng_tu *vul_rng_tu_create( u32 seed )
{
	u32 i, j;
	vul_rng_tu *r = ( vul_rng_tu* )malloc( sizeof( vul_rng_tu ) );
	assert( r != NULL );

	j = seed;
	for ( i = 0; i < VUL_RNG_TU_SEED_COUNT; ++i ) {
		j = j ^ ( j << 13 );
		j = j ^ ( j << 17 );
		j = j ^ ( j << 5 );
		r->q[ i ] = j;
	}
	r->i = VUL_RNG_TU_SEED_COUNT - 1;
	r->c = 362436;

	return r;
}

void vul_rng_tu_destroy( vul_rng_tu *r )
{
	assert( r != NULL );
	free( r );
}

u32 vul_rng_tu_next_unsigned( vul_rng_tu *r ) 
{
	u64 t;
	u32 x, val;

	assert( r != NULL );
	r->i = (r->i + 1) & VUL_RNG_TU_SEED_COUNT - 1;
	t = VUL_RNG_TU_SEED_A * r->q[ r->i ] + r->c;
	r->c = ( u32 ) ( t >> 32 );
	x = ( u32 ) ( t + r->c );
	if ( x < r->c ) {
		x += 1;
		r->c += 1;
	}

	val = 0xfffffffe - x;
	r->q[ r->i ] = val;
	return val;
}

f32 vul_rng_tu_next_float( vul_rng_tu *r ) 
{
	u32 val = vul_rng_tu_next_unsigned( r );
	return ( f32 ) ( val >> 8 ) / (16777216.0f - 1.0f);
}

vul_rng_xorshift *vul_rng_xorshift_create( )
{
	vul_rng_xorshift *r = ( vul_rng_xorshift* )malloc( sizeof( vul_rng_xorshift ) );
	assert( r );

	r->x = 123456789;
	r->y = 362436069;
	r->z = 521288629;
	r->w = 88675123;

	return r;
}

u32 vul_rng_xorshift_next_unsigned( vul_rng_xorshift *r )
{
	u32 t;
	t = r->x ^ ( r->x << 11 );
	r->x = r->y;
	r->y = r->z;
	r->z = r->w;
	return ( r->w = (r->w ^ (r->w >> 19)) ^ (t ^ (t >> 8)));
}

f32 vul_rng_xorshift_next_float( vul_rng_xorshift *r )
{
	return ( f32 )ldexp( vul_rng_xorshift_next_unsigned( r ), -32 );
}

vul_rng_xorhash *vul_rng_xorhash_create( u32 seed )
{
	vul_rng_xorhash *r = ( vul_rng_xorhash* )malloc( sizeof( vul_rng_xorhash ) );
	assert( r != NULL );

	seed = ( seed ^61 ) ^ ( seed >> 16 );
	seed = seed + ( seed < 9 );
	seed = seed ^ ( seed >> 4 );
	seed *= 0x27d4eb2d;
	seed = seed ^ ( seed >> 15 );
	r->s = seed;

	return r;
}

void vul_rng_xorhash_destroy( vul_rng_xorhash *r )
{
	assert( r != NULL );
	free( r );
}

u32 vul_rng_xorhash_next_unsigned( vul_rng_xorhash *r )
{
	r->s ^= ( r->s << 13 );
	r->s ^= ( r->s >> 17 );
	r->s ^= ( r->s << 5 );
	return r->s;
}

f32 vul_rng_xorhash_next_float( vul_rng_xorhash *r )
{
	return ( f32 )ldexp( vul_rng_xorhash_next_unsigned( r ), -32 );
}

vul_rng_mt19937 *vul_rng_mt19937_create( u32 seed )
{
	u32 i;

	vul_rng_mt19937 *r = ( vul_rng_mt19937* )malloc( sizeof( vul_rng_mt19937 ) );
	assert( r != NULL );

	r->index = 0;
	r->state[ 0 ] = seed;

	for( i = 1; i < 624; ++i ) {
		r->state[ i ] = ( s32 )( 1812433253LL 
					  * ( ( s64 )r->state[ i -1 ] ^ ( ( s64 )r->state[ i ] >> 30LL ) ) + i );
	}
	return r;	
}

void vul_rng_mt19937_generate( vul_rng_mt19937 * r )
{
	u32 i;
	s32 y;

	for ( i = 0; i < 624; ++ i ) {
		y = ( r->state[ i ] & 0x80000000 ) + ( r->state[ ( i + 1 ) % 624 ] & 0x7fffffff );
		r->state[ i ] = r->state[ ( i + 397 ) % 624 ] ^ ( y >> 1 );
		if( y % 2 != 0 ) {
			r->state[ i ] = r->state[ i ] ^ 0x9908b0df;
		}
	}
}

void vul_rng_mt19937_destroy( vul_rng_mt19937 *r )
{
	assert( r != NULL );
	free( r );
}

u32 vul_rng_mt19937_next_unsigned( vul_rng_mt19937 *r )
{
	u32 y;

	if( r->index == 0 ) {
		vul_rng_mt19937_generate( r );
	}
	
	y = *( ( u32* )&r->state[ r->index ] );
	y = y ^ ( y >> 11 );
	y = y ^ ( ( y << 7 ) & 0x9d2c5680 );
	y = y ^ ( ( y << 15 ) & 0xefc60000 );
	y = y ^ ( y >> 18 );

	r->index = ( r->index + 1 ) % 624;
	
	return y;	
}

f32 vul_rng_mt19937_next_float( vul_rng_mt19937 *r )
{
	return ( f32 )ldexp( vul_rng_mt19937_next_unsigned( r ), -32 );
}

vul_rng_pcg32 *vul_rng_pcg32_create( u64 initstate, u64 initseq )
{
	vul_rng_pcg32 *r = ( vul_rng_pcg32* )malloc( sizeof( vul_rng_pcg32 ) );
	assert( r != NULL );

	r->state = initstate;
	r->inc = initseq;

	return r;
}

void vul_rng_pcg32_destroy( vul_rng_pcg32 *r )
{
	free( r );
}

/**
 * The following function follows it's own licence:
 * *Really* minimal PCG32 code / (c) 2014 M.E. O'Neill / pcg-random.org
 * Licensed under Apache License 2.0 (NO WARRANTY, etc. see website)
 */
u32 vul_rng_pcg32_next_unsigned( vul_rng_pcg32 *r )
{
	u64 old = r->state;
	r->state = old * 6364136223846793005ULL + ( r->inc | 1 );
	u32 xsh = ( u32 )( ( ( old >> 18u ) ^ old ) >> 27u );
	u32 rot = old >> 59u;
	return ( xsh >> rot ) | ( xsh << ( ( -( s32 )rot ) & 31 ) );
}

f32 vul_rng_pcg32_next_float( vul_rng_pcg32 *r )
{
	return ( f32 )ldexp( vul_rng_pcg32_next_unsigned( r ), -32 );
}
#ifdef _cplusplus
}
#endif

#endif
