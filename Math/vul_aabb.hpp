/*
 * Villains' Utility Library - Thomas Martin Schmid, 2016. Public domain?
 *
 * An AABB struct that works in unlimited dimensions.
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

#ifndef VUL_AABB_HPP
#define VUL_AABB_HPP

#if defined( VUL_AOSOA_SSE ) 
#include <immintrin.h>
#elif defined( VUL_AOSOA_AVX )
#include <immintrin.h>
#endif
#ifdef VUL_AOSOA_NEON
#include <arm_neon.h>
#endif
#include <float.h>

#include "vul_types.hpp"
#include "vul_vector.hpp"
#include "vul_matrix.hpp"
#include "vul_affine.hpp"

namespace vul {

	//----------------
	// Declarations
	// 	

	template< typename T, s32 n >
	struct AABB {
		// @TODO(thynn): Consider changing this to center, extent
		Point< T, n > _min;
		Point< T, n > _max;

#ifdef VUL_CPLUSPLUS11
		explicit AABB< T, n >( );
		explicit AABB< T, n >( const Vector< T, n > &mini, const Vector< T, n > &maxi );
		explicit AABB< T, n >( const Point< T, n > &mini, const Point< T, n > &maxi );
		explicit AABB< T, n >( T (& a)[ 2 ][ n ] );
		explicit AABB< T, n >( std::initializer_list<T> list );
		AABB< T, n >( const AABB< T, n > &rhs );
#endif
		AABB< T, n > &operator=( const AABB< T, n > &rhs );
	};

	/**
	 * Translate the AABB by a vector.
	 */
	template< typename T, s32 n >
	AABB< T, n > translate( const AABB< T, n > &aabb, const Vector< T, n > &vec );
	/**
	 * Scale the AABB.
	 */
	template< typename T, s32 n >
	AABB< T, n > scale( const AABB< T, n > &aabb, const Vector< T, n > &v );
	/**
	 * Perform an affine transformation on a single AABB.
	 */
	template< typename T, s32 n >
	AABB< T, n > transform( const AABB< T, n > &aabb, const Affine< T, n > &a );

	/**
	 * Returns the center of an AABB
	 */
	template< typename T, s32 n >
	Point< T, n > center( const AABB< T, n > &aabb );
	/**
	 * Returns half the extent of an AABB,
	 * so corners are described by center( aabb ) +- extent( aabb ).
	 */
	template< typename T, s32 n >
	Vector< T, n > extent( const AABB< T, n > &aabb );
	/**
	 * Tests if a point is inside an AABB within a given epsilon
	 * @TODO(thynn): Move from eps to ULP comparison, see https://randomascii.wordpress.com/2012/02/25/comparing-floating-point-numbers-2012-edition/
	 */
	template< typename T, s32 n >
	bool inside( const AABB< T, n > &aabb, const Point< T, n > &pt, T epsilon = T( 0.f ) );
	/**
	 * Tests if an AABB entirely contains another AABB
	 */
	template< typename T, s32 n >
	bool contains( const AABB< T, n > &outer, const AABB< T, n > &inner );
	/**
	 * Tests whether two AABBs intersect.
	 */
	template< typename T, s32 n >
	bool intersect( const AABB< T, n > &a, const AABB< T, n > &b );
	/**
	 * Computes the union of two AABBs, the smallest AABB that contains both
	 * AABBs given. Expects the AABBs to be well defined (_min < _max for both).
	 */
	template< typename T, s32 n >
	AABB< T, n > unionize( const AABB< T, n > &a, const AABB< T, n > &b );

	/**
	 * Test if the given AABB is inside the given frustum.
	 */
	template< typename T, s32 n >
	bool insideFrustum( const AABB< T, n > &aabb, const Vector< T, n + 1 > planes[ n * 2 ] );

#ifndef VUL_CPLUSPLUS11
	template< typename T, s32 n >
	AABB< T, n > makeAABB( );
	template< typename T, s32 n >
	AABB< T, n > makeAABB( const Point< T, n > &mini, const Point< T, n > &maxi );
	template< typename T, s32 n >
	AABB< T, n > makeAABB( const Vector< T, n > &mini, const Vector< T, n > &maxi );
	template< typename T, s32 n >
	AABB< T, n > makeAABB( T( &a )[ 2 ][ n ] );
#endif
	template< typename T, s32 n >
	AABB< T, n > makeAABB( f32( &a )[ 2 ][ n ] );
	template< typename T, s32 n >
	AABB< T, n > makeAABB( s32( &a )[ 2 ][ n ] );

#ifdef VUL_AOSOA_SSE
	/**
	 * Apply a 3D affine transform to multiple SSE packed AABBs with 3D-vectors.
	 * Each AABB here contains 4 AABBs, where the vector data has the format __m128[ 3 ]:
	 * __m128[ 0 ] = xxxx, __m128[ 1 ] = yyyy, __m128[ 2 ] = zzzz
	 */
	void transform3D( AABB< __m128, 3 > *out, const AABB< __m128, 3 > *in, const Affine< f32, 3 > &trans, u32 count );
	/**
	 * Apply a 3D affine transform to multiple SSE packed AABBs with 3D-vectors.
	 * Each AABB here contains 2 AABBs, where the vector data has the format __m128d[ 3 ]:
	 * __m128d[ 0 ] = xx, __m128d[ 1 ] = yy, __m128d[ 2 ] = zz
	 */
	void transform3D( AABB< __m128d, 3 > *out, const AABB< __m128d, 3 > *in, const Affine< f64, 3 > &trans, u32 count );
	/**
	* Test an array of packed AABBs against a frustum of 6 planes.
	* Fill the result into the array out, where each result is 8 bytes.
	* Single precision.
	*/
	void inside_test( u32 *out, const AABB< __m128, 3 > *aabbs, Vector< f32, 4 > planes[ 6 ], u32 count );
	/**
	* Test an array of packed AABBs against a frustum of 6 planes.
	* Fill the result into the array out, where each result is 8 bytes.
	* Double precision.
	*/
	void inside_test( u32 *out, const AABB< __m128d, 3 > *aabbs, Vector< f64, 4 > planes[ 6 ], u32 count );
#endif
#ifdef VUL_AOSOA_AVX
	/**
	 * Apply a 3D affine transform to multiple SSE packed AABBs with 3D-vectors.
	 * Each AABB here contains 8 AABBs, where the vector data has the format __m256[ 3 ]:
	 * __m256[ 0 ] = xxxxxxxx, __m256[ 1 ] = yyyyyyyy, __m256[ 2 ] = zzzzzzzz
	 */
	void transform3D( AABB< __m256, 3 > *out, const AABB< __m256, 3 > *in, const Affine< f32, 3 > &trans, u32 count );
	/**
	 * Apply a 3D affine transform to multiple SSE packed AABBs with 3D-vectors.
	 * Each AABB here contains 4 AABBs, where the vector data has the format __m256d[ 3 ]:
	 * __m256d[ 0 ] = xxxx, __m256d[ 1 ] = yyyy, __m256d[ 2 ] = zzzz
	 */
	void transform3D( AABB< __m256d, 3 > *out, const AABB< __m256d, 3 > *in, const Affine< f64, 3 > &trans, u32 count );
	/**
	* Test an array of packed AABBs against a frustum of 6 planes.
	* Fill the result into the array out, where each result is 8 bytes.
	* Single precision.
	*/
	void inside_test( u32 *out, const AABB< __m256, 3 > *aabbs, Vector< f32, 4 > planes[ 6 ], u32 count );
	/**
	* Test an array of packed AABBs against a frustum of 6 planes.
	* Fill the result into the array out, where each result is 8 bytes.
	* Double precision.
	*/
	void inside_test( u32 *out, const AABB< __m256d, 3 > *aabbs, Vector< f64, 4 > planes[ 6 ], u32 count );
