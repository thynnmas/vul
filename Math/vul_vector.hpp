/*
 * Villains' Utility Library - Thomas Martin Schmid, 2017. Public domain¹
 *
 * This file describes generic vectors. Specializations for 2, 3 & 4 vectors of 
 * all reasonable float, fixed, int and uint types (not 128bit) are made.
 *
 * The vectors are scalar.
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

#ifndef VUL_VECTOR_HPP
#define VUL_VECTOR_HPP

#include "vul_types.hpp"
#include "vul_point.hpp"
#include "vul_matrix.hpp"

#ifndef VUL_MATH_ASSERT
#include <assert.h>
#define VUL_MATH_ASSERT assert
#endif
#include <cstring> // For memset

namespace vul {
   
   //----------------
   // Declarations
   //
   
   // Generic Definitions
   template< typename T, s32 n >
   struct Vector {
      // Data
      T data[ n ];

#ifdef VUL_CPLUSPLUS11
      // Constructors
      explicit Vector< T, n >( );                        // Empty constructor. Memsets entirety of data to 0.
      explicit Vector< T, n >( T val );                  // Initialize to single value
      Vector< T, n >( const Vector< T, n > &vec );       // Copy constructor
      explicit Vector< T, n >( const T (& a)[ n ] );        // Generic array constructor
      explicit Vector< T, n >( const Point< T, n > &p );    // Explicit conversion from a point
      explicit Vector< T, n >( std::initializer_list<T> list );// From initializer list. Componentwise init is non-c++11 equivalent.
      
#endif
      // Operators
      /**
       * Copy assignment operator
       */
      Vector< T, n >& operator=( const Vector< T, n > & rhs );

      /**
       * Componentwise addition.
       */
      Vector< T, n >& operator+=( T scalar );
      /**
       * Componentwise subtraction.
       */
      Vector< T, n >& operator-=( T scalar );
      /**
       * Componentwise multiplication.
       */
      Vector< T, n >& operator*=( T scalar );
      /**
       * Componentwise division.
       */
      Vector< T, n >& operator/=( T scalar );

      /**
       * Componentwise addition.
       */
      Vector< T, n >& operator+=( const Vector< T, n > &vec );
      /**
       * Componentwise subtraction.
       */
      Vector< T, n >& operator-=( const Vector< T, n > &vec );
      /**
       * Componentwise multiplication.
       */
      Vector< T, n >& operator*=( const Vector< T, n > &vec );
      /**
       * Componentwise division.
       */
      Vector< T, n >& operator/=( const Vector< T, n > &vec );
      
      /**
       * Indexing opertator.
       */
      T &operator[ ]( s32 i );
      /**
       * Cosntant indexing opertator.
       */
      T const &operator[ ]( s32 i ) const;
      /**
       * Cast to point.
       */
      Point< T, n > &as_point( );
      /**
       * Cast to const point. 
       */
      const Point< T, n > &as_point( ) const;
      /**
       * Cast to a matrix column. Useful for computing outer products.
       */
      Matrix< T, 1, n > &as_column( );
      /**
       * Cast to a matrix const column. Useful for computing outer products.
       */
      const Matrix< T, 1, n > &as_column( ) const;
      /**
       * Cast to a matrix row. Useful for computing outer products.
       */
      Matrix< T, n, 1 > &as_row( );
      /**
       * Cast to a matrix const row. Useful for computing outer products.
       */
      const Matrix< T, n, 1 > &as_row( ) const;
   };
   
   // Typedefs for these specializations
   typedef Vector< f16, 2 > f16x2;
   typedef Vector< f16, 3 > f16x3;
   typedef Vector< f16, 4 > f16x4;
   typedef Vector< f32, 2 > f32x2;
   typedef Vector< f32, 3 > f32x3;
   typedef Vector< f32, 4 > f32x4;
   typedef Vector< f64, 2 > f64x2;
   typedef Vector< f64, 3 > f64x3;
   typedef Vector< f64, 4 > f64x4;

   typedef Vector< fi32, 2 > fi32x2;
   typedef Vector< fi32, 3 > fi32x3;
   typedef Vector< fi32, 4 > fi32x4;

   typedef Vector< s8, 2 > s8x2;
   typedef Vector< s8, 3 > s8x3;
   typedef Vector< s8, 4 > s8x4;
   typedef Vector< s16, 2 > s16x2;
   typedef Vector< s16, 3 > s16x3;
   typedef Vector< s16, 4 > s16x4;
   typedef Vector< s32, 2 > s32x2;
   typedef Vector< s32, 3 > s32x3;
   typedef Vector< s32, 4 > s32x4;
   typedef Vector< s64, 2 > s64x2;
   typedef Vector< s64, 3 > s64x3;
   typedef Vector< s64, 4 > s64x4;

   typedef Vector< u8, 2 > u8x2;
   typedef Vector< u8, 3 > u8x3;
   typedef Vector< u8, 4 > u8x4;
   typedef Vector< u16, 2 > u16x2;
   typedef Vector< u16, 3 > u16x3;
   typedef Vector< u16, 4 > u16x4;
   typedef Vector< u32, 2 > u32x2;
   typedef Vector< u32, 3 > u32x3;
   typedef Vector< u32, 4 > u32x4;
   typedef Vector< u64, 2 > u64x2;
   typedef Vector< u64, 3 > u64x3;
   typedef Vector< u64, 4 > u64x4;

   // Member functions if not C++11, since we otherwise couldn't have vectors in unions
