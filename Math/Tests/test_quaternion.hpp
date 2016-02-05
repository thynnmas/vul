/*
 * Villains' Utility Library - Thomas Martin Schmid, 2016. Public domain¹
 *
 * This file contains tests for the quaternion struct in vul_quaternion.hpp
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
#ifndef VUL_TEST_QUATERNION_HPP
#define VUL_TEST_QUATERNION_HPP

#include <cassert>

#include "../vul_math.hpp"

#pragma warning(disable: 6001)
#define VUL_TEST_RNG ( float )( ( float )rand( ) / ( float )RAND_MAX )

using namespace vul;

namespace vul_test {
	class TestQuaternion {

	public:
		static bool test( );

	private:
		static bool make( );
		static bool member_ops( );
		static bool comparisons( );
		static bool ops( );
		static bool functions( );
	};

	bool TestQuaternion::test( )
	{
		assert( make( ) );
		assert( member_ops( ) );
		assert( comparisons( ) );
		assert( ops( ) );
		assert( functions( ) );

		return true;
	}

	bool TestQuaternion::make( )
	{
		f32 f32eps = 1e-5f;

#ifdef VUL_CPLUSPLUS11
		Quaternion< f32 > qf;
		Quaternion< s64 > qi;
#else
		Quaternion< f32 > qf = makeQuat< f32 >( );
		Quaternion< s64 > qi = makeQuat< s64 >( );
#endif
		assert( qf[ 0 ] == 0.f ); assert( qf[ 1 ] == 0.f ); assert( qf[ 2 ] == 0.f );
		assert( qf[ 3 ] == 1.f );
		assert( qi[ 0 ] == 0.f ); assert( qi[ 1 ] == 0.f ); assert( qi[ 2 ] == 0.f );
		assert( qi[ 3 ] == 1.f );

#ifdef VUL_CPLUSPLUS11
		qf = Quaternion< f32 >{ 1.f, 0.5f, -0.2f, 1.7f };
		qi = Quaternion< s64 >{ 1L, 0L, -2L, 7L };
#else
		qf = makeQuat< f32 >( 1.f, 0.5f, -0.2f, 1.7f );
		qi = makeQuat< s64 >( 1L, 0L, -2L, 7L );
#endif
		assert( qf[ 0 ] == 1.f ); assert( qf[ 1 ] == 0.5f ); assert( qf[ 2 ] == -0.2f ); assert( qf[ 3 ] == 1.7f );
		assert( qi[ 0 ] == 1L );  assert( qi[ 1 ] == 0L );   assert( qi[ 2 ] == -2L );   assert( qi[ 3 ] == 7L );

#ifdef VUL_CPLUSPLUS11
		qf = Quaternion< f32 >( Vector< f32, 3 >{ 1.f, 0.5f, -0.2f }, 1.7f );
		qi = Quaternion< s64 >( Vector< s64, 3 >{ 1L, 0L, -2L }, 7L );
#else
		qf = makeQuat< f32 >( makeVector< f32 >( 1.f, 0.5f, -0.2f ), 1.7f );
		qi = makeQuat< s64 >( makeVector< s64 >( 1L, 0L, -2L ), 7L );
#endif
		assert( qf[ 0 ] == 1.f ); assert( qf[ 1 ] == 0.5f ); assert( qf[ 2 ] == -0.2f ); assert( qf[ 3 ] == 1.7f );
		assert( qi[ 0 ] == 1L );  assert( qi[ 1 ] == 0L );   assert( qi[ 2 ] == -2L );   assert( qi[ 3 ] == 7L );

		f32 af[ 4 ] = { 1.f, 0.5f, -0.2f, 1.7f };
		s32 ai[ 4 ] = { 1, 0, -2, 7 };
		s64 al[ 4 ] = { 1L, 0L, -2L, 7L };
#ifdef VUL_CPLUSPLUS11
		qf = Quaternion< f32 >( af );
		qi = Quaternion< s64 >( al );
#else
		qf = makeQuat< f32 >( af );
		qi = makeQuat< s64 >( al );
#endif
		assert( qf[ 0 ] == 1.f ); assert( qf[ 1 ] == 0.5f ); assert( qf[ 2 ] == -0.2f ); assert( qf[ 3 ] == 1.7f );
		assert( qi[ 0 ] == 1L );  assert( qi[ 1 ] == 0L );   assert( qi[ 2 ] == -2L );   assert( qi[ 3 ] == 7L );

#ifdef VUL_CPLUSPLUS11
		qf = Quaternion< f32 >( ai );
		qi = Quaternion< s64 >( af );
#else
		qf = makeQuat< f32 >( ai );
		qi = makeQuat< s64 >( af );
#endif
		assert( qf[ 0 ] == 1.f ); assert( qf[ 1 ] == 0.f ); assert( qf[ 2 ] == -2.f ); assert( qf[ 3 ] == 7.f );
		assert( qi[ 0 ] == 1L );  assert( qi[ 1 ] == 0L );  assert( qi[ 2 ] == -0L );  assert( qi[ 3 ] == 1L );
		
		f32 el = 1.f / sqrt( 3.f );
#ifdef VUL_CPLUSPLUS11
		qf = makeQuatFromAxisAngle( Vector< f32, 3 >( el ),
								    ( f32 )VUL_PI / 4.f);
#else
		qf = makeQuatFromAxisAngle( makeVector< f32, 3 >( el ),
								    ( f32 )VUL_PI / 4.f);
#endif
		assert( abs( qf.x - el * sin( ( f32 )VUL_PI / 8.f ) ) < f32eps );
		assert( abs( qf.y - el * sin( ( f32 )VUL_PI / 8.f ) ) < f32eps );
		assert( abs( qf.z - el * sin( ( f32 )VUL_PI / 8.f ) ) < f32eps );
		assert( abs( qf.w - cos( ( f32 )VUL_PI / 8.f ) ) < f32eps );

		Vector< f32, 3 > v3c[ 3 ], v3r[ 3 ];
#ifdef VUL_CPLUSPLUS11
		v3c[ 0 ] = Vector< f32, 3 >{ -1.f/sqrtf( 6.f ),  1.f/sqrtf( 2.f ), 1.f/sqrtf( 3.f ) };
		v3c[ 1 ] = Vector< f32, 3 >{ -1.f/sqrtf( 6.f ), -1.f/sqrtf( 2.f ), 1.f/sqrtf( 3.f ) };
		v3c[ 2 ] = Vector< f32, 3 >{  2.f/sqrtf( 6.f ),					0, 1.f/sqrtf( 3.f ) };
		v3r[ 0 ] = Vector< f32, 3 >{ -1.f/sqrtf( 6.f ), -1.f/sqrtf( 6.f ), 2.f/sqrtf( 6.f ) };
		v3r[ 1 ] = Vector< f32, 3 >{  1.f/sqrtf( 2.f ), -1.f/sqrtf( 2.f ), 0 };
		v3r[ 2 ] = Vector< f32, 3 >{  1.f/sqrtf( 3.f ),  1.f/sqrtf( 3.f ), 1.f/sqrtf( 3.f ) };
#else
		v3c[ 0 ] = makeVector< f32 >( -1.f/sqrtf( 6.f ),  1.f/sqrtf( 2.f ), 1.f/sqrtf( 3.f ) );
		v3c[ 1 ] = makeVector< f32 >( -1.f/sqrtf( 6.f ), -1.f/sqrtf( 2.f ), 1.f/sqrtf( 3.f ) );
		v3c[ 2 ] = makeVector< f32 >(  2.f/sqrtf( 6.f ),				 0, 1.f/sqrtf( 3.f ) );
		v3r[ 0 ] = makeVector< f32 >( -1.f/sqrtf( 6.f ), -1.f/sqrtf( 6.f ), 2.f/sqrtf( 6.f ) );
		v3r[ 1 ] = makeVector< f32 >(  1.f/sqrtf( 2.f ), -1.f/sqrtf( 2.f ), 0 );
		v3r[ 2 ] = makeVector< f32 >(  1.f/sqrtf( 3.f ),  1.f/sqrtf( 3.f ), 1.f/sqrtf( 3.f ) );
#endif
		qf = makeQuatFromAxes( v3c[ 0 ], v3c[ 1 ], v3c[ 2 ] );
		f32 root = sqrtf( 1.0f + 1.f / sqrtf( 6.f ) + 1.f / sqrtf( 2.f ) + 1.f / sqrt( 3.f ) );
		assert( abs( qf.x - ( 0.5f / root ) * (  2.f / sqrtf( 6.f ) + 1.f / sqrtf( 3.f ) ) ) < f32eps );
		assert( abs( qf.y - ( 0.5f / root ) * (  1.f / sqrtf( 3.f ) ) ) < f32eps );
		assert( abs( qf.z - ( 0.5f * root ) ) < f32eps );
		assert( abs( qf.w - ( 0.5f / root ) * ( -1.f / sqrtf( 6.f ) - 1.f / sqrtf( 2.f ) ) ) < f32eps );
		
		Matrix< f32, 3, 3 > m33 = makeMatrixFromRows< f32, 3, 3 >( v3r );
		qf = makeQuatFromMatrix( m33 );
		assert( abs( qf.x - ( 0.5f / root ) * (  2.f / sqrtf( 6.f ) + 1.f / sqrtf( 3.f ) ) ) < f32eps );
		assert( abs( qf.y - ( 0.5f / root ) * (  1.f / sqrtf( 3.f ) ) ) < f32eps );
		assert( abs( qf.z - ( 0.5f * root ) ) < f32eps );
		assert( abs( qf.w - ( 0.5f / root ) * ( -1.f / sqrtf( 6.f ) - 1.f / sqrtf( 2.f ) ) ) < f32eps );
				
		qf = makeZero< f32 >( );
		assert( qf[ 0 ] == 0.f ); assert( qf[ 1 ] == 0.f ); assert( qf[ 2 ] == 0.f ); assert( qf[ 3 ] == 0.f );
		qi = makeZero< s64 >( );
		assert( qi[ 0 ] == 0L ); assert( qi[ 1 ] == 0L ); assert( qi[ 2 ] == 0L ); assert( qi[ 3 ] == 0L );
		qf = makeIdentity< f32 >( );
		assert( qf[ 0 ] == 0.f ); assert( qf[ 1 ] == 0.f ); assert( qf[ 2 ] == 0.f );
		assert( qf[ 3 ] == 1.f );
		qi = makeIdentity< s64 >( );
		assert( qi[ 0 ] == 0L ); assert( qi[ 1 ] == 0L ); assert( qi[ 2 ] == 0L );
		assert( qi[ 3 ] == 1L );
		
		return true;
	}

	bool TestQuaternion::member_ops( )
	{
		f32 f32eps = 1e-5f;

		Quaternion< f32 > q = makeIdentity< f32 >( );
#ifdef VUL_CPLUSPLUS11
		assert( q.xyz[ 0 ] == 0.f );
		assert( q.xyz[ 1 ] == 0.f );
		assert( q.xyz[ 2 ] == 0.f );
#else
		assert( q.xyz( )[ 0 ] == 0.f );
		assert( q.xyz( )[ 1 ] == 0.f );
		assert( q.xyz( )[ 2 ] == 0.f );
#endif
		
		assert( q.x == 0.f );
		assert( q.y == 0.f );
		assert( q.z == 0.f );

#ifdef VUL_CPLUSPLUS11
		q.xyz[ 1 ] = 4.f;
#else
		q.xyz( )[ 1 ] = 4.f;
#endif
		assert( q.y == 4.f );
		
#ifdef VUL_CPLUSPLUS11
		Vector< f32, 4 > v = q.as_vec4;
		assert( all( v == Vector< f32, 4 >{ 0.f, 4.f, 0.f, 1.f } ) );
#else
		Vector< f32, 4 > v = q.as_vec4( );
		assert( all( v == makeVector< f32 >( 0.f, 4.f, 0.f, 1.f ) ) );
#endif

		assert( q[ 0 ] == 0.f );
		assert( q[ 1 ] == 4.f );
		assert( q[ 2 ] == 0.f );
		assert( q[ 3 ] == 1.f );

		q[ 2 ] = 0.3f;
		assert( q[ 2 ] == 0.3f );

#ifdef VUL_CPLUSPLUS11
		q += Quaternion< f32 >{ 0.2f, -1.f, 1.3f, 0.f };
#else
		q += makeQuat< f32 >( 0.2f, -1.f, 1.3f, 0.f );
#endif
		assert( abs( q[ 0 ] - 0.2f ) < f32eps );
		assert( abs( q[ 1 ] - 3.f ) < f32eps );
		assert( abs( q[ 2 ] - 1.6f ) < f32eps );
		assert( abs( q[ 3 ] - 1.f ) < f32eps );
		
#ifdef VUL_CPLUSPLUS11
		q -= Quaternion< f32 >{ 0.1f, 1.f, -0.3f, 0.4f };
#else
		q -= makeQuat< f32 >( 0.1f, 1.f, -0.3f, 0.4f );
#endif
		assert( abs( q[ 0 ] - 0.1f ) < f32eps );
		assert( abs( q[ 1 ] - 2.f ) < f32eps );
		assert( abs( q[ 2 ] - 1.9f ) < f32eps );
		assert( abs( q[ 3 ] - 0.6f ) < f32eps );
		
		q *= 1.5f;
		assert( abs( q[ 0 ] - 0.15f ) < f32eps );
		assert( abs( q[ 1 ] - 3.f ) < f32eps );
		assert( abs( q[ 2 ] - 2.85f ) < f32eps );
		assert( abs( q[ 3 ] - 0.9f ) < f32eps );

		Quaternion< f32 > a, b;
#ifdef VUL_CPLUSPLUS11
		a = Quaternion< f32 >{ 2.f, 3.f, 2.f, 3.f };
		b = Quaternion< f32 >{ 3.f, 2.f, 3.f, 2.f };
#else
		a = makeQuat< f32 >( 2.f, 3.f, 2.f, 3.f );
		b = makeQuat< f32 >( 3.f, 2.f, 3.f, 2.f );
#endif
		a *= b;
		assert( abs( a.x - 18.f ) < f32eps );
		assert( abs( a.y - 12.f ) < f32eps );
		assert( abs( a.z -  8.f ) < f32eps );
		assert( abs( a.w + 12.f ) < f32eps );

		assert( abs( b.x - 3.f ) < f32eps ); // Make sure b is unchanged
		assert( abs( b.y - 2.f ) < f32eps );
		assert( abs( b.z - 3.f ) < f32eps );
		assert( abs( b.w - 2.f ) < f32eps );
				
		return true;
	}

	bool TestQuaternion::comparisons( )
	{
		Quaternion< f32 > a, b, c;
#ifdef VUL_CPLUSPLUS11
		a = normalize( Quaternion< f32 >{ VUL_TEST_RNG, VUL_TEST_RNG, VUL_TEST_RNG, VUL_TEST_RNG * ( f32 )VUL_PI } );
		c = normalize( Quaternion< f32 >{ -a.x, -a.y, -a.z, a.w + 0.1f } ); // We guarantee c != a 
#else
		a = normalize( makeQuat< f32 >( VUL_TEST_RNG, VUL_TEST_RNG, VUL_TEST_RNG, VUL_TEST_RNG * ( f32 )VUL_PI ) );
		c = normalize( makeQuat< f32 >( -a.x, -a.y, -a.z, a.w + 0.1f ) ); // We guarantee c != a 
#endif
		b = a;
		
		assert( all( a == a ) );
		assert( all( a == b ) );
		assert( !any( a == c ) );

		assert( !any( a != a ) );
		assert( !any( a != b ) );
		assert( all( a != c ) );

		assert( equals( a, b, 1e-3f ) ); // Need to be quite lenient here since precision issues were found in the GCC/linux tests (debian 64-bit)
		assert( !equals( a, c, 1e-3f ) );
		
		return true;
	}

	bool TestQuaternion::ops( )
	{
		Quaternion< f32 > r, a, b;
		f32 f32eps = 1e-5f;

#ifdef VUL_CPLUSPLUS11
		a = Quaternion< f32 >{ 0.f, 4.f, 0.f, 1.f };
		b = Quaternion< f32 >{ 0.2f, -1.f, 1.3f, 0.f };
#else
		a = makeQuat< f32 >( 0.f, 4.f, 0.f, 1.f );
		b = makeQuat< f32 >( 0.2f, -1.f, 1.3f, 0.f );
#endif
		r = a + b;
		assert( r[ 0 ] == 0.2f );
		assert( r[ 1 ] == 3.f );
		assert( r[ 2 ] == 1.3f );
		assert( r[ 3 ] == 1.f );
		
		r = a - b;
		assert( r[ 0 ] == -0.2f );
		assert( r[ 1 ] == 5.f );
		assert( r[ 2 ] == -1.3f );
		assert( r[ 3 ] == 1.f );
		
		r = b * 1.5f;
		assert( r[ 0 ] ==  0.3f );
		assert( r[ 1 ] == -1.5f );
		assert( r[ 2 ] ==  1.3f * 1.5f );
		assert( r[ 3 ] ==  0.f );

#ifdef VUL_CPLUSPLUS11
		a = Quaternion< f32 >{ 2.f, 3.f, 2.f, 3.f };
		b = Quaternion< f32 >{ 3.f, 2.f, 3.f, 2.f };
#else
		a = makeQuat< f32 >( 2.f, 3.f, 2.f, 3.f );
		b = makeQuat< f32 >( 3.f, 2.f, 3.f, 2.f );
#endif
		r = a * b;
		assert( r.x ==  18.f );
		assert( r.y ==  12.f );
		assert( r.z ==  8.f );
		assert( r.w == -12.f );

		r = -b;
		assert( r[ 0 ] == -3.f );
		assert( r[ 1 ] == -2.f );
		assert( r[ 2 ] == -3.f );
		assert( r[ 3 ] ==  2.f );

#ifdef VUL_CPLUSPLUS11
		a = makeQuatFromAxisAngle( Vector< f32, 3 >{ 0.f, 1.f, 0.f }, ( f32 )VUL_PI / 4.f );
		Vector< f32, 3 > rv, v{ 1.f, 0.f, 0.f };
#else
		a = makeQuatFromAxisAngle( makeVector< f32 >( 0.f, 1.f, 0.f ), ( f32 )VUL_PI / 4.f );
		Vector< f32, 3 > rv, v = makeVector< f32 >( 1.f, 0.f, 0.f );
#endif
		rv = a * v;
		assert( abs( rv[ 0 ] - 1.f / sqrt( 2.f ) ) < f32eps );
		assert( abs( rv[ 1 ] - 0.f ) < f32eps );
		assert( abs( rv[ 2 ] + 1.f / sqrt( 2.f ) ) < f32eps );
		
		return true;
	}

	bool TestQuaternion::functions( )
	{
		// In this, we rely on TestVector having passed!
		Matrix< f32, 3, 3 > m33, r33;
		Vector< f32, 3 > v3[ 3 ];
		f32 f32eps = 1e-5f;
#ifdef VUL_CPLUSPLUS11
		v3[ 0 ] = normalize( Vector< f32, 3 >{ 1.f, 0.f, 0.f } ); // Normalized vector in 3D space
		v3[ 1 ] = normalize( Vector< f32, 3 >{ 0.f, 1.f, 0.f } ); // Perpendicular vector
#else
		v3[ 0 ] = normalize( makeVector< f32 >( 1.f, 0.f, 0.f ) ); // Normalized vector in 3D space
		v3[ 1 ] = normalize( makeVector< f32 >( 0.f, 1.f, 0.f ) ); // Perpendicular vector
#endif
		v3[ 2 ] = normalize( cross( v3[ 0 ], v3[ 1 ] ) );
		m33 = makeMatrixFromRows< f32, 3, 3 >( v3 );
		Quaternion< f32 > q = makeQuatFromMatrix( m33 );
		r33 = makeMatrix( q );
		Quaternion< f32 > q2 = makeQuatFromMatrix( r33 );
		assert( equals( q, q2, 1e-3f ) );

#ifdef VUL_CPLUSPLUS11
		assert( norm( q ) == norm( q.as_vec4 ) );
#else
		assert( norm( q ) == norm( q.as_vec4( ) ) );
#endif

#ifdef VUL_CPLUSPLUS11
		q = Quaternion< f32 >{ 2.4f, 4.2f, 0.8f, 1.2f };
#else
		q = makeQuat< f32 >( 2.4f, 4.2f, 0.8f, 1.2f );
#endif
		Quaternion< f32 > r = normalize( q );
		f32 qn = norm( q );
		assert( abs( r[ 0 ] - q[ 0 ] / qn ) < f32eps );
		assert( abs( r[ 1 ] - q[ 1 ] / qn ) < f32eps );
		assert( abs( r[ 2 ] - q[ 2 ] / qn ) < f32eps );
		assert( abs( r[ 3 ] - q[ 3 ] / qn ) < f32eps );

#ifdef VUL_CPLUSPLUS11
		assert( abs( dot( q, q ) - dot( q.as_vec4, q.as_vec4 ) ) < f32eps );
#else
		assert( abs( dot( q, q ) - dot( q.as_vec4( ), q.as_vec4( ) ) ) < f32eps );
#endif

		r = inverse( q );
		assert( abs( r[ 0 ] - -q[ 0 ] / qn ) < f32eps );
		assert( abs( r[ 1 ] - -q[ 1 ] / qn ) < f32eps );
		assert( abs( r[ 2 ] - -q[ 2 ] / qn ) < f32eps );
		assert( abs( r[ 3 ] -  q[ 3 ] / qn ) < f32eps );

		q = normalize( q );
		r = unitInverse( q );
		assert( abs( r[ 0 ] + q[ 0 ] ) < f32eps );
		assert( abs( r[ 1 ] + q[ 1 ] ) < f32eps );
		assert( abs( r[ 2 ] + q[ 2 ] ) < f32eps );
		assert( abs( r[ 3 ] - q[ 3 ] ) < f32eps );

#ifdef VUL_CPLUSPLUS11
		Quaternion< f32 > a{  0.0f, 1.0f, -0.5f, 0.0f },
						    b{ -0.6f, 0.2f,  0.5f, 1.5f * ( f32 )VUL_PI };
#else
		Quaternion< f32 > a = makeQuat< f32 >(  0.0f, 1.0f, -0.5f, 0.0f ),
						    b = makeQuat< f32 >( -0.6f, 0.2f,  0.5f, 1.5f * ( f32 )VUL_PI );
#endif
		r = nlerp( a, b, 0.f, false );
		q = normalize( a );
		assert( equals( r, q, 1e-3f ) );
		r = nlerp( a, b, 1.f, false );
		q = normalize( b );
		assert( equals( r, q, 1e-3f ) );
		r = nlerp( a, b, 0.6f, false );
#ifdef VUL_CPLUSPLUS11
		q = normalize( Quaternion< f32 >{ -0.36f, 0.52f, 0.1f, 0.9f * ( f32 )VUL_PI } );
#else
		q = normalize( makeQuat< f32 >( -0.36f, 0.52f, 0.1f, 0.9f * ( f32 )VUL_PI ) );
#endif
		assert( equals( r, q, 1e-3f ) );
		r = nlerp( a, b, 0.6f, true );
#ifdef VUL_CPLUSPLUS11
		q = normalize( Quaternion< f32 >{ 0.36f, 0.28f, -0.5f, 0.9f * ( f32 )VUL_PI } );
#else
		q = normalize( makeQuat< f32 >( 0.36f, 0.28f, -0.5f, 0.9f * ( f32 )VUL_PI ) );
#endif
		assert( equals( r, q, 1e-3f ) );

#ifdef VUL_CPLUSPLUS11
		a = Quaternion< f32 >{ 0.f, 0.f, 1.f / sqrtf( 2.f ), 1.f / sqrtf( 2.f ) };
#else
		a = makeQuat< f32 >( 0.f, 0.f, 1.f / sqrtf( 2.f ), 1.f / sqrtf( 2.f ) );
#endif
		b = makeIdentity< f32 >( );

		r = slerp( a, b, 0.33f, false );
		assert( r[ 0 ] == 0.f ); assert( r[ 1 ] == 0.f ); assert( abs( r[ 2 ] - 0.5055f ) < 1e-3f ); assert( abs( r[ 3 ] - 0.86285f ) < 1e-3f );
		
		q = slerp( a,  b, 0.66f, false );
		assert( equals( q, slerp( a, -b, 0.66f, true ), 1e-3f ) );
		
		assert( equals( squadp( a, r, q, b, 0.5f, false ), slerp( a, b, 0.5f, false ), 1e-2f ) );
		
		m33 = makeMatrix( q );
		assert( all( extractAxis( q, 0 ) == column( m33, 0 ) ) );
		assert( all( extractAxis( q, 1 ) == column( m33, 1 ) ) );
		assert( all( extractAxis( q, 2 ) == column( m33, 2 ) ) );

		return true;
	}
}

#undef VUL_TEST_RNG
#pragma warning(default: 6001)

#endif
