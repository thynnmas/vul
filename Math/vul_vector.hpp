/*
 * Villains' Utility Library - Thomas Martin Schmid, 2014. Public domain¹
 *
 * A vector math library for generic vectors. Format inspired by 
 * http://www.reedbeta.com/blog/2013/12/28/on-vector-math-libraries/
 * Specializations for 2, 3 & 4 vectors of all reasonable float, fixed
 * int and uint types (not 128bit) are made. Interfaces with the
 * matrices of vul_matrix.h
 *
 * The vectors are scalar.
 * 
 * ¹ If public domain is not legally valid in your country and or legal area,
 *   the MIT licence applies (see the LICENCE file)
 */

#ifndef VUL_VECTOR_H
#define VUL_VECTOR_H

#include "vul_types.hpp"
#include <assert.h>

/**
 * If defined, the functions are defined and not just declared. Only do this in _one_ c/cpp file!
 */
//#define VUL_DEFINE

// Define this for the c++11 version
//#define VUL_CPLUSPLUS11

namespace vul {
	
	//----------------
	// Declarations
	//

	template< typename T, i32_t n >
	struct Point;

	// Generic Definitions
	template< typename T, i32_t n >
	struct Vector {
		// Data
		T data[ n ];

#ifdef VUL_CPLUSPLUS11
		// Constructors
		constexpr Vector< T, n >( );							// Empty constructor
		explicit Vector< T, n >( T val ); 						// Initialize to single value
		explicit Vector< T, n >( const Vector< T, n > &vec );	// Copy constructor
		explicit Vector< T, n >( const T (& a)[ n ] ); 					// Generic array constructor
		explicit Vector< T, n >( f32_t (& a)[ n ] ); 				// From float arrays, to interface with other libraries
		explicit Vector< T, n >( i32_t (& a)[ n ] ); 				// From int arrays, to interface with other libraries
		explicit Vector< T, n >( const Point< T, n > &p );		// Explicit conversion from a point
#endif
		// Operators
		Vector< T, n >& operator=( const Vector< T, n > & rhs );

		Vector< T, n >& operator+=( T scalar );
		Vector< T, n >& operator-=( T scalar );
		Vector< T, n >& operator*=( T scalar );
		Vector< T, n >& operator/=( T scalar );

		Vector< T, n >& operator+=( Vector< T, n > vec ); // Componentwise addition
		Vector< T, n >& operator-=( Vector< T, n > vec ); // Componentwise subtraction
		Vector< T, n >& operator*=( Vector< T, n > vec ); // Componentwise multiplication
		Vector< T, n >& operator/=( Vector< T, n > vec ); // Componentwise division
		
		T &operator[ ]( i32_t i ); // Index selector
		T const &operator[ ]( i32_t i ) const; // Index selector, const
	};
	
	/*
	// Specializations for 2, 3, 4
	template < typename T >
	struct Vector< T, 2 > { 
		union { 
			T data[ 2 ]; 
			struct { T x, y; };
		};

#ifdef VUL_CPLUSPLUS11
		Vector< T, 2 >( T x, T y ); 
#endif
	};
	template < typename T >
	struct Vector< T, 3 > { 
		union { 
			T data[ 3 ]; 
			struct { T x, y, z; };
			struct { T r, g, b; };
#ifdef VUL_CPLUSPLUS11
			Vector< T, 2 > xy;
#endif
		};
#ifdef VUL_CPLUSPLUS11
		Vector< T, 3 >( T x, T y, T z ); 
		Vector< T, 3 >( Vector< T, 2 > xy, T z );
#else
		Vector< T, 2 > & xy( ) { return reinterpret_cast< Vector< T, 2 > & >( data ); }
		const Vector< T, 2 > & xy( ) const { return reinterpret_cast< const Vector< T, 2 > & >( data ); }
#endif
	};
	template < typename T >
	struct Vector< T, 4 > { 
		union { 
			T data[ 4 ]; 
			struct { T x, y, z, w; };
			struct { T r, g, b, a; };
#ifdef VUL_CPLUSPLUS11
			Vector< T, 2 > xy;
			Vector< T, 3 > xyz;
			Vector< T, 3 > rgb;
#endif
		};
#ifdef VUL_CPLUSPLUS11
		Vector< T, 4 >( T x, T y, T z, T w ); 
		Vector< T, 4 >( Vector< T, 2 > xy, Vector< T, 2 > zw ); 
		Vector< T, 4 >( Vector< T, 3 > xyz, T w ); 
#else
		Vector< T, 2 > & xy( ) { return reinterpret_cast< Vector< T, 2 > & >( data ); }
		const Vector< T, 2 > & xy( ) const { return reinterpret_cast< const Vector< T, 2 > & >( data ); }
		Vector< T, 3 > & xyz( ) { return reinterpret_cast< Vector< T, 3 > & >( data ); }
		const Vector< T, 3 > & xyz( ) const { return reinterpret_cast< const Vector< T, 3 > & >( data ); }
		Vector< T, 3 > & rgb( ) { return reinterpret_cast< Vector< T, 3 > & >( data ); }
		const Vector< T, 3 > & rgb( ) const { return reinterpret_cast< const Vector< T, 3 > & >( data ); }
#endif
	};*/

