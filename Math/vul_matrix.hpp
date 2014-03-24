/*
 * Villains' Utility Library - Thomas Martin Schmid, 2014. Public domain¹
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

#include "vul_types.hpp"

namespace vul {
	
	//----------------
	// Declarations
	// 

	template< typename T, i32_t n >
	struct Vector;

	// Generic definition
	template< typename T, i32_t cols, i32_t rows >
	struct Matrix {
		// Data
		T data[ cols ][ rows ];	// Column major

#ifdef VUL_CPLUSPLUS11
		// Constructor
		explicit Matrix< T, cols, rows >( );									// Empty constructor
		explicit Matrix< T, cols, rows >( T val );								// Initialize to a single value
		Matrix< T, cols, rows >( const Matrix< T, cols, rows > &m );			// Copy constructor
		explicit Matrix< T, cols, rows >( T (& a)[ cols ][ rows ] ); 			// Generic array constructor
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
		 * Indexing operator.
		 */
		T &operator( )( i32_t c, i32_t r );
		/**
		 * Constant indexing operator.
		 */
		T const &operator( )( i32_t c, i32_t r ) const;
	};

	// Typedefs
	typedef Matrix< f16_t, 2, 2 > f16_22t;
	typedef Matrix< f32_t, 2, 2 > f32_22t;
	typedef Matrix< f64_t, 2, 2 > f64_22t;
	typedef Matrix< f16_t, 3, 3 > f16_33t;
	typedef Matrix< f32_t, 3, 3 > f32_33t;
	typedef Matrix< f64_t, 3, 3 > f64_33t;
	typedef Matrix< f16_t, 4, 4 > f16_44t;
	typedef Matrix< f32_t, 4, 4 > f32_44t;
	typedef Matrix< f64_t, 4, 4 > f64_44t;

	typedef Matrix< fi32_t, 2, 2 > fi32_22t;
	typedef Matrix< fi32_t, 3, 3 > fi32_33t;
	typedef Matrix< fi32_t, 4, 4 > fi32_44t;

	typedef Matrix< i8_t, 2, 2 > i8_22t;
	typedef Matrix< i16_t, 2, 2 > i16_22t;
	typedef Matrix< i32_t, 2, 2 > i32_22t;
	typedef Matrix< i64_t, 2, 2 > i64_22t;
	typedef Matrix< i8_t, 3, 3 > i8_33t;
	typedef Matrix< i16_t, 3, 3 > i16_33t;
	typedef Matrix< i32_t, 3, 3 > i32_33t;
	typedef Matrix< i64_t, 3, 3 > i64_33t;
	typedef Matrix< i8_t, 4, 4 > i8_44t;
	typedef Matrix< i16_t, 4, 4 > i16_44t;
	typedef Matrix< i32_t, 4, 4 > i32_44t;
	typedef Matrix< i64_t, 4, 4 > i64_44t;

	typedef Matrix< ui8_t, 2, 2 > ui8_22t;
	typedef Matrix< ui16_t, 2, 2 > ui16_22t;
	typedef Matrix< ui32_t, 2, 2 > ui32_22t;
	typedef Matrix< ui64_t, 2, 2 > ui64_22t;
	typedef Matrix< ui8_t, 3, 3 > ui8_33t;
	typedef Matrix< ui16_t, 3, 3 > ui16_33t;
	typedef Matrix< ui32_t, 3, 3 > ui32_33t;
	typedef Matrix< ui64_t, 3, 3 > ui64_33t;
	typedef Matrix< ui8_t, 4, 4 > ui8_44t;
	typedef Matrix< ui16_t, 4, 4 > ui16_44t;
	typedef Matrix< ui32_t, 4, 4 > ui32_44t;
	typedef Matrix< ui64_t, 4, 4 > ui64_44t;

	// Non-member constructors for pre C++11 versions
#ifndef VUL_CPLUSPLUS11
	template< typename T, i32_t cols, i32_t rows >
	Matrix< T, cols, rows > makeMatrix( );								// Empty constructor
	template< typename T, i32_t cols, i32_t rows >
	Matrix< T, cols, rows > makeMatrix( T val );						// Initialize to a single value
	template< typename T, i32_t cols, i32_t rows >
	Matrix< T, cols, rows > makeMatrix( const Matrix< T, cols, rows > &m );	// Copy constructor
	template< typename T, i32_t cols, i32_t rows >
	Matrix< T, cols, rows > makeMatrix( T (& a)[ cols ][ rows ] ); 		// Generic array constructor
