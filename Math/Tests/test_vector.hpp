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
#ifndef VUL_TEST_VECTOR_HPP
#define VUL_TEST_VECTOR_HPP

#include <cassert>

#include "../vul_math.hpp"
#define VUL_TEST_RNG ( float )( ( float )rand( ) / ( float )RAND_MAX )

using namespace vul;

namespace vul_test {
	class TestVector {

	public:
		static bool test( );

	private:
		static bool make( );
		static bool member_ops( );
		static bool comparisons( );
		static bool ops( );
		static bool functions( );
	};

	bool TestVector::test( )
	{
		assert( make( ) );
		assert( comparisons( ) );
		assert( member_ops( ) );
		assert( ops( ) );
		assert( functions( ) );

		return true;
	}

	bool TestVector::make( )
	{
		Vector< fi32, 2 > v2;
		Vector< f32, 3 > v3;
		Vector< s64, 9 > v9;

#ifdef VUL_CPLUSPLUS11
		v2 = Vector< fi32, 2 >( );
		v3 = Vector< f32, 3 >( );
		v9 = Vector< s64, 9 >( );
#else
		v2 = makeVector< fi32, 2 >( );
		v3 = makeVector< f32, 3 >( );
		v9 = makeVector< s64, 9 >( );
#endif
		assert( v2[ 0 ] == fi32( 0.f ) ); assert( v2[ 1 ] == fi32( 0.f ) );
		assert( v3[ 0 ] == 0.f ); assert( v3[ 1 ] == 0.f ); assert( v3[ 2 ] == 0.f );
		for( u32 i = 0; i < 9; ++i ) {
			assert( v9[ i ] == 0L );
		}

#ifdef VUL_CPLUSPLUS11
		v2 = Vector< fi32, 2 >( fi32( 7.f ) );
		v3 = Vector< f32, 3 >( -8.f );
		v9 = Vector< s64, 9 >( 32L );
#else
		v2 = makeVector< fi32, 2 >( fi32( 7.f ) );
		v3 = makeVector< f32, 3 >( -8.f );
		v9 = makeVector< s64, 9 >( 32L );
#endif
		assert( v2[ 0 ] == fi32( 7.f ) ); assert( v2[ 1 ] == fi32( 7.f ) );
		assert( v3[ 0 ] == -8.f ); assert( v3[ 1 ] == -8.f ); assert( v3[ 2 ] == -8.f );
		for( u32 i = 0; i < 9; ++i ) {
			assert( v9[ i ] == 32L );
		}

#ifdef VUL_CPLUSPLUS11
		Vector< fi32, 2 > v2c = Vector< fi32, 2 >( v2 );
		Vector< f32, 3 > v3c = Vector< f32, 3 >( v3 );
		Vector< s64, 9 > v9c = Vector< s64, 9 >( v9 );
#else
		Vector< fi32, 2 > v2c = makeVector< fi32, 2 >( v2 );
		Vector< f32, 3 > v3c = makeVector< f32, 3 >( v3 );
		Vector< s64, 9 > v9c = makeVector< s64, 9 >( v9 );
#endif
		assert( v2c[ 0 ] == fi32( 7.f ) ); assert( v2c[ 1 ] == fi32( 7.f ) );
		assert( v3c[ 0 ] == -8.f ); assert( v3c[ 1 ] == -8.f ); assert( v3c[ 2 ] == -8.f );
		for( u32 i = 0; i < 9; ++i ) {
			assert( v9c[ i ] == 32L );
		}

		fi32 a2[ 2 ] = { ( fi32 )VUL_TEST_RNG, ( fi32 )VUL_TEST_RNG };
		f32 a3[ 3 ] = { VUL_TEST_RNG, VUL_TEST_RNG, VUL_TEST_RNG };
		s64 a9[ 9 ] = { -5L, -4L, -3L, -2L, -1L, 0L, 1L, 2L, 3L };
#ifdef VUL_CPLUSPLUS11
		v2 = Vector< fi32, 2 >( a2 );
		v3 = Vector< f32, 3 >( a3 );
		v9 = Vector< s64, 9 >( a9 );
#else
		v2 = makeVector< fi32, 2 >( a2 );
		v3 = makeVector< f32, 3 >( a3 );
		v9 = makeVector< s64, 9 >( a9 );
#endif
		assert( v2[ 0 ] == a2[ 0 ] ); assert( v2[ 1 ] == a2[ 1 ] );
		assert( v3[ 0 ] == a3[ 0 ] ); assert( v3[ 1 ] == a3[ 1 ] ); assert( v3[ 2 ] == a3[ 2 ] );
		for( u32 i = 0; i < 9; ++i ) {
			assert( v9[ i ] == a9[ i ] );
		}

		s32 ai[ 3 ] = { -9, 5, 17 };
		f32 af[ 9 ];
		for( u32 i = 0; i < 9; ++i ) {
			af[ i ] = VUL_TEST_RNG;
		}

		v3 = makeVector< f32, 3 >( ai );
		v9 = makeVector< s64, 9 >( af );

		assert( v3[ 0 ] == -9.f ); assert( v3[ 1 ] == 5.f ); assert( v3[ 2 ] == 17.f );
		for( u32 i = 0; i < 9; ++i ) {
			assert( v9[ i ] == ( s64 )af[ i ] );
		}

#ifdef VUL_CPLUSPLUS11
		Point< fi32, 2 > p2{ fi32( VUL_TEST_RNG ), fi32( VUL_TEST_RNG ) };
		p2 = Point< fi32, 2 >( v2 );
#else
		fi32 afi2[ 2 ] = { fi32( VUL_TEST_RNG ), fi32( VUL_TEST_RNG ) };
		Point< fi32, 2 > p2 = makePoint< fi32 >( afi2 );
		p2 = makePoint< fi32, 2 >( v2 );
#endif
		assert( v2[ 0 ] == p2[ 0 ] ); assert( v2[ 1 ] == p2[ 1 ] );
		
#ifdef VUL_CPLUSPLUS11
		v2 = Vector< fi32, 2 >( fi32( 7.f ) );
		v9 = Vector< s64, 9 >( -3 );
#else
		v2 = makeVector< fi32, 2 >( fi32( 7.f ) );
		v9 = makeVector< s64, 9 >( -3.f );
#endif
		assert( v2[ 0 ] == fi32( 7.f ) ); assert( v2[ 1 ] == fi32( 7.f ) );
		for( u32 i = 0; i < 9; ++i ) {
			assert( v9[ i ] == -3L );
		}

		Vector< f32, 2 > v2f = makeVector< f32 >( -1.f, 2.f );
		assert( v2f[ 0 ] == -1.f ); assert( v2f[ 1 ] == 2.f );

		Vector< f32, 3 > v3f = makeVector< f32 >( -1.f, 2.f, 0.5f );
		assert( v3f[ 0 ] == -1.f ); assert( v3f[ 1 ] == 2.f ); assert( v3f[ 2 ] == 0.5f );

		Vector< f32, 4 > v4f = makeVector< f32 >( 0.02f, 0.14f, -0.17f, 0.8f );
		assert( v4f[ 0 ] == 0.02f ); assert( v4f[ 1 ] == 0.14f ); assert( v4f[ 2 ] == -0.17f ); assert( v4f[ 3 ] == 0.8f );
		
		v4f = makeVector< f32 >( v2f, v2f );
		assert( v4f[ 0 ] == -1.f ); assert( v4f[ 1 ] == 2.f ); assert( v4f[ 2 ] == -1.f ); assert( v4f[ 3 ] == 2.f );

		v4f = makeVector< f32 >( v3f, 0.17f );
		assert( v4f[ 0 ] == -1.f ); assert( v4f[ 1 ] == 2.f ); assert( v4f[ 2 ] == 0.5f );  assert( v4f[ 3 ] == 0.17f );

		v3f = makeVector< f32 >( v2f, 0.5f );
		assert( v3f[ 0 ] == -1.f ); assert( v3f[ 1 ] == 2.f ); assert( v3f[ 2 ] == 0.5f );

		Vector< s32, 2 > v2i = makeVector< s32 >( -1.f, 2.f );
		assert( v2i[ 0 ] == -1 ); assert( v2i[ 1 ] == 2 );

		Vector< s32, 3 > v3i = makeVector< s32 >( -1.f, 2.f, 0.5f );
		assert( v3i[ 0 ] == -1 ); assert( v3i[ 1 ] == 2 ); assert( v3i[ 2 ] == 0 );

		Vector< s32, 4 > v4i = makeVector< s32 >( -4.f, 1.f, -1.4f, 8.f );
		assert( v4i[ 0 ] == -4 ); assert( v4i[ 1 ] == 1 ); assert( v4i[ 2 ] == -1 ); assert( v4i[ 3 ] == 8 );

		return true;
	}

