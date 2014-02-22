/*
 * Villains' Utility Library - Thomas Martin Schmid, 2014. Public domain�
 *
 * This file contains tests for the AOSOA packing/unpacking functions in vul_aosoa.hpp
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
#ifndef VUL_TEST_AOSOA_HPP
#define VUL_TEST_AOSOA_HPP

#include <cassert>

#include "../vul_math.hpp"

#define VUL_TEST_RNG ( float )( ( float )rand( ) / ( float )RAND_MAX )

using namespace vul;

namespace vul_test {
	class TestAOSOA {

	public:
		static bool test( );

	private:
		static bool vectors( );
		static bool aabbs( );
	};

	bool TestAOSOA::test( )
	{
		assert( vectors( ) );
		assert( aabbs( ) );

		return true;
	}

	bool TestAOSOA::vectors( )
	{	
		Vector< f32_t, 9 > v32[ 16 ], o32[ 16 ];
		Vector< __m128, 9 > p32_4[ 4 ];
		Vector< __m256, 9 > p32_8[ 2 ];
		Vector< f64_t, 9 > v64[ 16 ], o64[ 16 ];
		Vector< __m128d, 9 > p64_2[ 8 ];
		Vector< __m256d, 9 > p64_4[ 4 ];
		
		for( ui32_t i = 0; i < 16; ++i ) {
			f32_t vec[ 9 ] = { ( f32_t )VUL_TEST_RNG, ( f32_t )VUL_TEST_RNG, ( f32_t )VUL_TEST_RNG, 
							   ( f32_t )VUL_TEST_RNG, ( f32_t )VUL_TEST_RNG, ( f32_t )VUL_TEST_RNG, 
							   ( f32_t )VUL_TEST_RNG, ( f32_t )VUL_TEST_RNG, ( f32_t )VUL_TEST_RNG };
#ifdef VUL_CPLUSPLUS11
			v32[ i ] = Vector< f32_t, 9 >( vec );
			v64[ i ] = Vector< f64_t, 9 >( vec );
#else
			v32[ i ] = makeVector< f32_t, 9 >( vec );
			v64[ i ] = makeVector< f64_t, 9 >( vec );
#endif
		}

		pack( p32_4, v32, 16 );
		pack( p32_8, v32, 16 );
		pack( p64_2, v64, 16 );
		pack( p64_4, v64, 16 );

		for( ui32_t j = 0; j < 9; ++j ) {
			for( ui32_t i = 0; i < 2; ++i ) {
				assert( p64_2[ i ][ j ].m128d_f64[ 0 ] == v64[ i * 4     ][ j ] );
				assert( p64_2[ i ][ j ].m128d_f64[ 1 ] == v64[ i * 4 + 1 ][ j ] );
			}
			for( ui32_t i = 0; i < 4; ++i ) {
				assert( p32_4[ i ][ j ].m128_f32[ 0 ] == v32[ i * 4     ][ j ] );
				assert( p32_4[ i ][ j ].m128_f32[ 1 ] == v32[ i * 4 + 1 ][ j ] );
				assert( p32_4[ i ][ j ].m128_f32[ 2 ] == v32[ i * 4 + 2 ][ j ] );
				assert( p32_4[ i ][ j ].m128_f32[ 3 ] == v32[ i * 4 + 3 ][ j ] );

				assert( p64_4[ i ][ j ].m256d_f64[ 0 ] == v64[ i * 4     ][ j ] );
				assert( p64_4[ i ][ j ].m256d_f64[ 1 ] == v64[ i * 4 + 1 ][ j ] );
				assert( p64_4[ i ][ j ].m256d_f64[ 2 ] == v64[ i * 4 + 2 ][ j ] );
				assert( p64_4[ i ][ j ].m256d_f64[ 3 ] == v64[ i * 4 + 3 ][ j ] );
			}
			for( ui32_t i = 0; i < 8; ++i ) {
				assert( p32_8[ i ][ j ].m256_f32[ 0 ] == v32[ i * 4     ][ j ] );
				assert( p32_8[ i ][ j ].m256_f32[ 1 ] == v32[ i * 4 + 1 ][ j ] );
				assert( p32_8[ i ][ j ].m256_f32[ 2 ] == v32[ i * 4 + 2 ][ j ] );
				assert( p32_8[ i ][ j ].m256_f32[ 3 ] == v32[ i * 4 + 3 ][ j ] );
				assert( p32_8[ i ][ j ].m256_f32[ 4 ] == v32[ i * 4 + 4 ][ j ] );
				assert( p32_8[ i ][ j ].m256_f32[ 5 ] == v32[ i * 4 + 5 ][ j ] );
				assert( p32_8[ i ][ j ].m256_f32[ 6 ] == v32[ i * 4 + 6 ][ j ] );
				assert( p32_8[ i ][ j ].m256_f32[ 7 ] == v32[ i * 4 + 7 ][ j ] );
			}
		}

		unpack( o32, p32_4, 16 );
		unpack( o64, p64_4, 16 );
		for( ui32_t i = 0; i < 16; ++i ) {
			assert( all( o32[ i ] == v32[ i ] ) );
			assert( all( o64[ i ] == v64[ i ] ) );
		}
		unpack( o32, p32_8, 16 );
		unpack( o64, p64_2, 16 );
		for( ui32_t i = 0; i < 16; ++i ) {
			assert( all( o32[ i ] == v32[ i ] ) );
			assert( all( o64[ i ] == v64[ i ] ) );
		}

		return true;
	}
	bool TestAOSOA::aabbs( )
	{	
		AABB< f32_t, 9 > v32[ 16 ], o32[ 16 ];
		AABB< __m128, 9 > p32_4[ 4 ];
		AABB< __m256, 9 > p32_8[ 2 ];
		AABB< f64_t, 9 > v64[ 16 ], o64[ 16 ];
		AABB< __m128d, 9 > p64_2[ 8 ];
		AABB< __m256d, 9 > p64_4[ 4 ];
		
		for( ui32_t i = 0; i < 16; ++i ) {
			f32_t mini[ 9 ] = { ( f32_t )VUL_TEST_RNG, ( f32_t )VUL_TEST_RNG, ( f32_t )VUL_TEST_RNG, 
								( f32_t )VUL_TEST_RNG, ( f32_t )VUL_TEST_RNG, ( f32_t )VUL_TEST_RNG, 
								( f32_t )VUL_TEST_RNG, ( f32_t )VUL_TEST_RNG, ( f32_t )VUL_TEST_RNG };
			f32_t maxi[ 9 ] = { ( f32_t )VUL_TEST_RNG, ( f32_t )VUL_TEST_RNG, ( f32_t )VUL_TEST_RNG, 
								( f32_t )VUL_TEST_RNG, ( f32_t )VUL_TEST_RNG, ( f32_t )VUL_TEST_RNG, 
								( f32_t )VUL_TEST_RNG, ( f32_t )VUL_TEST_RNG, ( f32_t )VUL_TEST_RNG };
#ifdef VUL_CPLUSPLUS11
			v32[ i ] = AABB< f32_t, 9 >( Vector< f32_t, 9 >( mini ), Vector< f32_t, 9 >( maxi ) );
			v64[ i ] = AABB< f64_t, 9 >( Vector< f64_t, 9 >( mini ), Vector< f64_t, 9 >( maxi ) );
#else
			v32[ i ] = makeAABB< f32_t, 9 >( makeVector< f32_t, 9 >( mini ), makeVector< f32_t, 9 >( maxi ) );
			v64[ i ] = makeAABB< f64_t, 9 >( makeVector< f64_t, 9 >( mini ), makeVector< f64_t, 9 >( maxi ) );
#endif
		}

		pack( p32_4, v32, 16 );
		pack( p32_8, v32, 16 );
		pack( p64_2, v64, 16 );
		pack( p64_4, v64, 16 );

		for( ui32_t j = 0; j < 9; ++j ) {
			for( ui32_t i = 0; i < 2; ++i ) {
				assert( p64_2[ i ]._min[ j ].m128d_f64[ 0 ] == v64[ i * 4     ]._min[ j ] );
				assert( p64_2[ i ]._min[ j ].m128d_f64[ 1 ] == v64[ i * 4 + 1 ]._min[ j ] );

				assert( p64_2[ i ]._max[ j ].m128d_f64[ 0 ] == v64[ i * 4     ]._max[ j ] );
				assert( p64_2[ i ]._max[ j ].m128d_f64[ 1 ] == v64[ i * 4 + 1 ]._max[ j ] );
			}
			for( ui32_t i = 0; i < 4; ++i ) {
				assert( p32_4[ i ]._min[ j ].m128_f32[ 0 ] == v32[ i * 4     ]._min[ j ] );
				assert( p32_4[ i ]._min[ j ].m128_f32[ 1 ] == v32[ i * 4 + 1 ]._min[ j ] );
				assert( p32_4[ i ]._min[ j ].m128_f32[ 2 ] == v32[ i * 4 + 2 ]._min[ j ] );
				assert( p32_4[ i ]._min[ j ].m128_f32[ 3 ] == v32[ i * 4 + 3 ]._min[ j ] );
				
				assert( p32_4[ i ]._max[ j ].m128_f32[ 0 ] == v32[ i * 4     ]._max[ j ] );
				assert( p32_4[ i ]._max[ j ].m128_f32[ 1 ] == v32[ i * 4 + 1 ]._max[ j ] );
				assert( p32_4[ i ]._max[ j ].m128_f32[ 2 ] == v32[ i * 4 + 2 ]._max[ j ] );
				assert( p32_4[ i ]._max[ j ].m128_f32[ 3 ] == v32[ i * 4 + 3 ]._max[ j ] );
				
				assert( p64_4[ i ]._min[ j ].m256d_f64[ 0 ] == v64[ i * 4     ]._min[ j ] );
				assert( p64_4[ i ]._min[ j ].m256d_f64[ 1 ] == v64[ i * 4 + 1 ]._min[ j ] );
				assert( p64_4[ i ]._min[ j ].m256d_f64[ 2 ] == v64[ i * 4 + 2 ]._min[ j ] );
				assert( p64_4[ i ]._min[ j ].m256d_f64[ 3 ] == v64[ i * 4 + 3 ]._min[ j ] );

				assert( p64_4[ i ]._max[ j ].m256d_f64[ 0 ] == v64[ i * 4     ]._max[ j ] );
				assert( p64_4[ i ]._max[ j ].m256d_f64[ 1 ] == v64[ i * 4 + 1 ]._max[ j ] );
				assert( p64_4[ i ]._max[ j ].m256d_f64[ 2 ] == v64[ i * 4 + 2 ]._max[ j ] );
				assert( p64_4[ i ]._max[ j ].m256d_f64[ 3 ] == v64[ i * 4 + 3 ]._max[ j ] );
			}
			for( ui32_t i = 0; i < 8; ++i ) {
				assert( p32_8[ i ]._min[ j ].m256_f32[ 0 ] == v32[ i * 4     ]._min[ j ] );
				assert( p32_8[ i ]._min[ j ].m256_f32[ 1 ] == v32[ i * 4 + 1 ]._min[ j ] );
				assert( p32_8[ i ]._min[ j ].m256_f32[ 2 ] == v32[ i * 4 + 2 ]._min[ j ] );
				assert( p32_8[ i ]._min[ j ].m256_f32[ 3 ] == v32[ i * 4 + 3 ]._min[ j ] );
				assert( p32_8[ i ]._min[ j ].m256_f32[ 4 ] == v32[ i * 4 + 4 ]._min[ j ] );
				assert( p32_8[ i ]._min[ j ].m256_f32[ 5 ] == v32[ i * 4 + 5 ]._min[ j ] );
				assert( p32_8[ i ]._min[ j ].m256_f32[ 6 ] == v32[ i * 4 + 6 ]._min[ j ] );
				assert( p32_8[ i ]._min[ j ].m256_f32[ 7 ] == v32[ i * 4 + 7 ]._min[ j ] );
				
				assert( p32_8[ i ]._max[ j ].m256_f32[ 0 ] == v32[ i * 4     ]._max[ j ] );
				assert( p32_8[ i ]._max[ j ].m256_f32[ 1 ] == v32[ i * 4 + 1 ]._max[ j ] );
				assert( p32_8[ i ]._max[ j ].m256_f32[ 2 ] == v32[ i * 4 + 2 ]._max[ j ] );
				assert( p32_8[ i ]._max[ j ].m256_f32[ 3 ] == v32[ i * 4 + 3 ]._max[ j ] );
				assert( p32_8[ i ]._max[ j ].m256_f32[ 4 ] == v32[ i * 4 + 4 ]._max[ j ] );
				assert( p32_8[ i ]._max[ j ].m256_f32[ 5 ] == v32[ i * 4 + 5 ]._max[ j ] );
				assert( p32_8[ i ]._max[ j ].m256_f32[ 6 ] == v32[ i * 4 + 6 ]._max[ j ] );
				assert( p32_8[ i ]._max[ j ].m256_f32[ 7 ] == v32[ i * 4 + 7 ]._max[ j ] );
			}
		}

		unpack( o32, p32_4, 16 );
		unpack( o64, p64_4, 16 );
		for( ui32_t i = 0; i < 16; ++i ) {
			assert( all( o32[ i ]._min == v32[ i ]._min ) );
			assert( all( o32[ i ]._max == v32[ i ]._max ) );

			assert( all( o64[ i ]._min == v64[ i ]._min ) );
			assert( all( o64[ i ]._max == v64[ i ]._max ) );
		}
		unpack( o32, p32_8, 16 );
		unpack( o64, p64_2, 16 );
		for( ui32_t i = 0; i < 16; ++i ) {
			assert( all( o32[ i ]._min == v32[ i ]._min ) );
			assert( all( o32[ i ]._max == v32[ i ]._max ) );

			assert( all( o64[ i ]._min == v64[ i ]._min ) );
			assert( all( o64[ i ]._max == v64[ i ]._max ) );
		}

		return true;
	}
};

#endif