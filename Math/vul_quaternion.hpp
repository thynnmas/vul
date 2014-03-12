/*
 * Villains' Utility Library - Thomas Martin Schmid, 2014. Public domain¹
 *
 * This file describes a quaternion type that interfaces with the linear math
 * classes otherwise defined in this library. While any numeric type is in theory
 * allowed, it only makes sense to use real types. i32_termediate calculations
 * and results of norm, dot etc. are returned in the same type as the quaternion's
 * content.
 *
 * The quaternions are scalar.
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
#ifndef VUL_QUATERNION_HPP
#define VUL_QUATERNION_HPP

#include "vul_types.hpp"
#include "vul_vector.hpp"
#include "vul_matrix.hpp"

/**
 * PI...
 */
#define VUL_PI 3.1415926535897932384626433832795

/**
 * Some epsilon, for angles smaller than which a normal linear interpolation
 * is performed instead of a spherical one.
 */
#define VUL_QUATERNION_SLERP_EPSILON 1e-03

namespace vul {
	
	//----------------
	// Declarations
	//

	template< typename T >
	struct Quaternion { 
		union { 
			T data[ 4 ]; 
			struct { T x, y, z, w; };
#ifdef VUL_CPLUSPLUS11
			Vector< T, 3 > xyz;
			Vector< T, 4 > as_vec4;
#endif
		};
#ifdef VUL_CPLUSPLUS11
		constexpr Quaternion< T >( );
		explicit Quaternion< T >( T x, T y, T z, T w ); 
		explicit Quaternion< T >( Vector< T, 3 > xyz, T w );
		explicit Quaternion< T >( const T (& a)[ n ] );
		explicit Quaternion< T >( f32_t (& a)[ n ] );
		explicit Quaternion< T >( i32_t (& a)[ n ] );
#else
		Vector< T, 3 > & xyz( ) { return reinterpret_cast< Vector< T, 3 > & >( data ); }
		const Vector< T, 3 > & xyz( ) const { return reinterpret_cast< const Vector< T, 3 > & >( data ); }
		Vector< T, 4 > & as_vec4( ) { return reinterpret_cast< Vector< T, 4 > & >( data ); }
		const Vector< T, 4 > & as_vec4( ) const { return reinterpret_cast< const Vector< T, 4 > & >( data ); }
#endif
		// Operators
		/**
		 * Compnentwise addition.
		 */
		Quaternion< T > &operator+=( const Quaternion &rhs );
		/**
		 * Compnentwise subtraction.
		 */
		Quaternion< T > &operator-=( const Quaternion &rhs );

		/**
		 * Compnentwise mutliplication.
		 */
		Quaternion< T > &operator*=( const T scalar );
		/**
		 * Quaternion multiplication.
		 * @Note: This is not generally commutative.
		 */
		Quaternion< T > &operator*=( const Quaternion &rhs );

		/**
		 * Indexing operator.
		 */
		T &operator[ ]( i32_t i );
		/**
		 * Constant indexing operator.
		 */
		T const &operator[ ]( i32_t i ) const;
	}; 

	typedef Quaternion< f32_t > quat;
	typedef Quaternion< f64_t > dquat;
	typedef Quaternion< f16_t > hquat;

#ifndef VUL_CPLUSPLUS11
	template< typename T >
	Quaternion< T > makeQuat( T x, T y, T z, T w );

	template< typename T >
	Quaternion< T > makeQuat( Vector< T, 3 > xyz, T w ); 

	template< typename T >
	Quaternion< T > makeQuat( );
	
	template< typename T >
	Quaternion< T > makeQuat( const T (& a)[ 4 ] );
	
	template< typename T >
	Quaternion< T > makeQuat( f32_t (& a)[ 4 ] );
	
