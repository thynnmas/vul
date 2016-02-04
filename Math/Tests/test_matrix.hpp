/*
 * Villains' Utility Library - Thomas Martin Schmid, 2016. Public domain¹
 *
 * This file contains tests for the matrix struct in vul_matrix.hpp
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
#ifndef VUL_TEST_MATRIX_HPP
#define VUL_TEST_MATRIX_HPP

#include <cassert>

#include "../vul_math.hpp"
#ifdef VUL_TEST_REFERENCE
	#include "glm.hpp"
#endif

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
		Matrix< f32, 2, 2> m22;
		Matrix< fi32, 3, 3> m33;
		Matrix< s64, 4, 4> m44;
		Matrix< f32, 6, 8> m68;
#else
		Matrix< f32, 2, 2> m22 = makeMatrix< f32, 2, 2 >( );
		Matrix< fi32, 3, 3> m33 = makeMatrix< fi32, 3, 3 >( );
		Matrix< s64, 4, 4> m44 = makeMatrix< s64, 4, 4 >( );
		Matrix< f32, 6, 8> m68 = makeMatrix< f32, 6, 8 >( );
#endif
		assert( m22( 0, 0 ) == 0.f );
		assert( m22( 1, 0 ) ==  0.f );
		assert( m22( 0, 1 ) ==  0.f );
		assert( m22( 1, 1 ) == 0.f );
		assert( m33( 0, 0 ) == fi32( 0.f ) );
		assert( m33( 1, 0 ) == fi32( 0.f ) );
		assert( m33( 2, 0 ) == fi32( 0.f ) );
		assert( m33( 0, 1 ) == fi32( 0.f ) );
		assert( m33( 1, 1 ) == fi32( 0.f ) );
		assert( m33( 2, 1 ) == fi32( 0.f ) );
		assert( m33( 0, 2 ) == fi32( 0.f ) );
		assert( m33( 1, 2 ) == fi32( 0.f ) );
		assert( m33( 2, 2 ) == fi32( 0.f ) );
		for( u32 c = 0; c < 4; ++c ) {
			for( u32 r = 0; r < 4; ++r ) {
				assert( m44( c, r ) == 0L );
			}
		}
		for( u32 c = 0; c < 6; ++c ) {
			for( u32 r = 0; r < 8; ++r ) {
				assert( m68( c, r ) == 0.f );
			}
		}

		// Repeat for all, but m22 and m68 should be fine
#ifdef VUL_CPLUSPLUS11
		m22 = Matrix< f32, 2, 2 >( 2.f );
		m68 = Matrix< f32, 6, 8 >( -1.f );
#else
		m22 = makeMatrix< f32, 2, 2 >( 2.f );
		m68 = makeMatrix< f32, 6, 8 >( -1.f );
#endif
		assert( m22( 0, 0 ) == 2.f );
		assert( m22( 1, 0 ) == 2.f );
		assert( m22( 0, 1 ) == 2.f );
		assert( m22( 1, 1 ) == 2.f );
		for( u32 c = 0; c < 6; ++c ) {
			for( u32 r = 0; r < 8; ++r ) {
				assert( m68( c, r ) == -1.f );
			}
		}

#ifdef VUL_CPLUSPLUS11
		m22 = Matrix< f32, 2, 2 >( Matrix< f32, 2, 2 >( ) );
		m68 = Matrix< f32, 6, 8 >( Matrix< f32, 6, 8 >( ) );
#else
		m22 = makeMatrix< f32, 2, 2 >( makeMatrix< f32, 2, 2 >( ) );
		m68 = makeMatrix< f32, 6, 8 >( makeMatrix< f32, 6, 8 >( ) );
#endif
		
		assert( m22( 0, 0 ) == 0.f );
		assert( m22( 1, 0 ) == 0.f );
		assert( m22( 0, 1 ) == 0.f );
		assert( m22( 1, 1 ) == 0.f );
		for( u32 c = 0; c < 6; ++c ) {
			for( u32 r = 0; r < 8; ++r ) {
				assert( m68( c, r ) == 0.f );
			}
		}

		// For this we need to test a non-f32 or i32 type, so we only test the m33 (fi32)
		fi32 a33[ 3 ][ 3 ];
		for( u32 c = 0; c < 3; ++c ) {
			for( u32 r = 0; r < 3; ++r ) {
				a33[ c ][ r ] = fi32( VUL_TEST_RNG );
			}
		}
#ifdef VUL_CPLUSPLUS11
		m33 = Matrix< fi32, 3, 3 >( a33 );
#else
		m33 = makeMatrix< fi32, 3, 3 >( a33 );
#endif
		
		for( u32 c = 0; c < 3; ++c ) {
			for( u32 r = 0; r < 3; ++r ) {
				assert( m33( c, r ) == a33[ c ][ r ] );
			}
		}
		
		f32 af[ 6 ][ 8 ];
		for( u32 c = 0; c < 6; ++c ) {
			for( u32 r = 0; r < 8; ++r ) {
				af[ c ][ r ] = VUL_TEST_RNG;
			}
		}
		f32 af22[ 2 ][ 2 ] = { { af[ 0 ][ 0 ], af[ 0 ][ 1 ] }, { af[ 1 ][ 0 ], af[ 1 ][ 1 ] } };
#ifdef VUL_CPLUSPLUS11
		m22 = Matrix< f32, 2, 2 >( af22 );
		m68 = Matrix< f32, 6, 8 >( af );
#else
		m22 = makeMatrix< f32, 2, 2 >( af22 );
		m68 = makeMatrix< f32, 6, 8 >( af );
#endif
		assert( m22( 0, 0 ) == af22[ 0 ][ 0 ] );
		assert( m22( 1, 0 ) == af22[ 1 ][ 0 ] );
		assert( m22( 0, 1 ) == af22[ 0 ][ 1 ] );
		assert( m22( 1, 1 ) == af22[ 1 ][ 1 ] );
		for( u32 c = 0; c < 6; ++c ) {
			for( u32 r = 0; r < 8; ++r ) {
				assert( m68( c, r ) == af[ c ][ r ] );
			}
		}
		
		s64 ai[ 4 ][ 4 ];
		for( u32 c = 0; c < 4; ++c ) {
			for( u32 r = 0; r < 4; ++r ) {
				ai[ c ][ r ] = ( s64 )rand( );
			}
		}


#ifdef VUL_CPLUSPLUS11
		m44 = Matrix< s64, 4, 4 >( ai );
#else
		m44 = makeMatrix< s64, 4, 4 >( ai );
#endif
		for( u32 c = 0; c < 4; ++c ) {
			for( u32 r = 0; r < 4; ++r ) {
				assert( m44( c, r ) == ai[ c ][ r ] );
			}
		}

		// Only available in C++11
#ifdef VUL_CPLUSPLUS11
		Matrix< f32, 2, 2 > c11m22{ 1.f, -2.f,
									  4.f, 3.f };
		assert( c11m22( 0, 0 ) == 1.f );
		assert( c11m22( 0, 1 ) == 4.f );
		assert( c11m22( 1, 0 ) == -2.f );
		assert( c11m22( 1, 1 ) == 3.f );
#endif

		m22 = makeIdentity< f32, 2 >( );
		m44 = makeIdentity< s64, 4 >( );
		for( u32 r = 0; r < 2; ++r ) {
			for( u32 c = 0; c < 2; ++c ) {
				if( c == r ) {
					assert( m22( c, r )	== 1.f );
					assert( m44( c, r )	== 1L );
				} else {
					assert( m22( c, r )	== 0.f );
					assert( m44( c, r )	== 0L );
				}
			}
		}


		m22 = makeMatrix22< f32 >(  1.f, 4.f, 
								     -2.f, 3.f );
		assert( m22( 0, 0 ) == 1.f );
		assert( m22( 0, 1 ) == -2.f );
		assert( m22( 1, 0 ) == 4.f );
		assert( m22( 1, 1 ) == 3.f );

		m33 = makeMatrix33< fi32 >( fi32( -4.f ), fi32( -3.f ), fi32( -2.f ),
									  fi32( -1.f ), fi32(  0.f ), fi32(  1.f ),
									  fi32(  2.f ), fi32(  3.f ), fi32(  4.f ) );
		assert( m33( 0, 0 ) == fi32( -4.f ) );
		assert( m33( 1, 0 ) == fi32( -3.f ) );
		assert( m33( 2, 0 ) == fi32( -2.f ) );
		assert( m33( 0, 1 ) == fi32( -1.f ) );
		assert( m33( 1, 1 ) == fi32(  0.f ) );
		assert( m33( 2, 1 ) == fi32(  1.f ) );
		assert( m33( 0, 2 ) == fi32(  2.f ) );
		assert( m33( 1, 2 ) == fi32(  3.f ) );
		assert( m33( 2, 2 ) == fi32(  4.f ) );

		m44 = makeMatrix44< s64 >( 1L, 5L,  9L, 13L,
									 2L, 6L, 10L, 14L,
									 3L, 7L, 11L, 15L,
									 4L, 8L, 12L, 16L );
		for( u32 c = 0; c < 4; ++c ) {
			for( u32 r = 0; r < 4; ++r ) {
				assert( m44( c, r ) == ( s64 )( c * 4 + r + 1 ) );
			}
		}
		
		Vector< f32, 8 > cols[ 6 ];
		Vector< f32, 6 > rows[ 8 ];
		for( u32 i = 0; i < 8; ++i ) {
			for( u32 j = 0; j < 6; ++j ) {
				rows[ i ][ j ] = ( f32 )rand( ) / ( f32 )FLT_MAX;
				cols[ j ][ i ] = ( f32 )rand( ) / ( f32 )FLT_MAX;
			}
		}
		m68 = makeMatrixFromRows< f32, 6, 8 >( rows );
		for( u32 i = 0; i < 8; ++i ) {
			for( u32 j = 0; j < 6; ++j ) {
				assert( m68( j, i ) == rows[ i ][ j ] );
			}
		}
		m68 = makeMatrixFromColumns< f32, 6, 8 >( cols );
		for( u32 i = 0; i < 6; ++i ) {
			for( u32 j = 0; j < 8; ++j ) {
				assert( m68( i, j ) == cols[ i ][ j ] );
			}
		}

#ifdef VUL_CPLUSPLUS11
		Vector< f32, 2 > v21{ 0.f, 1.f }, v22{ 2.f, 3.f };
		Vector< f32, 3 > v31{ 0.f, 1.f, 2.f }, v32{ 3.f, 4.f, 5.f }, v33{ 6.f, 7.f, 8.f };
		Vector< f32, 4 > v41{ 0.f, 1.f, 2.f, 3.f }, v42{ 4.f, 5.f, 6.f, 7.f }, 
						   v43{ 8.f, 9.f, 10.f, 11.f }, v44{ 12.f, 13.f, 14.f, 15.f };
#else
		Vector< f32, 2 > v21 = makeVector< f32 >( 0, 1 ), 
						   v22 = makeVector< f32 >( 2, 3 );
		Vector< f32, 3 > v31 = makeVector< f32 >( 0, 1, 2 ), 
						   v32 = makeVector< f32 >( 3, 4, 5 ), 
						   v33 = makeVector< f32 >( 6, 7, 8 );
		Vector< f32, 4 > v41 = makeVector< f32 >( 0, 1, 2, 3 ), 
						   v42 = makeVector< f32 >( 4, 5, 6, 7 ),
						   v43 = makeVector< f32 >( 8, 9, 10, 11 ), 
						   v44 = makeVector< f32 >( 12, 13, 14, 15 );
#endif

		m22 = makeMatrix22FromColumns( v21, v22 );
		assert( m22( 0, 0 ) == v21[ 0 ] );
		assert( m22( 0, 1 ) == v21[ 1 ] );
		assert( m22( 1, 0 ) == v22[ 0 ] );
		assert( m22( 1, 1 ) == v22[ 1 ] );

		Matrix< f32, 3, 3 > mf33 = makeMatrix33FromColumns( v31, v32, v33 );
		assert( mf33( 0, 0 ) == v31[ 0 ] );
		assert( mf33( 0, 1 ) == v31[ 1 ] );
		assert( mf33( 0, 2 ) == v31[ 2 ] );
		assert( mf33( 1, 0 ) == v32[ 0 ] );
		assert( mf33( 1, 1 ) == v32[ 1 ] );
		assert( mf33( 1, 2 ) == v32[ 2 ] );
		assert( mf33( 2, 0 ) == v33[ 0 ] );
		assert( mf33( 2, 1 ) == v33[ 1 ] );
		assert( mf33( 2, 2 ) == v33[ 2 ] );

		Matrix< f32, 4, 4 > mf44 = makeMatrix44FromColumns( v41, v42, v43, v44 );
		for( u32 i = 0; i < 4; ++i ) {
			assert( mf44( 0, i ) == v41[ i ] );
			assert( mf44( 1, i ) == v42[ i ] );
			assert( mf44( 2, i ) == v43[ i ] );
			assert( mf44( 3, i ) == v44[ i ] );	
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
		Matrix< f32, 4, 4 > ma( 1.f );
		Matrix< f32, 4, 4 > mb( 2.f );
#else
		Matrix< f32, 4, 4 > ma = makeMatrix< f32, 4, 4 >( 1.f );
		Matrix< f32, 4, 4 > mb = makeMatrix< f32, 4, 4 >( 2.f );
#endif
		assert( all( ma == ma ) );
		assert( any( ma == ma ) );
		assert( !any( ma == mb ) );
		
		assert( !all( ma == mb ) );
		assert( !any( ma == mb ) );

#ifdef VUL_CPLUSPLUS11
		Matrix< f32, 2, 2 > ms{  0.f, 2.f,
								  -2.f, 1.f };
#else
		Matrix< f32, 2, 2 > ms = makeMatrix22< f32 >(  0.f, 2.f,
														  -2.f, 1.f );
#endif
		assert( select( ms ) == 2.f ); // Row major, so ma( 0, 1 )comes before ma( 1, 0 )
		return true;
	}
	bool TestMatrix::member_ops( )
	{
		// Eveything will fail if this fails, pretty much
		Matrix< f32, 3, 2 > m32, mtmp;
		m32.data[ 0 ][ 0 ] = 1.f;
		m32.data[ 0 ][ 1 ] = 2.f;

		m32.data[ 1 ][ 0 ] = 3.f;
		m32.data[ 1 ][ 1 ] = 4.f;

		m32.data[ 2 ][ 0 ] = 5.f;
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
		assert( mtmp( 0, 0 ) == 6.f );
		assert( mtmp( 0, 1 ) == 13.f );
		assert( mtmp( 1, 0 ) == -5.f );
		assert( mtmp( 1, 1 ) == 3.f );
		assert( mtmp( 2, 0 ) == 4.f );
		assert( mtmp( 2, 1 ) == 5.f );

		mtmp = m32;
		mtmp += 1.f;
		assert( mtmp( 0, 0 ) == 8.f );
		assert( mtmp( 0, 1 ) == 15.f );
		assert( mtmp( 1, 0 ) == -3.f );
		assert( mtmp( 1, 1 ) == 5.f );
		assert( mtmp( 2, 0 ) == 6.f );
		assert( mtmp( 2, 1 ) == 7.f );
		
		mtmp = m32;
		mtmp *= 2.f;
		assert( mtmp( 0, 0 ) == 14.f );
		assert( mtmp( 0, 1 ) == 28.f );
		assert( mtmp( 1, 0 ) == -8.f );
		assert( mtmp( 1, 1 ) == 8.f );
		assert( mtmp( 2, 0 ) == 10.f );
		assert( mtmp( 2, 1 ) == 12.f );
		
		mtmp = m32;
		mtmp /= 4.f;
		assert( mtmp( 0, 0 ) == 7.f / 4.f );
		assert( mtmp( 0, 1 ) == 14.f / 4.f );
		assert( mtmp( 1, 0 ) == -4.f / 4.f );
		assert( mtmp( 1, 1 ) == 4.f / 4.f );
		assert( mtmp( 2, 0 ) == 5.f / 4.f );
		assert( mtmp( 2, 1 ) == 6.f / 4.f );
		
		mtmp = m32;
		mtmp += m32;
		assert( mtmp( 0, 0 ) == 14.f );
		assert( mtmp( 0, 1 ) == 28.f );
		assert( mtmp( 1, 0 ) == -8.f );
		assert( mtmp( 1, 1 ) == 8.f );
		assert( mtmp( 2, 0 ) == 10.f );
		assert( mtmp( 2, 1 ) == 12.f );

		mtmp = m32;
		mtmp -= m32;
		assert( mtmp( 0, 0 ) == 0.f );
		assert( mtmp( 0, 1 ) == 0.f );
		assert( mtmp( 1, 0 ) == 0.f );
		assert( mtmp( 1, 1 ) == 0.f );
		assert( mtmp( 2, 0 ) == 0.f );
		assert( mtmp( 2, 1 ) == 0.f );

		return true;
	}
	bool TestMatrix::ops( )
	{
		Matrix< f32, 3, 2 > m32, mr;
		m32.data[ 0 ][ 0 ] = 1.f;
		m32.data[ 0 ][ 1 ] = 2.f;
		m32.data[ 1 ][ 0 ] = 3.f;
		m32.data[ 1 ][ 1 ] = 4.f;
		m32.data[ 2 ][ 0 ] = 5.f;
		m32.data[ 2 ][ 1 ] = 6.f;

		Matrix< f32, 2, 3 > m23;
		m23.data[ 0 ][ 0 ] = 0.5f;
		m23.data[ 0 ][ 1 ] = 0.f;
		m23.data[ 0 ][ 2 ] = -0.75f;
		m23.data[ 1 ][ 0 ] = 1.f;
		m23.data[ 1 ][ 1 ] = -4.f;
		m23.data[ 1 ][ 2 ] = 8.5f;

		Matrix< f32, 2, 2 > m22;
		
		m22 = m23 * m32;
		assert( m22( 0, 0 ) == -3.25f );
		assert( m22( 0, 1 ) == -3.5f );
		assert( m22( 1, 0 ) ==  31.5f );
		assert( m22( 1, 1 ) ==  37.f );
		
		m22( 0, 0 ) = 0.f; m22( 1, 0 ) = -1.f;
		m22( 0, 1 ) = 1.f; m22( 1, 1 ) =  0.f;
		Vector< f32, 2 > v2, vr;
		v2.data[ 0 ] = 1.f;
		v2.data[ 1 ] = 0.f;
		vr = m22 * v2;
		assert( vr[ 0 ] == 0.f );
		assert( vr[ 1 ] == 1.f );

		vr = v2 * m22;
		assert( vr[ 0 ] == 0.f );
		assert( vr[ 1 ] == -1.f );

		Point< f32, 2 > pr;

		pr = m22 * v2.as_point( );
		assert( pr[ 0 ] == 0.f );
		assert( pr[ 1 ] == 1.f );

		pr = v2.as_point( ) * m22;
		assert( pr[ 0 ] == 0.f );
		assert( pr[ 1 ] == -1.f );

		// For matrix multiplication (and our vector specializations), we also compare to reference implementation.
#ifdef VUL_TEST_REFERENCE
		f32 f32eps = 1e-5f;
		for( u32 i = 0; i < VUL_TEST_FUZZ_COUNT; ++i ) {
			Matrix< f32, 4, 4 > m44a, m44b, m44r;
			glm::mat4x4 gm44a, gm44b, gm44r;

			for( u32 c = 0; c < 4; ++c ) {
				for( u32 r = 0; r < 4; ++r ) {
					f32 a = VUL_TEST_RNG;
					f32 b = VUL_TEST_RNG;
					m44a( c, r ) = a;
					m44b( c, r ) = b;
					gm44a[ r ][ c ] = a;
					gm44b[ r ][ c ] = b;
				}
			}
			m44r = m44a * m44b;
			gm44r = gm44a * gm44b;
			for( u32 c = 0; c < 4; ++c ) {
				for( u32 r = 0; r < 4; ++r ) {
					assert( abs( m44r( c, r ) - gm44r[ r ][ c ] ) < f32eps );
				}
			}

			Vector< f32, 4 > v4, v4r;
			glm::vec4 gv4, gv4r;
			for( u32 r = 0; r < 4; ++r ) {
				f32 a = VUL_TEST_RNG;
				gv4[ r ] = a;
				v4[ r ] = a;
			}
			gv4r = gv4 * gm44a;
			v4r = m44a * v4;
			for( u32 j = 0; j < 4; ++j ) {
				assert( abs( gv4r[ j ] - v4r[ j ] ) < f32eps );
			}
			gv4r = gm44a * gv4;
			v4r = v4 * m44a;
			for( u32 j = 0; j < 4; ++j ) {
				assert( abs( gv4r[ j ] - v4r[ j ] ) < f32eps );
			}
		}	
#endif

		// Componentwise arithmetics
		mr = m32 - 1.f;
		assert( mr( 0, 0 ) == 0.f );
		assert( mr( 0, 1 ) == 1.f );
		assert( mr( 1, 0 ) == 2.f );
		assert( mr( 1, 1 ) == 3.f );
		assert( mr( 2, 0 ) == 4.f );
		assert( mr( 2, 1 ) == 5.f );

		mr = m32 + 1.f;
		assert( mr( 0, 0 ) == 2.f );
		assert( mr( 0, 1 ) == 3.f );
		assert( mr( 1, 0 ) == 4.f );
		assert( mr( 1, 1 ) == 5.f );
		assert( mr( 2, 0 ) == 6.f );
		assert( mr( 2, 1 ) == 7.f );
		
		mr = m32 * 2.f;
		assert( mr( 0, 0 ) == 2.f );
		assert( mr( 0, 1 ) == 4.f );
		assert( mr( 1, 0 ) == 6.f );
		assert( mr( 1, 1 ) == 8.f );
		assert( mr( 2, 0 ) == 10.f );
		assert( mr( 2, 1 ) == 12.f );
		
		mr = m32 / 4.f;
		assert( mr( 0, 0 ) == 1.f / 4.f );
		assert( mr( 0, 1 ) == 2.f / 4.f );
		assert( mr( 1, 0 ) == 3.f / 4.f );
		assert( mr( 1, 1 ) == 4.f / 4.f );
		assert( mr( 2, 0 ) == 5.f / 4.f );
		assert( mr( 2, 1 ) == 6.f / 4.f );
		
		mr = m32 + m32;
		assert( mr( 0, 0 ) == 2.f );
		assert( mr( 0, 1 ) == 4.f );
		assert( mr( 1, 0 ) == 6.f );
		assert( mr( 1, 1 ) == 8.f );
		assert( mr( 2, 0 ) == 10.f );
		assert( mr( 2, 1 ) == 12.f );

		mr = m32 - m32;
		assert( mr( 0, 0 ) == 0.f );
		assert( mr( 0, 1 ) == 0.f );
		assert( mr( 1, 0 ) == 0.f );
		assert( mr( 1, 1 ) == 0.f );
		assert( mr( 2, 0 ) == 0.f );
		assert( mr( 2, 1 ) == 0.f );

		mr = -m32;
		assert( mr( 0, 0 ) == -m32( 0, 0 ) );
		assert( mr( 0, 1 ) == -m32( 0, 1 ) );
		assert( mr( 1, 0 ) == -m32( 1, 0 ) );
		assert( mr( 1, 1 ) == -m32( 1, 1 ) );
		assert( mr( 2, 0 ) == -m32( 2, 0 ) );
		assert( mr( 2, 1 ) == -m32( 2, 1 ) );

		return true;
	}
	bool TestMatrix::functions( )
	{
#ifdef VUL_CPLUSPLUS11
		Matrix< f32, 4, 3 > m43;
#else
		Matrix< f32, 4, 3 > m43 = makeMatrix< f32, 4, 3 >( );
#endif
		for( u32 c = 0; c < 4; ++c ) {
			for( u32 r = 0; r < 3; ++r ) {
				m43( c, r ) = VUL_TEST_RNG;
			}
		}
		
		for( u32 c = 0; c < 4; ++c ) {
			Vector< f32, 3 > col = column( m43, c );
			for( u32 r = 0; r < 3; ++r ) {
				assert( m43( c, r ) == col[ r ] );
			}
		}
		for( u32 r = 0; r < 3; ++r ) {
			Vector< f32, 4 > rw = row( m43, r );
			for( u32 c = 0; c < 4; ++c ) {
				assert( m43( c, r ) == rw[ c ] );
			}
		}

		Matrix< f32, 3, 3 > m33 = truncate< f32, 3, 3 >( m43 );
		for( u32 c = 0; c < 3; ++c ) {
			for( u32 r = 0; r < 3; ++r ) {
				assert( m43( c, r ) == m33( c, r ) );
				m33( c, r ) = VUL_TEST_RNG;
			}
		}
		Vector< f32, 3 > col3 = column( m43, 3 );
		copy( &m43, m33 );
		for( u32 c = 0; c < 3; ++c ) {
			for( u32 r = 0; r < 3; ++r ) {
				assert( m43( c, r ) == m33( c, r ) );
			}
		}
		assert( m43( 3, 0 ) == col3[ 0 ] );
		assert( m43( 3, 1 ) == col3[ 1 ] );
		assert( m43( 3, 2 ) == col3[ 2 ] );
		
		Vector< f32, 3 > col0 = column( m43, 0 );
		copy( &m43, m33, 1, 0 );
		for( u32 c = 0; c < 3; ++c ) {
			for( u32 r = 0; r < 3; ++r ) {
				assert( m43( c + 1, r ) == m33( c, r ) );
			}
		}
		assert( m43( 0, 0 ) == col0[ 0 ] );
		assert( m43( 0, 1 ) == col0[ 1 ] );
		assert( m43( 0, 2 ) == col0[ 2 ] );
		
		Matrix< f32, 2, 2 > m22 = makeMatrix22< f32 >( 7.f, -8.f,
														   4.f,  2.f ),
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

		m33 = makeMatrix33< f32 >( -1.f,  3.f, -3.f,
									  0.f, -6.f,  5.f,
									 -5.f, -3.f,  1.f );
		assert( determinant( m33 ) == 6.f );

		Matrix< f32, 3, 3 > m33i = inverse( m33 );
		f32 f32eps = 1e-5f;
		assert( abs( m33i( 0, 0 ) - 1.5f ) < f32eps );
		assert( abs( m33i( 1, 0 ) - 1.f ) < f32eps );
		assert( abs( m33i( 2, 0 ) - ( -0.5f ) ) < f32eps );
		assert( abs( m33i( 0, 1 ) - ( -25.f / 6.f ) ) < f32eps ); 
		assert( abs( m33i( 1, 1 ) - (-16.f / 6.f ) ) < f32eps ); 
		assert( abs( m33i( 2, 1 ) - ( 5.f / 6.f ) ) < f32eps );
		assert( abs( m33i( 0, 2 ) - ( -30.f / 6.f ) ) < f32eps );
		assert( abs( m33i( 1, 2 ) - ( -3.f ) ) < f32eps );
		assert( abs( m33i( 2, 2 ) - 1.f ) < f32eps );
		
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

#undef VUL_TEST_RNG
#undef VUL_TEST_FUZZ_COUNT

#endif
