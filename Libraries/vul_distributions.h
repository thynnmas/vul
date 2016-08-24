/*
 * Villains' Utility Library - Thomas Martin Schmid, 2016. Public domain?
 *
 * This file contains several number distribution functions.
 * -vul_halton: Halton series of a given base number. Distributes values 
 *				uniformely in the range [0, 1]
 * 
 * ? If public domain is not legally valid in your legal jurisdiction
 *   the MIT licence applies (see the LICENCE file)
 */
#ifndef VUL_DISTRIBUTIONS_H
#define VUL_DISTRIBUTIONS_H

#ifndef VUL_TYPES_H
#include <stdint.h>
#define f32 float
#define u32 uint32_t
#endif

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
typedef struct vul_halton {
	f32 value;
	f32 inv_base;
} vul_halton;

#ifdef _cplusplus
extern "C" {
#endif
/**
 * Create a vul_halton state.  Uses Thomas Wang's integer hash to 
 * initialize values from the seed.
 */
vul_halton *vul_halton_series_create( u32 base1, u32 seed );
/**
 * Destroy a vul_halton_pair state.
 */
void vul_halton_series_destroy( vul_halton *r );
/**
 * Advances the state of the halton series and return 
 * the next value.
 */
f32 vul_halton_series_next( vul_halton *rng );

#ifdef _cplusplus
}
#endif

#ifndef VUL_TYPES_H
#undef f32
#undef u32
#endif

#endif // VUL_DISTRIBUTIONS_H

#ifdef VUL_DEFINE

#ifndef VUL_TYPES_H
#define f32 float
#define u32 uint32_t
#endif

#ifdef _cplusplus
extern "C" {
#endif

vul_halton *vul_halton_series_create( u32 base1, u32 seed )
{
	vul_halton *r = ( vul_halton* )malloc( sizeof( vul_halton ) );
	assert( r != NULL );

	seed = (seed ^ 61) ^ (seed >> 16);
	seed *= 9;
	seed = seed ^ (seed >> 4);
	seed *= 0x27d4eb2d;
	seed = seed ^ (seed >> 15);
	r->value = ( f32 )seed / 4294967295.f;

	r->inv_base = 1.f / base1;

	return r;
}

void vul_halton_series_destroy( vul_halton *r )
{
	assert( r != NULL );
	free( r );
}

f32 vul_halton_series_next( vul_halton *rng )
{
	assert( rng != NULL );
	
	f32 r = 1.f - rng->value - 0.0000001f;
	if ( rng->inv_base < r ) {
		rng->value += rng->inv_base;
	} else {
		f32 h = rng->inv_base, h2;
		do{
			h2 = h; 
			h *= rng->inv_base;
		} while ( h >= r );
		rng->value += h2 + h - 1.f;
	}
	return rng->value;
}

#ifdef _cplusplus
}
#endif

#ifndef VUL_TYPES_H
#undef f32
#undef u32
#endif

#endif // VUL_DEFINE
