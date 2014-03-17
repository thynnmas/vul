/*
 * Villains' Utility Library - Thomas Martin Schmid, 2014. Public domain¹
 *
 * This file contains tests for the fixed point type in vul_fixed.hpp
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
#ifndef VUL_TEST_FIXED_HPP
#define VUL_TEST_FIXED_HPP

#include <cassert>

#include "../vul_math.hpp"

#define VUL_TEST_FUZZ_COUNT 10000
#define VUL_TEST_RNG ( float )( ( float )rand( ) / ( float )RAND_MAX )

using namespace vul;

namespace vul_test {
	class TestFixed {

	public:
		static bool test( );

	private:
		static bool sets( );
		static bool compares( );
		static bool casts( );
		static bool ops( );
	};

	bool TestFixed::test( )
	{
		assert( sets( ) );
		assert( compares( ) );
		assert( casts( ) );
		assert( ops( ) );
		
		return true;
	}

	bool TestFixed::sets( )
	{
		fixed_32< 16 > t( 1.0 );
		assert( t.data == 1 << 16 );
		
		t = fixed_32< 16 >( 1.5f );
		assert( t.data == ( 1 << 16 ) + ( 1 << 15 ) );
		
		t = fixed_32< 16 >( ( half )1.f );
		assert( t.data == 1 << 16 );
		
		fixed_32< 8 > t2( t );
		assert( t2.data == 1 << 8 );

		t = 1.f;
		assert( t.data == 1 << 16 );
		
		t = 1.0;
		assert( t.data == 1 << 16 );
		
		t = fixed_32< 16 >( 2.f );
		assert( t.data == 1 << 17 );

		return true;
	}
	bool TestFixed::compares( )
	{		
		fixed_32< 16 > a( 1.f ), b( 2.f ), c( 1.f );

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
	bool TestFixed::casts( )
	{		
		fixed_32< 16 > t( 1.f );
		assert( ( float )t == 1.f );
		assert( ( double )t == 1.0 );
		assert( ( half )t == ( half )1.f );
		assert( ( ( fixed_32< 8 > )t).data == 1 << 8 );

		return true;
	}
	bool TestFixed::ops( )
	{
		
		f32_t f = VUL_TEST_RNG, 
			  f32eps = 1e-5f;
		while( abs( f ) >= 1 << ( 32 - 16 ) ) {
			f = VUL_TEST_RNG;
		}
		fixed_32< 16 > fi( f ), fim( -( ( f32_t )abs( f ) ) );
		assert( abs( ( f32_t )abs( ( f32_t )fi ) - ( f32_t )abs( f ) ) < f32eps);
		assert( abs( ( f32_t )abs( ( f32_t )fim ) - ( f32_t )abs( ( f32_t )fi ) ) < f32eps );
		
		for( ui32_t i = 0; i < VUL_TEST_FUZZ_COUNT; ++i ) {
			fixed_32< 24 > f24_1, f24_2, r24, old24;
			fixed_32< 16 > f16_1, f16_2, r16, old16;
			fixed_32< 10 > f10_1, f10_2, r10, old10;
			
			f32_t tmp = VUL_TEST_RNG, rf;
			while( fabs( tmp ) >= ( f32_t )( 1 << ( 32 - 10 ) ) ) {
				tmp = VUL_TEST_RNG;
			}
			f10_1 = tmp;
			while( fabs( tmp ) >= ( f32_t )( 1 << ( 32 - 10 ) ) || fabs( tmp ) <= std::pow( 2.f, -10 ) ) { // Make sure we don't div by 0
				tmp = VUL_TEST_RNG;
			}
			f10_2 = tmp;
			
			while( fabs( tmp ) >= ( f32_t )( 1 << ( 32 - 16 ) ) ) {
				tmp = VUL_TEST_RNG;
			}
			f16_1 = tmp;

			while( fabs( tmp ) >= ( f32_t )( 1 << ( 32 - 16 ) ) || fabs( tmp ) <= std::pow( 2.f, -16 ) ) { // Make sure we don't div by 0
				tmp = VUL_TEST_RNG;
			}
			f16_2 = tmp;
			
			while( fabs( tmp ) >= ( f32_t )( 1 << ( 32 - 24 ) ) ) {
				tmp = VUL_TEST_RNG;
			}
			f24_1 = tmp;

			while( fabs( tmp ) >= ( f32_t )( 1 << ( 32 - 24 ) ) || fabs( tmp ) <= std::pow( 2.f, -24 ) ) { // Make sure we don't div by 0
				tmp = VUL_TEST_RNG;
			}
			f24_2 = tmp;

			r10 = f10_1 + f10_2;
			rf = ( f32_t )f10_1 + ( f32_t )f10_2;
			assert( ( f32_t )abs( r10 ) > ( f32_t )( 1 << ( 32 - 10 ) )
				 || ( f32_t )abs( r10 ) < ( f32_t )( 1.f / pow( 2.f, 10 ) )
				 || ( f32_t )abs( ( f32_t )r10 - rf ) < ( ( 1.f / pow( 2.f, 10 ) ) + f32eps * ( f32_t )abs( rf ) ) );
			r16 = f16_1 + f16_2;
			rf = ( f32_t )f16_1 + ( f32_t )f16_2;
			assert( ( f32_t )abs( r16 ) > ( f32_t )( 1 << ( 32 - 16 ) )
				 || ( f32_t )abs( r16 ) < ( f32_t )( 1.f / pow( 2.f, 16 ) )
				 || ( f32_t )abs( ( f32_t )r16 - rf ) < ( ( 1.f / pow( 2.f, 16 ) ) + f32eps * ( f32_t )abs( rf ) ) );
			r24 = f24_1 + f24_2;
			rf = ( f32_t )f24_1 + ( f32_t )f24_2;
			assert( ( f32_t )abs( r24 ) > ( f32_t )( 1 << ( 32 - 24 ) )
				 || ( f32_t )abs( r24 ) < ( f32_t )( 1.f / pow( 2.f, 24 ) )
				 || ( f32_t )abs( ( f32_t )r24 - rf ) < ( ( 1.f / pow( 2.f, 24 ) ) + f32eps * ( f32_t )abs( rf ) ) );

			
			r10 = f10_1 - f10_2;
			rf = ( f32_t )f10_1 - ( f32_t )f10_2;
			assert( ( f32_t )abs( r10 ) > ( f32_t )( 1 << ( 32 - 10 ) )
				 || ( f32_t )abs( r10 ) < ( f32_t )( 1.f / pow( 2.f, 10 ) )
				 || ( f32_t )abs( ( f32_t )r10 - rf ) < ( ( 1.f / pow( 2.f, 10 ) ) + f32eps * ( f32_t )abs( rf ) ) );
			r16 = f16_1 - f16_2;
			rf = ( f32_t )f16_1 - ( f32_t )f16_2;
			assert( ( f32_t )abs( r16 ) > ( f32_t )( 1 << ( 32 - 16 ) )
				 || ( f32_t )abs( r16 ) < ( f32_t )( 1.f / pow( 2.f, 16 ) )
				 || ( f32_t )abs( ( f32_t )r16 - rf ) < ( ( 1.f / pow( 2.f, 16 ) ) + f32eps * ( f32_t )abs( rf ) ) );
			r24 = f24_1 - f24_2;
			rf = ( f32_t )f24_1 - ( f32_t )f24_2;
			assert( ( f32_t )abs( r24 ) > ( f32_t )( 1 << ( 32 - 24 ) )
				 || ( f32_t )abs( r24 ) < ( f32_t )( 1.f / pow( 2.f, 24 ) )
				 || ( f32_t )abs( ( f32_t )r24 - rf ) < ( ( 1.f / pow( 2.f, 24 ) ) + f32eps * ( f32_t )abs( rf ) ) );
			
			r10 = f10_1 * f10_2;
			rf = ( f32_t )f10_1 * ( f32_t )f10_2;
			assert( ( f32_t )abs( r10 ) > ( f32_t )( 1 << ( 32 - 10 ) )
				 || ( f32_t )abs( r10 ) < ( f32_t )( 1.f / pow( 2.f, 10 ) )
				 || ( f32_t )abs( ( f32_t )r10 - rf ) < ( ( 1.f / pow( 2.f, 10 ) ) + f32eps * ( f32_t )abs( rf ) ) );
			r16 = f16_1 * f16_2;
			rf = ( f32_t )f16_1 * ( f32_t )f16_2;
			assert( ( f32_t )abs( r16 ) > ( f32_t )( 1 << ( 32 - 16 ) )
				 || ( f32_t )abs( r16 ) < ( f32_t )( 1.f / pow( 2.f, 16 ) )
				 || ( f32_t )abs( ( f32_t )r16 - rf ) < ( ( 1.f / pow( 2.f, 16 ) ) + f32eps * ( f32_t )abs( rf ) ) );
			r24 = f24_1 * f24_2;
			rf = ( f32_t )f24_1 * ( f32_t )f24_2;
			assert( ( f32_t )abs( r24 ) > ( f32_t )( 1 << ( 32 - 24 ) )
				 || ( f32_t )abs( r24 ) < ( f32_t )( 1.f / pow( 2.f, 24 ) )
				 || ( f32_t )abs( ( f32_t )r24 - rf ) < ( ( 1.f / pow( 2.f, 24 ) ) + f32eps * ( f32_t )abs( rf ) ) );

			
			r10 = f10_1 / f10_2;
			rf = ( f32_t )f10_1 / ( f32_t )f10_2;
			assert( ( f32_t )abs( r10 ) > ( f32_t )( 1 << ( 32 - 10 ) )
				 || ( f32_t )abs( r10 ) < ( f32_t )( 1.f / pow( 2.f, 10 ) )
				 || ( f32_t )abs( ( f32_t )r10 - rf ) < ( ( 1.f / pow( 2.f, 10 ) ) + f32eps * ( f32_t )abs( rf ) ) );
			r16 = f16_1 / f16_2;
			rf = ( f32_t )f16_1 / ( f32_t )f16_2;
			assert( ( f32_t )abs( r16 ) > ( f32_t )( 1 << ( 32 - 16 ) )
				 || ( f32_t )abs( r16 ) < ( f32_t )( 1.f / pow( 2.f, 16 ) )
				 || ( f32_t )abs( ( f32_t )r16 - rf ) < ( ( 1.f / pow( 2.f, 16 ) ) + f32eps * ( f32_t )abs( rf ) ) );
			r24 = f24_1 / f24_2;
			rf = ( f32_t )f24_1 / ( f32_t )f24_2;
			assert( ( f32_t )abs( r24 ) > ( f32_t )( 1 << ( 32 - 24 ) )
				 || ( f32_t )abs( r24 ) < ( f32_t )( 1.f / pow( 2.f, 24 ) )
				 || ( f32_t )abs( ( f32_t )r24 - rf ) < ( ( 1.f / pow( 2.f, 24 ) ) + f32eps * ( f32_t )abs( rf ) ) );

			old10 = f10_1;
			f10_1 += f10_2;
			assert( f10_1 == old10 + f10_2 );
			old16 = f16_1;
			f16_1 += f16_2;
			assert( f16_1 == old16 + f16_2 );
			old24 = f24_1;
			f24_1 += f24_2;
			assert( f24_1 == old24 + f24_2 );

			old10 = f10_1;
			f10_1 -= f10_2;
			assert( f10_1 == old10 - f10_2 );
			old16 = f16_1;
			f16_1 -= f16_2;
			assert( f16_1 == old16 - f16_2 );
			old24 = f24_1;
			f24_1 -= f24_2;
			assert( f24_1 == old24 - f24_2 );

			old10 = f10_1;
			f10_1 *= f10_2;
			assert( f10_1 == old10 * f10_2 );
			old16 = f16_1;
			f16_1 *= f16_2;
			assert( f16_1 == old16 * f16_2 );
			old24 = f24_1;
			f24_1 *= f24_2;
			assert( f24_1 == old24 * f24_2 );

			old10 = f10_1;
			f10_1 /= f10_2;
			assert( f10_1 == old10 / f10_2 );
			old16 = f16_1;
			f16_1 /= f16_2;
			assert( f16_1 == old16 / f16_2 );
			old24 = f24_1;
			f24_1 /= f24_2;
			assert( f24_1 == old24 / f24_2 );

			old10 = f10_1;
			f10_1 += ( f32_t )f10_2;
			assert( ( f32_t )abs( old10 + f10_2 - f10_1 ) < ( 1.f / pow( 2.f, 10 ) + 1e-5 * ( f32_t )abs( f10_1 ) ) );
			old16 = f16_1;
			f16_1 += ( f32_t )f16_2;
			assert( ( f32_t )abs( old16 + f16_2 - f16_1 ) < ( 1.f / pow( 2.f, 16 ) + 1e-5 * ( f32_t )abs( f16_1 ) ) );
			old24 = f24_1;
			f24_1 += ( f32_t )f24_2;
			assert( ( f32_t )abs( old24 + f24_2 - f24_1 ) < ( 1.f / pow( 2.f, 24 ) + 1e-5 * ( f32_t )abs( f24_1 ) ) );

			old10 = f10_1;
			f10_1 -= ( f32_t )f10_2;
			assert( ( f32_t )abs( ( old10 - f10_2 ) - f10_1 ) < ( 1.f / pow( 2.f, 10 ) + 1e-5 * ( f32_t )abs( f10_1 ) ) );
			old16 = f16_1;
			f16_1 -= ( f32_t )f16_2;
			assert( ( f32_t )abs( ( old16 - f16_2 ) - f16_1 ) < ( 1.f / pow( 2.f, 16 ) + 1e-5 * ( f32_t )abs( f16_1 ) ) );
			old24 = f24_1;
			f24_1 -= ( f32_t )f24_2;
			assert( ( f32_t )abs( ( old24 - f24_2 ) - f24_1 ) < ( 1.f / pow( 2.f, 24 ) + 1e-5 * ( f32_t )abs( f24_1 ) ) );

			old10 = f10_1;
			f10_1 *= ( f32_t )f10_2;
			assert( ( f32_t )abs( old10 * f10_2 - f10_1 ) < ( 1.f / pow( 2.f, 10 ) + 1e-5 * ( f32_t )abs( f10_1 ) ) );
			old16 = f16_1;
			f16_1 *= ( f32_t )f16_2;
			assert( ( f32_t )abs( old16 * f16_2 - f16_1 ) < ( 1.f / pow( 2.f, 16 ) + 1e-5 * ( f32_t )abs( f16_1 ) ) );
			old24 = f24_1;
			f24_1 *= ( f32_t )f24_2;
			assert( ( f32_t )abs( old24 * f24_2 - f24_1 ) < ( 1.f / pow( 2.f, 24 ) + 1e-5 * ( f32_t )abs( f24_1 ) ) );

			old10 = f10_1;
			f10_1 /= ( f32_t )f10_2;
			assert( ( f32_t )abs( old10 / f10_2 - f10_1 ) < ( 1.f / pow( 2.f, 10 ) + 1e-5 * ( f32_t )abs( f10_1 ) ) );
			old16 = f16_1;
			f16_1 /= ( f32_t )f16_2;
			assert( ( f32_t )abs( old16 / f16_2 - f16_1 ) < ( 1.f / pow( 2.f, 16 ) + 1e-5 * ( f32_t )abs( f16_1 ) ) );
			old24 = f24_1;
			f24_1 /= ( f32_t )f24_2;
			assert( ( f32_t )abs( old24 / f24_2 - f24_1 ) < ( 1.f / pow( 2.f, 24 ) + 1e-5 * ( f32_t )abs( f24_1 ) ) );

			old10 = f10_1;
			f10_1 += ( f64_t )f10_2;
			assert( ( f32_t )abs( old10 + f10_2 - f10_1 ) < ( 1.f / pow( 2.f, 10 ) + 1e-5 * ( f32_t )abs( f10_1 ) ) );
			old16 = f16_1;
			f16_1 += ( f64_t )f16_2;
			assert( ( f32_t )abs( old16 + f16_2 - f16_1 ) < ( 1.f / pow( 2.f, 16 ) + 1e-5 * ( f32_t )abs( f16_1 ) ) );
			old24 = f24_1;
			f24_1 += ( f64_t )f24_2;
			assert( ( f32_t )abs( old24 + f24_2 - f24_1 ) < ( 1.f / pow( 2.f, 24 ) + 1e-5 * ( f32_t )abs( f24_1 ) ) );

			old10 = f10_1;
			f10_1 -= ( f64_t )f10_2;
			assert( ( f32_t )abs( ( old10 - f10_2 ) - f10_1 ) < ( 1.f / pow( 2.f, 10 ) + 1e-5 * ( f32_t )abs( f10_1 ) ) );
			old16 = f16_1;
			f16_1 -= ( f64_t )f16_2;
			assert( ( f32_t )abs( ( old16 - f16_2 ) - f16_1 ) < ( 1.f / pow( 2.f, 16 ) + 1e-5 * ( f32_t )abs( f16_1 ) ) );
			old24 = f24_1;
			f24_1 -= ( f64_t )f24_2;
			assert( ( f32_t )abs( ( old24 - f24_2 ) - f24_1 ) < ( 1.f / pow( 2.f, 24 ) + 1e-5 * ( f32_t )abs( f24_1 ) ) );

			old10 = f10_1;
			f10_1 *= ( f64_t )f10_2;
			assert( ( f32_t )abs( old10 * f10_2 - f10_1 ) < ( 1.f / pow( 2.f, 10 ) + 1e-5 * ( f32_t )abs( f10_1 ) ) );
			old16 = f16_1;
			f16_1 *= ( f64_t )f16_2;
			assert( ( f32_t )abs( old16 * f16_2 - f16_1 ) < ( 1.f / pow( 2.f, 16 ) + 1e-5 * ( f32_t )abs( f16_1 ) ) );
			old24 = f24_1;
			f24_1 *= ( f64_t )f24_2;
			assert( ( f32_t )abs( old24 * f24_2 - f24_1 ) < ( 1.f / pow( 2.f, 24 ) + 1e-5 * ( f32_t )abs( f24_1 ) ) );

			old10 = f10_1;
			f10_1 /= ( f64_t )f10_2;
			assert( ( f32_t )abs( old10 / f10_2 - f10_1 ) < ( 1.f / pow( 2.f, 10 ) + 1e-5 * ( f32_t )abs( f10_1 ) ) );
			old16 = f16_1;
			f16_1 /= ( f64_t )f16_2;
			assert( ( f32_t )abs( old16 / f16_2 - f16_1 ) < ( 1.f / pow( 2.f, 16 ) + 1e-5 * ( f32_t )abs( f16_1 ) ) );
			old24 = f24_1;
			f24_1 /= ( f64_t )f24_2;
			assert( ( f32_t )abs( old24 / f24_2 - f24_1 ) < ( 1.f / pow( 2.f, 24 ) + 1e-5 * ( f32_t )abs( f24_1 ) ) );
		}
		
		fixed_32< 16 > fi_old = fi;
		assert( ( f32_t )++fi == ( f32_t )fi_old + 1.f );
		assert( --fi == fi_old );
		assert( -fi == fixed_32< 16 >( -f ) );
		assert( +fi == fixed_32< 16 >( +f ) );

		return true;
	}
};

#undef VUL_TEST_RNG
#undef VUL_TEST_FUZZ_COUNT

#endif