#endif
#ifdef VUL_AOSOA_NEON
	/**
	 * Apply a 3D affine transform to multiple NEON packed AABBs with 3D-vectors.
	 * Each AABB here contains 4 AABBs, where the vector data has the format float32x4_t[ 3 ]:
	 * float32x4_t[ 0 ] = xxxx, float32x4_[ 1 ] = yyyy, float32x4_t[ 2 ] = zzzz
	 */
	void transform3D( AABB< float32x4_t, 3 > *out, const AABB< float32x4_t, 3 > *in, const Affine< f32, 3 > &trans, u32 count );
	/**
	* Test an array of packed AABBs against a frustum of 6 planes.
	* Fill the result into the array out, where each result is 8 bytes.
	*/
	void inside_test( u32 *out, const AABB< float32x4_t, 3 > *aabbs, Vector< f32, 4 > planes[ 6 ], u32 count );
#endif
	
	//----------------
	// Definition
	//

#ifdef VUL_CPLUSPLUS11
	template< typename T, s32 n >
	AABB< T, n >::AABB( )
	{
		_min = Point< T, n >( );
		_max = Point< T, n >( );
	}
	template< typename T, s32 n >
	AABB< T, n >::AABB( const Point< T, n > &mini, const Point< T, n > &maxi )
	{
		_min = mini;
		_max = maxi;
	}
	template< typename T, s32 n >
	AABB< T, n >::AABB( const Vector< T, n > &mini, const Vector< T, n > &maxi )
	{
		_min = mini.as_point( );
		_max = maxi.as_point( );
	}
	template< typename T, s32 n >
	AABB< T, n >::AABB( T (& a)[ 2 ][ n ] )
	{
		_min = Point< T, n >( a[ 0 ] );
		_max = Point< T, n >( a[ 1 ] );
	}
	template< typename T, s32 n >
	AABB< T, n >::AABB( std::initializer_list< T > list )
	{
		s32 i;
		typename std::initializer_list< T >::iterator it;

		for( it = list.begin( ), i = 0; it != list.end( ) && i < n; ++it, ++i ) {
			_min.data[ i ] = *it;
		}
		for( i = 0; it != list.end( ) && i < n; ++it, ++i ) {
			_max.data[ i ] = *it;
		}
	}
	template< typename T, s32 n >
	AABB< T, n >::AABB( const AABB< T, n > &rhs )
	{		
		_min = rhs._min;
		_max = rhs._max;
	}
#else
#pragma warning(disable: 6001)
	template< typename T, s32 n >
	AABB< T, n > makeAABB( )
	{
		AABB< T, n > aabb;

		aabb._min = makePoint< T, n >( T( 0.f ) );
		aabb._max = makePoint< T, n >( T( 0.f ) );

		return aabb;
	}
	template< typename T, s32 n >
	AABB< T, n > makeAABB( const Point< T, n > &mini, const Point< T, n > &maxi )
	{
		AABB< T, n > aabb;

		aabb._min = mini;
		aabb._max = maxi;

		return aabb;
	}
	template< typename T, s32 n >
	AABB< T, n > makeAABB( const Vector< T, n > &mini, const Vector< T, n > &maxi )
	{
		AABB< T, n > aabb;

		aabb._min = mini.as_point( );
		aabb._max = maxi.as_point( );

		return aabb;
	}
	template< typename T, s32 n >
	AABB< T, n > makeAABB( T (& a)[ 2 ][ n ] )
	{
		AABB< T, n > aabb;
		
		aabb._min = makePoint< T, n >( a[ 0 ] );
		aabb._max = makePoint< T, n >( a[ 1 ] );

		return aabb;
	}
#endif
	template< typename T, s32 n >
	AABB< T, n > makeAABB( f32 (& a)[ 2 ][ n ] )
	{
		AABB< T, n > aabb;
		
		aabb._min = makePoint< T, n >( a[ 0 ] );
		aabb._max = makePoint< T, n >( a[ 1 ] );

		return aabb;
	}
	template< typename T, s32 n >
	AABB< T, n > makeAABB( s32 (& a)[ 2 ][ n ] )
	{
		AABB< T, n > aabb;
		
		aabb._min = makePoint< T, n >( a[ 0 ] );
		aabb._max = makePoint< T, n >( a[ 1 ] );

		return aabb;
	}
	
	template< typename T, s32 n >
	AABB< T, n > &AABB< T, n >::operator=( const AABB< T, n > &rhs )
	{
		_min = rhs._min;
		_max = rhs._max;
		return *this;
	}

	template< typename T, s32 n >
	AABB< T, n > translate( const AABB< T, n > &aabb, const Vector< T, n > &vec )
	{
		AABB< T, n > r;

		r._min = aabb._min + vec;
		r._max = aabb._max + vec;

		return r;
	}
	template< typename T, s32 n >
	AABB< T, n > scale( const AABB< T, n > &aabb, const Vector< T, n > &v )
	{
		AABB< T, n > r, tmp;

		tmp._min = ( aabb._min.as_vec( ) * v ).as_point( );
		tmp._max = ( aabb._max.as_vec( ) * v ).as_point( );

		// Handle negative scales
		r._min = min( tmp._min, tmp._max ).as_point( );
		r._max = max( tmp._min, tmp._max ).as_point( );

		return r;
	}
	template< typename T, s32 n >
	AABB< T, n > transform( const AABB< T, n > &aabb, const Affine< T, n > &a )
	{
		AABB< T, n > res;
		Vector< T, n > corner, newmin, newmax;
		unsigned int i, j;

#ifdef VUL_CPLUSPLUS11
		newmin = Vector< T, n >(  std::numeric_limits< T >::max( ) );
		newmax = Vector< T, n >( -std::numeric_limits< T >::max( ) );
#else
		newmin = makeVector< T, n >(  std::numeric_limits< T >::max( ) );
		newmax = makeVector< T, n >( -std::numeric_limits< T >::max( ) );
#endif
		for( i = 0; i < ( u32 )pow( 2.f, n ); ++i )
		{
			for( j = 0; j < n; ++j )
			{
				corner.data[ j ] = ( ( i & ( 1 << j ) ) != 0 ) ? aabb._max.data[ j ] : aabb._min.data[ j ];
			}
			corner = ( a * corner ) + a.vec;
			newmin = min( newmin, corner );
			newmax = max( newmax, corner );
		}
#ifdef VUL_CPLUSPLUS11
		res = AABB< T, n >( newmin, newmax );
#else
		res = makeAABB< T, n >( newmin, newmax );
#endif

		return res;
	}
#pragma warning(default: 6001)
	
	template< typename T, s32 n >
	Point< T, n > center( const AABB< T, n > &aabb )
	{
		return ( ( aabb._max.as_vec( ) + aabb._min.as_vec( ) ) * static_cast< T >( 0.5f ) ).as_point( );
	}

	template< typename T, s32 n >
	Vector< T, n > extent( const AABB< T, n > &aabb )
	{
		return ( aabb._max - aabb._min ) * static_cast< T >( 0.5f );
	}

	template< typename T, s32 n >
	bool inside( const AABB< T, n > &aabb, const Point< T, n > &pt, T epsilon )
	{
		Vector< T, n > eps = makeVector< T, n >( epsilon );
		return all(									// Are all coordinates'
					abs( pt - center( aabb ) )		// distance to the center
				  - abs( extent( aabb ) )			// smaller than or equal to
					<= eps );						// the size of the extent
	}
	template< typename T, s32 n >
	bool contains( const AABB< T, n > &outer, const AABB< T, n > &inner )
	{
		return all( outer._min.as_vec( ) <= inner._min.as_vec( ) )
			&& all( outer._max.as_vec( ) >= inner._max.as_vec( ) );
	}
	template< typename T, s32 n >
	bool intersect( const AABB< T, n > &a, const AABB< T, n > &b )
	{
		return all( a._min.as_vec( ) <= b._max.as_vec( ) ) 
			&& all( a._max.as_vec( ) >= b._min.as_vec( ) );
	}
	template< typename T, s32 n >
	AABB< T, n > unionize( const AABB< T, n > &a, const AABB< T, n > &b )
	{
#ifdef VUL_CPLUSPLUS11
		return AABB< T, n >( min( a._min.as_vec( ), 
								  b._min.as_vec( ) ).as_point( ),
						     max( a._max.as_vec( ), 
								  b._max.as_vec( ) ).as_point( ) );
#else
		return makeAABB< T, n >( min( a._min.as_vec( ), 
									  b._min.as_vec( ) ).as_point( ),
							     max( a._max.as_vec( ), 
									  b._max.as_vec( ) ).as_point( ) );
#endif
	}

	template< typename T, s32 n >
	bool insideFrustum( const AABB< T, n > &aabb, const Vector< T, n + 1 > planes[ n * 2 ] )
	{
		u32 i;
		Vector< T, n > c, e, p;
		
		c = aabb._min.as_vec( ) + aabb._max.as_vec( );
		e = aabb._max.as_vec( ) - aabb._min.as_vec( );

		for( i = 0; i < n * 2; ++i ) {
			p = truncate< T, n, n + 1 >( planes[ i ] );
			// This isn't really optimized, for that you'd use the SIMD versions
			if( dot( c, p ) + dot( e, abs( p ) ) > -planes[ i ].data[ n ] ) {
				return true;
			}
		}

		return false;
	}

	//--------------------------
	// AOSOA SSE functions
	// These are specializations for vectors of sse types (see vul_aosoa.hpp)
	//