#endif		
	template< typename T, i32_t cols, i32_t rows >
	Matrix< T, cols, rows > makeMatrix( f32_t (& a)[ rows * cols ] );	// From float array, to interface with other libraries
	template< typename T, i32_t cols, i32_t rows >
	Matrix< T, cols, rows > makeMatrix( i32_t (& a)[ rows * cols ] ); 	// From int array, to interface with other libraries
	
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
	
	template< typename T, i32_t n >
	Matrix< T, n, n > makeIdentity( );
	template< typename T, i32_t cols, i32_t rows >
	Matrix< T, cols, rows > makeMatrixFromRows( const Vector< T, cols > r[ rows ] );
	template< typename T, i32_t cols, i32_t rows >
	Matrix< T, cols, rows > makeMatrixFromColumns( const Vector< T, rows > c[ cols ] );
	/**
	 * Copies the top left coloXrowo matrix of mat into a new matrix of size
	 * colXrown, essentially truncating the top left submatrix of mat into a
	 * new, smaller matrix.
	 */
	template< typename T, i32_t coln, i32_t rown, i32_t colo, i32_t rowo >
	Matrix< T, coln, rown > truncate( const Matrix< T, colo, rowo > &mat );
	/**
	 * Copies the contents of matrix src (sized srcc X srcr) into the top left 
	 * ( srcc X srcr ) submatrix of dst. Dst must be at least as big as src.
	 */
	template< typename T, i32_t dstc, i32_t dstr, i32_t srcc, i32_t srcr >
	void copy( Matrix< T, dstc, dstr > *dst, const Matrix< T, srcc, srcr > &src );

	// Operations
	/**
	 * Componentwise comparison. Returns a matrix of bools indicating if the compnents 
	 * in the corresponding position are equal.
	 */
	template< typename T, i32_t cols, i32_t rows >
	Matrix< bool, cols, rows > operator==( const Matrix< T, cols, rows > &a, const Matrix< T, cols, rows > &b );
	/**
	 * Componentwise comparison. Returns a matrix of bools indicating if the compnents 
	 * in the corresponding position are not equal.
	 */
	template< typename T, i32_t cols, i32_t rows >
	Matrix< bool, cols, rows > operator!=( const Matrix< T, cols, rows > &a, const Matrix< T, cols, rows > &b );

	/**
	 * Returns true if all compnents are true. Is valid for any type if( T ) is valid.
	 * Equivalent of AND-ing together all compnents.
	 */
	template< typename T, i32_t cols, i32_t rows >
	bool all( const Matrix< T, cols, rows > &mat );
	/**
	 * Returns true if any compnents is true. Is valid for any type if( T ) is valid.
	 * Equivalent of OR-ing together all compnents.
	 */
	template< typename T, i32_t cols, i32_t rows >
	bool any( const Matrix< T, cols, rows > &mat );
	/**
	 * Returns the first compnent that is evaluated to true by if( T ).
	 * Equivalent to the ?: selector.
	 * Operates row major; mat_row0_column1 comes before mat_row1_column0.
	 */
	template< typename T, i32_t cols, i32_t rows >
	T select( const Matrix< T, cols, rows > &mat ); 

	/**
	 * Componentwise addition.
	 */
	template< typename T, i32_t cols, i32_t rows >
	Matrix< T, cols, rows > operator+( const Matrix< T, cols, rows >& mat, T scalar );
	/**
	 * Componentwise subtraction.
	 */
	template< typename T, i32_t cols, i32_t rows >
	Matrix< T, cols, rows > operator-( const Matrix< T, cols, rows >& mat, T scalar );
	/**
	 * Componentwise multiplication.
	 */
	template< typename T, i32_t cols, i32_t rows >
	Matrix< T, cols, rows > operator*( const Matrix< T, cols, rows >& mat, T scalar );
	/**
	 * Componentwise division.
	 */
	template< typename T, i32_t cols, i32_t rows >
	Matrix< T, cols, rows > operator/( const Matrix< T, cols, rows >& mat, T scalar );

	/**
	 * Componentwise addition of matrices.
	 */
	template< typename T, i32_t cols, i32_t rows >
	Matrix< T, cols, rows > operator+( const Matrix< T, cols, rows >& a, const Matrix< T, cols, rows > &b ); // Componentwise addition
	/**
	 * Componentwise subtraction of matrices.
	 */
	template< typename T, i32_t cols, i32_t rows >
	Matrix< T, cols, rows > operator-( const Matrix< T, cols, rows >& a, const Matrix< T, cols, rows > &b ); // Componentwise subtraction
	
	/**
	 * Componentwise min( compnent, b ).
	 */
	template< typename T, i32_t cols, i32_t rows >
	Matrix< T, cols, rows > min( const Matrix< T, cols, rows > &a, T b );
	/**
	 * Componentwise max( compnent, b ).
	 */
	template< typename T, i32_t cols, i32_t rows >
	Matrix< T, cols, rows > max( const Matrix< T, cols, rows > &a, T b );
	/**
	 * Componentwise absolute value.
	 */
	template< typename T, i32_t cols, i32_t rows >
	Matrix< T, cols, rows > abs( const Matrix< T, cols, rows > &a );
	/**
	 * Compomentwise clamp( x, min, max ).
	 */
	template< typename T, i32_t cols, i32_t rows >
	Matrix< T, cols, rows > clamp( const Matrix< T, cols, rows > &a, T mini, T maxi );
	/**
	 * Compomentwise saturate, so clamp( x, 0, 1 ).
	 */
	template< typename T, i32_t cols, i32_t rows >
	Matrix< T, cols, rows > saturate( const Matrix< T, cols, rows > &a );
	/** 
	 * Componentwise linear interpolation based on t.
	 */
	template< typename T, i32_t cols, i32_t rows, typename T_t >
	Matrix< T, cols, rows > lerp( const Matrix< T, cols, rows > &mini, const Matrix< T, cols, rows > &maxi, T_t t );
	/**
	 * Returns the smalles compnent.
	 */
	template< typename T, i32_t cols, i32_t rows >
	T minComponent( const Matrix< T, cols, rows > &a );
	/**
	 * Returns the largest compnent.
	 */
	template< typename T, i32_t cols, i32_t rows >
	T maxComponent( const Matrix< T, cols, rows > &a );
	
	/**
	 * Matrix multiplication. This is NOT componentwise, but proper matrix mul.
	 */
	template< typename T, i32_t cola, i32_t shared, i32_t rowb >
	Matrix< T, cola, rowb > operator*( const Matrix< T, cola, shared > &a, const Matrix< T, shared, rowb > &b );
	
	/** 
	 * Right-side multiplication of a matrix and a column vector.
	 */
	template< typename T, i32_t cols, i32_t rows >
	Vector< T, rows > operator*( const Matrix< T, cols, rows >& mat, const Vector< T, cols > &vec );
	/** 
	 * Left-side multiplication of a matrix and a column vector.
	 */
	template< typename T, i32_t cols, i32_t rows >
	Vector< T, cols > operator*( const Vector< T, rows > &vec , const Matrix< T, cols, rows >& mat);	
	/** 
	 * Right-side multiplication of a matrix and a point interpreted as a column vector.
	 */
	template< typename T, i32_t cols, i32_t rows >
	Point< T, rows > operator*( const Matrix< T, cols, rows >& mat, const Point< T, cols > &vec );
	/** 
	 * Left-side multiplication of a matrix and a point interpreted as a column vector.
	 */
	template< typename T, i32_t cols, i32_t rows >
	Point< T, cols > operator*( const Point< T, rows > &vec , const Matrix< T, cols, rows >& mat);
	
	// Functions
	/**
	 * Returns the inverse of the matrix.
	 */
	template< typename T, i32_t n >
	Matrix< T, n, n > inverse( const Matrix< T, n, n > &mat );
	/**
	 * Returns the transpose of the matrix.
	 */
	template< typename T, i32_t cols, i32_t rows >
	Matrix< T, cols, rows > transpose( const Matrix< T, cols, rows > &mat );
	
	/**
	 * Calculates the determinant of the matrix. This is the 2x2 special case.
	 */
	template< typename T >
	f32_t determinant( const Matrix< T, 2, 2 > &mat );
	/**
	 * Calculates the determinant of the matrix. Recursively simplifies down to
	 * the 2x2 case. Uses vul_matrix_find_most_zero at every level to recurse down
	 * the path with the least sub-expressions at that level.
	 */
	template< typename T, i32_t cols, i32_t rows >
	f32_t determinant( const Matrix< T, cols, rows > &mat );

	/** 
	 * Returns the column at index n in the given matrix.
	 */
	template< typename T, i32_t cols, i32_t rows >
	Vector< T, rows > column( const Matrix< T, cols, rows > &mat, i32_t n );
	/** 
	 * Returns the row at index n in the given matrix.
	 */
	template< typename T, i32_t cols, i32_t rows >
	Vector< T, cols > row( const Matrix< T, cols, rows > &mat, i32_t n );

	
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
		i32_t index;
		i32_t count;
	};
	/** 
	 * Helper function of detereminant.
	 * Counts the number of zeros in each column and row. 
	 * Returns a vul_matrix_zero_helper with row/column type, index and count
	 */
	template< typename T, i32_t cols, i32_t rows >
	vul_matrix_zero_helper vul_matrix_find_most_zero( const Matrix< T, cols, rows > &mat );
	
	//---------------------------
	// Definitions
	//
	
