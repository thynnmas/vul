/*
 * Villains' Utility Library - Thomas Martin Schmid, 2014. Public domain¹
 *
 * A vector math library for generic points. These are similar to the vectors
 * in vul_vector.h, but only define operations defined on points. Explicit
 * conversions are given to/from vectors.
 *
 * The points are scalar.
 * 
 * ¹ If public domain is not legally valid in your country and or legal area,
 *   the MIT licence applies (see the LICENCE file)
 */
#ifndef VUL_POINT_HPP
#define VUL_POINT_HPP

#include "vul_types.hpp"

/**
 * Define this for the c++11 version
 */
//#define VUL_CPLUSPLUS11

namespace vul {
	
	template< typename T, i32_t n >
	struct Vector;

	//----------------
	// Declarations
	//
	
	template< typename T, i32_t n >
	struct Point {
		T data[ n ];

#ifdef VUL_CPLUSPLUS11
		// Constructors
		constexpr Point< T, n >( );								// Empty constructor
		explicit Point< T, n >( T val ); 						// Initialize to single value
		explicit Point< T, n >( const Point< T, n > &p ); 		// Copy constructor
		explicit Point< T, n >( T (& a)[ n ] ); 				// Generic array constructor
		explicit Point< T, n >( f32_t (& a)[ n ] ); 				// From float arrays, to interface with other libraries
		explicit Point< T, n >( i32_t (& a)[ n ] ); 				// From int arrays, to interface with other libraries
		explicit Point< T, n >( const Vector< T, n > &vec );	// Explicit conversion from a vector
#endif
		/**
		 * Copy assignment operator.
		 */
		Point< T, n >& operator=( const Point< T, n > & rhs );
	
		/**
		 * Translation of the point by the given vector.
		 */
		Point< T, n >& operator+=( const Vector< T, n > &vec );
		/**
		 * Componentwise multiplication.
		 */
		Point< T, n >& operator*=( const Vector< T, n > &vec );
		/**
		 * Componentwise division.
		 */
		Point< T, n >& operator/=( const Vector< T, n > &vec );
		
		/**
		 * Indexing operator.
		 */
		T &operator[ ]( i32_t i );
		/**
		 * Constant indexing operator.
		 */
		T const &operator[ ]( i32_t i ) const;
	};


#ifndef VUL_CPLUSPLUS11
	// Constructors
	template< typename T, i32_t n >
	Point< T, n > makePoint( );								// Empty constructor
	template< typename T, i32_t n >
	Point< T, n > makePoint( T val ); 						// Initialize to single value
	template< typename T, i32_t n >
	Point< T, n > makePoint( const Point< T, n > &p ); 		// Copy constructor
	template< typename T, i32_t n >
	Point< T, n > makePoint( T (& a)[ n ] ); 						// Generic array constructor
	template< typename T, i32_t n >
	Point< T, n > makePoint( f32_t (& a)[ n ] ); 					// From float arrays, to interface with other libraries
	template< typename T, i32_t n >
	Point< T, n > makePoint( i32_t (& a)[ n ] ); 					// From int arrays, to interface with other libraries
	template< typename T, i32_t n >
	Point< T, n > makePoint( const Vector< T, n > &vec );	// Explicit conversion from a vector
#endif

	// Specializations
	template< typename T >
	Point< T, 2 > makePoint( T x, T y );
	template< typename T >
	Point< T, 3 > makePoint( T x, T y, T z );
	template< typename T >
	Point< T, 4 > makePoint( T x, T y, T z, T w );

	// Operations
	/**
	 * Componentwise coparison. Returns a vector of boolean indicating if
	 * compnents in corresponding positions are equal.
	 */
	template< typename T, i32_t n >
	Vector< bool, n > operator==( const Point< T, n > &a, const Point< T, n > &b );
	/**
	 * Componentwise coparison. Returns a vector of boolean indicating if
	 * compnents in corresponding positions are not equal.
	 */
	template< typename T, i32_t n >
	Vector< bool, n > operator!=( const Point< T, n > &a, const Point< T, n > &b );