#ifdef VUL_DEFINE
#ifdef VUL_AOSOA_SSE
	void transform3D( AABB< __m128, 3 > *out, const AABB< __m128, 3 > *in, const Affine< f32, 3 > &trans, u32 count )
	{
		u32 i, j, simdCount;
		__m128 mat[ 4 ][ 3 ];

		// Fill the matrix from the Affine
		for( i = 0; i < 3; ++i ) {
			for( j = 0;j < 3; ++j ) {
				mat[ i ][ j ] = _mm_set1_ps( trans.mat.data[ j ][ i ] );
			}
		}
		mat[ 3 ][ 0 ] = _mm_set1_ps( trans.vec.data[ 0 ] );
		mat[ 3 ][ 1 ] = _mm_set1_ps( trans.vec.data[ 1 ] );
		mat[ 3 ][ 2 ] = _mm_set1_ps( trans.vec.data[ 2 ] );

		// Tranform
		simdCount = ( count + 3 ) / 4;
		for( i = 0; i < simdCount; ++i )
		{
			__m128 xNewMinis = {  FLT_MAX,  FLT_MAX,  FLT_MAX,  FLT_MAX };
			__m128 xNewMaxes = { -FLT_MAX, -FLT_MAX, -FLT_MAX, -FLT_MAX };
			__m128 yNewMinis = {  FLT_MAX,  FLT_MAX,  FLT_MAX,  FLT_MAX };
			__m128 yNewMaxes = { -FLT_MAX, -FLT_MAX, -FLT_MAX, -FLT_MAX };
			__m128 zNewMinis = {  FLT_MAX,  FLT_MAX,  FLT_MAX,  FLT_MAX };
			__m128 zNewMaxes = { -FLT_MAX, -FLT_MAX, -FLT_MAX, -FLT_MAX };

			for( j = 0; j < 8; ++j )
			{
				const AABB< __m128, 3 > bb = in[ i ];
				__m128 xCorners = ( ( j & 1 ) != 0 ) ? bb._max.data[ 0 ] : bb._min.data[ 0 ];
				__m128 yCorners = ( ( j & 2 ) != 0 ) ? bb._max.data[ 1 ] : bb._min.data[ 1 ];
				__m128 zCorners = ( ( j & 4 ) != 0 ) ? bb._max.data[ 2 ] : bb._min.data[ 2 ];

				__m128 xCornerTrans = _mm_add_ps( _mm_add_ps( _mm_mul_ps( xCorners, mat[ 0 ][ 0 ] ),
															  _mm_mul_ps( yCorners, mat[ 1 ][ 0 ] ) ),
												  _mm_add_ps( _mm_mul_ps( zCorners, mat[ 2 ][ 0 ] ),
																					mat[ 3 ][ 0 ] ) );
				__m128 yCornerTrans = _mm_add_ps( _mm_add_ps( _mm_mul_ps( xCorners, mat[ 0 ][ 1 ] ),
															  _mm_mul_ps( yCorners, mat[ 1 ][ 1 ] ) ),
												  _mm_add_ps( _mm_mul_ps( zCorners, mat[ 2 ][ 1 ] ),
																					mat[ 3 ][ 1 ] ) );
				__m128 zCornerTrans = _mm_add_ps( _mm_add_ps( _mm_mul_ps( xCorners, mat[ 0 ][ 2 ] ),
															  _mm_mul_ps( yCorners, mat[ 1 ][ 2 ] ) ),
												  _mm_add_ps( _mm_mul_ps( zCorners, mat[ 2 ][ 2 ] ),
																					mat[ 3 ][ 2 ] ) );

				xNewMinis = _mm_min_ps( xNewMinis, xCornerTrans );
				xNewMaxes = _mm_max_ps( xNewMaxes, xCornerTrans );
				yNewMinis = _mm_min_ps( yNewMinis, yCornerTrans );
				yNewMaxes = _mm_max_ps( yNewMaxes, yCornerTrans );
				zNewMinis = _mm_min_ps( zNewMinis, zCornerTrans );
				zNewMaxes = _mm_max_ps( zNewMaxes, zCornerTrans );
			}

			AABB< __m128, 3 > *bb = &out[ i ];
			bb->_min.data[ 0 ] = xNewMinis;
			bb->_max.data[ 0 ] = xNewMaxes;
			bb->_min.data[ 1 ] = yNewMinis;
			bb->_max.data[ 1 ] = yNewMaxes;
			bb->_min.data[ 2 ] = zNewMinis;
			bb->_max.data[ 2 ] = zNewMaxes;
		}
	}
	void transform3D( AABB< __m128d, 3 > *out, const AABB< __m128d, 3 > *in, const Affine< f64, 3 > &trans, u32 count )
	{
		u32 i, j, simdCount;
		__m128d mat[ 4 ][ 3 ];

		// Fill the matrix from the Affine
		for( i = 0; i < 3; ++i ) {
			for( j = 0;j < 3; ++j ) {
				mat[ i ][ j ] = _mm_set1_pd( trans.mat.data[ j ][ i ] );
			}
		}
		mat[ 3 ][ 0 ] = _mm_set1_pd( trans.vec.data[ 0 ] );
		mat[ 3 ][ 1 ] = _mm_set1_pd( trans.vec.data[ 1 ] );
		mat[ 3 ][ 2 ] = _mm_set1_pd( trans.vec.data[ 2 ] );

		// Tranform
		simdCount = ( count + 1 ) / 2;
		for( i = 0; i < simdCount; ++i )
		{
			__m128d xNewMinis = {  DBL_MAX,  DBL_MAX };
			__m128d xNewMaxes = { -DBL_MAX, -DBL_MAX };
			__m128d yNewMinis = {  DBL_MAX,  DBL_MAX };
			__m128d yNewMaxes = { -DBL_MAX, -DBL_MAX };
			__m128d zNewMinis = {  DBL_MAX,  DBL_MAX };
			__m128d zNewMaxes = { -DBL_MAX, -DBL_MAX };

			for( j = 0; j < 8; ++j )
			{
				const AABB< __m128d, 3 > bb = in[ i ];
				__m128d xCorners = ( ( j & 1 ) != 0 ) ? bb._max.data[ 0 ] : bb._min.data[ 0 ];
				__m128d yCorners = ( ( j & 2 ) != 0 ) ? bb._max.data[ 1 ] : bb._min.data[ 1 ];
				__m128d zCorners = ( ( j & 4 ) != 0 ) ? bb._max.data[ 2 ] : bb._min.data[ 2 ];

				
				__m128d xCornerTrans = _mm_add_pd( _mm_add_pd( _mm_mul_pd( xCorners, mat[ 0 ][ 0 ] ),
															   _mm_mul_pd( yCorners, mat[ 1 ][ 0 ] ) ),
												   _mm_add_pd( _mm_mul_pd( zCorners, mat[ 2 ][ 0 ] ),
																					 mat[ 3 ][ 0 ] ) );
				__m128d yCornerTrans = _mm_add_pd( _mm_add_pd( _mm_mul_pd( xCorners, mat[ 0 ][ 1 ] ),
															   _mm_mul_pd( yCorners, mat[ 1 ][ 1 ] ) ),
												   _mm_add_pd( _mm_mul_pd( zCorners, mat[ 2 ][ 1 ] ),
																					 mat[ 3 ][ 1 ] ) );
				__m128d zCornerTrans = _mm_add_pd( _mm_add_pd( _mm_mul_pd( xCorners, mat[ 0 ][ 2 ] ),
															   _mm_mul_pd( yCorners, mat[ 1 ][ 2 ] ) ),
												   _mm_add_pd( _mm_mul_pd( zCorners, mat[ 2 ][ 2 ] ),
												 									 mat[ 3 ][ 2 ] ) );
				
				xNewMinis = _mm_min_pd( xNewMinis, xCornerTrans );
				xNewMaxes = _mm_max_pd( xNewMaxes, xCornerTrans );
				yNewMinis = _mm_min_pd( yNewMinis, yCornerTrans );
				yNewMaxes = _mm_max_pd( yNewMaxes, yCornerTrans );
				zNewMinis = _mm_min_pd( zNewMinis, zCornerTrans );
				zNewMaxes = _mm_max_pd( zNewMaxes, zCornerTrans );
			}

			AABB< __m128d, 3 > *bb = &out[ i ];
			bb->_min.data[ 0 ] = xNewMinis;
			bb->_max.data[ 0 ] = xNewMaxes;
			bb->_min.data[ 1 ] = yNewMinis;
			bb->_max.data[ 1 ] = yNewMaxes;
			bb->_min.data[ 2 ] = zNewMinis;
			bb->_max.data[ 2 ] = zNewMaxes;
		}
	}
