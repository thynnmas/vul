/*
 * Villains' Utility Library - Thomas Martin Schmid, 2017. Public domain¹
 *
 * A matrix math library for generic matrices. Format inspired by 
 * http://www.reedbeta.com/blog/2013/12/28/on-vector-math-libraries/
 * Specializations for 2x2, 3x3 & 4x4 matrices of all reasonable float,
 * fixed, int and uint types (not 128bit) are made. Interfaces with
 * the vectors of vul_vector.h
 *
 * The matrices are scalar. Vectors are column vectors!
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

#ifndef VUL_MATRIX_HPP
#define VUL_MATRIX_HPP

#ifndef VUL_MATH_ASSERT
#include <assert.h>
#define VUL_MATH_ASSERT assert
#endif

#include "vul_types.hpp"

namespace vul {
	
	//----------------
	// Declarations
	// 

	template< typename T, s32 n >
	struct Vector;

	// Generic definition
	template< typename T, s32 cols, s32 rows >
	struct Matrix {
		// Data
		T data[ cols ][ rows ];	// Column major

#ifdef VUL_CPLUSPLUS11
		// Constructor
		explicit Matrix< T, cols, rows >( );									// Empty constructor
		explicit Matrix< T, cols, rows >( T val );								// Initialize to a single value
		Matrix< T, cols, rows >( const Matrix< T, cols, rows > &m );			// Copy constructor
		explicit Matrix< T, cols, rows >( T (& a)[ cols ][ rows ] ); 			// Generic array constructor
		/**
		 * Initializer list takes values row-major. This is purely because it makes more sense
		 * to write Matrix< T, 2, 2 > mat{ 1, 2,					[ 1 2 ]		  mat{ 1, 3,
										   3, 4 } for the matrix	[ 3 4 ] than       2, 4 }
		*/
		explicit Matrix< T, cols, rows >( std::initializer_list<T> list );		// From initializer list. Componentwise init is non-c++11 equivalent.
#endif	
		// Operators
		/**
		 * Copy assignment operator
		 */
		Matrix< T, cols, rows >& operator=( const Matrix< T, cols, rows > & rhs );
		/**
		 * Componentwise addition.
		 */
		Matrix< T, cols, rows >& operator+=( T scalar );
		/**
		 * Componentwise subtraction.
		 */
		Matrix< T, cols, rows >& operator-=( T scalar );
		/**
		 * Componentwise multiplication.
		 */
		Matrix< T, cols, rows >& operator*=( T scalar );
		/**
		 * Componentwise division.
		 */
		Matrix< T, cols, rows >& operator/=( T scalar );

		/**
		 * Componentwise addition of two matrices.
		 */
		Matrix< T, cols, rows >& operator+=( const Matrix< T, cols, rows > &rhs );
		/**
		 * Componentwise subtraction of two matrices.
		 */
		Matrix< T, cols, rows >& operator-=( const Matrix< T, cols, rows > &rhs );
		/**
		 * Componentwise negation.
		 */
		Matrix< T, cols, rows > operator-( ) const;
		

		/**
		 * Indexing operator.
		 */
		T &operator( )( s32 c, s32 r );
		/**
		 * Constant indexing operator.
		 */
		T const &operator( )( s32 c, s32 r ) const;
	};

	// Typedefs
	typedef Matrix< f16, 2, 2 > f16x22;
	typedef Matrix< f32, 2, 2 > f32x22;
	typedef Matrix< f64, 2, 2 > f64x22;
	typedef Matrix< f16, 3, 3 > f16x33;
	typedef Matrix< f32, 3, 3 > f32x33;
	typedef Matrix< f64, 3, 3 > f64x33;
	typedef Matrix< f16, 4, 4 > f16x44;
	typedef Matrix< f32, 4, 4 > f32x44;
	typedef Matrix< f64, 4, 4 > f64x44;

	typedef Matrix< fi32, 2, 2 > fi32x22;
	typedef Matrix< fi32, 3, 3 > fi32x33;
	typedef Matrix< fi32, 4, 4 > fi32x44;

	typedef Matrix< s8, 2, 2 > s8x22;
	typedef Matrix< s16, 2, 2 > s16x22;
	typedef Matrix< s32, 2, 2 > s32x22;
	typedef Matrix< s64, 2, 2 > s64x22;
	typedef Matrix< s8, 3, 3 > s8x33;
	typedef Matrix< s16, 3, 3 > s16x33;
	typedef Matrix< s32, 3, 3 > s32x33;
	typedef Matrix< s64, 3, 3 > s64x33;
	typedef Matrix< s8, 4, 4 > s8x44;
	typedef Matrix< s16, 4, 4 > s16x44;
	typedef Matrix< s32, 4, 4 > s32x44;
	typedef Matrix< s64, 4, 4 > s64x44;

	typedef Matrix< u8, 2, 2 > u8x22;
	typedef Matrix< u16, 2, 2 > u16x22;
	typedef Matrix< u32, 2, 2 > u32x22;
	typedef Matrix< u64, 2, 2 > u64x22;
	typedef Matrix< u8, 3, 3 > u8x33;
	typedef Matrix< u16, 3, 3 > u16x33;
	typedef Matrix< u32, 3, 3 > u32x33;
	typedef Matrix< u64, 3, 3 > u64x33;
	typedef Matrix< u8, 4, 4 > u8x44;
	typedef Matrix< u16, 4, 4 > u16x44;
	typedef Matrix< u32, 4, 4 > u32x44;
	typedef Matrix< u64, 4, 4 > u64x44;

	// Non-member constructors for pre C++11 versions
#ifndef VUL_CPLUSPLUS11
	template< typename T, s32 cols, s32 rows >
	Matrix< T, cols, rows > makeMatrix( );								// Empty constructor
	template< typename T, s32 cols, s32 rows >
	Matrix< T, cols, rows > makeMatrix( T val );						// Initialize to a single value
	template< typename T, s32 cols, s32 rows >
	Matrix< T, cols, rows > makeMatrix( const Matrix< T, cols, rows > &m );	// Copy constructor
	template< typename T, s32 cols, s32 rows >
	Matrix< T, cols, rows > makeMatrix( T (& a)[ cols ][ rows ] ); 		// Generic array constructor