	template< typename T >
	Quaternion< T > makeQuat( i32_t (& a)[ 4 ] );
#endif
	/**
	 * Construct a quaternion from and axis and an angle of rotation around that axis.
	 */
	template< typename T >
	Quaternion< T > makeQuatFromAxisAngle( const Vector< T, 3 > &axis, T angleRadians );
	/**
	 * Construct a quaternion from a 3-axis system.
	 */
	template< typename T >
	Quaternion< T > makeQuatFromAxes( const Vector< T, 3 > &x, const Vector< T, 3 > &y, const Vector< T, 3 > &z );
	/**
	 * Construct a quaternion from a rotation matrix.
	 * Uses Ken Shoemake's algorithm from the 1987 SIGGRAPH course notes
     * "Quaternion Calculus and Fast Animation".
	 */
	template< typename T >
	Quaternion< T > makeQuatFromMatrix( const Matrix< T, 3, 3 > &mat );
	// Special cases
	/**
	 * Create the zero-quaternion.
	 */
	template< typename T >
	Quaternion< T > makeZero( );
	/**
	 * Create the identity quaternion.
	 */
	template< typename T >
	Quaternion< T > makeIdentity( );

	// Quaternion operators
	/**
	 * Componentwise addition.
	 */
	template< typename T >
	Quaternion< T > operator+( const Quaternion< T > &a, const Quaternion< T > &b );
	/**
	 * Componentwise subtraction.
	 */
	template< typename T >
	Quaternion< T > operator-( const Quaternion< T > &a, const Quaternion< T > &b );
	
	/**
	 * Componentwise multiplication.
	 */
	template< typename T >
	Quaternion< T > operator*( const Quaternion< T > &q, const T scalar );
	
	/**
	* Quaternion multiplication.
	* @Note: This is not generally commutative.
	*/
	template< typename T >
	Quaternion< T > operator*( const Quaternion< T > &a, const Quaternion< T > &b );
	/**
	 * Rotate a vector by a quaternion.
	 */
	template< typename T >
	Vector< T, 3 > operator*( const Quaternion< T > &q, const Vector< T, 3 > &v );
	
	/**
	 * Negate a quaternion. Equivalent to filpping all axes.
	 */
	template< typename T >
	Quaternion< T > operator-( const Quaternion< T > &q );

	/**
	 * Componentwise equality. To test if two quaternions are entirely equal,
	 * use all( a == b ), or two see if two quaternions describe the same orientation
	 * within a given tolerance, use equals.
	 */
	template< typename T >
	Vector< bool, 4 > operator==( const Quaternion< T > &a, const Quaternion< T > &b ); 
	/**
	 * Componentwise inequality. To test if two quaternions are entirely unequal,
	 * use !any( a != b );
	 */
	template< typename T >
	Vector< bool, 4 > operator!=( const Quaternion< T > &a, const Quaternion< T > &b );
	
	// Quaternion functions
	/**
	 * Construct a 3x3 rotation matrix from the orientation of the given quaternion.
	 */
	template< typename T >
	Matrix< T, 3, 3 > makeMatrix( const Quaternion< T > &quat );
	/**
	 * Compare two quaternions within a given tolerance.
	 */
	template< typename T >
	bool equals( const Quaternion< T > &a, const Quaternion< T > &b, const T tolerance );
	/**
	 * Calculate the norm of the quaternion.
	 */
	template< typename T >
	T norm( const Quaternion< T > &q );
	/**
	 * Normalize the quaternion. Not in place.
	 */
	template< typename T >
	Quaternion< T > normalize( const Quaternion< T > &q );
	/**
	 * Returns the dot product of two quaternions, as if they were 4-vectors.
	 */
	template< typename T >
	T dot( const Quaternion< T > &a, const Quaternion< T > &b );
	/**
	 * Calculates the normalized inverse of a quaternion.
	 */
	template< typename T >
	Quaternion< T > inverse( const Quaternion< T > &q );
	/**
	 * Calculates the inverse of a normalized quaternion.
	 */
	template< typename T >
	Quaternion< T > unitInverse( const Quaternion< T > &q );
	
	/** 
	 * Spherical linear interpolation. If useShortestPath is set, 
	 * the shortes way around the sphere is chosen.
	 */
	template< typename T >
	Quaternion< T > slerp( const Quaternion< T > &a, const Quaternion< T > &b, 
						   T t, bool useShortestPath );
	/**
	 * Normalized linear interpolation
	 */
	template< typename T >
	Quaternion< T > nlerp( const Quaternion< T > &a, const Quaternion< T > &b, T t );
	/** 
	 * Spherical quadratic interpolation. If useShortestPath is set, 
	 * the shortes way around the sphere is chosen.
	 */
	template< typename T >
	Quaternion< T > squadp( const Quaternion< T > &a, const Quaternion< T > &b,
							const Quaternion< T > &c, const Quaternion< T > &d, 
							T t, bool useShortestPath );


