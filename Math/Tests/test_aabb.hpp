/*
 * Villains' Utility Library - Thomas Martin Schmid, 2014. Public domain¹
 *
 * This file contains tests for the AABB struct in vul_aabb.hpp
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
#ifndef VUL_TEST_AABB_HPP
#define VUL_TEST_AABB_HPP

#include <cassert>

#include "../vul_math.hpp"

using namespace vul;

namespace vul_test {
	class TestAABB {

	public:
		static bool test( );

	private:
		static bool make( );
		static bool assign( );
		static bool translates( );
		static bool scales( );
		static bool transforms( );
		static bool centers( );
		static bool extents( );
		static bool insides( );
		static bool transforms3D( );
	};

	bool TestAABB::test( )
	{
		assert( make( ) );
		assert( assign( ) );
		assert( translates( ) );
		assert( scales( ) );
		assert( centers( ) );
		assert( extents( ) );
		assert( insides( ) );
		assert( transforms( ) );
		assert( transforms3D( ) );

		return true;
	}
	bool TestAABB::make( )
	{
		// Test different dimensions
#ifdef VUL_CPLUSPLUS11
		AABB< f32_t, 3 > d3f32( Vector< f32_t >( -1.f, -2.f, -3.f ), Vector< f32_t >( 1.f, 2.f, 3.f ) );
		AABB< i16_t, 2 > d2i16( Vector< i16_t >( 10, -5 ), Vector< i16_t >( 11, -4 ) );
		AABB< fi32_t, 6 > d6fi32( Vector< fi32_t, 6 >( 7.f ), Vector< fi32_t, 6 >( 8.f ) );
#else
		AABB< f32_t, 3 > d3f32 = makeAABB< f32_t, 3 >( makeVector< f32_t >( -1.f, -2.f, -3.f ), makeVector< f32_t >( 1.f, 2.f, 3.f ) );
		AABB< i16_t, 2 > d2i16 = makeAABB< i16_t, 2 >( makeVector< i16_t >( 10, -5 ), makeVector< i16_t >( 11, -4 ) );
		AABB< fi32_t, 6 > d6fi32 = makeAABB< fi32_t, 6 >( makeVector< fi32_t, 6 >( 7.f ), makeVector< fi32_t, 6 >( 8.f ) );
#endif
		assert( d3f32._min[ 0 ] == -1.f );
		assert( d3f32._min[ 1 ] == -2.f );
		assert( d3f32._min[ 2 ] == -3.f );
		assert( d3f32._max[ 0 ] == 1.f );
		assert( d3f32._max[ 1 ] == 2.f );
		assert( d3f32._max[ 2 ] == 3.f );

		assert( d2i16._min[ 0 ] == ( i16_t )10 );
		assert( d2i16._min[ 1 ] == ( i16_t )-5 );
		assert( d2i16._max[ 0 ] == ( i16_t )11 );
		assert( d2i16._max[ 1 ] == ( i16_t )-4 );

		for( ui32_t i = 0; i < 6; ++i ) {
			assert( d6fi32._min[ i ] == fi32_t( 7.f ) );
			assert( d6fi32._max[ i ] == fi32_t( 8.f ) );
		}

		// Test different constructors
		fi32_t arr[ 6 ] = { fi32_t( -1.f ), fi32_t( -1.f ), fi32_t( -1.f ),
							fi32_t( 1.f ), fi32_t( 1.f ), fi32_t( 1.f ) };
		f32_t arr2[ 6 ] = { -1.f, -1.f, -1.f, 1.f, 1.f, 1.f };
		i32_t arr3[ 6 ] = { -1, -1, -1, 1, 1, 1 };
		
#ifdef VUL_CPLUSPLUS11
		AABB< f32_t, 3 > empty( );
		AABB< f32_t, 3 > pt( makePoint< f32_t, 3 >( -1.f ), makePoint< f32_t, 3 >( 1.f ) );
		AABB< fi32_t, 3 > arrT( arr );
		AABB< fi32_t, 3 > arrF( arr2 );
		AABB< f32_t, 3 > arrI( arr3 );
#else
		AABB< f32_t, 3 > empty = makeAABB< f32_t, 3 >( );
		AABB< f32_t, 3 > pt = makeAABB< f32_t, 3 >( makePoint< f32_t, 3 >( -1.f ), makePoint< f32_t, 3 >( 1.f ) );
		AABB< fi32_t, 3 > arrT = makeAABB< fi32_t, 3 >( arr );
		AABB< fi32_t, 3 > arrF = makeAABB< fi32_t, 3 >( arr2 );
		AABB< f32_t, 3 > arrI = makeAABB< f32_t, 3 >( arr3 );
#endif
		assert( empty._min[ 0 ] == 0.f );
		assert( empty._min[ 1 ] == 0.f );
		assert( empty._min[ 2 ] == 0.f );
		assert( empty._max[ 0 ] == 0.f );
		assert( empty._max[ 1 ] == 0.f );
		assert( empty._max[ 2 ] == 0.f );
		
		assert( pt._min[ 0 ] == -1.f );
		assert( pt._min[ 1 ] == -1.f );
		assert( pt._min[ 2 ] == -1.f );
		assert( pt._max[ 0 ] ==  1.f );
		assert( pt._max[ 1 ] ==  1.f );
		assert( pt._max[ 2 ] ==  1.f );

		assert( arrT._min[ 0 ] == fi32_t( -1.f ) 
			 && arrT._min[ 1 ] == fi32_t( -1.f ) 
			 && arrT._min[ 2 ] == fi32_t( -1.f ) );
		assert( arrT._max[ 0 ] == fi32_t(  1.f )
			 && arrT._max[ 1 ] == fi32_t(  1.f )
			 && arrT._max[ 2 ] == fi32_t(  1.f ) );
		
		assert( arrF._min[ 0 ] == fi32_t( -1.f ) 
			 && arrF._min[ 1 ] == fi32_t( -1.f ) 
			 && arrF._min[ 2 ] == fi32_t( -1.f ) );
		assert( arrF._max[ 0 ] == fi32_t(  1.f )
			 && arrF._max[ 1 ] == fi32_t(  1.f )
			 && arrF._max[ 2 ] == fi32_t(  1.f ) );
		
		assert( arrI._min[ 0 ] == -1 );
		assert( arrI._min[ 1 ] == -1 );
		assert( arrI._min[ 2 ] == -1 );
		assert( arrI._max[ 0 ] ==  1 );
		assert( arrI._max[ 1 ] ==  1 );
		assert( arrI._max[ 2 ] ==  1 );
		
		return true;
	}
	bool TestAABB::assign( )
	{
#ifdef VUL_CPLUSPLUS11
		AABB< f32_t, 6 > a( Vector< f32_t, 6 >( 7.f ), Vector< f32_t, 6 >( 8.f ) );
		a = AABB< f32_t >( makeVector< f32_t, 6 >( 5.f ), makeVector< f32_t, 6 >( 9.f ) );
#else
		AABB< f32_t, 6 > a = makeAABB< f32_t >( makeVector< f32_t, 6 >( 7.f ), makeVector< f32_t, 6 >( 8.f ) );
		a = makeAABB< f32_t >( makeVector< f32_t, 6 >( 5.f ), makeVector< f32_t, 6 >( 9.f ) );
#endif
		for( ui32_t i = 0; i < 6; ++i ) {
			assert( a._min[ i ] == 5.f );
			assert( a._max[ i ] == 9.f );
		}

		return true;
	}
	bool TestAABB::translates( )
	{
		// Test 2D, 3D, 8D vs known cases.
#ifdef VUL_CPLUSPLUS11
		AABB< f32_t, 2 > d2( Vector< f32_t >( 0.f, 1.f ), Vector< f32_t >( 1.f, 2.f ) );
		AABB< f32_t, 3 > d3( Vector< f32_t, 3 >( 0.f ), Vector< f32_t, 3 >( 1.f ) );
		AABB< f32_t, 8 > d8( Vector< f32_t, 8 >( 0.f ), Vector< f32_t, 8 >( 1.f ) );
		Vector< f32_t, 2 > v2( 1.f, 2.f );
		Vector< f32_t, 3 > v3( 1.f );
		Vector< f32_t, 8 > v8( 1.f );
#else
		AABB< f32_t, 2 > d2 = makeAABB< f32_t >( makeVector< f32_t >( 0.f, 1.f ), makeVector< f32_t >( 1.f, 2.f ) );
		AABB< f32_t, 3 > d3 = makeAABB< f32_t >( makeVector< f32_t, 3 >( 0.f ), makeVector< f32_t, 3 >( 1.f ) );
		AABB< f32_t, 8 > d8 = makeAABB< f32_t >( makeVector< f32_t, 8 >( 0.f ), makeVector< f32_t, 8 >( 1.f ) );
		Vector< f32_t, 2 > v2 = makeVector< f32_t >( 1.f, 2.f );
		Vector< f32_t, 3 > v3 = makeVector< f32_t, 3 >( 1.f );
		Vector< f32_t, 8 > v8 = makeVector< f32_t, 8 >( -1.f );
#endif
		d2 = translate( d2, v2 );
		d3 = translate( d3, v3 );
		d8 = translate( d8, v8 );

		assert( d2._min[ 0 ] == 1.f );
		assert( d2._min[ 1 ] == 3.f );
		assert( d2._max[ 0 ] == 2.f );
		assert( d2._max[ 1 ] == 4.f );

		for( ui32_t i = 0; i < 3; ++i ) {
			assert( d3._min[ i ] == 1.f );
			assert( d3._max[ i ] == 2.f );
		}
		
		for( ui32_t i = 0; i < 8; ++i ) {
			assert( d8._min[ i ] == -1.f );
			assert( d8._max[ i ] == 0.f );
		}

		return true;
	}
	bool TestAABB::scales( )
	{
		// Test 2D, 3D, 8D vs known cases.
#ifdef VUL_CPLUSPLUS11
		AABB< f32_t, 2 > d2( Vector< f32_t >( 0.f, 1.f ), Vector< f32_t >( 1.f, 2.f ) );
		AABB< f32_t, 3 > d3( Vector< f32_t, 3 >( 0.f ), Vector< f32_t, 3 >( 1.f ) );
		AABB< f32_t, 8 > d8( Vector< f32_t, 8 >( -1.f ), Vector< f32_t, 8 >( 1.f ) );
		Vector< f32_t, 2 > v2( -1.f, 2.f );
		Vector< f32_t, 3 > v3( -1.f );
		Vector< f32_t, 8 > v8( 2.f );
#else
		AABB< f32_t, 2 > d2 = makeAABB< f32_t >( makeVector< f32_t >( 0.f, 1.f ), makeVector< f32_t >( 1.f, 2.f ) );
		AABB< f32_t, 3 > d3 = makeAABB< f32_t >( makeVector< f32_t, 3 >( 0.f ), makeVector< f32_t, 3 >( 1.f ) );
		AABB< f32_t, 8 > d8 = makeAABB< f32_t >( makeVector< f32_t, 8 >( -1.f ), makeVector< f32_t, 8 >( 1.f ) );
		Vector< f32_t, 2 > v2 = makeVector< f32_t >( -1.f, 2.f );
		Vector< f32_t, 3 > v3 = makeVector< f32_t, 3 >( -1.f );
		Vector< f32_t, 8 > v8 = makeVector< f32_t, 8 >( 2.f );
#endif
		d2 = scale( d2, v2 );
		d3 = scale( d3, v3 );
		d8 = scale( d8, v8 );

		assert( d2._min[ 0 ] == 0.f );
		assert( d2._min[ 1 ] == 2.f );
		assert( d2._max[ 0 ] == -1.f );
		assert( d2._max[ 1 ] == 4.f );

		for( ui32_t i = 0; i < 3; ++i ) {
			assert( d3._min[ i ] == 0.f );
			assert( d3._max[ i ] == -1.f );
		}
		
		for( ui32_t i = 0; i < 8; ++i ) {
			assert( d8._min[ i ] == -2.f );
			assert( d8._max[ i ] == 2.f );
		}

		return true;
	}
	bool TestAABB::centers( )
	{
		// Test 2D, 3D, 8D vs known cases.
#ifdef VUL_CPLUSPLUS11
		AABB< f32_t, 2 > d2( Vector< f32_t >( 0.f, 1.f ), Vector< f32_t >( 1.f, 2.f ) );
		AABB< f32_t, 3 > d3( Vector< f32_t, 3 >( 0.f ), Vector< f32_t, 3 >( 1.f ) );
		AABB< f32_t, 8 > d8( Vector< f32_t, 8 >( -1.f ), Vector< f32_t, 8 >( 1.f ) );
#else
		AABB< f32_t, 2 > d2 = makeAABB< f32_t >( makeVector< f32_t >( 0.f, 1.f ), makeVector< f32_t >( 1.f, 2.f ) );
		AABB< f32_t, 3 > d3 = makeAABB< f32_t >( makeVector< f32_t, 3 >( 0.f ), makeVector< f32_t, 3 >( 1.f ) );
		AABB< f32_t, 8 > d8 = makeAABB< f32_t >( makeVector< f32_t, 8 >( -1.f ), makeVector< f32_t, 8 >( 1.f ) );
#endif
		Point< f32_t, 2 > p2 = center( d2 );
		Point< f32_t, 3 > p3 = center( d3 );
		Point< f32_t, 8 > p8 = center( d8 );

		assert( p2[ 0 ] == 0.5f );
		assert( p2[ 1 ] == 1.5f );

		for( ui32_t i = 0; i < 3; ++i ) {
			assert( p3[ i ] == 0.5f );
		}
		
		for( ui32_t i = 0; i < 8; ++i ) {
			assert( p8[ i ] == 0.f );
		}

		return true;
	}
	bool TestAABB::extents( )
	{
		// Test 2D, 3D, 8D vs known cases.
#ifdef VUL_CPLUSPLUS11
		AABB< f32_t, 2 > d2( Vector< f32_t >( 0.f, 1.f ), Vector< f32_t >( 1.f, 2.f ) );
		AABB< f32_t, 3 > d3( Vector< f32_t, 3 >( 0.f ), Vector< f32_t, 3 >( 1.f ) );
		AABB< f32_t, 8 > d8( Vector< f32_t, 8 >( -1.f ), Vector< f32_t, 8 >( 1.f ) );
#else
		AABB< f32_t, 2 > d2 = makeAABB< f32_t >( makeVector< f32_t >( 0.f, 1.f ), makeVector< f32_t >( 1.f, 2.f ) );
		AABB< f32_t, 3 > d3 = makeAABB< f32_t >( makeVector< f32_t, 3 >( 0.f ), makeVector< f32_t, 3 >( 1.f ) );
		AABB< f32_t, 8 > d8 = makeAABB< f32_t >( makeVector< f32_t, 8 >( -1.f ), makeVector< f32_t, 8 >( 1.f ) );
#endif
		Point< f32_t, 2 > p2 = center( d2 );
		Point< f32_t, 3 > p3 = center( d3 );
		Point< f32_t, 8 > p8 = center( d8 );

		assert( p2[ 0 ] == 1.f );
		assert( p2[ 1 ] == 1.f );

		for( ui32_t i = 0; i < 3; ++i ) {
			assert( p3[ i ] == 1.f );
		}
		
		for( ui32_t i = 0; i < 8; ++i ) {
			assert( p8[ i ] == 2.f );
		}

		return true;
	}
	bool TestAABB::insides( )
	{
		// Test 2D, 3D, 8D vs known cases.
#ifdef VUL_CPLUSPLUS11
		AABB< f32_t, 2 > d2( Vector< f32_t >( 0.f, 1.f ), Vector< f32_t >( 1.f, 2.f ) );
		AABB< f32_t, 3 > d3( Vector< f32_t, 3 >( 0.f ), Vector< f32_t, 3 >( 1.f ) );
		AABB< f32_t, 8 > d8( Vector< f32_t, 8 >( -1.f ), Vector< f32_t, 8 >( 0.f ) );
#else
		AABB< f32_t, 2 > d2 = makeAABB< f32_t >( makeVector< f32_t >( 0.f, 1.f ), makeVector< f32_t >( 1.f, 2.f ) );
		AABB< f32_t, 3 > d3 = makeAABB< f32_t >( makeVector< f32_t, 3 >( 0.f ), makeVector< f32_t, 3 >( 1.f ) );
		AABB< f32_t, 8 > d8 = makeAABB< f32_t >( makeVector< f32_t, 8 >( -1.f ), makeVector< f32_t, 8 >( 0.f ) );
#endif
		Point< f32_t, 2 > p2 = center( d2 );
		Point< f32_t, 3 > p3 = center( d3 );
		Point< f32_t, 8 > p8 = center( d8 );

		// Check that the centers are inside
		assert( inside( d2, p2 ) );
		assert( inside( d3, p3 ) );
		assert( inside( d8, p8 ) );

		// For each component, invert it, which puts the point outside our box, and test it is not inside.
		for( ui32_t i = 0; i < 2; ++i ) {
			p2[ i ] = -p2[ i ];
			assert( !inside( d2, p2 ) );
			p2[ i ] = -p2[ i ];
		}

		for( ui32_t i = 0; i < 3; ++i ) {
			p3[ i ] = -p3[ i ];
			assert( !inside( d3, p3 ) );
			p3[ i ] = -p3[ i ];
		}

		for( ui32_t i = 0; i < 8; ++i ) {
			p8[ i ] = -p8[ i ];
			assert( !inside( d8, p8 ) );
			p8[ i ] = -p8[ i ];
		}

		// Finally, test that edges are inside. One case is enough here
		p3 = makePoint< f32_t, 3 >( 0.f );
		assert( inside( d3, p3 ) );

		return true;
	}
	bool TestAABB::transforms( )
	{
		// We test 2D and 3D case
#ifdef VUL_CPLUSPLUS11
		AABB< f32_t, 2 > d2( Vector< f32_t >( 0.f, 1.f ), Vector< f32_t >( 1.f, 2.f ) );
		AABB< f32_t, 3 > d3( Vector< f32_t, 3 >( 0.f ), Vector< f32_t, 3 >( 1.f ) );
		Affine< f32_t, 2 > a2( Matrix< f32_t, 2, 2 >( rotate ), Vector< f32_t, 2 >( translate ) );
		Affine< f32_t, 3 > a3 = makeAffine3D( Vector< f32_t, 3 >( translate ),
											  Vector< f32_t, 3 >( scale ),
											  Quaternion< f32_t >( rotate ) );
#else
		AABB< f32_t, 2 > d2 = makeAABB< f32_t >( makeVector< f32_t, 2 >( -1.f ), makeVector< f32_t, 2 >( 1.f ) );
		AABB< f32_t, 3 > d3 = makeAABB< f32_t >( makeVector< f32_t, 3 >( -1.f ), makeVector< f32_t, 3 >( 1.f ) );
		// Rotate CW 45 degrees and move 1 unit towards negative x and positive y..
		Affine< f32_t, 2 > a2 = makeAffine< f32_t, 2 >( makeMatrix22< f32_t >( cos( ( f32_t )VUL_PI / 6 ), -sin( ( f32_t )VUL_PI / 6 ),
																			   sin( ( f32_t )VUL_PI / 6 ),  cos( ( f32_t )VUL_PI / 6 ) ), 
														makeVector< f32_t >( -1.f, 1.f ) );
		// Rotate CCW 45 degrees around Z, scale (2,-2,1), translate +1 in all directions.
		Affine< f32_t, 3 > a3 = makeAffine3D< f32_t >( makeVector< f32_t, 3 >( 1.f ), 
													   makeVector< f32_t >( 2.f, -2.f, 1.f ), 
													   makeQuatFromAxisAngle< f32_t >( makeVector< f32_t >( 0.f, 0.f, 1.f ), ( f32_t )-VUL_PI / 4.f ) );
#endif
		AABB< f32_t, 2 > r2;
		AABB< f32_t, 3 > r3;
		r2 = transform( d2, a2 );
		r3 = transform( d3, a3 );
		
		// The rotate means we now have corners at sqrt(2) distance.

		assert( r2._min[ 0 ] == -sqrt( 2.f ) - 1.f );
		assert( r2._min[ 1 ] == -sqrt( 2.f ) + 1.f );
		assert( r2._max[ 0 ] == sqrt( 2.f ) - 1.f );
		assert( r2._max[ 1 ] == sqrt( 2.f ) + 1.f );
				
		assert( r3._min[ 0 ] == -sqrt( 2.f ) * 2.f + 1.f );
		assert( r3._min[ 1 ] == sqrt( 2.f ) * -2.f + 1.f );
		assert( r3._min[ 2 ] == -sqrt( 2.f ) * 1.f + 1.f );
		assert( r3._min[ 0 ] == sqrt( 2.f ) * 2.f + 1.f );
		assert( r3._min[ 1 ] == -sqrt( 2.f ) * -2.f + 1.f );
		assert( r3._min[ 2 ] == sqrt( 2.f ) * 1.f + 1.f );

		return true;
	}

	bool TestAABB::transforms3D( )
	{
		AABB< __m128, 3 > m128[ 4 ], r128[ 4 ];
		AABB< __m128d, 3 > m128d[ 2 ], r128d[ 2 ];
		AABB< __m256, 3 > m256[ 8 ], r256[ 8 ];
		AABB< __m256d, 3 > m256d[ 4 ], r256d[ 4 ];
		AABB< f32_t, 3 > in32[ 8 ];
		AABB< f64_t, 3 > in64[ 4 ];
		f32_t mat[ 9 ], vec[ 3 ], aabb_min[ 3 ], aabb_max[ 3 ];
		
		// Rotate 45 deg around y
		mat[ 0 ] = cos( ( f32_t )VUL_PI / 6 );
		mat[ 1 ] = 0;
		mat[ 2 ] = sin( ( f32_t )VUL_PI / 6 );
		mat[ 3 ] = 0;
		mat[ 4 ] = 1;
		mat[ 5 ] = 0;
		mat[ 6 ] = -sin( ( f32_t )VUL_PI / 6 );
		mat[ 7 ] = 0;
		mat[ 8 ] = cos( ( f32_t )VUL_PI / 6 );
		// Translate (1, 2, -4)
		vec[ 0 ] = 1.f;
		vec[ 1 ] = 2.f;
		vec[ 2 ] = -4.f;
		// -1, 1 AABB
		aabb_min[ 0 ] = aabb_min[ 1 ] = aabb_min[ 2 ] = -1.f;
		aabb_max[ 0 ] = aabb_max[ 1 ] = aabb_max[ 2 ] = 1.f;
		// At this point, we have asserted that the normal transform works, so use that as a reference
		AABB< f32_t, 3 > ref32_in, ref32_out;
		AABB< f64_t, 3 > ref64_in, ref64_out;
		
#ifdef VUL_CPLUSPLUS11
		ref32_in._min = Point< f32_t, 3 >( aabb_min );
		ref32_in._max = Point< f32_t, 3 >( aabb_max );
		ref64_in._min = Point< f64_t, 3 >( aabb_min );
		ref64_in._max = Point< f64_t, 3 >( aabb_max );
		
		Affine< f64_t, 3 > a64 = Affine< f64_t, 3 >( Matrix< f64_t, 3, 3 >( mat ), Vector< f64_t, 3 >( vec ) );
		Affine< f32_t, 3 > a32 = Affine< f32_t, 3 >( Matrix< f32_t, 3, 3 >( mat ), Vector< f32_t, 3 >( vec ) );

		in32[ 0 ] = AABB< f32_t, 3 >( Point< f32_t, 3 >( aabb_min ), Point< f32_t, 3 >( aabb_max ) );
		in64[ 0 ] = AABB< f64_t, 3 >( Point< f64_t, 3 >( aabb_min ), Point< f64_t, 3 >( aabb_max ) );
#else
		ref32_in._min = makePoint< f32_t, 3 >( aabb_min );
		ref32_in._max = makePoint< f32_t, 3 >( aabb_max );
		ref64_in._min = makePoint< f64_t, 3 >( aabb_min );
		ref64_in._max = makePoint< f64_t, 3 >( aabb_max );
		
		Affine< f64_t, 3 > a64 = makeAffine< f64_t, 3 >( makeMatrix< f64_t, 3, 3 >( mat ), makeVector< f64_t, 3 >( vec ) );
		Affine< f32_t, 3 > a32 = makeAffine< f32_t, 3 >( makeMatrix< f32_t, 3, 3 >( mat ), makeVector< f32_t, 3 >( vec ) );

		in32[ 0 ] = makeAABB< f32_t, 3 >( makePoint< f32_t, 3 >( aabb_min ), makePoint< f32_t, 3 >( aabb_max ) );
		in64[ 0 ] = makeAABB< f64_t, 3 >( makePoint< f64_t, 3 >( aabb_min ), makePoint< f64_t, 3 >( aabb_max ) );
#endif
		// Transform the reference
		ref32_out = transform( ref32_in, a32 );
		ref64_out = transform( ref64_in, a64 );
		
		// Pack, transform.
		pack( &m128[ 0 ], &in32[ 0 ], 1 );
		pack( &m128d[ 0 ], &in64[ 0 ], 1 );
		pack( &m256[ 0 ], &in32[ 0 ], 1 );
		pack( &m256d[ 0 ], &in64[ 0 ], 1 );

		transform3D( &r128[ 0 ], &m128[ 0 ], a32, 1 );
		transform3D( &r128d[ 0 ], &m128d[ 0 ], a64, 1 );
		transform3D( &r256[ 0 ], &m256[ 0 ], a32, 1 );
		transform3D( &r256d[ 0 ], &m256d[ 0 ], a64, 1 );
		
		//unpack the vectorized ones and compare to reference
		unpack( &in32[ 0 ], &m128[ 0 ], 1 );
		unpack( &in64[ 0 ], &m128d[ 0 ], 1 );

		assert( all( in32[ 0 ]._min == ref32_out._min ) );
		assert( all( in32[ 0 ]._max == ref32_out._max ) );
		assert( all( in64[ 0 ]._min == ref64_out._min ) );
		assert( all( in64[ 0 ]._max == ref64_out._max ) );

		unpack( &in32[ 0 ], &m256[ 0 ], 1 );
		unpack( &in64[ 0 ], &m256d[ 0 ], 1 );

		assert( all( in32[ 0 ]._min == ref32_out._min ) );
		assert( all( in32[ 0 ]._max == ref32_out._max ) );
		assert( all( in64[ 0 ]._min == ref64_out._min ) );
		assert( all( in64[ 0 ]._max == ref64_out._max ) );

		return true;
	}
};

#endif