/*
 * Villains' Utility Library - Thomas Martin Schmid, 2017. Public domain¹
 *
 * This file describes a quaternion type that interfaces with the linear math
 * classes otherwise defined in this library. While any numeric type is in theory
 * allowed, it only makes sense to use f32 types. s32ermediate calculations
 * and results of norm, dot etc. are returned in the same type as the quaternion's
 * content.
 *
 * The quaternions are scalar.
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
      T data[ 4 ]; 

#ifdef VUL_CPLUSPLUS11
		explicit Quaternion< T >( );
		explicit Quaternion< T >( T x, T y, T z, T w ); 
		explicit Quaternion< T >( Vector< T, 3 > xyz, T w );
		explicit Quaternion< T >( const T (& a)[ 4 ] );
		explicit Quaternion< T >( f32 (& a)[ 4 ] );
		explicit Quaternion< T >( s32 (& a)[ 4 ] );
		explicit Quaternion< T >( std::initializer_list<T> list );
		Quaternion< T >( const Quaternion< T > &rhs );
#endif
      T &x( ) { return data[ 0 ]; }
      const T &x( ) const { return data[ 0 ]; }
      T &y( ) { return data[ 1 ]; }
      const T &y( ) const { return data[ 1 ]; }
      T &z( ) { return data[ 2 ]; }
      const T &z( ) const { return data[ 2 ]; }
      T &w( ) { return data[ 3 ]; }
      const T &w( ) const { return data[ 3 ]; }
		Vector< T, 3 > & xyz( ) { return reinterpret_cast< Vector< T, 3 > & >( data ); }
		const Vector< T, 3 > & xyz( ) const { return reinterpret_cast< const Vector< T, 3 > & >( data ); }
		Vector< T, 4 > & as_vec4( ) { return reinterpret_cast< Vector< T, 4 > & >( data ); }
		const Vector< T, 4 > & as_vec4( ) const { return reinterpret_cast< const Vector< T, 4 > & >( data ); }

		// Operators
		/** 
		 * Assignment.
		 * */
		Quaternion< T > &operator=( const Quaternion< T > &rhs );
		/**
		 * Compnentwise addition.
		 */
		Quaternion< T > &operator+=( const Quaternion< T > &rhs );
		/**
		 * Compnentwise subtraction.
		 */
		Quaternion< T > &operator-=( const Quaternion< T > &rhs );

		/**
		 * Compnentwise mutliplication.
		 */
		Quaternion< T > &operator*=( const T scalar );
		/**
		 * Quaternion multiplication.
		 * @Note: This is not generally commutative.
		 */
		Quaternion< T > &operator*=( const Quaternion< T > &rhs );

		/**
		 * Indexing operator.
		 */
		T &operator[ ]( s32 i );
		/**
		 * Constant indexing operator.
		 */
		T const &operator[ ]( s32 i ) const;
	}; 

	typedef Quaternion< f32 > quat;
	typedef Quaternion< f64 > dquat;
	typedef Quaternion< f16 > hquat;

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
	Quaternion< T > makeQuat( f32 (& a)[ 4 ] );
	
	template< typename T >
	Quaternion< T > makeQuat( s32 (& a)[ 4 ] );
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
   /**
    * Construct a quaternion from a set of euler angles
    */
   template< typename T >
   Quaternion< T > makeQuatFromEuler( const Vector< T, 3 > &angles );
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
	// QTangents
	/**
	* Construct a QTangent from a 3-axis system.
	* Axes must be normalized.
	*/
	template< typename T >
	Quaternion< T > makeQtangent( const Vector< T, 3 > &b, const Vector< T, 3 > &t, const Vector< T, 3 > &n );
	/**
	 * Construct a tangent frame from a Qtangent
	 */
	template< typename T >
	Matrix< T, 3, 3 > makeTangentFrame( const Quaternion< T > &quat );

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
	Matrix< T, 3, 3 > makeMatrix( const Quaternion< T > &q );
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
	/**
	 * Extracts the normalized positive vector of the given dimension from 
	 * a quaternions interpreted as a 3-axis right-hand coordinate system.
	 * This essentially contructs only the relevant column of the matrix
	 * contruscted by makeMatrix( q ) and returns it as a vector,
	 * thus being a faster way to extract the wanted vector from that matrix.
	 */
	template< typename T >
	Vector< T, 3 > extractAxis( const Quaternion< T > &q, u32 dimension );

   /**
    * Extracts the Euler angles from the quaternion
    */
   template< typename T >
   Vector< T, 3 > euler_angles( const Quaternion< T > &q );

	//----------------
	// Definitions
	//

