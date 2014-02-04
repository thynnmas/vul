/*
 * Villains' Utility Library - Thomas Martin Schmid, 2014. Public domain¹
 *
 * An AABB struct that works in unlimited dimensions.
 * 
 * ¹ If public domain is not legally valid in your country and or legal area,
 *   the MIT licence applies (see the LICENCE file)
 */

#ifndef VUL_AABB_H
#define VUL_AABB_H

#include "vul_types.hpp"

// Define this for the c++11 version
//#define VUL_CPLUSPLUS11

namespace vul {
	
	//----------------
	// Declarations
	// 

	template< typename T, i32_t n >
	struct Vector;
	template< typename T, i32_t n >
	struct Affine;

	template< typename T, i32_t n >
	struct AABB {
		Vector< T, n > _min;
		Vector< T, n > _max;

#ifdef VUL_CPLUSPLUS11
		constexpr AABB< T, n >( );
		explicit AABB< T, n >( const Vector< T, n > &mini, const Vector< T, n > &maxi );
		explicit AABB< T, n >( T (& a)[ n ] );
		explicit AABB< T, n >( f32_t[ n ] a );
		explicit AABB< T, n >( i32_t[ n ] a );
#endif
		AABB< T, n > &operator=( const AABB< T, n > &rhs );
	};

	template< typename T, i32_t n >
	AABB< T, n > translate( const AABB< T, n > &aabb, const Vector< T, n > &vec );	// Translation
	template< typename T, i32_t n >
	AABB< T, n > scale( const AABB< T, n > &aabb, T scalar );						// Scale
	template< typename T, i32_t n >
	AABB< T, n > transform( const AABB< T, n > &aabb, const Affine< T, n > &a );	// Transform
	
	/** 
	 * Returns the center of an AABB
	 */
	template< typename T, i32_t n >
	Vector< T, n > center( const AABB< T, n > &aabb );
	/** 
	 * Returns half the extent of an AABB,
	 * so corners are described by center( aabb ) +- extent( aabb ).
	 */
	template< typename T, i32_t n >
	Vector< T, n > extent( const AABB< T, n > &aabb );
	/**
	 * Tests if a point is inside an AABB
	 */
	template< typename T, i32_t n >
	bool inside( const AABB< T, n > &aabb, const Point< T, n > &pt );
	/**
	 * Tests if a vector (interpreted as a point) is inside an AABB. Not sensible for vectors, but useful
	 * for speed in systems where points are represented with vectors.
	 */
	template< typename T, i32_t n >
	bool inside( const AABB< T, n > &aabb, const Vector< T, n > &vec );

#ifndef VUL_CPLUSPLUS11
	template< typename T, i32_t n >
	AABB< T, n > makeAABB( );
	template< typename T, i32_t n >
	AABB< T, n > makeAABB( const Vector< T, n > &mini, const Vector< T, n > &maxi );
	template< typename T, i32_t n >
	AABB< T, n > makeAABB( T (& a)[ n * 2 ] );
	template< typename T, i32_t n >
	AABB< T, n > makeAABB( f32_t (& a)[ n * 2 ] );
	template< typename T, i32_t n >
	AABB< T, n > makeAABB( i32_t (& a)[ n * 2 ] );
#endif

	//----------------
	// Definition
	//

#ifdef VUL_CPLUSPLUS11
	template< typename T, i32_t n >
	AABB< T, n >::AABB( )
	{
		_min = Point< T, n >( T( 0.f ) );
		_max = Point< T, n >( T( 0.f ) );
	}
	template< typename T, i32_t n >
	AABB< T, n >::AABB( const Point< T, n > &mini, const Point< T, n > &maxi )
	{
		_min = mini;
		_max = maxi;
	}
	template< typename T, i32_t n >
	AABB< T, n >::AABB( T (& a)[ n * 2 ] )
	{
		_min = Point< T, n >( a );
		_max = Point< T, n >( &a[ n ] );
	}
	template< typename T, i32_t n >
	AABB< T, n >::AABB( f32_t[ n * 2 ] a )
	{
		_min = Point< T, n >( a );
		_max = Point< T, n >( &a[ n ] );
	}
	template< typename T, i32_t n >
	AABB< T, n >::AABB( i32_t[ n * 2 ] a )
	{
		_min = Point< T, n >( a );
		_max = Point< T, n >( &a[ n ] );
	}
#else
	template< typename T, i32_t n >
	AABB< T, n > makeAABB( )
	{
		_min = Point< T, n >( T( 0.f ) );
		_max = Point< T, n >( T( 0.f ) );
	}
	template< typename T, i32_t n >
	AABB< T, n > makeAABB( const Vector< T, n > &mini, const Vector< T, n > &maxi )
	{
		AABB< T, n > aabb;

		aabb._min = mini;
		aabb._max = maxi;

		return aabb;
	}
	template< typename T, i32_t n >
	AABB< T, n > makeAABB( T (& a)[ n * 2 ] )
	{
		AABB< T, n > aabb;

		_min = Point< T, n >( a );
		_max = Point< T, n >( &a[ n ] );

		return aabb;
	}
	template< typename T, i32_t n >
	AABB< T, n > makeAABB( f32_t (& a)[ n * 2 ] )
	{
		AABB< T, n > aabb;

		_min = Point< T, n >( a );
		_max = Point< T, n >( &a[ n ] );

		return aabb;
	}
	template< typename T, i32_t n >
	AABB< T, n > makeAABB( i32_t (& a)[ n * 2 ] )
	{
		AABB< T, n > aabb;

		_min = Point< T, n >( a );
		_max = Point< T, n >( &a[ n ] );

		return aabb;
	}
#endif
	template< typename T, i32_t n >
	AABB< T, n > &AABB< T, n >::operator=( const AABB< T, n > &rhs )
	{
		_min = rhs._min;
		_max = rhs._max;
		return *this;
	}

