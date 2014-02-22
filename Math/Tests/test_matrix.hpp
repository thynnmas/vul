/*
 * Villains' Utility Library - Thomas Martin Schmid, 2014. Public domain¹
 *
 * This file contains tests for the matrix struct in vul_matrix.hpp
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
#ifndef VUL_TEST_MATRIX_HPP
#define VUL_TEST_MATRIX_HPP

#include <cassert>

#include "../vul_math.hpp"
#include "glm.hpp"

#define VUL_TEST_FUZZ_COUNT 10000
#define VUL_TEST_RNG ( float )( ( float )rand( ) / ( float )RAND_MAX )

using namespace vul;

namespace vul_test {
	class TestMatrix {

	public:
		static bool test( );

	private:
		static bool make( );
		static bool member_ops( );
		static bool comparisons( );
		static bool ops( );
		static bool functions( );
		//static bool specializations( );
	};

	bool TestMatrix::test( )
	{
		assert( make( ) );
		assert( comparisons( ) );
		assert( member_ops( ) );
		assert( ops( ) );
		assert( functions( ) );
		//assert( specializations( ) );

		return true;
	}

	bool TestMatrix::make( )
	{		
#ifdef VUL_CPLUSPLUS11
		Matrix< f32_t, 2, 2> m22( );
		Matrix< fi32_t, 3, 3> m33( );
		Matrix< i64_t, 4, 4> m44( );
		Matrix< f32_t, 6, 8> m68( );
#else
		Matrix< f32_t, 2, 2> m22 = makeMatrix< f32_t, 2, 2 >( );
		Matrix< fi32_t, 3, 3> m33 = makeMatrix< fi32_t, 3, 3 >( );
		Matrix< i64_t, 4, 4> m44 = makeMatrix< i64_t, 4, 4 >( );
		Matrix< f32_t, 6, 8> m68 = makeMatrix< f32_t, 6, 8 >( );
#endif
		assert( m22( 0, 0 ) == m22( 1, 0 ) == m22( 0, 1 ) == m22( 1, 1 ) == 0.f );
		assert( m33( 0, 0 ) == fi32_t( 0.f ) );
		assert( m33( 1, 0 ) == fi32_t( 0.f ) );
		assert( m33( 2, 0 ) == fi32_t( 0.f ) );
		assert( m33( 0, 1 ) == fi32_t( 0.f ) );
		assert( m33( 1, 1 ) == fi32_t( 0.f ) );
		assert( m33( 2, 1 ) == fi32_t( 0.f ) );
		assert( m33( 0, 2 ) == fi32_t( 0.f ) );
		assert( m33( 1, 2 ) == fi32_t( 0.f ) );
		assert( m33( 2, 2 ) == fi32_t( 0.f ) );
		assert( m44( 0, 0 ) == m44( 1, 0 ) == m44( 2, 0 ) == m44( 3, 0 ) == 
				m44( 0, 1 ) == m44( 1, 1 ) == m44( 2, 1 ) == m44( 3, 1 ) == 
				m44( 0, 2 ) == m44( 1, 2 ) == m44( 2, 2 ) == m44( 3, 2 ) ==
				m44( 0, 3 ) == m44( 1, 3 ) == m44( 2, 3 ) == m44( 3, 3 ) == 0L );
		assert( m68( 0, 0 ) == m68( 1, 0 ) == m68( 2, 0 ) == m44( 3, 0 ) == m44( 4, 0 ) == m44( 5, 0 ) == m44( 6, 0 ) == m44( 7, 0 ) ==
				m68( 0, 1 ) == m68( 1, 1 ) == m68( 2, 1 ) == m68( 3, 1 ) == m44( 4, 1 ) == m44( 5, 1 ) == m44( 6, 1 ) == m44( 7, 1 ) ==
				m68( 0, 2 ) == m68( 1, 2 ) == m68( 2, 2 ) == m68( 3, 2 ) == m44( 4, 2 ) == m44( 5, 2 ) == m44( 6, 2 ) == m44( 7, 2 ) ==
				m68( 0, 3 ) == m68( 1, 3 ) == m68( 2, 3 ) == m68( 3, 3 ) == m44( 4, 3 ) == m44( 5, 3 ) == m44( 6, 3 ) == m44( 7, 3 ) ==
				m68( 0, 4 ) == m68( 1, 4 ) == m68( 2, 4 ) == m68( 3, 4 ) == m44( 4, 4 ) == m44( 5, 4 ) == m44( 6, 4 ) == m44( 7, 4 ) ==
				m68( 0, 5 ) == m68( 1, 5 ) == m68( 2, 5 ) == m68( 3, 5 ) == m44( 4, 5 ) == m44( 5, 5 ) == m44( 6, 5 ) == m44( 7, 5 ) == 0.f );
		
		// Repeat for all, but m22 and m68 should be fine
#ifdef VUL_CPLUSPLUS11
		m22 = Matrix< f32_t, 2, 2 >( 2.f );
		m68 = Matrix< f32_t, 6, 8 >( -1.f );
#else
		m22 = makeMatrix< f32_t, 2, 2 >( 2.f );
		m68 = makeMatrix< f32_t, 6, 8 >( -1.f );
#endif
		assert( m22( 0, 0 ) == m22( 1, 0 ) == m22( 0, 1 ) == m22( 1, 1 ) == 2.f );
		assert( m68( 0, 0 ) == m68( 1, 0 ) == m68( 2, 0 ) == m44( 3, 0 ) == m44( 4, 0 ) == m44( 5, 0 ) == m44( 6, 0 ) == m44( 7, 0 ) ==
				m68( 0, 1 ) == m68( 1, 1 ) == m68( 2, 1 ) == m68( 3, 1 ) == m44( 4, 1 ) == m44( 5, 1 ) == m44( 6, 1 ) == m44( 7, 1 ) ==
				m68( 0, 2 ) == m68( 1, 2 ) == m68( 2, 2 ) == m68( 3, 2 ) == m44( 4, 2 ) == m44( 5, 2 ) == m44( 6, 2 ) == m44( 7, 2 ) ==
				m68( 0, 3 ) == m68( 1, 3 ) == m68( 2, 3 ) == m68( 3, 3 ) == m44( 4, 3 ) == m44( 5, 3 ) == m44( 6, 3 ) == m44( 7, 3 ) ==
				m68( 0, 4 ) == m68( 1, 4 ) == m68( 2, 4 ) == m68( 3, 4 ) == m44( 4, 4 ) == m44( 5, 4 ) == m44( 6, 4 ) == m44( 7, 4 ) ==
				m68( 0, 5 ) == m68( 1, 5 ) == m68( 2, 5 ) == m68( 3, 5 ) == m44( 4, 5 ) == m44( 5, 5 ) == m44( 6, 5 ) == m44( 7, 5 ) == -1.f );

#ifdef VUL_CPLUSPLUS11
		m22 = Matrix< f32_t, 2, 2 >( Matrix< f32_t, 2, 2 >( ) );
		m68 = Matrix< f32_t, 6, 8 >( Matrix< f32_t, 6, 8 >( ) );
#else
		m22 = makeMatrix< f32_t, 2, 2 >( makeMatrix< f32_t, 2, 2 >( ) );
		m68 = makeMatrix< f32_t, 6, 8 >( makeMatrix< f32_t, 6, 8 >( ) );
#endif
		assert( m22( 0, 0 ) == m22( 1, 0 ) == m22( 0, 1 ) == m22( 1, 1 ) == 0.f );
		assert( m68( 0, 0 ) == m68( 1, 0 ) == m68( 2, 0 ) == m44( 3, 0 ) == m44( 4, 0 ) == m44( 5, 0 ) == m44( 6, 0 ) == m44( 7, 0 ) ==
				m68( 0, 1 ) == m68( 1, 1 ) == m68( 2, 1 ) == m68( 3, 1 ) == m44( 4, 1 ) == m44( 5, 1 ) == m44( 6, 1 ) == m44( 7, 1 ) ==
				m68( 0, 2 ) == m68( 1, 2 ) == m68( 2, 2 ) == m68( 3, 2 ) == m44( 4, 2 ) == m44( 5, 2 ) == m44( 6, 2 ) == m44( 7, 2 ) ==
				m68( 0, 3 ) == m68( 1, 3 ) == m68( 2, 3 ) == m68( 3, 3 ) == m44( 4, 3 ) == m44( 5, 3 ) == m44( 6, 3 ) == m44( 7, 3 ) ==
				m68( 0, 4 ) == m68( 1, 4 ) == m68( 2, 4 ) == m68( 3, 4 ) == m44( 4, 4 ) == m44( 5, 4 ) == m44( 6, 4 ) == m44( 7, 4 ) ==
				m68( 0, 5 ) == m68( 1, 5 ) == m68( 2, 5 ) == m68( 3, 5 ) == m44( 4, 5 ) == m44( 5, 5 ) == m44( 6, 5 ) == m44( 7, 5 ) == 0.f );

		// For this we need to test a non-f32 or i32 type, so we only test the m33 (fi32_t)
		fi32_t a33[ 9 ];
		for( ui32_t i = 0; i < 9; ++i ) {
			a33[ i ] = fi32_t( ( f32_t )rand( ) / ( f32_t )FLT_MAX );
		}
#ifdef VUL_CPLUSPLUS11
		m33 = Matrix< fi32_t, 3, 3 >( a33 );
#else
		m33 = makeMatrix< fi32_t, 3, 3 >( a33 );
#endif
		assert( m33( 0, 0 ) == a33[ 0 ] );
		assert( m33( 1, 0 ) == a33[ 1 ] );
		assert( m33( 2, 0 ) == a33[ 2 ] );
		assert( m33( 0, 1 ) == a33[ 3 ] );
		assert( m33( 1, 1 ) == a33[ 4 ] );
		assert( m33( 2, 1 ) == a33[ 5 ] );
		assert( m33( 0, 2 ) == a33[ 6 ] );
		assert( m33( 1, 2 ) == a33[ 7 ] );
		assert( m33( 2, 2 ) == a33[ 8 ] );
		
		f32_t af[ 6 * 8 ];
		for( ui32_t i = 0; i < 6 * 8; ++i ) {
			af[ i ] = ( f32_t )rand( ) / ( f32_t )FLT_MAX;
		}		
#ifdef VUL_CPLUSPLUS11
		m22 = Matrix< f32_t, 2, 2 >( af );
		m68 = Matrix< f32_t, 6, 8 >( af );
#else
		m22 = makeMatrix< f32_t, 2, 2 >( af );
		m68 = makeMatrix< f32_t, 6, 8 >( af );
#endif
		assert( m22( 0, 0 ) == af[ 0 ] );
		assert( m22( 1, 0 ) == af[ 1 ] );
		assert( m22( 0, 1 ) == af[ 2 ] );
		assert( m22( 1, 1 ) == af[ 3 ] );
		for( ui32_t c = 0; c < 8; ++c ) {
			for( ui32_t r = 0; r < 6; ++r ) {
				assert( m68( c, r ) == af[ c * 8 + r ] );
			}
		}
		
		i64_t ai[ 4 * 4 ];
		for( ui32_t i = 0; i < 6 * 8; ++i ) {
			ai[ i ] = ( i64_t )rand( );
		}


#ifdef VUL_CPLUSPLUS11
		m44 = Matrix< i64_t, 4, 4 >( ai );
#else
		m44 = makeMatrix< i64_t, 4, 4 >( ai );
#endif
		for( ui32_t c = 0; c < 4; ++c ) {
			for( ui32_t r = 0; r < 4; ++r ) {
				assert( m44( c, r ) == ai[ c * 8 + r ] );
			}
		}

		// Only available in C++11
#ifdef VUL_CPLUSPLUS11
		Matrix< f32_t, 2, 2 > c11m22 = { 1.f, -2.f, 4.f, 3.f };
		assert( c11m22( 0, 0 ) == 1.f );
		assert( c11m22( 1, 0 ) == -2.f );
		assert( c11m22( 0, 1 ) == 4.f );
		assert( c11m22( 1, 1 ) == 3.f );
#endif

		m22 = makeIdentity< f32_t, 2 >( );
		m44 = makeIdentity< i64_t, 4 >( );
		for( ui32_t r = 0; r < 2; ++r ) {
			for( ui32_t c = 0; c < 2; ++c ) {
				if( c == r ) {
					assert( m22( c, r )	== 1.f );
					assert( m44( c, r )	== 1L );
				} else {
					assert( m22( c, r )	== 0.f );
					assert( m44( c, r )	== 0L );
				}
			}
		}


		m22 = makeMatrix22< f32_t >( 1.f, -2.f, 4.f, 3.f );
		assert( m22( 0, 0 ) == 1.f );
		assert( m22( 0, 1 ) == -2.f );
		assert( m22( 1, 0 ) == 4.f );
		assert( m22( 1, 1 ) == 3.f );

		m33 = makeMatrix33< fi32_t >( fi32_t( -4.f ), fi32_t( -3.f ), fi32_t( -2.f ),
									  fi32_t( -1.f ), fi32_t(  0.f ), fi32_t(  1.f ),
									  fi32_t(  2.f ), fi32_t(  3.f ), fi32_t(  4.f ) );
		assert( m33( 0, 0 ) == fi32_t( -4.f ) );
		assert( m33( 0, 1 ) == fi32_t( -3.f ) );
		assert( m33( 0, 2 ) == fi32_t( -2.f ) );
		assert( m33( 1, 0 ) == fi32_t( -1.f ) );
		assert( m33( 1, 1 ) == fi32_t(  0.f ) );
		assert( m33( 1, 2 ) == fi32_t(  1.f ) );
		assert( m33( 2, 0 ) == fi32_t(  2.f ) );
		assert( m33( 2, 1 ) == fi32_t(  3.f ) );
		assert( m33( 2, 2 ) == fi32_t(  4.f ) );

		m44 = makeMatrix44< i64_t >( 1L,   2L,  3L,  4L,
									 5L,   6L,  7L,  8L,
									 9L,  10L, 11L, 12L,
									 13L, 14L, 15L, 16L );
		for( ui32_t c = 0; c < 4; ++c ) {
			for( ui32_t r = 0; r < 4; ++r ) {
				assert( m44( c, r ) == ( i64_t )( c * 4 + r + 1 ) );
			}
		}
		
		Vector< f32_t, 8 > cols[ 6 ];
		Vector< f32_t, 6 > rows[ 8 ];
		for( ui32_t i = 0; i < 8; ++i ) {
			for( ui32_t j = 0; j < 6; ++j ) {
				rows[ i ][ j ] = ( f32_t )rand( ) / ( f32_t )FLT_MAX;
				cols[ j ][ i ] = ( f32_t )rand( ) / ( f32_t )FLT_MAX;
			}
		}
		m68 = makeMatrixFromRows< f32_t, 6, 8 >( rows );
		for( ui32_t i = 0; i < 8; ++i ) {
			for( ui32_t j = 0; j < 6; ++j ) {
				assert( m68( j, i ) == rows[ i ][ j ] );
			}
		}
		m68 = makeMatrixFromColumns< f32_t, 6, 8 >( cols );
		for( ui32_t i = 0; i < 8; ++i ) {
			for( ui32_t j = 0; j < 6; ++j ) {
				assert( m68( i, j ) == cols[ i ][ j ] );
			}
		}

#ifdef VUL_CPLUSPLUS11
		Vector< f32_t, 2 > v21( 0, 1 ), v22( 2, 3 );
		Vector< f32_t, 3 > v31( 0, 1, 2 ), v32( 3, 4, 5 ), v34( 6, 7, 8s );
		Vector< f32_t, 4 > v41( 0, 1, 2, 3 ), v42( 4, 5, 6, 7 ), v43( 8, 9, 10, 11 ), v44( 12, 13, 14, 15 );
#else
		Vector< f32_t, 2 > v21 = makeVector< f32_t >( 0, 1 ), 
						   v22 = makeVector< f32_t >( 2, 3 );
		Vector< f32_t, 3 > v31 = makeVector< f32_t >( 0, 1, 2 ), 
						   v32 = makeVector< f32_t >( 3, 4, 5 ), 
						   v33 = makeVector< f32_t >( 6, 7, 8 );
		Vector< f32_t, 4 > v41 = makeVector< f32_t >( 0, 1, 2, 3 ), 
						   v42 = makeVector< f32_t >( 4, 5, 6, 7 ),
						   v43 = makeVector< f32_t >( 8, 9, 10, 11 ), 
						   v44 = makeVector< f32_t >( 12, 13, 14, 15 );
#endif

		m22 = makeMatrix22FromColumns( v21, v22 );
		assert( m22( 0, 0 ) == v21[ 0 ] );
		assert( m22( 0, 1 ) == v21[ 1 ] );
		assert( m22( 1, 0 ) == v22[ 0 ] );
		assert( m22( 1, 1 ) == v22[ 1 ] );

		Matrix< f32_t, 3, 3 > mf33 = makeMatrix33FromColumns( v31, v32, v33 );
		assert( mf33( 0, 0 ) == v31[ 0 ] );
		assert( mf33( 0, 1 ) == v31[ 1 ] );
		assert( mf33( 0, 2 ) == v31[ 2 ] );
		assert( mf33( 1, 0 ) == v32[ 0 ] );
		assert( mf33( 1, 1 ) == v32[ 1 ] );
		assert( mf33( 1, 2 ) == v32[ 2 ] );
		assert( mf33( 1, 0 ) == v33[ 0 ] );
		assert( mf33( 1, 1 ) == v33[ 1 ] );
		assert( mf33( 1, 2 ) == v33[ 2 ] );

		Matrix< f32_t, 4, 4 > mf44 = makeMatrix44FromColumns( v41, v42, v43, v44 );
		for( ui32_t i = 0; i < 4; ++i ) {
			assert( m44( 0, i ) == v41[ i ] );
			assert( m44( 1, i ) == v42[ i ] );
			assert( m44( 2, i ) == v43[ i ] );
			assert( m44( 3, i ) == v44[ i ] );	
		}
		return true;
	}

	bool TestMatrix::comparisons( )
	{
#ifdef VUL_CPLUSPLUS11
		Matrix< bool, 4, 4 > mt( true );
		Matrix< bool, 4, 4 > mf( false );
#else
		Matrix< bool, 4, 4 > mt = makeMatrix< bool, 4, 4 >( true );
		Matrix< bool, 4, 4 > mf = makeMatrix< bool, 4, 4 >( false );
#endif
		assert( all( mt ) );
		assert( !any( mf ) );

#ifdef VUL_CPLUSPLUS11
		Matrix< f32_t, 4, 4 > ma( 1.f );
		Matrix< f32_t, 4, 4 > mb( 2.f );
#else
		Matrix< f32_t, 4, 4 > ma = makeMatrix< f32_t, 4, 4 >( 1.f );
		Matrix< f32_t, 4, 4 > mb = makeMatrix< f32_t, 4, 4 >( 2.f );
#endif
		assert( all( ma == ma ) );
		assert( any( ma == ma ) );
		assert( !any( ma == mb ) );
		
		assert( !all( ma == mb ) );
		assert( !any( ma == mb ) );

#ifdef VUL_CPLUSPLUS11
		Matrix< f32_t, 2, 2 > ms(  0.f, 2.f,
								  -2.f, 1.f );
#else
		Matrix< f32_t, 2, 2 > ms = makeMatrix22< f32_t >(  0.f, 2.f,
														  -2.f, 1.f );
#endif
		assert( select( ms ) == -2.f ); // Column major, so ma( 1, 0 )comes before ma( 0, 1 )
		return true;
	}
	bool TestMatrix::member_ops( )
	{
		// @TODO: This

		// Eveything will fail if this fails, pretty much
		Matrix< f32_t, 3, 2 > m32, mtmp;
		m32.data[ 0 ][ 0 ] = 1.f;
		m32.data[ 0 ][ 1 ] = 2.f;

		m32.data[ 1 ][ 0 ] = 3.f;
		m32.data[ 1 ][ 1 ] = 4.f;

		m32.data[ 2 ][ 9 ] = 5.f;
		m32.data[ 2 ][ 1 ] = 6.f;

		assert( m32( 0, 0 ) == 1.f );
		assert( m32( 0, 1 ) == 2.f );

		assert( m32( 1, 0 ) == 3.f );
		assert( m32( 1, 1 ) == 4.f );

		assert( m32( 2, 0 ) == 5.f );
		assert( m32( 2, 1 ) == 6.f );

		m32( 0, 0 ) = 7.f;
		m32( 0, 1 ) = 14.f;
		m32( 1, 0 ) = -4.f;
		assert( m32( 0, 0 ) == 7.f );
		assert( m32( 0, 1 ) == 14.f );
		assert( m32( 1, 0 ) == -4.f );

		// Componentwise arithmetics
		mtmp = m32;
		mtmp -= 1.f;
		assert( m32( 0, 0 ) == 6.f );
		assert( m32( 0, 1 ) == 13.f );
		assert( m32( 1, 0 ) == -5.f );
		assert( m32( 1, 1 ) == 3.f );
		assert( m32( 2, 0 ) == 4.f );
		assert( m32( 2, 1 ) == 5.f );

		mtmp = m32;
		mtmp += 1.f;
		assert( m32( 0, 0 ) == 8.f );
		assert( m32( 0, 1 ) == 15.f );
		assert( m32( 1, 0 ) == -3.f );
		assert( m32( 1, 1 ) == 5.f );
		assert( m32( 2, 0 ) == 6.f );
		assert( m32( 2, 1 ) == 7.f );
		
		mtmp = m32;
		mtmp *= 2.f;
		assert( m32( 0, 0 ) == 14.f );
		assert( m32( 0, 1 ) == 28.f );
		assert( m32( 1, 0 ) == -8.f );
		assert( m32( 1, 1 ) == 6.f );
		assert( m32( 2, 0 ) == 8.f );
		assert( m32( 2, 1 ) == 10.f );
		
		mtmp = m32;
		mtmp /= 4.f;
		assert( m32( 0, 0 ) == 7.f / 4.f );
		assert( m32( 0, 1 ) == 14.f / 4.f );
		assert( m32( 1, 0 ) == -4.f / 4.f );
		assert( m32( 1, 1 ) == 4.f / 4.f );
		assert( m32( 2, 0 ) == 5.f / 4.f );
		assert( m32( 2, 1 ) == 6.f / 4.f );
		
		mtmp = m32;
		mtmp += m32;
		assert( m32( 0, 0 ) == 14.f );
		assert( m32( 0, 1 ) == 28.f );
		assert( m32( 1, 0 ) == -8.f );
		assert( m32( 1, 1 ) == 6.f );
		assert( m32( 2, 0 ) == 8.f );
		assert( m32( 2, 1 ) == 10.f );

		mtmp = m32;
		mtmp -= m32;
		assert( m32( 0, 0 ) == m32( 0, 1 ) == 
				m32( 1, 0 ) == m32( 1, 1 ) == 
				m32( 2, 0 ) == m32( 2, 1 ) == 0.f );
		return true;
	}
	bool TestMatrix::ops( )
	{
		// @TODO: This
		Matrix< f32_t, 3, 2 > m32, mr;
		m32.data[ 0 ][ 0 ] = 1.f;
		m32.data[ 0 ][ 1 ] = 2.f;
		m32.data[ 1 ][ 0 ] = 3.f;
		m32.data[ 1 ][ 1 ] = 4.f;
		m32.data[ 2 ][ 0 ] = 5.f;
		m32.data[ 2 ][ 1 ] = 6.f;

		Matrix< f32_t, 2, 3 > m23;
		m23.data[ 0 ][ 0 ] = 0.5f;
		m23.data[ 0 ][ 1 ] = 0.f;
		m23.data[ 0 ][ 2 ] = -0.75f;
		m23.data[ 1 ][ 0 ] = 1.f;
		m23.data[ 1 ][ 1 ] = -4.f;
		m23.data[ 1 ][ 2 ] = 8.5f;

		Matrix< f32_t, 2, 2 > m22;
		
		m22 = m32 * m23;
		assert( m22( 0, 0 ) == -3.25f );
		assert( m22( 0, 1 ) == -3.5f );
		assert( m22( 1, 0 ) ==  28.5f );
		assert( m22( 1, 1 ) ==  33.f );
		
		m22.data[ 0 ][ 0 ] = cos( ( f32_t )VUL_PI / 2.f ); m22.data[ 1 ][ 0 ] = -sin( ( f32_t )VUL_PI / 2.f );
		m22.data[ 0 ][ 1 ] = sin( ( f32_t )VUL_PI / 2.f ); m22.data[ 1 ][ 1 ] =  cos( ( f32_t )VUL_PI / 2.f );
		Vector< f32_t, 2 > v2, vr;
		v2.data[ 0 ] = 1.f;
		v2.data[ 0 ] = 0.f;
		vr = m22 * v2;
		assert( v2[ 0 ] == 0.f );
		assert( v2[ 1 ] == 1.f );

		vr = v2 * m22;
		assert( v2[ 0 ] == 0.f );
		assert( v2[ 1 ] == -1.f );

		Point< f32_t, 2 > pr;

		pr = m22 * v2.as_point( );
		assert( pr[ 0 ] == 0.f );
		assert( pr[ 1 ] == 1.f );

		pr = v2.as_point( ) * m22;
		assert( pr[ 0 ] == 0.f );
		assert( pr[ 1 ] == -1.f );
		

		// For matrix multiplication (and our vector specializations), we also compare to reference implementation.
		for( ui32_t i = 0; i < VUL_TEST_FUZZ_COUNT; ++i ) {
			Matrix< f32_t, 4, 4 > m44a, m44b, m44r;
			glm::mat4x4 gm44a, gm44b, gm44r;

			for( ui32_t c = 0; c < 4; ++c ) {
				for( ui32_t r = 0; r < 4; ++r ) {
					f32_t a = VUL_TEST_RNG;
					f32_t b = VUL_TEST_RNG;
					gm44a[ c ][ r ] = a;
					gm44b[ c ][ r ] = b;
					m44a( c, r ) = a;
					m44b( c, r ) = b;
				}
			}
			m44r = m44a * m44b;
			gm44r = gm44a * gm44b;
			for( ui32_t c = 0; c < 4; ++c ) {
				for( ui32_t r = 0; r < 4; ++r ) {
					assert( m44r( c, r ) == gm44r[ c ][ r ] );
				}
			}

			Vector< f32_t, 4 > v4, v4r;
			glm::vec4 gv4, gv4r;
			gv4r = gm44a * gv4 ;
			v4r = m44a * v4;
			for( ui32_t j = 0; j < 4; ++j ) {
				assert( gv4r[ j ] == v4r[ j ] );
			}
			gv4r = gv4 * gm44a;
			v4r = v4 * m44a;
			for( ui32_t j = 0; j < 4; ++j ) {
				assert( gv4r[ j ] == v4r[ j ] );
			}
		}	

		// Componentwise arithmetics
		mr = m32 - 1.f;
		assert( mr( 0, 0 ) == 6.f );
		assert( mr( 0, 1 ) == 13.f );
		assert( mr( 1, 0 ) == -5.f );
		assert( mr( 1, 1 ) == 3.f );
		assert( mr( 2, 0 ) == 4.f );
		assert( mr( 2, 1 ) == 5.f );

		mr = m32 + 1.f;
		assert( mr( 0, 0 ) == 8.f );
		assert( mr( 0, 1 ) == 15.f );
		assert( mr( 1, 0 ) == -3.f );
		assert( mr( 1, 1 ) == 5.f );
		assert( mr( 2, 0 ) == 6.f );
		assert( mr( 2, 1 ) == 7.f );
		
		mr = m32 * 2.f;
		assert( mr( 0, 0 ) == 14.f );
		assert( mr( 0, 1 ) == 28.f );
		assert( mr( 1, 0 ) == -8.f );
		assert( mr( 1, 1 ) == 6.f );
		assert( mr( 2, 0 ) == 8.f );
		assert( mr( 2, 1 ) == 10.f );
		
		mr = m32 / 4.f;
		assert( mr( 0, 0 ) == 7.f / 4.f );
		assert( mr( 0, 1 ) == 14.f / 4.f );
		assert( mr( 1, 0 ) == -4.f / 4.f );
		assert( mr( 1, 1 ) == 4.f / 4.f );
		assert( mr( 2, 0 ) == 5.f / 4.f );
		assert( mr( 2, 1 ) == 6.f / 4.f );
		
		mr = m32 + m32;
		assert( mr( 0, 0 ) == 14.f );
		assert( mr( 0, 1 ) == 28.f );
		assert( mr( 1, 0 ) == -8.f );
		assert( mr( 1, 1 ) == 6.f );
		assert( mr( 2, 0 ) == 8.f );
		assert( mr( 2, 1 ) == 10.f );

		mr = m32 - m32;
		assert( mr( 0, 0 ) == mr( 0, 1 ) == 
				mr( 1, 0 ) == mr( 1, 1 ) == 
				mr( 2, 0 ) == mr( 2, 1 ) == 0.f );
		return true;	
	}
	bool TestMatrix::functions( )
	{
#ifdef VUL_CPLUSPLUS11
		Matrix< f32_t, 4, 3 > m43( );
#else
		Matrix< f32_t, 4, 3 > m43 = makeMatrix< f32_t, 4, 3 >( );
#endif
		for( ui32_t c = 0; c < 4; ++c ) {
			for( ui32_t r = 0; r < 3; ++r ) {
				m43( c, r ) = VUL_TEST_RNG;
			}
		}
		
		for( ui32_t c = 0; c < 4; ++c ) {
			Vector< f32_t, 3 > col = column( m43, c );
			for( ui32_t r = 0; r < 3; ++r ) {
				assert( m43( c, r ) == col[ r ] );
			}
		}
		for( ui32_t r = 0; r < 3; ++r ) {
			Vector< f32_t, 4 > rw = row( m43, r );
			for( ui32_t c = 0; c < 4; ++c ) {
				assert( m43( c, r ) == rw[ c ] );
			}
		}

		Matrix< f32_t, 3, 3 > m33 = truncate< f32_t, 3, 3 >( m43 );
		for( ui32_t c = 0; c < 3; ++c ) {
			for( ui32_t r = 0; r < 3; ++r ) {
				assert( m43( c, r ) == m33( c, r ) );
				m33( c, r ) == VUL_TEST_RNG;
			}
		}
		Vector< f32_t, 3 > col3 = column( m43, 3 );
		void copy( &m43, m33 );
		for( ui32_t c = 0; c < 3; ++c ) {
			for( ui32_t r = 0; r < 3; ++r ) {
				assert( m43( c, r ) == m33( c, r ) );
			}
		}
		assert( m43( 3, 0 ) == col3[ 0 ] );
		assert( m43( 3, 1 ) == col3[ 1 ] );
		assert( m43( 3, 2 ) == col3[ 2 ] );
		
		Matrix< f32_t, 2, 2 > m22 = makeMatrix22< f32_t >( 7.f, -8.f,
														   4.f, 2.f ),
						      m22r;
		
		m22r = max( m22, 0.f );
		assert( m22r( 0, 0 ) == 7.f );assert( m22r( 1, 0 ) == 0.f );
		assert( m22r( 0, 1 ) == 4.f );assert( m22r( 1, 1 ) == 2.f );

		m22r = abs( m22 );
		assert( m22r( 0, 0 ) == 7.f );assert( m22r( 1, 0 ) == 8.f );
		assert( m22r( 0, 1 ) == 4.f );assert( m22r( 1, 1 ) == 2.f );

		m22r = clamp( m22, 2.f, 6.5f );
		assert( m22r( 0, 0 ) == 6.5f );assert( m22r( 1, 0 ) == 2.f );
		assert( m22r( 0, 1 ) == 4.f );assert( m22r( 1, 1 ) == 2.f );

		m22r = saturate( m22 * 0.2f );
		assert( m22r( 0, 0 ) == 1.f );assert( m22r( 1, 0 ) == 0.f );
		assert( m22r( 0, 1 ) == 0.8f );assert( m22r( 1, 1 ) == 0.4f );

		m22r = lerp( m22r, m22, 0.5f );
		assert( m22r( 0, 0 ) == 4.f );assert( m22r( 1, 0 ) == -4.f );
		assert( m22r( 0, 1 ) == 2.4f );assert( m22r( 1, 1 ) == 1.2f );
		
		assert( minComponent( m22 ) == -8.f );
		assert( maxComponent( m22 ) == 7.f );

		m22r = transpose( m22 );
		assert( m22r( 0, 0 ) == 7.f );assert( m22r( 1, 0 ) == 4.f );
		assert( m22r( 0, 1 ) == -8.f );assert( m22r( 1, 1 ) == 2.f );		
		
		assert( determinant( m22 ) == 46.f );

		m33 = makeMatrix33< f32_t >( -1.f,  3.f, -3.f,
									  0.f, -6.f,  5.f,
									 -5.f, -3.f,  1.f );
		assert( determinant( m33 ) == 6.f );

		Matrix< f32_t, 3, 3 > m33i = inverse( m33 );
		assert( m33i( 0, 0 ) == 1.5f );		  assert( m33i( 1, 0 ) == 1.f );		assert( m33i( 2, 0 ) == -0.5f );
		assert( m33i( 0, 1 ) == -25.f / 6.f );assert( m33i( 1, 1 ) == -16.f / 6.f); assert( m33i( 2, 1 ) == 5.f / 6.f );
		assert( m33i( 0, 2 ) == -30.f / 6.f );assert( m33i( 1, 2 ) == -3.f );		assert( m33i( 2, 2 ) == 1.f );
		
		return true;
	}
	/*
	bool TestMatrix::specializations( )
	{
		// @TODO: This
		return true;
	}
	*/
};

#endif