#ifdef VUL_CPLUSPLUS11	
	template< typename T >
	Quaternion< T >::Quaternion( )
	{
		data[ 0 ] = data[ 1 ] = data[ 2 ] = static_cast< T >( 0.f );
		data[ 3 ] = static_cast< T >( 1.f );
	}
	
	template< typename T >
	Quaternion< T >::Quaternion( T x, T y, T z, T w )
	{
		data[ 0 ] = x;
		data[ 1 ] = y;
		data[ 2 ] = z;
		data[ 3 ] = w;
	}
	
	template< typename T >
	Quaternion< T >::Quaternion( Vector< T, 3 > xyz, T w )
	{
		data[ 0 ] = xyz[ 0 ];
		data[ 1 ] = xyz[ 1 ];
		data[ 2 ] = xyz[ 2 ];
		data[ 3 ] = w;
	}
	
	template< typename T >
	Quaternion< T >::Quaternion( const T (& a)[ 4 ] )
	{
		memcpy( data, a, sizeof( T ) * 4 );
	}
	
	template< typename T >
	Quaternion< T >::Quaternion( f32 (& a)[ 4 ] )
	{
		data[ 0 ] = static_cast< T >( a[ 0 ] );
		data[ 1 ] = static_cast< T >( a[ 1 ] );
		data[ 2 ] = static_cast< T >( a[ 2 ] );
		data[ 3 ] = static_cast< T >( a[ 3 ] );
	}
	
	template< typename T >
	Quaternion< T >::Quaternion( s32 (& a)[ 4 ] )
	{
		data[ 0 ] = static_cast< T >( a[ 0 ] );
		data[ 1 ] = static_cast< T >( a[ 1 ] );
		data[ 2 ] = static_cast< T >( a[ 2 ] );
		data[ 3 ] = static_cast< T >( a[ 3 ] );
	}
	template< typename T >
	Quaternion< T >::Quaternion( std::initializer_list< T > list )
	{
		s32 i;
		typename std::initializer_list< T >::iterator it;

		for( it = list.begin( ), i = 0; it != list.end( ) && i < 4; ++it, ++i ) {
			data[ i ] = *it;
		}
	}
	template< typename T >
	Quaternion< T >::Quaternion( const Quaternion< T > &rhs )
	{
		data[ 0 ] = rhs.data[ 0 ];
		data[ 1 ] = rhs.data[ 1 ];
		data[ 2 ] = rhs.data[ 2 ];
		data[ 3 ] = rhs.data[ 3 ];
	}