#ifdef VUL_CPLUSPLUS11
	// Constructor
	template< typename T, i32_t cols, i32_t rows >
	Matrix< T, cols, rows >::Matrix( )
	{
		i32_t i, j;
		for( i = 0; i < cols; ++i ) {
			for( j = 0; j < rows; ++j ) {
				data[ i ][ j ] = static_cast< T >( 0.f );
			}
		}
	}
	template< typename T, i32_t cols, i32_t rows >
	Matrix< T, cols, rows >::Matrix( T val )
	{
		i32_t i, j;
		for( i = 0; i < cols; ++i ) {
			for( j = 0; j < rows; ++j ) {
				data[ i ][ j ] = val;
			}
		}
	}
	template< typename T, i32_t cols, i32_t rows >
	Matrix< T, cols, rows >::Matrix( const Matrix< T, cols, rows > &m )
	{
		i32_t i, j;
		for( i = 0; i < cols; ++i ) {
			for( j = 0; j < rows; ++j ) {
				data[ i ][ j ] = m( i, j );
			}
		}
	}
	template< typename T, i32_t cols, i32_t rows >
	Matrix< T, cols, rows >::Matrix( T (& a)[ cols ][ rows ] )
	{
		memcpy( data, a, sizeof( T ) * rows * cols );
	}
	template< typename T, i32_t cols, i32_t rows >
	Matrix< T, cols, rows >::Matrix( std::initializer_list< T > list )
	{
		i32_t i;
		typename std::initializer_list< T >::iterator it;

		for( it = list.begin( ), i = 0; it != list.end( ) && i < cols * rows; ++it, ++i ) {
			data[ i / rows ][ i % rows ] = *it;
		}
	}