	/**
	 * Translate point a by vector b. Returns the resulting point.
	 */
	template< typename T, i32_t n >
	Point< T, n > operator+( const Point< T, n > &a, const Vector< T, n > &b );
	/**
	 * Find the translation between points a and b, and return it as a vector.
	 */
	template< typename T, i32_t n >
	Vector< T, n > operator-( const Point< T, n > &a, const Point< T, n > &b );
	
	/**
	 * Componentwise min( compnent, b )
	 */
	template< typename T, i32_t n >
	Vector< T, n > min( const Point< T, n > &a, T b );
	/**
	 * Componentwise max( compnent, b ).
	 */
	template< typename T, i32_t n >
	Vector< T, n > max( const Point< T, n > &a, T b );
	/**
	 * Componentwise min( compnent_a, compnent_b )
	 */
	template< typename T, i32_t n >
	Vector< T, n > min( const Point< T, n > &a, const Point< T, n > &b );
	/**
	 * Componentwise max( compnent_a, compnent_b )
	 */
	template< typename T, i32_t n >
	Vector< T, n > max( const Point< T, n > &a, const Point< T, n > &b );
	/**
	 * Compnentwise abs
	 */
	template< typename T, i32_t n >
	Vector< T, n > abs( const Point< T, n > &a );
	/**
	 * Componentwise clamp
	 */
	template< typename T, i32_t n >
	Vector< T, n > clamp( const Point< T, n > &a, T mini, T maxi );
	/**
	 * Componentwise saturate, so clamp( x, 0, 1 )
	 */
	template< typename T, i32_t n >
	Vector< T, n > saturate( const Point< T, n > &a );
	/**
	 * Componentwise linear interpolation based on t.
	 */
	template< typename T, i32_t n, typename T_t >
	Vector< T, n > lerp( const Point< T, n > &mini, const Point< T, n > &maxi, T_t t );
	/**
	 * Returns the smallest compnent.
	 */
	template< typename T, i32_t n >
	T minComponent( const Point< T, n > &a );
	/**
	 * Returns the largest compnent.
	 */
	template< typename T, i32_t n >
	T maxComponent( const Point< T, n > &a );
		
	
	//---------------------------
	// Definitions
	//

#ifdef VUL_CPLUSPLUS11
	template< typename T, i32_t n >
	Point< T, n >::Point< T, n >( )
	{
		i32_t i;
		for( i = 0; i < n; ++i ) {
			data[ i ] = static_cast< T >( 0 );
		}
	}
	template< typename T, i32_t n >
	Point< T, n >::Point< T, n >( T val )
	{
		i32_t i;
		for( i = 0; i < n; ++i ) {
			data[ i ] = val;
		}
	}
	template< typename T, i32_t n >
	Point< T, n >::Point< T, n >( const Point< T, n > &p )
	{
		i32_t i;
		for( i = 0; i < n; ++i ) {
			data[ i ] = p[ i ];
		}
	}
	template< typename T, i32_t n >
	Point< T, n >::Point< T, n >( T (& a)[ n ] )
	{
		i32_t i;
		for( i = 0; i < n; ++i ) {
			data[ i ] = a[ i ];
		}
	}
	template< typename T, i32_t n >
	Point< T, n >::Point< T, n >( f32_t (& a)[ n ] )
	{
		i32_t i;
		for( i = 0; i < n; ++i ) {
			data[ i ] = static_cast< T >( a[ i ] );
		}
	}
	template< typename T, i32_t n >
	Point< T, n >::Point< T, n >( i32_t (& a)[ n ] )
	{
		i32_t i;
		for( i = 0; i < n; ++i ) {
			data[ i ] = static_cast< T >( a[ i ] );
		}
	}
	template< typename T, i32_t n >
	Point< T, n >::Point< T, n >( const Vector< T, n > &vec )
	{
		i32_t i;
		for( i = 0; i < n; ++i ) {
			data[ i ] = vec[ i ];
		}
	}
#else
	template< typename T, i32_t n >
	Point< T, n > makePoint( )
	{
		Point< T, n > p;
		i32_t i;

		for( i = 0; i < n; ++i ) {
			p.data[ i ] = static_cast< T >( 0 );
		}

		return p;
	}
	template< typename T, i32_t n >
	Point< T, n > makePoint( T val )
	{
		Point< T, n > p;
		i32_t i;

		for( i = 0; i < n; ++i ) {
			p.data[ i ] = val;
		}

		return p;
	}
	template< typename T, i32_t n >
	Point< T, n > makePoint( const Point< T, n > &p )
	{
		Point< T, n > r;
		i32_t i;

		for( i = 0; i < n; ++i ) {
			r.data[ i ] = p[ i ];
		}

		return r;
	}
	template< typename T, i32_t n >
	Point< T, n > makePoint( T (& a)[ n ] )
	{
		Point< T, n > p;
		i32_t i;

		for( i = 0; i < n; ++i ) {
			p.data[ i ] = a[ i ];
		}

		return p;
	}
	template< typename T, i32_t n >
	Point< T, n > makePoint( f32_t (& a)[ n ] )
	{
		Point< T, n > p;
		i32_t i;

		for( i = 0; i < n; ++i ) {
			p.data[ i ] = static_cast< T >( a[ i ] );
		}

		return p;
	}
	template< typename T, i32_t n >
	Point< T, n > makePoint( i32_t (& a)[ n ] )
	{
		Point< T, n > p;
		i32_t i;

		for( i = 0; i < n; ++i ) {
			p.data[ i ] = static_cast< T >( a[ i ] );
		}

		return p;
	}
	template< typename T, i32_t n >
	Point< T, n > makePoint( const Vector< T, n > &vec )
	{
		Point< T, n > p;
		i32_t i;

		for( i = 0; i < n; ++i ) {
			p.data[ i ] = vec[ i ];
		}

		return p;
	}
#endif
	