#endif // VUL_AOSOA_SSE
#ifdef VUL_AOSOA_AVX
	void transform3D( AABB< __m256, 3 > *out, const AABB< __m256, 3 > *in, const Affine< f32, 3 > &trans, u32 count )
	{
		u32 i, j, simdCount;
		__m256 mat[ 4 ][ 3 ];

		// Fill the matrix from the Affine
		for( i = 0; i < 3; ++i ) {
			for( j = 0;j < 3; ++j ) {
				mat[ i ][ j ] = _mm256_set1_ps( trans.mat.data[ j ][ i ] );
			}
		}
		mat[ 3 ][ 0 ] = _mm256_set1_ps( trans.vec.data[ 0 ] );
		mat[ 3 ][ 1 ] = _mm256_set1_ps( trans.vec.data[ 1 ] );
		mat[ 3 ][ 2 ] = _mm256_set1_ps( trans.vec.data[ 2 ] );

		// Tranform
		simdCount = ( count + 7 ) / 8;
		for( i = 0; i < simdCount; ++i )
		{
			__m256 xNewMinis = {  FLT_MAX,  FLT_MAX,  FLT_MAX,  FLT_MAX,  FLT_MAX,  FLT_MAX,  FLT_MAX,  FLT_MAX };
			__m256 xNewMaxes = { -FLT_MAX, -FLT_MAX, -FLT_MAX, -FLT_MAX, -FLT_MAX, -FLT_MAX, -FLT_MAX, -FLT_MAX };
			__m256 yNewMinis = {  FLT_MAX,  FLT_MAX,  FLT_MAX,  FLT_MAX,  FLT_MAX,  FLT_MAX,  FLT_MAX,  FLT_MAX };
			__m256 yNewMaxes = { -FLT_MAX, -FLT_MAX, -FLT_MAX, -FLT_MAX, -FLT_MAX, -FLT_MAX, -FLT_MAX, -FLT_MAX };
			__m256 zNewMinis = {  FLT_MAX,  FLT_MAX,  FLT_MAX,  FLT_MAX,  FLT_MAX,  FLT_MAX,  FLT_MAX,  FLT_MAX };
			__m256 zNewMaxes = { -FLT_MAX, -FLT_MAX, -FLT_MAX, -FLT_MAX, -FLT_MAX, -FLT_MAX, -FLT_MAX, -FLT_MAX };

			for( j = 0; j < 8; ++j )
			{
				const AABB< __m256, 3 > bb = in[ i ];
				__m256 xCorners = ( ( j & 1 ) != 0 ) ? bb._max.data[ 0 ] : bb._min.data[ 0 ];
				__m256 yCorners = ( ( j & 2 ) != 0 ) ? bb._max.data[ 1 ] : bb._min.data[ 1 ];
				__m256 zCorners = ( ( j & 4 ) != 0 ) ? bb._max.data[ 2 ] : bb._min.data[ 2 ];

				
				__m256 xCornerTrans = _mm256_add_ps( _mm256_add_ps( _mm256_mul_ps( xCorners, mat[ 0 ][ 0 ] ),
																	_mm256_mul_ps( yCorners, mat[ 1 ][ 0 ] ) ),
												     _mm256_add_ps( _mm256_mul_ps( zCorners, mat[ 2 ][ 0 ] ),
																							 mat[ 3 ][ 0 ] ) );
				__m256 yCornerTrans = _mm256_add_ps( _mm256_add_ps( _mm256_mul_ps( xCorners, mat[ 0 ][ 1 ] ),
																	_mm256_mul_ps( yCorners, mat[ 1 ][ 1 ] ) ),
													 _mm256_add_ps( _mm256_mul_ps( zCorners, mat[ 2 ][ 1 ] ),
																							 mat[ 3 ][ 1 ] ) );
				__m256 zCornerTrans = _mm256_add_ps( _mm256_add_ps( _mm256_mul_ps( xCorners, mat[ 0 ][ 2 ] ),
																	_mm256_mul_ps( yCorners, mat[ 1 ][ 2 ] ) ),
													 _mm256_add_ps( _mm256_mul_ps( zCorners, mat[ 2 ][ 2 ] ),
																							 mat[ 3 ][ 2 ] ) );

				xNewMinis = _mm256_min_ps( xNewMinis, xCornerTrans );
				xNewMaxes = _mm256_max_ps( xNewMaxes, xCornerTrans );
				yNewMinis = _mm256_min_ps( yNewMinis, yCornerTrans );
				yNewMaxes = _mm256_max_ps( yNewMaxes, yCornerTrans );
				zNewMinis = _mm256_min_ps( zNewMinis, zCornerTrans );
				zNewMaxes = _mm256_max_ps( zNewMaxes, zCornerTrans );
			}

			AABB< __m256, 3 > *bb = &out[ i ];
			bb->_min.data[ 0 ] = xNewMinis;
			bb->_max.data[ 0 ] = xNewMaxes;
			bb->_min.data[ 1 ] = yNewMinis;
			bb->_max.data[ 1 ] = yNewMaxes;
			bb->_min.data[ 2 ] = zNewMinis;
			bb->_max.data[ 2 ] = zNewMaxes;
		}
	}
	void transform3D( AABB< __m256d, 3 > *out, const AABB< __m256d, 3 > *in, const Affine< f64, 3 > &trans, u32 count )
	{
		u32 i, j, simdCount;
		__m256d mat[ 4 ][ 3 ];

		// Fill the matrix from the Affine
		for( i = 0; i < 3; ++i ) {
			for( j = 0;j < 3; ++j ) {
				mat[ i ][ j ] = _mm256_set1_pd( trans.mat.data[ j ][ i ] );
			}
		}
		mat[ 3 ][ 0 ] = _mm256_set1_pd( trans.vec.data[ 0 ] );
		mat[ 3 ][ 1 ] = _mm256_set1_pd( trans.vec.data[ 1 ] );
		mat[ 3 ][ 2 ] = _mm256_set1_pd( trans.vec.data[ 2 ] );

		// Tranform
		simdCount = ( count + 3) / 4;
		for( i = 0; i < simdCount; ++i )
		{
			__m256d xNewMinis = {  DBL_MAX,  DBL_MAX,  DBL_MAX,  DBL_MAX };
			__m256d xNewMaxes = { -DBL_MAX, -DBL_MAX, -DBL_MAX, -DBL_MAX };
			__m256d yNewMinis = {  DBL_MAX,  DBL_MAX,  DBL_MAX,  DBL_MAX };
			__m256d yNewMaxes = { -DBL_MAX, -DBL_MAX, -DBL_MAX, -DBL_MAX };
			__m256d zNewMinis = {  DBL_MAX,  DBL_MAX,  DBL_MAX,  DBL_MAX };
			__m256d zNewMaxes = { -DBL_MAX, -DBL_MAX, -DBL_MAX, -DBL_MAX };

			for( j = 0; j < 8; ++j )
			{
				const AABB< __m256d, 3 > bb = in[ i ];
				__m256d xCorners = ( ( j & 1 ) != 0 ) ? bb._max.data[ 0 ] : bb._min.data[ 0 ];
				__m256d yCorners = ( ( j & 2 ) != 0 ) ? bb._max.data[ 1 ] : bb._min.data[ 1 ];
				__m256d zCorners = ( ( j & 4 ) != 0 ) ? bb._max.data[ 2 ] : bb._min.data[ 2 ];

				__m256d xCornerTrans = _mm256_add_pd( _mm256_add_pd( _mm256_mul_pd( xCorners, mat[ 0 ][ 0 ] ),
																	 _mm256_mul_pd( yCorners, mat[ 1 ][ 0 ] ) ),
												      _mm256_add_pd( _mm256_mul_pd( zCorners, mat[ 2 ][ 0 ] ),
																							  mat[ 3 ][ 0 ] ) );
				__m256d yCornerTrans = _mm256_add_pd( _mm256_add_pd( _mm256_mul_pd( xCorners, mat[ 0 ][ 1 ] ),
																	 _mm256_mul_pd( yCorners, mat[ 1 ][ 1 ] ) ),
													  _mm256_add_pd( _mm256_mul_pd( zCorners, mat[ 2 ][ 1 ] ),
																							  mat[ 3 ][ 1 ] ) );
				__m256d zCornerTrans = _mm256_add_pd( _mm256_add_pd( _mm256_mul_pd( xCorners, mat[ 0 ][ 2 ] ),
																	 _mm256_mul_pd( yCorners, mat[ 1 ][ 2 ] ) ),
													  _mm256_add_pd( _mm256_mul_pd( zCorners, mat[ 2 ][ 2 ] ),
																							  mat[ 3 ][ 2 ] ) );

				xNewMinis = _mm256_min_pd( xNewMinis, xCornerTrans );
				xNewMaxes = _mm256_max_pd( xNewMaxes, xCornerTrans );
				yNewMinis = _mm256_min_pd( yNewMinis, yCornerTrans );
				yNewMaxes = _mm256_max_pd( yNewMaxes, yCornerTrans );
				zNewMinis = _mm256_min_pd( zNewMinis, zCornerTrans );
				zNewMaxes = _mm256_max_pd( zNewMaxes, zCornerTrans );
			}

			AABB< __m256d, 3 > *bb = &out[ i ];
			bb->_min.data[ 0 ] = xNewMinis;
			bb->_max.data[ 0 ] = xNewMaxes;
			bb->_min.data[ 1 ] = yNewMinis;
			bb->_max.data[ 1 ] = yNewMaxes;
			bb->_min.data[ 2 ] = zNewMinis;
			bb->_max.data[ 2 ] = zNewMaxes;
		}
	}
