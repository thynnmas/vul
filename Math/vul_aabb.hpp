/*
 * Villains' Utility Library - Thomas Martin Schmid, 2014. Public domain�
 *
 * An AABB struct that works in unlimited dimensions.
 * 
 * � If public domain is not legally valid in your country and or legal area,
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

#include "vul_types.hpp"
#include "vul_vector.hpp"
#include "vul_matrix.hpp"
#include "vul_affine.hpp"

/**
 * Define this for the c++11 version
 */
//#define VUL_CPLUSPLUS11

namespace vul {
	
	//----------------
	// Declarations
	// 	

	template< typename T, i32_t n >
	struct AABB {
		Point< T, n > _min;
		Point< T, n > _max;

#ifdef VUL_CPLUSPLUS11
		constexpr AABB< T, n >( );
		explicit AABB< T, n >( const Vector< T, n > &mini, const Vector< T, n > &maxi );
		explicit AABB< T, n >( const Point< T, n > &mini, const Point< T, n > &maxi );
		explicit AABB< T, n >( T (& a)[ n ] );
		explicit AABB< T, n >( f32_t[ n ] a );
		explicit AABB< T, n >( i32_t[ n ] a );
#endif
		AABB< T, n > &operator=( const AABB< T, n > &rhs );
	};

	/**
	 * Translate the AABB by a vector.
	 */
	template< typename T, i32_t n >
	AABB< T, n > translate( const AABB< T, n > &aabb, const Vector< T, n > &vec );
	/**
	 * Scale the AABB.
	 */
	template< typename T, i32_t n >
	AABB< T, n > scale( const AABB< T, n > &aabb, const Vector< T, n > &v );
	/**
	 * Perform an affine transformation on a single AABB.
	 */
	template< typename T, i32_t n >
	AABB< T, n > transform( const AABB< T, n > &aabb, const Affine< T, n > &a );
	
	/** 
	 * Returns the center of an AABB
	 */
	template< typename T, i32_t n >
	Point< T, n > center( const AABB< T, n > &aabb );
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

#ifndef VUL_CPLUSPLUS11
	template< typename T, i32_t n >
	AABB< T, n > makeAABB( );
	template< typename T, i32_t n >
	AABB< T, n > makeAABB( const Point< T, n > &mini, const Point< T, n > &maxi );
	template< typename T, i32_t n >
	AABB< T, n > makeAABB( const Vector< T, n > &mini, const Vector< T, n > &maxi );
	template< typename T, i32_t n >
	AABB< T, n > makeAABB( T (& a)[ n * 2 ] );
	template< typename T, i32_t n >
	AABB< T, n > makeAABB( f32_t (& a)[ n * 2 ] );
	template< typename T, i32_t n >
	AABB< T, n > makeAABB( i32_t (& a)[ n * 2 ] );
#endif