	// Typedefs for these specializations
	typedef Vector< f16_t, 2 > f16_2t;
	typedef Vector< f16_t, 3 > f16_3t;
	typedef Vector< f16_t, 4 > f16_4t;
	typedef Vector< f32_t, 2 > f32_2t;
	typedef Vector< f32_t, 3 > f32_3t;
	typedef Vector< f32_t, 4 > f32_4t;
	typedef Vector< f64_t, 2 > f64_2t;
	typedef Vector< f64_t, 3 > f64_3t;
	typedef Vector< f64_t, 4 > f64_4t;

	typedef Vector< fi32_t, 2 > fi32_2t;
	typedef Vector< fi32_t, 3 > fi32_3t;
	typedef Vector< fi32_t, 4 > fi32_4t;

	typedef Vector< i8_t, 2 > i8_2t;
	typedef Vector< i8_t, 3 > i8_3t;
	typedef Vector< i8_t, 4 > i8_4t;
	typedef Vector< i16_t, 2 > i16_2t;
	typedef Vector< i16_t, 3 > i16_3t;
	typedef Vector< i16_t, 4 > i16_4t;
	typedef Vector< i32_t, 2 > i32_2t;
	typedef Vector< i32_t, 3 > i32_3t;
	typedef Vector< i32_t, 4 > i32_4t;
	typedef Vector< i64_t, 2 > i64_2t;
	typedef Vector< i64_t, 3 > i64_3t;
	typedef Vector< i64_t, 4 > i64_4t;

	typedef Vector< ui8_t, 2 > ui8_2t;
	typedef Vector< ui8_t, 3 > ui8_3t;
	typedef Vector< ui8_t, 4 > ui8_4t;
	typedef Vector< ui16_t, 2 > ui16_2t;
	typedef Vector< ui16_t, 3 > ui16_3t;
	typedef Vector< ui16_t, 4 > ui16_4t;
	typedef Vector< ui32_t, 2 > ui32_2t;
	typedef Vector< ui32_t, 3 > ui32_3t;
	typedef Vector< ui32_t, 4 > ui32_4t;
	typedef Vector< ui64_t, 2 > ui64_2t;
	typedef Vector< ui64_t, 3 > ui64_3t;
	typedef Vector< ui64_t, 4 > ui64_4t;

	// Member functions if not C++11, since we otherwise couldn't have vectors in structs
#ifndef VUL_CPLUSPLUS11
	template< typename T, i32_t n >
	Vector< T, n > makeVector( T val );
	
	template< typename T, i32_t n >
	Vector< T, n > makeVector( const Vector< T, n > &vec );

	template< typename T, i32_t n >
	Vector< T, n > makeVector( );
	
	template< typename T, i32_t n >
	Vector< T, n > makeVector( T (& a)[ n ] );

	template< typename T, i32_t n >
	Vector< T, n > makeVector( const Point< T, n > &p );