#else
	template< typename T >
	Quaternion< T > makeQuat( )
	{
		Quaternion< T > q;

		q.data[ 0 ] = q.data[ 1 ] = q.data[ 2 ] = static_cast< T >( 0.f );
		q.data[ 3 ] = static_cast< T >( 1.f );

		return q;
	}
	
	template< typename T >
	Quaternion< T > makeQuat( T x, T y, T z, T w )
	{
		Quaternion< T > q;

		q.data[ 0 ] = x;
		q.data[ 1 ] = y;
		q.data[ 2 ] = z;
		q.data[ 3 ] = w;

		return q;
	}

	template< typename T >
	Quaternion< T > makeQuat( Vector< T, 3 > xyz, T w )
	{
		Quaternion< T > q;
		
		q.data[ 0 ] = xyz[ 0 ];
		q.data[ 1 ] = xyz[ 1 ];
		q.data[ 2 ] = xyz[ 2 ];
		q.data[ 3 ] = w;

		return q;
	}
	
	template< typename T >
	Quaternion< T > makeQuat( const T (& a)[ 4 ] )
	{
		Quaternion< T > q;

		q.data[ 0 ] = a[ 0 ];
		q.data[ 1 ] = a[ 1 ];
		q.data[ 2 ] = a[ 2 ];
		q.data[ 3 ] = a[ 3 ];

		return q;
	}
	
	template< typename T >
	Quaternion< T > makeQuat( f32 (& a)[ 4 ] )
	{
		Quaternion< T > q;
		
		q.data[ 0 ] = static_cast< T >( a[ 0 ] );
		q.data[ 1 ] = static_cast< T >( a[ 1 ] );
		q.data[ 2 ] = static_cast< T >( a[ 2 ] );
		q.data[ 3 ] = static_cast< T >( a[ 3 ] );

		return q;
	}
	
	template< typename T >
	Quaternion< T > makeQuat( s32 (& a)[ 4 ] )
	{
		Quaternion< T > q;

		q.data[ 0 ] = static_cast< T >( a[ 0 ] );
		q.data[ 1 ] = static_cast< T >( a[ 1 ] );
		q.data[ 2 ] = static_cast< T >( a[ 2 ] );
		q.data[ 3 ] = static_cast< T >( a[ 3 ] );

		return q;
	}
