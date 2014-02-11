/*
 * Villains' Utility Library - Thomas Martin Schmid, 2014. Public domain�
 *
 * This defines normal type operators on __m128 and __m256 which allow us
 * to write SOA simd code just like scalar code with Vector< __m128, n >-style.
 * Depending on VUL_AOSOA_FLOAT_WIDTH we use 32 or 64 bit floats
 * 
 * � If public domain is not legally valid in your country and or legal area,
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
#ifndef VUL_AOSOA_HPP
#define VUL_AOSOA_HPP

#include <mmintrin.h>
#include <emmintrin.h>
#include <xmmintrin.h>

#include "vul_types.hpp"
#include "vul_vector.hpp"
#include "vul_aabb.hpp"

namespace vul {
	
	//----------------
	// Declarations
	//	
	/**
	 * Pack an array of Vector< f32_t, n > into an array of Vector< __m128, n >
	 * Expects in to be of size count + count % 4, and out to be of size (count + 3 ) / 4.
	 */
	template< ui32_t n >
	void pack( Vector< __m128, n > *out, const Vector< f32_t, n > *in, ui32_t count );
	/**
	 * Pack an array of Vector< f64_t, n > into an array of Vector< __m128d, n >
	 * Expects in to be of size count + count % 2, and out to be of size (count + 1 ) / 2.
	 */
	template< ui32_t n >
	void pack( Vector< __m128d, n > *out, const Vector< f64_t, n > *in, ui32_t count );
	/**
	 * Pack an array of Vector< f32_t, n > into an array of Vector< __m256, n >
	 * Expects in to be of size count + count % 8, and out to be of size (count + 7 ) / 8.
	 */
	template< ui32_t n >
	void pack( Vector< __m256, n > *out, const Vector< f32_t, n > *in, ui32_t count );
	/**
	 * Pack an array of Vector< f64_t, n > into an array of Vector< __m256d, n >
	 * Expects in to be of size count + count % 4, and out to be of size (count + 3 ) / 4.
	 */
	template< ui32_t n >
	void pack( Vector< __m256d, n > *out, const Vector< f64_t, n > *in, ui32_t count );
	
	/**
	 * Unpacks an array of Vector< __m128, n > into an array of Vector< f32_t, n >
	 * Expects out to be of size count + count % 4, and in to be of size (count + 3 ) / 4.
	 */
	template< ui32_t n >
	void unpack( Vector< f32_t, n > *out, const Vector< __m128, n > *in, ui32_t count );
	/**
	 * Pack an array of Vector< __m128d, n > into an array of Vector< f64_t, n >
	 * Expects out to be of size count + count % 2, and in to be of size (count + 1 ) / 2.
	 */
	template< ui32_t n >
	void unpack( Vector< f64_t, n > *out, const Vector< __m128d, n > *in, ui32_t count );
	/**
	 * Pack an array of Vector< __m256, n > into an array of Vector< f32_t, n >
	 * Expects out to be of size count + count % 8, and in to be of size (count + 7 ) / 8.
	 */
	template< ui32_t n >
	void unpack( Vector< f32_t, n > *out, const Vector< __m256, n > *in, ui32_t count );
	/**
	 * Pack an array of Vector< __m256d, n > into an array of Vector< f64_t, n >
	 * Expects out to be of size count + count % 4, and in to be of size (count + 3 ) / 4.
	 */
	template< ui32_t n >
	void unpack( Vector< f64_t, n > *out, const Vector< __m256d, n > *in, ui32_t count );
	
	/**
	 * Pack an array of AABB< f32_t, n > into an array of AABB< __m128, n >
	 * Expects in to be of size count + count % 4, and out to be of size (count + 3 ) / 4.
	 */
	template< ui32_t n >
	void pack( AABB< __m128, n > *out, const AABB< f32_t, n > *in, ui32_t count );
	/**
	 * Pack an array of AABB< f64_t, n > into an array of AABB< __m128d, n >
	 * Expects in to be of size count + count % 2, and out to be of size (count + 1 ) / 2.
	 */
	template< ui32_t n >
	void pack( AABB< __m128d, n > *out, const AABB< f64_t, n > *in, ui32_t count );
	/**
	 * Pack an array of AABB< f32_t, n > into an array of AABB< __m256, n >
	 * Expects in to be of size count + count % 8, and out to be of size (count + 7 ) / 8.
	 */
	template< ui32_t n >
	void pack( AABB< __m256, n > *out, const AABB< f32_t, n > *in, ui32_t count );
	/**
	 * Pack an array of AABB< f64_t, n > into an array of AABB< __m256d, n >
	 * Expects in to be of size count + count % 4, and out to be of size (count + 3 ) / 4.
	 */
	template< ui32_t n >
	void pack( AABB< __m256d, n > *out, const AABB< f64_t, n > *in, ui32_t count );
	
	/**
	 * Unpacks an array of AABB< __m128, n > into an array of AABB< f32_t, n >
	 * Expects out to be of size count + count % 4, and in to be of size (count + 3 ) / 4.
	 */
	template< ui32_t n >
	void unpack( AABB< f32_t, n > *out, const AABB< __m128, n > *in, ui32_t count );
	/**
	 * Pack an array of AABB< __m128d, n > into an array of AABB< f64_t, n >
	 * Expects out to be of size count + count % 2, and in to be of size (count + 1 ) / 2.
	 */
	template< ui32_t n >
	void unpack( AABB< f64_t, n > *out, const AABB< __m128d, n > *in, ui32_t count );
	/**
	 * Pack an array of AABB< __m256, n > into an array of AABB< f32_t, n >
	 * Expects out to be of size count + count % 8, and in to be of size (count + 7 ) / 8.
	 */
	template< ui32_t n >
	void unpack( AABB< f32_t, n > *out, const AABB< __m256, n > *in, ui32_t count );
	/**
	 * Pack an array of AABB< __m256d, n > into an array of AABB< f64_t, n >
	 * Expects out to be of size count + count % 4, and in to be of size (count + 3 ) / 4.
	 */
	template< ui32_t n >
	void unpack( AABB< f64_t, n > *out, const AABB< __m256d, n > *in, ui32_t count );
	//-------------------
	// Definitions
	//	
	template< ui32_t n >
	void pack( Vector< __m128, n > *out, const Vector< f32_t, n > *in, ui32_t count )
	{
		ui32_t simdCount, i, j;
		__m128 tmp[ n ];
		
		simdCount = ( count + 3 ) / 4;
		for( i = 0; i < simdCount; ++i )
		{
			for( j = 0; j < n; ++j )
			{
				tmp[ j ] = _mm_set_ps( in[ i * 4     ][ j ], 
									   in[ i * 4 + 1 ][ j ], 
									   in[ i * 4 + 2 ][ j ], 
									   in[ i * 4 + 3 ][ j ] );
			}
			out[ i ] = makeVector< __m128, n >( tmp );
		}
	}
	template< ui32_t n >
	void pack( Vector< __m128d, n > *out, const Vector< f64_t, n > *in, ui32_t count )
	{
		ui32_t simdCount, i, j;
		__m128d tmp[ n ];
		
		simdCount = ( count + 1 ) / 2;
		for( i = 0; i < simdCount; ++i )
		{
			for( j = 0; j < n; ++j )
			{
				tmp[ j ] = _mm_set_pD( in[ i * 2     ][ j ], 
									   in[ i * 2 + 1 ][ j ] );
			}
			out[ i ] = makeVector< __m128d, n >( tmp );
		}
	}
	template< ui32_t n >
	void pack( Vector< __m256, n > *out, const Vector< f32_t, n > *in, ui32_t count )
	{
		ui32_t simdCount, i, j;
		__m256 tmp[ n ];
		
		simdCount = ( count + 7 ) / 8;
		for( i = 0; i < simdCount; ++i )
		{
			for( j = 0; j < n; ++j )
			{
				tmp[ j ] = _mm_set_ps( in[ i * 8     ][ j ], 
									   in[ i * 8 + 1 ][ j ], 
									   in[ i * 8 + 2 ][ j ], 
									   in[ i * 8 + 3 ][ j ],
									   in[ i * 8 + 4 ][ j ], 
									   in[ i * 8 + 5 ][ j ], 
									   in[ i * 8 + 6 ][ j ], 
									   in[ i * 8 + 7 ][ j ] );
			}
			out[ i ] = makeVector< __m256, n >( tmp );
		}
	}
	template< ui32_t n >
	void pack( Vector< __m256d, n > *out, const Vector< f64_t, n > *in, ui32_t count )
	{
		ui32_t simdCount, i, j;
		__m256d tmp[ n ];
		
		simdCount = ( count + 3 ) / 4;
		for( i = 0; i < simdCount; ++i )
		{
			for( j = 0; j < n; ++j )
			{
				tmp[ j ] = _mm_set_ps( in[ i * 4     ][ j ], 
									   in[ i * 4 + 1 ][ j ], 
									   in[ i * 4 + 2 ][ j ], 
									   in[ i * 4 + 3 ][ j ] );
			}
			out[ i ] = makeVector< __m256d, n >( tmp );
		}
	}
	
	template< ui32_t n >
	void unpack( Vector< f32_t, n > *out, const Vector< __m128, n > *in, ui32_t count )
	{
		ui32_t simdCount, i, j;
		
		simdCount = ( count + 3 ) / 4;
		for( i = 0; i < simdCount; ++i )
		{
			for( j = 0; j < n; ++j )
			{
				out[ i * 4     ][ j ] = in[ i ][ j ].m128_f32[ 0 ];
				out[ i * 4 + 1 ][ j ] = in[ i ][ j ].m128_f32[ 1 ];
				out[ i * 4 + 2 ][ j ] = in[ i ][ j ].m128_f32[ 2 ];
				out[ i * 4 + 3 ][ j ] = in[ i ][ j ].m128_f32[ 3 ];
			}
		}
	}
	template< ui32_t n >
	void unpack( Vector< f64_t, n > *out, const Vector< __m128d, n > *in, ui32_t count )
	{
		ui32_t simdCount, i, j;
		
		simdCount = ( count + 1 ) / 2;
		for( i = 0; i < simdCount; ++i )
		{
			for( j = 0; j < n; ++j )
			{
				out[ i * 2     ][ j ] = in[ i ][ j ].m128d_f64[ 0 ];
				out[ i * 2 + 1 ][ j ] = in[ i ][ j ].m128d_f64[ 1 ];
			}
		}
	}
	template< ui32_t n >
	void unpack( Vector< f32_t, n > *out, const Vector< __m256, n > *in, ui32_t count )
	{		
		ui32_t simdCount, i, j;
		
		simdCount = ( count + 7 ) / 8;
		for( i = 0; i < simdCount; ++i )
		{
			for( j = 0; j < n; ++j )
			{
				out[ i * 8     ][ j ] = in[ i ][ j ].m256_f32[ 0 ];
				out[ i * 8 + 1 ][ j ] = in[ i ][ j ].m256_f32[ 1 ];
				out[ i * 8 + 2 ][ j ] = in[ i ][ j ].m256_f32[ 2 ];
				out[ i * 8 + 3 ][ j ] = in[ i ][ j ].m256_f32[ 3 ];
				out[ i * 8 + 4 ][ j ] = in[ i ][ j ].m256_f32[ 4 ];
				out[ i * 8 + 5 ][ j ] = in[ i ][ j ].m256_f32[ 5 ];
				out[ i * 8 + 6 ][ j ] = in[ i ][ j ].m256_f32[ 6 ];
				out[ i * 8 + 7 ][ j ] = in[ i ][ j ].m256_f32[ 7 ];
			}
		}
	}
	template< ui32_t n >
	void unpack( Vector< f64_t, n > *out, const Vector< __m256d, n > *in, ui32_t count )
	{		
		ui32_t simdCount, i, j;
		
		simdCount = ( count + 3 ) / 4;
		for( i = 0; i < simdCount; ++i )
		{
			for( j = 0; j < n; ++j )
			{
				out[ i * 4     ][ j ] = in[ i ][ j ].m256d_f64[ 0 ];
				out[ i * 4 + 1 ][ j ] = in[ i ][ j ].m256d_f64[ 1 ];
				out[ i * 4 + 2 ][ j ] = in[ i ][ j ].m256d_f64[ 2 ];
				out[ i * 4 + 3 ][ j ] = in[ i ][ j ].m256d_f64[ 3 ];
			}
		}
	}
	template< ui32_t n >
	void pack( AABB< __m128, n > *out, const AABB< f32_t, n > *in, ui32_t count )
	{
		ui32_t simdCount, i, j;
		 __m128 mini[ n ], maxi[ n ];
		 Point< __m128, n > _CRT_ALIGN(16) vmin;
		 Point< __m128, n > _CRT_ALIGN(16) vmax;
		
		simdCount = ( count + 3 ) / 4;
		for( i = 0; i < simdCount; ++i )
		{
			for( j = 0; j < n; ++j )
			{
				mini[ j ] = _mm_set_ps( in[ i * 4     ]._min[ j ], 
									    in[ i * 4 + 1 ]._min[ j ], 
									    in[ i * 4 + 2 ]._min[ j ], 
									    in[ i * 4 + 3 ]._min[ j ] );
				maxi[ j ] = _mm_set_ps( in[ i * 4     ]._max[ j ], 
									    in[ i * 4 + 1 ]._max[ j ], 
									    in[ i * 4 + 2 ]._max[ j ], 
									    in[ i * 4 + 3 ]._max[ j ] );
			}
			vmin = makePoint< __m128, n >( mini );
			vmax = makePoint< __m128, n >( maxi );
			out[ i ] = makeAABB< __m128, n >( vmin, vmax );
		}
	}
	template< ui32_t n >
	void pack( AABB< __m128d, n > *out, const AABB< f64_t, n > *in, ui32_t count )
	{
		ui32_t simdCount, i, j;
		__m128d mini[ n ], maxi[ n ];
		 Point< __m128d, n > _CRT_ALIGN(16) vmin;
		 Point< __m128d, n > _CRT_ALIGN(16) vmax;
		
		simdCount = ( count + 1 ) / 2;
		for( i = 0; i < simdCount; ++i )
		{
			for( j = 0; j < n; ++j )
			{
				mini[ j ] = _mm_set_pd( in[ i * 2     ]._min[ j ], 
									    in[ i * 2 + 1 ]._min[ j ] );
				maxi[ j ] = _mm_set_pd( in[ i * 2     ]._max[ j ], 
									    in[ i * 2 + 1 ]._max[ j ] );
			}
			vmin = makePoint< __m128d, n >( mini );
			vmax = makePoint< __m128d, n >( maxi );
			out[ i ] = makeAABB< __m128d, n >( vmin, vmax );
		}
	}
	template< ui32_t n >
	void pack( AABB< __m256, n > *out, const AABB< f32_t, n > *in, ui32_t count )
	{
		ui32_t simdCount, i, j;
		__m256 mini[ n ], maxi[ n ];
		 Point< __m256, n > _CRT_ALIGN(32) vmin;
		 Point< __m256, n > _CRT_ALIGN(32) vmax;
		
		simdCount = ( count + 7 ) / 8;
		for( i = 0; i < simdCount; ++i )
		{
			for( j = 0; j < n; ++j )
			{
				mini[ j ] = _mm256_set_ps( in[ i * 8     ]._min[ j ], 
										   in[ i * 8 + 1 ]._min[ j ], 
									       in[ i * 8 + 2 ]._min[ j ], 
									       in[ i * 8 + 3 ]._min[ j ],
										   in[ i * 8 + 4 ]._min[ j ], 
									       in[ i * 8 + 5 ]._min[ j ], 
									       in[ i * 8 + 6 ]._min[ j ], 
									       in[ i * 8 + 7 ]._min[ j ] );
				maxi[ j ] = _mm256_set_ps( in[ i * 8     ]._max[ j ], 
									       in[ i * 8 + 1 ]._max[ j ], 
									       in[ i * 8 + 2 ]._max[ j ], 
									       in[ i * 8 + 3 ]._max[ j ],
										   in[ i * 8 + 4 ]._max[ j ], 
									       in[ i * 8 + 5 ]._max[ j ], 
									       in[ i * 8 + 6 ]._max[ j ], 
									       in[ i * 8 + 7 ]._max[ j ] );
			}
			vmin = makePoint< __m256, n >( mini );
			vmax = makePoint< __m256, n >( maxi );
			out[ i ] = makeAABB< __m256, n >( vmin, vmax );
		}
	}
	template< ui32_t n >
	void pack( AABB< __m256d, n > *out, const AABB< f64_t, n > *in, ui32_t count )
	{
		ui32_t simdCount, i, j;
		__m256d mini[ n ], maxi[ n ];
		 Point< __m256d, n > _CRT_ALIGN(32) vmin;
		 Point< __m256d, n > _CRT_ALIGN(32) vmax;
		
		simdCount = ( count + 3 ) / 4;
		for( i = 0; i < simdCount; ++i )
		{
			for( j = 0; j < n; ++j )
			{
				mini[ j ] = _mm256_set_pd( in[ i * 4     ]._min[ j ], 
										   in[ i * 4 + 1 ]._min[ j ], 
									       in[ i * 4 + 2 ]._min[ j ], 
									       in[ i * 4 + 3 ]._min[ j ] );
				maxi[ j ] = _mm256_set_pd( in[ i * 4     ]._max[ j ], 
									       in[ i * 4 + 1 ]._max[ j ], 
									       in[ i * 4 + 2 ]._max[ j ], 
									       in[ i * 4 + 3 ]._max[ j ] );
			}
			vmin = makePoint< __m256d, n >( mini );
			vmax = makePoint< __m256d, n >( maxi );
			out[ i ] = makeAABB< __m256d, n >( vmin, vmax );
		}
	}
	template< ui32_t n >
	void unpack( AABB< f32_t, n > *out, const AABB< __m128, n > *in, ui32_t count )
	{
		ui32_t simdCount, i, j;
		
		simdCount = ( count + 3 ) / 4;
		for( i = 0; i < simdCount; ++i )
		{
			for( j = 0; j < n; ++j )
			{
				out[ i * 4     ]._min[ j ] = in[ i ]._min[ j ].m128_f32[ 3 ];
				out[ i * 4 + 1 ]._min[ j ] = in[ i ]._min[ j ].m128_f32[ 2 ];
				out[ i * 4 + 2 ]._min[ j ] = in[ i ]._min[ j ].m128_f32[ 1 ];
				out[ i * 4 + 3 ]._min[ j ] = in[ i ]._min[ j ].m128_f32[ 0 ];
				out[ i * 4     ]._max[ j ] = in[ i ]._max[ j ].m128_f32[ 3 ];
				out[ i * 4 + 1 ]._max[ j ] = in[ i ]._max[ j ].m128_f32[ 2 ];
				out[ i * 4 + 2 ]._max[ j ] = in[ i ]._max[ j ].m128_f32[ 1 ];
				out[ i * 4 + 3 ]._max[ j ] = in[ i ]._max[ j ].m128_f32[ 0 ];
			}
		}
	}
	template< ui32_t n >
	void unpack( AABB< f64_t, n > *out, const AABB< __m128d, n > *in, ui32_t count )
	{
		ui32_t simdCount, i, j;
		
		simdCount = ( count + 1 ) / 2;
		for( i = 0; i < simdCount; ++i )
		{
			for( j = 0; j < n; ++j )
			{
				out[ i * 2     ]._min[ j ] = in[ i ]._min[ j ].m128_f64[ 1 ];
				out[ i * 2 + 1 ]._min[ j ] = in[ i ]._min[ j ].m128_f64[ 0 ];
				out[ i * 2     ]._max[ j ] = in[ i ]._max[ j ].m128_f64[ 1 ];
				out[ i * 2 + 1 ]._max[ j ] = in[ i ]._max[ j ].m128_f64[ 0 ];
			}
		}
	}
	template< ui32_t n >
	void unpack( AABB< f32_t, n > *out, const AABB< __m256, n > *in, ui32_t count )
	{
		ui32_t simdCount, i, j;
		
		simdCount = ( count + 7 ) / 8;
		for( i = 0; i < simdCount; ++i )
		{
			for( j = 0; j < n; ++j )
			{
				out[ i * 8     ]._min[ j ] = in[ i ]._min[ j ].m256_f32[ 7 ];
				out[ i * 8 + 1 ]._min[ j ] = in[ i ]._min[ j ].m256_f32[ 6 ];
				out[ i * 8 + 2 ]._min[ j ] = in[ i ]._min[ j ].m256_f32[ 5 ];
				out[ i * 8 + 3 ]._min[ j ] = in[ i ]._min[ j ].m256_f32[ 4 ];
				out[ i * 8 + 4 ]._min[ j ] = in[ i ]._min[ j ].m256_f32[ 3 ];
				out[ i * 8 + 5 ]._min[ j ] = in[ i ]._min[ j ].m256_f32[ 2 ];
				out[ i * 8 + 6 ]._min[ j ] = in[ i ]._min[ j ].m256_f32[ 1 ];
				out[ i * 8 + 7 ]._min[ j ] = in[ i ]._min[ j ].m256_f32[ 0 ];
				out[ i * 8     ]._max[ j ] = in[ i ]._max[ j ].m256_f32[ 7 ];
				out[ i * 8 + 1 ]._max[ j ] = in[ i ]._max[ j ].m256_f32[ 6 ];
				out[ i * 8 + 2 ]._max[ j ] = in[ i ]._max[ j ].m256_f32[ 5 ];
				out[ i * 8 + 3 ]._max[ j ] = in[ i ]._max[ j ].m256_f32[ 4 ];
				out[ i * 8 + 4 ]._max[ j ] = in[ i ]._max[ j ].m256_f32[ 3 ];
				out[ i * 8 + 5 ]._max[ j ] = in[ i ]._max[ j ].m256_f32[ 2 ];
				out[ i * 8 + 6 ]._max[ j ] = in[ i ]._max[ j ].m256_f32[ 1 ];
				out[ i * 8 + 7 ]._max[ j ] = in[ i ]._max[ j ].m256_f32[ 0 ];
			}
		}
	}
	template< ui32_t n >
	void unpack( AABB< f64_t, n > *out, const AABB< __m256d, n > *in, ui32_t count )
	{
		ui32_t simdCount, i, j;
		
		simdCount = ( count + 3 ) / 4;
		for( i = 0; i < simdCount; ++i )
		{
			for( j = 0; j < n; ++j )
			{
				out[ i * 4     ]._min[ j ] = in[ i ]._min[ j ].m256_f64[ 3 ];
				out[ i * 4 + 1 ]._min[ j ] = in[ i ]._min[ j ].m256_f64[ 2 ];
				out[ i * 4 + 2 ]._min[ j ] = in[ i ]._min[ j ].m256_f64[ 1 ];
				out[ i * 4 + 3 ]._min[ j ] = in[ i ]._min[ j ].m256_f64[ 0 ];
				out[ i * 4     ]._max[ j ] = in[ i ]._max[ j ].m256_f64[ 3 ];
				out[ i * 4 + 1 ]._max[ j ] = in[ i ]._max[ j ].m256_f64[ 2 ];
				out[ i * 4 + 2 ]._max[ j ] = in[ i ]._max[ j ].m256_f64[ 1 ];
				out[ i * 4 + 3 ]._max[ j ] = in[ i ]._max[ j ].m256_f64[ 0 ];
			}
		}
	}
}

#endif