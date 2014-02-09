/*
 * Villains' Utility Library - Thomas Martin Schmid, 2014. Public domain�
 *
 * This file contains several pseudorandom number generators:
 * -vul_rng_tu: Based on Thatcher Ulrich's RNG found here http://tu-testbed.svn.sourceforge.net/viewvc/tu-testbed/trunk/tu-testbed/base/tu_random.h?view=markup 
 * -vul_rng_xorshift: The Xorshift RNG, as described here http://www.jstatsoft.org/v08/i14/paper
 * -vul_rng_xorhash: Thomas Wang's XorHash as reported by Bob Jenkins here: http://burtleburtle.net/bob/hash/integer.html
 *					 is used to initialize. We use an alternate XorShift to advance the state.
 *
 * � If public domain is not legally valid in your country and or legal area,
 *   the MIT licence applies (see the LICENCE file)
 */
#ifndef VUL_RNGS_H
#define VUL_RNGS_H

#include "vul_types.h"

#include <stdlib.h>
#include <malloc.h>
#include <assert.h>

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
typedef struct {
	ui32_t q[ VUL_RNG_TU_SEED_COUNT ];
	ui32_t i;
	ui32_t c;
} vul_rng_tu_t;

/**
 * Creates a new vul_rng_tu RNG with the given seed.
 */
#ifndef VUL_DEFINE
vul_rng_tu_t *vul_rng_tu_create( ui32_t seed );
#else
vul_rng_tu_t *vul_rng_tu_create( ui32_t seed )
{
	ui32_t i, j;
	vul_rng_tu_t *r = ( vul_rng_tu_t* )malloc( sizeof( vul_rng_tu_t ) );
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
#endif

/**
 * Destroys a vul_rnd_tu state.
 */
void vul_rng_tu_destroy( vul_rng_tu_t *r )
{
	assert( r != NULL );
	free( r );
}

/**
 * Gets the next unsigned integer from the given vul_rng_tu state, and advances it.
 */
#ifndef VUL_DEFINE
ui32_t vul_rng_tu_next( vul_rng_tu_t *r );
#else
ui32_t vul_rng_tu_next( vul_rng_tu_t *r ) 
{
	ui64_t t;
	ui32_t x, val;

	assert( r != NULL );
	r->i = (r->i + 1) & VUL_RNG_TU_SEED_COUNT - 1;
	t = VUL_RNG_TU_SEED_A * r->q[ r->i ] + r->c;
	r->c = ( ui32_t ) ( t >> 32 );
	x = ( ui32_t ) ( t + r->c );
	if ( x < r->c ) {
		x += 1;
		r->c += 1;
	}

	val = 0xfffffffe - x;
	r->q[ r->i ] = val;
	return val;
}
#endif
/**
 * Gets the next float from the given vul_rng_tu state, and advacnes it.
 */
#ifndef VUL_DEFINE
f32_t vul_rng_tu_next( vul_rng_tu_t *r );
#else
f32_t vul_rng_tu_next( vul_rng_tu_t *r ) 
{
	ui32_t val = vul_rng_tu_next( r );
	return ( f32_t ) ( val >> 8 ) / (16777216.0f - 1.0f);
}
#endif


//--------------------------------
// vul_rng_xorshift
//

/** 
 * State of the vul_rng_xorshift RNG.
 */
typedef struct {
	ui32_t x, y, z, w;
} vul_rng_xorshift_t;

/**
 * Creates a new vul_rng_xorshift RNG.
 */
#ifndef VUL_DEFINE
vul_rng_xorshift_t *vul_rng_xorshift_create( );
#else
vul_rng_xorshift_t *vul_rng_xorshift_create( )
{
	vul_rng_xorshift_t *r = ( vul_rng_xorshift_t* )malloc( sizeof( vul_rng_xorshift_t ) );

	r->x = 123456789;
	r->y = 362436069;
	r->z = 521288629;
	r->w = 88675123;

	return r;
}
#endif

/**
 * Returns the next ui32_t in the given vul_rng_xorshift state, and advances it.
 */
#ifndef VUL_DEFINE
ui32_t vul_rng_xorshift_next( vul_rng_xorshift_t *r );
#else
ui32_t vul_rng_xorshift_next( vul_rng_xorshift_t *r )
{
	ui32_t t;
	t = r->x ^ ( r->x << 11 );
	r->x = r->y;
	r->y = r->z;
	r->z = r->w;
	return ( r->w = (r->w ^ (r->w >> 19)) ^ (t ^ (t >> 8)));
}
#endif

/**
 * Returns the next f32_t in the given vul_rng_xorshift state, and advances it.
 */
#ifndef VUL_DEFINE
f32_t vul_rng_xorshift_next( vul_rng_xorshift_t *r );
#else
f32_t vul_rng_xorshift_next( vul_rng_xorshift_t *r )
{
	return ( f32_t )( vul_rng_xorshift_next( r ) ) * ( 1.0f / 4294967296.0f );
}
#endif

//--------------------------------
// vul_rng_xorhash
//

/**
 * The vul_rng_xorhash RNG's state
 */
typedef struct {
	ui32_t s;
} vul_rng_xorhash_t;

/**
 * Create a new vul_rng_xorhash state with the given seed.
 * @NOTE: There are reports that it doesn't do well with seeds
 * that are multiples of 34.
 */
#ifndef VUL_DEFINE
vul_rng_xorhash_t *vul_rng_xorhash_create( ui32_t seed );
#else
vul_rng_xorhash_t *vul_rng_xorhash_create( ui32_t seed )
{
	vul_rng_xorhash_t *r = ( vul_rng_xorhash_t* )malloc( sizeof( vul_rng_xorhash_t ) );
	assert( r != NULL );

	seed = ( seed ^61 ) ^ ( seed >> 16 );
	seed = seed + ( seed < 9 );
	seed = seed ^ ( seed >> 4 );
	seed *= 0x27d4eb2d;
	seed = seed ^ ( seed >> 15 );
	r->s = seed;

	return r;
}
#endif

/**
 * Destroy a vul_rng_xorhash RNG state
 */
#ifndef VUL_DEFINE
void vul_rng_xorhash_destroy( vul_rng_xorhash_t *r );
#else
void vul_rng_xorhash_destroy( vul_rng_xorhash_t *r )
{
	assert( r != NULL );
	free( r );
}
#endif

/**
 * Returns the next ui32_t in the given vul_rng_xorhash state, and advances it.
 */
#ifndef VUL_DEFINE
ui32_t vul_rng_xorhash_next( vul_rng_xorhash_t *r );
#else
ui32_t vul_rng_xorhash_next( vul_rng_xorhash_t *r )
{
	r->s ^= ( r->s << 13 );
	r->s ^= ( r->s >> 17 );
	r->s ^= ( r->s << 5 );
	return r->s;
}
#endif

/**
 * Returns the next f32_t in the given vul_rng_xorhash state, and advances it.
 */
#ifndef VUL_DEFINE
f32_t vul_rng_xorhash_next( vul_rng_xorhash_t *r );
#else
f32_t vul_rng_xorhash_next( vul_rng_xorhash_t *r )
{
	return ( f32_t )( vul_rng_xorhash_next( r ) ) * ( 1.0f / 4294967296.0f );
}
#endif

#endif