#endif // VUL_AOSOA_AVX
#ifdef VUL_AOSOA_NEON
	void transform3D( AABB< float32x4_t, 3 > *out, const AABB< float32x4_t, 3 > *in, const Affine< f32, 3 > &trans, u32 count )
	{
		u32 i, j, simdCount;
		float32x4_t mat[ 4 ][ 3 ];

		// Fill the matrix from the Affine
		for( i = 0; i < 3; ++i ) {
			for( j = 0;j < 3; ++j ) {
				mat[ i ][ j ] = vdupq_n_f32( trans.mat.data[ j ][ i ] );
			}
		}
		mat[ 3 ][ 0 ] = vdupq_n_f32( trans.vec.data[ 0 ] );
		mat[ 3 ][ 1 ] = vdupq_n_f32( trans.vec.data[ 1 ] );
		mat[ 3 ][ 2 ] = vdupq_n_f32( trans.vec.data[ 2 ] );

		// Tranform
		simdCount = ( count + 3 ) / 4;
		for( i = 0; i < simdCount; ++i )
		{
			float32x4_t xNewMinis = {  FLT_MAX,  FLT_MAX,  FLT_MAX,  FLT_MAX };
			float32x4_t xNewMaxes = { -FLT_MAX, -FLT_MAX, -FLT_MAX, -FLT_MAX };
			float32x4_t yNewMinis = {  FLT_MAX,  FLT_MAX,  FLT_MAX,  FLT_MAX };
			float32x4_t yNewMaxes = { -FLT_MAX, -FLT_MAX, -FLT_MAX, -FLT_MAX };
			float32x4_t zNewMinis = {  FLT_MAX,  FLT_MAX,  FLT_MAX,  FLT_MAX };
			float32x4_t zNewMaxes = { -FLT_MAX, -FLT_MAX, -FLT_MAX, -FLT_MAX };

			for( j = 0; j < 8; ++j )
			{
				const AABB< float32x4_t, 3 > bb = in[ i ];
				float32x4_t xCorners = ( ( j & 1 ) != 0 ) ? bb._max.data[ 0 ] : bb._min.data[ 0 ];
				float32x4_t yCorners = ( ( j & 2 ) != 0 ) ? bb._max.data[ 1 ] : bb._min.data[ 1 ];
				float32x4_t zCorners = ( ( j & 4 ) != 0 ) ? bb._max.data[ 2 ] : bb._min.data[ 2 ];

				float32x4_t xCornerTrans = vaddq_f32( vaddq_f32( vmulq_f32( xCorners, mat[ 0 ][ 0 ] ),
																 vmulq_f32( yCorners, mat[ 1 ][ 0 ] ) ),
													  vaddq_f32( vmulq_f32( zCorners, mat[ 2 ][ 0 ] ),
																					  mat[ 3 ][ 0 ] ) );
				float32x4_t yCornerTrans = vaddq_f32( vaddq_f32( vmulq_f32( xCorners, mat[ 0 ][ 1 ] ),
																 vmulq_f32( yCorners, mat[ 1 ][ 1 ] ) ),
													  vaddq_f32( vmulq_f32( zCorners, mat[ 2 ][ 1 ] ),
																					  mat[ 3 ][ 1 ] ) );	
				float32x4_t zCornerTrans = vaddq_f32( vaddq_f32( vmulq_f32( xCorners, mat[ 0 ][ 2 ] ),
																 vmulq_f32( yCorners, mat[ 1 ][ 2 ] ) ),
													  vaddq_f32( vmulq_f32( zCorners, mat[ 2 ][ 2 ] ),
																					  mat[ 3 ][ 2 ] ) );	

				xNewMinis = vminq_f32( xNewMinis, xCornerTrans );
				xNewMaxes = vmaxq_f32( xNewMaxes, xCornerTrans );
				yNewMinis = vminq_f32( yNewMinis, yCornerTrans );
				yNewMaxes = vmaxq_f32( yNewMaxes, yCornerTrans );
				zNewMinis = vminq_f32( zNewMinis, zCornerTrans );
				zNewMaxes = vmaxq_f32( zNewMaxes, zCornerTrans );
			}

			AABB< float32x4_t, 3 > *bb = &out[ i ];
			bb->_min.data[ 0 ] = xNewMinis;
			bb->_max.data[ 0 ] = xNewMaxes;
			bb->_min.data[ 1 ] = yNewMinis;
			bb->_max.data[ 1 ] = yNewMaxes;
			bb->_min.data[ 2 ] = zNewMinis;
			bb->_max.data[ 2 ] = zNewMaxes;
		}
	}
