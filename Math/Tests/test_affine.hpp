/*
 * Villains' Utility Library - Thomas Martin Schmid, 2014. Public domain¹
 *
 * This file contains tests for the Affine struct in vul_affine.hpp
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
		f32_t mat[ 64 ], vec[ 8 ], submat2[ 4 ], submat3[ 9 ], subvec2[ 2 ], subvec3[ 3 ];
		for( ui32_t i = 0; i < 64; ++i ) {
			mat[ i ] = VUL_TEST_RNG;
		}
		for( ui32_t i = 0; i < 8; ++i ) {
			vec[ i ] = VUL_TEST_RNG;
		}
		for( ui32_t c = 0; c < 2; ++c ) {
			for( ui32_t r = 0; r < 2; ++r ) {
				submat2[ c * 2 + r ] = mat[ c * 2 + r ];
			}
			subvec2[ c ] = vec[ c ];
		}
		for( ui32_t c = 0; c < 3; ++c ) {
			for( ui32_t r = 0; r < 3; ++r ) {
				submat3[ c * 3 + r ] = mat[ c * 3 + r ];
			}
			subvec3[ c ] = vec[ c ];
		}
#ifdef VUL_CPLUSPLUS11
		Affine< f32_t, 2 > i2, 
						   a2( i2 ), 
						   mv2( makeMatrix< f32_t, 2, 2 >( submat2 ), makeVector< f32_t, 2 >( subvec2 ) );
		Affine< f32_t, 3 > i3, 
						   a3( i3 ), 
						   mv3( makeMatrix< f32_t, 3, 3 >( submat3 ), makeVector< f32_t, 3 >( subvec3 ) );
		Affine< f32_t, 8 > i8, 
						   a8( i8 ), 
						   mv8( makeMatrix< f32_t, 8, 8 >( mat ), makeVector< f32_t, 8 >( vec ) );
#else
		Affine< f32_t, 2 > i2 = makeAffine< f32_t, 2 >( ), 
						   a2  = makeAffine< f32_t, 2 >( i2 ), 
						   mv2 = makeAffine< f32_t, 2 >( makeMatrix< f32_t, 2, 2 >( submat2 ), 
														 makeVector< f32_t, 2 >( subvec2 ) );
		Affine< f32_t, 3 > i3 = makeAffine< f32_t, 3 >( ), 
						   a3  = makeAffine< f32_t, 3 >( i3 ), 
						   mv3 = makeAffine< f32_t, 3 >( makeMatrix< f32_t, 3, 3 >( submat3 ), 
														 makeVector< f32_t, 3 >( subvec3 ) );
		Affine< f32_t, 8 > i8 = makeAffine< f32_t, 8 >( ), 
						   a8  = makeAffine< f32_t, 8 >( i8 ), 
						   mv8 = makeAffine< f32_t, 8 >( makeMatrix< f32_t, 8, 8 >( mat ), 
														 makeVector< f32_t, 8 >( vec ) );
#endif

		assert( i2.mat( 0, 0 ) == 1.f ); assert( a2.mat( 0, 0 ) == 1.f );
		assert( i2.mat( 1, 1 ) == 1.f ); assert( a2.mat( 1, 1 ) == 1.f );
		assert( i2.mat( 0, 1 ) == 0.f ); assert( a2.mat( 0, 1 ) == 0.f );
		assert( i2.mat( 1, 0 ) == 0.f ); assert( a2.mat( 1, 0 ) == 0.f );
		assert( i2.vec[ 0 ] == 0.f ); assert( a2.vec[ 0 ] == 0.f );
		assert( i2.vec[ 1 ] == 0.f ); assert( a2.vec[ 1 ] == 0.f );
		assert( mv2.mat( 0, 0 ) == mat[ 0 ] );
		assert( mv2.mat( 0, 1 ) == mat[ 1 ] );
		assert( mv2.mat( 1, 0 ) == mat[ 2 ] );
		assert( mv2.mat( 1, 1 ) == mat[ 3 ] );
		assert( mv2.vec[ 0 ] == vec[ 0 ] );
		assert( mv2.vec[ 1 ] == vec[ 1 ] );
		
		assert( i3.mat( 0, 0 ) == 1.f ); assert( a3.mat( 0, 0 ) == 1.f );
		assert( i3.mat( 1, 1 ) == 1.f ); assert( a3.mat( 1, 1 ) == 1.f );
		assert( i3.mat( 2, 2 ) == 1.f ); assert( a3.mat( 2, 2 ) == 1.f );
		assert( i3.mat( 0, 1 ) == 0.f ); assert( a3.mat( 0, 1 ) == 0.f );
		assert( i3.mat( 0, 2 ) == 0.f ); assert( a3.mat( 0, 2 ) == 0.f );
		assert( i3.mat( 1, 0 ) == 0.f ); assert( a3.mat( 1, 0 ) == 0.f );
		assert( i3.mat( 1, 2 ) == 0.f ); assert( a3.mat( 1, 2 ) == 0.f );
		assert( i3.mat( 2, 0 ) == 0.f ); assert( a3.mat( 2, 0 ) == 0.f );
		assert( i3.mat( 2, 1 ) == 0.f ); assert( a3.mat( 2, 1 ) == 0.f );
		assert( i3.vec[ 0 ] == 0.f ); assert( a3.vec[ 0 ] == 0.f );
		assert( i3.vec[ 1 ] == 0.f ); assert( a3.vec[ 1 ] == 0.f );
		assert( i3.vec[ 2 ] == 0.f ); assert( a3.vec[ 2 ] == 0.f );
		for( ui32_t i = 0; i < 3; ++i ) {
			for( ui32_t j = 0; j < 3; ++j ) {
				assert( mv3.mat( i, j ) == mat[ i * 3 + j ] );
			}
			assert( mv3.vec[ i ] == vec[ i ] );
		}

		for( ui32_t i = 0; i < 8; ++i ) {
			for( ui32_t j = 0; j < 8; ++j ) {
				if( i == j ) {
					assert( i8.mat( i, j ) == 1.f );
					assert( a8.mat( i, j ) == 1.f );
				} else {
					assert( i8.mat( i, j ) == 0.f ); 
					assert( a8.mat( i, j ) == 0.f );
				}
				assert( mv8.mat( i, j ) == mat[ i * 8 + j ] );
			}
			assert( i8.vec[ i ] == 0.f ); 
			assert( a8.vec[ i ] == 0.f );
		}

		return true;
	}
	bool TestAffine::transforms( )
	{		
		// We actually get proper testing of the functionality of this in TestAABB::transforms()
		// so what we test here is that Point and Vector functionality is different, as it should be.
#ifdef VUL_CPLUSPLUS11
		Affine< f32_t, 2 > i2,
						   mv2( makeMatrix22< f32_t >(  cos( VUL_PI / 4 ),  sin( VUL_PI / 4 ),
													   -sin( VUL_PI / 4 ),  cos( VUL_PI / 4 ) ),
						   Vector< f32_t, 2 >{ 0.f, 1.f } );
		Affine< f32_t, 3 > i3;
		Affine< f32_t, 8 > i8;
		Vector< f32_t, 2 > v2( 1.f );
		Vector< f32_t, 3 > v3( 1.f );
		Vector< f32_t, 8 > v8( 1.f );
		Point< f32_t, 2 > p2( 1.f );
		Point< f32_t, 3 > p3( 1.f );
		Point< f32_t, 8 > p8( 1.f );
#else
		Affine< f32_t, 2 > i2 = makeAffine< f32_t, 2 >( ), 
						   mv2 = makeAffine< f32_t, 2 >( makeMatrix22< f32_t >(  cos( ( f32_t )VUL_PI / 4.f ), sin( ( f32_t )VUL_PI / 4.f ),
																				-sin( ( f32_t )VUL_PI / 4.f ), cos( ( f32_t )VUL_PI / 4.f ) ),
														 makeVector< f32_t >( 0.f, 1.f ) );
		Affine< f32_t, 3 > i3 = makeAffine< f32_t, 3 >( );
		Affine< f32_t, 8 > i8 = makeAffine< f32_t, 8 >( );
		Vector< f32_t, 2 > v2 = makeVector< f32_t, 2 >( 1.f );
		Vector< f32_t, 3 > v3 = makeVector< f32_t, 3 >( 1.f );
		Vector< f32_t, 8 > v8 = makeVector< f32_t, 8 >( 1.f );
		Point< f32_t, 2 > p2 = makePoint< f32_t, 2 >( 1.f );
#endif
		f32_t f32eps = 1e-5f;
		
		// Test identity transfomrations
		v2 = i2 * v2;
		assert( v2[ 0 ] - 1.f < f32eps );
		assert( v2[ 1 ] - 1.f < f32eps );

		v3 = i3 * v3;
		assert( v3[ 0 ] - 1.f < f32eps );
		assert( v3[ 1 ] - 1.f < f32eps );
		assert( v3[ 2 ] - 1.f < f32eps );

		v8 = i8 * v8;
		for( ui32_t i = 0; i < 8; ++i ) {
			assert( v8[ i ] - 1.f < f32eps );
		}

		// Test transforms of points and vectors behaving differently (and correctly).
		v2 = mv2 * v2;
		p2 = mv2 * p2;
		assert( v2[ 0 ] - sqrt( 2.f ) < f32eps );
		assert( v2[ 1 ] - 0.f < f32eps );
		assert( p2[ 0 ] - sqrt( 2.f ) < f32eps );
		assert( p2[ 1 ] - 1.f < f32eps );

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
		Affine< f32_t, 3 > i3,
						   mv3( makeMatrix< f32_t, 3, 3 >( mat ), makeVector< f32_t, 3 >( vec ) );
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
				assert( rmv( i, j ) == mat[ i * 3 + j ] );
			}
		}
		assert( ri( 3, 3 ) == 1.f );
		assert( ri( 0, 3 ) == 0.f );
		assert( ri( 1, 3 ) == 0.f );
		assert( ri( 2, 3 ) == 0.f );
		assert( ri( 3, 0 ) == 0.f );
		assert( ri( 3, 1 ) == 0.f );
		assert( ri( 3, 2 ) == 0.f );
		
		assert( rmv( 3, 3 ) == 1.f );
		assert( rmv( 0, 3 ) == 0.f );
		assert( rmv( 1, 3 ) == 0.f );
		assert( rmv( 2, 3 ) == 0.f );
		assert( rmv( 3, 0 ) == vec[ 0 ] );
		assert( rmv( 3, 1 ) == vec[ 1 ] );
		assert( rmv( 3, 2 ) == vec[ 2 ] );
		
		return true;
	}
};

#undef VUL_TEST_RNG

#endif