	// Specialization variations
	template< typename T >
	Vector< T, 2 > makeVector( T x, T y );
	template< typename T >
	Vector< T, 3 > makeVector( T x, T y, T z );
	template< typename T >
	Vector< T, 3 > makeVector( Vector< T, 2 > xy, T z );
	template< typename T >
	Vector< T, 4 > makeVector( T x, T y, T z, T w );
	template< typename T >
	Vector< T, 4 > makeVector( Vector< T, 2 > xy, Vector< T, 2 > zw );
	template< typename T >
	Vector< T, 4 > makeVector( Vector< T, 3 > xyz, T w );

	// Anything from float and int arrays; to interface with other libraries
	template< typename T, i32_t n >
	Vector< T, n > makeVector( f32_t (& a)[ n ] );

	template< typename T, i32_t n >
	Vector< T, n > makeVector( i32_t (& a)[ n ] );
#endif
	/**
	 * Truncates a vector of size m > n into a vector of size n.
	 * Copies the n first elements, discards all others.
	 */
	template< typename T, i32_t n, i32_t m >
	Vector< T, n > truncate( const Vector< T, m > &vec );

	// Operations
	template< typename T, i32_t n >	// Componentwise comparisons
	Vector< bool, n > operator==( const Vector< T, n > &a, const Vector< T, n > &b );
	template< typename T, i32_t n >
	Vector< bool, n > operator!=( const Vector< T, n > &a, const Vector< T, n > &b );
	template< typename T, i32_t n >
	Vector< bool, n > operator<( const Vector< T, n > &a, const Vector< T, n > &b );
	template< typename T, i32_t n >
	Vector< bool, n > operator>( const Vector< T, n > &a, const Vector< T, n > &b );
	template< typename T, i32_t n >
	Vector< bool, n > operator<=( const Vector< T, n > &a, const Vector< T, n > &b );
	template< typename T, i32_t n >
	Vector< bool, n > operator>=( const Vector< T, n > &a, const Vector< T, n > &b );

	template< typename T, i32_t n >
	bool all( const Vector< T, n > &vec ); // ANDs together all elements and returns the result
	template< typename T, i32_t n >
	bool any( const Vector< T, n > &vec ); // ORs together all elements and returns the result
	template< typename T, i32_t n >
	T select( const Vector< T, n > &vec ); // Componentwise ?: selector { vec[0] ? vec[0] : ( vec[1] ? vec[1] : ... ) } Returns 0 if all false

	template< typename T, i32_t n >
	Vector< T, n > operator+( const Vector< T, n > &vec, const T scalar );
	template< typename T, i32_t n >
	Vector< T, n > operator-( const Vector< T, n > &vec, const T scalar );
	template< typename T, i32_t n >
	Vector< T, n > operator*( const Vector< T, n > &vec, const T scalar );
	template< typename T, i32_t n >
	Vector< T, n > operator/( const Vector< T, n > &vec, const T scalar );

	template< typename T, i32_t n >
	Vector< T, n > operator+( const Vector< T, n > &a, const Vector< T, n > &b ); // Componentwise addition
	template< typename T, i32_t n >
	Vector< T, n > operator-( const Vector< T, n > &a, const Vector< T, n > &b ); // Componentwise subtraction
	template< typename T, i32_t n >
	Vector< T, n > operator*( const Vector< T, n > &a, const Vector< T, n > &b ); // Componentwise multiplication
	template< typename T, i32_t n >
	Vector< T, n > operator/( const Vector< T, n > &a, const Vector< T, n > &b ); // Componentwise division
	
	template< typename T, i32_t n >
	Vector< T, n > operator-( const Vector< T, n > &vec );

	template< typename T, i32_t n >
	Vector< T, 3 > cross( const Vector< T, n > &a, const Vector< T, n > &b ); // Cross product only defined in 3D (@TODO: include 2D as well?)
	template< typename T, i32_t n >
	T dot( const Vector< T, n > &a, const Vector< T, n > &b ); 