#else
	template< typename T, i32_t cols, i32_t rows >
	Matrix< T, cols, rows > makeMatrix( )
	{
		Matrix< T, cols, rows > m;
		i32_t i, j;

		for( i = 0; i < cols; ++i ) {
			for( j = 0; j < rows; ++j ) {
				m.data[ i ][ j ] = static_cast< T >( 0.f );
			}
		}

		return m;
	}
	
	template< typename T, i32_t cols, i32_t rows >
	Matrix< T, cols, rows > makeMatrix( T val )
	{
		Matrix< T, cols, rows > m;
		i32_t i, j;

		for( i = 0; i < cols; ++i ) {
			for( j = 0; j < rows; ++j ) {
				m.data[ i ][ j ] = val;
			}
		}

		return m;
	}
	
	template< typename T, i32_t cols, i32_t rows >
	Matrix< T, cols, rows > makeMatrix( const Matrix< T, cols, rows > &m )
	{
		Matrix< T, cols, rows > r;
		i32_t i, j;
		
		for( i = 0; i < cols; ++i ) {
			for( j = 0; j < rows; ++j ) {
				r.data[ i ][ j ] = m( i, j );
			}
		}

		return r;
	}	
	template< typename T, i32_t cols, i32_t rows >
	Matrix< T, cols, rows > makeMatrix( T (& a)[ cols ][ rows ] )
	{
		Matrix< T, cols, rows > m;
		i32_t i, j;

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

		m( 0, 0 ) = c1r1;
		m( 0, 1 ) = c1r2;
		m( 1, 0 ) = c2r1;
		m( 1, 1 ) = c2r2;

		return m;
	}
	template< typename T >
	Matrix< T, 3, 3 > makeMatrix33( T c1r1, T c2r1, T c3r1,
									T c1r2, T c2r2, T c3r2,
									T c1r3, T c2r3, T c3r3 )
	{
		Matrix< T, 3, 3 > m;

		m( 0, 0 ) = c1r1;
		m( 0, 1 ) = c1r2;
		m( 0, 2 ) = c1r3;
		m( 1, 0 ) = c2r1;
		m( 1, 1 ) = c2r2;
		m( 1, 2 ) = c2r3;
		m( 2, 0 ) = c3r1;
		m( 2, 1 ) = c3r2;
		m( 2, 2 ) = c3r3;

		return m;
	}
	template< typename T >
	Matrix< T, 4, 4 > makeMatrix44( T c1r1, T c2r1, T c3r1, T c4r1, 
									T c1r2, T c2r2, T c3r2, T c4r2,
									T c1r3, T c2r3, T c3r3, T c4r3,
									T c1r4, T c2r4, T c3r4, T c4r4 )
	{
		Matrix< T, 4, 4 > m;

		m( 0, 0 ) = c1r1;
		m( 0, 1 ) = c1r2;
		m( 0, 2 ) = c1r3;
		m( 0, 3 ) = c1r4;
		m( 1, 0 ) = c2r1;
		m( 1, 1 ) = c2r2;
		m( 1, 2 ) = c2r3;
		m( 1, 3 ) = c2r4;
		m( 2, 0 ) = c3r1;
		m( 2, 1 ) = c3r2;
		m( 2, 2 ) = c3r3;
		m( 2, 3 ) = c3r4;
		m( 3, 0 ) = c4r1;
		m( 3, 1 ) = c4r2;
		m( 3, 2 ) = c4r3;
		m( 3, 3 ) = c4r4;

		return m;
	}

	template< typename T, i32_t cols, i32_t rows >
	Matrix< T, cols, rows > makeMatrix( f32_t (& a)[ rows * cols ] )
	{
		Matrix< T, cols, rows > m;
		i32_t i, j;

		for( i = 0; i < cols; ++i ) {
			for( j = 0; j < rows; ++j ) {
				m.data[ i ][ j ] = static_cast< T >( a[ i * cols + j ] );
			}
		}
		
		return m;
	}	
	template< typename T, i32_t cols, i32_t rows >
	Matrix< T, cols, rows > makeMatrix( i32_t (& a)[ rows * cols ] )
	{
		Matrix< T, cols, rows > m;
		i32_t i, j;

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

		m( 0, 0 ) = c1[ 0 ];
		m( 0, 1 ) = c1[ 1 ];
		m( 1, 0 ) = c2[ 0 ];
		m( 1, 1 ) = c2[ 1 ];

		return m;
	}
	template< typename T >
	Matrix< T, 3, 3 > makeMatrix33FromColumns( const Vector< T, 3 > &c1,
											   const Vector< T, 3 > &c2,
											   const Vector< T, 3 > &c3   )
	{
		Matrix< T, 3, 3 > m;

		m( 0, 0 ) = c1[ 0 ];
		m( 0, 1 ) = c1[ 1 ];
		m( 0, 2 ) = c1[ 2 ];
		m( 1, 0 ) = c2[ 0 ];
		m( 1, 1 ) = c2[ 1 ];
		m( 1, 2 ) = c2[ 2 ];
		m( 2, 0 ) = c3[ 0 ];
		m( 2, 1 ) = c3[ 1 ];
		m( 2, 2 ) = c3[ 2 ];

		return m;
	}
	template< typename T >
	Matrix< T, 4, 4 > makeMatrix44FromColumns( const Vector< T, 4 > &c1,
											   const Vector< T, 4 > &c2,
											   const Vector< T, 4 > &c3,
											   const Vector< T, 4 > &c4 )
	{
		Matrix< T, 4, 4 > m;

		
		m( 0, 0 ) = c1[ 0 ];
		m( 0, 1 ) = c1[ 1 ];
		m( 0, 2 ) = c1[ 2 ];
		m( 0, 3 ) = c1[ 3 ];
		m( 1, 0 ) = c2[ 0 ];
		m( 1, 1 ) = c2[ 1 ];
		m( 1, 2 ) = c2[ 2 ];
		m( 1, 3 ) = c2[ 3 ];
		m( 2, 0 ) = c3[ 0 ];
		m( 2, 1 ) = c3[ 1 ];
		m( 2, 2 ) = c3[ 2 ];
		m( 2, 3 ) = c3[ 3 ];
		m( 3, 0 ) = c4[ 0 ];
		m( 3, 1 ) = c4[ 1 ];
		m( 3, 2 ) = c4[ 2 ];
		m( 3, 3 ) = c4[ 3 ];

		return m;
	}
	template< typename T, i32_t n >
	Matrix< T, n, n > makeIdentity( )
	{
		Matrix< T, n, n > m;
		i32_t i;

#ifdef VUL_CPLUSPLUS11
		m = Matrix< T, n, n >( static_cast< T >( 0.f ) );
#else
		m = makeMatrix< T, n, n >( static_cast< T >( 0.f ) );
#endif
		for( i = 0; i < n; ++i ) {
			m( i, i ) = static_cast< T >( 1 );
		}

		return m;
	}
	template< typename T, i32_t cols, i32_t rows >
	Matrix< T, cols, rows > makeMatrixFromRows( const Vector< T, cols > r[ rows ] )
	{
		Matrix< T, cols, rows > m;
		i32_t i, j;

		for( i = 0; i < cols; ++i ) {
			for( j = 0; j < rows; ++j ) {
				m( i, j ) = r[ j ][ i ];
			}
		}

		return m;
	}
	template< typename T, i32_t cols, i32_t rows >
	Matrix< T, cols, rows > makeMatrixFromColumns( const Vector< T, rows > c[ cols ] )
	{
		Matrix< T, cols, rows > m;
		i32_t i, j;

		for( i = 0; i < cols; ++i ) {
			for( j = 0; j < rows; ++j ) {
				m( i, j ) = c[ i ][ j ];
			}
		}

		return m;
	}
	template< typename T, i32_t coln, i32_t rown, i32_t colo, i32_t rowo >
	Matrix< T, coln, rown > truncate( const Matrix< T, colo, rowo > &mat )
	{
		Matrix< T, coln, rown > m;
		i32_t i, j;

		assert( colo >= coln && rowo >= rown );

		for( i = 0; i < coln; ++i ) {
			for( j = 0; j < rown; ++j ) {
				m( i, j ) = mat( i, j );
			}
		}

		return m;
	}
	template< typename T, i32_t dstc, i32_t dstr, i32_t srcc, i32_t srcr >
	void copy( Matrix< T, dstc, dstr > *dst, const Matrix< T, srcc, srcr > &src )
	{
		i32_t i, j;

		assert( dstc >= srcc && dstr >= srcr );

		for( i = 0; i < srcc; ++i ) {
			for( j = 0; j < srcr; ++j ) {
				(*dst)( i, j ) = src( i, j );
			}
		}
	}

	// Operators
	template< typename T, i32_t cols, i32_t rows >
	Matrix< T, cols, rows >& Matrix< T, cols, rows >::operator=( const Matrix< T, cols, rows > & rhs )
	{
		i32_t i, j;
		
		for( i = 0; i < cols; ++i ) {
			for( j = 0; j < rows; ++j ) {
				data[ i ][ j ] = rhs( i, j );
			}
		}

		return *this;
	}
	template< typename T, i32_t cols, i32_t rows >
	Matrix< T, cols, rows > &Matrix< T, cols, rows >::operator+=( T scalar )
	{
		i32_t i, j;

		for( i = 0; i < cols; ++i ) {
			for( j = 0; j < rows; ++j ) {
				data[ i ][ j ] += scalar;
			}
		}

		return *this;
	}
	template< typename T, i32_t cols, i32_t rows >
	Matrix< T, cols, rows > &Matrix< T, cols, rows >::operator-=( T scalar )
	{
		i32_t i, j;

		for( i = 0; i < cols; ++i ) {
			for( j = 0; j < rows; ++j ) {
				data[ i ][ j ] -= scalar;
			}
		}

		return *this;
	}
	template< typename T, i32_t cols, i32_t rows >
	Matrix< T, cols, rows > &Matrix< T, cols, rows >::operator*=( T scalar )
	{
		i32_t i, j;

		for( i = 0; i < cols; ++i ) {
			for( j = 0; j < rows; ++j ) {
				data[ i ][ j ] *= scalar;
			}
		}

		return *this;
	}
	template< typename T, i32_t cols, i32_t rows >
	Matrix< T, cols, rows > &Matrix< T, cols, rows >::operator/=( T scalar )
	{
		i32_t i, j;

		for( i = 0; i < cols; ++i ) {
			for( j = 0; j < rows; ++j ) {
				data[ i ][ j ] /= scalar;
			}
		}

		return *this;
	}

	template< typename T, i32_t cols, i32_t rows >
	Matrix< T, cols, rows > &Matrix< T, cols, rows >::operator+=( const Matrix< T, cols, rows > &rhs )
	{
		i32_t i, j;

		for( i = 0; i < cols; ++i ) {
			for( j = 0; j < rows; ++j ) {
				data[ i ][ j ] += rhs( i, j );
			}
		}

		return *this;
	}
	template< typename T, i32_t cols, i32_t rows >
	Matrix< T, cols, rows > &Matrix< T, cols, rows >::operator-=( const Matrix< T, cols, rows > &rhs )
	{
		i32_t i, j;

		for( i = 0; i < cols; ++i ) {
			for( j = 0; j < rows; ++j ) {
				data[ i ][ j ] -= rhs( i, j );
			}
		}

		return *this;
	}

	template< typename T, i32_t cols, i32_t rows >
	T &Matrix< T, cols, rows >::operator( )( i32_t i, i32_t j )
	{
		assert( i < cols );
		assert( j < rows );
		return data[ i ][ j ];
	}
		
	template< typename T, i32_t cols, i32_t rows >
	T const &Matrix< T, cols, rows >::operator( )( i32_t i, i32_t j ) const
	{
		assert( i < cols );
		assert( j < rows );
		return data[ i ][ j ];
	}

	// Operations
	template< typename T, i32_t cols, i32_t rows >
	Matrix< bool, cols, rows > operator==( const Matrix< T, cols, rows > &a, const Matrix< T, cols, rows > &b )
	{
		Matrix< bool, cols, rows > m;
		i32_t i, j;

		for( i = 0; i < cols; ++i ) {
			for( j = 0; j < rows; ++j ) {
				m( i, j ) = a( i, j ) == b( i, j );
			}
		}

		return m;
	}
	template< typename T, i32_t cols, i32_t rows >
	Matrix< bool, cols, rows > operator!=( const Matrix< T, cols, rows > &a, const Matrix< T, cols, rows > &b )
	{
		Matrix< bool, cols, rows > m;
		i32_t i, j;

		for( i = 0; i < cols; ++i ) {
			for( j = 0; j < rows; ++j ) {
				m( i, j ) = a( i, j ) != b( i, j );
			}
		}

		return m;
	}

	template< typename T, i32_t cols, i32_t rows >
	bool all( const Matrix< T, cols, rows > &mat )
	{
		i32_t i, j;

		for( i = 0; i < cols; ++i ) {
			for( j = 0; j < rows; ++j ) {
				if ( !mat( i, j ) ) {
					return false;
				}
			}
		}

		return true;
	}
	template< typename T, i32_t cols, i32_t rows >
	bool any( const Matrix< T, cols, rows > &mat )
	{
		i32_t i, j;

		for( i = 0; i < cols; ++i ) {
			for( j = 0; j < rows; ++j ) {
				if ( mat( i, j ) ) {
					return true;
				}
			}
		}

		return false;
	}
	template< typename T, i32_t cols, i32_t rows >
	T select( const Matrix< T, cols, rows > &mat )
	{
		i32_t i, j;
		
		for( j = 0; j < rows; ++j ) {
			for( i = 0; i < cols; ++i ) {
				if ( mat( i, j ) ) {
					return mat( i, j );
				}
			}
		}

		return static_cast< T >( 0.f );
	}

	template< typename T, i32_t cols, i32_t rows >
	Matrix< T, cols, rows > operator+( const Matrix< T, cols, rows >& mat, T scalar )
	{
		Matrix< T, cols, rows > m;
		i32_t i, j;

		for( i = 0; i < cols; ++i ) {
			for( j = 0; j < rows; ++j ) {
				m( i, j ) = mat( i, j ) + scalar;
			}
		}

		return m;
	}
	template< typename T, i32_t cols, i32_t rows >
	Matrix< T, cols, rows > operator-( const Matrix< T, cols, rows >& mat, T scalar )
	{
		Matrix< T, cols, rows > m;
		i32_t i, j;

		for( i = 0; i < cols; ++i ) {
			for( j = 0; j < rows; ++j ) {
				m( i, j ) = mat( i, j ) - scalar;
			}
		}

		return m;
	}
	template< typename T, i32_t cols, i32_t rows >
	Matrix< T, cols, rows > operator*( const Matrix< T, cols, rows >& mat, T scalar )
	{
		Matrix< T, cols, rows > m;
		i32_t i, j;

		for( i = 0; i < cols; ++i ) {
			for( j = 0; j < rows; ++j ) {
				m( i, j ) = mat( i, j ) * scalar;
			}
		}

		return m;
	}
	template< typename T, i32_t cols, i32_t rows >
	Matrix< T, cols, rows > operator/( const Matrix< T, cols, rows >& mat, T scalar )
	{
		Matrix< T, cols, rows > m;
		i32_t i, j;

		for( i = 0; i < cols; ++i ) {
			for( j = 0; j < rows; ++j ) {
				m( i, j ) = mat( i, j ) / scalar;
			}
		}

		return m;
	}

	template< typename T, i32_t cols, i32_t rows >
	Matrix< T, cols, rows > operator+( const Matrix< T, cols, rows >& a, const Matrix< T, cols, rows > &b )
	{
		Matrix< T, cols, rows > m;
		i32_t i, j;

		for( i = 0; i < cols; ++i ) {
			for( j = 0; j < rows; ++j ) {
				m( i, j ) = a( i, j ) + b( i, j );
			}
		}

		return m;
	}
	template< typename T, i32_t cols, i32_t rows >
	Matrix< T, cols, rows > operator-( const Matrix< T, cols, rows >& a, const Matrix< T, cols, rows > &b )
	{
		Matrix< T, cols, rows > m;
		i32_t i, j;

		for( i = 0; i < cols; ++i ) {
			for( j = 0; j < rows; ++j ) {
				m( i, j ) = a( i, j ) - b( i, j );
			}
		}

		return m;
	}
	
	
	template< typename T, i32_t cola, i32_t shared, i32_t rowb >
	Matrix< T, cola, rowb > operator*( const Matrix< T, cola, shared > &a, const Matrix< T, shared, rowb > &b )
	{
		Matrix< T, cola, rowb > m;
		i32_t i, j, k;
		T sum;

		for( i = 0; i < cola; ++i ) {
			for( j = 0; j < rowb; ++j ) {
				sum = static_cast< T >( 0.f );
				for( k = 0; k < shared; ++k ) {
					sum += a( i, k ) * b( k, j );
				}
				m( i, j ) = sum;
			}
		}

		return m;
	}

	template< typename T, i32_t cols, i32_t rows >
	Matrix< T, cols, rows > min( const Matrix< T, cols, rows > &a, T b )
	{
		Matrix< T, cols, rows > m;
		i32_t i, j;

		for( i = 0; i < cols; ++i ) {
			for( j = 0; j < rows; ++j ) {
				m( i, j ) = a( i, j ) < b ? a( i, j ) : b;
			}
		}

		return m;
	}
	template< typename T, i32_t cols, i32_t rows >
	Matrix< T, cols, rows > max( const Matrix< T, cols, rows > &a, T b )
	{
		Matrix< T, cols, rows > m;
		i32_t i, j;

		for( i = 0; i < cols; ++i ) {
			for( j = 0; j < rows; ++j ) {
				m( i, j ) = a( i, j ) > b ? a( i, j ) : b;
			}
		}

		return m;
	}
	template< typename T, i32_t cols, i32_t rows >
	Matrix< T, cols, rows > abs( const Matrix< T, cols, rows > &a )
	{
		Matrix< T, cols, rows > m;
		i32_t i, j;

		for( i = 0; i < cols; ++i ) {
			for( j = 0; j < rows; ++j ) {
				m( i, j ) = a( i, j ) >= 0 ? a( i, j ) : -a( i, j );
			}
		}

		return m;
	}
	template< typename T, i32_t cols, i32_t rows >
	Matrix< T, cols, rows > clamp( const Matrix< T, cols, rows > &a, T mini, T maxi )
	{
		Matrix< T, cols, rows > m;
		i32_t i, j;

		for( i = 0; i < cols; ++i ) {
			for( j = 0; j < rows; ++j ) {
				m( i, j ) = a( i, j ) < mini ? mini
							 : a( i, j ) > maxi ? maxi
							 : a( i, j );
			}
		}

		return m;
	}
	template< typename T, i32_t cols, i32_t rows >
	Matrix< T, cols, rows > saturate( const Matrix< T, cols, rows > &a )
	{
		Matrix< T, cols, rows > m;
		i32_t i, j;

		for( i = 0; i < cols; ++i ) {
			for( j = 0; j < rows; ++j ) {
				m( i, j ) = a( i, j ) < static_cast< T >( 0.f ) ? static_cast< T >( 0.f )
							 : a( i, j ) > static_cast< T >( 1.f ) ? static_cast< T >( 1.f )
							 : a( i, j );
			}
		}

		return m;
	}
	template< typename T, i32_t cols, i32_t rows, typename T_t >
	Matrix< T, cols, rows > lerp( const Matrix< T, cols, rows > &mini, const Matrix< T, cols, rows > &maxi, T_t t )
	{
		Matrix< T, cols, rows > m;
		i32_t i, j;
		T_t t1;

		t1 = static_cast< T >( 1.f ) - t;
		for( i = 0; i < cols; ++i ) {
			for( j = 0; j < rows; ++j ) {
				m( i, j ) = ( mini( i, j ) * t ) + ( maxi( i, j ) * t1 );
			}
		}

		return m;
	}
	template< typename T, i32_t cols, i32_t rows >
	T minComponent( const Matrix< T, cols, rows > &a )
	{
		i32_t i, j;
		T mini = std::numeric_limits< T >::max( );

		for( i = 0; i < cols; ++i ) {
			for( j = 0; j < rows; ++j ) {
				mini = a( i, j ) < mini ? a( i, j ) :  mini;
			}
		}

		return mini;
	}
	template< typename T, i32_t cols, i32_t rows >
	T maxComponent( const Matrix< T, cols, rows > &a )
	{
		i32_t i, j;
		T maxi = -std::numeric_limits< T >::max( );

		for( i = 0; i < cols; ++i ) {
			for( j = 0; j < rows; ++j ) {
				maxi = a( i, j ) > maxi ? a( i, j ) :  maxi;
			}
		}

		return maxi;
	}
	
	template< typename T, i32_t cols, i32_t rows >
	Vector< T, rows > operator*( const Matrix< T, cols, rows >& mat, const Vector< T, cols > &vec )
	{
		Vector< T, rows > v;
		i32_t i, j;

#ifdef VUL_CPLUSPLUS11
		v = Vector< T, rows >( static_cast< T >( 0.f ) );
#else
		v = makeVector< T, rows >( static_cast< T >( 0.f ) );
#endif
		for( j = 0; j < rows; ++j ) {
			for( i = 0; i < cols; ++i ) {
				v[ j ] += mat( i, j ) * vec[ i ];
			}
		}

		return v;
	}
	template< typename T, i32_t cols, i32_t rows >
	Vector< T, cols > operator*( const Vector< T, rows > &vec , const Matrix< T, cols, rows >& mat)
	{
		Vector< T, cols > v;
		i32_t i, j;
		
#ifdef VUL_CPLUSPLUS11
		v = Vector< T, cols >( static_cast< T >( 0.f ) );
#else
		v = makeVector< T, cols >( static_cast< T >( 0.f ) );
#endif
		for( i = 0; i < cols; ++i ) {
			for( j = 0; j < rows; ++j ) {
				v[ i ] += mat( i, j ) * vec[ j ];
			}
		}

		return v;
	}
	template< typename T, i32_t cols, i32_t rows >
	Point< T, rows > operator*( const Matrix< T, cols, rows >& mat, const Point< T, cols > &pt )
	{
		Point< T, rows > p;
		i32_t i, j;

#ifdef VUL_CPLUSPLUS11
		p = Point< T, rows >( static_cast< T >( 0.f ) );
#else
		p = makePoint< T, rows >( static_cast< T >( 0.f ) );
#endif
		for( i = 0; i < cols; ++i ) {
			for( j = 0; j < rows; ++j ) {
				p[ j ] += mat( i, j ) * pt[ i ];
			}
		}

		return p;
	}
	template< typename T, i32_t cols, i32_t rows >
	Point< T, cols > operator*( const Point< T, rows > &pt , const Matrix< T, cols, rows > &mat)
	{
		Point< T, cols > p;
		i32_t i, j;
		
#ifdef VUL_CPLUSPLUS11
		p = Point< T, cols >( static_cast< T >( 0.f ) );
#else
		p = makePoint< T, cols >( static_cast< T >( 0.f ) );
#endif
		for( j = 0; j < rows; ++j ) {
			for( i = 0; i < cols; ++i ) {
				p[ i ] += mat( i, j ) * pt[ j ];
			}
		}

		return p;
	}
	
	// Functions

	template< typename T, i32_t n >
	Matrix< T, n, n > inverse( const Matrix< T, n, n > &mat )
	{
		f32_t det;
		i32_t r, c, i, j, a, b;

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
						m( a, b ) = mat( i, j );
						++b;
					}
					++a;
				}
				adjugate( r, c ) = pow( -1.f, c + r ) * determinant( m );
			}
			// Test if determinant is zero here on first iteration; since we avoid doing double work.
			if ( c == 0 ) {
				det = 0;
				for( r = 0; r < n; ++r ) {
					det += mat( c, r ) * adjugate( r, c );
				}
				assert( det != 0 
					&& "vul_matrix.inverse: Determinant is zero, inverse is not defined!" );
			}
		}
		
		return adjugate * ( 1.f / det );
	}
	template< typename T, i32_t cols, i32_t rows >
	Matrix< T, cols, rows > transpose( const Matrix< T, cols, rows > &mat )
	{
		Matrix< T, rows, cols > m;
		i32_t i, j;

		for( i = 0; i < cols; ++i ) {
			for( j = 0; j < rows; ++j ) {
				m( j, i ) = mat( i, j );
			}
		}

		return m;
	}
	
	template< typename T >
	f32_t determinant( const Matrix< T, 2, 2 > &mat )
	{
		return ( f32_t )mat( 0, 0 ) * ( f32_t )mat( 1, 1 )
			   - ( f32_t )mat( 0, 1 ) * ( f32_t )mat( 1, 0 );
	}
	template< typename T, i32_t n >
	f32_t determinant( const Matrix< T, n, n > &mat )
	{
		vul_matrix_zero_helper zero;
		Matrix< T, n-1, n-1 > m;
		i32_t i, j, k, a, b;
		f32_t det;

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
						m( a, b ) = mat( i, j );
						++b;
					}
					++a;
				}
				det += pow( -1.f, zero.index + k ) * mat( zero.index, k ) * determinant( m );
			}
		} else { // VUL_MATRIX_ROW
			for( k = 0; k < n; ++k ) {
				for( j = 0, b = 0; j < n; ++j ) {
					if ( j == zero.index ) {
						continue;
					}
					for( i = 0, a = 0; i < n; ++i ) {
						if ( j == k ) {
							continue;
						}
						m( a, b ) = mat( i, j );
						++a;
					}
					 ++b;
				}
				det += pow( -1.f, zero.index + k ) * mat( k, zero.index ) * determinant( m );
			}
		}

		return det;
	}

	template< typename T, i32_t cols, i32_t rows >
	Vector< T, rows > column( const Matrix< T, cols, rows > &mat, i32_t n )
	{
		Vector< T, rows > v;
		i32_t i;

		for( i = 0; i < rows; ++i ) {
			v[ i ] = mat( n, i );
		}

		return v;
	}
	template< typename T, i32_t cols, i32_t rows >
	Vector< T, cols > row( const Matrix< T, cols, rows > &mat, i32_t n )
	{
		Vector< T, cols > v;
		i32_t i;

		for( i = 0; i < cols; ++i ) {
			v[ i ] = mat( i, n );
		}

		return v;
	}

	// Helper function to speed up determinant calculation
	template< typename T, i32_t cols, i32_t rows > // Helper function. Counts the number of zeros in each column, returning a the index of the column with most zeroes
	vul_matrix_zero_helper vul_matrix_find_most_zero( const Matrix< T, cols, rows > &mat )
	{
		i32_t i, j, c;
		vul_matrix_zero_helper r;

		r.count = std::numeric_limits< i32_t >::max( );
		r.index = 0;

		// Columns
		for( i = 0; i < cols; ++i ) {
			c = 0;
			for( j = 0; j < rows; ++j ) {
				c += mat( i, j ) != 0 ? 0 : 1;
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
				c += mat( i, j ) != 0 ? 0 : 1;
			}
			if ( c < r.count ) {
				r.count = c;
				r.index = i;
				r.type = VUL_MATRIX_ROW;
			}
		}

		return r;
	}

}

#endif