	template< typename T >
	Point< T, 2 > makePoint( T x, T y )
	{
		Point< T, 2 > p;

		p[ 0 ] = x;
		p[ 1 ] = y;

		return p;
	}
	template< typename T >
	Point< T, 3 > makePoint( T x, T y, T z )
	{
		Point< T, 3 > p;

		p[ 0 ] = x;
		p[ 1 ] = y;
		p[ 2 ] = z;

		return p;
	}
	template< typename T >
	Point< T, 4 > makePoint( T x, T y, T z, T w )
	{
		Point< T, 4 > p;

		p[ 0 ] = x;
		p[ 1 ] = y;
		p[ 2 ] = z;
		p[ 3 ] = w;

		return p;
	}

	// Operators
	template< typename T, i32_t n >
	Point< T, n > &Point< T, n >::operator=( const Point< T, n > & rhs )
	{
		i32_t i;

		for( i = 0; i < n; ++i ) {
			data[ i ] = rhs[ i ];
		}
		return *this;
	}

	template< typename T, i32_t n >
	Point< T, n >& Point< T, n >::operator+=( const Vector< T, n > &vec )
	{
		i32_t i;

		for( i = 0; i < n; ++i ) {
			data[ i ] += vec[ i ];
		}

		return *this;
	}
	template< typename T, i32_t n >
	Point< T, n >& Point< T, n >::operator*=( const Vector< T, n > &vec )
	{
		i32_t i;

		for( i = 0; i < n; ++i ) {
			data[ i ] *= vec[ i ];
		}

		return *this;
	}
	template< typename T, i32_t n >
	Point< T, n >& Point< T, n >::operator/=( const Vector< T, n > &vec )
	{
		i32_t i;

		for( i = 0; i < n; ++i ) {
			data[ i ] /= vec[ i ];
		}

		return *this;
	}

	template< typename T, i32_t n >
	T &Point< T, n >::operator[ ]( i32_t i )
	{
		assert( i < n );
		return data[ i ];
	}
	template< typename T, i32_t n >
	T const &Point< T, n >::operator[ ]( i32_t i ) const
	{
		assert( i < n );
		return data[ i ];
	}
	
	// Operations
	template< typename T, i32_t n >
	Vector< bool, n > operator==( const Point< T, n > &a, const Point< T, n > &b )
	{
		Vector< bool, n > v;
		i32_t i;

		for( i = 0; i < n; ++i ) {
			v[ i ] = a[ i ] == b[ i ];
		}

		return v;
	}
	template< typename T, i32_t n >
	Vector< bool, n > operator!=( const Point< T, n > &a, const Point< T, n > &b )
	{
		Vector< bool, n > v;
		i32_t i;

		for( i = 0; i < n; ++i ) {
			v[ i ] = a[ i ] != b[ i ];
		}

		return v;
	}

