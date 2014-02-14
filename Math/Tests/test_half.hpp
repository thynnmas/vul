/*
 * Villains' Utility Library - Thomas Martin Schmid, 2014. Public domain¹
 *
 * This file contains tests for the half precision floating point 
 * type in vul_half.hpp. We perform exhaustive testing where we can.
 * We test all three versions for the half-float conversions (VUL_HALF_TABLE,
 * VUL_HALF_SSE and standard).
 * 
 * ¹ If public domain is not legally valid in your country and or legal area,
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
#define VUL_TEST_RNG ( float )( ( float )rand( ) / ( float )std::numeric_limits< half >::max( ) )

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
		half h = f, hm = -abs( f );
		assert( abs( h ) == abs( f ) );
		assert( abs( hm ) == abs( h ) );
		
		for( ui32_t i = 0; i < VUL_TEST_FUZZ_COUNT; ++i ) {
			half a, b, r;
			
			a = VUL_TEST_RNG;
			b = VUL_TEST_RNG;

			r = a + b;
			f = ( f32_t )a + ( f32_t )b;
			assert( abs( r - f ) < ( 1e-5 + 1e-5 * abs( r ) );
			r = a - b;
			f = ( f32_t )a - ( f32_t )b;
			assert( abs( r - f ) < ( 1e-5 + 1e-5 * abs( r ) );
			r = a * b;
			f = ( f32_t )a * ( f32_t )b;
			assert( abs( r - f ) < ( 1e-5 + 1e-5 * abs( r ) );
			r = a / b;
			f = ( f32_t )a / ( f32_t )b;
			assert( abs( r - f ) < ( 1e-5 + 1e-5 * abs( r ) );
			
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
		
		assert( ++h == f + 1 );
		assert( --h == f );
		assert( -h == half( -f ) );
		assert( +h == half( +f ) );

		return true;
	}

	bool TestHalf::masses( )
	{
		f64_t doubles[ VUL_TEST_FUZZ_COUNT ];
		f32_t floats[ VUL_TEST_FUZZ_COUNT ];
		half halfs[ VUL_TEST_FUZZ_COUNT ];

		for( ui32_t i = 0; i < VUL_TEST_FUZZ_COUNT; ++i ) {
			floats[ i ] = VUL_TEST_RNG;
			doubles[ i ] = ( double )floats[ i ];
		}

		vul_single_to_half_array( halfs, floats, VUL_TEST_FUZZ_COUNT );
		for( ui32_t i = 0; i < VUL_TEST_FUZZ_COUNT; ++i ) {
			assert( abs( ( f32_t )halfs[ i ] - floats[ i ] ) < ( 1e-5 + 1e-5 * abs( r )
		}

		vul_double_to_half_array( halfs, doubles, VUL_TEST_FUZZ_COUNT );
		for( ui32_t i = 0; i < VUL_TEST_FUZZ_COUNT; ++i ) {
			assert( abs( ( f64_t )halfs[ i ] - doubles[ i ] ) < ( 1e-5 + 1e-5 * abs( r )
		}

		// Exhaustive
		half all_halfs[ 1 << 16 ];
		for( ui32_t i = 0; i < 1 << 16; ++i ) {
			all_halfs[ i ].data = i;
		}

		vul_half_to_single_array( floats, all_halfs, 1 << 16 );
		vul_half_to_double_array( doubles, all_halfs, 1 << 16 );
		
		assert( abs( ( f32_t )halfs[ 0 ] - floats[ 0 ] ) < ( 1e-5 + 1e-5 * abs( r )
		assert( abs( ( f64_t )halfs[ 0 ] - doubles[ 0 ] ) < ( 1e-5 + 1e-5 * abs( r )
		for( ui32_t i = 1; i < 1 << 16; ++i ) {
			assert( abs( halfs[ i ] - halfs[ i - 1 ] ) <= 1e-5 + 1e-5 * abs( halfs[ i ] ) );
			assert( abs( ( f32_t )halfs[ i ] - floats[ i ] ) < ( 1e-5 + 1e-5 * abs( r )
			assert( abs( ( f64_t )halfs[ i ] - doubles[ i ] ) < ( 1e-5 + 1e-5 * abs( r )
		}
	}
};

#endif