#ifndef VUL_CPLUSPLUS11
   template< typename T, s32 n >
   Vector< T, n > makeVector( T val );
   
   template< typename T, s32 n >
   Vector< T, n > makeVector( const Vector< T, n > &vec );

   template< typename T, s32 n >
   Vector< T, n > makeVector( );
   
   template< typename T, s32 n >
   Vector< T, n > makeVector( T (& a)[ n ] );

   template< typename T, s32 n >
   Vector< T, n > makeVector( const Point< T, n > &p );
#endif

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
   // Specialization variations with float arguments  
   template< typename T >
   Vector< T, 2 > makeVector( f32 x, f32 y );
   template< typename T >
   Vector< T, 3 > makeVector( f32 x, f32 y, f32 z );
   template< typename T >
   Vector< T, 4 > makeVector( f32 x, f32 y, f32 z, f32 w );

   // Anything from float and int arrays; to interface with other libraries
   template< typename T, s32 n >
   Vector< T, n > makeVector( f32 val );
   
   template< typename T, s32 n >
   Vector< T, n > makeVector( f32 (& a)[ n ] );

   template< typename T, s32 n >
   Vector< T, n > makeVector( s32 (& a)[ n ] );
   /**
    * Truncates a vector of size m > n into a vector of size n.
    * Copies the n first elements, discards all others.
    */
   template< typename T, s32 n, s32 m >
   Vector< T, n > truncate( const Vector< T, m > &vec );

   // Operations
   /**
    * Comopnentwise comparsion. Returns a vector of bools indicating if
    * corresponding components are equal.
    */
   template< typename T, s32 n >
   Vector< bool, n > operator==( const Vector< T, n > &a, const Vector< T, n > &b );
   /**
    * Comopnentwise comparsion. Returns a vector of bools indicating if
    * corresponding components are not equal.
    */
   template< typename T, s32 n >
   Vector< bool, n > operator!=( const Vector< T, n > &a, const Vector< T, n > &b );
   /**
    * Comopnentwise comparsion. Returns a vector of bools indicating if
    * corresponding components of a are smaller than those of b.
    */
   template< typename T, s32 n >
   Vector< bool, n > operator<( const Vector< T, n > &a, const Vector< T, n > &b );
   /**
    * Comopnentwise comparsion. Returns a vector of bools indicating if
    * corresponding components of a are greater than those of b.
    */
   template< typename T, s32 n >
   Vector< bool, n > operator>( const Vector< T, n > &a, const Vector< T, n > &b );
   /**
    * Comopnentwise comparsion. Returns a vector of bools indicating if
    * corresponding components of a are smaller than or equal to those of b.
    */
   template< typename T, s32 n >
   Vector< bool, n > operator<=( const Vector< T, n > &a, const Vector< T, n > &b );
   /**
    * Comopnentwise comparsion. Returns a vector of bools indicating if
    * corresponding components of a are greater than or equal to those of b.
    */
   template< typename T, s32 n >
   Vector< bool, n > operator>=( const Vector< T, n > &a, const Vector< T, n > &b );

   /**
    * Returns true if all compnents are true. Is valid for any type if( T ) is valid.
    * Equivalent of AND-ing together all compnents.
    */
   template< typename T, s32 n >
   bool all( const Vector< T, n > &vec );
   /**
    * Returns true if any compnents is true. Is valid for any type if( T ) is valid.
    * Equivalent of OR-ing together all compnents.
    */
   template< typename T, s32 n >
   bool any( const Vector< T, n > &vec );
   /**
    * Returns the first compnent that is evaluated to true by if( T ).
    * Equivalent to the ?: selector. Does
    * vec[0] ? vec[0] : ( vec[1] ? vec[1] : ... )
    */
   template< typename T, s32 n >
   T select( const Vector< T, n > &vec );

   /**
    * Componentwise addition. Not in place.
    */
   template< typename T, s32 n >
   Vector< T, n > operator+( const Vector< T, n > &vec, const T scalar );
   /**
    * Componentwise subtraction. Not in place.
    */
   template< typename T, s32 n >
   Vector< T, n > operator-( const Vector< T, n > &vec, const T scalar );
   /**
    * Componentwise multiplication. Not in place.
    */
   template< typename T, s32 n >
   Vector< T, n > operator*( const Vector< T, n > &vec, const T scalar );
   /**
    * Componentwise division. Not in place.
    */
   template< typename T, s32 n >
   Vector< T, n > operator/( const Vector< T, n > &vec, const T scalar );

   /**
    * Componentwise addition. Not in place.
    */
   template< typename T, s32 n >
   Vector< T, n > operator+( const Vector< T, n > &a, const Vector< T, n > &b );
   /**
    * Componentwise subtraction. Not in place.
    */
   template< typename T, s32 n >
   Vector< T, n > operator-( const Vector< T, n > &a, const Vector< T, n > &b );
   /**
    * Componentwise multiplication. Not in place.
    */
   template< typename T, s32 n >
   Vector< T, n > operator*( const Vector< T, n > &a, const Vector< T, n > &b );
   /**
    * Componentwise division. Not in place.
    */
   template< typename T, s32 n >
   Vector< T, n > operator/( const Vector< T, n > &a, const Vector< T, n > &b );
   
   /**
    * Negation operator. Negates every component.
    */
   template< typename T, s32 n >
   Vector< T, n > operator-( const Vector< T, n > &vec );

   /**
    * Cross product of two 3-vectors. Geometrically, this is a vector that is 
    * orthonormal two both vectors.
    */
   template< typename T >
   Vector< T, 3 > cross( const Vector< T, 3 > &a, const Vector< T, 3 > &b ); 
   /**
    * Cross product of 2-vectors. Geometrically, the sign of the returned value 
    * indicates whether b is to the right or left of a, if a is taken as the 
    * front-facing direction.
    */
   template< typename T >
   T cross( const Vector< T, 2 > &a, const Vector< T, 2 > &b );
   /**
    * The euclidian inner product of two vectors.
    */
   template< typename T, s32 n >
   T dot( const Vector< T, n > &a, const Vector< T, n > &b ); 

   /**
    * Computes the norm of the vector.
    * @NOTE: Returns the value in the type of the vector, meaning for integer vectors, 
    * this will be an integer. See fnorm for a norm-function that always returns a float.
    */
   template< typename T, s32 n >
   T norm( const Vector< T, n > &a );
   /**
    * Computes the norm of the vector.
    * Returns a 32-bit float no matter what type the vector is.
    */
   template< typename T, s32 n >
   f32 fnorm( const Vector< T, n > &a );
   /**
    * Returns a normalized version of the vector. This does not alter the vector itself.
    */
   template< typename T, s32 n >
   Vector< T, n > normalize( const Vector< T, n > &a );
   /**
    * Componentwise min( x, b )
    */
   template< typename T, s32 n >
   Vector< T, n > min( const Vector< T, n > &a, T b );
   /**
    * Componentwise max( x, b )
    */
   template< typename T, s32 n >
   Vector< T, n > max( const Vector< T, n > &a, T b );
   /**
    * Componentwise min( a, b )
    */
   template< typename T, s32 n >
   Vector< T, n > min( const Vector< T, n > &a, const Vector< T, n > &b );
   /**
    * Componentwise max( a, b )
    */
   template< typename T, s32 n >
   Vector< T, n > max( const Vector< T, n > &a, const Vector< T, n > &b );
   /** 
    * Componentwise abs( a )
    */
   template< typename T, s32 n >
   Vector< T, n > abs( const Vector< T, n > &a );
   /**
    * Componentwise clamp( a, mini, maxi )
    */
   template< typename T, s32 n >
   Vector< T, n > clamp( const Vector< T, n > &a, T mini, T maxi );
   /**
    * Componentwise per-axis clamp( a, mini, maxi )
    */
   template< typename T, s32 n >
   Vector< T, n > clamp( const Vector< T, n > &a, const Vector< T, n > &mini, const Vector< T, n > &maxi );
   /**
    * Componentwise modulo mod( a, divisor )
    */
   template< typename T, s32 n >
   Vector< T, n > mod( const Vector< T, n > &a, const T d );
   /**
    * Componentwise saturate, so clamp( a, 0, 1 )
    */
   template< typename T, s32 n >
   Vector< T, n > saturate( const Vector< T, n > &a );
   /**
    * Linear interpolation based on t.
    */
   template< typename T, s32 n, typename T_t >
   Vector< T, n > lerp( const Vector< T, n > &min, const Vector< T, n > &max, T_t t );
   /**
    * Returns the smallest component.
    */
   template< typename T, s32 n >
   T minComponent( const Vector< T, n > &a );
   /**
    * Returns the largest component.
    */
   template< typename T, s32 n >
   T maxComponent( const Vector< T, n > &a );      
   /**
    * Cast to a shorter version of itself 
    */
   template< typename T, s32 n_new, s32 n_old >
   const Vector< T, n_new > &truncate_cast( const Vector< T, n_old > &v );
   
   //---------------------------
   // Definitions
   //
      