#endif		
	template< typename T, s32 cols, s32 rows >
	Matrix< T, cols, rows > makeMatrix( f32 (& a)[ rows * cols ] );	// From float array, to interface with other libraries
	template< typename T, s32 cols, s32 rows >
	Matrix< T, cols, rows > makeMatrix( s32 (& a)[ rows * cols ] ); 	// From int array, to interface with other libraries
	
	template< typename T >
	Matrix< T, 2, 2 > makeMatrix22( T c1r1, T c2r1, 
									T c1r2, T c2r2 );	// Componentwise
	template< typename T >
	Matrix< T, 3, 3 > makeMatrix33( T c1r1, T c2r1, T c3r1,
									T c1r2, T c2r2, T c3r2,
									T c1r3, T c2r3, T c3r3 );
	template< typename T >
	Matrix< T, 4, 4 > makeMatrix44( T c1r1, T c2r1, T c3r1, T c4r1, 
									T c1r2, T c2r2, T c3r2, T c4r2,
									T c1r3, T c2r3, T c3r3, T c4r3,
									T c1r4, T c2r4, T c3r4, T c4r4 );
	template< typename T >
	Matrix< T, 2, 2 > makeMatrix22FromColumns( const Vector< T, 2 > &c1,
											   const Vector< T, 2 > &c2 );
	template< typename T >
	Matrix< T, 3, 3 > makeMatrix33FromColumns( const Vector< T, 3 > &c1,
											   const Vector< T, 3 > &c2,
											   const Vector< T, 3 > &c3   );
	template< typename T >
	Matrix< T, 4, 4 > makeMatrix44FromColumns( const Vector< T, 4 > &c1,
											   const Vector< T, 4 > &c2,
											   const Vector< T, 4 > &c3,
											   const Vector< T, 4 > &c4 );
	
	template< typename T, s32 n >
	Matrix< T, n, n > makeIdentity( );
	template< typename T, s32 cols, s32 rows >
	Matrix< T, cols, rows > makeMatrixFromRows( const Vector< T, cols > r[ rows ] );
	template< typename T, s32 cols, s32 rows >
	Matrix< T, cols, rows > makeMatrixFromColumns( const Vector< T, rows > c[ cols ] );
	template< typename T >
	Matrix< T, 3, 3 > makeMatrix33FromEuler( const Vector< T, 3 > &angles );
	/**
	 * Copies the top left coloXrowo matrix of mat into a new matrix of size
	 * colXrown, essentially truncating the top left submatrix of mat into a
	 * new, smaller matrix.
	 */
	template< typename T, s32 coln, s32 rown, s32 colo, s32 rowo >
	Matrix< T, coln, rown > truncate( const Matrix< T, colo, rowo > &mat );
	/**
	 * Copies the contents of matrix src (sized srcc X srcr) into the top left 
	 * ( srcc X srcr ) submatrix of dst. Dst must be at least as big as src.
	 */
	template< typename T, s32 dstc, s32 dstr, s32 srcc, s32 srcr >
	void copy( Matrix< T, dstc, dstr > *dst, const Matrix< T, srcc, srcr > &src );
	/**
	 * Copies the contents of matrix src (sized srcc X srcr) into the 
	 * ( srcc X srcr ) submatrix of dst starting at column firstcol and row
	 * firstrow. Dst must be at least as big as <srcc + firstcol, srcr + firstrow>.
	 */
	template< typename T, s32 dstc, s32 dstr, s32 srcc, s32 srcr >
	void copy( Matrix< T, dstc, dstr > *dst, const Matrix< T, srcc, srcr > &src, s32 firstcol, s32 firstrow );

	// Operations
	/**
	 * Componentwise comparison. Returns a matrix of bools indicating if the compnents 
	 * in the corresponding position are equal.
	 */
	template< typename T, s32 cols, s32 rows >
	Matrix< bool, cols, rows > operator==( const Matrix< T, cols, rows > &a, const Matrix< T, cols, rows > &b );
	/**
	 * Componentwise comparison. Returns a matrix of bools indicating if the compnents 
	 * in the corresponding position are not equal.
	 */
	template< typename T, s32 cols, s32 rows >
	Matrix< bool, cols, rows > operator!=( const Matrix< T, cols, rows > &a, const Matrix< T, cols, rows > &b );

	/**
	 * Returns true if all compnents are true. Is valid for any type if( T ) is valid.
	 * Equivalent of AND-ing together all compnents.
	 */
	template< typename T, s32 cols, s32 rows >
	bool all( const Matrix< T, cols, rows > &mat );
	/**
	 * Returns true if any compnents is true. Is valid for any type if( T ) is valid.
	 * Equivalent of OR-ing together all compnents.
	 */
	template< typename T, s32 cols, s32 rows >
	bool any( const Matrix< T, cols, rows > &mat );
	/**
	 * Returns the first compnent that is evaluated to true by if( T ).
	 * Equivalent to the ?: selector.
	 * Operates row major; mat_row0_column1 comes before mat_row1_column0.
	 */
	template< typename T, s32 cols, s32 rows >
	T select( const Matrix< T, cols, rows > &mat ); 

	/**
	 * Componentwise addition.
	 */
	template< typename T, s32 cols, s32 rows >
	Matrix< T, cols, rows > operator+( const Matrix< T, cols, rows >& mat, T scalar );
	/**
	 * Componentwise subtraction.
	 */
	template< typename T, s32 cols, s32 rows >
	Matrix< T, cols, rows > operator-( const Matrix< T, cols, rows >& mat, T scalar );
	/**
	 * Componentwise multiplication.
	 */
	template< typename T, s32 cols, s32 rows >
	Matrix< T, cols, rows > operator*( const Matrix< T, cols, rows >& mat, T scalar );
	/**
	 * Componentwise division.
	 */
	template< typename T, s32 cols, s32 rows >
	Matrix< T, cols, rows > operator/( const Matrix< T, cols, rows >& mat, T scalar );

	/**
	 * Componentwise addition of matrices.
	 */
	template< typename T, s32 cols, s32 rows >
	Matrix< T, cols, rows > operator+( const Matrix< T, cols, rows >& a, const Matrix< T, cols, rows > &b ); // Componentwise addition
	/**
	 * Componentwise subtraction of matrices.
	 */
	template< typename T, s32 cols, s32 rows >
	Matrix< T, cols, rows > operator-( const Matrix< T, cols, rows >& a, const Matrix< T, cols, rows > &b ); // Componentwise subtraction
	
	/**
	 * Componentwise min( compnent, b ).
	 */
	template< typename T, s32 cols, s32 rows >
	Matrix< T, cols, rows > min( const Matrix< T, cols, rows > &a, T b );
	/**
	 * Componentwise max( compnent, b ).
	 */
	template< typename T, s32 cols, s32 rows >
	Matrix< T, cols, rows > max( const Matrix< T, cols, rows > &a, T b );
	/**
	 * Componentwise absolute value.
	 */
	template< typename T, s32 cols, s32 rows >
	Matrix< T, cols, rows > abs( const Matrix< T, cols, rows > &a );
	/**
	 * Compomentwise clamp( x, min, max ).
	 */
	template< typename T, s32 cols, s32 rows >
	Matrix< T, cols, rows > clamp( const Matrix< T, cols, rows > &a, T mini, T maxi );
	/**
	 * Compomentwise saturate, so clamp( x, 0, 1 ).
	 */
	template< typename T, s32 cols, s32 rows >
	Matrix< T, cols, rows > saturate( const Matrix< T, cols, rows > &a );
	/** 
	 * Componentwise linear interpolation based on t.
	 */
	template< typename T, s32 cols, s32 rows, typename T_t >
	Matrix< T, cols, rows > lerp( const Matrix< T, cols, rows > &mini, const Matrix< T, cols, rows > &maxi, T_t t );
	/**
	 * Returns the smalles compnent.
	 */
	template< typename T, s32 cols, s32 rows >
	T minComponent( const Matrix< T, cols, rows > &a );
	/**
	 * Returns the largest compnent.
	 */
	template< typename T, s32 cols, s32 rows >
	T maxComponent( const Matrix< T, cols, rows > &a );
	
	/**
	 * Matrix multiplication. This is NOT componentwise, but proper matrix mul.
	 */
	template< typename T, s32 cola, s32 shared, s32 rowb >
	Matrix< T, cola, rowb > operator*( const Matrix< T, cola, shared > &a, const Matrix< T, shared, rowb > &b );
	
	/** 
	 * Right-side multiplication of a matrix and a column vector.
	 */
	template< typename T, s32 cols, s32 rows >
	Vector< T, rows > operator*( const Matrix< T, cols, rows >& mat, const Vector< T, cols > &vec );
	/** 
	 * Left-side multiplication of a matrix and a column vector.
	 */
	template< typename T, s32 cols, s32 rows >
	Vector< T, cols > operator*( const Vector< T, rows > &vec , const Matrix< T, cols, rows >& mat);	
	/** 
	 * Right-side multiplication of a matrix and a point interpreted as a column vector.
	 */
	template< typename T, s32 cols, s32 rows >
	Point< T, rows > operator*( const Matrix< T, cols, rows >& mat, const Point< T, cols > &vec );
	/** 
	 * Left-side multiplication of a matrix and a point interpreted as a column vector.
	 */
	template< typename T, s32 cols, s32 rows >
	Point< T, cols > operator*( const Point< T, rows > &vec , const Matrix< T, cols, rows >& mat);
	
	// Functions
	/**
	 * Returns the inverse of the matrix.
	 */
	template< typename T, s32 n >
	Matrix< T, n, n > inverse( const Matrix< T, n, n > &mat );
	/**
	 * Returns the transpose of the matrix.
	 */
	template< typename T, s32 cols, s32 rows >
	Matrix< T, cols, rows > transpose( const Matrix< T, cols, rows > &mat );
	
	/**
	 * Calculates the determinant of the matrix. This is the 2x2 special case.
	 */
	template< typename T >
	f32 determinant( const Matrix< T, 2, 2 > &mat );
	/**
	 * Calculates the determinant of the matrix. This is the 3x3 special case.
	 */
	template< typename T >
	f32 determinant( const Matrix< T, 3, 3 > &mat );
	/**
	 * Calculates the determinant of the matrix. Recursively simplifies down to
	 * the 2x2 case. Uses vul_matrix_find_most_zero at every level to recurse down
	 * the path with the least sub-expressions at that level.
    * @TODO(thynn): Write 3x3 specialization, probably 4x4 as well!
	 */
	template< typename T, s32 cols, s32 rows >
	f32 determinant( const Matrix< T, cols, rows > &mat );

	/** 
	 * Returns the column at index n in the given matrix.
	 */
	template< typename T, s32 cols, s32 rows >
	Vector< T, rows > column( const Matrix< T, cols, rows > &mat, s32 n );
	/** 
	 * Returns the row at index n in the given matrix.
	 */
	template< typename T, s32 cols, s32 rows >
	Vector< T, cols > row( const Matrix< T, cols, rows > &mat, s32 n );
   /**
    * Returns the Euler angles from the given 3x3 matrix
    */
    template< typename T >
    Vector< T, 3 > euler_angles( const Matrix< T, 3, 3 > &mat );
	/**
    * Returns an orthonormal basis constructed from a normal.
    */
    template< typename T >
    Matrix< T, 3, 3 > orthonormal_basis( const Vector< T, 3 > &n );
   

	/**
	 * Describes the type of column or row that contains the most zeroes.
	 * Used by vul_matrix_zero_helper.
	 */
	enum vul_matrix_zero_type {
		VUL_MATRIX_COLUMN = 0,
		VUL_MATRIX_ROW = 1
	};
	/**
	 * The result of vul_matrix_find_most_zero, this describes the column or
	 * row with the most zeroes in a matrix.
	 */
	struct vul_matrix_zero_helper {
		vul_matrix_zero_type type;
		s32 index;
		s32 count;
	};
	/** 
	 * Helper function of detereminant.
	 * Counts the number of zeros in each column and row. 
	 * Returns a vul_matrix_zero_helper with row/column type, index and count
	 */
	template< typename T, s32 cols, s32 rows >
	vul_matrix_zero_helper vul_matrix_find_most_zero( const Matrix< T, cols, rows > &mat );
	
	//---------------------------
	// Definitions
	//
	