	template< typename T, i32_t n >
	T norm( const Vector< T, n > &a );								// @TODO: Make a versions of this that return float/fixed even if T is int?
	template< typename T, i32_t n >
	Vector< T, n > normalize( const Vector< T, n > &a );			// NOT in place
	template< typename T, i32_t n >
	Vector< T, n > min( const Vector< T, n > &a, T b );				// Componentwise min( x, b )
	template< typename T, i32_t n >
	Vector< T, n > max( const Vector< T, n > &a, T b );				// Componentwise max( x, b )
	template< typename T, i32_t n >
	Vector< T, n > min( const Vector< T, n > &a, const Vector< T, n > &b );	// Componentwise min( xa, xb )
	template< typename T, i32_t n >
	Vector< T, n > max( const Vector< T, n > &a, const Vector< T, n > &b );	// Componentwise max( xa, xb )
	template< typename T, i32_t n >
	Vector< T, n > abs( const Vector< T, n > &a );					// Componentwise abs
	template< typename T, i32_t n >
	Vector< T, n > clamp( const Vector< T, n > &a, T min, T max );	// Componentwise clamp( x, min, max )	
	template< typename T, i32_t n >
	Vector< T, n > saturate( const Vector< T, n > &a );				// Componentwise saturate, so clamp( x, 0, 1 )
	template< typename T, i32_t n, typename T_t >
	Vector< T, n > lerp( const Vector< T, n > &min, const Vector< T, n > &max, T_t t );	// Componentwise lerp based on t
	template< typename T, i32_t n >
	T minComponent( const Vector< T, n > &a );			// Returns the smalles component
	template< typename T, i32_t n >
	T maxComponent( const Vector< T, n > &a );			// Returns the largest component

	// Functions. This is essentially operators on arrays of vectors.
	// @TODO: Vector functions

	// SIMD functions (AOSOA version of things)
	// @TODO: SIMD vector functions

	

	//---------------------------
	// Definitions
	//
		
#ifdef VUL_CPLUSPLUS11
	template< typename T, i32_t n >
	Vector< T, n >::Vector< T, n >( )
	{
		i32_t i;
		for( i = 0; i < n; ++i ) {
			data[ i ] = static_cast< T >( 0 );
		}
	}
	
	template< typename T, i32_t n >
	Vector< T, n >( T val )
	{
		i32_t i;
		for( i = 0; i < n; ++i ) {
			data[ i ] = val;
		}
	}

	template< typename T, i32_t n >
	Vector< T, n >::Vector< T, n >( const Vector< T, n > &vec )
	{
		i32_t i;
		for( i = 0; i < n; ++i ) {
			data[ i ] = vec[ i ];
		}
	}

	template< typename T, i32_t n >
	Vector< T, n >::Vector< T, n >( const T (& a)[ n ] )
	{
		memcpy( data, a, sizeof( T ) * n );
	}
	
	template< typename T, i32_t n >
	Vector< T, n >::Vector< T, n >( f32_t (& a)[ n ] )
	{
		i32_t i;
		for( i = 0; i < n; ++i ) {
			data[ i ] = static_cast< T >( a[ i ] );
		}		
	}
	
	template< typename T, i32_t n >
	Vector< T, n >::Vector< T, n >( i32_t (& a)[ n ] )
	{
		i32_t i;
		for( i = 0; i < n; ++i ) {
			data[ i ] = static_cast< T >( a[ i ] );
		}		
	}
	
	template< typename T, i32_t n >
	Vector< T, n >::Vector< T, n >( const Point< T, n > &p )
	{
		i32_t i;
		for( i = 0; i < n; ++i ) {
			data[ i ] = p[ i ];
		}		
	}

	// Specialization variations
	template < typename T >
	Vector< T, n >::Vector< T, 2 >( T x, T y ) 
	{
		data[ 0 ] = x; 
		data[ 1 ] = y;
	}
	template < typename T >
	Vector< T, n >::Vector< T, 3 >( T x, T y, T z ) 
	{
		data[ 0 ] = x; 
		data[ 1 ] = y; 
		data[ 2 ] = z; 
	}
	template < typename T >
	Vector< T, n >::Vector< T, 3 >( Vector< T, 2 > xy, T z ) 
	{ 
		data[ 0 ] = xy[ 0 ];
		data[ 1 ] = xy[ 1 ];
		data[ 2 ] = z; 
	}
	template < typename T >
	Vector< T, n >::Vector< T, 4 >( T x, T y, T z, T w )
	{ 
		data[ 0 ] = x;
		data[ 1 ] = y;
		data[ 2 ] = z; 
		data[ 3 ] = w; 
	}
	template < typename T >
	Vector< T, n >::Vector< T, 4 >( Vector< T, 2 > xy, Vector< T, 2 > zw )
	{
		data[ 0 ] = xy[ 0 ]; 
		data[ 1 ] = xy[ 1 ]; 
		data[ 2 ] = zw[ 0 ]; 
		data[ 3 ] = zw[ 1 ];
	}
	template < typename T >
	Vector< T, n >::Vector< T, 4 >( Vector< T, 3 > xyz, T w )
	{
		data[ 0 ] = xyz[ 0 ]; 
		data[ 1 ] = xyz[ 1 ];
		data[ 2 ] = xyz[ 2 ];
		data[ 3 ] = w;
	}
	
#else
	template < typename T, i32_t n >
	Vector< T, n > makeVector( T val )
	{
		Vector< T, n > v;
		i32_t i;

		for( i = 0; i < n; ++i ) {
			v[ i ] = val;
		}

		return v;
	}
	