	//----------------
	// Definitions
	//

#ifdef VUL_CPLUSPLUS11	
	template< typename T >
	constexpr Quaternion< T >::Quaternion< T >( )
	{
		data[ 0 ] = data[ 1 ] = data[ 2 ] = static_cast< T >( 0.f );
		data[ 3 ] = static_cast< T >( 1.f );
	}
	
	template< typename T >
	Quaternion< T >::Quaternion< T >( T x, T y, T z, T w )
	{
		data[ 0 ] = x;
		data[ 1 ] = y;
		data[ 2 ] = z;
		data[ 3 ] = w;
	}
	
	template< typename T >
	Quaternion< T >::Quaternion< T >( Vector< T, 3 > xyz, T w )
	{
		data[ 0 ] = xyz[ 0 ];
		data[ 1 ] = xyz[ 1 ];
		data[ 2 ] = xyz[ 2 ];
		data[ 3 ] = w;
	}
	
	template< typename T >
	Quaternion< T >::Quaternion< T >( const T (& a)[ n ] )
	{
		memcpy( data, a, sizeof( T ) * 4 );
	}
	
	template< typename T >
	Quaternion< T >::Quaternion< T >( f32_t (& a)[ n ] )
	{
		data[ 0 ] = static_cast< T >( a[ 0 ] );
		data[ 1 ] = static_cast< T >( a[ 1 ] );
		data[ 2 ] = static_cast< T >( a[ 2 ] );
		data[ 3 ] = static_cast< T >( a[ 3 ] );
	}
	
	template< typename T >
	Quaternion< T >::Quaternion< T >( i32_t (& a)[ n ] )
	{
		data[ 0 ] = static_cast< T >( a[ 0 ] );
		data[ 1 ] = static_cast< T >( a[ 1 ] );
		data[ 2 ] = static_cast< T >( a[ 2 ] );
		data[ 3 ] = static_cast< T >( a[ 3 ] );
	}
#else
	template< typename T >
	Quaternion< T > makeQuat( )
	{
		Quaternion< T > q;

		q[ 0 ] = q[ 1 ] = q[ 2 ] = static_cast< T >( 0.f );
		q[ 3 ] = static_cast< T >( 1.f );

		return q;
	}
	
	template< typename T >
	Quaternion< T > makeQuat( T x, T y, T z, T w )
	{
		Quaternion< T > q;

		q[ 0 ] = x;
		q[ 1 ] = y;
		q[ 2 ] = z;
		q[ 3 ] = w;

		return q;
	}

	template< typename T >
	Quaternion< T > makeQuat( Vector< T, 3 > xyz, T w )
	{
		Quaternion< T > q;
		
		q[ 0 ] = xyz[ 0 ];
		q[ 1 ] = xyz[ 1 ];
		q[ 2 ] = xyz[ 2 ];
		q[ 3 ] = w;

		return q;
	}
	
	template< typename T >
	Quaternion< T > makeQuat( const T (& a)[ 4 ] )
	{
		Quaternion< T > q;

		q[ 0 ] = a[ 0 ];
		q[ 1 ] = a[ 1 ];
		q[ 2 ] = a[ 2 ];
		q[ 3 ] = a[ 3 ];

		return q;
	}
	
	template< typename T >
	Quaternion< T > makeQuat( f32_t (& a)[ 4 ] )
	{
		Quaternion< T > q;

		q[ 0 ] = static_cast< T >( a[ 0 ] );
		q[ 1 ] = static_cast< T >( a[ 1 ] );
		q[ 2 ] = static_cast< T >( a[ 2 ] );
		q[ 3 ] = static_cast< T >( a[ 3 ] );

		return q;
	}
	