	bool TestVector::comparisons( )
	{
		Vector< bool, 2 > b2;
		Vector< bool, 4 > b4;

#ifdef VUL_CPLUSPLUS11
		assert( all( Vector< s32, 2 >{ 1, 2 } ) );
		assert( !all( Vector< s32, 2 >{ 1, 0 } ) );
		assert( any( Vector< s32, 2 >{ 1, 2 } ) );
		assert( any( Vector< s32, 2 >{ 1, 0 } ) );
		assert( !any( Vector< s32, 2 >{ 0, 0 } ) );

		assert( all( Vector< bool, 2 >{ true, true } ) );
		assert( !all( Vector< bool, 2 >{ true, false } ) );
		assert( any( Vector< bool, 2 >{ true, true } ) );
		assert( any( Vector< bool, 2 >{ true, false } ) );
		assert( !any( Vector< bool, 2 >{ false, false } ) );
#else
		assert( all( makeVector< s32 >( 1, 2 ) ) );
		assert( !all( makeVector< s32 >( 1, 0 ) ) );
		assert( any( makeVector< s32 >( 1, 2 ) ) );
		assert( any( makeVector< s32 >( 1, 0 ) ) );
		assert( !any( makeVector< s32 >( 0, 0 ) ) );

		assert( all( makeVector< bool >( true, true ) ) );
		assert( !all( makeVector< bool >( true, false ) ) );
		assert( any( makeVector< bool >( true, true ) ) );
		assert( any( makeVector< bool >( true, false ) ) );
		assert( !any( makeVector< bool >( false, false ) ) );
#endif
		
#ifdef VUL_CPLUSPLUS11
		Vector< fi32, 2 > v2a{ ( fi32 )1.f,  ( fi32 )-1.f },
						    v2b{ ( fi32 )-1.f, ( fi32 )-1.f };
		Vector< f32, 4 > v4a{ -1.f, 2.f, 3.f, 0.5f }, 
						   v4b{ -2.f, 4.f, 6.f, 1.f };
		Vector< s32, 4 > v4i{ -2,   4,   6,   1 };
#else
		Vector< fi32, 2 > v2a = makeVector< fi32 >( ( fi32 )1.f,  ( fi32 )-1.f ),
						    v2b = makeVector< fi32 >( ( fi32 )-1.f, ( fi32 )-1.f );
		Vector< f32, 4 > v4a = makeVector< f32 >( -1.f, 2.f, 3.f, 0.5f ), 
						   v4b = makeVector< f32 >( -2.f, 4.f, 6.f, 1.f );
#endif

		b2 = v2a == v2a;
		assert(  b2[ 0 ] ); assert( b2[ 1 ] );
		b2 = v2a == v2b;
		assert( !b2[ 0 ] ); assert( b2[ 1 ] );
		
		b4 = v4a == v4a;
		assert(  b4[ 0 ] ); assert(  b4[ 1 ] ); assert(  b4[ 2 ] ); assert(  b4[ 3 ] );
		b4 = v4a == v4b;
		assert( !b4[ 0 ] ); assert( !b4[ 1 ] ); assert( !b4[ 2 ] ); assert( !b4[ 3 ] );

		b2 = v2a != v2a;
		assert( !b2[ 0 ] ); assert( !b2[ 1 ] );
		b2 = v2a != v2b;
		assert(  b2[ 0 ] ); assert( !b2[ 1 ] );		
		
		b4 = v4a != v4a;
		assert( !b4[ 0 ] ); assert( !b4[ 1 ] ); assert( !b4[ 2 ] ); assert( !b4[ 3 ] );
		b4 = v4a != v4b;
		assert(  b4[ 0 ] ); assert(  b4[ 1 ] ); assert(  b4[ 2 ] ); assert(  b4[ 3 ] );
				
		b2 = v2a < v2a;
		assert( !b2[ 0 ] ); assert( !b2[ 1 ] );
		b2 = v2b < v2a;
		assert(  b2[ 0 ] ); assert( !b2[ 1 ] );		
		
		b4 = v4a < v4a;
		assert( !b4[ 0 ] ); assert( !b4[ 1 ] ); assert( !b4[ 2 ] ); assert( !b4[ 3 ] );
		b4 = v4a < v4b;
		assert( !b4[ 0 ] ); assert(  b4[ 1 ] ); assert(  b4[ 2 ] ); assert(  b4[ 3 ] );

		b2 = v2a > v2a;
		assert( !b2[ 0 ] ); assert( !b2[ 1 ] );
		b2 = v2a > v2b;
		assert(  b2[ 0 ] ); assert( !b2[ 1 ] );		
		
		b4 = v4a > v4a;
		assert( !b4[ 0 ] ); assert( !b4[ 1 ] ); assert( !b4[ 2 ] ); assert( !b4[ 3 ] );
		b4 = v4a > v4b;
		assert(  b4[ 0 ] ); assert( !b4[ 1 ] ); assert( !b4[ 2 ] ); assert( !b4[ 3 ] );

		b2 = v2a <= v2a;
		assert(  b2[ 0 ] ); assert(  b2[ 1 ] );
		b2 = v2a <= v2b;
		assert( !b2[ 0 ] ); assert(  b2[ 1 ] );		
		
		b4 = v4a <= v4a;
		assert(  b4[ 0 ] ); assert(  b4[ 1 ] ); assert(  b4[ 2 ] ); assert(  b4[ 3 ] );
		b4 = v4a <= v4b;
		assert( !b4[ 0 ] ); assert(  b4[ 1 ] ); assert(  b4[ 2 ] ); assert(  b4[ 3 ] );

		b2 = v2a >= v2a;
		assert(  b2[ 0 ] ); assert(  b2[ 1 ] );
		b2 = v2b >= v2a;
		assert( !b2[ 0 ] ); assert(  b2[ 1 ] );		
		
		b4 = v4a >= v4a;
		assert(  b4[ 0 ] ); assert(  b4[ 1 ] ); assert(  b4[ 2 ] ); assert(  b4[ 3 ] );
		b4 = v4a >= v4b;
		assert(  b4[ 0 ] ); assert( !b4[ 1 ] ); assert( !b4[ 2 ] ); assert( !b4[ 3 ] );

		return true;
	}

