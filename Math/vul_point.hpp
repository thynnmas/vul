/*
 * Villains' Utility Library - Thomas Martin Schmid, 2017. Public domain¹
 *
 * A vector math library for generic points. These are similar to the vectors
 * in vul_vector.h, but only define operations defined on points. Explicit
 * conversions are given to/from vectors.
 *
 * The points are scalar.
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
#ifndef VUL_POINT_HPP
#define VUL_POINT_HPP

#include "vul_types.hpp"

#include <cstring> // For memset
#ifndef VUL_MATH_ASSERT
#include <assert.h>
#define VUL_MATH_ASSERT assert
#endif


namespace vul {
   
   template< typename T, s32 n >
   struct Vector;

   //----------------
   // Declarations
   //
   
   template< typename T, s32 n >
   struct Point {
      T data[ n ];

#ifdef VUL_CPLUSPLUS11
      // Constructors
      explicit Point< T, n >( );                      // Empty constructor. Memsets entirety of data to 0.
      explicit Point< T, n >( T val );                   // Initialize to single value
      Point< T, n >( const Point< T, n > &p );           // Copy constructor
      explicit Point< T, n >( T (& a)[ n ] );            // Generic array constructor
      explicit Point< T, n >( const Vector< T, n > &vec );  // Explicit conversion from a vector
      explicit Point< T, n >( std::initializer_list<T> list );// From initializer list. Componentwise init is non-c++11 equivalent.
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
      T &operator[ ]( s32 i );
      /**
       * Constant indexing operator.
       */
      T const &operator[ ]( s32 i ) const;
      /**
       * Cast to a vector
       */
      Vector< T, n > &as_vec( );
      /**
       * Cast to const vector.
       */
      const Vector< T, n > &as_vec( ) const;
   };


#ifndef VUL_CPLUSPLUS11
   // Constructors
   template< typename T, s32 n >
   Point< T, n > makePoint( );                        // Empty constructor. Memsets entirety of data to 0.
   template< typename T, s32 n >
   Point< T, n > makePoint( T val );                  // Initialize to single value
   template< typename T, s32 n >
   Point< T, n > makePoint( const Point< T, n > &p );       // Copy constructor
   template< typename T, s32 n >
   Point< T, n > makePoint( T (& a)[ n ] );                 // Generic array constructor
   template< typename T, s32 n >
   Point< T, n > makePoint( const Vector< T, n > &vec ); // Explicit conversion from a vector
#endif
   template< typename T, s32 n >
   Point< T, n > makePoint( f32 (& a)[ n ] );               // From float arrays, to interface with other libraries
   template< typename T, s32 n >
   Point< T, n > makePoint( s32 (& a)[ n ] );               // From int arrays, to interface with other libraries

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
   template< typename T, s32 n >
   Vector< bool, n > operator==( const Point< T, n > &a, const Point< T, n > &b );
   /**
    * Componentwise coparison. Returns a vector of boolean indicating if
    * compnents in corresponding positions are not equal.
    */
   template< typename T, s32 n >
   Vector< bool, n > operator!=( const Point< T, n > &a, const Point< T, n > &b );

   /**
    * Translate point a by vector b. Returns the resulting point.
    */
   template< typename T, s32 n >
   Point< T, n > operator+( const Point< T, n > &a, const Vector< T, n > &b );
   /**
    * Find the translation between points b and a, and return it as a vector.
    * Which means it behaves as a componentwise subtraction.
    */
   template< typename T, s32 n >
   Vector< T, n > operator-( const Point< T, n > &a, const Point< T, n > &b );
   
   /**
    * Componentwise min( compnent, b )
    */
   template< typename T, s32 n >
   Vector< T, n > min( const Point< T, n > &a, T b );
   /**
    * Componentwise max( compnent, b ).
    */
   template< typename T, s32 n >
   Vector< T, n > max( const Point< T, n > &a, T b );
   /**
    * Componentwise min( compnent_a, compnent_b )
    */
   template< typename T, s32 n >
   Vector< T, n > min( const Point< T, n > &a, const Point< T, n > &b );
   /**
    * Componentwise max( compnent_a, compnent_b )
    */
   template< typename T, s32 n >
   Vector< T, n > max( const Point< T, n > &a, const Point< T, n > &b );
   /**
    * Compnentwise abs
    */
   template< typename T, s32 n >
   Vector< T, n > abs( const Point< T, n > &a );
   /**
    * Componentwise clamp
    */
   template< typename T, s32 n >
   Vector< T, n > clamp( const Point< T, n > &a, T mini, T maxi );
   /**
    * Componentwise per-axis clamp
    */
   template< typename T, s32 n >
   Vector< T, n > clamp( const Point< T, n > &a, const Point< T, n > &mini, const Point< T, n > &maxi );
   /**
    * Componentwise saturate, so clamp( x, 0, 1 )
    */
   template< typename T, s32 n >
   Vector< T, n > saturate( const Point< T, n > &a );
   /**
    * Componentwise linear interpolation based on t.
    */
   template< typename T, s32 n, typename T_t >
   Vector< T, n > lerp( const Point< T, n > &mini, const Point< T, n > &maxi, T_t t );
   /**
    * Returns the smallest compnent.
    */
   template< typename T, s32 n >
   T minComponent( const Point< T, n > &a );
   /**
    * Returns the largest compnent.
    */
   template< typename T, s32 n >
   T maxComponent( const Point< T, n > &a );
      
   
   //---------------------------
   // Definitions
   //

