/*
 * Villains' Utility Library - Thomas Martin Schmid, 2017. Public domain?
 *
 * This file contains some noise functions
 *
 * ? If public domain is not legally valid in your legal jurisdiction
 *   the MIT licence applies (see the LICENCE file)
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#ifndef VUL_NOISE_H
#define VUL_NOISE_H

#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#define VUL_NOISE_RNG_LCG
#define VUL_NOISE_WORLEY_TILE_SIZE 256

#ifdef __cplusplus
extern "C" {
#endif

static float *noise_tile;

#ifdef VUL_NOISE_RNG_LCG
static float vul__noise_rng( );
#endif
void vul_noise_gaussian( float *out, int size );

static void vul__noise_worley_downsample( float *from, float *to, int n, int stride );
static void vul__noise_worley_upsample( float *from, float *to, int n, int stride );
static int vul__noise_worley_mod( int x, int n );

static void vul__noise_worley_generate_tile_1d( float *tiles, int n );
static void vul__noise_worley_generate_tile_2d( float *tiles, int n );
static void vul__noise_worley_generate_tile_3d( float *tiles, int n );

static float vul__noise_worley_noise_1d( float x );
static float vul__noise_worley_noise_2d( float x, float y );
static float vul__noise_worley_noise_3d( float x, float y, float z );

float vul_noise_worley_1d( float x, int bands, float *weights );
float vul_noise_worley_2d( float x, float y, int bands, float *weights );
float vul_noise_worley_3d( float x, float y, float z, int bands, float *weights );

#ifdef __cplusplus
}
#endif
#endif

#ifdef VUL_DEFINE

#ifdef __cplusplus
extern "C" {
#endif

#ifdef VUL_NOISE_RNG_LCG
static float vul__noise_rng( )
{
	return rand( ) / ( float )RAND_MAX;
}
#endif

void vul_noise_gaussian( float *out, int size )
{
	int i;
	for( i = 0; i < size; ++i ) {
		out[ i ] = vul__noise_rng( ) * 2.f - 1.f;
	}
}

static int vul__noise_worley_mod( int x, int n )
{
	int m = x % n;
	return ( m < 0 ) ? m + n : m;
}
static void vul__noise_worley_downsample( float *from, float *to, int n, int stride )
{
	int i, k;
	float *coeffs, coeff_array[ 32 ] = {
		0.000334,-0.001528, 0.000410, 0.003545,-0.000938,-0.008233, 0.002172, 0.019120,
		-0.005040,-0.044412, 0.011655, 0.103311,-0.025936,-0.243780, 0.033979, 0.655340,
		0.655340, 0.033979,-0.243780,-0.025936, 0.103311, 0.011655,-0.044412,-0.005040,
		0.019120, 0.002172,-0.008233,-0.000938, 0.003546, 0.000410,-0.001528, 0.000334
	};
	coeffs = &coeff_array[ 16 ];

	for( i = 0; i < n / 2; ++i ) {
		to[ i * stride ] = 0.f;
		for( k = 2 * i - 16; k <= 2 * i + 15; ++k ) {
			// @BUG(thynn): We have a read past the coefficient matrix here leading to a NaN!
			to[ i * stride ] += coeffs[ k - 2 * i ] * from[ vul__noise_worley_mod( k, n ) * stride ];
		}
	}
}
static void vul__noise_worley_upsample( float *from, float *to, int n, int stride )
{
	int i, k;
	float *coeffs, coeff_array[ 4 ] = {
		0.25, 0.75, 0.75, 0.25
	};
	coeffs = &coeff_array[ 2 ];
	for( i = 0; i < n; ++i ) {
		to[ i * stride ] = 0.f;
		for( k = i / 2; k <= i / 2 + 1; ++k ) {
			// @BUG(thynn): Wemight have a read past the coefficient matrix here?
			to[ i * stride ] += coeffs[ i - 2 * k ] * from[ vul__noise_worley_mod( k, n / 2 ) * stride ];
		}
	}
}

static void vul__noise_worley_generate_tile_1d( float *tiles, int n )
{
	float *temp1, *temp2, *noise;
	int x, y, z, i, size;
	size = n * sizeof( float );
	temp1 = ( float* )malloc( size );
	temp2 = ( float* )malloc( size );
	if( n % 2 ) {
		++n;
	}
	// Fill with random numbers in [-1, 1]
	vul_noise_gaussian( tiles, n );
	// Separable down- and up-sample
	vul__noise_worley_downsample( tiles, temp1, n, 1 );
	vul__noise_worley_upsample( temp1, temp2, n, 1 );
	// Subtract out coarse-scale contribution
	for( i = 0; i < n; ++i ) {
		tiles[ i ] -= temp2[ i ];
	}
	// Avoid even/odd variance difference (add odd-offset version of noise to itself)
	int offset = n / 2;
	if( offset % 2 == 0 ) {
		++offset;
	}
	for( i = 0; i < n; ++i ) {
		temp1[ i ] = tiles[ vul__noise_worley_mod( i + offset, n ) ];
	}
	for( i = 0; i < n; ++i ) {
		tiles[ i ] += temp1[ i ];
	}
	free( temp1 );
	free( temp2 );
}
static void vul__noise_worley_generate_tile_2d( float *tiles, int n )
{
	float *temp1, *temp2, *noise;
	int x, y, z, i, size;
	size = n * n * sizeof( float );
	temp1 = ( float* )malloc( size );
	temp2 = ( float* )malloc( size );
	if( n % 2 ) {
		++n;
	}
	// Fill with random numbers in [-1, 1]
	vul_noise_gaussian( tiles, n * n );
	// Separable down- and up-sample
	for( y = 0; y < n; ++y ) {
		i = y * n;
		vul__noise_worley_downsample( &tiles[i], &temp1[i], n, 1 );
		vul__noise_worley_upsample( &temp1[i], &temp2[i], n, 1 );
	}
	for( x = 0; x < n; ++x ) {
		i = x;
		vul__noise_worley_downsample( &temp2[i], &temp1[i], n, n );
		vul__noise_worley_upsample( &temp1[i], &temp2[i], n, n );
	}
	// Subtract out coarse-scale contribution
	for( i = 0; i < n * n; ++i ) {
		tiles[ i ] -= temp2[ i ];
	}
	// Avoid even/odd variance difference (add odd-offset version of noise to itself)
	int offset = n / 2;
	if( offset % 2 == 0 ) {
		++offset;
	}
	for( i = 0, x = 0; x < n; ++x ) {
		for( y = 0; y < n; ++y ) {
			temp1[ i++ ] = tiles[ vul__noise_worley_mod( x + offset, n )
									  + vul__noise_worley_mod( y + offset, n ) * n ];
		}
	}
	for( i = 0; i < n * n; ++i ) {
		tiles[ i ] += temp1[ i ];
	}
	free( temp1 );
	free( temp2 );
}

static void vul__noise_worley_generate_tile_3d( float *tiles, int n )
{
	float *temp1, *temp2, *noise;
	int x, y, z, i, size;
	size = n * n * n * sizeof( float );
	temp1 = ( float* )malloc( size );
	temp2 = ( float* )malloc( size );
	if( n % 2 ) {
		++n;
	}
	// Fill with random numbers in [-1, 1]
	vul_noise_gaussian( tiles, n * n * n );
	// Separable down- and up-sample
	for( y = 0; y < n; ++y ) {
		for( z = 0; z < n; ++z ) {
			i = z * n * n + y * n;
			vul__noise_worley_downsample( &tiles[i], &temp1[i], n, 1 );
			vul__noise_worley_upsample( &temp1[i], &temp2[i], n, 1 );
		}
	}
	for( x = 0; x < n; ++x ) {
		for( z = 0; z < n; ++z ) {
			i = z * n * n + x;
			vul__noise_worley_downsample( &temp2[i], &temp1[i], n, n );
			vul__noise_worley_upsample( &temp1[i], &temp2[i], n, n );
		}
	}
	for( x = 0; x < n; ++x ) {
		for( y = 0; y < n; ++y ) {
			i = y * n + x;
			vul__noise_worley_downsample( &temp2[i], &temp1[i], n, n * n );
			vul__noise_worley_upsample( &temp1[i], &temp2[i], n, n * n );
		}
	}
	// Subtract out coarse-scale contribution
	for( i = 0; i < n * n * n; ++i ) {
		tiles[ i ] -= temp2[ i ];
	}
	// Avoid even/odd variance difference (add odd-offset version of noise to itself)
	int offset = n / 2;
	if( offset % 2 == 0 ) {
		++offset;
	}
	for( i = 0, x = 0; x < n; ++x ) {
		for( y = 0; y < n; ++y ) {
			for( z = 0; z < n; ++z ) {
				temp1[ i++ ] = tiles[ vul__noise_worley_mod( x + offset, n )
										  + vul__noise_worley_mod( y + offset, n ) * n
										  + vul__noise_worley_mod( z + offset, n ) * n * n ];
			}
		}
	}
	for( i = 0; i < n * n * n; ++i ) {
		tiles[ i ] += temp1[ i ];
	}
	free( temp1 );
	free( temp2 );
}

static float vul__noise_worley_noise_1d( float x )
{
	int i, f, c, mid, n = VUL_NOISE_WORLEY_TILE_SIZE;
	float w[ 3 ], t, result = 0.f;

	if( !noise_tile ) {
		noise_tile = ( float* )malloc( sizeof( float ) * n );
		vul__noise_worley_generate_tile_1d( noise_tile, n );
	}

	mid = ceil( x - 0.5 );
	t = mid - ( x - 0.5 );
	w[ 0 ] = t * t / 2.f;
	w[ 2 ] = ( 1.f - t ) * ( 1.f - t ) / 2.f;
	w[ 1 ] = 1.f - w[ 0 ] - w[ 2 ];
	for( f = -1; f <= 1; f++ ) {
		float weight = 1.f;
		c = vul__noise_worley_mod( mid + f, n );
		weight *= w[ f + 1 ];
		result += weight * noise_tile[ c ];
	}
	return result;
}
static float vul__noise_worley_noise_2d( float x, float y )
{
	int i, f[ 2 ], c[ 2 ], mid[ 2 ], n = VUL_NOISE_WORLEY_TILE_SIZE;
	float w[ 2 ][ 3 ], p[ 2 ], t, result = 0.f;

	if( !noise_tile ) {
		noise_tile = ( float* )malloc( sizeof( float ) * n * n );
		vul__noise_worley_generate_tile_2d( noise_tile, n );
	}

	p[ 0 ] = x;
	p[ 1 ] = y;
	for( i = 0; i < 2; ++i ) {
		mid[ i ] = ceil( p[ i ] - 0.5 );
		t = mid[ i ] - ( p[ i ] - 0.5 );
		w[ i ][ 0 ] = t * t / 2.f;
		w[ i ][ 2 ] = ( 1.f - t ) * ( 1.f - t ) / 2.f;
		w[ i ][ 1 ] = 1.f - w[ i ][ 0 ] - w[ i ][ 2 ];
	}
	for( f[ 1 ] = -1; f[ 1 ] <= 1; f[ 1 ]++ ) {
		for( f[ 0 ] = -1; f[ 0 ] <= 1; f[ 0 ]++ ) {
			float weight = 1.f;
			for( i = 0; i < 2; ++i ) {
				c[ i ] = vul__noise_worley_mod( mid[ i ] + f[ i ], n );
				weight *= w[ i ][ f[ i ] + 1 ];
			}
			result += weight * noise_tile[ c[ 1 ] * n + c[ 0 ] ];
		}
	}
	return result;
}
static float vul__noise_worley_noise_3d( float x, float y, float z )
{
	int i, f[ 3 ], c[ 3 ], mid[ 3 ], n = VUL_NOISE_WORLEY_TILE_SIZE;
	float w[ 3 ][ 3 ], p[ 3 ], t, result = 0.f;

	if( !noise_tile ) {
		noise_tile = ( float* )malloc( sizeof( float ) * n * n * n );
		vul__noise_worley_generate_tile_3d( noise_tile, n );
	}

	p[ 0 ] = x;
	p[ 1 ] = y;
	p[ 2 ] = z;
	for( i = 0; i < 3; ++i ) {
		mid[ i ] = ceil( p[ i ] - 0.5 );
		t = mid[ i ] - ( p[ i ] - 0.5 );
		w[ i ][ 0 ] = t * t / 2.f;
		w[ i ][ 2 ] = ( 1.f - t ) * ( 1.f - t ) / 2.f;
		w[ i ][ 1 ] = 1.f - w[ i ][ 0 ] - w[ i ][ 2 ];
	}
	for( f[ 2 ] = -1; f[ 2 ] <= 1; f[ 2 ]++ ) {
		for( f[ 1 ] = -1; f[ 1 ] <= 1; f[ 1 ]++ ) {
			for( f[ 0 ] = -1; f[ 0 ] <= 1; f[ 0 ]++ ) {
				float weight = 1.f;
				for( i = 0; i < 3; ++i ) {
					c[ i ] = vul__noise_worley_mod( mid[ i ] + f[ i ], n );
					weight *= w[ i ][ f[ i ] + 1 ];
				}
				result += weight * noise_tile[ c[ 2 ] * n * n + c[ 1 ] * n + c[ 0 ] ];
			}
		}
	}
	return result;
}

float vul_noise_worley_1d( float x, int bands, float *weights )
{
	float q, result, variance;
	int i, b;
	result = 0.f;
	for( b = 0; b < bands; ++b ) {
		q = 2.f * x * powf( 2.f, b );
		result += weights[ b ] * vul__noise_worley_noise_1d( q );
	}
	variance = 0.f;
	for( b = 0; b < bands; ++b ) {
		variance += weights[ b ] * weights[ b ];
	}
	if( variance ) {
		result /= sqrtf( variance * 0.265 );
	}
	return result;
}
float vul_noise_worley_2d( float x, float y, int bands, float *weights )
{
	float qx, qy, result, variance;
	int i, b;
	result = 0.f;
	for( b = 0; b < bands; ++b ) {
		qx = 2.f * x * powf( 2.f, b );
		qy = 2.f * y * powf( 2.f, b );
		result += weights[ b ] * vul__noise_worley_noise_2d( qx, qy );
	}
	variance = 0.f;
	for( b = 0; b < bands; ++b ) {
		variance += weights[ b ] * weights[ b ];
	}
	if( variance ) {
		result /= sqrtf( variance * 0.265 );
	}
	return result;
}
float vul_noise_worley_3d( float x, float y, float z, int bands, float *weights )
{
	float qx, qy, qz, result, variance;
	int i, b;
	result = 0.f;
	for( b = 0; b < bands; ++b ) {
		qx = 2.f * x * powf( 2.f, b );
		qy = 2.f * y * powf( 2.f, b );
		qz = 2.f * z * powf( 2.f, b );
		result += weights[ b ] * vul__noise_worley_noise_3d( qx, qy, qz );
	}
	variance = 0.f;
	for( b = 0; b < bands; ++b ) {
		variance += weights[ b ] * weights[ b ];
	}
	if( variance ) {
		result /= sqrtf( variance * 0.265 );
	}
	return result;
}

#ifdef __cplusplus
}
#endif

#endif