	template < typename T, i32_t n >
	Vector< T, n > makeVector( const Vector< T, n > &vec )
	{
		Vector< T, n > v;
		i32_t i;

		for( i = 0; i < n; ++i ) {
			v[ i ] = vec[ i ];
		}

		return v;
	}

	template < typename T, i32_t n >
	Vector< T, n > makeVector( )
	{
		Vector< T, n > v;
		i32_t i;

		for( i = 0; i < n; ++i ) {
			v[ i ] = T( 0.0f );
		}

		return v;
	}
	
	template < typename T, i32_t n >
	Vector< T, n > makeVector( T (& a)[ n ] )
	{
		Vector< T, n > v;
		i32_t i;

		for( i = 0; i < n; ++i ) {
			v[ i ] = a[ i ];
		}
		return v;
	}

	template < typename T, i32_t n >
	Vector< T, n > makeVector( const Point< T, n > &p )
	{
		Vector< T, n > v;
		i32_t i;

		for( i = 0; i < n; ++i ) {
			v[ i ] = p[ i ];
		}
		return v;
	}

	// Specialization variations
	template< typename T >
	Vector< T, 2 > makeVector( T x, T y )
	{ 
		Vector< T, 2 > v;
		v[ 0 ] = x;
		v[ 1 ] = y;
		return v;
	}
	template< typename T >
	Vector< T, 3 > makeVector( T x, T y, T z )
	{ 
		Vector< T, 3 > v;
		v[ 0 ] = x; 
		v[ 1 ] = y; 
		v[ 2 ] = z;
		return v; 
	}
	template< typename T >
	Vector< T, 3 > makeVector( Vector< T, 2 > xy, T z ) 	
	{ 
		Vector< T, 3 > v;
		v[ 0 ] = xy[ 0 ];
		v[ 1 ] = xy[ 1 ];
		v[ 2 ] = z;
		return v; 
	}
	template< typename T >
	Vector< T, 4 > makeVector( T x, T y, T z, T w ) 
	{
		Vector< T, 4 > v; 
		v[ 0 ] = x;
		v[ 1 ] = y;
		v[ 2 ] = z;
		v[ 3 ] = w;
		return v;
	}
	template< typename T >
	Vector< T, 4 > makeVector( Vector< T, 2 > xy, Vector< T, 2 > zw ) 
	{
		Vector< T, 4 > v ; 
		v[ 0 ] = xy[ 0 ]; 
		v[ 1 ] = xy[ 1 ]; 
		v[ 2 ] = zw[ 0 ];
		v[ 3 ] = zw[ 1 ];
		return v; 
	}
	template< typename T >
	Vector< T, 4 > makeVector( Vector< T, 3 > xyz, T w ) 
	{	
		Vector< T, 4 > v;
		v[ 0 ] = xyz[ 0 ];
		v[ 1 ] = xyz[ 1 ]; 
		v[ 2 ] = xyz[ 2 ]; 
		v[ 3 ] = w;
		return v; 
	}
	
	// Anything from float and int arrays; to interface with other libraries
	template< typename T, i32_t n >
	Vector< T, n > makeVector( f32_t (& a)[ n ] )
	{
		Vector< T, n > v;
		i32_t i;

		for( i = 0; i < n; ++i ) {
			v[ i ] = static_cast< T >( a[ i ] );
		}
		return v;
	}