	bool TestVector::member_ops( )
	{
		Vector< f64, 9 > v9, vr9;
		Point< f64, 9 > p9;
		f64 a9[ 9 ];
		f64 f64eps = 1e-8;

		for( u32 i = 0; i < 9; ++i ) {
			a9[ i ] = ( f64 )VUL_TEST_RNG;
			v9[ i ] = a9[ i ];
			p9[ i ] = ( f64 )VUL_TEST_RNG;
		}

		vr9 = v9;
		for( u32 i = 0; i < 9; ++i ) {
			assert( abs( vr9[ i ] - v9[ i ] ) < f64eps );
		}

		vr9 += 1.0;
		for( u32 i = 0; i < 9; ++i ) {
			assert( abs( vr9[ i ] - ( v9[ i ] + 1.0 ) ) < f64eps );
		}
		
		vr9 = v9;
		vr9 -= 7.0;
		for( u32 i = 0; i < 9; ++i ) {
			assert( abs( vr9[ i ] - ( v9[ i ] - 7.0 ) ) < f64eps );
		}

		vr9 = v9;
		vr9 *= 1.5;
		for( u32 i = 0; i < 9; ++i ) {
			assert( abs( vr9[ i ] - v9[ i ] * 1.5 ) < f64eps );
		}
		
		vr9 = v9;
		vr9 /= 2.2;
		for( u32 i = 0; i < 9; ++i ) {
			assert( abs( vr9[ i ] - v9[ i ] / 2.2 ) < f64eps );
		}
		
		vr9 = v9;
		vr9 += v9;
		for( u32 i = 0; i < 9; ++i ) {
			assert( abs( vr9[ i ] - ( v9[ i ] + v9[ i ] ) ) < f64eps );
		}
		
		vr9 = v9;
		vr9 -= v9;
		for( u32 i = 0; i < 9; ++i ) {
			assert( abs( vr9[ i ] - ( v9[ i ] - v9[ i ] ) ) < f64eps );
		}

		vr9 = v9;
		vr9 *= v9;
		for( u32 i = 0; i < 9; ++i ) {
			assert( abs( vr9[ i ] - v9[ i ] * v9[ i ] ) < f64eps );
		}
		
		vr9 = v9;
		vr9 /= v9;
		for( u32 i = 0; i < 9; ++i ) {
			assert( abs( vr9[ i ] - v9[ i ] / v9[ i ] ) < f64eps );
		}
		
		for( u32 i = 0; i < 9; ++i ) {
			assert( abs( v9[ i ] - a9[ i ] ) < f64eps );
		}

		if( v9[ 7 ] == 1.7 ) {
			v9[ 7 ] = 1.8;
			assert( v9[ 7 ]== 1.8 );
		} else {
			v9[ 7 ] = 1.7;
			assert( v9[ 7 ]== 1.7 );
		}
		
		p9 = v9.as_point( );
		const Point< f64, 9 > pc9 = v9.as_point( );
		for( u32 i = 0; i < 9; ++i ) {
			assert( v9[ i ] == p9[ i ] );
			assert( v9[ i ] == pc9[ i ] );
		}

		Matrix< f64, 1, 9 > m9c = v9.as_column( );
		Matrix< f64, 9, 1 > m9r = v9.as_row( );
		for( u32 i = 0; i < 9; ++i ) {
			assert( v9[ i ] == m9c( 0, i ) );
			assert( v9[ i ] == m9r( i, 0 ) );
		}
		
		return true;
	}