	/**
	 * Apply a 3D affine transform to multiple SSE packed AABBs with 3D-vectors.
	 * Each AABB here contains 4 AABBs, where the vector data has the format __m128[ 3 ]:
	 * __m128[ 0 ] = xxxx, __m128[ 1 ] = yyyy, __m128[ 2 ] = zzzz
	 */
	void transform3D( AABB< __m128, 3 > *out, const AABB< __m128, 3 > *in, const Affine< f32_t, 3 > &trans, ui32_t count );
	/**
	 * Apply a 3D affine transform to multiple SSE packed AABBs with 3D-vectors.
	 * Each AABB here contains 2 AABBs, where the vector data has the format __m128d[ 3 ]:
	 * __m128d[ 0 ] = xxxx, __m128d[ 1 ] = yyyy, __m128d[ 2 ] = zzzz
	 */
	void transform3D( AABB< __m128d, 3 > *out, const AABB< __m128d, 3 > *in, const Affine< f64_t, 3 > &trans, ui32_t count );
	/**
	 * Apply a 3D affine transform to multiple SSE packed AABBs with 3D-vectors.
	 * Each AABB here contains 8 AABBs, where the vector data has the format __m256[ 3 ]:
	 * __m256[ 0 ] = xxxx, __m256[ 1 ] = yyyy, __m256[ 2 ] = zzzz
	 */
	void transform3D( AABB< __m256, 3 > *out, const AABB< __m256, 3 > *in, const Affine< f32_t, 3 > &trans, ui32_t count );
	/**
	 * Apply a 3D affine transform to multiple SSE packed AABBs with 3D-vectors.
	 * Each AABB here contains 4 AABBs, where the vector data has the format __m256d[ 3 ]:
	 * __m256d[ 0 ] = xxxx, __m256d[ 1 ] = yyyy, __m256d[ 2 ] = zzzz
	 */
	void transform3D( AABB< __m256d, 3 > *out, const AABB< __m256d, 3 > *in, const Affine< f64_t, 3 > &trans, ui32_t count );
	

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
	AABB< T, n >::AABB( const Vector< T, n > &mini, const Vector< T, n > &maxi )
	{
		_min = mini.as_point( );
		_max = maxi.as_point( );
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
		AABB< T, n > aabb;

		aabb._min = makePoint< T, n >( T( 0.f ) );
		aabb._max = makePoint< T, n >( T( 0.f ) );

		return aabb;
	}
	template< typename T, i32_t n >
	AABB< T, n > makeAABB( const Point< T, n > &mini, const Point< T, n > &maxi )
	{
		AABB< T, n > aabb;

		aabb._min = mini;
		aabb._max = maxi;

		return aabb;
	}
	template< typename T, i32_t n >
	AABB< T, n > makeAABB( const Vector< T, n > &mini, const Vector< T, n > &maxi )
	{
		AABB< T, n > aabb;

		aabb._min = mini.as_point( );
		aabb._max = maxi.as_point( );

		return aabb;
	}
	template< typename T, i32_t n >
	AABB< T, n > makeAABB( T (& a)[ n * 2 ] )
	{
		AABB< T, n > aabb;
		T p1[ n ], p2[ n ];

		for( ui32_t i = 0; i < n; ++i ) {
			p1[ i ] = a[ i ];
			p2[ i ] = a[ n + i ];
		}
		aabb._min = makePoint< T, n >( p1 );
		aabb._max = makePoint< T, n >( p2 );

		return aabb;
	}
	template< typename T, i32_t n >
	AABB< T, n > makeAABB( f32_t (& a)[ n * 2 ] )
	{
		AABB< T, n > aabb;
		f32_t p1[ n ], p2[ n ];

		for( ui32_t i = 0; i < n; ++i ) {
			p1[ i ] = a[ i ];
			p2[ i ] = a[ n + i ];
		}
		aabb._min = makePoint< T, n >( p1 );
		aabb._max = makePoint< T, n >( p2 );

		return aabb;
	}
	template< typename T, i32_t n >
	AABB< T, n > makeAABB( i32_t (& a)[ n * 2 ] )
	{
		AABB< T, n > aabb;
		
		i32_t p1[ n ], p2[ n ];

		for( ui32_t i = 0; i < n; ++i ) {
			p1[ i ] = a[ i ];
			p2[ i ] = a[ n + i ];
		}
		aabb._min = makePoint< T, n >( p1 );
		aabb._max = makePoint< T, n >( p2 );

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
	AABB< T, n > scale( const AABB< T, n > &aabb, const Vector< T, n > &v )
	{
		AABB< T, n > r, tmp;

		tmp._min = ( r._min.as_vec( ) * v ).as_point( );
		tmp._max = ( r._max.as_vec( ) * v ).as_point( );

		// Handle negative scales
		r._min = min( tmp._min, tmp._max ).as_point( );
		r._max = max( tmp._min, tmp._max ).as_point( );

		return r;
	}
	template< typename T, i32_t n >
	AABB< T, n > transform( const AABB< T, n > &aabb, const Affine< T, n > &a )
	{
		AABB< T, n > res;
		Vector< T, n > corner, newmin, newmax;
		unsigned int i, j;

#ifdef VUL_CPLUSPLUS11
		newmin = Vector< T, n >( std::numeric_limits< T >::max( ) );
		newmax = Vector< T, n >( std::numeric_limits< T >::lowest( ) );
#else
		newmin = makeVector< T, n >( std::numeric_limits< T >::max( ) );
		newmax = makeVector< T, n >( std::numeric_limits< T >::lowest( ) );
#endif
		for( i = 0; i < ( ui32_t )pow( 2.f, n ); ++i )
		{
			for( j = 0; j < n; ++j )
			{
				ui32_t t = ( i & ( 1 << j ) );
				corner[ j ] = ( ( i & ( 1 << j ) ) != 0 ) ? aabb._max[ j ] : aabb._min[ j ];
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
	
	template< typename T, i32_t n >
	Point< T, n > center( const AABB< T, n > &aabb )
	{
		return ( ( aabb._max.as_vec( ) + aabb._min.as_vec( ) ) * static_cast< T >( 0.5f ) ).as_point( );
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
	
	//--------------------------
	// AOSOA SSE functions
	// These are specializations for vectors of sse types (see vul_aosoa.hpp)
	//
	/* @TODO: These don't agree with transform(); test both and check which is right! */
	void transform3D( AABB< __m128, 3 > *out, const AABB< __m128, 3 > *in, const Affine< f32_t, 3 > &trans, ui32_t count )
	{
		ui32_t i, j, simdCount;
		__m128 mat[ 4 ][ 3 ];

		// Fill the matrix from the Affine
		for( i = 0; i < 3; ++i ) {
			for( j = 0;j < 3; ++j ) {
				mat[ i ][ j ] = _mm_set1_ps( trans.mat( j, i ) );
			}
		}
		mat[ 3 ][ 0 ] = _mm_set1_ps( trans.vec[ 0 ] );
		mat[ 3 ][ 1 ] = _mm_set1_ps( trans.vec[ 1 ] );
		mat[ 3 ][ 2 ] = _mm_set1_ps( trans.vec[ 2 ] );

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
				__m128 xCorners = ( ( j & 1 ) != 0 ) ? bb._max[ 0 ] : bb._min[ 0 ];
				__m128 yCorners = ( ( j & 2 ) != 0 ) ? bb._max[ 1 ] : bb._min[ 1 ];
				__m128 zCorners = ( ( j & 4 ) != 0 ) ? bb._max[ 2 ] : bb._min[ 2 ];

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
			bb->_min[ 0 ] = xNewMinis;
			bb->_max[ 0 ] = xNewMaxes;
			bb->_min[ 1 ] = yNewMinis;
			bb->_max[ 1 ] = yNewMaxes;
			bb->_min[ 2 ] = zNewMinis;
			bb->_max[ 2 ] = zNewMaxes;
		}
	}
	void transform3D( AABB< __m128d, 3 > *out, const AABB< __m128d, 3 > *in, const Affine< f64_t, 3 > &trans, ui32_t count )
	{
		ui32_t i, j, simdCount;
		__m128d mat[ 4 ][ 3 ];

		// Fill the matrix from the Affine
		for( i = 0; i < 3; ++i ) {
			for( j = 0;j < 3; ++j ) {
				mat[ i ][ j ] = _mm_set1_pd( trans.mat( j, i ) );
			}
		}
		mat[ 3 ][ 0 ] = _mm_set1_pd( trans.vec[ 0 ] );
		mat[ 3 ][ 1 ] = _mm_set1_pd( trans.vec[ 1 ] );
		mat[ 3 ][ 2 ] = _mm_set1_pd( trans.vec[ 2 ] );

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
				__m128d xCorners = ( ( j & 1 ) != 0 ) ? bb._max[ 0 ] : bb._min[ 0 ];
				__m128d yCorners = ( ( j & 2 ) != 0 ) ? bb._max[ 1 ] : bb._min[ 1 ];
				__m128d zCorners = ( ( j & 4 ) != 0 ) ? bb._max[ 2 ] : bb._min[ 2 ];

				
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
			bb->_min[ 0 ] = xNewMinis;
			bb->_max[ 0 ] = xNewMaxes;
			bb->_min[ 1 ] = yNewMinis;
			bb->_max[ 1 ] = yNewMaxes;
			bb->_min[ 2 ] = zNewMinis;
			bb->_max[ 2 ] = zNewMaxes;
		}
	}
	void transform3D( AABB< __m256, 3 > *out, const AABB< __m256, 3 > *in, const Affine< f32_t, 3 > &trans, ui32_t count )
	{
		ui32_t i, j, simdCount;
		__m256 mat[ 4 ][ 3 ];

		// Fill the matrix from the Affine
		for( i = 0; i < 3; ++i ) {
			for( j = 0;j < 3; ++j ) {
				mat[ i ][ j ] = _mm256_set1_ps( trans.mat( j, i ) );
			}
		}
		mat[ 3 ][ 0 ] = _mm256_set1_ps( trans.vec[ 0 ] );
		mat[ 3 ][ 1 ] = _mm256_set1_ps( trans.vec[ 1 ] );
		mat[ 3 ][ 2 ] = _mm256_set1_ps( trans.vec[ 2 ] );

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
				__m256 xCorners = ( ( j & 1 ) != 0 ) ? bb._max[ 0 ] : bb._min[ 0 ];
				__m256 yCorners = ( ( j & 2 ) != 0 ) ? bb._max[ 1 ] : bb._min[ 1 ];
				__m256 zCorners = ( ( j & 4 ) != 0 ) ? bb._max[ 2 ] : bb._min[ 2 ];

				
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
			bb->_min[ 0 ] = xNewMinis;
			bb->_max[ 0 ] = xNewMaxes;
			bb->_min[ 1 ] = yNewMinis;
			bb->_max[ 1 ] = yNewMaxes;
			bb->_min[ 2 ] = zNewMinis;
			bb->_max[ 2 ] = zNewMaxes;
		}
	}
	void transform3D( AABB< __m256d, 3 > *out, const AABB< __m256d, 3 > *in, const Affine< f64_t, 3 > &trans, ui32_t count )
	{
		ui32_t i, j, simdCount;
		__m256d mat[ 4 ][ 3 ];

		// Fill the matrix from the Affine
		for( i = 0; i < 3; ++i ) {
			for( j = 0;j < 3; ++j ) {
				mat[ i ][ j ] = _mm256_set1_pd( trans.mat( j, i ) );
			}
		}
		mat[ 3 ][ 0 ] = _mm256_set1_pd( trans.vec[ 0 ] );
		mat[ 3 ][ 1 ] = _mm256_set1_pd( trans.vec[ 1 ] );
		mat[ 3 ][ 2 ] = _mm256_set1_pd( trans.vec[ 2 ] );

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
				__m256d xCorners = ( ( j & 1 ) != 0 ) ? bb._max[ 0 ] : bb._min[ 0 ];
				__m256d yCorners = ( ( j & 2 ) != 0 ) ? bb._max[ 1 ] : bb._min[ 1 ];
				__m256d zCorners = ( ( j & 4 ) != 0 ) ? bb._max[ 2 ] : bb._min[ 2 ];

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
			bb->_min[ 0 ] = xNewMinis;
			bb->_max[ 0 ] = xNewMaxes;
			bb->_min[ 1 ] = yNewMinis;
			bb->_max[ 1 ] = yNewMaxes;
			bb->_min[ 2 ] = zNewMinis;
			bb->_max[ 2 ] = zNewMaxes;
		}
	}
}

#endif