	template< typename T, i32_t n >
	Vector< T, n > makeVector( i32_t (& a)[ n ] )
	{		
		Vector< T, n > v;
		i32_t i;

		for( i = 0; i < n; ++i ) {
			v[ i ] = static_cast< T >( a[ i ] );
		}
		return v;
	}
#endif
	template< typename T, i32_t n, i32_t m >
	Vector< T, n > truncate( const Vector< T, m > &vec )
	{
		Vector< T, n > r;
		i32_t i;

		assert( m > n );
		for( i = 0; i < n; ++i ) {
			r[ i ] = vec[ i ];
		}

		return r;
	}

	template< typename T, i32_t n >
	Vector< T, n > &Vector< T, n >::operator=( const Vector< T, n > & rhs )
	{
		i32_t i;

		for( i = 0; i < n; ++i ) {
			data[ i ] = rhs[ i ];
		}
		
		return *this;
	}

	template< typename T, i32_t n >
	Vector< T, n >& Vector< T, n >::operator+=( T scalar )
	{
		i32_t i;
		
		for( i = 0; i < n; ++i ) {
			data[ i ] += scalar;
		}

		return *this;
	}
	
	template< typename T, i32_t n >
	Vector< T, n >& Vector< T, n >::operator-=( T scalar )
	{
		i32_t i;
		
		for( i = 0; i < n; ++i ) {
			data[ i ] -= scalar;
		}

		return *this;
	}
	
	template< typename T, i32_t n >
	Vector< T, n >& Vector< T, n >::operator*=( T scalar )
	{
		i32_t i;
		
		for( i = 0; i < n; ++i ) {
			data[ i ] *= scalar;
		}

		return *this;	
	}
	
	template< typename T, i32_t n >
	Vector< T, n >& Vector< T, n >::operator/=( T scalar )
	{
		i32_t i;
		
		for( i = 0; i < n; ++i ) {
			data[ i ] /= scalar;
		}

		return *this;
	}
	
	template< typename T, i32_t n >
	Vector< T, n >& Vector< T, n >::operator+=( Vector< T, n > vec )
	{
		i32_t i;

		for( i = 0; i < n; ++i ) {
			data[ i ] += vec[ i ];
		}		

		return *this;
	}

	template< typename T, i32_t n >
	Vector< T, n >& Vector< T, n >::operator-=( Vector< T, n > vec )
	{
		i32_t i;

		for( i = 0; i < n; ++i ) {
			data[ i ] -= vec[ i ];
		}		

		return *this;
	}

	template< typename T, i32_t n >
	Vector< T, n >& Vector< T, n >::operator*=( Vector< T, n > vec )
	{
		i32_t i;

		for( i = 0; i < n; ++i ) {
			data[ i ] *= vec[ i ];
		}		

		return *this;	
	}

	template< typename T, i32_t n >
	Vector< T, n >& Vector< T, n >::operator/=( Vector< T, n > vec )
	{
		i32_t i;

		for( i = 0; i < n; ++i ) {
			data[ i ] /= vec[ i ];
		}		

		return *this;
	}

	template< typename T, i32_t n >
	T& Vector< T, n >::operator[ ]( i32_t i )
	{
		assert( i < n );
		return data[ i ];
	}
	
	template< typename T, i32_t n >
	T const &Vector< T, n >::operator[ ]( i32_t i ) const
	{
		assert( i < n );
		return data[ i ];
	}
	

