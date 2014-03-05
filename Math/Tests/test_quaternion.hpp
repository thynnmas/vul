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
#ifndef VUL_TEST_QUATERNION_HPP
#define VUL_TEST_QUATERNION_HPP

#include <cassert>

#include "../vul_math.hpp"
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

	bool TestQuaternion::make( )
	{
		Quaternion< f32_t > qf;
		Quaternion< i64_t > qi;

#ifdef VUL_CPlUSPLUS11
		qf = Quaternion< f32_t >( );
		qi = Quaternion< i64_t >( );
#else
		qf = makeQuat< f32_t >( );
		qi = makeQuat< i64_t >( );
#endif
		assert( qf[ 0 ] == 0.f ); assert( qf[ 1 ] == 0.f ); assert( qf[ 2 ] == 0.f );
		assert( qf[ 3 ] == 1.f );
		assert( qi[ 0 ] == 0.f ); assert( qi[ 1 ] == 0.f ); assert( qi[ 2 ] == 0.f );
		assert( qi[ 3 ] == 1.f );

#ifdef VUL_CPlUSPLUS11
		qf = Quaternion< f32_t >( 1.f, 0.5f, -0.2f, 1.7f );
		qi = Quaternion< i64_t >( 1L, 0L, -2L, 7L );
#else
		qf = makeQuat< f32_t >( 1.f, 0.5f, -0.2f, 1.7f );
		qi = makeQuat< i64_t >( 1L, 0L, -2L, 7L );
#endif
		assert( qf[ 0 ] == 1.f ); assert( qf[ 1 ] == 0.5f ); assert( qf[ 2 ] == -0.2f ); assert( qf[ 3 ] == 1.7f );
		assert( qi[ 0 ] == 1L );  assert( qi[ 1 ] == 0L );   assert( qi[ 2 ] == -2L );   assert( qi[ 3 ] == 7L );

#ifdef VUL_CPlUSPLUS11
		qf = Quaternion< f32_t >( Vector< f32_t >( 1.f, 0.5f, -0.2f ), 1.7f );
		qi = Quaternion< i64_t >( Vector< i64_t >( 1L, 0L, -2L ), 7L );
#else
		qf = makeQuat< f32_t >( makeVector< f32_t >( 1.f, 0.5f, -0.2f ), 1.7f );
		qi = makeQuat< i64_t >( makeVector< i64_t >( 1L, 0L, -2L ), 7L );
#endif
		assert( qf[ 0 ] == 1.f ); assert( qf[ 1 ] == 0.5f ); assert( qf[ 2 ] == -0.2f ); assert( qf[ 3 ] == 1.7f );
		assert( qi[ 0 ] == 1L );  assert( qi[ 1 ] == 0L );   assert( qi[ 2 ] == -2L );   assert( qi[ 3 ] == 7L );

		f32_t af[ 4 ] = { 1.f, 0.5f, -0.2f, 1.7f };
		i32_t ai[ 4 ] = { 1, 0, -2, 7 };
		i64_t al[ 4 ] = { 1L, 0L, -2L, 7L };
#ifdef VUL_CPlUSPLUS11
		qf = Quaternion< f32_t >( af );
		qi = Quaternion< i64_t >( al );
#else
		qf = makeQuat< f32_t >( af );
		qi = makeQuat< i64_t >( al );
#endif
		assert( qf[ 0 ] == 1.f ); assert( qf[ 1 ] == 0.5f ); assert( qf[ 2 ] == -0.2f ); assert( qf[ 3 ] == 1.7f );
		assert( qi[ 0 ] == 1L );  assert( qi[ 1 ] == 0L );   assert( qi[ 2 ] == -2L );   assert( qi[ 3 ] == 7L );

#ifdef VUL_CPLUSPLUS11
		qf = Quaternion< f32_t >( ai );
		qi = Quaternion< i64_t >( af );
#else
		qf = makeQuat< f32_t >( ai );
		qi = makeQuat< i64_t >( af );
#endif
		assert( qf[ 0 ] == 1.f ); assert( qf[ 1 ] == 0.f ); assert( qf[ 2 ] == -2.f ); assert( qf[ 3 ] == 7.f );
		assert( qi[ 0 ] == 1L );  assert( qi[ 1 ] == 0L );  assert( qi[ 2 ] == -0L );  assert( qi[ 3 ] == 1L );
		
		f32_t el = 1.f / sqrt( 3.f );
#ifdef VUL_CPLUSPLUS11
		qf = makeQuatFromAxisAngle( Vector< f32_t >( el, el, el ),
								    ( f32_t )VUL_PI / 4.f);
#else
		qf = makeQuatFromAxisAngle( makeVector< f32_t >( el, el, el ),
								    ( f32_t )VUL_PI / 4.f);
#endif
		assert( qf.x == el * sin( ( f32_t )VUL_PI / 8.f ) );
		assert( qf.y == el * sin( ( f32_t )VUL_PI / 8.f ) );
		assert( qf.z == el * sin( ( f32_t )VUL_PI / 8.f ) );
		assert( qf.w == cos( ( f32_t )VUL_PI / 8.f ) );

		Vector< f32_t, 3 > v3[ 3 ];
#ifdef VUL_CPLUSPLUS11
		v3[ 0 ] = Vector< f32_t >( -1.f/sqrt( 6.f ), -1.f/sqrt( 6.f ), 2.f/sqrt( 6.f ) );
		v3[ 1 ] = Vector< f32_t >(  1.f/sqrt( 2.f ), -1.f/sqrt( 2.f ), 0 );
		v3[ 2 ] = Vector< f32_t >(  1.f/sqrt( 3.f ), 1.f/sqrt( 3.f ),  1.f/sqrt( 3.f ) );
#else
		v3[ 0 ] = makeVector< f32_t >( -1.f/sqrt( 6.f ), -1.f/sqrt( 6.f ), 2.f/sqrt( 6.f ) );
		v3[ 1 ] = makeVector< f32_t >(  1.f/sqrt( 2.f ), -1.f/sqrt( 2.f ), 0 );
		v3[ 2 ] = makeVector< f32_t >(  1.f/sqrt( 3.f ),  1.f/sqrt( 3.f ), 1.f/sqrt( 3.f ) );
#endif
		qf = makeQuatFromAxes( v3[ 0 ], v3[ 1 ], v3[ 2 ] );
		assert( qf.x == el * sin( ( f32_t )VUL_PI / 8.f ) );
		assert( qf.y == el * sin( ( f32_t )VUL_PI / 8.f ) );
		assert( qf.z == el * sin( ( f32_t )VUL_PI / 8.f ) );
		assert( qf.w == cos( ( f32_t )VUL_PI / 8.f ) );
		
		Matrix< f32_t, 3, 3 > m33 = makeMatrixFromRows< f32_t, 3, 3 >( v3 );
		qf = makeQuatFromMatrix( m33 );
		assert( qf.x == el * sin( ( f32_t )VUL_PI / 8.f ) );
		assert( qf.y == el * sin( ( f32_t )VUL_PI / 8.f ) );
		assert( qf.z == el * sin( ( f32_t )VUL_PI / 8.f ) );
		assert( qf.w == cos( ( f32_t )VUL_PI / 8.f ) );
		
		qf = makeZero< f32_t >( );
		assert( qf[ 0 ] == 0.f ); assert( qf[ 1 ] == 0.f ); assert( qf[ 2 ] == 0.f ); assert( qf[ 3 ] == 0.f );
		qi = makeZero< i64_t >( );
		assert( qi[ 0 ] == 0L ); assert( qi[ 1 ] == 0L ); assert( qi[ 2 ] == 0L ); assert( qi[ 3 ] == 0L );
		qf = makeIdentity< f32_t >( );
		assert( qf[ 0 ] == 0.f ); assert( qf[ 1 ] == 0.f ); assert( qf[ 2 ] == 0.f );
		assert( qf[ 3 ] == 1.f );
		qi = makeIdentity< i64_t >( );
		assert( qi[ 0 ] == 0L ); assert( qi[ 1 ] == 0L ); assert( qi[ 2 ] == 0L );
		assert( qi[ 3 ] == 1L );
		
		return true;
	}

	bool TestQuaternion::member_ops( )
	{
		Quaternion< f32_t > q = makeIdentity< f32_t >( );
		assert( q.xyz( )[ 0 ] == 0.f );
		assert( q.xyz( )[ 1 ] == 0.f );
		assert( q.xyz( )[ 2 ] == 0.f );
		
		assert( q.x == 0.f );
		assert( q.y == 0.f );
		assert( q.z == 0.f );

		q.xyz( )[ 1 ] = 4.f;
		assert( q.y = 4.f );
		
		Vector< f32_t, 4 > v = q.as_vec4( );
#ifdef VUL_CPLUSPLUS11
		assert( all( v == Vector< f32_t >( 0.f, 4.f, 0.f, 1.f ) ) );
#else
		assert( all( v == makeVector< f32_t >( 0.f, 4.f, 0.f, 1.f ) ) );
#endif

		assert( q[ 0 ] == 0.f );
		assert( q[ 1 ] == 4.f );
		assert( q[ 2 ] == 0.f );
		assert( q[ 3 ] == 1.f );

		q[ 2 ] = 0.3f;
		assert( q[ 2 ] == 0.3f );

#ifdef VUL_CPLUSPLUS11
		q += Quat< f32_t >( 0.2f, -1.f, 1.3f, 0.f );
#else
		q += makeQuat< f32_t >( 0.2f, -1.f, 1.3f, 0.f );
#endif
		assert( q[ 0 ] == 0.2f );
		assert( q[ 1 ] == 3.f );
		assert( q[ 2 ] == 1.6f );
		assert( q[ 3 ] == 1.f );
		
#ifdef VUL_CPLUSPLUS11
		q -= Quat< f32_t >( 0.1f, 1.f, -0.3f, 0.4f );
#else
		q -= makeQuat< f32_t >( 0.1f, 1.f, -0.3f, 0.4f );
#endif
		assert( q[ 0 ] == 0.1f );
		assert( q[ 1 ] == 2.f );
		assert( q[ 2 ] == 1.9f );
		assert( q[ 3 ] == 0.6f );
		
		q *= 1.5f;
		assert( q[ 0 ] == 0.15f );
		assert( q[ 1 ] == 3.f );
		assert( q[ 2 ] == 2.85f );
		assert( q[ 3 ] == 0.9f );

		Quaternion< f32_t > a, b;
#ifdef VUL_CPLUSPLUS11
		a = Quat< f32_t >( 2.f, 3.f, 2.f, 3.f );
		b = Quat< f32_t >( 3.f, 2.f, 3.f, 2.f );
#else
		a = makeQuat< f32_t >( 2.f, 3.f, 2.f, 3.f );
		b = makeQuat< f32_t >( 3.f, 2.f, 3.f, 2.f );
#endif
		a *= b;
		assert( a.x == -12.f );
		assert( a.y == 8.f );
		assert( a.z == 12.f );
		assert( a.w == 18.f );

		assert( b.x == 3.f ); // Make sure b is unchanged
		assert( b.y == 2.f );
		assert( b.z == 3.f );
		assert( b.w == 2.f );
				
		return true;
	}

	bool TestQuaternion::comparisons( )
	{
		Quaternion< f32_t > a, b, b2, c;
#ifdef VUL_CPLUSPLUS11
		a = Quat< f32_t >( VUL_TEST_RNG, VUL_TEST_RNG, VUL_TEST_RNG, VUL_TEST_RNG * ( f32_t )VUL_PI );
		c = Quat< f32_t >( -a.x, -a.y, -a.z, a.w + 0.1f ); // We guarantee c != a 
#else
		a = makeQuat< f32_t >( VUL_TEST_RNG, VUL_TEST_RNG, VUL_TEST_RNG, VUL_TEST_RNG * ( f32_t )VUL_PI );
		c = makeQuat< f32_t >( -a.x, -a.y, -a.z, a.w + 0.1f ); // We guarantee c != a 
#endif
		b = a;
		b2 = b;
		b2.w = b2.w - ( 2.f * ( f32_t )VUL_PI );
		
		assert( all( a == a ) );
		assert( all( a == b ) );
		assert( !any( a == c ) );

		assert( !any( a != a ) );
		assert( !any( a != b ) );
		assert( all( a != c ) );

		assert( equals( a, b, 1e-5f ) );
		assert( !equals( a, c, 1e-5f ) );
		// Now check that full rotations around the given axis, which are geometrically equal, are equal
		assert( equals( a, b2, 1e-5f ) );
		
		return true;
	}

	bool TestQuaternion::ops( )
	{
		Quaternion< f32_t > r, a, b;

#ifdef VUL_CPLUSPLUS11
		a = Quat< f32_t >( 0.f, 4.f, 0.f, 1.f );
		b = Quat< f32_t >( 0.2f, -1.f, 1.3f, 0.f );
#else
		a = makeQuat< f32_t >( 0.f, 4.f, 0.f, 1.f );
		b = makeQuat< f32_t >( 0.2f, -1.f, 1.3f, 0.f );
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
		a = Quat< f32_t >( 2.f, 3.f, 2.f, 3.f );
		b = Quat< f32_t >( 3.f, 2.f, 3.f, 2.f );
#else
		a = makeQuat< f32_t >( 2.f, 3.f, 2.f, 3.f );
		b = makeQuat< f32_t >( 3.f, 2.f, 3.f, 2.f );
#endif
		r = a * b;
		assert( r.x == -12.f );
		assert( r.y == 8.f );
		assert( r.z == 12.f );
		assert( r.w == 18.f );

		r = -b;
		assert( r[ 0 ] == -0.2f );
		assert( r[ 0 ] ==  1.f );
		assert( r[ 0 ] == -1.3f );
		assert( r[ 0 ] == -0.f );

#ifdef VUL_CPLUSPLUS11
		a = makeQuatFromAxisAngle( Vector< f32_t >( 0.f, 1.f, 0.f ), ( f32_t )VUL_PI / 4.f );
		Vector< f32_t, 3 > rv, v( 1.f, 0.f, 0.f );
#else
		a = makeQuatFromAxisAngle( makeVector< f32_t >( 0.f, 1.f, 0.f ), ( f32_t )VUL_PI / 4.f );
		Vector< f32_t, 3 > rv, v = makeVector< f32_t >( 1.f, 0.f, 0.f );
#endif
		rv = a * v;
		assert( rv[ 0 ] == 1.f / sqrt( 2.f ) );
		assert( rv[ 1 ] == 0.f );
		assert( rv[ 2 ] == 1.f / sqrt( 2.f ) );
		
		return true;
	}

	bool TestQuaternion::functions( )
	{
		// In this, we rely on TestVector having passed!
		Matrix< f32_t, 3, 3 > m33, r33;
		Vector< f32_t, 3 > v3[ 3 ];
#ifdef VUL_CPLUSPLUS11
		v3[ 0 ] = Vector< f32_t >( VUL_TEST_RNG * 2.f - 1.f, VUL_TEST_RNG * 2.f - 1.f, VUL_TEST_RNG * 2.f - 1.f ); // Normalized vector in 3D space
		v3[ 1 ] = cross( v31, Vector< f32_t >( 1.f, 0.f, 0.f ) ); // Cross with some vector to get a orthogonal second vector
#else
		v3[ 0 ] = makeVector< f32_t >( VUL_TEST_RNG * 2.f - 1.f, VUL_TEST_RNG * 2.f - 1.f, VUL_TEST_RNG * 2.f - 1.f ); // Normalized vector in 3D space
		v3[ 1 ] = cross( v3[ 0 ], makeVector< f32_t >( 1.f, 0.f, 0.f ) ); // Cross with some vector to get a orthogonal second vector
#endif
		v3[ 2 ] = cross( v3[ 0 ], v3[ 1 ] );
		m33 = makeMatrixFromRows< f32_t, 3, 3 >( v3 );
		Quaternion< f32_t > q = makeQuatFromMatrix( m33 );
		r33 = makeMatrix( q );
		for( ui32_t y = 0; y < 3; ++y ) {
			for( ui32_t x = 0; x < 3; ++x ) {
				assert( r33( y, x ) == m33( y, x ) );
			}
		}

		assert( norm( q ) == norm( q.as_vec4( ) ) );

#ifdef VUL_CPLUSPLUS11
		q = Quaternion< f32_t >( 2.4f, 4.2f, 0.8f, 1.2f );
#else
		q = makeQuat< f32_t >( 2.4f, 4.2f, 0.8f, 1.2f );
#endif
		Quaternion< f32_t > r = normalize( q );
		f32_t qn = norm( q );
		assert( r[ 0 ] == q[ 0 ] / qn );
		assert( r[ 1 ] == q[ 1 ] / qn );
		assert( r[ 2 ] == q[ 2 ] / qn );
		assert( r[ 3 ] == q[ 3 ] / qn );

		assert( dot( q, q ) == dot( q.as_vec4( ), q.as_vec4( ) ) );

		r = inverse( q );
		assert( r[ 0 ] == -q[ 0 ] / qn );
		assert( r[ 1 ] == -q[ 1 ] / qn );
		assert( r[ 2 ] == -q[ 2 ] / qn );
		assert( r[ 3 ] ==  q[ 3 ] / qn );

		q = normalize( q );
		r = unitInverse( q );
		assert( r[ 0 ] == -q[ 0 ] );
		assert( r[ 1 ] == -q[ 1 ] );
		assert( r[ 2 ] == -q[ 2 ] );
		assert( r[ 3 ] ==  q[ 3 ] );

		
		/* @TODO: These. For all three, test t=0, t=1, t=0.6. 
		          For slerp/squadp, choose a case where useShortestPath is different from not.

		Quaternion< T > slerp( const Quaternion< T > &a, const Quaternion< T > &b, T t, bool useShortestPath );
		Quaternion< T > nlerp( const Quaternion< T > &a, const Quaternion< T > &b, T t );
		Quaternion< T > squadp( const Quaternion< T > &a, const Quaternion< T > &b,
								const Quaternion< T > &c, const Quaternion< T > &d, 
								T t, bool useShortestPath );
		*/
		
		return true;
	}
}

#undef VUL_TEST_RNG

#endif