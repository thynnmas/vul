/*
 * Villains' Utility Library - Thomas Martin Schmid, 2014. Public domain¹
 *
 * This file contains tests for the half precision floating point 
 * type in vul_half.hpp. We perform exhaustive testing where we can.
 * We test all three versions for the half-float conversions (VUL_HALF_TABLE,
 * VUL_HALF_SSE and standard).
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
		assert( ( f32_t )t == 1.f );
		
		t = half( 1.f );
		assert( ( f32_t )t == 1.f );
		
		t = ( half )( fixed_32< 16 >( 1.f ) );
		assert( ( f32_t )t == 1.f );
		assert( ( f64_t )t == 1.0 );
		assert( ( fixed_32< 16 > )t == fixed_32< 16 >( 1.f ) );

		t = 1.f;
		assert( ( f32_t )t == 1.f );
		
		half t2 = t;
		assert( ( f32_t )t2 == 1.f );

		t = fixed_32< 16 >( 1.f );
		assert( ( f32_t )t == 1.f );

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
		f32_t f = VUL_TEST_RNG;
		half h, hm;
		h = f;
		hm = -fabs( f );
		f32_t f16eps = std::numeric_limits< half >::epsilon( );
		assert( ( f32_t )fabs( h ) - ( f32_t )fabs( f ) < f16eps );
		assert( ( f32_t )fabs( hm ) - ( f32_t )fabs( h ) < f16eps );
		
		for( ui32_t i = 0; i < VUL_TEST_FUZZ_COUNT; ++i ) {
			half a, b, r;
			
			a = VUL_TEST_RNG;
			b = VUL_TEST_RNG;
			while( ( f32_t )( a * b ) < ( f32_t )f16eps ) {
				a = VUL_TEST_RNG;
				b = VUL_TEST_RNG;
			}

			r = a + b;
			f = ( f32_t )a + ( f32_t )b;			
			assert( ( f32_t )fabs( ( f32_t )r - f ) <= ( f32_t )( f16eps + f16eps * ( f32_t )fabs( r ) ) );
			r = a - b;
			f = ( f32_t )a - ( f32_t )b;
			assert( ( f32_t )fabs( ( f32_t )r - f ) <= ( f32_t )( f16eps + f16eps * ( f32_t )fabs( r ) ) );
			r = a * b;
			f = ( f32_t )a * ( f32_t )b;
			assert( ( f32_t )fabs( ( f32_t )r - f ) <= ( f32_t )( f16eps + f16eps * ( f32_t )fabs( r ) ) );
			r = a / b;
			f = ( f32_t )a / ( f32_t )b;
			assert( ( f32_t )fabs( ( f32_t )r - f ) <= ( f32_t )( f16eps + f16eps * ( f32_t )fabs( r ) ) );
			
			r = a;
			a += b;
			assert( r + b == a );
			r = a;
			a -= b;
			assert( r - b == a );
			r = a;
			a *= b;
			assert( r * b == a );
			r = a;
			a /= b;
			assert( r / b == a );

			r = a;
			a += ( f32_t )b;
			assert( r + b == a );
			r = a;
			a -= ( f32_t )b;
			assert( r - b == a );
			r = a;
			a *= ( f32_t )b;
			assert( r * b == a );
			r = a;
			a /= ( f32_t )b;
			assert( r / b == a );
			
			r = a;
			a += ( fixed_32< 8 > )b;
			assert( r + ( fixed_32< 8 > )b == a );
			r = a;
			a -= ( fixed_32< 8 > )b;
			assert( r - ( fixed_32< 8 > )b == a );
			r = a;
			a *= ( fixed_32< 8 > )b;
			assert( r * ( fixed_32< 8 > )b == a );
			r = a;
			a /= ( fixed_32< 8 > )b;
			assert( r / ( fixed_32< 8 > )b == a );
		}
		
		f = VUL_TEST_RNG;
		h = ( half )f;
		half h_old = h;
		assert( ( f32_t )( ++h ) - ( ( f32_t )h_old + 1.f ) < f16eps );
		assert( ( f32_t )( --h ) - ( f32_t )h_old < f16eps );
		assert( ( f32_t )( -h - half( -f ) ) < f16eps );
		assert( ( f32_t )( +h - half( +f ) ) < f16eps );

		return true;
	}

	bool TestHalf::masses( )
	{
		f64_t doubles[ VUL_TEST_FUZZ_COUNT ];
		f32_t floats[ VUL_TEST_FUZZ_COUNT ];
		half halfs[ VUL_TEST_FUZZ_COUNT ];
		
		f32_t f16eps = std::numeric_limits< half >::epsilon( );

		for( ui32_t i = 0; i < VUL_TEST_FUZZ_COUNT; ++i ) {
			floats[ i ] = VUL_TEST_RNG;
			doubles[ i ] = ( double )floats[ i ];
		}

		vul_single_to_half_array( halfs, floats, VUL_TEST_FUZZ_COUNT );
		for( ui32_t i = 0; i < VUL_TEST_FUZZ_COUNT; ++i ) {
			assert( ( f32_t )fabs( ( f32_t )halfs[ i ] - floats[ i ] ) < ( f32_t )( f16eps + f16eps * ( f32_t )fabs( halfs[ i ] ) ) );
		}

		vul_double_to_half_array( halfs, doubles, VUL_TEST_FUZZ_COUNT );
		for( ui32_t i = 0; i < VUL_TEST_FUZZ_COUNT; ++i ) {
			assert( ( f32_t )fabs( ( f64_t )halfs[ i ] - doubles[ i ] ) < ( f32_t )( f16eps + f16eps * ( f32_t )fabs( halfs[ i ] ) ) );
		}

		// Exhaustive
		half all_halfs[ 1 << 16 ];
		for( ui32_t i = 0; i < 1 << 16; ++i ) {
			all_halfs[ i ].data = i;
		}
		f32_t *all_halfs_f32s = new f32_t[ 1 << 16 ];
		f64_t *all_halfs_f64s = new f64_t[ 1 << 16 ];

		vul_half_to_single_array( all_halfs_f32s, all_halfs, 1 << 16 );
		vul_half_to_double_array( all_halfs_f64s, all_halfs, 1 << 16 );
		
		for( ui32_t i = 1; i < 1 << 16; ++i ) {
			assert( ( f32_t )fabs( ( f32_t )all_halfs[ i ] - all_halfs_f32s[ i ] ) < ( f32_t )( f16eps + f16eps * ( f32_t )fabs( all_halfs[ i ] ) ) );
			assert( ( f32_t )fabs( ( f64_t )all_halfs[ i ] - all_halfs_f64s[ i ] ) < ( f32_t )( f16eps + f16eps * ( f32_t )fabs( all_halfs[ i ] ) ) );		
		}

		delete [] all_halfs_f32s;
		delete [] all_halfs_f64s;

		return true;
	}
};

#undef VUL_TEST_RNG
#undef VUL_TEST_FUZZ_COUNT

#endif