#ifdef VUL_CPLUSPLUS11
	// Constructor
	template< typename T, s32 cols, s32 rows >
	Matrix< T, cols, rows >::Matrix( )
	{
		s32 i, j;
		for( i = 0; i < cols; ++i ) {
			for( j = 0; j < rows; ++j ) {
				data[ i ][ j ] = static_cast< T >( 0.f );
			}
		}
	}
	template< typename T, s32 cols, s32 rows >
	Matrix< T, cols, rows >::Matrix( T val )
	{
		s32 i, j;
		for( i = 0; i < cols; ++i ) {
			for( j = 0; j < rows; ++j ) {
				data[ i ][ j ] = val;
			}
		}
	}
	template< typename T, s32 cols, s32 rows >
	Matrix< T, cols, rows >::Matrix( const Matrix< T, cols, rows > &m )
	{
		s32 i, j;
		for( i = 0; i < cols; ++i ) {
			for( j = 0; j < rows; ++j ) {
				data[ i ][ j ] = m( i, j );
			}
		}
	}
	template< typename T, s32 cols, s32 rows >
	Matrix< T, cols, rows >::Matrix( T (& a)[ cols ][ rows ] )
	{
		memcpy( data, a, sizeof( T ) * rows * cols );
	}
	template< typename T, s32 cols, s32 rows >
	Matrix< T, cols, rows >::Matrix( std::initializer_list< T > list )
	{
		s32 i;
		typename std::initializer_list< T >::iterator it;

		for( it = list.begin( ), i = 0; it != list.end( ) && i < cols * rows; ++it, ++i ) {
			data[ i % rows ][ i / rows ] = *it;
		}
	}
#else
	template< typename T, s32 cols, s32 rows >
	Matrix< T, cols, rows > makeMatrix( )
	{
		Matrix< T, cols, rows > m;
		s32 i, j;

		for( i = 0; i < cols; ++i ) {
			for( j = 0; j < rows; ++j ) {
				m.data[ i ][ j ] = static_cast< T >( 0.f );
			}
		}

		return m;
	}
	
	template< typename T, s32 cols, s32 rows >
	Matrix< T, cols, rows > makeMatrix( T val )
	{
		Matrix< T, cols, rows > m;
		s32 i, j;

		for( i = 0; i < cols; ++i ) {
			for( j = 0; j < rows; ++j ) {
				m.data[ i ][ j ] = val;
			}
		}

		return m;
	}
	
	template< typename T, s32 cols, s32 rows >
	Matrix< T, cols, rows > makeMatrix( const Matrix< T, cols, rows > &m )
	{
		Matrix< T, cols, rows > r;
		s32 i, j;
		
		for( i = 0; i < cols; ++i ) {
			for( j = 0; j < rows; ++j ) {
				r.data[ i ][ j ] = m.data[ i ][ j ];
			}
		}

		return r;
	}	
	template< typename T, s32 cols, s32 rows >
	Matrix< T, cols, rows > makeMatrix( T (& a)[ cols ][ rows ] )
	{
		Matrix< T, cols, rows > m;
		s32 i, j;

		for( i = 0; i < cols; ++i ) {
			for( j = 0; j < rows; ++j ) {
				m.data[ i ][ j ] = a[ i ][ j ];
			}
		}

		return m;
	}	
