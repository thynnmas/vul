/*
 * Villains' Utility Library - Thomas Martin Schmid, 2017. Public domain¹
 *
 * This file contains tests for the half precision floating point 
 * type in vul_half.hpp. We perform exhaustive testing where we can.
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
#ifndef VUL_TEST_HALF_HPP
#define VUL_TEST_HALF_HPP

#include <cassert>

#include "../vul_math.hpp"

#define VUL_TEST_FUZZ_COUNT 10000
#define VUL_TEST_RNG ( float )( ( ( float )rand( ) / RAND_MAX ) * 255.f )

using namespace vul;

namespace vul_test {
	class TestHalf {

	public:
		static bool test( );

	private:
		static bool setsandcasts( );
		static bool compares( );
		static bool ops( );
		static bool masses( );

		static bool comparefloats( float a, float b, float max_rel_diff );
	};

	bool TestHalf::test( )
	{
		assert( setsandcasts( ) );
		assert( compares( ) );
		assert( ops( ) );
		assert( masses( ) );

		return true;
	}
	
	bool TestHalf::setsandcasts( )
	{
		half t( 1.0 );
		assert( ( f32 )t == 1.f );
		
		t = half( 1.f );
		assert( ( f32 )t == 1.f );
		
		t = ( half )( fixed_32< 16 >( 1.f ) );
		assert( ( f32 )t == 1.f );
		assert( ( f64 )t == 1.0 );
		assert( ( fixed_32< 16 > )t == fixed_32< 16 >( 1.f ) );

		t = 1.f;
		assert( ( f32 )t == 1.f );
		
		half t2 = t;
		assert( ( f32 )t2 == 1.f );

		t = fixed_32< 16 >( 1.f );
		assert( ( f32 )t == 1.f );

		return true;
	}
	bool TestHalf::compares( )
	{		
		half a( 1.f ), b( 2.f ), c( 1.f );

		assert( a != b );
		assert( a <= b );
		assert( a < b );
		assert( b >= a );
		assert( b > a );
		assert( a == c );
		assert( a >= c );
		assert( a <= c );
		
		assert( !( b == a ) );
		assert( !( b < a ) );
		assert( !( a != c ) );
		assert( !( a > c ) );
		assert( !( a < c ) );

		return true;
	}
	bool TestHalf::ops( )
	{		
		f32 f = VUL_TEST_RNG;
		half h, hm;
		h = f;
		hm = -fabs( f );
		f32 f16eps = std::numeric_limits< half >::epsilon( );
		assert( comparefloats( ( f32 )fabs( ( f32 )h ), ( f32 )fabs( f ), f16eps ) );
		assert( comparefloats( ( f32 )fabs( ( f32 )hm ), ( f32 )fabs( ( f32 )h ), f16eps ) );
		
		for( u32 i = 0; i < VUL_TEST_FUZZ_COUNT; ++i ) {
			half a, b, r;
			
			a = VUL_TEST_RNG;
			b = VUL_TEST_RNG;
			while( ( f32 )( a * b ) < ( f32 )f16eps ) {
				a = VUL_TEST_RNG;
				b = VUL_TEST_RNG;
			}

			r = a + b;
			f = ( f32 )a + ( f32 )b;		
			assert( comparefloats( r, f, f16eps ) );
			r = a - b;
			f = ( f32 )a - ( f32 )b;
			assert( comparefloats( r, f, f16eps ) );
			r = a * b;
			f = ( f32 )a * ( f32 )b;
			assert( comparefloats( r, f, f16eps ) );
			r = a / b;
			f = ( f32 )a / ( f32 )b;
			assert( comparefloats( r, f, f16eps ) );
			
			r = a;
			a += b;
			assert( comparefloats( r + b, a, f16eps ) );
			r = a;
			a -= b;
			assert( comparefloats( r - b, a, f16eps ) );
			r = a;
			a *= b;
			assert( comparefloats( r * b, a, f16eps ) );
			r = a;
			a /= b;
			assert( comparefloats( r / b, a, f16eps ) );
			
			r = a;
			a += ( f32 )b;
			assert( comparefloats( r + b, a, f16eps ) );
			r = a;
			a -= ( f32 )b;
			assert( comparefloats( r - b, a, f16eps ) );
			r = a;
			a *= ( f32 )b;
			assert( comparefloats( r * b, a, f16eps ) );
			r = a;
			a /= ( f32 )b;
			assert( comparefloats( r / b, a, f16eps ) );
			
			r = a;
			a += ( fixed_32< 8 > )b;
			assert( comparefloats( r + ( fixed_32< 8 > )b, a, f16eps ) );
			r = a;
			a -= ( fixed_32< 8 > )b;
			assert( comparefloats( r - ( fixed_32< 8 > )b, a, f16eps ) );
			r = a;
			a *= ( fixed_32< 8 > )b;
			assert( comparefloats( r * ( fixed_32< 8 > )b, a, f16eps ) );
			r = a;
			a /= ( fixed_32< 8 > )b;
			assert( comparefloats( r / ( fixed_32< 8 > )b, a, f16eps ) );
		}
		
		f = VUL_TEST_RNG;
		h = ( half )f;
		half h_old = h;
		assert( comparefloats( ( f32 )( ++h ), ( ( f32 )h_old + 1.f ), f16eps ) );
		assert( comparefloats( ( f32 )( --h ), ( f32 )h_old, f16eps ) );
		assert( comparefloats( ( f32 )( -h - half( -f ) ), 0.f, f16eps ) );
		assert(comparefloats(  ( f32 )( +h - half( +f ) ), 0.f, f16eps ) );

		return true;
	}

	bool TestHalf::masses( )
	{
		f64 *doubles = new f64[ VUL_TEST_FUZZ_COUNT ];
		f32 *floats = new f32[ VUL_TEST_FUZZ_COUNT ];
		half *halfs = new half[ VUL_TEST_FUZZ_COUNT ];
		
		f32 f16eps = std::numeric_limits< half >::epsilon( );

		for( u32 i = 0; i < VUL_TEST_FUZZ_COUNT; ++i ) {
			floats[ i ] = VUL_TEST_RNG;
			doubles[ i ] = ( double )floats[ i ];
		}

		vul_single_to_half_array( halfs, floats, VUL_TEST_FUZZ_COUNT );
		for( u32 i = 0; i < VUL_TEST_FUZZ_COUNT; ++i ) {
			assert( comparefloats( ( f32 )halfs[ i ], floats[ i ], f16eps ) );
		}

		vul_double_to_half_array( halfs, doubles, VUL_TEST_FUZZ_COUNT );
		for( u32 i = 0; i < VUL_TEST_FUZZ_COUNT; ++i ) {
			assert( comparefloats( ( f32 )halfs[ i ], ( f32 )doubles[ i ], f16eps ) );
		}

		// Exhaustive
		half *all_halfs = new half[ 1 << 16 ];
		for( u32 i = 0; i < 1 << 16; ++i ) {
			all_halfs[ i ].data = i;
		}
		f32 *all_halfs_f32s = new f32[ 1 << 16 ];
		f64 *all_halfs_f64s = new f64[ 1 << 16 ];

		vul_half_to_single_array( all_halfs_f32s, all_halfs, 1 << 16 );
		vul_half_to_double_array( all_halfs_f64s, all_halfs, 1 << 16 );
		
		for( u32 i = 0; i < 1 << 16; ++i ) {
			if( ( all_halfs[ i ].data & 0x7c00 ) == 0x7c00 ) continue; // Skip INFs and NaNs
			assert( comparefloats( all_halfs[ i ], all_halfs_f32s[ i ], f16eps ) );
			assert( comparefloats( all_halfs[ i ], ( f32 )all_halfs_f64s[ i ], f16eps ) );
		}

		delete[ ] all_halfs;
		delete[ ] all_halfs_f32s;
		delete[ ] all_halfs_f64s;

		delete[ ] doubles;
		delete[ ] floats;
		delete[ ] halfs;
		
		return true;
	}

	bool TestHalf::comparefloats( float a, float b, float max_rel_diff )
	{
		float d, l;
		
		/* For our exhaustive tests, we want inf to equal inf */
		if( a == INFINITY ) {
			return b == INFINITY;
		}
		/* For our exhaustive tests, we want nan to equal nan */
		if( a == NAN ) {
			return b == NAN;
		}

		d = fabs( a - b );
		a = fabs( a );
		b = fabs( b );
		l = ( b > a ) ? b : a;

		if( d <= l * max_rel_diff ) 
			return true;
		return false;
	}
};

#undef VUL_TEST_RNG
#undef VUL_TEST_FUZZ_COUNT

#endif