	template< typename T >
	Quaternion< T > makeQuat( i32_t (& a)[ 4 ] )
	{
		Quaternion< T > q;

		q[ 0 ] = static_cast< T >( a[ 0 ] );
		q[ 1 ] = static_cast< T >( a[ 1 ] );
		q[ 2 ] = static_cast< T >( a[ 2 ] );
		q[ 3 ] = static_cast< T >( a[ 3 ] );

		return q;
	}
#endif
	template< typename T >
	Quaternion< T > makeQuatFromAxisAngle( const Vector< T, 3 > &axis, T angleRadians )
	{
		Quaternion< T > q;
		T sina, halfAngle;

		halfAngle = angleRadians / static_cast< T >( 2.f );
		
		sina = sin( halfAngle );
		q[ 0 ] = axis[ 0 ] * sina;
		q[ 1 ] = axis[ 1 ] * sina;
		q[ 2 ] = axis[ 2 ] * sina;
		q[ 3 ] = cos( halfAngle );	

		return q;
	}

	template< typename T >
	Quaternion< T > makeQuatFromAxes( const Vector< T, 3 > &x, const Vector< T, 3 > &y, const Vector< T, 3 > &z )
	{
		Matrix< T, 3, 3 > m;
				
		m = makeMatrix33FromColumns( x, y, z );
		
		return makeQuatFromMatrix( m );
	}
	template< typename T >
	Quaternion< T > makeQuatFromMatrix( const Matrix< T, 3, 3 > &mat )
	{

		Quaternion< T > q;
		T trace, root;
		T zero, pfive, one;
		ui32_t i, j, k;
		const ui32_t next[ 3 ] = { 1, 2, 0 };

		zero = static_cast< T >( 0.f );
		pfive = static_cast< T >( 0.5f );
		one = static_cast< T >( 1.f );

		trace = mat( 0, 0 ) + mat( 1, 1 ) + mat( 2, 2 );

		if ( trace > zero )
		{
			root = sqrt( trace + one );
			q[ 3 ] = pfive * root;
			root = pfive / root;
			q[ 0 ] = root * ( mat( 1, 2 ) - mat( 2, 1 ) );
			q[ 1 ] = root * ( mat( 2, 0 ) - mat( 0, 2 ) );
			q[ 2 ] = root * ( mat( 0, 1 ) - mat( 1, 0 ) );
		} else {
			i = 0;
			if( mat( 1, 1 ) > mat( 0, 0 ) ) {
				i = 1;
			}
			if( mat( 2, 2 ) > mat( 1, 1 ) ) {
				i = 2;
			}
			j = next[ i ];
			k = next[ j ];

			root = sqrt( mat( i, i ) - mat( j, j ) - mat( k, k ) + one );
			q[ i ] = pfive * root;
			root = pfive / root;
			q[ 3 ] = root * ( mat( j, k ) - mat( k, j ) );
			q[ j ] = root * ( mat( i, j ) - mat( j, i ) );
			q[ k ] = root * ( mat( i, k ) - mat( k, i ) );
		}

		return q;
	}
	// Special cases
	template< typename T >
	Quaternion< T > makeZero( )
	{
		Quaternion< T > q;

		q[ 0 ] = q[ 1 ] = q[ 2 ] = q[ 3 ] = static_cast< T >( 0.f );

		return q;
	}
	template< typename T >
	Quaternion< T > makeIdentity( )
	{
		Quaternion< T > q;

		q[ 0 ] = q[ 1 ] = q[ 2 ] = static_cast< T >( 0.f );
		q[ 3 ] = static_cast< T >( 1.f );

		return q;
	}


	// Quaternion operators	
	template< typename T >
	Quaternion< T > &Quaternion< T >::operator+=( const Quaternion< T > &rhs )
	{
		data[ 0 ] += rhs[ 0 ];
		data[ 1 ] += rhs[ 1 ];
		data[ 2 ] += rhs[ 2 ];
		data[ 3 ] += rhs[ 3 ];
		return *this;
	}	
	template< typename T >
	Quaternion< T > &Quaternion< T >::operator-=( const Quaternion< T > &rhs )
	{
		data[ 0 ] -= rhs[ 0 ];
		data[ 1 ] -= rhs[ 1 ];
		data[ 2 ] -= rhs[ 2 ];
		data[ 3 ] -= rhs[ 3 ];
		return *this;
	}

