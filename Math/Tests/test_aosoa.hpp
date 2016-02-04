/*
 * Villains' Utility Library - Thomas Martin Schmid, 2016. Public domain¹
 *
 * This file contains tests for the AOSOA packing/unpacking functions in vul_aosoa.hpp
 * 
 * ¹ If public domain is not legally valid in your legal jurisdiction
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
#ifndef VUL_TEST_AOSOA_HPP
#define VUL_TEST_AOSOA_HPP

#include <cassert>

#include "../vul_math.hpp"

#pragma warning(disable: 6001)

#define VUL_TEST_RNG ( float )( ( float )rand( ) / ( float )RAND_MAX )

using namespace vul;

namespace vul_test {
	class TestAOSOA {

	public:
		static bool test( );

	private:
#ifdef VUL_AOSOA_SSE
		static bool vectors_sse( );
		static bool aabbs_sse( );
#endif
#ifdef VUL_AOSOA_AVX
		static bool vectors_avx( );
		static bool aabbs_avx( );
#endif
#ifdef VUL_AOSOA_NEON
		static bool vectors_neon( );	
		static bool aabbs_neon( );
#endif
	};

	bool TestAOSOA::test( )
	{
#ifdef VUL_AOSOA_SSE
		assert( vectors_sse( ) );
		assert( aabbs_sse( ) );
#endif
#ifdef VUL_AOSOA_AVX
		assert( vectors_avx( ) );
		assert( aabbs_avx( ) );
#endif
#ifdef VUL_AOSOA_NEON
		assert( vectors_neon( ) );
		assert( aabbs_neon( ) );
#endif

		return true;
	}
	
#ifdef VUL_AOSOA_SSE
	bool TestAOSOA::vectors_sse( )
	{	
		Vector< f32, 9 > v32[ 16 ], o32[ 16 ];
		Vector< __m128, 9 > p32_4[ 4 ];
		Vector< f64, 9 > v64[ 16 ], o64[ 16 ];
		Vector< __m128d, 9 > p64_2[ 8 ];
		
		for( u32 i = 0; i < 16; ++i ) {
			f32 vec[ 9 ] = { ( f32 )VUL_TEST_RNG, ( f32 )VUL_TEST_RNG, ( f32 )VUL_TEST_RNG, 
							   ( f32 )VUL_TEST_RNG, ( f32 )VUL_TEST_RNG, ( f32 )VUL_TEST_RNG, 
							   ( f32 )VUL_TEST_RNG, ( f32 )VUL_TEST_RNG, ( f32 )VUL_TEST_RNG };
#ifdef VUL_CPLUSPLUS11
			v32[ i ] = Vector< f32, 9 >( vec );
			v64[ i ] = Vector< f64, 9 >( vec );
#else
			v32[ i ] = makeVector< f32, 9 >( vec );
			v64[ i ] = makeVector< f64, 9 >( vec );
#endif
		}
		
		pack< 9 >( p32_4, v32, 16 );
		pack< 9 >( p64_2, v64, 16 );

		for( u32 j = 0; j < 9; ++j ) {
			for( u32 i = 0; i < 8; ++i ) {
#ifdef __GNUC__
				f64 arr[ 2 ];
				_mm_store_pd( arr, p64_2[ i ][ j ] );
				assert( arr[ 1 ] == v64[ i * 2     ][ j ] );
				assert( arr[ 0 ] == v64[ i * 2 + 1 ][ j ] );
#else
				assert( p64_2[ i ][ j ].m128d_f64[ 1 ] == v64[ i * 2     ][ j ] );
				assert( p64_2[ i ][ j ].m128d_f64[ 0 ] == v64[ i * 2 + 1 ][ j ] );
#endif
			}
			for( u32 i = 0; i < 4; ++i ) {
#ifdef __GNUC__
				f32 arr[ 4 ];
				_mm_store_ps( arr, p32_4[ i ][ j ] );
				assert( arr[ 3 ] == v32[ i * 4     ][ j ] );
				assert( arr[ 2 ] == v32[ i * 4 + 1 ][ j ] );
				assert( arr[ 1 ] == v32[ i * 4 + 2 ][ j ] );
				assert( arr[ 0 ] == v32[ i * 4 + 3 ][ j ] );
#else
				assert( p32_4[ i ][ j ].m128_f32[ 3 ] == v32[ i * 4     ][ j ] );
				assert( p32_4[ i ][ j ].m128_f32[ 2 ] == v32[ i * 4 + 1 ][ j ] );
				assert( p32_4[ i ][ j ].m128_f32[ 1 ] == v32[ i * 4 + 2 ][ j ] );
				assert( p32_4[ i ][ j ].m128_f32[ 0 ] == v32[ i * 4 + 3 ][ j ] );
#endif
			}
		}
		
		unpack< 9 >( o32, p32_4, 16 );
		unpack< 9 >( o64, p64_2, 16 );
		for( u32 i = 0; i < 16; ++i ) {
			assert( all( o32[ i ] == v32[ i ] ) );
			assert( all( o64[ i ] == v64[ i ] ) );
		}
		return true;
	}
#endif // VUL_AOSOA_SSE
	
#ifdef VUL_AOSOA_AVX
	bool TestAOSOA::vectors_avx( )
	{	
		Vector< f32, 9 > v32[ 16 ], o32[ 16 ];
		Vector< f64, 9 > v64[ 16 ], o64[ 16 ];
		Vector< __m256, 9 > p32_8[ 2 ];
		Vector< __m256d, 9 > p64_4[ 4 ];
		
		for( u32 i = 0; i < 16; ++i ) {
			f32 vec[ 9 ] = { ( f32 )VUL_TEST_RNG, ( f32 )VUL_TEST_RNG, ( f32 )VUL_TEST_RNG, 
							   ( f32 )VUL_TEST_RNG, ( f32 )VUL_TEST_RNG, ( f32 )VUL_TEST_RNG, 
							   ( f32 )VUL_TEST_RNG, ( f32 )VUL_TEST_RNG, ( f32 )VUL_TEST_RNG };
#ifdef VUL_CPLUSPLUS11
			v32[ i ] = Vector< f32, 9 >( vec );
			v64[ i ] = Vector< f64, 9 >( vec );
#else
			v32[ i ] = makeVector< f32, 9 >( vec );
			v64[ i ] = makeVector< f64, 9 >( vec );
#endif
		}
		
		pack< 9 >( p32_8, v32, 16 );
		pack< 9 >( p64_4, v64, 16 );

		for( u32 j = 0; j < 9; ++j ) {
			for( u32 i = 0; i < 4; ++i ) {
#ifdef __GNUC__
				f64 arr2[ 4 ];
				_mm256_store_pd( arr2, p64_4[ i ][ j ] );
				assert( arr2[ 3 ] == v64[ i * 4     ][ j ] );
				assert( arr2[ 2 ] == v64[ i * 4 + 1 ][ j ] );
				assert( arr2[ 1 ] == v64[ i * 4 + 2 ][ j ] );
				assert( arr2[ 0 ] == v64[ i * 4 + 3 ][ j ] );
#else
				assert( p64_4[ i ][ j ].m256d_f64[ 3 ] == v64[ i * 4     ][ j ] );
				assert( p64_4[ i ][ j ].m256d_f64[ 2 ] == v64[ i * 4 + 1 ][ j ] );
				assert( p64_4[ i ][ j ].m256d_f64[ 1 ] == v64[ i * 4 + 2 ][ j ] );
				assert( p64_4[ i ][ j ].m256d_f64[ 0 ] == v64[ i * 4 + 3 ][ j ] );
#endif
			}
			for( u32 i = 0; i < 2; ++i ) {
#ifdef __GNUC__
				f32 arr[ 8 ];
				_mm256_store_ps( arr, p32_8[ i ][ j ] );
				assert( arr[ 7 ] == v64[ i * 8     ][ j ] );
				assert( arr[ 6 ] == v64[ i * 8 + 1 ][ j ] );
				assert( arr[ 5 ] == v64[ i * 8 + 2 ][ j ] );
				assert( arr[ 4 ] == v64[ i * 8 + 3 ][ j ] );
				assert( arr[ 3 ] == v64[ i * 8 + 4 ][ j ] );
				assert( arr[ 2 ] == v64[ i * 8 + 5 ][ j ] );
				assert( arr[ 1 ] == v64[ i * 8 + 6 ][ j ] );
				assert( arr[ 0 ] == v64[ i * 8 + 7 ][ j ] );
#else
				assert( p32_8[ i ][ j ].m256_f32[ 7 ] == v32[ i * 8     ][ j ] );
				assert( p32_8[ i ][ j ].m256_f32[ 6 ] == v32[ i * 8 + 1 ][ j ] );
				assert( p32_8[ i ][ j ].m256_f32[ 5 ] == v32[ i * 8 + 2 ][ j ] );
				assert( p32_8[ i ][ j ].m256_f32[ 4 ] == v32[ i * 8 + 3 ][ j ] );
				assert( p32_8[ i ][ j ].m256_f32[ 3 ] == v32[ i * 8 + 4 ][ j ] );
				assert( p32_8[ i ][ j ].m256_f32[ 2 ] == v32[ i * 8 + 5 ][ j ] );
				assert( p32_8[ i ][ j ].m256_f32[ 1 ] == v32[ i * 8 + 6 ][ j ] );
				assert( p32_8[ i ][ j ].m256_f32[ 0 ] == v32[ i * 8 + 7 ][ j ] );
#endif
			}
		}
		
		unpack< 9 >( o32, p32_8, 16 );
		unpack< 9 >( o64, p64_4, 16 );
		for( u32 i = 0; i < 16; ++i ) {
			assert( all( o32[ i ] == v32[ i ] ) );
			assert( all( o64[ i ] == v64[ i ] ) );
		}

		return true;
	}
#endif // VUL_AOSOA_AVX

#ifdef VUL_AOSOA_NEON
	bool TestAOSOA::vectors_neon( )
	{	
		Vector< f32, 9 > v32[ 16 ], o32[ 16 ];
		Vector< float32x4_t, 9 > p32_4[ 4 ];
		
		for( u32 i = 0; i < 16; ++i ) {
			f32 vec[ 9 ] = { ( f32 )VUL_TEST_RNG, ( f32 )VUL_TEST_RNG, ( f32 )VUL_TEST_RNG, 
							   ( f32 )VUL_TEST_RNG, ( f32 )VUL_TEST_RNG, ( f32 )VUL_TEST_RNG, 
							   ( f32 )VUL_TEST_RNG, ( f32 )VUL_TEST_RNG, ( f32 )VUL_TEST_RNG };
#ifdef VUL_CPLUSPLUS11
			v32[ i ] = Vector< f32, 9 >( vec );
#else
			v32[ i ] = makeVector< f32, 9 >( vec );
#endif
		}
		
		pack< 9 >( p32_4, v32, 16 );

		for( u32 j = 0; j < 9; ++j ) {
			for( u32 i = 0; i < 4; ++i ) {
				assert( vgetq_lane_f32( p32_4[ i ][ j ], 3 ) == v32[ i * 4     ][ j ] );
				assert( vgetq_lane_f32( p32_4[ i ][ j ], 2 ) == v32[ i * 4 + 1 ][ j ] );
				assert( vgetq_lane_f32( p32_4[ i ][ j ], 1 ) == v32[ i * 4 + 2 ][ j ] );
				assert( vgetq_lane_f32( p32_4[ i ][ j ], 0 ) == v32[ i * 4 + 3 ][ j ] );
			}
		}
		
		unpack< 9 >( o32, p32_4, 16 );
		for( u32 i = 0; i < 16; ++i ) {
			assert( all( o32[ i ] == v32[ i ] ) );
		}
		return true;
	}
#endif // VUL_AOSOA_NEON

#ifdef VUL_AOSOA_SSE	
	bool TestAOSOA::aabbs_sse( )
	{	
		AABB< f32, 9 > v32[ 16 ], o32[ 16 ];
		AABB< f64, 9 > v64[ 16 ], o64[ 16 ];
		AABB< __m128, 9 > p32_4[ 4 ];
		AABB< __m128d, 9 > p64_2[ 8 ];
		
		for( u32 i = 0; i < 16; ++i ) {
			f32 mini[ 9 ] = { ( f32 )VUL_TEST_RNG, ( f32 )VUL_TEST_RNG, ( f32 )VUL_TEST_RNG, 
								( f32 )VUL_TEST_RNG, ( f32 )VUL_TEST_RNG, ( f32 )VUL_TEST_RNG, 
								( f32 )VUL_TEST_RNG, ( f32 )VUL_TEST_RNG, ( f32 )VUL_TEST_RNG };
			f32 maxi[ 9 ] = { ( f32 )VUL_TEST_RNG, ( f32 )VUL_TEST_RNG, ( f32 )VUL_TEST_RNG, 
								( f32 )VUL_TEST_RNG, ( f32 )VUL_TEST_RNG, ( f32 )VUL_TEST_RNG, 
								( f32 )VUL_TEST_RNG, ( f32 )VUL_TEST_RNG, ( f32 )VUL_TEST_RNG };
#ifdef VUL_CPLUSPLUS11
			v32[ i ] = AABB< f32, 9 >( Vector< f32, 9 >( mini ), Vector< f32, 9 >( maxi ) );
			v64[ i ] = AABB< f64, 9 >( Vector< f64, 9 >( mini ), Vector< f64, 9 >( maxi ) );
#else
			v32[ i ] = makeAABB< f32, 9 >( makeVector< f32, 9 >( mini ), makeVector< f32, 9 >( maxi ) );
			v64[ i ] = makeAABB< f64, 9 >( makeVector< f64, 9 >( mini ), makeVector< f64, 9 >( maxi ) );
#endif
		}

		pack< 9 >( p32_4, v32, 16 );
		pack< 9 >( p64_2, v64, 16 );

		for( u32 j = 0; j < 9; ++j ) {
			for( u32 i = 0; i < 8; ++i ) {
#ifdef __GNUC__
				f64 arr[ 2 ];
				_mm_store_pd( arr, p64_2[ i ]._min[ j ] );
				assert( arr[ 1 ] == v64[ i * 2     ]._min[ j ] );
				assert( arr[ 0 ] == v64[ i * 2 + 1 ]._min[ j ] );

				_mm_store_pd( arr, p64_2[ i ]._max[ j ] );
				assert( arr[ 1 ] == v64[ i * 2     ]._max[ j ] );
				assert( arr[ 0 ] == v64[ i * 2 + 1 ]._max[ j ] );
#else
				assert( p64_2[ i ]._min[ j ].m128d_f64[ 1 ] == v64[ i * 2     ]._min[ j ] );
				assert( p64_2[ i ]._min[ j ].m128d_f64[ 0 ] == v64[ i * 2 + 1 ]._min[ j ] );

				assert( p64_2[ i ]._max[ j ].m128d_f64[ 1 ] == v64[ i * 2     ]._max[ j ] );
				assert( p64_2[ i ]._max[ j ].m128d_f64[ 0 ] == v64[ i * 2 + 1 ]._max[ j ] );
#endif
			}
			for( u32 i = 0; i < 4; ++i ) {
#ifdef __GNUC__
				f32 arr[ 4 ];
				_mm_store_ps( arr, p32_4[ i ]._min[ j ] );
				assert( arr[ 3 ] == v32[ i * 4     ]._min[ j ] );
				assert( arr[ 2 ] == v32[ i * 4 + 1 ]._min[ j ] );
				assert( arr[ 1 ] == v32[ i * 4 + 2 ]._min[ j ] );
				assert( arr[ 0 ] == v32[ i * 4 + 3 ]._min[ j ] );

				_mm_store_ps( arr, p32_4[ i ]._max[ j ] );
				assert( arr[ 3 ] == v32[ i * 4     ]._max[ j ] );
				assert( arr[ 2 ] == v32[ i * 4 + 1 ]._max[ j ] );
				assert( arr[ 1 ] == v32[ i * 4 + 2 ]._max[ j ] );
				assert( arr[ 0 ] == v32[ i * 4 + 3 ]._max[ j ] );
#else
				assert( p32_4[ i ]._min[ j ].m128_f32[ 3 ] == v32[ i * 4     ]._min[ j ] );
				assert( p32_4[ i ]._min[ j ].m128_f32[ 2 ] == v32[ i * 4 + 1 ]._min[ j ] );
				assert( p32_4[ i ]._min[ j ].m128_f32[ 1 ] == v32[ i * 4 + 2 ]._min[ j ] );
				assert( p32_4[ i ]._min[ j ].m128_f32[ 0 ] == v32[ i * 4 + 3 ]._min[ j ] );
				
				assert( p32_4[ i ]._max[ j ].m128_f32[ 3 ] == v32[ i * 4     ]._max[ j ] );
				assert( p32_4[ i ]._max[ j ].m128_f32[ 2 ] == v32[ i * 4 + 1 ]._max[ j ] );
				assert( p32_4[ i ]._max[ j ].m128_f32[ 1 ] == v32[ i * 4 + 2 ]._max[ j ] );
				assert( p32_4[ i ]._max[ j ].m128_f32[ 0 ] == v32[ i * 4 + 3 ]._max[ j ] );
#endif
			}
		}

		unpack< 9 >( o32, p32_4, 16 );
		unpack< 9 >( o64, p64_2, 16 );
		for( u32 i = 0; i < 16; ++i ) {
			assert( all( o32[ i ]._min == v32[ i ]._min ) );
			assert( all( o32[ i ]._max == v32[ i ]._max ) );

			assert( all( o64[ i ]._min == v64[ i ]._min ) );
			assert( all( o64[ i ]._max == v64[ i ]._max ) );
		}

		return true;
	}
#endif // VUL_AOSOA_SSE
	
#ifdef VUL_AOSOA_AVX
	bool TestAOSOA::aabbs_avx( )
	{	
		AABB< f32, 9 > v32[ 16 ], o32[ 16 ];
		AABB< f64, 9 > v64[ 16 ], o64[ 16 ];
		AABB< __m256, 9 > p32_8[ 2 ];
		AABB< __m256d, 9 > p64_4[ 4 ];
		
		for( u32 i = 0; i < 16; ++i ) {
			f32 mini[ 9 ] = { ( f32 )VUL_TEST_RNG, ( f32 )VUL_TEST_RNG, ( f32 )VUL_TEST_RNG, 
								( f32 )VUL_TEST_RNG, ( f32 )VUL_TEST_RNG, ( f32 )VUL_TEST_RNG, 
								( f32 )VUL_TEST_RNG, ( f32 )VUL_TEST_RNG, ( f32 )VUL_TEST_RNG };
			f32 maxi[ 9 ] = { ( f32 )VUL_TEST_RNG, ( f32 )VUL_TEST_RNG, ( f32 )VUL_TEST_RNG, 
								( f32 )VUL_TEST_RNG, ( f32 )VUL_TEST_RNG, ( f32 )VUL_TEST_RNG, 
								( f32 )VUL_TEST_RNG, ( f32 )VUL_TEST_RNG, ( f32 )VUL_TEST_RNG };
#ifdef VUL_CPLUSPLUS11
			v32[ i ] = AABB< f32, 9 >( Vector< f32, 9 >( mini ), Vector< f32, 9 >( maxi ) );
			v64[ i ] = AABB< f64, 9 >( Vector< f64, 9 >( mini ), Vector< f64, 9 >( maxi ) );
#else
			v32[ i ] = makeAABB< f32, 9 >( makeVector< f32, 9 >( mini ), makeVector< f32, 9 >( maxi ) );
			v64[ i ] = makeAABB< f64, 9 >( makeVector< f64, 9 >( mini ), makeVector< f64, 9 >( maxi ) );
#endif
		}

		pack< 9 >( p32_8, v32, 16 );
		pack< 9 >( p64_4, v64, 16 );

		for( u32 j = 0; j < 9; ++j ) {
			for( u32 i = 0; i < 4; ++i ) {
#ifdef __GNUC__
				f64 arr2[ 4 ];
				_mm256_store_pd( arr2, p64_4[ i ]._min[ j ] );
				assert( arr2[ 3 ] == v64[ i * 4     ]._min[ j ] );
				assert( arr2[ 2 ] == v64[ i * 4 + 1 ]._min[ j ] );
				assert( arr2[ 1 ] == v64[ i * 4 + 2 ]._min[ j ] );
				assert( arr2[ 0 ] == v64[ i * 4 + 3 ]._min[ j ] );

				_mm256_store_pd( arr2, p64_4[ i ]._max[ j ] );
				assert( arr2[ 3 ] == v64[ i * 4     ]._max[ j ] );
				assert( arr2[ 2 ] == v64[ i * 4 + 1 ]._max[ j ] );
				assert( arr2[ 1 ] == v64[ i * 4 + 2 ]._max[ j ] );
				assert( arr2[ 0 ] == v64[ i * 4 + 3 ]._max[ j ] );
#else
				assert( p64_4[ i ]._min[ j ].m256d_f64[ 3 ] == v64[ i * 4     ]._min[ j ] );
				assert( p64_4[ i ]._min[ j ].m256d_f64[ 2 ] == v64[ i * 4 + 1 ]._min[ j ] );
				assert( p64_4[ i ]._min[ j ].m256d_f64[ 1 ] == v64[ i * 4 + 2 ]._min[ j ] );
				assert( p64_4[ i ]._min[ j ].m256d_f64[ 0 ] == v64[ i * 4 + 3 ]._min[ j ] );

				assert( p64_4[ i ]._max[ j ].m256d_f64[ 3 ] == v64[ i * 4     ]._max[ j ] );
				assert( p64_4[ i ]._max[ j ].m256d_f64[ 2 ] == v64[ i * 4 + 1 ]._max[ j ] );
				assert( p64_4[ i ]._max[ j ].m256d_f64[ 1 ] == v64[ i * 4 + 2 ]._max[ j ] );
				assert( p64_4[ i ]._max[ j ].m256d_f64[ 0 ] == v64[ i * 4 + 3 ]._max[ j ] );
#endif
			}
			for( u32 i = 0; i < 2; ++i ) {
#ifdef __GNUC__
				f32 arr[ 8 ];
				_mm256_store_ps( arr, p32_8[ i ]._min[ j ] );
				assert( arr[ 7 ] == v32[ i * 8     ]._min[ j ] );
				assert( arr[ 6 ] == v32[ i * 8 + 1 ]._min[ j ] );
				assert( arr[ 5 ] == v32[ i * 8 + 2 ]._min[ j ] );
				assert( arr[ 4 ] == v32[ i * 8 + 3 ]._min[ j ] );
				assert( arr[ 3 ] == v32[ i * 8 + 4 ]._min[ j ] );
				assert( arr[ 2 ] == v32[ i * 8 + 5 ]._min[ j ] );
				assert( arr[ 1 ] == v32[ i * 8 + 6 ]._min[ j ] );
				assert( arr[ 0 ] == v32[ i * 8 + 7 ]._min[ j ] );

				_mm256_store_ps( arr, p32_8[ i ]._max[ j ] );
				assert( arr[ 7 ] == v32[ i * 8     ]._max[ j ] );
				assert( arr[ 6 ] == v32[ i * 8 + 1 ]._max[ j ] );
				assert( arr[ 5 ] == v32[ i * 8 + 2 ]._max[ j ] );
				assert( arr[ 4 ] == v32[ i * 8 + 3 ]._max[ j ] );
				assert( arr[ 3 ] == v32[ i * 8 + 4 ]._max[ j ] );
				assert( arr[ 2 ] == v32[ i * 8 + 5 ]._max[ j ] );
				assert( arr[ 1 ] == v32[ i * 8 + 6 ]._max[ j ] );
				assert( arr[ 0 ] == v32[ i * 8 + 7 ]._max[ j ] );
#else
				assert( p32_8[ i ]._min[ j ].m256_f32[ 7 ] == v32[ i * 8     ]._min[ j ] );
				assert( p32_8[ i ]._min[ j ].m256_f32[ 6 ] == v32[ i * 8 + 1 ]._min[ j ] );
				assert( p32_8[ i ]._min[ j ].m256_f32[ 5 ] == v32[ i * 8 + 2 ]._min[ j ] );
				assert( p32_8[ i ]._min[ j ].m256_f32[ 4 ] == v32[ i * 8 + 3 ]._min[ j ] );
				assert( p32_8[ i ]._min[ j ].m256_f32[ 3 ] == v32[ i * 8 + 4 ]._min[ j ] );
				assert( p32_8[ i ]._min[ j ].m256_f32[ 2 ] == v32[ i * 8 + 5 ]._min[ j ] );
				assert( p32_8[ i ]._min[ j ].m256_f32[ 1 ] == v32[ i * 8 + 6 ]._min[ j ] );
				assert( p32_8[ i ]._min[ j ].m256_f32[ 0 ] == v32[ i * 8 + 7 ]._min[ j ] );
				
				assert( p32_8[ i ]._max[ j ].m256_f32[ 7 ] == v32[ i * 8     ]._max[ j ] );
				assert( p32_8[ i ]._max[ j ].m256_f32[ 6 ] == v32[ i * 8 + 1 ]._max[ j ] );
				assert( p32_8[ i ]._max[ j ].m256_f32[ 5 ] == v32[ i * 8 + 2 ]._max[ j ] );
				assert( p32_8[ i ]._max[ j ].m256_f32[ 4 ] == v32[ i * 8 + 3 ]._max[ j ] );
				assert( p32_8[ i ]._max[ j ].m256_f32[ 3 ] == v32[ i * 8 + 4 ]._max[ j ] );
				assert( p32_8[ i ]._max[ j ].m256_f32[ 2 ] == v32[ i * 8 + 5 ]._max[ j ] );
				assert( p32_8[ i ]._max[ j ].m256_f32[ 1 ] == v32[ i * 8 + 6 ]._max[ j ] );
				assert( p32_8[ i ]._max[ j ].m256_f32[ 0 ] == v32[ i * 8 + 7 ]._max[ j ] );
#endif
			}
		}
		
		unpack< 9 >( o32, p32_8, 16 );
		unpack< 9 >( o64, p64_4, 16 );
		for( u32 i = 0; i < 16; ++i ) {
			assert( all( o32[ i ]._min == v32[ i ]._min ) );
			assert( all( o32[ i ]._max == v32[ i ]._max ) );

			assert( all( o64[ i ]._min == v64[ i ]._min ) );
			assert( all( o64[ i ]._max == v64[ i ]._max ) );
		}

		return true;
	}
#endif // VUL_AOSOA_AVX

#ifdef VUL_AOSOA_NEON
	bool TestAOSOA::aabbs_neon( )
	{	
		AABB< f32, 9 > v32[ 16 ], o32[ 16 ];
		AABB< float32x4_t, 9 > p32_4[ 4 ];
		
		for( u32 i = 0; i < 16; ++i ) {
			f32 mini[ 9 ] = { ( f32 )VUL_TEST_RNG, ( f32 )VUL_TEST_RNG, ( f32 )VUL_TEST_RNG, 
								( f32 )VUL_TEST_RNG, ( f32 )VUL_TEST_RNG, ( f32 )VUL_TEST_RNG, 
								( f32 )VUL_TEST_RNG, ( f32 )VUL_TEST_RNG, ( f32 )VUL_TEST_RNG };
			f32 maxi[ 9 ] = { ( f32 )VUL_TEST_RNG, ( f32 )VUL_TEST_RNG, ( f32 )VUL_TEST_RNG, 
								( f32 )VUL_TEST_RNG, ( f32 )VUL_TEST_RNG, ( f32 )VUL_TEST_RNG, 
								( f32 )VUL_TEST_RNG, ( f32 )VUL_TEST_RNG, ( f32 )VUL_TEST_RNG };
#ifdef VUL_CPLUSPLUS11
			v32[ i ] = AABB< f32, 9 >( Vector< f32, 9 >( mini ), Vector< f32, 9 >( maxi ) );
#else
			v32[ i ] = makeAABB< f32, 9 >( makeVector< f32, 9 >( mini ), makeVector< f32, 9 >( maxi ) );
#endif
		}

		pack< 9 >( p32_4, v32, 16 );

		for( u32 j = 0; j < 9; ++j ) {
			for( u32 i = 0; i < 4; ++i ) {
				assert( vgetq_lane_f32( p32_4[ i ]._min[ j ], 3 ) == v32[ i * 4     ]._min[ j ] );
				assert( vgetq_lane_f32( p32_4[ i ]._min[ j ], 2 ) == v32[ i * 4 + 1 ]._min[ j ] );
				assert( vgetq_lane_f32( p32_4[ i ]._min[ j ], 1 ) == v32[ i * 4 + 2 ]._min[ j ] );
				assert( vgetq_lane_f32( p32_4[ i ]._min[ j ], 0 ) == v32[ i * 4 + 3 ]._min[ j ] );
				
				assert( vgetq_lane_f32( p32_4[ i ]._max[ j ], 3 ) == v32[ i * 4     ]._max[ j ] );
				assert( vgetq_lane_f32( p32_4[ i ]._max[ j ], 2 ) == v32[ i * 4 + 1 ]._max[ j ] );
				assert( vgetq_lane_f32( p32_4[ i ]._max[ j ], 1 ) == v32[ i * 4 + 2 ]._max[ j ] );
				assert( vgetq_lane_f32( p32_4[ i ]._max[ j ], 0 ) == v32[ i * 4 + 3 ]._max[ j ] );
			}
		}

		unpack< 9 >( o32, p32_4, 16 );
		for( u32 i = 0; i < 16; ++i ) {
			assert( all( o32[ i ]._min == v32[ i ]._min ) );
			assert( all( o32[ i ]._max == v32[ i ]._max ) );
		}

		return true;
	}
#endif // VUL_AOSOA_NEON

};

#undef VUL_TEST_RNG
#pragma warning(default: 6001)

#endif