#endif
	template< typename T >
	Quaternion< T > makeQuatFromAxisAngle( const Vector< T, 3 > &axis, T angleRadians )
	{
		Quaternion< T > q;
		Vector< T, 3 > a;
		T sina, halfAngle;

		halfAngle = angleRadians / static_cast< T >( 2.f );
		a = normalize( axis );

		sina = sin( halfAngle );
		q.data[ 0 ] = a[ 0 ] * sina;
		q.data[ 1 ] = a[ 1 ] * sina;
		q.data[ 2 ] = a[ 2 ] * sina;
		q.data[ 3 ] = cos( halfAngle );	

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
		T t;
		T zero, one, pf;

		zero = static_cast< T >( 0.f );
		one = static_cast< T >( 1.f );
		pf = static_cast< T >( 0.5f );

		if( mat( 2, 2 ) < zero ) {
			if( mat( 0, 0 ) > mat( 1, 1 ) ) {
				t = one + mat( 0, 0 ) - mat( 1, 1 ) - mat( 2, 2 );
				q.data[ 0 ] = t;
				q.data[ 1 ] = mat( 1, 0 ) + mat( 0, 1 );
				q.data[ 2 ] = mat( 0, 2 ) + mat( 2, 0 );
				q.data[ 3 ] = mat( 2, 1 ) - mat( 1, 2 );
			} else {
				t = one - mat( 0, 0 ) + mat( 1, 1 ) - mat( 2, 2 );
				q.data[ 0 ] = mat( 1, 0 ) + mat( 0, 1 );
				q.data[ 1 ] = t;
				q.data[ 2 ] = mat( 2, 1 ) + mat( 1, 2 );
				q.data[ 3 ] = mat( 0, 2 ) - mat( 2, 0 );
			}
		} else {
			if( mat( 0, 0 ) < -mat( 1, 1 ) ) {
				t = one - mat( 0, 0 ) - mat( 1, 1 ) + mat( 2, 2 );
				q.data[ 0 ] = mat( 0, 2 ) + mat( 2, 0 );
				q.data[ 1 ] = mat( 2, 1 ) + mat( 1, 2 );
				q.data[ 2 ] = t;
				q.data[ 3 ] = mat( 1, 0 ) - mat( 0, 1 );
			} else {
				t = one + mat( 0, 0 ) + mat( 1, 1 ) + mat( 2, 2 );
				q.data[ 0 ] = mat( 2, 1 ) - mat( 1, 2 );
				q.data[ 1 ] = mat( 0, 2 ) - mat( 2, 0 );
				q.data[ 2 ] = mat( 1, 0 ) - mat( 0, 1 );
				q.data[ 3 ] = t;
			}
		}
		q *= pf / sqrt( t );
		return q;
	}
   template< typename T >
   Quaternion< T > makeQuatFromEuler( const Vector< T, 3 > &angles )
   {
      Quaternion< T > q;
      T half;

      half = T( 0.5f );
      T cy = cos( angles[ 0 ] * half );
      T sy = cos( angles[ 0 ] * half );
      T cr = cos( angles[ 1 ] * half );
      T sr = cos( angles[ 1 ] * half );
      T cp = cos( angles[ 2 ] * half );
      T sp = cos( angles[ 2 ] * half );
      
      q[ 0 ] = cy * cr * cp + sy * sr * sp;
      q[ 1 ] = cy * sr * cp - sy * cr * sp;
      q[ 2 ] = cy * cr * sp + sy * sr * cp;
      q[ 3 ] = sy * cr * cp - cy * sr * sp;

      return q;
   }
	// Special cases
	template< typename T >
	Quaternion< T > makeZero( )
	{
		Quaternion< T > q;

		q.data[ 0 ] = q.data[ 1 ] = q.data[ 2 ] = q.data[ 3 ] = static_cast< T >( 0.f );

		return q;
	}
	template< typename T >
	Quaternion< T > makeIdentity( )
	{
		Quaternion< T > q;

		q.data[ 0 ] = q.data[ 1 ] = q.data[ 2 ] = static_cast< T >( 0.f );
		q.data[ 3 ] = static_cast< T >( 1.f );

		return q;
	}
	// QTangents
	template< typename T >
	Quaternion< T > makeQtangent( const Vector< T, 3 > &b, const Vector< T, 3 > &t, const Vector< T, 3 > &n )
	{
		Matrix< T, 3, 3 > m;
		Quaternion< T > q;
		T bias, renorm, scale, qs;
		T zero, one;

		zero = static_cast< T >( 0.f );
		one = static_cast< T >( 1.f );

		m = transpose( makeMatrix33FromColumns( b, t, n ) );

		scale = determinant( m ) < zero ? -one : one;
		m( 0, 2 ) *= scale;
		m( 1, 2 ) *= scale;
		m( 2, 2 ) *= scale;
		
		q = normalize( makeQuatFromMatrix( m ) );

		bias = T( 1e-7 );
		renorm = sqrt( one - bias * bias );

		if( -bias < q.w( ) && q.w( ) < bias ) {
			q.data[ 3 ] = q.data[ 3 ] > zero ? bias : -bias;
			q.data[ 0 ] *= renorm;
			q.data[ 1 ] *= renorm;
			q.data[ 2 ] *= renorm;
		}

		qs = ( scale < zero && q.data[ 3 ] > zero ) ||
			 ( scale > zero && q.data[ 3 ] < zero ) ? -one : one;
		
		q.data[ 3 ] *= qs;
		q.data[ 0 ] *= qs;
		q.data[ 1 ] *= qs;
		q.data[ 2 ] *= qs;

		return q;
	}
	template< typename T >
	Matrix< T, 3, 3 > makeTangentFrame( const Quaternion< T > &q )
	{
		Matrix< T, 3, 3 > m;
		Vector< T, 3 > n;
		T f, one, two;

		one = static_cast< T >( 1.f );
		two = static_cast< T >( 2.f );

		f = ( q.w < T( 0.f ) ? T( -1.f ) : T( 1.f ) );
		
		m( 0, 0 ) = one - two * ( q.y() * q.y + q.z() * q.z );
		m( 0, 1 ) = two * ( q.x() * q.y() + q.z() * q.w() );
		m( 0, 2 ) = two * ( q.x() * q.z() - q.y() * q.w() );

		m( 1, 0 ) = two * ( q.x() * q.y() - q.z() * q.w() );
		m( 1, 1 ) = one - two * ( q.x() * q.x + q.z() * q.z );
		m( 1, 2 ) = two * ( q.y() * q.z() + q.x() * q.w() );

		n = cross( column( m, 0 ), column( m, 1 ) );

		m( 2, 0 ) = n[ 0 ] * f;
		m( 2, 1 ) = n[ 1 ] * f;
		m( 2, 2 ) = n[ 2 ] * f;

		return m;
	}

	// Quaternion operators	
	template< typename T >
	Quaternion< T > &Quaternion< T >::operator=( const Quaternion< T > &rhs )
	{
		data[ 0 ] = rhs.data[ 0 ];
		data[ 1 ] = rhs.data[ 1 ];
		data[ 2 ] = rhs.data[ 2 ];
		data[ 3 ] = rhs.data[ 3 ];
		return *this;
	}
	template< typename T >
	Quaternion< T > &Quaternion< T >::operator+=( const Quaternion< T > &rhs )
	{
		data[ 0 ] += rhs.data[ 0 ];
		data[ 1 ] += rhs.data[ 1 ];
		data[ 2 ] += rhs.data[ 2 ];
		data[ 3 ] += rhs.data[ 3 ];
		return *this;
	}	
	template< typename T >
	Quaternion< T > &Quaternion< T >::operator-=( const Quaternion< T > &rhs )
	{
		data[ 0 ] -= rhs.data[ 0 ];
		data[ 1 ] -= rhs.data[ 1 ];
		data[ 2 ] -= rhs.data[ 2 ];
		data[ 3 ] -= rhs.data[ 3 ];
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

		data[ 0 ] = w * rhs.x( ) + rhs.w( ) * x + y * rhs.z( ) - z * rhs.y( );
		data[ 1 ] = w * rhs.y( ) + rhs.w( ) * y + z * rhs.x( ) - x * rhs.z( );
		data[ 2 ] = w * rhs.z( ) + rhs.w( ) * z + x * rhs.y( ) - y * rhs.x( );

		data[ 3 ] = w * rhs.w( ) - x * rhs.x( ) - y * rhs.y( ) - z * rhs.z( );

		return *this;
	}

	template< typename T >
	T &Quaternion< T >::operator[ ]( s32 i )
	{
		VUL_MATH_ASSERT( i < 4 );
		return data[ i ];
	}
	template< typename T >
	T const &Quaternion< T >::operator[ ]( s32 i ) const
	{
		VUL_MATH_ASSERT( i < 4 );
		return data[ i ];
	}

	template< typename T >
	Quaternion< T > operator+( const Quaternion< T > &a, const Quaternion< T > &b )
	{
		Quaternion< T > q;
		
		q.data[ 0 ] = a.x( ) + b.x( );
		q.data[ 1 ] = a.y( ) + b.y( );
		q.data[ 2 ] = a.z( ) + b.z( );
		q.data[ 3 ] = a.w( ) + b.w( );

		return q;
	}
	template< typename T >
	Quaternion< T > operator-( const Quaternion< T > &a, const Quaternion< T > &b )
	{
		Quaternion< T > q;
		
		q.data[ 0 ] = a.x( ) - b.x( );
		q.data[ 1 ] = a.y( ) - b.y( );
		q.data[ 2 ] = a.z( ) - b.z( );
		q.data[ 3 ] = a.w( ) - b.w( );

		return q;
	}
	template< typename T >
	Quaternion< T > operator*( const Quaternion< T > &q, const T scalar )
		{
		Quaternion< T > r;
		
		r.data[ 0 ] = q.x( ) * scalar;
		r.data[ 1 ] = q.y( ) * scalar;
		r.data[ 2 ] = q.z( ) * scalar;
		r.data[ 3 ] = q.w( ) * scalar;

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
		
		q.data[ 0 ] = a.w( ) * b.x( ) + a.x( ) * b.w( ) + a.y( ) * b.z( ) - a.z( ) * b.y( );
		q.data[ 1 ] = a.w( ) * b.y( ) - a.x( ) * b.z( ) + a.y( ) * b.w( ) + a.z( ) * b.x( );
		q.data[ 2 ] = a.w( ) * b.z( ) + a.x( ) * b.y( ) - a.y( ) * b.x( ) + a.z( ) * b.w( );

		q.data[ 3 ] = a.w( ) * b.w( ) - a.x( ) * b.x( ) - a.y( ) * b.y( ) - a.z( ) * b.z( );

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
		uv = cross( q.xyz( ), v );
		uuv = cross( q.xyz( ), uv );

		uv *= two * q.w( );
		uuv *= two;

		return v + uv + uuv;
	}
	
	template< typename T >
	Quaternion< T > operator-( const Quaternion< T > &q )
	{
		Quaternion< T > r;
		
		// Negate the direction
		r.data[ 0 ] = -q.x( );
		r.data[ 1 ] = -q.y( );
		r.data[ 2 ] = -q.z( );
		// Maintain angle
		r.data[ 3 ] = q.w( );

		return r;
	}
	template< typename T >
	Vector< bool, 4 > operator==( const Quaternion< T > &a, const Quaternion< T > &b )
	{
		Vector< bool, 4 > v;
		
		v.data[ 0 ] = a.x( ) == b.x( );
		v.data[ 1 ] = a.y( ) == b.y( );
		v.data[ 2 ] = a.z( ) == b.z( );
		v.data[ 3 ] = a.w( ) == b.w( );

		return v;
	}
	template< typename T >
	Vector< bool, 4 > operator!=( const Quaternion< T > &a, const Quaternion< T > &b )
	{
		Vector< bool, 4 > v;
		
		v.data[ 0 ] = a.x( ) != b.x( );
		v.data[ 1 ] = a.y( ) != b.y( );
		v.data[ 2 ] = a.z( ) != b.z( );
		v.data[ 3 ] = a.w( ) != b.w( );

		return v;
	}
	
	// Quaternion functions!
	template< typename T >
	Matrix< T, 3, 3 > makeMatrix( const Quaternion< T > &q )
	{
		Matrix< T, 3, 3 > m;
		T x2, y2, z2, xy, xz, xw, yz, yw, zw;
		T one, two;

		one = static_cast< T >( 1.f );
		two = static_cast< T >( 2.f );
		x2 = q.x( ) * q.x( );
		y2 = q.y( ) * q.y( );
		z2 = q.z( ) * q.z( );
		xy = q.x( ) * q.y( );
		xz = q.x( ) * q.z( );
		xw = q.x( ) * q.w( );
		yz = q.y( ) * q.z( );
		yw = q.y( ) * q.w( );
		zw = q.z( ) * q.w( );

		m( 0, 0 ) = one - two * ( y2 + z2 );
		m( 0, 1 ) = two * ( xy + zw );
		m( 0, 2 ) = two * ( xz - yw );

		m( 1, 0 ) = two * ( xy - zw );
		m( 1, 1 ) = one - two * ( x2 + z2 );
		m( 1, 2 ) = two * ( yz + xw );

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
			angle = ( f32 )VUL_PI;
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
		T res = q.x( ) * q.x( )
				+ q.y( ) * q.y( )
				+ q.z( ) * q.z( )
				+ q.w( ) * q.w( );
		return static_cast< T >( sqrt( ( f32 )res ) );
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
		return a.x( ) * b.x( )
			  + a.y( ) * b.y( )
			  + a.z( ) * b.z( )
			  + a.w( ) * b.w( );
	}
	template< typename T >
	Quaternion< T > inverse( const Quaternion< T > &q )
	{
		Quaternion< T > r;
		T len, invLen;

		len = norm( q );

		// Inverse is only valid for length != 0.
		VUL_MATH_ASSERT( len > static_cast< T >( 0.f ) );

		invLen = static_cast< T >( 1.f ) / len;
		r.data[ 0 ] = -q.x( ) * invLen;
		r.data[ 1 ] = -q.y( ) * invLen;
		r.data[ 2 ] = -q.z( ) * invLen;
		r.data[ 3 ] =  q.w( ) * invLen;
		return r;
	}
	template< typename T >
	Quaternion< T > unitInverse( const Quaternion< T > &q )
	{
		Quaternion< T > r;
		
		r.data[ 0 ] = -q.x( ); // Invert axis
		r.data[ 1 ] = -q.y( ); 
		r.data[ 2 ] = -q.z( );
		r.data[ 3 ] =  q.w( ); // Keep w

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

		if( std::abs( cosine ) < one - static_cast< T >( ( f32 )VUL_QUATERNION_SLERP_EPSILON ) )
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

	template< typename T >
	Vector< T, 3 > extractAxis( const Quaternion< T > &q, u32 dimension )
	{
		Vector< T, 3 > ret;
		T x2, y2, z2, xy, xz, xw, yz, yw, zw;	

		VUL_MATH_ASSERT( dimension < 3 );

		T one, two;
		one = static_cast< T >( 1.f );
		two = static_cast< T >( 2.f );
		
		if( dimension == 0 ) {	
			y2 = q.y( ) * q.y( );
			z2 = q.z( ) * q.z( );
			xy = q.x( ) * q.y( );
			xz = q.x( ) * q.z( );
			yw = q.y( ) * q.w( );
			zw = q.z( ) * q.w( );
			ret.data[ 0 ] = one - two * ( y2 + z2 );
			ret.data[ 1 ] = two * ( xy + zw );
			ret.data[ 2 ] = two * ( xz - yw );
		} else if( dimension == 1 ) {		
			x2 = q.x( ) * q.x( );
			z2 = q.z( ) * q.z( );
			xy = q.x( ) * q.y( );
			xw = q.x( ) * q.w( );
			yz = q.y( ) * q.z( );
			zw = q.z( ) * q.w( );	
			ret.data[ 0 ] = two * ( xy - zw );
			ret.data[ 1 ] = one - two * ( x2 + z2 );
			ret.data[ 2 ] = two * ( yz - xw );
		} else if( dimension == 2 ) {
			x2 = q.x( ) * q.x( );
			y2 = q.y( ) * q.y( );
			xz = q.x( ) * q.z( );
			xw = q.x( ) * q.w( );
			yz = q.y( ) * q.z( );
			yw = q.y( ) * q.w( );
			ret.data[ 0 ] = two * ( xz + yw );
			ret.data[ 1 ] = two * ( yz - xw );
			ret.data[ 2 ] = one - two * ( x2 + y2 );
		}

		return normalize( ret );
	}
   
   template< typename T >
   Vector< T, 3 > euler_angles( const Quaternion< T > &q )
   {
      Vector< T, 3 > a;
      T y2, one, two, t0, t1, t2, t3, t4;
      
      two = T( 2.f );
      one = T( 1.f );
      y2 = q[ 1 ] * q[ 1 ];

      t0 = two * ( q[ 3 ] * q[ 0 ] + q[ 1 ] * q[ 2 ] );
      t1 = one - two * ( q[ 0 ] * q[ 0 ] + y2 );
      a[ 2 ] = atan2( t0, t1 );

      t2 = two * ( q[ 3 ] * q[ 1 ] - q[ 2 ] * q[ 0 ] );
      t2 = t2 > one ? one : t2;
      t2 = t2 < -one ? -one : t2;
      a[ 1 ] = asin( t2 );

      t3 = two * ( q[ 3 ] * q[ 2 ] + q[ 0 ] * q[ 1 ] );
      t4 = one - two * ( y2 + q[ 2 ] * q[ 2 ] );
      a[ 0 ] = atan2( t3, t4 );

      return a;
   }
}

// @TODO(thynn): Add forward, left, up functions that construct only the column of the matrix we're interested in, and normalize it!

#endif