	bool TestVector::ops( )
	{
		Vector< f32, 19 > va, vb, vr;
		f32 f32eps = 1e-5f;

		for( u32 i = 0; i < 19; ++i ) {
			va[ i ] = VUL_TEST_RNG;
			vb[ i ] = VUL_TEST_RNG;
		}

		vr = va + vb;
		for( u32 i = 0; i < 19; ++i ) {
			assert( abs( vr[ i ] - ( va[ i ] + vb[ i ] ) ) < f32eps );
		}
		vr = va - vb;
		for( u32 i = 0; i < 19; ++i ) {
			assert( abs( vr[ i ] - ( va[ i ] - vb[ i ] ) ) < f32eps );
		}
		vr = va * vb;
		for( u32 i = 0; i < 19; ++i ) {
			assert( abs( vr[ i ] - va[ i ] * vb[ i ] ) < f32eps );
		}
		vr = va / vb;
		for( u32 i = 0; i < 19; ++i ) {
			assert( abs( vr[ i ] - va[ i ] / vb[ i ] ) < f32eps );
		}
		
		vr = va + 1.f;
		for( u32 i = 0; i < 19; ++i ) {
			assert( abs( vr[ i ] - ( va[ i ] + 1.f ) ) < f32eps );
		}
		vr = va - 2.5f;
		for( u32 i = 0; i < 19; ++i ) {
			assert( abs( vr[ i ] - ( va[ i ] -2.5f ) ) < f32eps );
		}
		vr = va * 7.12f;
		for( u32 i = 0; i < 19; ++i ) {
			assert( abs( vr[ i ] - va[ i ] * 7.12f ) < f32eps );
		}
		vr = va / 2.3f;
		for( u32 i = 0; i < 19; ++i ) {
			assert( abs( vr[ i ] - va[ i ] / 2.3f ) < f32eps );
		}

		vr = -va;
		for( u32 i = 0; i < 19; ++i ) {
			assert( abs( vr[ i ] + va[ i ] ) < f32eps );
		}

		return true;
	}