#endif // VUL_AOSOA_NEON

#ifdef VUL_AOSOA_SSE
	void inside_test( u32 *out, const AABB< __m128, 3 > *aabbs, Vector< f32, 4 > planes[ 6 ], u32 count )
	{
		__m128 c[ 3 ];
		__m128 e[ 3 ];
		__m128 p[ 6 ][ 4 ]; // [ plane_nr ][ coordinate ]
		__m128 signFlip[ 3 ], signBit;
		__m128 t0[ 3 ];
		__m128 t1[ 3 ];
		u32 simdCount;

		signBit = _mm_set1_ps( 0x80000000 );

		for( u32 j = 0; j < 6; ++j ) {
			p[ j ][ 0 ] = _mm_set1_ps( planes[ j ].data[ 0 ] );
			p[ j ][ 1 ] = _mm_set1_ps( planes[ j ].data[ 1 ] );
			p[ j ][ 2 ] = _mm_set1_ps( planes[ j ].data[ 2 ] );
			p[ j ][ 3 ] = _mm_set1_ps( -planes[ j ].data[ 3 ] );
		}
		
		simdCount = ( count + 3 ) / 4;
		for( u32 i = 0; i < simdCount; ++i ) {
			c[ 0 ] = _mm_add_ps( aabbs[ i ]._min.data[ 0 ], aabbs[ i ]._max.data[ 0 ] );
			c[ 1 ] = _mm_add_ps( aabbs[ i ]._min.data[ 1 ], aabbs[ i ]._max.data[ 1 ] );
			c[ 2 ] = _mm_add_ps( aabbs[ i ]._min.data[ 2 ], aabbs[ i ]._max.data[ 2 ] );

			e[ 0 ] = _mm_sub_ps( aabbs[ i ]._max.data[ 0 ], aabbs[ i ]._min.data[ 0 ] );
			e[ 1 ] = _mm_sub_ps( aabbs[ i ]._max.data[ 1 ], aabbs[ i ]._min.data[ 1 ] );
			e[ 2 ] = _mm_sub_ps( aabbs[ i ]._max.data[ 2 ], aabbs[ i ]._min.data[ 2 ] );

			out[ i ] = 0x00000000;
			for( u32 j = 0; j < 6; ++j ) {
				signFlip[ 0 ] = _mm_and_ps( p[ j ][ 0 ], signBit );
				signFlip[ 1 ] = _mm_and_ps( p[ j ][ 1 ], signBit );
				signFlip[ 2 ] = _mm_and_ps( p[ j ][ 2 ], signBit );

				// t0 = xor( e, signFlip )
				t0[ 0 ] = _mm_xor_ps( e[ 0 ], signFlip[ 0 ] );
				t0[ 1 ] = _mm_xor_ps( e[ 1 ], signFlip[ 1 ] );
				t0[ 2 ] = _mm_xor_ps( e[ 2 ], signFlip[ 2 ] );

				// t1 = c + t0
				t1[ 0 ] = _mm_add_ps( c[ 0 ], t0[ 0 ] );
				t1[ 1 ] = _mm_add_ps( c[ 1 ], t0[ 1 ] );
				t1[ 2 ] = _mm_add_ps( c[ 2 ], t0[ 2 ] );

				// t2 = dot ( t1, plane.xyz )
				t0[ 0 ] = _mm_mul_ps( t1[ 0 ], p[ j ][ 0 ] );
				t0[ 1 ] = _mm_mul_ps( t1[ 1 ], p[ j ][ 1 ] );
				t0[ 2 ] = _mm_mul_ps( t1[ 2 ], p[ j ][ 2 ] );
				t1[ 0 ] = _mm_add_ps( t0[ 0 ], t0[ 1 ] );
				t1[ 1 ] = _mm_add_ps( t1[ 0 ], t0[ 2 ] );

				// t2 > -plane.w
				union {
					__m128 v;
					f32 a[ 4 ];
				} t;
				t.v = _mm_cmpgt_ps( t1[ 1 ], p[ j ][ 3 ] );

				// Store result
				out[ i ] = ( t.a[ 0 ] == 0xffffffff ? 0xff000000 : 0x00000000 )
						 | ( t.a[ 1 ] == 0xffffffff ? 0x00ff0000 : 0x00000000 )
						 | ( t.a[ 2 ] == 0xffffffff ? 0x0000ff00 : 0x00000000 )
						 | ( t.a[ 3 ] == 0xffffffff ? 0x000000ff : 0x00000000 );
			}
		}
	}
	void inside_test( u32 *out, const AABB< __m128d, 3 > *aabbs, Vector< f64, 4 > planes[ 6 ], u32 count )
	{
		__m128d c[ 3 ];
		__m128d e[ 3 ];
		__m128d p[ 6 ][ 4 ]; // [ plane_nr ][ coordinate ]
		__m128d signFlip[ 3 ], signBit;
		__m128d t0[ 3 ];
		__m128d t1[ 3 ];
		u32 simdCount;

		signBit = _mm_set1_pd( 0x80000000 );

		for( u32 j = 0; j < 6; ++j ) {
			p[ j ][ 0 ] = _mm_set1_pd( planes[ j ].data[ 0 ] );
			p[ j ][ 1 ] = _mm_set1_pd( planes[ j ].data[ 1 ] );
			p[ j ][ 2 ] = _mm_set1_pd( planes[ j ].data[ 2 ] );
			p[ j ][ 3 ] = _mm_set1_pd( -planes[ j ].data[ 3 ] );
		}

		simdCount = ( count + 1 ) / 2;
		for( u32 i = 0; i < simdCount; ++i ) {
			c[ 0 ] = _mm_add_pd( aabbs[ i ]._min.data[ 0 ], aabbs[ i ]._max.data[ 0 ] );
			c[ 1 ] = _mm_add_pd( aabbs[ i ]._min.data[ 1 ], aabbs[ i ]._max.data[ 1 ] );
			c[ 2 ] = _mm_add_pd( aabbs[ i ]._min.data[ 2 ], aabbs[ i ]._max.data[ 2 ] );

			e[ 0 ] = _mm_sub_pd( aabbs[ i ]._max.data[ 0 ], aabbs[ i ]._min.data[ 0 ] );
			e[ 1 ] = _mm_sub_pd( aabbs[ i ]._max.data[ 1 ], aabbs[ i ]._min.data[ 1 ] );
			e[ 2 ] = _mm_sub_pd( aabbs[ i ]._max.data[ 2 ], aabbs[ i ]._min.data[ 2 ] );

			out[ i ] = 0x00000000;
			for( u32 j = 0; j < 6; ++j ) {
				signFlip[ 0 ] = _mm_and_pd( p[ j ][ 0 ], signBit );
				signFlip[ 1 ] = _mm_and_pd( p[ j ][ 1 ], signBit );
				signFlip[ 2 ] = _mm_and_pd( p[ j ][ 2 ], signBit );

				// t0 = xor( e, signFlip )
				t0[ 0 ] = _mm_xor_pd( e[ 0 ], signFlip[ 0 ] );
				t0[ 1 ] = _mm_xor_pd( e[ 1 ], signFlip[ 1 ] );
				t0[ 2 ] = _mm_xor_pd( e[ 2 ], signFlip[ 2 ] );

				// t1 = c + t0
				t1[ 0 ] = _mm_add_pd( c[ 0 ], t0[ 0 ] );
				t1[ 1 ] = _mm_add_pd( c[ 1 ], t0[ 1 ] );
				t1[ 2 ] = _mm_add_pd( c[ 2 ], t0[ 2 ] );

				// t2 = dot ( t1, plane.xyz )
				t0[ 0 ] = _mm_mul_pd( t1[ 0 ], p[ j ][ 0 ] );
				t0[ 1 ] = _mm_mul_pd( t1[ 1 ], p[ j ][ 1 ] );
				t0[ 2 ] = _mm_mul_pd( t1[ 2 ], p[ j ][ 2 ] );
				t1[ 0 ] = _mm_add_pd( t0[ 0 ], t0[ 1 ] );
				t1[ 1 ] = _mm_add_pd( t1[ 0 ], t0[ 2 ] );

				// t2 > -plane.w
				union {
					__m128d v;
					f64 a[ 2 ];
				} t;
				t.v = _mm_cmpgt_pd( t1[ 1 ], p[ j ][ 3 ] );

				// Store result
				if( i % 1 ) {
					out[ i ] = ( *( ( u64* )&t.a[ 0 ] ) == 0xffffffffffffffff ? 0x0000ff00 : 0x00000000 )
							 | ( *( ( u64* )&t.a[ 1 ] ) == 0xffffffffffffffff ? 0x000000ff : 0x00000000 );
				} else {
					out[ i ] = ( *( ( u64* )&t.a[ 0 ] ) == 0xffffffffffffffff ? 0xff000000 : 0x00000000 )
							 | ( *( ( u64* )&t.a[ 1 ] ) == 0xffffffffffffffff ? 0x00ff0000 : 0x00000000 );
				}
			}
		}
	}