	template< typename T, i32_t n >
	Point< T, n > operator+( const Point< T, n > &a, const Vector< T, n > &b )
	{
		Point< T, n > p;
		i32_t i;

		for( i = 0; i < n; ++i ) {
			p[ i ] = a[ i ] + b[ i ];
		}

		return p;
	}
	template< typename T, i32_t n >
	Vector< T, n > operator-( const Point< T, n > &a, const Point< T, n > &b )
	{
		Vector< T, n > v;
		i32_t i;

		for( i = 0; i < n; ++i ) {
			v[ i ] = b[ i ] - a[ i ];
		}

		return v;
	}
	
	template< typename T, i32_t n >
	Vector< T, n > min( const Point< T, n > &a, T b )
	{
		Vector< T, n > v;
		i32_t i;

		for( i = 0; i < n; ++i ) {
			v[ i ] = std::min( a[ i ], b );
		}

		return v;
	}
	template< typename T, i32_t n >
	Vector< T, n > max( const Point< T, n > &a, T b )
	{
		Vector< T, n > v;
		i32_t i;

		for( i = 0; i < n; ++i ) {
			v[ i ] = std::max( a[ i ], b );
		}

		return v;
	}
	template< typename T, i32_t n >
	Vector< T, n > min( const Point< T, n > &a, const Point< T, n > &b )
	{
		Vector< T, n > v;
		i32_t i;

		for( i = 0; i < n; ++i ) {
			v[ i ] = std::min( a[ i ], b[ i ] );
		}

		return v;
	}
	template< typename T, i32_t n >
	Vector< T, n > max( const Point< T, n > &a, const Point< T, n > &b )
	{
		Vector< T, n > v;
		i32_t i;

		for( i = 0; i < n; ++i ) {
			v[ i ] = std::max( a[ i ], b[ i ] );
		}

		return v;
	}
	template< typename T, i32_t n >
	Vector< T, n > abs( const Point< T, n > &a )
	{
		Vector< T, n > v;
		i32_t i;

		for( i = 0; i < n; ++i ) {
			v[ i ] = std::abs( a[ i ] );
		}

		return v;
	}
	template< typename T, i32_t n >
	Vector< T, n > clamp( const Point< T, n > &a, T mini, T maxi )
	{
		Vector< T, n > v;
		i32_t i;

		for( i = 0; i < n; ++i ) {
			v[ i ] = a[ i ] < mini ? mini
					 : a[ i ] > maxi ? maxi
					 : a[ i ];
		}

		return v;
	}
	template< typename T, i32_t n >
	Vector< T, n > saturate( const Point< T, n > &a )
	{
		Vector< T, n > v;
		i32_t i;
		T t0, t1;

		t0 = static_cast< T >( 0.f );
		t1 = static_cast< T >( 1.f );
		for( i = 0; i < n; ++i ) {
			v[ i ] = a[ i ] < t0 ? t0
					 : a[ i ] > t1 ? t1
					 : a[ i ];
		}

		return v;
	}
	template< typename T, i32_t n, typename T_t >
	Vector< T, n > lerp( const Point< T, n > &mini, const Point< T, n > &maxi, T_t t )
	{
		Vector< T, n > v;
		i32_t i;
		T_t t1;

		t1 = static_cast< T >( 1.f ) - t;

		for( i = 0; i < n; ++i ) {
			v[ i ] = ( mini[ i ] * t ) + ( maxi[ i ] * t1 );
		}

		return v;
	}
	template< typename T, i32_t n >
	T minComponent( const Point< T, n > &a )
	{
		i32_t i;
		T m;

		m = std::numeric_limits< T >::max( );
		for( i = 0; i < n; ++i ) {
			m = a[ i ] < m ? a[ i ] : m;
		}

		return m;
	}
	template< typename T, i32_t n >
	T maxComponent( const Point< T, n > &a )
	{
		i32_t i;
		T m;

		m = std::numeric_limits< T >::min( );
		for( i = 0; i < n; ++i ) {
			m = a[ i ] > m ? a[ i ] : m;
		}

		return m;
	}

}

#endif