#ifdef VUL_CPLUSPLUS11
   template< typename T, s32 n >
   Vector< T, n >::Vector( )
   {
      memset( data, 0, sizeof( data ) );
   }
   
   template< typename T, s32 n >
   Vector< T, n >::Vector( T val )
   {
      s32 i;
      for( i = 0; i < n; ++i ) {
         data[ i ] = val;
      }
   }

   template< typename T, s32 n >
   Vector< T, n >::Vector( const Vector< T, n > &vec )
   {
      s32 i;
      for( i = 0; i < n; ++i ) {
         data[ i ] = vec[ i ];
      }
   }

   template< typename T, s32 n >
   Vector< T, n >::Vector( const T (& a)[ n ] )
   {
      memcpy( data, a, sizeof( T ) * n );
   }
      
   template< typename T, s32 n >
   Vector< T, n >::Vector( const Point< T, n > &p )
   {
      s32 i;
      for( i = 0; i < n; ++i ) {
         data[ i ] = p[ i ];
      }     
   }
   template< typename T, s32 n >
   Vector< T, n >::Vector( std::initializer_list< T > list )
   {
      s32 i;
      typename std::initializer_list< T >::iterator it;

      for( it = list.begin( ), i = 0; it != list.end( ) && i < n; ++it, ++i ) {
         data[ i ] = *it;
      }
   }