#endif // VUL_AOSOA_SSE
#ifdef VUL_AOSOA_AVX
	/**
	* Test an array of packed AABBs against a frustum of 6 planes.
	* Fill the result into the array out, where 0 is false and everything else is true.
	*/
	void inside_test( u32 *out, const AABB< __m256, 3 > *aabbs, Vector< f32, 4 > planes[ 6 ], u32 count )
	{
		__m256 c[ 3 ];
		__m256 e[ 3 ];
		__m256 p[ 6 ][ 4 ]; // [ plane_nr ][ coordinate ]
		__m256 signFlip[ 3 ], signBit;
		__m256 t0[ 3 ];
		__m256 t1[ 3 ];
		u32 simdCount;

		signBit = _mm256_set1_ps( 0x80000000 );

		for( u32 j = 0; j < 6; ++j ) {
			p[ j ][ 0 ] = _mm256_set1_ps( planes[ j ].data[ 0 ] );
			p[ j ][ 1 ] = _mm256_set1_ps( planes[ j ].data[ 1 ] );
			p[ j ][ 2 ] = _mm256_set1_ps( planes[ j ].data[ 2 ] );
			p[ j ][ 3 ] = _mm256_set1_ps( -planes[ j ].data[ 3 ] );
		}

		simdCount = ( count + 7 ) / 8;
		for( u32 i = 0; i < simdCount; ++i ) {
			c[ 0 ] = _mm256_add_ps( aabbs[ i ]._min.data[ 0 ], aabbs[ i ]._max.data[ 0 ] );
			c[ 1 ] = _mm256_add_ps( aabbs[ i ]._min.data[ 1 ], aabbs[ i ]._max.data[ 1 ] );
			c[ 2 ] = _mm256_add_ps( aabbs[ i ]._min.data[ 2 ], aabbs[ i ]._max.data[ 2 ] );

			e[ 0 ] = _mm256_sub_ps( aabbs[ i ]._max.data[ 0 ], aabbs[ i ]._min.data[ 0 ] );
			e[ 1 ] = _mm256_sub_ps( aabbs[ i ]._max.data[ 1 ], aabbs[ i ]._min.data[ 1 ] );
			e[ 2 ] = _mm256_sub_ps( aabbs[ i ]._max.data[ 2 ], aabbs[ i ]._min.data[ 2 ] );

			out[ i * 2 ] = 0x00000000;
			out[ i * 2 + 1 ] = 0x00000000;
			for( u32 j = 0; j < 6; ++j ) {
				signFlip[ 0 ] = _mm256_and_ps( p[ j ][ 0 ], signBit );
				signFlip[ 1 ] = _mm256_and_ps( p[ j ][ 1 ], signBit );
				signFlip[ 2 ] = _mm256_and_ps( p[ j ][ 2 ], signBit );

				// t0 = xor( e, signFlip )
				t0[ 0 ] = _mm256_xor_ps( e[ 0 ], signFlip[ 0 ] );
				t0[ 1 ] = _mm256_xor_ps( e[ 1 ], signFlip[ 1 ] );
				t0[ 2 ] = _mm256_xor_ps( e[ 2 ], signFlip[ 2 ] );

				// t1 = c + t0
				t1[ 0 ] = _mm256_add_ps( c[ 0 ], t0[ 0 ] );
				t1[ 1 ] = _mm256_add_ps( c[ 1 ], t0[ 1 ] );
				t1[ 2 ] = _mm256_add_ps( c[ 2 ], t0[ 2 ] );

				// t2 = dot ( t1, plane.xyz )
				t0[ 0 ] = _mm256_mul_ps( t1[ 0 ], p[ j ][ 0 ] );
				t0[ 1 ] = _mm256_mul_ps( t1[ 1 ], p[ j ][ 1 ] );
				t0[ 2 ] = _mm256_mul_ps( t1[ 2 ], p[ j ][ 2 ] );
				t1[ 0 ] = _mm256_add_ps( t0[ 0 ], t0[ 1 ] );
				t1[ 1 ] = _mm256_add_ps( t1[ 0 ], t0[ 2 ] );

				// t2 > -plane.w
				union {
					__m256 v;
					f32 a[ 8 ];
				} t;
				t.v = _mm256_cmp_ps( t1[ 1 ], p[ j ][ 3 ], _CMP_GT_OQ );

				// Store result
				out[ i * 2 ] |= ( *( ( u32* )&t.a[ 7 ] ) == 0.f ? 0x00000000 : 0xff000000 )
							  | ( *( ( u32* )&t.a[ 6 ] ) == 0.f ? 0x00000000 : 0x00ff0000 )
							  | ( *( ( u32* )&t.a[ 5 ] ) == 0.f ? 0x00000000 : 0x0000ff00 )
							  | ( *( ( u32* )&t.a[ 4 ] ) == 0.f ? 0x00000000 : 0x000000ff );
				out[ i * 2 + 1 ] |= ( *( ( u32* )&t.a[ 3 ] ) == 0.f ? 0x00000000 : 0xff000000 )
								  | ( *( ( u32* )&t.a[ 2 ] ) == 0.f ? 0x00000000 : 0x00ff0000 )
								  | ( *( ( u32* )&t.a[ 1 ] ) == 0.f ? 0x00000000 : 0x0000ff00 )
								  | ( *( ( u32* )&t.a[ 0 ] ) == 0.f ? 0x00000000 : 0x000000ff );
			}
		}
	}
	void inside_test( u32 *out, const AABB< __m256d, 3 > *aabbs, Vector< f64, 4 > planes[ 6 ], u32 count )
	{
		__m256d c[ 3 ];
		__m256d e[ 3 ];
		__m256d p[ 6 ][ 4 ]; // [ plane_nr ][ coordinate ]
		__m256d signFlip[ 3 ], signBit;
		__m256d t0[ 3 ];
		__m256d t1[ 3 ];
		u32 simdCount;

		signBit = _mm256_set1_pd( 0x80000000 );

		for( u32 j = 0; j < 6; ++j ) {
			p[ j ][ 0 ] = _mm256_set1_pd( planes[ j ].data[ 0 ] );
			p[ j ][ 1 ] = _mm256_set1_pd( planes[ j ].data[ 1 ] );
			p[ j ][ 2 ] = _mm256_set1_pd( planes[ j ].data[ 2 ] );
			p[ j ][ 3 ] = _mm256_set1_pd( -planes[ j ].data[ 3 ] );
		}

		simdCount = ( count + 3 ) / 4;
		for( u32 i = 0; i < simdCount; ++i ) {
			c[ 0 ] = _mm256_add_pd( aabbs[ i ]._min.data[ 0 ], aabbs[ i ]._max.data[ 0 ] );
			c[ 1 ] = _mm256_add_pd( aabbs[ i ]._min.data[ 1 ], aabbs[ i ]._max.data[ 1 ] );
			c[ 2 ] = _mm256_add_pd( aabbs[ i ]._min.data[ 2 ], aabbs[ i ]._max.data[ 2 ] );

			e[ 0 ] = _mm256_sub_pd( aabbs[ i ]._max.data[ 0 ], aabbs[ i ]._min.data[ 0 ] );
			e[ 1 ] = _mm256_sub_pd( aabbs[ i ]._max.data[ 1 ], aabbs[ i ]._min.data[ 1 ] );
			e[ 2 ] = _mm256_sub_pd( aabbs[ i ]._max.data[ 2 ], aabbs[ i ]._min.data[ 2 ] );

			out[ i ] = 0x00000000;
			for( u32 j = 0; j < 6; ++j ) {
				signFlip[ 0 ] = _mm256_and_pd( p[ j ][ 0 ], signBit );
				signFlip[ 1 ] = _mm256_and_pd( p[ j ][ 1 ], signBit );
				signFlip[ 2 ] = _mm256_and_pd( p[ j ][ 2 ], signBit );

				// t0 = xor( e, signFlip )
				t0[ 0 ] = _mm256_xor_pd( e[ 0 ], signFlip[ 0 ] );
				t0[ 1 ] = _mm256_xor_pd( e[ 1 ], signFlip[ 1 ] );
				t0[ 2 ] = _mm256_xor_pd( e[ 2 ], signFlip[ 2 ] );

				// t1 = c + t0
				t1[ 0 ] = _mm256_add_pd( c[ 0 ], t0[ 0 ] );
				t1[ 1 ] = _mm256_add_pd( c[ 1 ], t0[ 1 ] );
				t1[ 2 ] = _mm256_add_pd( c[ 2 ], t0[ 2 ] );

				// t2 = dot ( t1, plane.xyz )
				t0[ 0 ] = _mm256_mul_pd( t1[ 0 ], p[ j ][ 0 ] );
				t0[ 1 ] = _mm256_mul_pd( t1[ 1 ], p[ j ][ 1 ] );
				t0[ 2 ] = _mm256_mul_pd( t1[ 2 ], p[ j ][ 2 ] );
				t1[ 0 ] = _mm256_add_pd( t0[ 0 ], t0[ 1 ] );
				t1[ 1 ] = _mm256_add_pd( t1[ 0 ], t0[ 2 ] );

				// t2 > -plane.w
				union {
					__m256d v;
					f64 a[ 4 ];
				} t;
				t.v = _mm256_cmp_pd( t1[ 1 ], p[ j ][ 3 ], _CMP_GT_OQ );

				// Store result
				out[ i ] |= ( *( ( u32* )&t.a[ 3 ] ) == 0 ? 0x0000000000000000 : 0xff000000 )
						  | ( *( ( u32* )&t.a[ 2 ] ) == 0 ? 0x0000000000000000 : 0x00ff0000 )
						  | ( *( ( u32* )&t.a[ 1 ] ) == 0 ? 0x0000000000000000 : 0x0000ff00 )
						  | ( *( ( u32* )&t.a[ 0 ] ) == 0 ? 0x0000000000000000 : 0x000000ff );
			}
		}
	}