#ifdef VUL_CPLUSPLUS11
   template< typename T, s32 n >
   Point< T, n >::Point( )
   {
      memset( data, 0, sizeof( data ) );
   }
   template< typename T, s32 n >
   Point< T, n >::Point( T val )
   {
      s32 i;
      for( i = 0; i < n; ++i ) {
         data[ i ] = val;
      }
   }
   template< typename T, s32 n >
   Point< T, n >::Point( const Point< T, n > &p )
   {
      s32 i;
      for( i = 0; i < n; ++i ) {
         data[ i ] = p[ i ];
      }
   }
   template< typename T, s32 n >
   Point< T, n >::Point( T (& a)[ n ] )
   {
      s32 i;
      for( i = 0; i < n; ++i ) {
         data[ i ] = a[ i ];
      }
   }
   template< typename T, s32 n >
   Point< T, n >::Point( const Vector< T, n > &vec )
   {
      s32 i;
      for( i = 0; i < n; ++i ) {
         data[ i ] = vec[ i ];
      }
   }
   template< typename T, s32 n >
   Point< T, n >::Point( std::initializer_list< T > list )
   {
      s32 i;
      typename std::initializer_list< T >::iterator it;

      for( it = list.begin( ), i = 0; it != list.end( ) && i < n; ++it, ++i ) {
         data[ i ] = *it;
      }
   }
#else
   template< typename T, s32 n >
   Point< T, n > makePoint( )
   {
      Point< T, n > p;

      memset( p.data, 0, sizeof( p.data ) );

      return p;
   }
   template< typename T, s32 n >
   Point< T, n > makePoint( T val )
   {
      Point< T, n > p;
      s32 i;

      for( i = 0; i < n; ++i ) {
         p.data[ i ] = val;
      }

      return p;
   }
   template< typename T, s32 n >
   Point< T, n > makePoint( const Point< T, n > &p )
   {
      Point< T, n > r;
      s32 i;

      for( i = 0; i < n; ++i ) {
         r.data[ i ] = p[ i ];
      }

      return r;
   }
   template< typename T, s32 n >
   Point< T, n > makePoint( T (& a)[ n ] )
   {
      Point< T, n > p;
      s32 i;

      for( i = 0; i < n; ++i ) {
         p.data[ i ] = a[ i ];
      }

      return p;
   }
   template< typename T, s32 n >
   Point< T, n > makePoint( const Vector< T, n > &vec )
   {
      Point< T, n > p;
      s32 i;

      for( i = 0; i < n; ++i ) {
         p.data[ i ] = vec.data[ i ];
      }

      return p;
   }
