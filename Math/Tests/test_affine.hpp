/*
 * Villains' Utility Library - Thomas Martin Schmid, 2014. Public domain�
 *
 * This file contains tests for the Affine struct in vul_affine.hpp
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
#ifndef VUL_TEST_AFFINE_HPP
#define VUL_TEST_AFFINE_HPP

#include <cassert>

#include "../vul_math.hpp"

#define VUL_TEST_RNG ( float )( ( float )rand( ) / ( float )RAND_MAX )

using namespace vul;

namespace vul_test {
	class TestAffine {

	public:
		static bool test( );

	private:
		static bool make( );
		static bool transforms( ); // Point and vector rhs multiplications
		static bool makeMat44( );  // Test that the expected homogenous affine transofmration matrix is built
	};

	bool TestAffine::test( )
	{
		assert( make( ) );
		assert( transforms( ) );
		assert( makeMat44( ) );

		return true;
	}

	bool TestAffine::make( )
	{
		f32_t mat[ 64 ], vec[ 8 ];
		for( ui32_t i = 0; i < 64; ++i ) {
			mat[ i ] = VUL_TEST_RNG;
		}
		for( ui32_t i = 0; i < 8; ++i ) {
			vec[ i ] = VUL_TEST_RNG;
		}
#ifdef VUL_CPLUSPLUS11
		Affine< f32_t, 2 > i2( ), a2( i2 ), mv2( Matrix< f32_t, 2, 2 >( mat ), Vector< f32_t, 2 >( vec ) );
		Affine< f32_t, 3 > i3( ), a3( i3 ), mv3( Matrix< f32_t, 3, 3 >( mat ), Vector< f32_t, 3 >( vec ) );
		Affine< f32_t, 8 > i8( ), a8( i8 ), mv8( Matrix< f32_t, 8, 8 >( mat ), Vector< f32_t, 8 >( vec ) );
#else
		Affine< f32_t, 2 > i2 = makeAffine< f32_t, 2 >( ), 
						   a2  = makeAffine< f32_t, 2 >( i2 ), 
						   mv2 = makeAffine< f32_t, 2 >( makeMatrix< f32_t, 2, 2 >( mat ), 
														 makeVector< f32_t, 2 >( vec ) );
		Affine< f32_t, 3 > i3 = makeAffine< f32_t, 2 >( ), 
						   a3  = makeAffine< f32_t, 3 >( i3 ), 
						   mv3 = makeAffine< f32_t, 3 >( makeMatrix< f32_t, 3, 3 >( mat ), 
														 makeVector< f32_t, 3 >( vec ) );
		Affine< f32_t, 8 > i8 = makeAffine< f32_t, 8 >( ), 
						   a8  = makeAffine< f32_t, 8 >( i8 ), 
						   mv8 = makeAffine< f32_t, 8 >( makeMatrix< f32_t, 8, 8 >( mat ), 
														 makeVector< f32_t, 8 >( vec ) );
#endif

		assert( i2.mat( 0, 0 ) == a2.mat( 0, 0 ) == 1.f );
		assert( i2.mat( 1, 1 ) == a2.mat( 1, 1 ) == 1.f );
		assert( i2.mat( 0, 1 ) == a2.mat( 0, 1 ) == 0.f );
		assert( i2.mat( 1, 0 ) == a2.mat( 1, 0 ) == 0.f );
		assert( i2.vec[ 0 ] == a2.vec[ 0 ] == 0.f );
		assert( i2.vec[ 1 ] == a2.vec[ 1 ] == 0.f );
		assert( mv2.mat( 0, 0 ) == mat[ 0 ] );
		assert( mv2.mat( 0, 1 ) == mat[ 1 ] );
		assert( mv2.mat( 1, 0 ) == mat[ 2 ] );
		assert( mv2.mat( 1, 1 ) == mat[ 3 ] );
		assert( mv2.vec[ 0 ] == vec[ 0 ] );
		assert( mv2.vec[ 1 ] == vec[ 2 ] );
		
		assert( i3.mat( 0, 0 ) == a3.mat( 0, 0 ) == 1.f );
		assert( i3.mat( 1, 1 ) == a3.mat( 1, 1 ) == 1.f );
		assert( i3.mat( 2, 2 ) == a3.mat( 2, 2 ) == 1.f );
		assert( i3.mat( 0, 1 ) == a3.mat( 0, 1 ) == 0.f );
		assert( i3.mat( 0, 2 ) == a3.mat( 0, 2 ) == 0.f );
		assert( i3.mat( 1, 0 ) == a3.mat( 1, 0 ) == 0.f );
		assert( i3.mat( 1, 2 ) == a3.mat( 1, 2 ) == 0.f );
		assert( i3.mat( 2, 0 ) == a3.mat( 2, 0 ) == 0.f );
		assert( i3.mat( 2, 1 ) == a3.mat( 2, 1 ) == 0.f );
		assert( i3.vec[ 0 ] == a3.vec[ 0 ] == 0.f );
		assert( i3.vec[ 1 ] == a3.vec[ 1 ] == 0.f );
		assert( i3.vec[ 2 ] == a3.vec[ 2 ] == 0.f );
		for( ui32_t i = 0; i < 3; ++i ) {
			for( ui32_t j = 0; j < 3; ++j ) {
				assert( mv3.mat( i, j ) == mat[ i * 3 + j ] );
			}
			assert( mv3.vec[ i ] == vec[ i ] );
		}

		for( ui32_t i = 0; i < 8; ++i ) {
			for( ui32_t j = 0; j < 8; ++j ) {
				if( i == j ) {
					assert( i8.mat( i, j ) == a8.mat( i, j ) == 1.f );
				} else {
					assert( i8.mat( i, j ) == a8.mat( i, j ) == 0.f );
				}
				assert( mv8.mat( i, j ) == mat[ i * 8 + j ];
			}
			assert( i8.vec[ i ] == a8.vec[ i ] == 0.f );
		}

		return true;
	}
	bool TestAffine::transforms( )
	{		
		// We actually get proper testing of the functionality of this in TestAABB::transforms()
		// so what we test here is that Point and Vector functionality is different, as it should be.
#ifdef VUL_CPLUSPLUS11
		Affine< f32_t, 2 > i2( ), mv2( makeMatrix22< f32_t >( cos( VUL_PI / 4 ), -sin( VUL_PI / 4 ),
															  sin( VUL_PI / 4 ),  cos( VUL_PI / 4 ) ),
									   Vector< f32_t, 2 >( vec ) );
		Affine< f32_t, 3 > i3( );
		Affine< f32_t, 8 > i8( );
		Vector< f32_t, 2 > v2( 1.f );
		Vector< f32_t, 3 > v3( 1.f );
		Vector< f32_t, 8 > v8( 1.f );
		Point< f32_t, 2 > p2( 1.f );
		Point< f32_t, 3 > p3( 1.f );
		Point< f32_t, 8 > p8( 1.f );
#else
		Affine< f32_t, 2 > i2 = makeAffine< f32_t, 2 >( ), 
						   mv2 = makeAffine< f32_t, 2 >( makeMatrix22< f32_t >( cos( VUL_PI / 4 ), -sin( VUL_PI / 4 ),
																				sin( VUL_PI / 4 ),  cos( VUL_PI / 4 ) ),
														 makeVector< f32_t >( 0.f, 1.f ) );
		Affine< f32_t, 3 > i3 = makeAffine< f32_t, 2 >( );
		Affine< f32_t, 8 > i8 = makeAffine< f32_t, 8 >( );
		Vector< f32_t, 2 > v2 = makeVector( 1.f );
		Vector< f32_t, 3 > v3 = makeVector( 1.f );
		Vector< f32_t, 8 > v8 = makeVector( 1.f );
		Point< f32_t, 2 > p2 = makePoint( 1.f );
		Point< f32_t, 3 > p3 = makePoint( 1.f );
		Point< f32_t, 8 > p8 = makePoint( 1.f );
#endif
		// Test identity transfomrations
		v2 = i2 * v2;
		assert( v2[ 0 ] == v2[ 1 ] == 1.f );

		v3 = i3 * v3;
		assert( v3[ 0 ] == v3[ 1 ] == v3[ 2 ] == 1.f );

		v8 = i8 * v8;
		for( ui32_t i = 0; i < 8; ++i ) {
			assert( v8[ i ] == 1.f );
		}

		// Test transforms of points and vectors behaving differently (and correctly).
		v2 = mv2 * v2;
		p2 = mv2 * p2;
		assert( v2[ 0 ] == sqrt( 2.f ) );
		assert( v2[ 1 ] == 0.f );
		assert( p2[ 0 ] == sqrt( 2.f ) );
		assert( p2[ 1 ] == 1.f );

		return true;
	}
	bool TestAffine::makeMat44( )
	{
		f32_t mat[ 9 ], vec[ 3 ];
		for( ui32_t i = 0; i < 3; ++i )	{
			for( ui32_t j = 0; j < 3; ++j )	{
				mat[ i * 3 + j ] = VUL_TEST_RNG;
			}
			vec[ i ] = VUL_TEST_RNG;
		}
#ifdef VUL_CPLUSPLUS11
		Affine< f32_t, 3 > i3( ), mv3( Matrix< f32_t, 3, 3 >( mat ), Vector< f32_t, 3 >( vec ) );
#else
		Affine< f32_t, 3 > i3 = makeAffine< f32_t, 3 >( ), 
						   mv3 = makeAffine< f32_t, 3 >( makeMatrix< f32_t, 3, 3 >( mat ),
														 makeVector< f32_t, 3 >( vec ) );
#endif
		Matrix< f32_t, 4, 4 > ri = makeHomogeneousFromAffine( i3 );
		Matrix< f32_t, 4, 4 > rmv = makeHomogeneousFromAffine( mv3 );

		for( ui32_t i = 0; i < 3; ++i )	{
			for( ui32_t j = 0; j < 3; ++j )	{
				if( i == j ) {
					assert( ri( i, j ) == 1.f );
				} else {
					assert( ri( i, j ) == 0.f );
				}
				assert( rmv( i, j ) == mat[ i * 3 + j ];
			}
		}
		assert( ri( 3, 3 ) == 0.f );
		assert( ri( 0, 3 ) == ri( 1, 3 ) == ri( 2, 3 ) == ri( 3, 0 ) == ri( 3, 1 ) == ri( 3, 2 ) == 0.f );
		
		assert( rmv( 3, 3 ) == 0.f );
		assert( rmv( 0, 3 ) == rmv( 1, 3 ) == rmv( 2, 3 ) == 0.f );
		assert( rmv( 3, 0 ) == vec[ 0 ] );
		assert( rmv( 3, 1 ) == vec[ 1 ] );
		assert( rmv( 3, 2 ) == vec[ 2 ] );
		
		return true;
	}
};

#endif