#endif // VUL_AOSOA_AVX
#ifdef VUL_AOSOA_NEON
	/**
	* Test an array of packed AABBs against a frustum of 6 planes.
	* Fill the result into the array out, where 0 is false and everything else is true.
	*/
	void inside_test( u32 *out, const AABB< float32x4_t, 3 > *aabbs, Vector< f32, 4 > planes[ 6 ], u32 count )
	{
		float32x4_t c[ 3 ];
		float32x4_t e[ 3 ];
		float32x4_t p[ 6 ][ 4 ]; // [ plane_nr ][ coordinate ]
		int32x4_t signFlip[ 3 ], signBit;
		float32x4_t t0[ 3 ];
		float32x4_t t1[ 3 ];
		uint32x4_t res;
		u32 simdCount;

		signBit = vdupq_n_s32( 0x80000000 );

		for( u32 j = 0; j < 6; ++j ) {
			p[ j ][ 0 ] = vdupq_n_f32( planes[ j ].data[ 0 ] );
			p[ j ][ 1 ] = vdupq_n_f32( planes[ j ].data[ 1 ] );
			p[ j ][ 2 ] = vdupq_n_f32( planes[ j ].data[ 2 ] );
			p[ j ][ 3 ] = vdupq_n_f32( -planes[ j ].data[ 3 ] );
		}

		simdCount = ( count + 3 ) / 4;
		for( u32 i = 0; i < simdCount; ++i ) {
			c[ 0 ] = vaddq_f32( aabbs[ i ]._min.data[ 0 ], aabbs[ i ]._max.data[ 0 ] );
			c[ 1 ] = vaddq_f32( aabbs[ i ]._min.data[ 1 ], aabbs[ i ]._max.data[ 1 ] );
			c[ 2 ] = vaddq_f32( aabbs[ i ]._min.data[ 2 ], aabbs[ i ]._max.data[ 2 ] );

			e[ 0 ] = vsubq_f32( aabbs[ i ]._max.data[ 0 ], aabbs[ i ]._min.data[ 0 ] );
			e[ 1 ] = vsubq_f32( aabbs[ i ]._max.data[ 1 ], aabbs[ i ]._min.data[ 1 ] );
			e[ 2 ] = vsubq_f32( aabbs[ i ]._max.data[ 2 ], aabbs[ i ]._min.data[ 2 ] );

			out[ i ] = 0x00000000;
			for( u32 j = 0; j < 6; ++j ) {
				signFlip[ 0 ] = vandq_s32( *( ( int32x4_t* )&p[ j ][ 0 ]), signBit );
				signFlip[ 1 ] = vandq_s32( *( ( int32x4_t* )&p[ j ][ 1 ]), signBit );
				signFlip[ 2 ] = vandq_s32( *( ( int32x4_t* )&p[ j ][ 2 ]), signBit );

				// t0 = xor( e, signFlip )
				signFlip[ 0 ] = veorq_s32( e[ 0 ], signFlip[ 0 ] );
				signFlip[ 1 ] = veorq_s32( e[ 1 ], signFlip[ 1 ] );
				signFlip[ 2 ] = veorq_s32( e[ 2 ], signFlip[ 2 ] );

				// t1 = c + t0
				t1[ 0 ] = vaddq_f32( c[ 0 ], *( ( float32x4_t* )&signFlip[ 0 ] ) );
				t1[ 1 ] = vaddq_f32( c[ 1 ], *( ( float32x4_t* )&signFlip[ 1 ] ) );
				t1[ 2 ] = vaddq_f32( c[ 2 ], *( ( float32x4_t* )&signFlip[ 2 ] ) );

				// t2 = dot ( t1, plane.xyz )
				t0[ 0 ] = vmulq_f32( t1[ 0 ], p[ j ][ 0 ] );
				t0[ 1 ] = vmulq_f32( t1[ 1 ], p[ j ][ 1 ] );
				t0[ 2 ] = vmulq_f32( t1[ 2 ], p[ j ][ 2 ] );
				t1[ 0 ] = vaddq_f32( t0[ 0 ], t0[ 1 ] );
				t1[ 1 ] = vaddq_f32( t1[ 0 ], t0[ 2 ] );

				// t2 > -plane.w
				res = vcgtq_f32( t1[ 1 ], p[ j ][ 3 ] );

				// Store result
				out[ i ] |= ( vgetq_lane_f32( res, 0 ) == 0xffffffff ? 0xff000000 : 0x00000000 )
						  | ( vgetq_lane_f32( res, 1 ) == 0xffffffff ? 0x00ff0000 : 0x00000000 )
						  | ( vgetq_lane_f32( res, 2 ) == 0xffffffff ? 0x0000ff00 : 0x00000000 )
						  | ( vgetq_lane_f32( res, 3 ) == 0xffffffff ? 0x000000ff : 0x00000000 );
			}
		}
	}
#endif // VUL_AOSOA_NEON
	

#endif // VUL_DEFINE
}

#endif
