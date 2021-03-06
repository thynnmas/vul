/*
 * Villains' Utility Library - Thomas Martin Schmid, 2017. Public domain?
 *
 * This file contains tests for the AABB struct in vul_aabb.hpp
 * 
 * ? If public domain is not legally valid in your legal jurisdiction
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

#pragma warning(disable: 6001)

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
		static bool containment( );
		static bool unions( );
		static bool instersects( );
		static bool frustum_tests( );
		static bool transforms3D( );
		static bool inside_tests( );
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
		assert( containment( ) );
		assert( unions( ) );
		assert( instersects( ) );
		assert( transforms( ) );
		assert( frustum_tests( ) );
#if defined( VUL_AOSOA_SSE ) || defined( VUL_AOSOA_AVX ) || defined( VUL_AOSOA_NEON )
		assert( transforms3D( ) );
		assert( inside_tests( ) );
#endif

		return true;
	}
	bool TestAABB::make( )
	{
		// Test different dimensions
#ifdef VUL_CPLUSPLUS11
		AABB< f32, 3 > d3f32( Vector< f32, 3 >{ -1.f, -2.f, -3.f }, Vector< f32, 3 >{ 1.f, 2.f, 3.f } );
		AABB< s16, 2 > d2i16( Vector< s16, 2 >{ 10, -5}, Vector< s16, 2 >{ 11, -4 } );
		AABB< fi32, 6 > d6fi32( Vector< fi32, 6 >( ( fi32 )7.f ), Vector< fi32, 6 >( ( fi32 )8.f ) );
#else
		AABB< f32, 3 > d3f32 = makeAABB< f32, 3 >( makeVector< f32 >( -1.f, -2.f, -3.f ), makeVector< f32 >( 1.f, 2.f, 3.f ) );
		AABB< s16, 2 > d2i16 = makeAABB< s16, 2 >( makeVector< s16 >( 10, -5 ), makeVector< s16 >( 11, -4 ) );
		AABB< fi32, 6 > d6fi32 = makeAABB< fi32, 6 >( makeVector< fi32, 6 >( 7.f ), makeVector< fi32, 6 >( 8.f ) );
#endif
		assert( d3f32._min[ 0 ] == -1.f );
		assert( d3f32._min[ 1 ] == -2.f );
		assert( d3f32._min[ 2 ] == -3.f );
		assert( d3f32._max[ 0 ] == 1.f );
		assert( d3f32._max[ 1 ] == 2.f );
		assert( d3f32._max[ 2 ] == 3.f );

		assert( d2i16._min[ 0 ] == ( s16 )10 );
		assert( d2i16._min[ 1 ] == ( s16 )-5 );
		assert( d2i16._max[ 0 ] == ( s16 )11 );
		assert( d2i16._max[ 1 ] == ( s16 )-4 );

		for( u32 i = 0; i < 6; ++i ) {
			assert( d6fi32._min[ i ] == fi32( 7.f ) );
			assert( d6fi32._max[ i ] == fi32( 8.f ) );
		}

		// Test different constructors
		fi32 arr[ 2 ][ 3 ] = { { fi32( -1.f ), fi32( -1.f ), fi32( -1.f ) },
							     { fi32( 1.f ), fi32( 1.f ), fi32( 1.f ) } };
		f32 arr2[ 2 ][ 3 ] = { { -1.f, -1.f, -1.f }, { 1.f, 1.f, 1.f } };
		s32 arr3[ 2 ][ 3 ] = { { -1, -1, -1 }, { 1, 1, 1 } };
		
#ifdef VUL_CPLUSPLUS11
		AABB< f32, 3 > empty;
		AABB< f32, 3 > pt( Point< f32, 3 >( -1.f ), Point< f32, 3 >( 1.f ) );
		AABB< fi32, 3 > arrT( arr );
		AABB< fi32, 3 > arrF = makeAABB< fi32, 3 >( arr2 );
		AABB< f32, 3 > arrI = makeAABB< f32, 3 >( arr3 );
#else
		AABB< f32, 3 > empty = makeAABB< f32, 3 >( );
		AABB< f32, 3 > pt = makeAABB< f32, 3 >( makePoint< f32, 3 >( -1.f ), makePoint< f32, 3 >( 1.f ) );
		AABB< fi32, 3 > arrT = makeAABB< fi32, 3 >( arr );
		AABB< fi32, 3 > arrF = makeAABB< fi32, 3 >( arr2 );
		AABB< f32, 3 > arrI = makeAABB< f32, 3 >( arr3 );
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

		assert( arrT._min[ 0 ] == fi32( -1.f ) 
			 && arrT._min[ 1 ] == fi32( -1.f ) 
			 && arrT._min[ 2 ] == fi32( -1.f ) );
		assert( arrT._max[ 0 ] == fi32(  1.f )
			 && arrT._max[ 1 ] == fi32(  1.f )
			 && arrT._max[ 2 ] == fi32(  1.f ) );
		
		assert( arrF._min[ 0 ] == fi32( -1.f ) 
			 && arrF._min[ 1 ] == fi32( -1.f ) 
			 && arrF._min[ 2 ] == fi32( -1.f ) );
		assert( arrF._max[ 0 ] == fi32(  1.f )
			 && arrF._max[ 1 ] == fi32(  1.f )
			 && arrF._max[ 2 ] == fi32(  1.f ) );
		
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
		AABB< f32, 6 > a( Vector< f32, 6 >( 7.f ), Vector< f32, 6 >( 8.f ) );
		a = AABB< f32, 6 >( Vector< f32, 6 >( 5.f ), Vector< f32, 6 >( 9.f ) );
#else
		AABB< f32, 6 > a = makeAABB< f32 >( makeVector< f32, 6 >( 7.f ), makeVector< f32, 6 >( 8.f ) );
		a = makeAABB< f32 >( makeVector< f32, 6 >( 5.f ), makeVector< f32, 6 >( 9.f ) );
#endif
		for( u32 i = 0; i < 6; ++i ) {
			assert( a._min[ i ] == 5.f );
			assert( a._max[ i ] == 9.f );
		}

		return true;
	}
	bool TestAABB::translates( )
	{
		// Test 2D, 3D, 8D vs known cases.
#ifdef VUL_CPLUSPLUS11
		AABB< f32, 2 > d2( Vector< f32, 2 >{ 0.f, 1.f }, Vector< f32, 2 >{ 1.f, 2.f } );
		AABB< f32, 3 > d3( Vector< f32, 3 >( 0.f ), Vector< f32, 3 >( 1.f ) );
		AABB< f32, 8 > d8( Vector< f32, 8 >( 0.f ), Vector< f32, 8 >( 1.f ) );
		Vector< f32, 2 > v2{ 1.f, 2.f };
		Vector< f32, 3 > v3( 1.f );
		Vector< f32, 8 > v8( -1.f );
#else
		AABB< f32, 2 > d2 = makeAABB< f32 >( makeVector< f32 >( 0.f, 1.f ), makeVector< f32 >( 1.f, 2.f ) );
		AABB< f32, 3 > d3 = makeAABB< f32 >( makeVector< f32, 3 >( 0.f ), makeVector< f32, 3 >( 1.f ) );
		AABB< f32, 8 > d8 = makeAABB< f32 >( makeVector< f32, 8 >( 0.f ), makeVector< f32, 8 >( 1.f ) );
		Vector< f32, 2 > v2 = makeVector< f32 >( 1.f, 2.f );
		Vector< f32, 3 > v3 = makeVector< f32, 3 >( 1.f );
		Vector< f32, 8 > v8 = makeVector< f32, 8 >( -1.f );
#endif
		d2 = translate( d2, v2 );
		d3 = translate( d3, v3 );
		d8 = translate( d8, v8 );

		assert( d2._min[ 0 ] == 1.f );
		assert( d2._min[ 1 ] == 3.f );
		assert( d2._max[ 0 ] == 2.f );
		assert( d2._max[ 1 ] == 4.f );

		for( u32 i = 0; i < 3; ++i ) {
			assert( d3._min[ i ] == 1.f );
			assert( d3._max[ i ] == 2.f );
		}
		
		for( u32 i = 0; i < 8; ++i ) {
			assert( d8._min[ i ] == -1.f );
			assert( d8._max[ i ] == 0.f );
		}

		return true;
	}
	bool TestAABB::scales( )
	{
		// Test 2D, 3D, 8D vs known cases.
#ifdef VUL_CPLUSPLUS11
		AABB< f32, 2 > d2( Vector< f32, 2 >{ 0.f, 1.f }, Vector< f32, 2 >{ 1.f, 2.f } );
		AABB< f32, 3 > d3( Vector< f32, 3 >( 0.f ), Vector< f32, 3 >( 1.f ) );
		AABB< f32, 8 > d8( Vector< f32, 8 >( -1.f ), Vector< f32, 8 >( 1.f ) );
		Vector< f32, 2 > v2{ -1.f, 2.f };
		Vector< f32, 3 > v3( -1.f );
		Vector< f32, 8 > v8( 2.f );
#else
		AABB< f32, 2 > d2 = makeAABB< f32 >( makeVector< f32 >( 0.f, 1.f ), makeVector< f32 >( 1.f, 2.f ) );
		AABB< f32, 3 > d3 = makeAABB< f32 >( makeVector< f32, 3 >( 0.f ), makeVector< f32, 3 >( 1.f ) );
		AABB< f32, 8 > d8 = makeAABB< f32 >( makeVector< f32, 8 >( -1.f ), makeVector< f32, 8 >( 1.f ) );
		Vector< f32, 2 > v2 = makeVector< f32 >( -1.f, 2.f );
		Vector< f32, 3 > v3 = makeVector< f32, 3 >( -1.f );
		Vector< f32, 8 > v8 = makeVector< f32, 8 >( 2.f );
#endif
		d2 = scale( d2, v2 );
		d3 = scale( d3, v3 );
		d8 = scale( d8, v8 );

		assert( d2._min[ 0 ] == -1.f );
		assert( d2._min[ 1 ] ==  2.f );
		assert( d2._max[ 0 ] ==  0.f );
		assert( d2._max[ 1 ] ==  4.f );

		for( u32 i = 0; i < 3; ++i ) {
			assert( d3._min[ i ] == -1.f );
			assert( d3._max[ i ] ==  0.f );
		}
		
		for( u32 i = 0; i < 8; ++i ) {
			assert( d8._min[ i ] == -2.f );
			assert( d8._max[ i ] == 2.f );
		}

		return true;
	}
	bool TestAABB::centers( )
	{
		// Test 2D, 3D, 8D vs known cases.
#ifdef VUL_CPLUSPLUS11
		AABB< f32, 2 > d2( Vector< f32, 2 >{ 0.f, 1.f }, Vector< f32, 2 >{ 1.f, 2.f } );
		AABB< f32, 3 > d3( Vector< f32, 3 >( 0.f ), Vector< f32, 3 >( 1.f ) );
		AABB< f32, 8 > d8( Vector< f32, 8 >( -1.f ), Vector< f32, 8 >( 1.f ) );
#else
		AABB< f32, 2 > d2 = makeAABB< f32 >( makeVector< f32 >( 0.f, 1.f ), makeVector< f32 >( 1.f, 2.f ) );
		AABB< f32, 3 > d3 = makeAABB< f32 >( makeVector< f32, 3 >( 0.f ), makeVector< f32, 3 >( 1.f ) );
		AABB< f32, 8 > d8 = makeAABB< f32 >( makeVector< f32, 8 >( -1.f ), makeVector< f32, 8 >( 1.f ) );
#endif
		Point< f32, 2 > p2 = center( d2 );
		Point< f32, 3 > p3 = center( d3 );
		Point< f32, 8 > p8 = center( d8 );

		assert( p2[ 0 ] == 0.5f );
		assert( p2[ 1 ] == 1.5f );

		for( u32 i = 0; i < 3; ++i ) {
			assert( p3[ i ] == 0.5f );
		}
		
		for( u32 i = 0; i < 8; ++i ) {
			assert( p8[ i ] == 0.f );
		}

		return true;
	}
	bool TestAABB::extents( )
	{
		// Test 2D, 3D, 8D vs known cases.
#ifdef VUL_CPLUSPLUS11
		AABB< f32, 2 > d2( Vector< f32, 2 >{ 0.f, 1.f }, Vector< f32, 2 >{ 1.f, 2.f } );
		AABB< f32, 3 > d3( Vector< f32, 3 >( 0.f ), Vector< f32, 3 >( 1.f ) );
		AABB< f32, 8 > d8( Vector< f32, 8 >( -1.f ), Vector< f32, 8 >( 1.f ) );
#else
		AABB< f32, 2 > d2 = makeAABB< f32 >( makeVector< f32 >( 0.f, 1.f ), makeVector< f32 >( 1.f, 2.f ) );
		AABB< f32, 3 > d3 = makeAABB< f32 >( makeVector< f32, 3 >( 0.f ), makeVector< f32, 3 >( 1.f ) );
		AABB< f32, 8 > d8 = makeAABB< f32 >( makeVector< f32, 8 >( -1.f ), makeVector< f32, 8 >( 1.f ) );
#endif
		Point< f32, 2 > p2 = center( d2 );
		Point< f32, 3 > p3 = center( d3 );
		Point< f32, 8 > p8 = center( d8 );

		assert( p2[ 0 ] == 0.5f );
		assert( p2[ 1 ] == 1.5f );

		for( u32 i = 0; i < 3; ++i ) {
			assert( p3[ i ] == 0.5f );
		}
		
		for( u32 i = 0; i < 8; ++i ) {
			assert( p8[ i ] == 0.f );
		}

		return true;
	}
	bool TestAABB::insides( )
	{
		// Test 2D, 3D, 8D vs known cases.
#ifdef VUL_CPLUSPLUS11
		AABB< f32, 2 > d2( Vector< f32, 2 >{ 0.f, 1.f }, Vector< f32, 2 >{ 1.f, 2.f } );
		AABB< f32, 3 > d3( Vector< f32, 3 >( 0.f ), Vector< f32, 3 >( 1.f ) );
		AABB< f32, 8 > d8( Vector< f32, 8 >( -1.f ), Vector< f32, 8 >( 0.f ) );
#else
		AABB< f32, 2 > d2 = makeAABB< f32 >( makeVector< f32 >( 0.f, 1.f ), makeVector< f32 >( 1.f, 2.f ) );
		AABB< f32, 3 > d3 = makeAABB< f32 >( makeVector< f32, 3 >( 0.f ), makeVector< f32, 3 >( 1.f ) );
		AABB< f32, 8 > d8 = makeAABB< f32 >( makeVector< f32, 8 >( -1.f ), makeVector< f32, 8 >( 0.f ) );
#endif
		Point< f32, 2 > p2 = center( d2 );
		Point< f32, 3 > p3 = center( d3 );
		Point< f32, 8 > p8 = center( d8 );

		// Check that the centers are inside
		assert( inside( d2, p2 ) );
		assert( inside( d3, p3, ( f32 )1e-5 ) );
		assert( inside( d8, p8, ( f32 )1e-5 ) );

		// For each component, invert it, which puts the point outside our box, and test it is not inside.
		for( u32 i = 0; i < 2; ++i ) {
			p2[ i ] = -p2[ i ];
			assert( !inside( d2, p2 ) );
			p2[ i ] = -p2[ i ];
		}

		for( u32 i = 0; i < 3; ++i ) {
			p3[ i ] = -p3[ i ];
			assert( !inside( d3, p3 ) );
			p3[ i ] = -p3[ i ];
		}

		for( u32 i = 0; i < 8; ++i ) {
			p8[ i ] = -p8[ i ];
			assert( !inside( d8, p8 ) );
			p8[ i ] = -p8[ i ];
		}

		// Finally, test that edges are inside. One case is enough here
#ifdef VUL_CPLUSPLUS11
		p3 = Point< f32, 3 >( 0.f );
#else
		p3 = makePoint< f32, 3 >( 0.f );
#endif
		assert( inside( d3, p3 ) );

		return true;
	}
	bool TestAABB::containment( )
	{
		// Test 2D, 8D vs known cases.
#ifdef VUL_CPLUSPLUS11
		AABB< f32, 2 > a2( Vector< f32, 2 >{ 0.f, 1.f }, Vector< f32, 2 >{ 1.f, 2.f } );
		AABB< f32, 2 > b2( Vector< f32, 2 >{ 0.5f, 1.5f }, Vector< f32, 2 >{ 1.f, 2.f } ); // This tests inside but on boundary
		AABB< f32, 2 > c2( Vector< f32, 2 >{ -0.5f, 1.f }, Vector< f32, 2 >{ 1.f, 2.f } ); // This tests outside on one side
		AABB< f32, 8 > a8( Vector< f32, 8 >( -1.f ), Vector< f32, 8 >( 0.f ) );
		AABB< f32, 8 > b8( Vector< f32, 8 >( -0.75f ), Vector< f32, 8 >( -0.25f ) ); // This tests entirely inside
		AABB< f32, 8 > c8( Vector< f32, 8 >( -8.f ), Vector< f32, 8 >( -6.f ) ); // This tests entirely disparate
#else
		AABB< f32, 2 > a2 = makeAABB< f32 >( makeVector< f32 >( 0.f, 1.f ), makeVector< f32 >( 1.f, 2.f ) );
		AABB< f32, 2 > b2 = makeAABB< f32 >( makeVector< f32 >( 0.5f, 1.5f ), makeVector< f32 >( 1.f, 2.f ) ); // This tests inside but on boundary
		AABB< f32, 2 > c2 = makeAABB< f32 >( makeVector< f32 >( -0.5f, 1.f ), makeVector< f32 >( 1.f, 2.f ) ); // This tests outside on one side
		AABB< f32, 8 > a8 = makeAABB< f32 >( makeVector< f32, 8 >( -1.f ), makeVector< f32, 8 >( 0.f ) );
		AABB< f32, 8 > b8 = makeAABB< f32 >( makeVector< f32, 8 >( -0.75f ), makeVector< f32, 8 >( -0.25f ) ); // This tests entirely inside
		AABB< f32, 8 > c8 = makeAABB< f32 >( makeVector< f32, 8 >( -8.f ), makeVector< f32, 8 >( -6.f ) ); // This tests entirely disparate
#endif
		assert( contains( a2, a2 ) ); // Tests selfcontainment
		assert( contains( a2, b2 ) );
		assert( !contains( b2, a2 ) );
		assert( !contains( a2, c2 ) );

		assert( contains( a8, a8 ) );
		assert( contains( a8, b8 ) );
		assert( !contains( a8, c8 ) );

		return true;
	}

	bool TestAABB::frustum_tests( )
	{
		// @TODO(thynn): This needs to be tested!
		// Fully inside, partially inside, fully outside, identical
		// For 2D, 3D, 4D, 8D
		return true;
	}

	bool TestAABB::unions( )
	{
		// Test 2D, 8D vs known cases.
		AABB< f32, 2 > r2;
		AABB< f32, 8 > r8;
#ifdef VUL_CPLUSPLUS11
		AABB< f32, 2 > a2( Vector< f32, 2 >{ 0.f, 1.f }, Vector< f32, 2 >{ 1.f, 2.f } );
		AABB< f32, 2 > b2( Vector< f32, 2 >{ -1.f, 0.f }, Vector< f32, 2 >{ 2.f, 3.f } ); // This tests complete engulfment
		AABB< f32, 2 > c2( Vector< f32, 2 >{ 0.5f, 1.5f }, Vector< f32, 2 >{ 1.5f, 2.5f } ); // This tests partial overlap
		AABB< f32, 2 > d2( Vector< f32, 2 >{ 2.f, 1.5f }, Vector< f32, 2 >{ 3.f, 4.f } ); // This tests disparates
		AABB< f32, 8 > a8( Vector< f32, 8 >( 0.f ), Vector< f32, 8 >( 1.f ) );
		AABB< f32, 8 > b8( Vector< f32, 8 >( -0.5f ), Vector< f32, 8 >( 2.f ) ); // This tests complete engulfment
		AABB< f32, 8 > c8( Vector< f32, 8 >( 0.5f ), Vector< f32, 8 >( 2.f ) ); // This tests partial overlap
		AABB< f32, 8 > d8( Vector< f32, 8 >( 2.f ), Vector< f32, 8 >( 3.f ) ); // This tests disparates
#else
		AABB< f32, 2 > a2 = makeAABB< f32 >( makeVector< f32 >( 0.f, 1.f ), makeVector< f32 >( 1.f, 2.f ) );
		AABB< f32, 2 > b2 = makeAABB< f32 >( makeVector< f32 >( -1.f, 0.f ), makeVector< f32 >( 2.f, 3.f ) ); // This tests complete engulfment
		AABB< f32, 2 > c2 = makeAABB< f32 >( makeVector< f32 >( 0.5f, 1.5f ), makeVector< f32 >( 1.5f, 2.5f ) ); // This tests partial overlap
		AABB< f32, 2 > d2 = makeAABB< f32 >( makeVector< f32 >( 2.f, 1.5f ), makeVector< f32 >( 3.f, 4.f ) ); // This tests disparates
		AABB< f32, 8 > a8 = makeAABB< f32 >( makeVector< f32, 8 >( 0.f ), makeVector< f32, 8 >( 1.f ) );
		AABB< f32, 8 > b8 = makeAABB< f32 >( makeVector< f32, 8 >( -0.5f ), makeVector< f32, 8 >( 2.f ) ); // This tests complete engulfment
		AABB< f32, 8 > c8 = makeAABB< f32 >( makeVector< f32, 8 >( 0.5f ), makeVector< f32, 8 >( 2.f ) ); // This tests partial overlap
		AABB< f32, 8 > d8 = makeAABB< f32 >( makeVector< f32, 8 >( 2.f ), makeVector< f32, 8 >( 3.f ) ); // This tests disparates
#endif
		r2 = unionize( a2, a2 ); // self union is self
		assert( r2._min[ 0 ] == a2._min[ 0 ] ); assert( r2._min[ 1 ] == a2._min[ 1 ] );
		assert( r2._max[ 0 ] == a2._max[ 0 ] ); assert( r2._max[ 1 ] == a2._max[ 1 ] );
		r2 = unionize( a2, b2 ); // b contains a, so union is b
		assert( r2._min[ 0 ] == b2._min[ 0 ] ); assert( r2._min[ 1 ] == b2._min[ 1 ] );
		assert( r2._max[ 0 ] == b2._max[ 0 ] ); assert( r2._max[ 1 ] == b2._max[ 1 ] );
		r2 = unionize( a2, c2 ); // partial overlap
		assert( r2._min[ 0 ] == 0.f ); assert( r2._min[ 1 ] == 1.f );
		assert( r2._max[ 0 ] == 1.5f ); assert( r2._max[ 1 ] == 2.5f );
		r2 = unionize( a2, d2 ); // no overlap
		assert( r2._min[ 0 ] == 0.f ); assert( r2._min[ 1 ] == 1.f );
		assert( r2._max[ 0 ] == 3.f ); assert( r2._max[ 1 ] == 4.f );

		r8 = unionize( a8, a8 );
		assert( all( r8._min.as_vec( ) == a8._min.as_vec( ) ) ); assert( all( r8._max.as_vec( ) == a8._max.as_vec( ) ) );
		r8 = unionize( a8, b8 );
		assert( all( r8._min.as_vec( ) == b8._min.as_vec( ) ) ); assert( all( r8._max.as_vec( ) == b8._max.as_vec( ) ) );
		r8 = unionize( a8, c8 );
		assert( all( r8._min.as_vec( ) == a8._min.as_vec( ) ) ); assert( all( r8._max.as_vec( ) == c8._max.as_vec( ) ) );
		r8 = unionize( a8, d8 );
		assert( all( r8._min.as_vec( ) == a8._min.as_vec( ) ) ); assert( all( r8._max.as_vec( ) == d8._max.as_vec( ) ) );
		
		return true;
	}

	bool TestAABB::instersects( )
	{		
#ifdef VUL_CPLUSPLUS11
		AABB< f32, 2 > a2( Vector< f32, 2 >{ 0.f, 1.f }, Vector< f32, 2 >{ 1.f, 2.f } );
		AABB< f32, 2 > b2( Vector< f32, 2 >{ -1.f, 0.f }, Vector< f32, 2 >{ 2.f, 3.f } ); // This tests complete engulfment
		AABB< f32, 2 > c2( Vector< f32, 2 >{ 0.5f, 1.5f }, Vector< f32, 2 >{ 1.5f, 2.5f } ); // This tests partial overlap
		AABB< f32, 2 > d2( Vector< f32, 2 >{ 2.f, 1.5f }, Vector< f32, 2 >{ 3.f, 4.f } ); // This tests disparates
		AABB< f32, 8 > a8( Vector< f32, 8 >( 0.f ), Vector< f32, 8 >( 1.f ) );
		AABB< f32, 8 > b8( Vector< f32, 8 >( -0.5f ), Vector< f32, 8 >( 2.f ) ); // This tests complete engulfment
		AABB< f32, 8 > c8( Vector< f32, 8 >( 0.5f ), Vector< f32, 8 >( 2.f ) ); // This tests partial overlap
		AABB< f32, 8 > d8( Vector< f32, 8 >( 2.f ), Vector< f32, 8 >( 3.f ) ); // This tests disparates
#else
		AABB< f32, 2 > a2 = makeAABB< f32 >( makeVector< f32 >( 0.f, 1.f ), makeVector< f32 >( 1.f, 2.f ) );
		AABB< f32, 2 > b2 = makeAABB< f32 >( makeVector< f32 >( -1.f, 0.f ), makeVector< f32 >( 2.f, 3.f ) ); // This tests complete engulfment
		AABB< f32, 2 > c2 = makeAABB< f32 >( makeVector< f32 >( 0.5f, 1.5f ), makeVector< f32 >( 1.5f, 2.5f ) ); // This tests partial overlap
		AABB< f32, 2 > d2 = makeAABB< f32 >( makeVector< f32 >( 2.f, 1.5f ), makeVector< f32 >( 3.f, 4.f ) ); // This tests disparates
		AABB< f32, 8 > a8 = makeAABB< f32 >( makeVector< f32, 8 >( 0.f ), makeVector< f32, 8 >( 1.f ) );
		AABB< f32, 8 > b8 = makeAABB< f32 >( makeVector< f32, 8 >( -0.5f ), makeVector< f32, 8 >( 2.f ) ); // This tests complete engulfment
		AABB< f32, 8 > c8 = makeAABB< f32 >( makeVector< f32, 8 >( 0.5f ), makeVector< f32, 8 >( 2.f ) ); // This tests partial overlap
		AABB< f32, 8 > d8 = makeAABB< f32 >( makeVector< f32, 8 >( 2.f ), makeVector< f32, 8 >( 3.f ) ); // This tests disparates
#endif
		assert(  intersect( a2, a2 ) ); // Self-intersection
		assert(  intersect( a2, b2 ) );
		assert(  intersect( a2, c2 ) );
		assert( !intersect( a2, d2 ) );

		assert(  intersect( a8, a8 ) ); // Self-intersection
		assert(  intersect( a8, b8 ) );
		assert(  intersect( a8, c8 ) );
		assert( !intersect( a8, d8 ) );

		return true;
	}

	bool TestAABB::transforms( )
	{
		// We test 2D and 3D case
#ifdef VUL_CPLUSPLUS11
		
		AABB< f32, 2 > d2( Vector< f32, 2 >( -1.f ), Vector< f32, 2 >( 1.f ) );
		AABB< f32, 3 > d3( Vector< f32, 3 >( -1.f ), Vector< f32, 3 >( 1.f ) );
		Affine< f32, 2 > a2( Matrix< f32, 2, 2 >{ cosf( ( f32 )VUL_PI / 6 ), -sinf( ( f32 )VUL_PI / 6 ),
							      sinf( ( f32 )VUL_PI / 6 ),  cosf( ( f32 )VUL_PI / 6 ) },
							   Vector< f32, 2 >{ -1.f, 1.f } );
		Affine< f32, 3 > a3 = makeAffine3D( Vector< f32, 3 >( 1.f ),
											  Vector< f32, 3 >{ 2.f, -2.f, 1.f },
											  makeQuatFromAxisAngle< f32 >( Vector< f32, 3 >{ 0.f, 0.f, 1.f }, ( f32 )-VUL_PI / 4.f ) );
#else
		AABB< f32, 2 > d2 = makeAABB< f32 >( makeVector< f32, 2 >( -1.f ), makeVector< f32, 2 >( 1.f ) );
		AABB< f32, 3 > d3 = makeAABB< f32 >( makeVector< f32, 3 >( -1.f ), makeVector< f32, 3 >( 1.f ) );
		// Rotate CW 45 degrees and move 1 unit towards negative x and positive y..
		Affine< f32, 2 > a2 = makeAffine< f32, 2 >( makeMatrix22< f32 >( cos( ( f32 )VUL_PI / 6 ), -sin( ( f32 )VUL_PI / 6 ),
																			   sin( ( f32 )VUL_PI / 6 ),  cos( ( f32 )VUL_PI / 6 ) ), 
														makeVector< f32 >( -1.f, 1.f ) );
		// Rotate CCW 45 degrees around Z, scale (2,-2,1), translate +1 in all directions.
		Affine< f32, 3 > a3 = makeAffine3D< f32 >( makeVector< f32, 3 >( 1.f ), 
													   makeVector< f32 >( 2.f, -2.f, 1.f ), 
													   makeQuatFromAxisAngle< f32 >( makeVector< f32 >( 0.f, 0.f, 1.f ), ( f32 )-VUL_PI / 4.f ) );
#endif
		AABB< f32, 2 > r2;
		AABB< f32, 3 > r3;
		r2 = transform< f32, 2 >( d2, a2 );
		r3 = transform< f32, 3 >( d3, a3 );
		
		// The rotate means we now have corners at sqrt(2) distance.

		f32 f32eps = 1e-5f;
		assert( r2._min[ 0 ] + 2.3660254f < f32eps );
		assert( r2._min[ 1 ] + 0.36602545f < f32eps );
		assert( r2._max[ 0 ] - 0.36602545f < f32eps );
		assert( r2._max[ 1 ] - 2.3660254f < f32eps );
				
		assert( r3._min[ 0 ] + 1.8284273f < f32eps );
		assert( r3._min[ 1 ] + 1.8284273f < f32eps );
		assert( r3._min[ 2 ] - 0.f < f32eps );
		assert( r3._max[ 0 ] - 3.8284273f < f32eps );
		assert( r3._max[ 1 ] - 3.8284273f < f32eps );
		assert( r3._max[ 2 ] - 2.f < f32eps );

		return true;
	}

	bool TestAABB::transforms3D( )
	{
#ifdef VUL_AOSOA_SSE
		AABB< __m128, 3 > m128[ 4 ], r128[ 4 ];
		AABB< __m128d, 3 > m128d[ 2 ], r128d[ 2 ];
#endif
#ifdef VUL_AOSOA_AVX
		AABB< __m256, 3 > m256[ 8 ], r256[ 8 ];
		AABB< __m256d, 3 > m256d[ 4 ], r256d[ 4 ];
#endif
#ifdef VUL_AOSOA_NEON
		AABB< float32x4_t, 3 > m32x4[ 4 ], r32x4[ 4 ];
#endif
		AABB< f32, 3 > in32[ 8 ];
		AABB< f64, 3 > in64[ 4 ];
		f32 mat[ 9 ], vec[ 3 ], aabb_min[ 3 ], aabb_max[ 3 ];
		
		// Rotate 45 deg around y
		mat[ 0 ] = cos( ( f32 )VUL_PI / 6 );
		mat[ 1 ] = 0;
		mat[ 2 ] = sin( ( f32 )VUL_PI / 6 );
		mat[ 3 ] = 0;
		mat[ 4 ] = 1;
		mat[ 5 ] = 0;
		mat[ 6 ] = -sin( ( f32 )VUL_PI / 6 );
		mat[ 7 ] = 0;
		mat[ 8 ] = cos( ( f32 )VUL_PI / 6 );
		// Translate (1, 2, -4)
		vec[ 0 ] = 1.f;
		vec[ 1 ] = 2.f;
		vec[ 2 ] = -4.f;
		// -1, 1 AABB
		aabb_min[ 0 ] = aabb_min[ 1 ] = aabb_min[ 2 ] = -1.f;
		aabb_max[ 0 ] = aabb_max[ 1 ] = aabb_max[ 2 ] = 1.f;
		// At this point, we have asserted that the normal transform works, so use that as a reference
		AABB< f32, 3 > ref32_in, ref32_out;
		AABB< f64, 3 > ref64_in, ref64_out;
		
#ifdef VUL_CPLUSPLUS11
		ref32_in._min = Point< f32, 3 >( aabb_min );
		ref32_in._max = Point< f32, 3 >( aabb_max );
		ref64_in._min = makePoint< f64, 3 >( aabb_min );
		ref64_in._max = makePoint< f64, 3 >( aabb_max );
		
		Affine< f64, 3 > a64 = Affine< f64, 3 >( makeMatrix< f64, 3, 3 >( mat ), makeVector< f64, 3 >( vec ) );
		Affine< f32, 3 > a32 = Affine< f32, 3 >( makeMatrix< f32, 3, 3 >( mat ), makeVector< f32, 3 >( vec ) );

		in32[ 0 ] = AABB< f32, 3 >( Point< f32, 3 >( aabb_min ), Point< f32, 3 >( aabb_max ) );
		in64[ 0 ] = AABB< f64, 3 >( makePoint< f64, 3 >( aabb_min ), makePoint< f64, 3 >( aabb_max ) );
#else
		ref32_in._min = makePoint< f32, 3 >( aabb_min );
		ref32_in._max = makePoint< f32, 3 >( aabb_max );
		ref64_in._min = makePoint< f64, 3 >( aabb_min );
		ref64_in._max = makePoint< f64, 3 >( aabb_max );
		
		Affine< f64, 3 > a64 = makeAffine< f64, 3 >( makeMatrix< f64, 3, 3 >( mat ), makeVector< f64, 3 >( vec ) );
		Affine< f32, 3 > a32 = makeAffine< f32, 3 >( makeMatrix< f32, 3, 3 >( mat ), makeVector< f32, 3 >( vec ) );

		in32[ 0 ] = makeAABB< f32, 3 >( makePoint< f32, 3 >( aabb_min ), makePoint< f32, 3 >( aabb_max ) );
		in64[ 0 ] = makeAABB< f64, 3 >( makePoint< f64, 3 >( aabb_min ), makePoint< f64, 3 >( aabb_max ) );
#endif
		// Transform the reference
		ref32_out = transform( ref32_in, a32 );
		ref64_out = transform( ref64_in, a64 );
		
		// Pack, transform.
#ifdef VUL_AOSOA_SSE
		pack< 3 >( &m128[ 0 ], &in32[ 0 ], 1u );
		pack< 3 >( &m128d[ 0 ], &in64[ 0 ], 1u );
#endif
#ifdef VUL_AOSOA_NEON
		pack< 3 >( &m32x4[ 0 ], &in32[ 0 ], 1u );
#endif
#ifdef VUL_AOSOA_AVX
		pack< 3 >( &m256[ 0 ], &in32[ 0 ], 1u );
		pack< 3 >( &m256d[ 0 ], &in64[ 0 ], 1u );
#endif

#ifdef VUL_AOSOA_SSE
		transform3D( &r128[ 0 ], &m128[ 0 ], a32, 1u );
		transform3D( &r128d[ 0 ], &m128d[ 0 ], a64, 1u );
#endif
#ifdef VUL_AOSOA_NEON
		transform3D( &r32x4[ 0 ], &m32x4[ 0 ], a32, 1u );
#endif
#ifdef VUL_AOSOA_AVX
		transform3D( &r256[ 0 ], &m256[ 0 ], a32, 1u );
		transform3D( &r256d[ 0 ], &m256d[ 0 ], a64, 1u );
#endif
		
		//unpack the vectorized ones and compare to reference
#if defined( VUL_AOSOA_SSE ) || defined( VUL_AOSOA_AVX ) || defined( VUL_AOSOA_NEON )
		f32 f32eps = 1e-5f;
#endif
#if defined( VUL_AOSOA_SSE ) || defined( VUL_AOSOA_AVX )
		f64 f64eps = 1e-8;
#endif
#ifdef VUL_AOSOA_SSE
		unpack< 3 >( &in32[ 0 ], &r128[ 0 ], 1u );
		unpack< 3 >( &in64[ 0 ], &r128d[ 0 ], 1u );
#endif

#ifdef VUL_AOSOA_SSE
#ifdef VUL_CPLUSPLUS11
		assert( all( in32[ 0 ]._min - ref32_out._min < Vector< f32, 3 >( f32eps ) ) );
		assert( all( in32[ 0 ]._max - ref32_out._max < Vector< f32, 3 >( f32eps ) ) );
		assert( all( in64[ 0 ]._min - ref64_out._min < Vector< f64, 3 >( f64eps ) ) );
		assert( all( in64[ 0 ]._max - ref64_out._max < Vector< f64, 3 >( f64eps ) ) );
#else
		assert( all( in32[ 0 ]._min - ref32_out._min < makeVector< f32, 3 >( f32eps ) ) );
		assert( all( in32[ 0 ]._max - ref32_out._max < makeVector< f32, 3 >( f32eps ) ) );
		assert( all( in64[ 0 ]._min - ref64_out._min < makeVector< f64, 3 >( f64eps ) ) );
		assert( all( in64[ 0 ]._max - ref64_out._max < makeVector< f64, 3 >( f64eps ) ) );
#endif
#endif // VUL_AOSOA_SSE

#ifdef VUL_AOSOA_NEON
		unpack< 3 >( &in32[ 0 ], &r32x4[ 0 ], 1u );
#endif

#ifdef VUL_AOSOA_NEON
#ifdef VUL_CPLUSPLUS11
		assert( all( in32[ 0 ]._min - ref32_out._min < Vector< f32, 3 >( f32eps ) ) );
		assert( all( in32[ 0 ]._max - ref32_out._max < Vector< f32, 3 >( f32eps ) ) );
#else
		assert( all( in32[ 0 ]._min - ref32_out._min < makeVector< f32, 3 >( f32eps ) ) );
		assert( all( in32[ 0 ]._max - ref32_out._max < makeVector< f32, 3 >( f32eps ) ) );
#endif
#endif // VUL_AOSOA_NEON
		
#ifdef VUL_AOSOA_AVX
		unpack< 3 >( &in32[ 0 ], &r256[ 0 ], 1u );
		unpack< 3 >( &in64[ 0 ], &r256d[ 0 ], 1u );
		
#ifdef VUL_CPLUSPLUS11
		assert( all( in32[ 0 ]._min - ref32_out._min < Vector< f32, 3 >( f32eps ) ) );
		assert( all( in32[ 0 ]._max - ref32_out._max < Vector< f32, 3 >( f32eps ) ) );
		assert( all( in64[ 0 ]._min - ref64_out._min < Vector< f64, 3 >( f64eps ) ) );
		assert( all( in64[ 0 ]._max - ref64_out._max < Vector< f64, 3 >( f64eps ) ) );
#else
		assert( all( in32[ 0 ]._min - ref32_out._min < makeVector< f32, 3 >( f32eps ) ) );
		assert( all( in32[ 0 ]._max - ref32_out._max < makeVector< f32, 3 >( f32eps ) ) );
		assert( all( in64[ 0 ]._min - ref64_out._min < makeVector< f64, 3 >( f64eps ) ) );
		assert( all( in64[ 0 ]._max - ref64_out._max < makeVector< f64, 3 >( f64eps ) ) );
#endif
#endif // VUL_AOSOA_AVX

		return true;
	}


	bool TestAABB::inside_tests( )
	{
#ifdef VUL_AOSOA_SSE
		//AABB< __m128, 3 > m128[ 4 ], r128[ 4 ];
		//AABB< __m128d, 3 > m128d[ 2 ], r128d[ 2 ];
#endif
#ifdef VUL_AOSOA_AVX
		//AABB< __m256, 3 > m256[ 8 ], r256[ 8 ];
		//AABB< __m256d, 3 > m256d[ 4 ], r256d[ 4 ];
#endif
#ifdef VUL_AOSOA_NEON
		//AABB< float32x4_t, 3 > m32x4[ 4 ], r32x4[ 4 ];
#endif
		//AABB< f32, 3 > in32[ 8 ];
		//AABB< f64, 3 > in64[ 4 ];
		//Vector< f32, 4 > planes32[ 6 ];
		//Vector< f64, 4 > planes64[ 6 ];

		// @TODO(thynn): These tests
		// All the inside_tests(  )

		return true;
	}
};
#pragma warning(default: 6001)
#endif