	template< typename T >
	Quaternion< T > &Quaternion< T >::operator*=( const T scalar )
	{
		data[ 0 ] *= scalar;
		data[ 1 ] *= scalar;
		data[ 2 ] *= scalar;
		data[ 3 ] *= scalar;
		return *this;
	}
	/**
	 * Quaternion multiplication. In place.
	 * @Note: This is not generally commutative.
	 */
	template< typename T >
	Quaternion< T > &Quaternion< T >::operator*=( const Quaternion< T > &rhs )
	{
		const T x = data[ 0 ],
				y = data[ 1 ],
				z = data[ 2 ],
				w = data[ 3 ];

		data[ 0 ] = w * rhs[ 0 ] + rhs[ 3 ] * x + y * rhs[ 2 ] - z * rhs[ 1 ];
		data[ 1 ] = w * rhs[ 1 ] + rhs[ 3 ] * y + z * rhs[ 0 ] - x * rhs[ 2 ];
		data[ 2 ] = w * rhs[ 2 ] + rhs[ 3 ] * z + x * rhs[ 1 ] - y * rhs[ 0 ];

		data[ 3 ] = w * rhs[ 3 ] - x * rhs[ 0 ] - y * rhs[ 1 ] - z * rhs[ 2 ];

		return *this;
	}

	template< typename T >
	T &Quaternion< T >::operator[ ]( i32_t i )
	{
		assert( i < 4 );
		return data[ i ];
	}
	template< typename T >
	T const &Quaternion< T >::operator[ ]( i32_t i ) const
	{
		assert( i < 4 );
		return data[ i ];
	}

	template< typename T >
	Quaternion< T > operator+( const Quaternion< T > &a, const Quaternion< T > &b )
	{
		Quaternion< T > q;
		
		q[ 0 ] = a[ 0 ] + b[ 0 ];
		q[ 1 ] = a[ 1 ] + b[ 1 ];
		q[ 2 ] = a[ 2 ] + b[ 2 ];
		q[ 3 ] = a[ 3 ] + b[ 3 ];

		return q;
	}
	template< typename T >
	Quaternion< T > operator-( const Quaternion< T > &a, const Quaternion< T > &b )
	{
		Quaternion< T > q;
		
		q[ 0 ] = a[ 0 ] - b[ 0 ];
		q[ 1 ] = a[ 1 ] - b[ 1 ];
		q[ 2 ] = a[ 2 ] - b[ 2 ];
		q[ 3 ] = a[ 3 ] - b[ 3 ];

		return q;
	}
	template< typename T >
	Quaternion< T > operator*( const Quaternion< T > &q, const T scalar )
		{
		Quaternion< T > r;
		
		r[ 0 ] = q[ 0 ] * scalar;
		r[ 1 ] = q[ 1 ] * scalar;
		r[ 2 ] = q[ 2 ] * scalar;
		r[ 3 ] = q[ 3 ] * scalar;

		return r;
	}
	/**
	 * Quaternion multiplication. 
	 * @Note: This is not generally commutative.
	 */
	template< typename T >
	Quaternion< T > operator*( const Quaternion< T > &a, const Quaternion< T > &b )
	{
		Quaternion< T > q;
		
		q[ 0 ] = a[ 3 ] * b[ 0 ] + b[ 3 ] * a[ 0 ] + a[ 1 ] * b[ 2 ] - a[ 2 ] * b[ 1 ];
		q[ 1 ] = a[ 3 ] * b[ 1 ] + b[ 3 ] * a[ 1 ] + a[ 2 ] * b[ 0 ] - a[ 0 ] * b[ 2 ];
		q[ 2 ] = a[ 3 ] * b[ 2 ] + b[ 3 ] * a[ 2 ] + a[ 0 ] * b[ 1 ] - a[ 1 ] * b[ 0 ];

		q[ 3 ] = a[ 3 ] * b[ 3 ] - a[ 0 ] * b[ 0 ] - a[ 1 ] * b[ 1 ] - a[ 2 ] * b[ 2 ];

		return q;
	}