#else
   template < typename T, s32 n >
   Vector< T, n > makeVector( T val )
   {
      Vector< T, n > v;
      s32 i;

      for( i = 0; i < n; ++i ) {
         v.data[ i ] = val;
      }

      return v;
   }
   
   template < typename T, s32 n >
   Vector< T, n > makeVector( const Vector< T, n > &vec )
   {
      Vector< T, n > v;
      s32 i;

      for( i = 0; i < n; ++i ) {
         v.data[ i ] = vec[ i ];
      }

      return v;
   }

   template < typename T, s32 n >
   Vector< T, n > makeVector( )
   {
      Vector< T, n > v;

      memset( v.data, 0, sizeof( v.data ) );

      return v;
   }
   
   template < typename T, s32 n >
   Vector< T, n > makeVector( T (& a)[ n ] )
   {
      Vector< T, n > v;
      s32 i;

      for( i = 0; i < n; ++i ) {
         v.data[ i ] = a[ i ];
      }
      return v;
   }

   template < typename T, s32 n >
   Vector< T, n > makeVector( const Point< T, n > &p )
   {
      Vector< T, n > v;
      s32 i;

      for( i = 0; i < n; ++i ) {
         v.data[ i ] = p[ i ];
      }
      return v;
   }
#endif

   // Specialization variations
   template< typename T >
   Vector< T, 2 > makeVector( T x, T y )
   { 
      Vector< T, 2 > v;
      v.data[ 0 ] = x;
      v.data[ 1 ] = y;
      return v;
   }
   template< typename T >
   Vector< T, 3 > makeVector( T x, T y, T z )
   { 
      Vector< T, 3 > v;
      v.data[ 0 ] = x; 
      v.data[ 1 ] = y; 
      v.data[ 2 ] = z;
      return v; 
   }
   template< typename T >
   Vector< T, 3 > makeVector( Vector< T, 2 > xy, T z )   
   { 
      Vector< T, 3 > v;
      v.data[ 0 ] = xy[ 0 ];
      v.data[ 1 ] = xy[ 1 ];
      v.data[ 2 ] = z;
      return v; 
   }
   template< typename T >
   Vector< T, 4 > makeVector( T x, T y, T z, T w ) 
   {
      Vector< T, 4 > v; 
      v.data[ 0 ] = x;
      v.data[ 1 ] = y;
      v.data[ 2 ] = z;
      v.data[ 3 ] = w;
      return v;
   }
   template< typename T >
   Vector< T, 4 > makeVector( Vector< T, 2 > xy, Vector< T, 2 > zw ) 
   {
      Vector< T, 4 > v ; 
      v.data[ 0 ] = xy[ 0 ]; 
      v.data[ 1 ] = xy[ 1 ]; 
      v.data[ 2 ] = zw[ 0 ];
      v.data[ 3 ] = zw[ 1 ];
      return v; 
   }
   template< typename T >
   Vector< T, 4 > makeVector( Vector< T, 3 > xyz, T w ) 
   {  
      Vector< T, 4 > v;
      v.data[ 0 ] = xyz[ 0 ];
      v.data[ 1 ] = xyz[ 1 ]; 
      v.data[ 2 ] = xyz[ 2 ]; 
      v.data[ 3 ] = w;
      return v; 
   }
   template< typename T >
   Vector< T, 2 > makeVector( f32 x, f32 y )
   {
      Vector< T, 2 > v;
      v.data[ 0 ] = T( x );
      v.data[ 1 ] = T( y );
      return v;
   }
   template< typename T >
   Vector< T, 3 > makeVector( f32 x, f32 y, f32 z )
   {
      Vector< T, 3 > v;
      v.data[ 0 ] = T( x );
      v.data[ 1 ] = T( y );
      v.data[ 2 ] = T( z );
      return v;
   }
   template< typename T >
   Vector< T, 4 > makeVector( f32 x, f32 y, f32 z, f32 w )
   {
      Vector< T, 4 > v;
      v.data[ 0 ] = T( x );
      v.data[ 1 ] = T( y );
      v.data[ 2 ] = T( z );
      v.data[ 3 ] = T( w );
      return v;
   }
   
   // Anything from float and int arrays; to interface with other libraries
   template < typename T, s32 n >
   Vector< T, n > makeVector( f32 val )
   {
      Vector< T, n > v;
      s32 i;

      for( i = 0; i < n; ++i ) {
         v.data[ i ] = T( val );
      }

      return v;
   }
   
   template< typename T, s32 n >
   Vector< T, n > makeVector( f32 (& a)[ n ] )
   {
      Vector< T, n > v;
      s32 i;

      for( i = 0; i < n; ++i ) {
         v.data[ i ] = T( a[ i ] );
      }
      return v;
   }

   template< typename T, s32 n >
   Vector< T, n > makeVector( s32 (& a)[ n ] )
   {     
      Vector< T, n > v;
      s32 i;

      for( i = 0; i < n; ++i ) {
         v.data[ i ] = T( a[ i ] );
      }
      return v;
   }
   
   template< typename T, s32 n, s32 m >
   Vector< T, n > truncate( const Vector< T, m > &vec )
   {
      Vector< T, n > r;
      s32 i;

      VUL_MATH_ASSERT( m > n );
      for( i = 0; i < n; ++i ) {
         r.data[ i ] = vec[ i ];
      }

      return r;
   }

   template< typename T, s32 n >
   Vector< T, n > &Vector< T, n >::operator=( const Vector< T, n > & rhs )
   {
      s32 i;

      for( i = 0; i < n; ++i ) {
         data[ i ] = rhs[ i ];
      }
      
      return *this;
   }

   template< typename T, s32 n >
   Vector< T, n >& Vector< T, n >::operator+=( T scalar )
   {
      s32 i;
      
      for( i = 0; i < n; ++i ) {
         data[ i ] += scalar;
      }

      return *this;
   }
   
   template< typename T, s32 n >
   Vector< T, n >& Vector< T, n >::operator-=( T scalar )
   {
      s32 i;
      
      for( i = 0; i < n; ++i ) {
         data[ i ] -= scalar;
      }

      return *this;
   }
   
   template< typename T, s32 n >
   Vector< T, n >& Vector< T, n >::operator*=( T scalar )
   {
      s32 i;
      
      for( i = 0; i < n; ++i ) {
         data[ i ] *= scalar;
      }

      return *this;  
   }
   
   template< typename T, s32 n >
   Vector< T, n >& Vector< T, n >::operator/=( T scalar )
   {
      s32 i;
      
      for( i = 0; i < n; ++i ) {
         data[ i ] /= scalar;
      }

      return *this;
   }
   
   template< typename T, s32 n >
   Vector< T, n >& Vector< T, n >::operator+=( const Vector< T, n > &vec )
   {
      s32 i;

      for( i = 0; i < n; ++i ) {
         data[ i ] += vec[ i ];
      }     

      return *this;
   }

   template< typename T, s32 n >
   Vector< T, n >& Vector< T, n >::operator-=( const Vector< T, n > &vec )
   {
      s32 i;

      for( i = 0; i < n; ++i ) {
         data[ i ] -= vec[ i ];
      }     

      return *this;
   }

   template< typename T, s32 n >
   Vector< T, n >& Vector< T, n >::operator*=( const Vector< T, n > &vec )
   {
      s32 i;

      for( i = 0; i < n; ++i ) {
         data[ i ] *= vec[ i ];
      }     

      return *this;  
   }

   template< typename T, s32 n >
   Vector< T, n >& Vector< T, n >::operator/=( const Vector< T, n > &vec )
   {
      s32 i;

      for( i = 0; i < n; ++i ) {
         data[ i ] /= vec[ i ];
      }     

      return *this;
   }

   template< typename T, s32 n >
   T& Vector< T, n >::operator[ ]( s32 i )
   {
      VUL_MATH_ASSERT( i < n );
      return data[ i ];
   }
   
   template< typename T, s32 n >
   T const &Vector< T, n >::operator[ ]( s32 i ) const
   {
      VUL_MATH_ASSERT( i < n );
      return data[ i ];
   }

   template< typename T, s32 n >
   Point< T, n > &Vector< T, n >::as_point( )
   {
      return reinterpret_cast< Point< T, n > & >( data );
   }
   template< typename T, s32 n >
   const Point< T, n > &Vector< T, n >::as_point( ) const
   {
      return reinterpret_cast< const Point< T, n > & >( data );
   }
   
   template< typename T, s32 n >
   Matrix< T, 1, n > &Vector< T, n >::as_column( )
   {
      return reinterpret_cast< Matrix< T, 1, n > & >( data );
   }
   template< typename T, s32 n >
   const Matrix< T, 1, n > &Vector< T, n >::as_column( ) const
   {
      return reinterpret_cast< const Matrix< T, 1, n > & >( data );
   }
   template< typename T, s32 n >
   Matrix< T, n, 1 > &Vector< T, n >::as_row( )
   {
      return reinterpret_cast< Matrix< T, n, 1 > & >( data );
   }
   template< typename T, s32 n >
   const Matrix< T, n, 1 > &Vector< T, n >::as_row( ) const
   {
      return reinterpret_cast< const Matrix< T, n, 1 > & >( data );
   }

   template< typename T, s32 n >
   Vector< bool, n > operator==( const Vector< T, n > &a, const Vector< T, n > &b )
   {
      Vector< bool, n > v;
      s32 i;


      for( i = 0; i < n; ++i ) {
         v.data[ i ] = a.data[ i ] == b.data[ i ];
      }

      return v;
   }
   template< typename T, s32 n >
   Vector< bool, n > operator!=( const Vector< T, n > &a, const Vector< T, n > &b )
   {
      Vector< bool, n > v;
      s32 i;

      for( i = 0; i < n; ++i ) {
         v.data[ i ] = a.data[ i ] != b.data[ i ];
      }

      return v;
   }
   template< typename T, s32 n >
   Vector< bool, n > operator<( const Vector< T, n > &a, const Vector< T, n > &b )
   {
      Vector< bool, n > v;
      s32 i;

      for( i = 0; i < n; ++i ) {
         v.data[ i ] = a.data[ i ] < b.data[ i ];
      }

      return v;
   }
   template< typename T, s32 n >
   Vector< bool, n > operator>( const Vector< T, n > &a, const Vector< T, n > &b )
   {
      Vector< bool, n > v;
      s32 i;

      for( i = 0; i < n; ++i ) {
         v.data[ i ] = a.data[ i ] > b.data[ i ];
      }

      return v;
   }
   template< typename T, s32 n >
   Vector< bool, n > operator<=( const Vector< T, n > &a, const Vector< T, n > &b )
   {
      Vector< bool, n > v;
      s32 i;

      for( i = 0; i < n; ++i ) {
         v.data[ i ] = a.data[ i ] <= b.data[ i ];
      }

      return v;
   }
   template< typename T, s32 n >
   Vector< bool, n > operator>=( const Vector< T, n > &a, const Vector< T, n > &b )
   {
      Vector< bool, n > v;
      s32 i;

      for( i = 0; i < n; ++i ) {
         v.data[ i ] = a.data[ i ] >= b.data[ i ];
      }

      return v;
   }

   template< typename T, s32 n >
   bool all( const Vector< T, n > &vec )
   {
      s32 i;
      for( i = 0; i < n; ++i ) {
         if( !vec.data[ i ] ) {
            return false;
         }
      }
      return true;
   }

   template< typename T, s32 n >
   bool any( const Vector< T, n > &vec )
   {
      s32 i;
      for( i = 0; i < n; ++i ) {
         if( vec.data[ i ] ) {
            return true;
         }
      }
      return false;
   }

   template< typename T, s32 n >
   T select( const Vector< T, n > &vec )
   {
      s32 i;
      for( i = 0; i < n; ++i ) {
         if( vec.data[ i ] ) {
            return vec.data[ i ];
         }
      }
      return T( 0.f );
   }

   template< typename T, s32 n >
   Vector< T, n > operator+( const Vector< T, n > &vec, const T scalar )
   {
      
      Vector< T, n > v;
      s32 i;

      for( i = 0; i < n; ++i ) {
         v.data[ i ] = vec.data[ i ] + scalar;
      }

      return v;
   }

   template< typename T, s32 n >
   Vector< T, n > operator-( const Vector< T, n > &vec, const T scalar )
   {
      Vector< T, n > v;
      s32 i;

      for( i = 0; i < n; ++i ) {
         v.data[ i ] = vec.data[ i ] - scalar;
      }

      return v;
   }
   template< typename T, s32 n >
   Vector< T, n > operator*( const Vector< T, n > &vec, const T scalar )
   {
      
      Vector< T, n > v;
      s32 i;

      for( i = 0; i < n; ++i ) {
         v.data[ i ] = vec.data[ i ] * scalar;
      }

      return v;
   }

   template< typename T, s32 n >
   Vector< T, n > operator/( const Vector< T, n > &vec, const T scalar )

   {
      Vector< T, n > v;
      s32 i;

      for( i = 0; i < n; ++i ) {
         v.data[ i ] = vec.data[ i ] / scalar;
      }

      return v;
   }

   template< typename T, s32 n >
   Vector< T, n > operator+( const Vector< T, n > &a, const Vector< T, n > &b )
   {
      Vector< T, n > v;
      s32 i;

      for( i = 0; i < n; ++i ) {
         v.data[ i ] = a.data[ i ] + b.data[ i ];
      }

      return v;
   }

   template< typename T, s32 n >
   Vector< T, n > operator-( const Vector< T, n > &a, const Vector< T, n > &b )
   {
      Vector< T, n > v;
      s32 i;

      for( i = 0; i < n; ++i ) {
         v.data[ i ] = a.data[ i ] - b.data[ i ];
      }

      return v;
   }

   template< typename T, s32 n >
   Vector< T, n > operator*( const Vector< T, n > &a, const Vector< T, n > &b )
   {
      Vector< T, n > v;
      s32 i;

      for( i = 0; i < n; ++i ) {
         v.data[ i ] = a.data[ i ] * b.data[ i ];
      }

      return v;
   }

   template< typename T, s32 n >
   Vector< T, n > operator/( const Vector< T, n > &a, const Vector< T, n > &b )
   {
      Vector< T, n > v;
      s32 i;

      for( i = 0; i < n; ++i ) {
         v.data[ i ] = a.data[ i ] / b.data[ i ];
      }

      return v;
   }

   template< typename T, s32 n >
   Vector< T, n > operator-( const Vector< T, n > &vec )
   {
      Vector< T, n > v;
      s32 i;

      for( i = 0; i < n; ++i ) {
         v.data[ i ] = -vec.data[ i ];
      }

      return v;
   }
   
   template< typename T >
   T cross( const Vector< T, 2 > &a, const Vector< T, 2 > &b )
   {
      return a.data[ 0 ] * b.data[ 1 ] - a.data[ 1 ] * b.data[ 0 ];
   }

   template< typename T >
   Vector< T, 3 > cross( const Vector< T, 3 > &a, const Vector< T, 3 > &b )
   {
      Vector< T, 3 > v;
      
      v.data[ 0 ] = ( a.data[ 1 ] * b.data[ 2 ] ) - ( b.data[ 1 ] * a.data[ 2 ] );
      v.data[ 1 ] = ( a.data[ 2 ] * b.data[ 0 ] ) - ( b.data[ 2 ] * a.data[ 0 ] );
      v.data[ 2 ] = ( a.data[ 0 ] * b.data[ 1 ] ) - ( b.data[ 0 ] * a.data[ 1 ] );

      return v;
   }

   template< typename T, s32 n >
   T dot( const Vector< T, n > &a, const Vector< T, n > &b )
   {
      s32 i;
      T res;

      res = a.data[ 0 ] * b.data[ 0 ];
      for( i = 1; i < n; ++i ){ 
         res += a.data[ i ] * b.data[ i ];
      }

      return res;
   }

   template< typename T, s32 n >
   T norm( const Vector< T, n > &a )
   {
      T res;
      s32 i;

      res = T( 0.f );
      for( i = 0; i < n; ++i ) {
         res += a.data[ i ] * a.data[ i ];
      }

      return std::sqrt( res );
   }

   template< typename T, s32 n >
   f32 fnorm( const Vector< T, n > &a )
   {
      f32 res;
      s32 i;

      res = 0.f;
      for( i = 0; i < n; ++i ) {
         res += ( f32 )( a.data[ i ] * a.data[ i ] );
      }

      return std::sqrt( res );
   }

   template< typename T, s32 n >
   Vector< T, n > normalize( const Vector< T, n > &a )
   {
      Vector< T, n > v;
      s32 i;
      T invnorm;

      invnorm = T( 1.f ) / norm( a );
      for( i = 0; i < n; ++i ) {
         v.data[ i ] = a.data[ i ] * invnorm;
      }

      return v;
   }

   template< typename T, s32 n >
   Vector< T, n > min( const Vector< T, n > &a, T b )
   {
      Vector< T, n > v;
      s32 i;

      for( i = 0; i < n; ++i ) {
         v.data[ i ] = a.data[ i ] < b ? a.data[ i ] : b;
      }

      return v;
   }
   template< typename T, s32 n >
   Vector< T, n > max( const Vector< T, n > &a, T b )
   {
      Vector< T, n > v;
      s32 i;

      for( i = 0; i < n; ++i ) {
         v.data[ i ] = a.data[ i ] > b ? a.data[ i ] : b;
      }

      return v;
   }
   template< typename T, s32 n >
   Vector< T, n > min( const Vector< T, n > &a, const Vector< T, n > &b )
   {
      Vector< T, n > v;
      s32 i;

      for( i = 0; i < n; ++i ) {
         v.data[ i ] = a.data[ i ] < b.data[ i ] ? a.data[ i ] : b.data[ i ];
      }

      return v;
   }
   template< typename T, s32 n >
   Vector< T, n > max( const Vector< T, n > &a, const Vector< T, n > &b )
   {
      Vector< T, n > v;
      s32 i;

      for( i = 0; i < n; ++i ) {
         v.data[ i ] = a.data[ i ] > b.data[ i ] ? a.data[ i ] : b.data[ i ];
      }

      return v;
   }
   template< typename T, s32 n >
   Vector< T, n > abs( const Vector< T, n > &a )
   {
      Vector< T, n > v;
      s32 i;

      for( i = 0; i < n; ++i ) {
         v.data[ i ] = a.data[ i ] >= 0 ? a.data[ i ] : -a.data[ i ];
      }

      return v;
   }

   template< typename T, s32 n >
   Vector< T, n > clamp( const Vector< T, n > &a, T mini, T maxi )
   {
      Vector< T, n > v;
      s32 i;

      for( i = 0; i < n; ++i ) {
         v.data[ i ] = a.data[ i ] < mini ? mini
                     : a.data[ i ] > maxi ? maxi
                     : a.data[ i ];             
      }

      return v;
   }
   template< typename T, s32 n >
   Vector< T, n > clamp( const Vector< T, n > &a, const Vector< T, n > &mini, const Vector< T, n > &maxi )
   {
      Vector< T, n > v;
      s32 i;

      for( i = 0; i < n; ++i ) {
         v.data[ i ] = a.data[ i ] < mini.data[ i ] ? mini.data[ i ]
                     : a.data[ i ] > maxi.data[ i ] ? maxi.data[ i ]
                     : a.data[ i ];
      }
      
      return v;
   }

   template< typename T, s32 n >
   Vector< T, n > mod( const Vector< T, n > &a, const T d )
   {
      Vector< T, n > v;
      s32 i;

      for( i = 0; i < n; ++i ) {
         v.data[ i ] = std::fmod( a.data[ i ], d );
      }

      return v;
   }

   template< typename T, s32 n >
   Vector< T, n > saturate( const Vector< T, n > &a )
   {
      Vector< T, n > v;
      s32 i;
      T t0, t1;

      t0 = T( 0.f );
      t1 = T( 1.f );
      for( i = 0; i < n; ++i ) {
         v.data[ i ] = a.data[ i ] < t0 ? t0
                     : a.data[ i ] > t1 ? t1
                     : a.data[ i ];
      }

      return v;
   }

   template< typename T, s32 n, typename T_t >
   Vector< T, n > lerp( const Vector< T, n > &mini, const Vector< T, n > &maxi, T_t t )
   {
      Vector< T, n > v;
      s32 i;
      T_t t1;

      t1 = T( 1.f ) - t;
      for( i = 0; i < n; ++i ) {
         v.data[ i ] = ( mini.data[ i ] * t1 ) + ( maxi.data[ i ] * t );
      }

      return v;
   }

   template< typename T, s32 n >
   T minComponent( const Vector< T, n > &a )
   {
      s32 i;
      T m;

      m = std::numeric_limits< T >::max( );
      for( i = 0; i < n; ++i ) {
         m = a.data[ i ] < m ? a.data[ i ] : m;
      }

      return m;
   }
   template< typename T, s32 n >
   T maxComponent( const Vector< T, n > &a )
   {
      s32 i;
      T m;

      m = std::numeric_limits< T >::min( );
      for( i = 0; i < n; ++i ) {
         m = a.data[ i ] > m ? a.data[ i ] : m;
      }

      return m;
   }
   
   template< typename T, s32 n_new, s32 n_old >
   const Vector< T, n_new > &truncate_cast( const Vector< T, n_old > &v )
   {
      VUL_MATH_ASSERT( n_new <= n_old );
      return reinterpret_cast< const Vector< T, n_new > & >( v.data );
   }
}

#endif