	template< typename T, i32_t n >
	AABB< T, n > translate( const AABB< T, n > &aabb, const Vector< T, n > &vec )
	{
		AABB< T, n > r;

		r._min += vec;
		r._max += vec;

		return r;
	}
	template< typename T, i32_t n >
	AABB< T, n > scale( const AABB< T, n > &aabb, T scalar )
	{
		AABB< T, n > r;
		Vector< T, n > v;

#ifdef VUL_CPLUSPLUS11
		v = Vector< T, n >( scalar );
#else
		v = makeVector< T, n >( scalar );
#endif

		r._min *= v;
		r._max *= v;

		return r;
	}
	template< typename T, i32_t n >
	AABB< T, n > transform( const AABB< T, n > &aabb, const Affine< T, n > &a )
	{
		AABB< T, n > res;
		Vector< T, n > corner, newmin, newmax;
		unsigned int i, j;

#ifdef VUL_CPLUSPLUS11
		newmin = Vector< T, n >( std::numeric_limits< T >::lowest( ) );
		newmax = Vector< T, n >( std::numeric_limits< T >::max( ) );
#else
		newmin = makeVector< T, n >( std::numeric_limits< T >::lowest( ) );
		newmax = makeVector< T, n >( std::numeric_limits< T >::max( ) );
#endif
		for( i = 0; i < 8; ++i )
		{
			for( j = 0; j < n; ++j )
			{
				corner[ j ] = ( ( j & ( 1 << ( j - 1 ) ) ) != 0 ) ? aabb._max[ j ] : aabb._min[ j ];
			}
			corner = a * corner;
#ifdef VUL_CPLUSPLUS11
			newmin = Vector< T, n >( min( newmin, corner ) );
			newmax = Vector< T, n >( max( newmax, corner ) );
#else
			newmin = makeVector< T, n >( min( newmin, corner ) );
			newmax = makeVector< T, n >( max( newmax, corner ) );
#endif
		}
#ifdef VUL_CPLUSPLUS11
		res = AABB< T, n >( newmin, newmax );
#else
		res = makeAABB< T, n >( newmin, newmax );
#endif

		return res;
	}
	
	template< typename T, i32_t n >
	Vector< T, n > center( const AABB< T, n > &aabb )
	{
		return ( aabb._max + aabb._min ) * static_cast< T >( 0.5f );
	}

	template< typename T, i32_t n >
	Vector< T, n > extent( const AABB< T, n > &aabb )
	{
		return ( aabb._max - aabb._min ) * static_cast< T >( 0.5f );
	}

	template< typename T, i32_t n >
	bool inside( const AABB< T, n > &aabb, const Point< T, n > &pt )
	{
		return all(									// Are all coordinates'
					abs( pt - center( aabb ) )		// distance to the center
					<=								// smaller than or equal to
					abs( extent( aabb ) ) );		// the size of the extent
	}
	template< typename T, i32_t n >
	bool inside( const AABB< T, n > &aabb, const Vector< T, n > &vec )
	{
		return all(									// Are all coordinates'
					abs( vec - center( aabb ) )		// distance to the center
					<=								// smaller than or equal to
					abs( extent( aabb ) ) );		// the size of the extent
	}


	// @TODO: SSE version of transform ( AOSOA makes sense here )
}

#endif