	/**
	 * Rotate a vector by a quaternion
	 */
	template< typename T >
	Vector< T, 3 > operator*( const Quaternion< T > &q, const Vector< T, 3 > &v )
	{
		Vector< T, 3 > uv, uuv;
		T two;

		two = static_cast< T >( 2.f );
#ifdef VUL_CPLUSPLUS11
		uv = cross( q.xyz, v );
		uuv = cross( q.xyz, uv );
#else
		uv = cross( q.xyz( ), v );
		uuv = cross( q.xyz( ), uv );
#endif
		uv *= two * q.w;
		uuv *= two;

		return v + uv + uuv;
	}
	
	template< typename T >
	Quaternion< T > operator-( const Quaternion< T > &q )
	{
		Quaternion< T > r;
		
		// Negate the direction
		r[ 0 ] = -q[ 0 ];
		r[ 1 ] = -q[ 1 ];
		r[ 2 ] = -q[ 2 ];
		// Maintain angle
		r[ 3 ] = q[ 3 ];

		return r;
	}
	template< typename T >
	Vector< bool, 4 > operator==( const Quaternion< T > &a, const Quaternion< T > &b )
	{
		Vector< bool, 4 > v;
		
		v[ 0 ] = a[ 0 ] == b[ 0 ];
		v[ 1 ] = a[ 1 ] == b[ 1 ];
		v[ 2 ] = a[ 2 ] == b[ 2 ];
		v[ 3 ] = a[ 3 ] == b[ 3 ];

		return v;
	}
	template< typename T >
	Vector< bool, 4 > operator!=( const Quaternion< T > &a, const Quaternion< T > &b )
	{
		Vector< bool, 4 > v;
		
		v[ 0 ] = a[ 0 ] != b[ 0 ];
		v[ 1 ] = a[ 1 ] != b[ 1 ];
		v[ 2 ] = a[ 2 ] != b[ 2 ];
		v[ 3 ] = a[ 3 ] != b[ 3 ];

		return v;
	}
	
	// Quaternion functions!
	template< typename T >
	Matrix< T, 3, 3 > makeMatrix( const Quaternion< T > &q )
	{
		Matrix< T, 3, 3 > m;
		T x2, y2, z2, xy, xz, xw, yz, yw, zw;
		T zero, one, two;

		zero = static_cast< T >( 0.f );
		one = static_cast< T >( 1.f );
		two = static_cast< T >( 2.f );
		x2 = q[ 0 ] * q[ 0 ];
		y2 = q[ 1 ] * q[ 1 ];
		z2 = q[ 2 ] * q[ 2 ];
		xy = q[ 0 ] * q[ 1 ];
		xz = q[ 0 ] * q[ 2 ];
		xw = q[ 0 ] * q[ 3 ];
		yz = q[ 1 ] * q[ 2 ];
		yw = q[ 1 ] * q[ 3 ];
		zw = q[ 2 ] * q[ 3 ];

		m( 0, 0 ) = one - two * ( y2 + z2 );
		m( 0, 1 ) = two * ( xy + zw );
		m( 0, 2 ) = two * ( xz - yw );

		m( 1, 0 ) = two * ( xy - zw );
		m( 1, 1 ) = one - two * ( x2 + z2 );
		m( 1, 2 ) = two * ( yz - xw );

		m( 2, 0 ) = two * ( xz + yw );
		m( 2, 1 ) = two * ( yz - xw );
		m( 2, 2 ) = one - two * ( x2 + y2 );

		return m;
	}

	
	template< typename T >
	bool equals( const Quaternion< T > &a, const Quaternion< T > &b, const T tolerance )
	{
		T cosine, angle;

		cosine = dot( a, b );
		if( cosine <= -1.f ) {
			angle = ( f32_t )VUL_PI;
		} else {
			if( cosine >= 1.f ) {
				angle = 0.f;
			} else {
				angle = acos( cosine );
			}
		}

		return ( std::abs( angle ) <= tolerance
				 || ( std::abs( angle - static_cast< T >( VUL_PI ) ) <= tolerance ) );
	}
	template< typename T >
	T norm( const Quaternion< T > &q )
	{
		T res = q[ 0 ] * q[ 0 ]
			  + q[ 1 ] * q[ 1 ]
			  + q[ 2 ] * q[ 2 ]
			  + q[ 3 ] * q[ 3 ];
		return static_cast< T >( sqrt( ( f32_t )res ) );
	}
	template< typename T >
	Quaternion< T > normalize( const Quaternion< T > &q )
	{
		T fac;

		fac = static_cast< T >( 1.f ) / norm( q );
		
		return q * fac;		
	}
	template< typename T >
	T dot( const Quaternion< T > &a, const Quaternion< T > &b  )
	{
		return a[ 0 ] * b[ 0 ]
			 + a[ 1 ] * b[ 1 ]
			 + a[ 2 ] * b[ 2 ]
			 + a[ 3 ] * b[ 3 ];
	}
	template< typename T >
	Quaternion< T > inverse( const Quaternion< T > &q )
	{
		Quaternion< T > r;
		T len, invLen;

		len = norm( q );

		// Inverse is only valid for length != 0.
		assert( len > static_cast< T >( 0.f ) );

		invLen = static_cast< T >( 1.f ) / len;
		r[ 0 ] = -q[ 0 ] * invLen;
		r[ 1 ] = -q[ 1 ] * invLen;
		r[ 2 ] = -q[ 2 ] * invLen;
		r[ 3 ] = q[ 3 ] * invLen;
		return r;
	}
	template< typename T >
	Quaternion< T > unitInverse( const Quaternion< T > &q )
	{
		Quaternion< T > r;
		
		r[ 0 ] = -q[ 0 ]; // Invert axis
		r[ 1 ] = -q[ 1 ]; 
		r[ 2 ] = -q[ 2 ];
		r[ 3 ] =  q[ 3 ]; // Keep w

		return r;
	}
	