#endif

	template< typename T >
	Matrix< T, 2, 2 > makeMatrix22( T c1r1, T c2r1,
									T c1r2, T c2r2 )
	{
		Matrix< T, 2, 2 > m;

		m.data[ 0 ][ 0 ] = c1r1;
		m.data[ 0 ][ 1 ] = c1r2;
		m.data[ 1 ][ 0 ] = c2r1;
		m.data[ 1 ][ 1 ] = c2r2;

		return m;
	}
	template< typename T >
	Matrix< T, 3, 3 > makeMatrix33( T c1r1, T c2r1, T c3r1,
									T c1r2, T c2r2, T c3r2,
									T c1r3, T c2r3, T c3r3 )
	{
		Matrix< T, 3, 3 > m;

		m.data[ 0 ][ 0 ] = c1r1;
		m.data[ 0 ][ 1 ] = c1r2;
		m.data[ 0 ][ 2 ] = c1r3;
		m.data[ 1 ][ 0 ] = c2r1;
		m.data[ 1 ][ 1 ] = c2r2;
		m.data[ 1 ][ 2 ] = c2r3;
		m.data[ 2 ][ 0 ] = c3r1;
		m.data[ 2 ][ 1 ] = c3r2;
		m.data[ 2 ][ 2 ] = c3r3;

		return m;
	}
	template< typename T >
	Matrix< T, 4, 4 > makeMatrix44( T c1r1, T c2r1, T c3r1, T c4r1, 
									T c1r2, T c2r2, T c3r2, T c4r2,
									T c1r3, T c2r3, T c3r3, T c4r3,
									T c1r4, T c2r4, T c3r4, T c4r4 )
	{
		Matrix< T, 4, 4 > m;

		m.data[ 0 ][ 0 ] = c1r1;
		m.data[ 0 ][ 1 ] = c1r2;
		m.data[ 0 ][ 2 ] = c1r3;
		m.data[ 0 ][ 3 ] = c1r4;
		m.data[ 1 ][ 0 ] = c2r1;
		m.data[ 1 ][ 1 ] = c2r2;
		m.data[ 1 ][ 2 ] = c2r3;
		m.data[ 1 ][ 3 ] = c2r4;
		m.data[ 2 ][ 0 ] = c3r1;
		m.data[ 2 ][ 1 ] = c3r2;
		m.data[ 2 ][ 2 ] = c3r3;
		m.data[ 2 ][ 3 ] = c3r4;
		m.data[ 3 ][ 0 ] = c4r1;
		m.data[ 3 ][ 1 ] = c4r2;
		m.data[ 3 ][ 2 ] = c4r3;
		m.data[ 3 ][ 3 ] = c4r4;

		return m;
	}

	template< typename T, s32 cols, s32 rows >
	Matrix< T, cols, rows > makeMatrix( f32 (& a)[ rows * cols ] )
	{
		Matrix< T, cols, rows > m;
		s32 i, j;

		for( i = 0; i < cols; ++i ) {
			for( j = 0; j < rows; ++j ) {
				m.data[ i ][ j ] = static_cast< T >( a[ i * cols + j ] );
			}
		}
		
		return m;
	}	
	template< typename T, s32 cols, s32 rows >
	Matrix< T, cols, rows > makeMatrix( s32 (& a)[ rows * cols ] )
	{
		Matrix< T, cols, rows > m;
		s32 i, j;

		for( i = 0; i < cols; ++i ) {
			for( j = 0; j < rows; ++j ) {
				m.data[ i ][ j ] = static_cast< T >( a[ i * cols + j ] );
			}
		}
		
		return m;
	}
	
	template< typename T >
	Matrix< T, 2, 2 > makeMatrix22FromColumns( const Vector< T, 2 > &c1,
											   const Vector< T, 2 > &c2 )
	{
		Matrix< T, 2, 2 > m;

		m.data[ 0 ][ 0 ] = c1.data[ 0 ];
		m.data[ 0 ][ 1 ] = c1.data[ 1 ];
		m.data[ 1 ][ 0 ] = c2.data[ 0 ];
		m.data[ 1 ][ 1 ] = c2.data[ 1 ];

		return m;
	}
	template< typename T >
	Matrix< T, 3, 3 > makeMatrix33FromColumns( const Vector< T, 3 > &c1,
											   const Vector< T, 3 > &c2,
											   const Vector< T, 3 > &c3   )
	{
		Matrix< T, 3, 3 > m;

		m.data[ 0 ][ 0 ] = c1.data[ 0 ];
		m.data[ 0 ][ 1 ] = c1.data[ 1 ];
		m.data[ 0 ][ 2 ] = c1.data[ 2 ];
		m.data[ 1 ][ 0 ] = c2.data[ 0 ];
		m.data[ 1 ][ 1 ] = c2.data[ 1 ];
		m.data[ 1 ][ 2 ] = c2.data[ 2 ];
		m.data[ 2 ][ 0 ] = c3.data[ 0 ];
		m.data[ 2 ][ 1 ] = c3.data[ 1 ];
		m.data[ 2 ][ 2 ] = c3.data[ 2 ];

		return m;
	}
	template< typename T >
	Matrix< T, 4, 4 > makeMatrix44FromColumns( const Vector< T, 4 > &c1,
											   const Vector< T, 4 > &c2,
											   const Vector< T, 4 > &c3,
											   const Vector< T, 4 > &c4 )
	{
		Matrix< T, 4, 4 > m;

		
		m.data[ 0 ][ 0 ] = c1.data[ 0 ];
		m.data[ 0 ][ 1 ] = c1.data[ 1 ];
		m.data[ 0 ][ 2 ] = c1.data[ 2 ];
		m.data[ 0 ][ 3 ] = c1.data[ 3 ];
		m.data[ 1 ][ 0 ] = c2.data[ 0 ];
		m.data[ 1 ][ 1 ] = c2.data[ 1 ];
		m.data[ 1 ][ 2 ] = c2.data[ 2 ];
		m.data[ 1 ][ 3 ] = c2.data[ 3 ];
		m.data[ 2 ][ 0 ] = c3.data[ 0 ];
		m.data[ 2 ][ 1 ] = c3.data[ 1 ];
		m.data[ 2 ][ 2 ] = c3.data[ 2 ];
		m.data[ 2 ][ 3 ] = c3.data[ 3 ];
		m.data[ 3 ][ 0 ] = c4.data[ 0 ];
		m.data[ 3 ][ 1 ] = c4.data[ 1 ];
		m.data[ 3 ][ 2 ] = c4.data[ 2 ];
		m.data[ 3 ][ 3 ] = c4.data[ 3 ];

		return m;
	}
	template< typename T, s32 n >
	Matrix< T, n, n > makeIdentity( )
	{
		Matrix< T, n, n > m;
		s32 i;

#ifdef VUL_CPLUSPLUS11
		m = Matrix< T, n, n >( static_cast< T >( 0.f ) );
#else
		m = makeMatrix< T, n, n >( static_cast< T >( 0.f ) );
#endif
		for( i = 0; i < n; ++i ) {
			m.data[ i ][ i ] = static_cast< T >( 1 );
		}

		return m;
	}
	template< typename T, s32 cols, s32 rows >
	Matrix< T, cols, rows > makeMatrixFromRows( const Vector< T, cols > r[ rows ] )
	{
		Matrix< T, cols, rows > m;
		s32 i, j;

		for( i = 0; i < cols; ++i ) {
			for( j = 0; j < rows; ++j ) {
				m.data[ i ][ j ] = r[ j ].data[ i ];
			}
		}

		return m;
	}
	template< typename T, s32 cols, s32 rows >
	Matrix< T, cols, rows > makeMatrixFromColumns( const Vector< T, rows > c[ cols ] )
	{
		Matrix< T, cols, rows > m;
		s32 i, j;

		for( i = 0; i < cols; ++i ) {
			for( j = 0; j < rows; ++j ) {
				m.data[ i ][ j ] = c[ i ].data[ j ];
			}
		}

		return m;
	}
	template< typename T >
	Matrix< T, 3, 3 > makeMatrix33FromEuler( const Vector< T, 3 > &angles )
   {
      Matrix< T, 3, 3 > m;

      T cy = cos( angles[ 0 ] );
      T sy = sin( angles[ 0 ] );
      T cr = cos( angles[ 1 ] );
      T sr = sin( angles[ 1 ] );
      T cp = cos( angles[ 2 ] );
      T sp = sin( angles[ 2 ] );

      m.data[ 0 ][ 0 ] = cy * cr;
      m.data[ 0 ][ 1 ] = sy * sp - cy * sr * cp;
      m.data[ 0 ][ 2 ] = cy * sr * sp + sy * cp;
      m.data[ 1 ][ 0 ] = sr;
      m.data[ 1 ][ 1 ] = cr * cp;
      m.data[ 1 ][ 2 ] = -cr * sp;
      m.data[ 2 ][ 0 ] = -sy * cr;
      m.data[ 2 ][ 1 ] = sy * sr * cp + cy * sp;
      m.data[ 2 ][ 2 ] = -sy * sr * sp + cy * cp;

      return m;
   }
	template< typename T, s32 coln, s32 rown, s32 colo, s32 rowo >
	Matrix< T, coln, rown > truncate( const Matrix< T, colo, rowo > &mat )
	{
		Matrix< T, coln, rown > m;
		s32 i, j;

		VUL_MATH_ASSERT( colo >= coln && rowo >= rown );

		for( i = 0; i < coln; ++i ) {
			for( j = 0; j < rown; ++j ) {
				m.data[ i ][ j ] = mat.data[ i ][ j ];
			}
		}

		return m;
	}
	template< typename T, s32 dstc, s32 dstr, s32 srcc, s32 srcr >
	void copy( Matrix< T, dstc, dstr > *dst, const Matrix< T, srcc, srcr > &src )
	{
		s32 i, j;

		VUL_MATH_ASSERT( dstc >= srcc && dstr >= srcr );

		for( i = 0; i < srcc; ++i ) {
			for( j = 0; j < srcr; ++j ) {
				dst->data[ i ][ j ] = src.data[ i ][ j ];
			}
		}
	}
	template< typename T, s32 dstc, s32 dstr, s32 srcc, s32 srcr >
	void copy( Matrix< T, dstc, dstr > *dst, const Matrix< T, srcc, srcr > &src, s32 firstcol, s32 firstrow )
	{
		s32 i, j;

		VUL_MATH_ASSERT( dstc >= srcc + firstcol && dstr >= srcr + firstrow );

		for( i = 0; i < srcc; ++i ) {
			for( j = 0; j < srcr; ++j ) {
				dst->data[ i + firstcol ][ j + firstrow ] = src.data[ i ][ j ];
			}
		}
	}

	// Operators
	template< typename T, s32 cols, s32 rows >
	Matrix< T, cols, rows >& Matrix< T, cols, rows >::operator=( const Matrix< T, cols, rows > & rhs )
	{
		s32 i, j;
		
		for( i = 0; i < cols; ++i ) {
			for( j = 0; j < rows; ++j ) {
				data[ i ][ j ] = rhs.data[ i ][ j ];
			}
		}

		return *this;
	}
	template< typename T, s32 cols, s32 rows >
	Matrix< T, cols, rows > &Matrix< T, cols, rows >::operator+=( T scalar )
	{
		s32 i, j;

		for( i = 0; i < cols; ++i ) {
			for( j = 0; j < rows; ++j ) {
				data[ i ][ j ] += scalar;
			}
		}

		return *this;
	}
	template< typename T, s32 cols, s32 rows >
	Matrix< T, cols, rows > &Matrix< T, cols, rows >::operator-=( T scalar )
	{
		s32 i, j;

		for( i = 0; i < cols; ++i ) {
			for( j = 0; j < rows; ++j ) {
				data[ i ][ j ] -= scalar;
			}
		}

		return *this;
	}
	template< typename T, s32 cols, s32 rows >
	Matrix< T, cols, rows > &Matrix< T, cols, rows >::operator*=( T scalar )
	{
		s32 i, j;

		for( i = 0; i < cols; ++i ) {
			for( j = 0; j < rows; ++j ) {
				data[ i ][ j ] *= scalar;
			}
		}

		return *this;
	}
	template< typename T, s32 cols, s32 rows >
	Matrix< T, cols, rows > &Matrix< T, cols, rows >::operator/=( T scalar )
	{
		s32 i, j;

		for( i = 0; i < cols; ++i ) {
			for( j = 0; j < rows; ++j ) {
				data[ i ][ j ] /= scalar;
			}
		}

		return *this;
	}

	template< typename T, s32 cols, s32 rows >
	Matrix< T, cols, rows > &Matrix< T, cols, rows >::operator+=( const Matrix< T, cols, rows > &rhs )
	{
		s32 i, j;

		for( i = 0; i < cols; ++i ) {
			for( j = 0; j < rows; ++j ) {
				data[ i ][ j ] += rhs.data[ i ][ j ];
			}
		}

		return *this;
	}
	template< typename T, s32 cols, s32 rows >
	Matrix< T, cols, rows > &Matrix< T, cols, rows >::operator-=( const Matrix< T, cols, rows > &rhs )
	{
		s32 i, j;

		for( i = 0; i < cols; ++i ) {
			for( j = 0; j < rows; ++j ) {
				data[ i ][ j ] -= rhs.data[ i ][ j ];
			}
		}

		return *this;
	}
	template< typename T, s32 cols, s32 rows >
	Matrix< T, cols, rows > Matrix< T, cols, rows >::operator-( ) const
	{
		Matrix< T, cols, rows > m;
		s32 i, j;
		
		for( i = 0; i < cols; ++i ) {
			for( j = 0; j < rows; ++j ) {
				m.data[ i ][ j ] = -data[ i ][ j ];
			}
		}

		return m;
	}

	template< typename T, s32 cols, s32 rows >
	T &Matrix< T, cols, rows >::operator( )( s32 i, s32 j )
	{
		VUL_MATH_ASSERT( i < cols );
		VUL_MATH_ASSERT( j < rows );
		return data[ i ][ j ];
	}
		
	template< typename T, s32 cols, s32 rows >
	T const &Matrix< T, cols, rows >::operator( )( s32 i, s32 j ) const
	{
		VUL_MATH_ASSERT( i < cols );
		VUL_MATH_ASSERT( j < rows );
		return data[ i ][ j ];
	}

	// Operations
	template< typename T, s32 cols, s32 rows >
	Matrix< bool, cols, rows > operator==( const Matrix< T, cols, rows > &a, const Matrix< T, cols, rows > &b )
	{
		Matrix< bool, cols, rows > m;
		s32 i, j;

		for( i = 0; i < cols; ++i ) {
			for( j = 0; j < rows; ++j ) {
				m.data[ i ][ j ] = a.data[ i ][ j ] == b.data[ i ][ j ];
			}
		}

		return m;
	}
	template< typename T, s32 cols, s32 rows >
	Matrix< bool, cols, rows > operator!=( const Matrix< T, cols, rows > &a, const Matrix< T, cols, rows > &b )
	{
		Matrix< bool, cols, rows > m;
		s32 i, j;

		for( i = 0; i < cols; ++i ) {
			for( j = 0; j < rows; ++j ) {
				m.data[ i ][ j ] = a.data[ i ][ j ] != b.data[ i ][ j ];
			}
		}

		return m;
	}

	template< typename T, s32 cols, s32 rows >
	bool all( const Matrix< T, cols, rows > &mat )
	{
		s32 i, j;

		for( i = 0; i < cols; ++i ) {
			for( j = 0; j < rows; ++j ) {
				if ( !mat.data[ i ][ j ] ) {
					return false;
				}
			}
		}

		return true;
	}
	template< typename T, s32 cols, s32 rows >
	bool any( const Matrix< T, cols, rows > &mat )
	{
		s32 i, j;

		for( i = 0; i < cols; ++i ) {
			for( j = 0; j < rows; ++j ) {
				if ( mat.data[ i ][ j ] ) {
					return true;
				}
			}
		}

		return false;
	}
	template< typename T, s32 cols, s32 rows >
	T select( const Matrix< T, cols, rows > &mat )
	{
		s32 i, j;
		
		for( j = 0; j < rows; ++j ) {
			for( i = 0; i < cols; ++i ) {
				if ( mat.data[ i ][ j ] ) {
					return mat.data[ i ][ j ];
				}
			}
		}

		return static_cast< T >( 0.f );
	}

	template< typename T, s32 cols, s32 rows >
	Matrix< T, cols, rows > operator+( const Matrix< T, cols, rows >& mat, T scalar )
	{
		Matrix< T, cols, rows > m;
		s32 i, j;

		for( i = 0; i < cols; ++i ) {
			for( j = 0; j < rows; ++j ) {
				m.data[ i ][ j ] = mat.data[ i ][ j ] + scalar;
			}
		}

		return m;
	}
	template< typename T, s32 cols, s32 rows >
	Matrix< T, cols, rows > operator-( const Matrix< T, cols, rows >& mat, T scalar )
	{
		Matrix< T, cols, rows > m;
		s32 i, j;

		for( i = 0; i < cols; ++i ) {
			for( j = 0; j < rows; ++j ) {
				m.data[ i ][ j ] = mat.data[ i ][ j ] - scalar;
			}
		}

		return m;
	}
	template< typename T, s32 cols, s32 rows >
	Matrix< T, cols, rows > operator*( const Matrix< T, cols, rows >& mat, T scalar )
	{
		Matrix< T, cols, rows > m;
		s32 i, j;

		for( i = 0; i < cols; ++i ) {
			for( j = 0; j < rows; ++j ) {
				m.data[ i ][ j ] = mat.data[ i ][ j ] * scalar;
			}
		}

		return m;
	}
	template< typename T, s32 cols, s32 rows >
	Matrix< T, cols, rows > operator/( const Matrix< T, cols, rows >& mat, T scalar )
	{
		Matrix< T, cols, rows > m;
		s32 i, j;

		for( i = 0; i < cols; ++i ) {
			for( j = 0; j < rows; ++j ) {
				m.data[ i ][ j ] = mat.data[ i ][ j ] / scalar;
			}
		}

		return m;
	}

	template< typename T, s32 cols, s32 rows >
	Matrix< T, cols, rows > operator+( const Matrix< T, cols, rows >& a, const Matrix< T, cols, rows > &b )
	{
		Matrix< T, cols, rows > m;
		s32 i, j;

		for( i = 0; i < cols; ++i ) {
			for( j = 0; j < rows; ++j ) {
				m.data[ i ][ j ] = a.data[ i ][ j ] + b.data[ i ][ j ];
			}
		}

		return m;
	}
	template< typename T, s32 cols, s32 rows >
	Matrix< T, cols, rows > operator-( const Matrix< T, cols, rows >& a, const Matrix< T, cols, rows > &b )
	{
		Matrix< T, cols, rows > m;
		s32 i, j;

		for( i = 0; i < cols; ++i ) {
			for( j = 0; j < rows; ++j ) {
				m.data[ i ][ j ] = a.data[ i ][ j ] - b.data[ i ][ j ];
			}
		}

		return m;
	}
	
	
	template< typename T, s32 cola, s32 shared, s32 rowb >
	Matrix< T, cola, rowb > operator*( const Matrix< T, cola, shared > &a, const Matrix< T, shared, rowb > &b )
	{
		Matrix< T, cola, rowb > m;
		s32 i, j, k;
		T sum;

		for( i = 0; i < cola; ++i ) {
			for( j = 0; j < rowb; ++j ) {
				sum = static_cast< T >( 0.f );
				for( k = 0; k < shared; ++k ) {
					sum += a.data[ i ][ k ] * b.data[ k ][ j ];
				}
				m.data[ i ][ j ] = sum;
			}
		}

		return m;
	}

	template< typename T, s32 cols, s32 rows >
	Matrix< T, cols, rows > min( const Matrix< T, cols, rows > &a, T b )
	{
		Matrix< T, cols, rows > m;
		s32 i, j;

		for( i = 0; i < cols; ++i ) {
			for( j = 0; j < rows; ++j ) {
				m.data[ i ][ j ] = a.data[ i ][ j ] < b ? a.data[ i ][ j ] : b;
			}
		}

		return m;
	}
	template< typename T, s32 cols, s32 rows >
	Matrix< T, cols, rows > max( const Matrix< T, cols, rows > &a, T b )
	{
		Matrix< T, cols, rows > m;
		s32 i, j;

		for( i = 0; i < cols; ++i ) {
			for( j = 0; j < rows; ++j ) {
				m.data[ i ][ j ] = a.data[ i ][ j ] > b ? a.data[ i ][ j ] : b;
			}
		}

		return m;
	}
	template< typename T, s32 cols, s32 rows >
	Matrix< T, cols, rows > abs( const Matrix< T, cols, rows > &a )
	{
		Matrix< T, cols, rows > m;
		s32 i, j;

		for( i = 0; i < cols; ++i ) {
			for( j = 0; j < rows; ++j ) {
				m.data[ i ][ j ] = a.data[ i ][ j ] >= 0 ? a.data[ i ][ j ] : -a.data[ i ][ j ];
			}
		}

		return m;
	}
	template< typename T, s32 cols, s32 rows >
	Matrix< T, cols, rows > clamp( const Matrix< T, cols, rows > &a, T mini, T maxi )
	{
		Matrix< T, cols, rows > m;
		s32 i, j;

		for( i = 0; i < cols; ++i ) {
			for( j = 0; j < rows; ++j ) {
				m.data[ i ][ j ] = a.data[ i ][ j ] < mini ? mini
								 : a.data[ i ][ j ] > maxi ? maxi
								 : a.data[ i ][ j ];
			}
		}

		return m;
	}
	template< typename T, s32 cols, s32 rows >
	Matrix< T, cols, rows > saturate( const Matrix< T, cols, rows > &a )
	{
		Matrix< T, cols, rows > m;
		s32 i, j;

		for( i = 0; i < cols; ++i ) {
			for( j = 0; j < rows; ++j ) {
				m.data[ i ][ j ] = a.data[ i ][ j ] < static_cast< T >( 0.f ) ? static_cast< T >( 0.f )
								 : a.data[ i ][ j ] > static_cast< T >( 1.f ) ? static_cast< T >( 1.f )
								 : a.data[ i ][ j ];
			}
		}

		return m;
	}
	template< typename T, s32 cols, s32 rows, typename T_t >
	Matrix< T, cols, rows > lerp( const Matrix< T, cols, rows > &mini, const Matrix< T, cols, rows > &maxi, T_t t )
	{
		Matrix< T, cols, rows > m;
		s32 i, j;
		T_t t1;

		t1 = static_cast< T >( 1.f ) - t;
		for( i = 0; i < cols; ++i ) {
			for( j = 0; j < rows; ++j ) {
				m.data[ i ][ j ] = ( mini.data[ i ][ j ] * t ) + ( maxi.data[ i ][ j ] * t1 );
			}
		}

		return m;
	}
	template< typename T, s32 cols, s32 rows >
	T minComponent( const Matrix< T, cols, rows > &a )
	{
		s32 i, j;
		T mini = std::numeric_limits< T >::max( );

		for( i = 0; i < cols; ++i ) {
			for( j = 0; j < rows; ++j ) {
				mini = a.data[ i ][ j ] < mini ? a.data[ i ][ j ] :  mini;
			}
		}

		return mini;
	}
	template< typename T, s32 cols, s32 rows >
	T maxComponent( const Matrix< T, cols, rows > &a )
	{
		s32 i, j;
		T maxi = -std::numeric_limits< T >::max( );

		for( i = 0; i < cols; ++i ) {
			for( j = 0; j < rows; ++j ) {
				maxi = a.data[ i ][ j ] > maxi ? a.data[ i ][ j ] :  maxi;
			}
		}

		return maxi;
	}
	
	template< typename T, s32 cols, s32 rows >
	Vector< T, rows > operator*( const Matrix< T, cols, rows >& mat, const Vector< T, cols > &vec )
	{
		Vector< T, rows > v;
		s32 i, j;

#ifdef VUL_CPLUSPLUS11
		v = Vector< T, rows >( static_cast< T >( 0.f ) );
#else
		v = makeVector< T, rows >( static_cast< T >( 0.f ) );
#endif
		for( j = 0; j < rows; ++j ) {
			for( i = 0; i < cols; ++i ) {
				v.data[ j ] += mat.data[ i ][ j ] * vec.data[ i ];
			}
		}

		return v;
	}
	template< typename T, s32 cols, s32 rows >
	Vector< T, cols > operator*( const Vector< T, rows > &vec , const Matrix< T, cols, rows >& mat)
	{
		Vector< T, cols > v;
		s32 i, j;
		
#ifdef VUL_CPLUSPLUS11
		v = Vector< T, cols >( static_cast< T >( 0.f ) );
#else
		v = makeVector< T, cols >( static_cast< T >( 0.f ) );
#endif
		for( i = 0; i < cols; ++i ) {
			for( j = 0; j < rows; ++j ) {
				v.data[ i ] += mat.data[ i ][ j ] * vec.data[ j ];
			}
		}

		return v;
	}
	template< typename T, s32 cols, s32 rows >
	Point< T, rows > operator*( const Matrix< T, cols, rows >& mat, const Point< T, cols > &pt )
	{
		Point< T, rows > p;
		s32 i, j;

#ifdef VUL_CPLUSPLUS11
		p = Point< T, rows >( static_cast< T >( 0.f ) );
#else
		p = makePoint< T, rows >( static_cast< T >( 0.f ) );
#endif
		for( i = 0; i < cols; ++i ) {
			for( j = 0; j < rows; ++j ) {
				p.data[ j ] += mat.data[ i ][ j ] * pt.data[ i ];
			}
		}

		return p;
	}
	template< typename T, s32 cols, s32 rows >
	Point< T, cols > operator*( const Point< T, rows > &pt , const Matrix< T, cols, rows > &mat)
	{
		Point< T, cols > p;
		s32 i, j;
		
#ifdef VUL_CPLUSPLUS11
		p = Point< T, cols >( static_cast< T >( 0.f ) );
#else
		p = makePoint< T, cols >( static_cast< T >( 0.f ) );
#endif
		for( j = 0; j < rows; ++j ) {
			for( i = 0; i < cols; ++i ) {
				p.data[ i ] += mat.data[ i ][ j ] * pt.data[ j ];
			}
		}

		return p;
	}
	
	// Functions

	template< typename T, s32 n >
	Matrix< T, n, n > inverse( const Matrix< T, n, n > &mat )
	{
		f32 det;
		s32 r, c, i, j, a, b;

		// Find the adjugate
		Matrix< T, n, n > adjugate;
		Matrix< T, n-1, n-1 > m;
		for( c = 0; c < n; ++c ) {
			for( r = 0; r < n; ++r ) {
				for( i = 0, a = 0; i < n; ++i ) {
					if ( i == c ) {
						continue;
					}
					for( j = 0, b = 0; j < n; ++j ) {
						if ( j == r ) {
							continue;
						}
						m.data[ a ][ b ] = mat.data[ i ][ j ];
						++b;
					}
					++a;
				}
				adjugate.data[ r ][ c ] = pow( -1.f, c + r ) * determinant( m );
			}
			// Test if determinant is zero here on first iteration; since we avoid doing double work.
			if ( c == 0 ) {
				det = 0;
				for( r = 0; r < n; ++r ) {
					det += mat( c, r ) * adjugate.data[ r ][ c ];
				}
				VUL_MATH_ASSERT( det != 0 && "vul::Matrix.inverse(): Determinant is zero, inverse is not defined!" );
			}
		}
		
		return adjugate * ( 1.f / det );
	}
	template< typename T, s32 cols, s32 rows >
	Matrix< T, cols, rows > transpose( const Matrix< T, cols, rows > &mat )
	{
		Matrix< T, rows, cols > m;
		s32 i, j;

		for( i = 0; i < cols; ++i ) {
			for( j = 0; j < rows; ++j ) {
				m.data[ j ][ i ] = mat.data[ i ][ j ];
			}
		}

		return m;
	}
	
	template< typename T >
	f32 determinant( const Matrix< T, 3, 3 > &mat )
	{
      f32 t0, t1, t2;
      t0 = ( f32 )mat.data[ 1 ][ 1 ] * ( f32 )mat.data[ 2 ][ 1 ]
			- ( f32 )mat.data[ 1 ][ 2 ] * ( f32 )mat.data[ 2 ][ 2 ];
      t1 = ( f32 )mat.data[ 1 ][ 0 ] * ( f32 )mat.data[ 2 ][ 2 ]
			- ( f32 )mat.data[ 1 ][ 2 ] * ( f32 )mat.data[ 2 ][ 0 ];
      t2 = ( f32 )mat.data[ 1 ][ 0 ] * ( f32 )mat.data[ 2 ][ 1 ]
			- ( f32 )mat.data[ 1 ][ 1 ] * ( f32 )mat.data[ 2 ][ 0 ];


		return ( f32 )mat.data[ 0 ][ 0 ] * t0
           - ( f32 )mat.data[ 0 ][ 1 ] * t1
           + ( f32 )mat.data[ 0 ][ 2 ] * t2;
   }
	template< typename T >
	f32 determinant( const Matrix< T, 2, 2 > &mat )
	{
		return ( f32 )mat.data[ 0 ][ 0 ] * ( f32 )mat.data[ 1 ][ 1 ]
			 - ( f32 )mat.data[ 0 ][ 1 ] * ( f32 )mat.data[ 1 ][ 0 ];
	}
	template< typename T, s32 n >
	f32 determinant( const Matrix< T, n, n > &mat )
	{
		vul_matrix_zero_helper zero;
		Matrix< T, n-1, n-1 > m;
		s32 i, j, k, a, b;
		f32 det;

		det = 0;
		zero = vul_matrix_find_most_zero( mat );

		if( zero.type == VUL_MATRIX_COLUMN ) {
			for( k = 0; k < n; ++k ) {
				for( i = 0, a = 0; i < n; ++i ) {
					if ( i == zero.index ) {
						continue;
					}
					for( j = 0, b = 0; j < n; ++j ) {
						if ( j == k ) {
							continue;
						}
						m.data[ a ][ b ] = mat.data[ i ][ j ];
						++b;
					}
					++a;
				}
				det += pow( -1.f, zero.index + k ) * mat.data[ zero.index ][ k ] * determinant( m );
			}
		} else { // VUL_MATRIX_ROW
			for( k = 0; k < n; ++k ) {
				for( j = 0, b = 0; j < n; ++j ) {
					if ( j == zero.index ) {
						continue;
					}
					for( i = 0, a = 0; i < n; ++i ) {
						if ( i == k ) {
							continue;
						}
						m.data[ a ][ b ] = mat.data[ i ][ j ];
						++a;
					}
					 ++b;
				}
				det += pow( -1.f, zero.index + k ) * mat.data[ k ][ zero.index ] * determinant( m );
			}
		}

		return det;
	}

	template< typename T, s32 cols, s32 rows >
	Vector< T, rows > column( const Matrix< T, cols, rows > &mat, s32 n )
	{
		Vector< T, rows > v;
		s32 i;

		for( i = 0; i < rows; ++i ) {
			v.data[ i ] = mat.data[ n ][ i ];
		}

		return v;
	}
	template< typename T, s32 cols, s32 rows >
	Vector< T, cols > row( const Matrix< T, cols, rows > &mat, s32 n )
	{
		Vector< T, cols > v;
		s32 i;

		for( i = 0; i < cols; ++i ) {
			v.data[ i ] = mat.data[ i ][ n ];
		}

		return v;
	}
   template< typename T >
   Vector< T, 3 > euler_angles( const Matrix< T, 3, 3 > &mat )
   {
      Vector< T, 3 > a;

      a[ 0 ] = atan2(  mat.data[ 1 ][ 2 ], mat.data[ 2 ][ 2 ] );
      a[ 1 ] = atan2( -mat.data[ 0 ][ 2 ], sqrt( mat.data[ 1 ][ 2 ] * mat.data[ 1 ][ 2 ]
                                               + mat.data[ 2 ][ 2 ] * mat.data[ 2 ][ 2 ] ) );
      a[ 2 ] = atan2( mat.data[ 0 ][ 1 ], mat.data[ 0 ][ 0 ] );

      return a;
   }
   // Citation: Tom Duff, James Burgess, Per Christensen, Christophe Hery, Andrew Kensler, Max Liani, and Ryusuke Villemin, 
   // Building an Orthonormal Basis, Revisited, Journal of Computer Graphics Techniques (JCGT), vol. 6, no. 1, 1-8, 2017
   // Available online http://jcgt.org/published/0006/01/01/
   template< typename T >
   Matrix< T, 3, 3 > orthonormal_basis( const Vector< T, 3 > &n )
   {
      Vector< T, 3 > b1;
      Vector< T, 3 > b2;

      T one = T( 1.0f );
      T sign = copysign( one, n[ 2 ]);
      const T a = -one / ( sign + n[ 2 ] );
      const T b = n[ 0 ] * n[ 1 ] * a;
      
      b1[ 0 ] = one + sign * n.x * n.x * a;
      b1[ 1 ] = sign * b;
      b1[ 2 ] = -sign * n.x;
      b2[ 0 ] = b;
      b2[ 1 ] = sign + n.y * n.y * a;
      b2[ 2 ] = -n.y;

      // @TODO(thynn): Write test for this
      return makeMatrix33FromColumns( b1, b2, n ); // @TODO(thynn): Check handedness of this
   }

	// Helper function to speed up determinant calculation for large matrices
	// Counts the number of zeros in each column & row, returning a the index 
	// of the column or row awith most zeroes.
	// If the matrix is small (cols and rows <= 4), it just return the first column
	template< typename T, s32 cols, s32 rows > 
	vul_matrix_zero_helper vul_matrix_find_most_zero( const Matrix< T, cols, rows > &mat )
	{
		s32 i, j, c;
		vul_matrix_zero_helper r;

		r.count = std::numeric_limits< s32 >::max( );
		r.index = 0;

		// Early out for small matrices (just first column)
		if( cols <= 4 && rows <= 4 ) {
			r.type = VUL_MATRIX_COLUMN;
			return r;
		}

		// Columns
		for( i = 0; i < cols; ++i ) {
			c = 0;
			for( j = 0; j < rows; ++j ) {
				c += mat.data[ i ][ j ] != 0 ? 0 : 1;
			}
			if ( c < r.count ) {
				r.count = c;
				r.index = i;
				r.type = VUL_MATRIX_COLUMN;
			}
		}
		// Rows
		for( j = 0; j < rows; ++j ) {
			c = 0;
			for( i = 0; i < cols; ++i ) {
				c += mat.data[ i ][ j ] != 0 ? 0 : 1;
			}
			if ( c < r.count ) {
				r.count = c;
				r.index = j;
				r.type = VUL_MATRIX_ROW;
			}
		}

		return r;
	}

}

#endif