	template< typename T, i32_t n >
	Vector< bool, n > operator==( const Vector< T, n > &a, const Vector< T, n > &b )
	{
		Vector< bool, n > v;
		i32_t i;


		for( i = 0; i < n; ++i ) {
			v[ i ] = a[ i ] == b[ i ];
		}

		return v;
	}
	template< typename T, i32_t n >
	Vector< bool, n > operator!=( const Vector< T, n > &a, const Vector< T, n > &b )
	{
		Vector< bool, n > v;
		i32_t i;

		for( i = 0; i < n; ++i ) {
			v[ i ] = a[ i ] != b[ i ];
		}

		return v;
	}
	template< typename T, i32_t n >
	Vector< bool, n > operator<( const Vector< T, n > &a, const Vector< T, n > &b )
	{
		Vector< bool, n > v;
		i32_t i;

		for( i = 0; i < n; ++i ) {
			v[ i ] = a[ i ] < b[ i ];
		}

		return v;
	}
	template< typename T, i32_t n >
	Vector< bool, n > operator>( const Vector< T, n > &a, const Vector< T, n > &b )
	{
		Vector< bool, n > v;
		i32_t i;

		for( i = 0; i < n; ++i ) {
			v[ i ] = a[ i ] > b[ i ];
		}

		return v;
	}
	template< typename T, i32_t n >
	Vector< bool, n > operator<=( const Vector< T, n > &a, const Vector< T, n > &b )
	{
		Vector< bool, n > v;
		i32_t i;

		for( i = 0; i < n; ++i ) {
			v[ i ] = a[ i ] <= b[ i ];
		}

		return v;
	}
	template< typename T, i32_t n >
	Vector< bool, n > operator>=( const Vector< T, n > &a, const Vector< T, n > &b )
	{
		Vector< bool, n > v;
		i32_t i;

		for( i = 0; i < n; ++i ) {
			v[ i ] = a[ i ] >= b[ i ];
		}

		return v;
	}

	template< typename T, i32_t n >
	bool all( const Vector< T, n > &vec )
	{
		i32_t i;
		for( i = 0; i < n; ++i ) {
			if( !vec[ i ] ) {
				return false;
			}
		}
		return true;
	}

	template< typename T, i32_t n >
	bool any( const Vector< T, n > &vec )
	{
		i32_t i;
		for( i = 0; i < n; ++i ) {
			if( vec[ i ] ) {
				return true;
			}
		}
		return false;
	}

	template< typename T, i32_t n >
	T select( const Vector< T, n > &vec )
	{
		i32_t i;
		for( i = 0; i < n; ++i ) {
			if( vec[ i ] ) {
				return vec[ i ];
			}
		}
		return static_cast< T >( 0 );
	}

	template< typename T, i32_t n >
	Vector< T, n > operator+( const Vector< T, n > &vec, const T scalar )
	{
		
		Vector< T, n > v;
		i32_t i;

		for( i = 0; i < n; ++i ) {
			v[ i ] = vec[ i ] + scalar;
		}

		return v;
	}

	template< typename T, i32_t n >
	Vector< T, n > operator-( const Vector< T, n > &vec, const T scalar )
	{
		Vector< T, n > v;
		i32_t i;

		for( i = 0; i < n; ++i ) {
			v[ i ] = vec[ i ] - scalar;
		}

		return v;
	}
	template< typename T, i32_t n >
	Vector< T, n > operator*( const Vector< T, n > &vec, const T scalar )
	{
		
		Vector< T, n > v;
		i32_t i;

		for( i = 0; i < n; ++i ) {
			v[ i ] = vec[ i ] * scalar;
		}

		return v;
	}

	template< typename T, i32_t n >
	Vector< T, n > operator/( const Vector< T, n > &vec, const T scalar )

	{
		Vector< T, n > v;
		i32_t i;

		for( i = 0; i < n; ++i ) {
			v[ i ] = vec[ i ] / scalar;
		}

		return v;
	}

	template< typename T, i32_t n >
	Vector< T, n > operator+( const Vector< T, n > &a, const Vector< T, n > &b )
	{
		Vector< T, n > v;
		i32_t i;

		for( i = 0; i < n; ++i ) {
			v[ i ] = a[ i ] + b[ i ];
		}

		return v;
	}

	template< typename T, i32_t n >
	Vector< T, n > operator-( const Vector< T, n > &a, const Vector< T, n > &b )
	{
		Vector< T, n > v;
		i32_t i;

		for( i = 0; i < n; ++i ) {
			v[ i ] = a[ i ] - b[ i ];
		}

		return v;
	}

	template< typename T, i32_t n >
	Vector< T, n > operator*( const Vector< T, n > &a, const Vector< T, n > &b )
	{
		Vector< T, n > v;
		i32_t i;

		for( i = 0; i < n; ++i ) {
			v[ i ] = a[ i ] * b[ i ];
		}

		return v;
	}