	template< typename T >
	Quaternion< T > slerp( const Quaternion< T > &a, const Quaternion< T > &b, 
						   T t, bool useShortestPath )
	{
		T cosine, sine, angle, invSine, c0, c1;
		Quaternion< T > q, nb;
		T zero, one;

		zero = static_cast< T >( 0.f );
		one = static_cast< T >( 1.f );
		
		cosine = dot( a, b );
		if( cosine < zero && useShortestPath )
		{
			cosine = -cosine;
			nb = -b;
		} else {
			nb = b;
		}

		if( std::abs( cosine ) < one - static_cast< T >( ( f32_t )VUL_QUATERNION_SLERP_EPSILON ) )
		{
			sine = sqrt( one - sqrt( cosine ) );
			angle = atan2( sine, cosine );
			invSine = one / sine;
			c0 = sin( ( one - t ) * angle ) * invSine;
			c1 = sin( t * angle ) * invSine;
			q = a * c0 + nb * c1;
			return normalize( q );
		} else {
			// Either a and b are very close, and we can lerp, 
			// or a and b are almost inverses and we have infinite directions.
			// We sadly have no way of fixing the second case, so just lerp.
			q = a * ( one - t ) + nb * t;
			return normalize( q );
		}
	}
	template< typename T >
	Quaternion< T > nlerp( const Quaternion< T > &a, const Quaternion< T > &b, 
						   T t, bool useShortestPath  )
	{
		Quaternion< T > q;
		T cosine;

		cosine = dot( a, b );
		if( cosine < static_cast< T >( 0.f ) && useShortestPath )
		{
			q = a + ( ( -b ) - a ) * t;
		} else {
			q = a + ( b - a ) * t;
		}
		return normalize( q );
	}
	template< typename T >
	Quaternion< T > squadp( const Quaternion< T > &a, const Quaternion< T > &b,
							const Quaternion< T > &c, const Quaternion< T > &d,
							T t, bool useShortestPath )
	{
		T st;
		Quaternion< T > qa, qd;

		st = static_cast< T >( 2.f ) * t * ( static_cast< T >( 1.f ) - t );
		qa = slerp( a, d, t, useShortestPath );
		qd = slerp( b, c, t, false );

		return slerp( qa, qd, st, false );
	}

}

#endif