/*
 * Villains' Utility Library - Thomas Martin Schmid, 2014. Public domain¹
 *
 * This file describes an affine class, containing a matrix for the linear
 * part of an affine transformation and a vector describing the translation.
 * It also defines the application of the transformation on points and
 * vectors.
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
#ifndef VUL_AFFINE_HPP
#define VUL_AFFINE_HPP

#include "vul_types.hpp"
#include "vul_quaternion.hpp"
#include "vul_matrix.hpp"

/**
 * Define this for the c++11 version
 */
//#define VUL_CPLUSPLUS11

namespace vul {

	//----------------
	// Declarations
	//

	template< typename T, i32_t n >
	struct Vector;
	template< typename T, i32_t n >
	struct Point;
	template< typename T, i32_t m, i32_t n >
	struct Matrix;
	
	template< typename T, i32_t n >
	struct Affine { 
		Matrix< T, n, n > mat;
		Vector< T, n > vec;

#ifdef VUL_CPLUSPLUS11
		// Constructors
		/**
		 * Create an empty affine transformation. No translation 
		 * and an identity matrix are created.
		 */
		constexpr Affine< T, n >( );
		/**
		 * Create a copy of an affine transformation.
		 */
		explicit Affine< T, n >( const Affine< T, n > &a );
		/**
		 * Create an affine transformation from a rotation/scale matrix and a translation vector.
		 */
		Affine< T, n >( const Matrix< T, n, n > &mat, const Vector< T, n > &vec );
#endif
		Affine< T, n > &operator=( const Affine< T, n > &rhs );
	};

#ifndef VUL_CPLUSPLUS11
	/**
	 * Create an empty affine transformation. No translation 
	 * and an identity matrix are created.
	 */
	template< typename T, i32_t n >
	Affine< T, n > makeAffine( );
	/**
	 * Create a copy of an affine transformation.
	 */
	template< typename T, i32_t n >
	Affine< T, n > makeAffine( const Affine< T, n > &a );
	/**
	 * Create an affine transformation from a rotation/scale matrix and a translation vector.
	 */
	template< typename T, i32_t n >
	Affine< T, n > makeAffine( const Matrix< T, n, n > &mat, const Vector< T, n > &vec );	// Constructor from Mat<n,n> & Vec<n>.
#endif

	/**
	 * Apply an affine transformation to the point, including translation.
	 */
	template< typename T, i32_t n >
	Point< T, n > operator*( const Affine< T, n > &a, const Point< T, n> &p );
	/**
	 * Apply an affine transformation to the vector. This does not translate.
	 */
	template< typename T, i32_t n >
	Vector< T, n > operator*( const Affine< T, n > &a, const Vector< T, n> &p );

	/**
	 * Creates a 3D affine transformation to a 4x4 homogenous tronsformation matrix.
	 */
	template< typename T >
	Matrix< T, 4, 4 > makeHomogeneousFromAffine( const Affine< T, 3 > &a );
	/**
	 * Construct a 3D affine transformation from translation, scale and orientation bases.
	 */
	template< typename T >
	Affine< T, 3 > makeAffine3D( const Vector< T, 3 > &translation, const Vector< T, 3 > &scale, const Quaternion< T > &orientation );

	//---------------------------
	// Definitions
	//

#ifdef VUL_CPLUSPLUS11
	Affine::Affine< T, n >( )
	{
		mat = makeIdentity< T, n, n >( );
		vec = Vector< T, n >( );
	}

	Affine::Affine< T, n >( const Affine< T, n > &a )
	{
		mat = a.mat;
		vec = a.vec;
	}

	Affine::Affine< T, n >( const Matrix< T, n, n > &mat, const Vector< T, n > &vec )
	{
		this->mat = mat;
		this->vec = vec;
	}
#else
	template< typename T, i32_t n >
	Affine< T, n > *makeAffine( )
	{
		Affine< T, n > a;

		a.mat = makeIdentity< T, n, n >( );
		a.vec = makeVector< T, n >( );

		return a;
	}

	template< typename T, i32_t n >
	Affine< T, n > makeAffine( const Affine< T, n > &a )
	{
		Affine< T, n > *r;

		r.mat = a.mat;
		r.vec = a.vec;

		return r;
	}

	template< typename T, i32_t n >
	Affine< T, n > makeAffine( const Matrix< T, n, n > &mat, const Vector< T, n > &vec )
	{
		Affine< T, n > a;

		a.mat = mat;
		a.vec = vec;

		return a;
	}
#endif

	template< typename T, i32_t n >
	Affine< T, n > &Affine< T, n >::operator=( const Affine< T, n > &rhs )
	{
		mat = rhs.mat;
		vec = rhs.vec;
		return *this;
	}

	template< typename T, i32_t n >
	Point< T, n > operator*( const Affine< T, n > &a, const Point< T, n> &p )
	{
		Point< T, n > r;

		r = ( p * a.mat ) + a.vec;

		return r;
	}

	template< typename T, i32_t n >
	Vector< T, n > operator*( const Affine< T, n > &a, const Vector< T, n> &p )
	{
		Vector< T, n > v;

		v = p * a.mat;

		return v;
	}

	template< typename T >
	Matrix< T, 4, 4 > makeHomogeneousFromAffine( const Affine< T, 3 > &a )
	{
		Matrix< T, 4, 4 > m;
		T d[ 4 ][ 4 ];

		d[ 0 ][ 0 ] = a.mat( 0, 0 );
		d[ 1 ][ 0 ] = a.mat( 1, 0 );
		d[ 2 ][ 0 ] = a.mat( 2, 0 );
		d[ 3 ][ 0 ] = a.vec[ 0 ];
		d[ 0 ][ 1 ] = a.mat( 0, 1 );
		d[ 1 ][ 1 ] = a.mat( 1, 1 );
		d[ 2 ][ 1 ] = a.mat( 2, 1 );
		d[ 3 ][ 1 ] = a.vec[ 1 ];
		d[ 0 ][ 2 ] = a.mat( 0, 2 );
		d[ 1 ][ 2 ] = a.mat( 1, 2 );
		d[ 2 ][ 2 ] = a.mat( 2, 2 );
		d[ 3 ][ 2 ] = a.vec[ 2 ];
		d[ 0 ][ 3 ] = static_cast< T >( 0 );
		d[ 1 ][ 3 ] = static_cast< T >( 0 );
		d[ 2 ][ 3 ] = static_cast< T >( 0 );
		d[ 3 ][ 3 ] = static_cast< T >( 1 );

#ifndef VUL_CPLUSPLUS11
		m = makeMatrix( d );
#else
		m = new Matrix< T, n >( d );
#endif
		return m;
	}
	template< typename T >
	Affine< T, 3 > makeAffine3D( const Vector< T, 3 > &translation, const Vector< T, 3 > &scale, const Quaternion< T > &orientation )
	{
		Affine< T, 3 > a;
		i32_t i, j;
		
		// Copy translation straight
		a.vec = translation;
		// Build rotation matrix ( inverse of the orientation )
		a.mat = makeMatrix( inverse( orientation ) );
		// Multiply in scale
		for( i = 0; i < 3; ++i ) {
			for( j = 0; j < 3; ++j ) {
				a.mat( i, j ) = a.mat( i, j ) * scale[ j ];
			}
		}

		return a;
	}

}

#endif