	bool TestVector::functions( )
	{	
		Vector< s64, 2 > v2r;
		Vector< f32, 9 > v9r;
		f32 f32eps = 1e-5f;
		f32 a9a[ 9 ] = { -1.5f, -1.f, -0.75f, -0.5f, 0.f, 0.25f, 0.6f, 1.f, 2.f };
		f32 a9b[ 9 ] = { -1.7f, -1.2f, 0.75f, 0.6f, 0.1f, 0.f, -0.2f, 6.f, 4.f };
#ifdef VUL_CPLUSPLUS11
		Vector< s64, 2 > v2a{ 3L, -1L },
						   v2b{ 2L, 6L };
		Vector< f32, 9 > v9a( a9a ),
						  v9b( a9b );
		Vector< s32, 4 > v4{ 0, 0, 2, 4 };
#else
		Vector< s64, 2 > v2a = makeVector< s64 >( 3L, -1L ),
						  v2b = makeVector< s64 >( 2L, 6L );
		Vector< f32, 9 > v9a = makeVector< f32 >( a9a ),
						  v9b = makeVector< f32 >( a9b );
		Vector< s32, 4 > v4 = makeVector< s32 >( 0, 0, 2, 4 );
#endif

		Vector< f32, 7 > v7 = truncate< f32, 7, 9 >( v9a );
		for( u32 i = 0; i < 7; ++i ) {
			assert( v7[ i ] == v9a[ i ] );
		}

		assert( select( v4 ) == 2 );

#ifdef VUL_CPLUSPLUS11
		Vector< f32, 3 > v3r = cross( Vector< f32, 3 >{ 1.f, 0.f, 0.f }, Vector< f32, 3 >{ 0.f, 0.f, 1.f } );
#else
		Vector< f32, 3 > v3r = cross( makeVector< f32 >( 1.f, 0.f, 0.f ), makeVector< f32 >( 0.f, 0.f, 1.f ) );
#endif
		assert( v3r[ 0 ] == 0.f ); assert( v3r[ 1 ] == -1.f ); assert( v3r[ 2 ] == 0.f );
		
#ifdef VUL_CPLUSPLUS11
		assert( abs( cross( normalize( Vector< f32, 2 >{ 1.f, 1.f } ), normalize( Vector< f32, 2 >{ 1.f, -1.f } ) ) + 1.f ) < f32eps );
#else
		assert( abs( cross( normalize( makeVector< f32 >( 1.f, 1.f ) ), normalize( makeVector< f32 >( 1.f, -1.f ) ) ) + 1.f ) < f32eps );
#endif

		f32 dt = 0.f;
		for( u32 i = 0; i < 9; ++i ) {
			dt += v9a[ i ] * v9b[ i ];
		}
		assert( dt == dot( v9a, v9b ) );
		
#ifdef VUL_CPLUSPLUS11
		assert( norm( Vector< f32, 2 >{ 1.f, 1.f } ) - sqrt( 2.f ) < f32eps );
		assert( norm( Vector< s32, 2 >{ 1, 1 } ) == 1 );
		assert( fnorm( Vector< s32, 2 >{ 1, 1 } ) - sqrt( 2.f ) < f32eps );
#else
		assert( norm( makeVector< f32 >( 1.f, 1.f ) ) - sqrt( 2.f ) < f32eps );
		assert( norm( makeVector< s32 >( 1, 1 ) ) == 1 );
		assert( fnorm( makeVector< s32 >( 1, 1 ) ) - sqrt( 2.f ) < f32eps );
#endif

#ifdef VUL_CPLUSPLUS11
		v3r = normalize( Vector< f32, 3 >{ 1.f, 1.f, 1.f } );
#else
		v3r = normalize( makeVector< f32 >( 1.f, 1.f, 1.f ) );
#endif
		assert( abs( v3r[ 0 ] - 1.f / sqrt( 3.f ) ) < f32eps );
		assert( abs( v3r[ 1 ] - 1.f / sqrt( 3.f ) ) < f32eps );
		assert( abs( v3r[ 2 ] - 1.f / sqrt( 3.f ) ) < f32eps );
		
		v2r = min( v2a, ( s64 )1L );
		assert( v2r[ 0 ] == 1L ); assert( v2r[ 1 ] == -1L );
		v2r = min( v2a, ( s64 )7L );
		assert( v2r[ 0 ] == 3L ); assert( v2r[ 1 ] == -1L );
		v9r = min( v9a, 0.f );
		assert( v9r[ 0 ] == -1.5f ); assert( v9r[ 1 ] == -1.f ); assert( v9r[ 2 ] == -0.75f );
		assert( v9r[ 3 ] == -0.5f ); assert( v9r[ 4 ] ==  0.f ); assert( v9r[ 5 ] ==  0.f );
		assert( v9r[ 6 ] ==  0.f );  assert( v9r[ 7 ] ==  0.f ); assert( v9r[ 8 ] ==  0.f );

		v2r = min( v2a, v2b );
		v9r = min( v9a, v9b );
		assert( v2r[ 0 ] == 2L ); assert( v2r[ 1 ] == -1L );
		assert( v9r[ 0 ] == -1.7f );  assert( v9r[ 1 ] == -1.2f ); assert( v9r[ 2 ] == -0.75f );
		assert( v9r[ 3 ] == -0.5f );  assert( v9r[ 4 ] ==  0.f );  assert( v9r[ 5 ] ==  0.f );
		assert( v9r[ 6 ] ==  -0.2f ); assert( v9r[ 7 ] ==  1.f );  assert( v9r[ 8 ] ==  2.f );
				
		v2r = max( v2a, ( s64 )1L );
		assert( v2r[ 0 ] == 3L ); assert( v2r[ 1 ] == 1L );
		v2r = max( v2a, ( s64 )7L );
		assert( v2r[ 0 ] == 7L ); assert( v2r[ 1 ] == 7L );
		v9r = max( v9a, 0.f );
		assert( v9r[ 0 ] == 0.f );  assert( v9r[ 1 ] == 0.f ); assert( v9r[ 2 ] == 0.f );
		assert( v9r[ 3 ] == 0.f );  assert( v9r[ 4 ] == 0.f ); assert( v9r[ 5 ] == 0.25f );
		assert( v9r[ 6 ] == 0.6f ); assert( v9r[ 7 ] == 1.f ); assert( v9r[ 8 ] == 2.f );

		v2r = max( v2a, v2b );
		v9r = max( v9a, v9b );
		assert( v2r[ 0 ] == 3L ); assert( v2r[ 1 ] == 6L );
		assert( v9r[ 0 ] == -1.5f ); assert( v9r[ 1 ] == -1.f );  assert( v9r[ 2 ] == 0.75f );
		assert( v9r[ 3 ] ==  0.6f ); assert( v9r[ 4 ] ==  0.1f ); assert( v9r[ 5 ] == 0.25f );
		assert( v9r[ 6 ] ==  0.6f ); assert( v9r[ 7 ] ==  6.f );  assert( v9r[ 8 ] == 4.f );

		v2r = abs( v2a );
		v9r = abs( v9a );
		assert( v2r[ 0 ] == 3L ); assert( v2r[ 1 ] == 1L );
		assert( v9r[ 0 ] ==  1.5f ); assert( v9r[ 1 ] ==  1.f ); assert( v9r[ 2 ] == 0.75f );
		assert( v9r[ 3 ] ==  0.5f ); assert( v9r[ 4 ] ==  0.f ); assert( v9r[ 5 ] == 0.25f );
		assert( v9r[ 6 ] ==  0.6f ); assert( v9r[ 7 ] ==  1.f ); assert( v9r[ 8 ] == 2.f );

		v9r = clamp( v9a, -0.5f, 0.75f );
		assert( v9r[ 0 ] == -0.5f ); assert( v9r[ 1 ] == -0.5f );  assert( v9r[ 2 ] == -0.5f );
		assert( v9r[ 3 ] == -0.5f ); assert( v9r[ 4 ] ==  0.f );   assert( v9r[ 5 ] ==  0.25f );
		assert( v9r[ 6 ] ==  0.6f ); assert( v9r[ 7 ] ==  0.75f ); assert( v9r[ 8 ] ==  0.75f );
		
		v9r = saturate( v9b );
		assert( v9r[ 0 ] == 0.f );  assert( v9r[ 1 ] ==  0.f );  assert( v9r[ 2 ] == 0.75f );
		assert( v9r[ 3 ] == 0.6f ); assert( v9r[ 4 ] ==  0.1f ); assert( v9r[ 5 ] == 0.f );
		assert( v9r[ 6 ] == 0.f );  assert( v9r[ 7 ] ==  1.f );  assert( v9r[ 8 ] == 1.f );

		v9r = lerp( v9a, v9b, 0.5f );
		assert( abs( v9r[ 0 ] + 1.6f ) < f32eps );  assert( abs( v9r[ 1 ] + 1.1f ) < f32eps );  assert( abs( v9r[ 2 ] - 0.f ) < f32eps );
		assert( abs( v9r[ 3 ] - 0.05f ) < f32eps ); assert( abs( v9r[ 4 ] - 0.05f ) < f32eps ); assert( abs( v9r[ 5 ] - 0.125f ) < f32eps );
		assert( abs( v9r[ 6 ] - 0.2f ) < f32eps );  assert( abs( v9r[ 7 ] - 3.5f ) < f32eps );  assert( abs( v9r[ 8 ] - 3.f ) < f32eps );
		
		assert( minComponent( v2a ) == -1L );
		assert( minComponent( v2b ) ==  2L );
		
		assert( minComponent( v9a ) == -1.5f );
		assert( minComponent( v9b ) == -1.7f );

		assert( maxComponent( v2a ) ==  3L );
		assert( maxComponent( v2b ) ==  6L );

		assert( maxComponent( v9a ) ==  2.f );
		assert( maxComponent( v9b ) ==  6.f );
		
		return true;
	}
}

#undef VUL_TEST_RNG

#endif