	template< typename T, i32_t n >
	Vector< T, n > operator/( const Vector< T, n > &a, const Vector< T, n > &b )
	{
		Vector< T, n > v;
		i32_t i;

		for( i = 0; i < n; ++i ) {
			v[ i ] = a[ i ] / b[ i ];
		}

		return v;
	}

	template< typename T, i32_t n >
	Vector< T, n > operator-( const Vector< T, n > &vec )
	{
		Vector< T, n > v;
		i32_t i;

		for( i = 0; i < n; ++i ) {
			v[ i ] = -vec[ i ];
		}

		return v;
	}

	template< typename T, i32_t n >
	Vector< T, 3 > cross( const Vector< T, n > &a, const Vector< T, n > &b )
	{
		Vector< T, n > v;
		
		v[ 0 ] = ( a[ 1 ] * b[ 2 ] ) - ( b[ 1 ] * a[ 2 ] );
		v[ 1 ] = ( a[ 2 ] * b[ 0 ] ) - ( b[ 2 ] * a[ 0 ] );
		v[ 2 ] = ( a[ 0 ] * b[ 1 ] ) - ( b[ 0 ] * a[ 1 ] );

		return v;
	}

	template< typename T, i32_t n >
	T dot( const Vector< T, n > &a, const Vector< T, n > &b )
	{
		i32_t i;
		T res;

		res = a[ 0 ] * b[ 0 ];
		for( i = 1; i < n; ++i ){ 
			res += a[ i ] * b[ i ];
		}

		return res;
	}

	template< typename T, i32_t n >
	T norm( const Vector< T, n > &a )
	{
		T res;
		i32_t i;

		res = static_cast< T >( 0 );
		for( i = 0; i < n; ++i ) {
			res += a[ i ] * a[ i ];
		}

		return sqrt( res );
	}

	template< typename T, i32_t n >
	Vector< T, n > normalize( const Vector< T, n > &a )
	{
		Vector< T, n > v;
		i32_t i;
		T invnorm;

		invnorm = static_cast< T >( 1.0 ) / norm( a );
		for( i = 0; i < n; ++i ) {
			v[ i ] = a[ i ] * invnorm;
		}

		return v;
	}

	template< typename T, i32_t n >
	Vector< T, n > min( const Vector< T, n > &a, T b )
	{
		Vector< T, n > v;
		i32_t i;

		for( i = 0; i < n; ++i ) {
			v[ i ] = std::min( a[ i ], b );
		}

		return v;
	}
	template< typename T, i32_t n >
	Vector< T, n > max( const Vector< T, n > &a, T b )
	{
		Vector< T, n > v;
		i32_t i;

		for( i = 0; i < n; ++i ) {
			v[ i ] = std::max( a[ i ], b );
		}

		return v;
	}
	template< typename T, i32_t n >
	Vector< T, n > min( const Vector< T, n > &a, const Vector< T, n > &b )
	{
		Vector< T, n > v;
		i32_t i;

		for( i = 0; i < n; ++i ) {
			v[ i ] = std::max( a[ i ], b[ i ] );
		}

		return v;
	}
	template< typename T, i32_t n >
	Vector< T, n > max( const Vector< T, n > &a, const Vector< T, n > &b )
	{
		Vector< T, n > v;
		i32_t i;

		for( i = 0; i < n; ++i ) {
			v[ i ] = std::max( a[ i ], b[ i ] );
		}

		return v;
	}
	template< typename T, i32_t n >
	Vector< T, n > abs( const Vector< T, n > &a )
	{
		Vector< T, n > v;
		i32_t i;

		for( i = 0; i < n; ++i ) {
			v[ i ] = std::abs( a[ i ] );
		}

		return v;
	}

	template< typename T, i32_t n >
	Vector< T, n > clamp( const Vector< T, n > &a, T mini, T maxi )
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
	Vector< T, n > saturate( const Vector< T, n > &a )
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
	Vector< T, n > lerp( const Vector< T, n > &mini, const Vector< T, n > &maxi, T_t t )
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
	T minComponent( const Vector< T, n > &a )
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
	T maxComponent( const Vector< T, n > &a )
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