/*
 * Villains' Utility Library - Thomas Martin Schmid, 2017. Public domain?
 *
 * Some commonly useful linear system solvers (solvers of the linear system
 * Ax = b). The following solvers are supported:
 *  -Conjugate gradient method
 *  -LU decomposition
 *  -QR decomposition [BROKEN]
 *  -Cholesky decomposition
 *  -Successive over-relaxation
 * with planned future support for:
 *  -@TODO(thynn): Sinular value decomposition with fitting for general least squares
 *  -@TODO(thynn): Levenberg-Marquardt for non-linear fitting problems
 *  -@TODO(thynn): Newton-raphson for non-linear (both local and global with line searching and backtracking)
 *  -@TODO(thynn): Broyden's method for non-linear
 *
 * @TODO(thynn): Non-square matrices!
 *
 * All solvers are run iteratively until a desired tolerance or a maximum iteration count
 * is reached. Dense [@TODO(thynn): and sparse] matrices are supported. 
 *
 * Define VUL_LINEAR_SOLVERS_ROW_MAJOR to use row major matrices, otherwise column major
 * is assumed.
 * 
 * If you wish to use a costum allocator for temporaries, define VUL_LINEAR_SOLVERS_ALLOC
 * and VUL_LINEAR_SOLVERS_FREE to functions with signatures similar to malloc and free, which
 * are used if no alternative is given.
 *
 * All solvers leave input intact; only the "out"-argument is altered.
 *
 * WARNING: Errors (invalid calls and potential divide-by-zeroes) trigger asserts!
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
#ifndef VUL_LINEAR_HPP
#define VUL_LINEAR_HPP

#include "vul_types.hpp"
#include "vul_vector.hpp"
#include "vul_matrix.hpp"

namespace vul {
	
	//---------------
	// Helpers
	//

	// This exists to speed up common operations but isn't really needed elsewhere....
	// @TODO(thynn): Just move it to vec/mat??
	template< typename T, s32 n >
	void mulAdd( Vector< T, n > &out,
				 const Matrix< T, n, n > &A, 
				 const Vector< T, n > &x,
				 const Vector< T, n > &b )
	{
		s32 i, j;
		for( j = 0; j < n; ++j ) {
			out.data[ j ] = b.data[ j ];
			for( i = 0; i < n; ++i ) {
				out.data[ j ] += A.data[ i ][ j ] * x.data[ i ];
			}
		}
	}

	template< typename T, s32 n >
	void forwardSubstitute( Vector< T, n > &out,
							const Matrix< T, n, n > &A,
							const Vector< T, n > &b )
	{
		s32 r, c;
		for( r = 0; r < n; ++r ) {
			T sum = b.data[ r ];
			for( c = 0; c < r; ++c ) {
				sum -= A.data[ r ][ c ] * out.data[ c ];
			}
			out.data[ r ] = sum / A.data[ r ][ r ];
		}
	}
	
	template< typename T, s32 n >
	void backwardSubstitute( Vector< T, n > &out,
							const Matrix< T, n, n > &A,
							const Vector< T, n > &b )
	{
		s32 r, c;
		for( r = n - 1; r >= 0; --r ) {
			T sum = b.data[ r ];
			for( c = r + 1; c < n; ++c ) {
				sum -= A.data[ r ][ c ] * out.data[ c ];
			}
			out.data[ r ] = sum / A.data[ r ][ r ];
		}
	}

	//-------------------
	// Linear solvers
	//
	
	template< typename T, s32 n >
	Vector< T, n > solveConjugateGradient( const Matrix< T, n, n > &A, 
										   const Vector< T, n > &guess,
										   const Vector< T, n > &b,
										   s32 maxIterations,
										   T tolerance )
	{
		Vector< T, n > p, r, Ap, x;
		T rd, rd2, alpha, beta;
		s32 i;

		x = guess;
		mulAdd( r, A, x, -b );
		p = r;

		rd = dot( r, r );
		for( i = 0; i < maxIterations; ++i ) {
			Ap = A * p;
			alpha = rd / dot( p, Ap );
			x -= p * alpha;
			r -= Ap * alpha;
			rd2 = dot( r, r );

			/* Break if within tolerance */
			if( std::abs( rd2 - rd ) < tolerance * n ) {
				break;
			}
			beta = rd2 / rd;
			p = r + p * beta;
			rd = rd2;
		}

		return x;
	}

	template< typename T, s32 n >
	Vector< T, n > solveLUDecomposition( Matrix< T, n, n > &A,
										 const Vector< T, n > &guess,
										 const Vector< T, n > &b,
										 s32 maxIterations,
										 T tolerance )
	{
		Vector< T, n > x, r, scale;
		Vector< s32, n > indices;
		Matrix< T, n, n > LU;		
		T sum, rd, rd2, tmp, largest;
		s32 i, j, k, imax, iold;

		/* Crout's LUP decomposition (without pivoting/scaling, which means it's unstable and bad...@TODO(thynn): Pivoting and scaling!) */
		for( i = 0; i < n; ++i ) {
			T largest = T( 0.f );
			for( j = 0; j < n; ++ j ) {
				if( ( tmp = std::abs( A( i, j ) ) ) > largest ) {
					largest = tmp;
				}
			}
			assert( largest != T( 0.f ) ); // LU decomposition is not valid for singular matrices
			scale[ i ] = T( 1.f ) / largest;
		}
		for( j = 0; j < n; ++j ) {
			for( i = 0; i < j; ++i ) {
				sum = A( i, j );
				for( k = 0; k < i; ++k ) {
					sum -= LU( i, k ) * LU( k, j );
				}
				LU( i, j ) = sum;
			}

			largest = T( 0.f );
			for( i = j; i < n; ++i ) {
				sum = A( j, i );
				for( k = 0; k < j; ++k ) {
					sum -= LU( i, k ) * LU( k, j );
				}
				LU( i, j ) = sum;
				if( ( tmp = scale[ i ] * std::abs( sum ) ) >= largest ) {
					largest = tmp;
					imax = i;
				}
			}
			if( j != imax ) { 
				for( k = 0; k < n; ++k ) {
					tmp = LU( imax, k );
					LU( imax, k ) = LU( j, k );
					LU( j, k ) = tmp;
				}
				scale[ imax ] = scale[ j ];
			}
			indices[ j ] = imax;
			assert( LU( j, j ) != T( 0.f ) ); // Pivot element is close enough to zero that we're singular
			if( j != n - 1 ) {
				tmp = 1.f / LU( j, j );
				for( i = j + 1; i < n; ++i ) {
					LU( i, j ) *= tmp;
				}
			}
		}

		x = guess;
		mulAdd( r, A, x, -b ); // Initial residual
		rd = dot( r, r );

		for( k = 0; k < maxIterations; ++k ) {
			/* Solve Ly = r (solve for the residual error, not b)*/
			for( i = 0, iold = 0; i < n; ++i ) {
				imax = indices[ i ];
				sum = r[ imax ];
				r[ imax ] = r[ i ];
				if( iold ) {
					for( j = iold; j < i - 1; ++j ) {
						sum -= A( i, j ) * r[ j ];
					}
				} else if( sum ) {
					iold = i;
				}
				r[ i ] = sum;
			}
			/* Solve Ue = y (reuse r as e) */
			backwardSubstitute( r, LU, r );

			/* Subtract the error from the old solution */
			x -= r;

			/* Break if within tolerance */
			rd2 = dot( r, r );
			if( std::abs( rd2 - rd ) < tolerance * n ) {
				break;
			}
			mulAdd( r, A, x, -b ); // Initial residual
			rd = rd2;
		}
	
		return x;
	}
	
	template< typename T, s32 n >
	Vector< T, n > solveCholeskyDecomposition( Matrix< T, n, n > &A,
											   const Vector< T, n > &guess,
											   const Vector< T, n > &b,
											   s32 maxIterations,
											   T tolerance )
	{
		Vector< T, n > x, r;
		Matrix< T, n, n > D;
		T sum, rd, rd2;
		s32 i, j, k;

		/* Copy the matrix to the work-matrix (@TODO(thynn): This should either be a straight memcpy, or only on the values we need to copy (and weaved in below?)) */
		for( i = 0; i < n; ++i ) {
			for( j = 0; j < n; ++j ) {
				D( i, j ) = A( i, j );
				assert( A( i, j ) == A( j, i ) ); // Cholesky decomposition is only valid for positive-definite SYMMETRIC matrices
			}
		}
		/* Decomposition */
		for( i = 0; i < n; ++i ) {
			for( j = i; j < n; ++j ) {
				sum = D( i, j );
				for( k = i - 1; k >= 0; --k ) {
					sum -= D( i, k ) * D( j, k );
				}
				if( i == j ) {
					assert( sum > T( 0.f ) ); // Cholesky decomposition is only valid for POSITIVE-DEFINITE symmetric matrices
					D( i, i ) = sqrt( sum );
				} else {
					assert( D( i, i ) != 0.f ); // Determinant is sufficiently small that a divide-by-zero is imminent
					D( j, i ) = sum / D( i, i );
				}
			}
		}
	
		/* Calculate initial residual */
		x = guess;
		mulAdd( r, A, x, -b );
		rd = dot( r, r );

		for( k = 0; k < maxIterations; ++k ) {
			/* Solve Ly = r (solve for the residual error, not b)*/
			for( i = 0; i < n; ++i ) {
				sum = r[ i ];
				for( j = i - 1; j >= 0; --j ) {
					sum -= D( i, j ) * r[ j ];
				}
				r[ i ] = sum / D( i, i );
			}
			/* Solve L^Te = y (reuse r as e) */
			for( i = n - 1; i >= 0; --i ) {
				sum = r[ i ];
				for( j = i + 1; j < n; ++j ) {
					sum -= D( j, i ) * r[ j ];
				}
				r[ i ] = sum / D( i, i );
			}

			/* Subtract the error from the old solution */
			x -= r;

			/* Break if within tolerance */
			mulAdd( r, A, x, -b );
			rd2 = dot( r, r );
			if( std::abs( rd2 - rd ) < tolerance * n ) {
				break;
			}
			rd = rd2;
		}

		return x;
	}

	template< typename T, s32 n >
	Vector< T, n > solveQRDecomposition( Matrix< T, n, n > &A,
										 const Vector< T, n > &guess,
										 const Vector< T, n > &b,
										 s32 maxIterations,
										 T tolerance )
	{
		Vector< T, n > x, r, c, d;
		Matrix< T, n, n > QR;		
		T scale, sigma, tau, sum, rd, rd2;
		s32 i, j, k;
		b32 singular;

		/* Copy the matrix to the work-matrix (@TODO(thynn): This should either be a straight memcpy, or only on the values we need to copy (and weaved in below?)) */
		for( i = 0; i < n; ++i ) {
			for( j = 0; j < n; ++j ) {
				QR( i, j ) = A( i, j );
			}
		}
		/* Crout's LUP decomposition (without pivoting/scaling, which means it's unstable and bad...@TODO(thynn): Pivoting and scaling!) */
		singular = false;
		for( k = 0; k < n; ++k ) {
			scale = T( 0.f );
			for( i = k; i < n; ++i ) {
				scale = std::max( scale, std::abs( QR( i, k ) ) );
			}
			if( scale == T( 0.f ) ) {
				singular = true;
				c[ k ] = T( 0.f );
				d[ k ] = T( 0.f );
			} else {
				for( i = k; i < n; ++i ) {
					QR( i, k ) /= scale;
				}
				sum = T( 0.f );
				for( i = k; i < n; ++i ) {
					sum += QR( i, k ) * QR( i, k );
				}
				sigma = QR( k, k ) >= 0.f ? sqrt( sum ) : -sqrt( sum );
				QR( k, k ) += sigma;
				c[ k ] = sigma * QR( k, k );
				d[ k ] = -scale * sigma;
				for( j = k + 1; j < n; ++j ) {
					sum = 0.f;
					for( i = k; i < n; ++i ) {
						sum += QR( i, k ) * QR( i, j );
					}
					tau = sum / c[ k ];
					for( i = k; i < n; ++i ) {
						QR( i, j ) -= tau * QR( i, k );
					}
				}
			}
		}
		d[ n - 1 ] = QR( n - 1, n - 1 );
		if( d[ n - 1 ] == 0.f ) {
			singular = true;
		}

		x = guess;
		mulAdd( r, A, x, -b ); // Initial residual
		rd = dot( r, r );

		for( k = 0; k < maxIterations; ++k ) {
			/* Form Q^T * b in r */
			for( j = 0; j < n; ++j ) {
				sum = 0.f;
				for( i = j; i < n; ++i ) {
					sum += QR( i, j ) * r[ i ];
				}
				tau = sum / c[ j ];
				for( i = j; i < n; ++i ) {
					r[ i ] -= tau * QR( i, j );
				}
			}
			/* Solve R*x = Q^T*b */
			r[ n - 1 ] /= d[ n - 1 ];
			for( i = n - 2; i >= 0; --i ) {
				sum =  r[ i ];
				for( j = i + 1; j < n; ++j ) {
					sum -= QR( i, j ) * r[ j ];
				}
				r[ i ] = sum / d[ i ];
			}

			/* Subtract the error from the old solution */
			x -= r;

			/* Break if within tolerance */
			rd2 = dot( r, r );
			if( std::abs( rd2 - rd ) < tolerance * n ) {
				break;
			}
			rd = rd2;
		}
	
		return x;
	}
	
	template< typename T, s32 n >
	Vector< T, n > solveSOR( Matrix< T, n, n > &A,
							 const Vector< T, n > &guess,
							 const Vector< T, n > &b,
							 T relaxationFactor,
							 s32 maxIterations,
							 T tolerance )
	{
		Vector< T, n > x, r;
		s32 i, j, k;
		T omega, rd, rd2;
		
		x = guess;
		mulAdd( r, A, x, -b ); // Initial residual
		rd = dot( r, r );
		
		for( k = 0; k < maxIterations; ++k ) {
			/* Relax */
			for( i = 0; i < n; ++i ) {
				omega = T( 0.f );
				for( j = 0; j < n; ++j ) {
					if( i != j ) {
						omega += A( i, j ) * x[ j ];
					}
				}
				x[ i ] = ( T( 1.f ) - relaxationFactor ) * x[ i ]
					   + ( relaxationFactor / A( i, i ) ) * ( b[ i ] - omega );
			}
			/* Check for convergence */
			rd2 = dot( r, r );
			if( std::abs( rd2 - rd ) < tolerance * n ) {
				break;
			}
			rd = rd2;
		}
	
		return x;
	}

	//-----------------------------
	// Non-linear solvers
	// TODO: this
};
#endif