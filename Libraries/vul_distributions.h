/*
 * Villains' Utility Library - Thomas Martin Schmid, 2015. Public domain?
 *
 * This file contains several number distribution functions.
 * -vul_halton_t: Halton series of a given base number. Distributes values 
 *					uniformely in the range [0, 1]
 * 
 * ? If public domain is not legally valid in your legal jurisdiction
 *   the MIT licence applies (see the LICENCE file)
 */
#ifndef VUL_DISTRIBUTIONS_H
#define VUL_DISTRIBUTIONS_H

#include "vul_types.h"

#include <stdlib.h>
#ifndef VUL_OSX
#include <malloc.h>
#endif
#include <assert.h>

/**
 * If defined, the functions are defined and not just declared. Only do this in _one_ c/cpp file!
 */
//#define VUL_DEFINE

/**
 * State of a vul_halton_parir
 */
typedef struct {
	f32_t value;
	f32_t inv_base;
} vul_halton_t;

/**
 * Create a vul_halton_t state.  Uses Thomas Wang's integer hash to 
 * initialize values from the seed.
 */
#ifndef VUL_DEFINE
vul_halton_t *vul_halton_series_create( ui32_t base1, ui32_t seed );
#else
vul_halton_t *vul_halton_series_create( ui32_t base1, ui32_t seed )
{
	vul_halton_t *r = ( vul_halton_t* )malloc( sizeof( vul_halton_t ) );
	assert( r != NULL );

	seed = (seed ^ 61) ^ (seed >> 16);
	seed *= 9;
	seed = seed ^ (seed >> 4);
	seed *= 0x27d4eb2d;
	seed = seed ^ (seed >> 15);
	r->value = ( f32_t )seed / 4294967295.f;

	r->inv_base = 1.f / base1;

	return r;
}
#endif

/**
 * Destroy a vul_halton_pair state.
 */
#ifndef VUL_DEFINE
void vul_halton_series_destroy( vul_halton_t *r );
#else
void vul_halton_series_destroy( vul_halton_t *r )
{
	assert( r != NULL );
	free( r );
}
#endif

/**
 * Advances the state of the halton series and return 
 * the next value.
 */
#ifndef VUL_DEFINE
f32_t vul_halton_series_next( vul_halton_t *rng );
#else
f32_t vul_halton_series_next( vul_halton_t *rng )
{
	assert( rng != NULL );
	
	f32_t r = 1.f - rng->value - 0.0000001f;
	if ( rng->inv_base < r ) {
		rng->value += rng->inv_base;
	} else {
		f32_t h = rng->inv_base, h2;
		do{
			h2 = h; 
			h *= rng->inv_base;
		} while ( h >= r );
		rng->value += h2 + h - 1.f;
	}
	return rng->value;
}
#endif

#endif
