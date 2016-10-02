/*
 * Villains' Utility Library - Thomas Martin Schmid, 2016. Public domain?
 *
 * This file contains tests for the matrix struct in vul_matrix.hpp
 * 
 * ? If public domain is not legally valid in your legal jurisdiction
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
#ifndef VUL_TEST_POINT_HPP
#define VUL_TEST_POINT_HPP

#include <cassert>

#include "../vul_math.hpp"
#define VUL_TEST_RNG ( float )( ( float )rand( ) / ( float )RAND_MAX )

using namespace vul;

namespace vul_test {
   class TestPoint {

   public:
      static bool test( );

   private:
      static bool make( );
      static bool member_ops( );
      static bool comparisons( );
      static bool ops( );
      static bool functions( );
   };

   bool TestPoint::test( )
   {
      assert( make( ) );
      assert( comparisons( ) );
      assert( member_ops( ) );
      assert( ops( ) );
      assert( functions( ) );

      return true;
   }

   bool TestPoint::make( )
   {
      Point< fi32, 2 > p2;
      Point< f32, 3 > p3;
      Point< s64, 9 > p9;

#ifdef VUL_CPLUSPLUS11
      p2 = Point< fi32, 2 >( );
      p3 = Point< f32, 3 >( );
      p9 = Point< s64, 9 >( );
#else
      p2 = makePoint< fi32, 2 >( );
      p3 = makePoint< f32, 3 >( );
      p9 = makePoint< s64, 9 >( );
#endif
      assert( p2[ 0 ] == fi32( 0.f ) ); assert( p2[ 1 ] == fi32( 0.f ) );
      assert( p3[ 0 ] == 0.f ); assert( p3[ 1 ] == 0.f ); assert( p3[ 2 ] == 0.f );
      for( u32 i = 0; i < 9; ++i ) {
         assert( p9[ i ] == 0L );
      }

#ifdef VUL_CPLUSPLUS11
      p2 = Point< fi32, 2 >( fi32( 7.f ) );
      p3 = Point< f32, 3 >( -8.f );
      p9 = Point< s64, 9 >( 32L );
#else
      p2 = makePoint< fi32, 2 >( fi32( 7.f ) );
      p3 = makePoint< f32, 3 >( -8.f );
      p9 = makePoint< s64, 9 >( 32L );
#endif
      assert( p2[ 0 ] == fi32( 7.f ) ); assert( p2[ 1 ] == fi32( 7.f ) );
      assert( p3[ 0 ] == -8.f ); assert( p3[ 1 ] == -8.f ); assert( p3[ 2 ] == -8.f );
      for( u32 i = 0; i < 9; ++i ) {
         assert( p9[ i ] == 32L );
      }

#ifdef VUL_CPLUSPLUS11
      Point< fi32, 2 > p2c = Point< fi32, 2 >( p2 );
      Point< f32, 3 > p3c = Point< f32, 3 >( p3 );
      Point< s64, 9 > p9c = Point< s64, 9 >( p9 );
#else
      Point< fi32, 2 > p2c = makePoint< fi32, 2 >( p2 );
      Point< f32, 3 > p3c = makePoint< f32, 3 >( p3 );
      Point< s64, 9 > p9c = makePoint< s64, 9 >( p9 );
#endif
      assert( p2c[ 0 ] == fi32( 7.f ) ); assert( p2c[ 1 ] == fi32( 7.f ) );
      assert( p3c[ 0 ] == -8.f ); assert( p3c[ 1 ] == -8.f ); assert( p3c[ 2 ] == -8.f );
      for( u32 i = 0; i < 9; ++i ) {
         assert( p9c[ i ] == 32L );
      }

      fi32 a2[ 2 ] = { ( fi32 )VUL_TEST_RNG, ( fi32 )VUL_TEST_RNG };
      f32 a3[ 3 ] = { VUL_TEST_RNG, VUL_TEST_RNG, VUL_TEST_RNG };
      s64 a9[ 9 ] = { -5L, -4L, -3L, -2L, -1L, 0L, 1L, 2L, 3L };
#ifdef VUL_CPLUSPLUS11
      p2 = Point< fi32, 2 >( a2 );
      p3 = Point< f32, 3 >( a3 );
      p9 = Point< s64, 9 >( a9 );
#else
      p2 = makePoint< fi32, 2 >( a2 );
      p3 = makePoint< f32, 3 >( a3 );
      p9 = makePoint< s64, 9 >( a9 );
#endif
      assert( p2[ 0 ] == a2[ 0 ] ); assert( p2[ 1 ] == a2[ 1 ] );
      assert( p3[ 0 ] == a3[ 0 ] ); assert( p3[ 1 ] == a3[ 1 ] ); assert( p3[ 2 ] == a3[ 2 ] );
      for( u32 i = 0; i < 9; ++i ) {
         assert( p9[ i ] == a9[ i ] );
      }

      s32 ai[ 3 ] = { -9, 5, 17 };
      f32 af[ 9 ];
      for( u32 i = 0; i < 9; ++i ) {
         af[ i ] = VUL_TEST_RNG;
      }

      p3 = makePoint< f32, 3 >( ai );
      p9 = makePoint< s64, 9 >( af );

      assert( p3[ 0 ] == -9.f ); assert( p3[ 1 ] == 5.f ); assert( p3[ 2 ] == 17.f );
      for( u32 i = 0; i < 9; ++i ) {
         assert( p9[ i ] == ( s64 )af[ i ] );
      }

#ifdef VUL_CPLUSPLUS11
      Vector< fi32, 2 > v2( { ( fi32 )VUL_TEST_RNG, ( fi32 )VUL_TEST_RNG } );
      p2 = Point< fi32, 2 >( v2 );
#else
      fi32 afi2[ 2 ] = { ( fi32 )VUL_TEST_RNG, ( fi32 )VUL_TEST_RNG };
      Vector< fi32, 2 > v2 = makeVector< fi32 >( afi2 );
      p2 = makePoint< fi32, 2 >( v2 );
#endif
      assert( v2[ 0 ] == p2[ 0 ] ); assert( v2[ 1 ] == p2[ 1 ] );

      Point< f32, 2 > p2f = makePoint( -1.f, 2.f );
      assert( p2f[ 0 ] == -1.f ); assert( p2f[ 1 ] == 2.f );

      Point< f32, 3 > p3f = makePoint( -1.f, 2.f, 0.5f );
      assert( p3f[ 0 ] == -1.f ); assert( p3f[ 1 ] == 2.f ); assert( p3f[ 2 ] == 0.5f );

      Point< f32, 4 > p4f = makePoint( 0.02f, 0.14f, -0.17f, 0.8f );
      assert( p4f[ 0 ] == 0.02f ); assert( p4f[ 1 ] == 0.14f ); assert( p4f[ 2 ] == -0.17f ); assert( p4f[ 3 ] == 0.8f );

      return true;
   }

   bool TestPoint::comparisons( )
   {
      Vector< bool, 2 > b2;
      Vector< bool, 4 > b4;
#ifdef VUL_CPLUSPLUS11
      Point< fi32, 2 > p2a{ ( fi32 )1.f,  ( fi32 )-1.f },
                     p2b{ ( fi32 )-1.f, ( fi32 )-1.f };
      Point< f32, 4 > p4a{ -1.f, 2.f, 3.f, 0.5f }, 
                    p4b{ -2.f, 4.f, 6.f, 1.f };
      Point< s32, 4 > p4i{ -2,   4,   6,   1 };
#else
      Point< fi32, 2 > p2a = makePoint< fi32 >( ( fi32 )1.f,  ( fi32 )-1.f ),
                     p2b = makePoint< fi32 >( ( fi32 )-1.f, ( fi32 )-1.f );
      Point< f32, 4 > p4a = makePoint< f32 >( -1.f, 2.f, 3.f, 0.5f ), 
                    p4b = makePoint< f32 >( -2.f, 4.f, 6.f, 1.f );
#endif
      
      b2 = p2a == p2a;
      assert(  b2[ 0 ] ); assert( b2[ 1 ] );
      b2 = p2a == p2b;
      assert( !b2[ 0 ] ); assert( b2[ 1 ] );

      b2 = p2a != p2a;
      assert( !b2[ 0 ] ); assert( !b2[ 1 ] );
      b2 = p2a != p2b;
      assert(  b2[ 0 ] ); assert( !b2[ 1 ] );
      
      b4 = p4a == p4a;
      assert(  b4[ 0 ] ); assert(  b4[ 1 ] ); assert(  b4[ 2 ] ); assert(  b4[ 3 ] );
      b4 = p4a == p4b;
      assert( !b4[ 0 ] ); assert( !b4[ 1 ] ); assert( !b4[ 2 ] ); assert( !b4[ 3 ] );
      
      b4 = p4a != p4a;
      assert( !b4[ 0 ] ); assert( !b4[ 1 ] ); assert( !b4[ 2 ] ); assert( !b4[ 3 ] );
      b4 = p4a != p4b;
      assert(  b4[ 0 ] ); assert(  b4[ 1 ] ); assert(  b4[ 2 ] ); assert(  b4[ 3 ] );
      
      return true;
   }

   bool TestPoint::member_ops( )
   {
      Point< f64, 9 > p9, pr9;
      Vector< f64, 9 > v9;
      f64 a9[ 9 ];

      for( u32 i = 0; i < 9; ++i ) {
         a9[ i ] = ( f64 )VUL_TEST_RNG;
         p9[ i ] = a9[ i ];
         v9[ i ] = ( f64 )VUL_TEST_RNG;
      }

      pr9 = p9;
      for( u32 i = 0; i < 9; ++i ) {
         assert( pr9[ i ] == p9[ i ] );
      }

      pr9 += v9;
      for( u32 i = 0; i < 9; ++i ) {
         assert( pr9[ i ] == p9[ i ] + v9[ i ] );
      }

      pr9 = p9;
      pr9 *= v9;
      for( u32 i = 0; i < 9; ++i ) {
         assert( pr9[ i ] == p9[ i ] * v9[ i ] );
      }
      
      pr9 = p9;
      pr9 /= v9;
      for( u32 i = 0; i < 9; ++i ) {
         assert( pr9[ i ] == p9[ i ] / v9[ i ] );
      }
      
      for( u32 i = 0; i < 9; ++i ) {
         assert( p9[ i ] == a9[ i ] );
      }

      if( p9[ 7 ] == 1.7 ) {
         p9[ 7 ] = 1.8;
         assert( p9[ 7 ]== 1.8 );
      } else {
         p9[ 7 ] = 1.7;
         assert( p9[ 7 ]== 1.7 );
      }
      
      v9 = p9.as_vec( );
      const Vector< f64, 9 > vc9 = p9.as_vec( );
      for( u32 i = 0; i < 9; ++i ) {
         assert( p9[ i ] == v9[ i ] );
         assert( p9[ i ] == vc9[ i ] );
      }
      
      return true;
   }

   bool TestPoint::ops( )
   {
      Point< f32, 19 > p9a, p9b, pr;
      Vector< f32, 19 > v9, vr;

      for( u32 i = 0; i < 19; ++i ) {
         p9a[ i ] = VUL_TEST_RNG;
         p9b[ i ] = VUL_TEST_RNG;
         v9[ i ] = VUL_TEST_RNG;
      }

      pr = p9a + v9;
      vr = p9b - p9a;
      f32 f32eps = 1e-5f;

      for( u32 i = 0; i < 19; ++i ) {
         assert( fabs( pr[ i ] - ( p9a[ i ] + v9[ i ] ) ) < f32eps );
         assert( fabs( vr[ i ] - ( p9b[ i ] - p9a[ i ] ) ) < f32eps );
      }

      return true;
   }

   bool TestPoint::functions( )
   {
      f32 f32eps = 1e-5f;
      Vector< s64, 2 > v2;
      Vector< f32, 9 > v9, mn9, mx9;
      f32 a9a[ 9 ] = { -1.5f, -1.f, -0.75f, -0.5f, 0.f, 0.25f, 0.6f, 1.f, 2.f };
      f32 a9b[ 9 ] = { -1.7f, -1.2f, 0.75f, 0.6f, 0.1f, 0.f, -0.2f, 6.f, 4.f };
      f32 a9mn[ 9 ] = { -2.f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f };
      f32 a9mx[ 9 ] = { 0.75f, 0.75f, 0.75f, 0.75f, 0.75f, 0.75f, 0.75f, 0.75f, 3.f };
#ifdef VUL_CPLUSPLUS11
      Point< s64, 2 > p2a{ 3L, -1L },
                      p2b{ 2L, 6L };
      Point< f32, 9 > p9a( a9a ),
                      p9b( a9b ),
                      mn9( a9mn ),
                      mx9( a9mx );
#else
      Point< s64, 2 > p2a = makePoint< s64 >( 3L, -1L ),
                      p2b = makePoint< s64 >( 2L, 6L );
      Point< f32, 9 > p9a = makePoint< f32 >( a9a ),
                      p9b = makePoint< f32 >( a9b ),
                      mn9 = makePoint< f32 >( a9mn ),
                      mx9 = makePoint< f32 >( a9mx );
#endif

      v2 = min( p2a, ( s64 )1L );
      assert( v2[ 0 ] == 1L ); assert( v2[ 1 ] == -1L );
      v2 = min( p2a, ( s64 )7L );
      assert( v2[ 0 ] == 3L ); assert( v2[ 1 ] == -1L );
      v9 = min( p9a, 0.f );
      assert( v9[ 0 ] == -1.5f ); assert( v9[ 1 ] == -1.f ); assert( v9[ 2 ] == -0.75f );
      assert( v9[ 3 ] == -0.5f ); assert( v9[ 4 ] ==  0.f ); assert( v9[ 5 ] ==  0.f );
      assert( v9[ 6 ] ==  0.f );  assert( v9[ 7 ] ==  0.f ); assert( v9[ 8 ] ==  0.f );

      v2 = min( p2a, p2b );
      v9 = min( p9a, p9b );
      assert( v2[ 0 ] == 2L ); assert( v2[ 1 ] == -1L );
      assert( v9[ 0 ] == -1.7f );  assert( v9[ 1 ] == -1.2f ); assert( v9[ 2 ] == -0.75f );
      assert( v9[ 3 ] == -0.5f );  assert( v9[ 4 ] ==  0.f );  assert( v9[ 5 ] ==  0.f );
      assert( v9[ 6 ] ==  -0.2f ); assert( v9[ 7 ] ==  1.f );  assert( v9[ 8 ] ==  2.f );
            
      v2 = max( p2a, ( s64 )1L );
      assert( v2[ 0 ] == 3L ); assert( v2[ 1 ] ==  1L );
      v2 = max( p2a, ( s64 )7L );
      assert( v2[ 0 ] == 7L ); assert( v2[ 1 ] == 7L );
      v9 = max( p9a, 0.f );
      assert( v9[ 0 ] == 0.0f ); assert( v9[ 1 ] == 0.f ); assert( v9[ 2 ] == 0.f );
      assert( v9[ 3 ] == 0.0f ); assert( v9[ 4 ] == 0.f ); assert( v9[ 5 ] == 0.25f );
      assert( v9[ 6 ] == 0.6f ); assert( v9[ 7 ] == 1.f ); assert( v9[ 8 ] == 2.f );

      v2 = max( p2a, p2b );
      v9 = max( p9a, p9b );
      assert( v2[ 0 ] == 3L ); assert( v2[ 1 ] == 6L );
      assert( v9[ 0 ] == -1.5f ); assert( v9[ 1 ] == -1.f );  assert( v9[ 2 ] == 0.75f );
      assert( v9[ 3 ] ==  0.6f ); assert( v9[ 4 ] ==  0.1f ); assert( v9[ 5 ] == 0.25f );
      assert( v9[ 6 ] ==  0.6f ); assert( v9[ 7 ] ==  6.f );  assert( v9[ 8 ] == 4.f );

      v2 = abs( p2a );
      v9 = abs( p9a );
      assert( v2[ 0 ] == 3L ); assert( v2[ 1 ] == 1L );
      assert( v9[ 0 ] ==  1.5f ); assert( v9[ 1 ] ==  1.f ); assert( v9[ 2 ] == 0.75f );
      assert( v9[ 3 ] ==  0.5f ); assert( v9[ 4 ] ==  0.f ); assert( v9[ 5 ] == 0.25f );
      assert( v9[ 6 ] ==  0.6f ); assert( v9[ 7 ] ==  1.f ); assert( v9[ 8 ] == 2.f );

      v9 = clamp( p9a, -0.5f, 0.75f );
      assert( v9[ 0 ] == -0.5f ); assert( v9[ 1 ] == -0.5f );  assert( v9[ 2 ] == -0.5f );
      assert( v9[ 3 ] == -0.5f ); assert( v9[ 4 ] ==  0.f );   assert( v9[ 5 ] ==  0.25f );
      assert( v9[ 6 ] ==  0.6f ); assert( v9[ 7 ] ==  0.75f ); assert( v9[ 8 ] ==  0.75f );
      
      v9 = clamp( p9a, mn9, mx9 );
      assert( v9r[ 0 ] == -2.0f ); assert( v9r[ 1 ] == -0.5f );  assert( v9r[ 2 ] == -0.5f );
      assert( v9r[ 3 ] == -0.5f ); assert( v9r[ 4 ] ==  0.f );   assert( v9r[ 5 ] ==  0.25f );
      assert( v9r[ 6 ] ==  0.6f ); assert( v9r[ 7 ] ==  0.75f ); assert( v9r[ 8 ] ==  2.0f );
      
      v9 = saturate( p9b );
      assert( v9[ 0 ] ==  0.f );  assert( v9[ 1 ] ==  0.f );  assert( v9[ 2 ] == 0.75f );
      assert( v9[ 3 ] ==  0.6f ); assert( v9[ 4 ] ==  0.1f ); assert( v9[ 5 ] == 0.f );
      assert( v9[ 6 ] ==  0.f );  assert( v9[ 7 ] ==  1.f );  assert( v9[ 8 ] == 1.f );

      v9 = lerp( p9a, p9b, 0.5f );
      assert( fabs( v9[ 0 ] + 1.6f )  < f32eps ); assert( fabs( v9[ 1 ] + 1.1f )  < f32eps ); assert( fabs( v9[ 2 ] - 0.f ) < f32eps );
      assert( fabs( v9[ 3 ] - 0.05f ) < f32eps ); assert( fabs( v9[ 4 ] - 0.05f ) < f32eps ); assert( fabs( v9[ 5 ] - 0.125f ) < f32eps );
      assert( fabs( v9[ 6 ] - 0.2f )  < f32eps ); assert( fabs( v9[ 7 ] - 3.5f )  < f32eps ); assert( fabs( v9[ 8 ] - 3.f ) < f32eps );
      
      assert( minComponent( p2a ) == -1L );
      assert( minComponent( p2b ) ==  2L );
      
      assert( minComponent( p9a ) == -1.5f );
      assert( minComponent( p9b ) == -1.7f );

      assert( maxComponent( p2a ) ==  3L );
      assert( maxComponent( p2b ) ==  6L );

      assert( maxComponent( p9a ) ==  2.f );
      assert( maxComponent( p9b ) ==  6.f );
      
      return true;
   }
}

#undef VUL_TEST_RNG

#endif
