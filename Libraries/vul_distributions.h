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
#ifndef VUL_DISTRIBUTIONS_H
#define VUL_DISTRIBUTIONS_H

#include "vul_types.h"

#include <stdlib.h>
#include <malloc.h>
#include <assert.h>

/**
 * If defined, the functions are defined and not just declared. Only do this in _one_ c/cpp file!
 */
#define VUL_DEFINE

/**
 * State of a vul_halton_parir
 */
typedef struct {
	f32_t value_a, value_b;
	f32_t inv_base_a, inv_base_b;
} vul_halton_pair_t;

/**
 * Create a vul_halton_pair state.  Uses Thomas Wang's integer hash to 
* initialize values from the seed.
*/
#ifndef VUL_DEFINE
vul_halton_pair_t *vul_halton_pair_create( ui32_t base1, ui32_t base2, ui32_t seed );
#else
vul_halton_pair_t *vul_halton_pair_create( ui32_t base1, ui32_t base2, ui32_t seed )
{
	vul_halton_pair_t *r = ( vul_halton_pair_t* )malloc( sizeof( vul_halton_pair_t ) );
	assert( r != NULL );

	seed = (seed ^ 61) ^ (seed >> 16);
	seed *= 9;
	seed = seed ^ (seed >> 4);
	seed *= 0x27d4eb2d;
	seed = seed ^ (seed >> 15);

	r->value_a = seed;

	seed ^= ( seed << 13 );
	seed ^= ( seed >> 17 );
	seed ^= ( seed << 5 );

	r->value_b = seed;
	r->inv_base_a = 1.f / base1;
	r->inv_base_b = 1.f / base2;

	return r;
}
#endif

/**
 * Destroy a vul_halton_pair state.
 */
#ifndef VUL_DEFINE
void vul_halton_pair_destroy( vul_halton_pair_t *r );
#else
void vul_halton_pair_destroy( vul_halton_pair_t *r )
{
	assert( r != NULL );
	free( r );
}
#endif

/**
 * Advances the state of the halton pair values.
 */
#ifndef VUL_DEFINE
void vul_halton_pair_next( vul_halton_pair_t *rng );
#else
void vul_halton_pair_next( vul_halton_pair_t *rng )
{
	assert( rng != NULL );
	
	f32_t r = 1.0 - rng->value_a - 0.0000001;
	if ( rng->inv_base_a < r ) {
		rng->value_a += rng->inv_base_a;
	} else {
		f32_t h = rng->inv_base_a, h2;
		do{
			h2 = h; 
			h *= rng->inv_base_a;
		} while ( h >= r );
		rng->value_a += h2 + h - 1.0;
	}
	r = 1.0 - rng->value_a - 0.0000001;
	if ( rng->inv_base_a < r ) {
		rng->value_b += rng->inv_base_a;
	} else {
		f32_t h = rng->inv_base_a, h2;
		do{
			h2 = h; 
			h *= rng->inv_base_a;
		} while ( h >= r );
		rng->value_b += h2 + h - 1.0;
	}
}
#endif

/**
 * Fetch the current values of the halton pair.
 */
#ifndef VUL_DEFINE
void vul_halton_pair_fetch( vul_halton_pair_t *rng, f32_t *v1, f32_t *v2 );
#else
void vul_halton_pair_fetch( vul_halton_pair_t *rng, f32_t *v1, f32_t *v2 )
{
	*v1 = rng->value_a;
	*v2 = rng->value_b;
}
#endif

#endif