#endif
   
   template< typename T, s32 n >
   Point< T, n > makePoint( f32 (& a)[ n ] )
   {
      Point< T, n > p;
      s32 i;

      for( i = 0; i < n; ++i ) {
         p.data[ i ] = T( a[ i ] );
      }

      return p;
   }
   template< typename T, s32 n >
   Point< T, n > makePoint( s32 (& a)[ n ] )
   {
      Point< T, n > p;
      s32 i;

      for( i = 0; i < n; ++i ) {
         p.data[ i ] = T( a[ i ] );
      }

      return p;
   }
   template< typename T >
   Point< T, 2 > makePoint( T x, T y )
   {
      Point< T, 2 > p;

      p.data[ 0 ] = x;
      p.data[ 1 ] = y;

      return p;
   }
   template< typename T >
   Point< T, 3 > makePoint( T x, T y, T z )
   {
      Point< T, 3 > p;

      p.data[ 0 ] = x;
      p.data[ 1 ] = y;
      p.data[ 2 ] = z;

      return p;
   }
   template< typename T >
   Point< T, 4 > makePoint( T x, T y, T z, T w )
   {
      Point< T, 4 > p;

      p.data[ 0 ] = x;
      p.data[ 1 ] = y;
      p.data[ 2 ] = z;
      p.data[ 3 ] = w;

      return p;
   }

   // Operators
   template< typename T, s32 n >
   Point< T, n > &Point< T, n >::operator=( const Point< T, n > & rhs )
   {
      s32 i;

      for( i = 0; i < n; ++i ) {
         data[ i ] = rhs[ i ];
      }
      return *this;
   }

   template< typename T, s32 n >
   Point< T, n >& Point< T, n >::operator+=( const Vector< T, n > &vec )
   {
      s32 i;

      for( i = 0; i < n; ++i ) {
         data[ i ] += vec[ i ];
      }

      return *this;
   }
   template< typename T, s32 n >
   Point< T, n >& Point< T, n >::operator*=( const Vector< T, n > &vec )
   {
      s32 i;

      for( i = 0; i < n; ++i ) {
         data[ i ] *= vec[ i ];
      }

      return *this;
   }
   template< typename T, s32 n >
   Point< T, n >& Point< T, n >::operator/=( const Vector< T, n > &vec )
   {
      s32 i;

      for( i = 0; i < n; ++i ) {
         data[ i ] /= vec[ i ];
      }

      return *this;
   }

   template< typename T, s32 n >
   T &Point< T, n >::operator[ ]( s32 i )
   {
      VUL_MATH_ASSERT( i < n );
      return data[ i ];
   }
   template< typename T, s32 n >
   T const &Point< T, n >::operator[ ]( s32 i ) const
   {
      VUL_MATH_ASSERT( i < n );
      return data[ i ];
   }

   template< typename T, s32 n >
   Vector< T, n > &Point< T, n >::as_vec( )
    {
       return reinterpret_cast< Vector< T, n > & >( data );
   }
   template< typename T, s32 n >
   const Vector< T, n > &Point< T, n >::as_vec( ) const
    {
       return reinterpret_cast< const Vector< T, n > & >( data );
   }
   // Operations
   template< typename T, s32 n >
   Vector< bool, n > operator==( const Point< T, n > &a, const Point< T, n > &b )
   {
      Vector< bool, n > v;
      s32 i;

      for( i = 0; i < n; ++i ) {
         v.data[ i ] = a.data[ i ] == b.data[ i ];
      }

      return v;
   }
   template< typename T, s32 n >
   Vector< bool, n > operator!=( const Point< T, n > &a, const Point< T, n > &b )
   {
      Vector< bool, n > v;
      s32 i;

      for( i = 0; i < n; ++i ) {
         v.data[ i ] = a.data[ i ] != b.data[ i ];
      }

      return v;
   }

   template< typename T, s32 n >
   Point< T, n > operator+( const Point< T, n > &a, const Vector< T, n > &b )
   {
      Point< T, n > p;
      s32 i;

      for( i = 0; i < n; ++i ) {
         p.data[ i ] = a.data[ i ] + b.data[ i ];
      }

      return p;
   }
   template< typename T, s32 n >
   Vector< T, n > operator-( const Point< T, n > &a, const Point< T, n > &b )
   {
      Vector< T, n > v;
      s32 i;

      for( i = 0; i < n; ++i ) {
         v.data[ i ] = a.data[ i ] - b.data[ i ];
      }

      return v;
   }
   
   template< typename T, s32 n >
   Vector< T, n > min( const Point< T, n > &a, T b )
   {
      Vector< T, n > v;
      s32 i;

      for( i = 0; i < n; ++i ) {
         v.data[ i ] = a.data[ i ] < b ? a.data[ i ] : b;
      }

      return v;
   }
   template< typename T, s32 n >
   Vector< T, n > max( const Point< T, n > &a, T b )
   {
      Vector< T, n > v;
      s32 i;

      for( i = 0; i < n; ++i ) {
         v.data[ i ] = a.data[ i ] > b ? a.data[ i ] : b;
      }

      return v;
   }
   template< typename T, s32 n >
   Vector< T, n > min( const Point< T, n > &a, const Point< T, n > &b )
   {
      Vector< T, n > v;
      s32 i;

      for( i = 0; i < n; ++i ) {
         v.data[ i ] = a.data[ i ] < b.data[ i ] ? a.data[ i ] : b.data[ i ];
      }

      return v;
   }
   template< typename T, s32 n >
   Vector< T, n > max( const Point< T, n > &a, const Point< T, n > &b )
   {
      Vector< T, n > v;
      s32 i;

      for( i = 0; i < n; ++i ) {
         v.data[ i ] = a.data[ i ] > b.data[ i ] ? a.data[ i ] : b.data[ i ];
      }

      return v;
   }
   template< typename T, s32 n >
   Vector< T, n > abs( const Point< T, n > &a )
   {
      Vector< T, n > v;
      s32 i;

      for( i = 0; i < n; ++i ) {
         v.data[ i ] = a.data[ i ] >= 0 ? a.data[ i ] : -a.data[ i ];
      }

      return v;
   }
   template< typename T, s32 n >
   Vector< T, n > clamp( const Point< T, n > &a, T mini, T maxi )
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
   Vector< T, n > clamp( const Point< T, n > &a, const Point< T, n > &mini, const Point< T, n > &maxi )
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
   Vector< T, n > saturate( const Point< T, n > &a )
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
   Vector< T, n > lerp( const Point< T, n > &mini, const Point< T, n > &maxi, T_t t )
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
   T minComponent( const Point< T, n > &a )
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
   T maxComponent( const Point< T, n > &a )
   {
      s32 i;
      T m;

      m = std::numeric_limits< T >::min( );
      for( i = 0; i < n; ++i ) {
         m = a.data[ i ] > m ? a.data[ i ] : m;
      }

      return m;
   }

}

#endif
