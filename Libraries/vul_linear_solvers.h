/*
 * Villains' Utility Library - Thomas Martin Schmid, 2015. Public domain¹
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
 * Define VUL_DEFINE in exactly _one_ compilation unit to define the implementation.
 *
 * WARNING: Errors (invalid calls and potential divide-by-zeroes) trigger asserts!
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
#ifndef VUL_LINEAR_SOLVERS_H
#define VUL_LINEAR_SOLVERS_H

#include <math.h>
#include <assert.h>

#ifdef VUL_LINEAR_SOLVERS_ROW_MAJOR
#define IDX( A, r, c, n ) A[ ( r ) * ( n ) + ( c ) ]
#else
#define IDX( A, r, c, n ) A[ ( c ) * ( n ) + ( r ) ]
#endif

#ifndef VUL_LINEAR_SOLVERS_ALLOC
#include <stdlib.h>
#define VUL_LINEAR_SOLVERS_ALLOC malloc
#define VUL_LINEAR_SOLVERS_FREE free
#else
	#ifndef VUL_LINEAR_SOLVERS_FREE
		vul_optimize.h: You must also specify a deallocation function to go with VUL_LINEAR_SOLVERS_ALLOC
	#endif
#endif

//---------------
// Helpers
//

#define DECLARE_VECTOR_OP( name, op ) void name( float *out, float *a, float *b, int n );
#define DEFINE_VECTOR_OP( name, op )\
	void name( float *out, float *a, float *b, int n )\
	{\
		int i;\
		for( i = 0; i < n; ++i ) {\
			out[ i ] = a[ i ] op b[ i ];\
		}\
	}

#ifdef VUL_DEFINE
#define VECTOR_OP DEFINE_VECTOR_OP
#else
#define VECTOR_OP DECLARE_VECTOR_OP
#endif

VECTOR_OP( vulo__vadd, + )
VECTOR_OP( vulo__vsub, - )
VECTOR_OP( vulo__vmul, * )

#ifndef VUL_DEFINE
void vulo__vmul_sub( float *out, float *a, float x, float *b, int n );
#else
void vulo__vmul_sub( float *out, float *a, float x, float *b, int n )
{
	int i;
	for( i = 0; i < n; ++i ) {
		out[ i ] = a[ i ] * x - b[ i ];
	}
}
#endif

#ifndef VUL_DEFINE
void vulo__vmul_add( float *out, float *a, float x, float *b, int n );
#else
void vulo__vmul_add( float *out, float *a, float x, float *b, int n )
{
	int i;
	for( i = 0; i < n; ++i ) {
		out[ i ] = a[ i ] * x + b[ i ];
	}
}
#endif
		

#ifndef VUL_DEFINE
void vulo__vcopy( float *out, float *x, int n );
#else
void vulo__vcopy( float *out, float *x, int n )
{
	int i;
	for( i = 0; i < n; ++i ) {
		out[ i ] = x[ i ];
	}
}
#endif


#ifndef VUL_DEFINE
float vulo__dot( float *a, float *b, int n );
#else
float vulo__dot( float *a, float *b, int n )
{
	float f;
	int i;

	f = a[ 0 ] * b[ 0 ];
	for( i = 1; i < n; ++i ) {
		f += a[ i ] * b[ i ];
	}
	return f;
}
#endif

#ifndef VUL_DEFINE
void vulo__mmul( float *out, float *A, float *x, int n );
#else
void vulo__mmul( float *out, float *A, float *x, int n )
{
	int r, c;
	for( r = 0; r < n; ++r ) {
		out[ r ] = 0;
		for( c = 0; c < n; ++c ) {
			out[ r ] += IDX( A, r, c, n ) * x[ c ];
		}
	}
}
#endif

#ifndef VUL_DEFINE
void vulo__mmul_add( float *out, float *A, float *x, float *b, int n );
#else
void vulo__mmul_add( float *out, float *A, float *x, float *b, int n )
{
	int r, c;
	for( r = 0; r < n; ++r ) {
		out[ r ] = b[ r ];
		for( c = 0; c < n; ++c ) {
			out[ r ] += IDX( A, r, c, n )* x[ c ];
		}
	}
}
#endif

#ifndef VUL_DEFINE
void vulo__forward_substitute( float *out, float *A, float *b, int n );
#else
void vulo__forward_substitute( float *out, float *A, float *b, int n )
{
	int r, c;

	for( r = 0; r < n; ++r ) {
		float sum = b[ r ];
		for( c = 0; c < r; ++c ) {
			sum -= IDX( A, r, c, n ) * out[ c ];
		}
		out[ r ] = sum / IDX( A, r, r, n );
	}
}
#endif

#ifndef VUL_DEFINE
void vulo__backward_substitute( float *out, float *A, float *b, int n );
#else
void vulo__backward_substitute( float *out, float *A, float *b, int n )
{
	int r, c;

	for( r = n - 1; r >= 0; --r ) {
		float sum = b[ r ];
		for( c = r + 1; c < n; ++c ) {
			sum -= IDX( A, r, c, n ) * out[ c ];
		}
		out[ r ] = sum / IDX( A, r, r, n );
	}
}
#endif

//----------------------------------------------
// Optimization functions (the API)
//

#ifndef VUL_DEFINE
void vul_solve_conjugate_gradient_dense( float *out,
										 float *A,
										 float *initial_guess,
										 float *b,
										 int n,
										 int max_iterations,
										 float tolerance );
#else
void vul_solve_conjugate_gradient_dense( float *out,
										 float *A,
										 float *initial_guess,
										 float *b,
										 int n,
										 int max_iterations,
										 float tolerance )
{
	float *x, *r, *Ap, *p;
	float rd, rd2, alpha, beta;
	int i, j;

	r = ( float* )VUL_LINEAR_SOLVERS_ALLOC( sizeof( float ) * n );
	p = ( float* )VUL_LINEAR_SOLVERS_ALLOC( sizeof( float ) * n );
	Ap = ( float* )VUL_LINEAR_SOLVERS_ALLOC( sizeof( float ) * n );
	
	x = out;
	vulo__vcopy( out, initial_guess, n );
	vulo__mmul( r, A, x, n );
	vulo__vsub( r, r, b, n );
	vulo__vcopy( p, r, n );

	rd = vulo__dot( r, r, n );
	for( i = 0; i < max_iterations; ++i ) {
		vulo__mmul( Ap, A, p, n );
		alpha = rd / vulo__dot( p, Ap, n );
		for( j = 0; j < n; ++j ) {
			x[ j ] -= p[ j ] * alpha;
			r[ j ] -= Ap[ j ] * alpha;
		}
		rd2 = vulo__dot( r, r, n );
		if( fabsf( rd2 - rd ) < tolerance * n ) {
			break;
		}
		beta = rd2 / rd;
		for( j = 0; j < n; ++j ) {
			p[ j ] = r[ j ] + p[ j ] * beta;
		}
		rd = rd2;
	}

	VUL_LINEAR_SOLVERS_FREE( p );
	VUL_LINEAR_SOLVERS_FREE( r );
	VUL_LINEAR_SOLVERS_FREE( Ap );
}
#endif

#ifndef VUL_DEFINE
void vul_solve_lu_decomposition_dense( float *out,
									   float *A,
									   float *initial_guess,
									   float *b,
									   int n,
									   int max_iterations,
									   float tolerance );
#else
void vul_solve_lu_decomposition_dense( float *out,
									   float *A,
									   float *initial_guess,
									   float *b,
									   int n,
									   int max_iterations,
									   float tolerance )
{
	float *x, *LU, *r, *scale;
	float sum, rd, rd2, tmp, largest;
	int i, j, k, imax, iold, *indices;

	LU = ( float* )VUL_LINEAR_SOLVERS_ALLOC( sizeof( float ) * n * n );
	r = ( float* )VUL_LINEAR_SOLVERS_ALLOC( sizeof( float ) * n );
	scale = ( float* )VUL_LINEAR_SOLVERS_ALLOC( sizeof( float ) * n );
	indices = ( int* )VUL_LINEAR_SOLVERS_ALLOC( sizeof( int ) * n );
	x = out;
	
	/* Crout's LUP decomposition (without pivoting/scaling, which means it's unstable and bad...@TODO(thynn): Pivoting and scaling!) */
	for( i = 0; i < n; ++i ) {
			largest = 0.f;
			for( j = 0; j < n; ++ j ) {
				if( ( tmp = fabsf( IDX( A, i, j, n ) ) ) > largest ) {
					largest = tmp;
				}
			}
			assert( largest != 0.f ); // LU decomposition is not valid for singular matrices
			scale[ i ] = 1.f / largest;
		}
		for( j = 0; j < n; ++j ) {
			for( i = 0; i < j; ++i ) {
				sum = IDX( A, i, j, n );
				for( k = 0; k < i; ++k ) {
					sum -= IDX( LU, i, k, n ) * IDX( LU, k, j, n );
				}
				IDX( LU, i, j, n ) = sum;
			}

			largest = 0.f;
			for( i = j; i < n; ++i ) {
				sum = IDX( A, j, i, n );
				for( k = 0; k < j; ++k ) {
					sum -= IDX( LU, i, k, n ) * IDX( LU, k, j, n );
				}
				IDX( LU, i, j, n ) = sum;
				if( ( tmp = scale[ i ] * fabsf( sum ) ) >= largest ) {
					largest = tmp;
					imax = i;
				}
			}
			if( j != imax ) { 
				for( k = 0; k < n; ++k ) {
					tmp = IDX( LU, imax, k, n );
					IDX( LU, imax, k, n ) = IDX( LU, j, k, n );
					IDX( LU, j, k ,n ) = tmp;
				}
				scale[ imax ] = scale[ j ];
			}
			indices[ j ] = imax;
			assert( IDX( LU, j, j, n ) != 0.f ); // Pivot element is close enough to zero that we're singular
			if( j != n - 1 ) {
				tmp = 1.f / IDX( LU, j, j, n );
				for( i = j + 1; i < n; ++i ) {
					IDX( LU, i, j, n ) *= tmp;
				}
			}
		}
	
	/* Calculate initial residual */
	vulo__vcopy( x, initial_guess, n );
	vulo__mmul( r, A, x, n );
	vulo__vsub( r, b, r, n );
	rd = vulo__dot( r, r, n );

	for( k = 0; k < max_iterations; ++k ) {
		/* Solve Ly = r (solve for the residual error, not b)*/
		for( i = 0, iold = 0; i < n; ++i ) {
			imax = indices[ i ];
			sum = r[ imax ];
			r[ imax ] = r[ i ];
			if( iold ) {
				for( j = iold; j < i - 1; ++j ) {
					sum -= IDX( A, i, j, n ) * r[ j ];
				}
			} else if( sum ) {
				iold = i;
			}
			r[ i ] = sum;
		}
		/* Solve Ue = y (reuse r as e) */
		vulo__backward_substitute( r, LU, r, n );

		/* Subtract the error from the old solution */
		vulo__vsub( x, x, r, n );

		/* Break if within tolerance */
		rd2 = vulo__dot( r, r, n );
		if( fabs( rd2 - rd ) < tolerance * n ) {
			break;
		}
		vulo__mmul( r, A, x, n );
		vulo__vsub( r, r, b, n );
		rd = rd2;
	}
	
	VUL_LINEAR_SOLVERS_FREE( LU );
	VUL_LINEAR_SOLVERS_FREE( r );
	VUL_LINEAR_SOLVERS_FREE( scale );
	VUL_LINEAR_SOLVERS_FREE( indices );
}
#endif

/**
 * Valid for positive-definite symmetric matrices
 */
#ifndef VUL_DEFINE
void vul_solve_cholesky_decomposition_dense( float *out,
											 float *A,
											 float *initial_guess,
											 float *b,
											 int n,
											 int max_iterations,
											 float tolerance );
#else
void vul_solve_cholesky_decomposition_dense( float *out,
											 float *A,
											 float *initial_guess,
											 float *b,
											 int n,
											 int max_iterations,
											 float tolerance )
{
	float *x, *D, *r;
	float sum, rd, rd2;
	int i, j, k;

	D = ( float* )VUL_LINEAR_SOLVERS_ALLOC( sizeof( float ) * n * n );
	r = ( float* )VUL_LINEAR_SOLVERS_ALLOC( sizeof( float ) * n );
	x = out;
	
	/* Copy the matrix to the work-matrix (@TODO(thynn): This should either be a straight memcpy, or only on the values we need to copy (and weaved in below?)) */
		for( i = 0; i < n; ++i ) {
			for( j = 0; j < n; ++j ) {
				IDX( D, i, j, n ) = IDX( A, i, j, n );
				assert( IDX( A, i, j, n ) == IDX( A, j, i, n ) ); // Cholesky decomposition is only valid for positive-definite SYMMETRIC matrices
			}
		}
	/* Decomposition */
	for( i = 0; i < n; ++i ) {
		for( j = i; j < n; ++j ) {
			sum = IDX( D, i, j, n );
			for( k = i - 1; k >= 0; --k ) {
				sum -= IDX( D, i, k, n ) * IDX( D, j, k, n );
			}
			if( i == j ) {
				assert( sum > 0.f ); // Cholesky decomposition is only valid for POSITIVE-DEFINITE symmetric matrices
				IDX( D, i, i, n ) = sqrtf( sum );
			} else {
				assert( IDX( D, i, i, n ) != 0.f ); // Determinant is sufficiently small that a divide-by-zero is imminent
				IDX( D, j, i, n ) = sum / IDX( D, i, i, n );
			}
		}
	}
	
	/* Calculate initial residual */
	vulo__vcopy( x, initial_guess, n );
	vulo__mmul( r, A, x, n );
	vulo__vsub( r, r, b, n );
	rd = vulo__dot( r, r, n );

	for( k = 0; k < max_iterations; ++k ) {
		/* Solve Ly = r (solve for the residual error, not b)*/
		for( i = 0; i < n; ++i ) {
			sum = r[ i ];
			for( j = i - 1; j >= 0; --j ) {
				sum -= IDX( D, i, j, n ) * r[ j ];
			}
			r[ i ] = sum / IDX( D, i, i, n );
		}
		/* Solve L^Te = y (reuse r as e) */
		for( i = n - 1; i >= 0; --i ) {
			sum = r[ i ];
			for( j = i + 1; j < n; ++j ) {
				sum -= IDX( D, j, i, n ) * r[ j ];
			}
			r[ i ] = sum / IDX( D, i, i, n );
		}
		
		/* Subtract the error from the old solution */
		vulo__vsub( x, x, r, n );

		/* Break if within tolerance */
		vulo__mmul( r, A, x, n );
		vulo__vsub( r, r, b, n );
		rd2 = vulo__dot( r, r, n );
		if( fabs( rd2 - rd ) < tolerance * n ) {
			break;
		}
		rd = rd2;
	}
	
	VUL_LINEAR_SOLVERS_FREE( D );
	VUL_LINEAR_SOLVERS_FREE( r );
}
#endif

#ifndef VUL_DEFINE
void vul_solve_qr_decomposition_dense( float *out,
									   float *A,
									   float *initial_guess,
									   float *b,
									   int n,
									   int max_iterations,
									   float tolerance );
#else
void vul_solve_qr_decomposition_dense( float *out,
									   float *A,
									   float *initial_guess,
									   float *b,
									   int n,
									   int max_iterations,
									   float tolerance )
{
	float *x, *QR, *c, *d, *r;
	float scale, sigma, tau, sum, rd, rd2;
	int i, j, k, singular;

	QR = ( float* )VUL_LINEAR_SOLVERS_ALLOC( sizeof( float ) * n * n );
	d = ( float* )VUL_LINEAR_SOLVERS_ALLOC( sizeof( float ) * n );
	c = ( float* )VUL_LINEAR_SOLVERS_ALLOC( sizeof( float ) * n );
	r = ( float* )VUL_LINEAR_SOLVERS_ALLOC( sizeof( float ) * n );
	x = out;
	
	/* Copy the matrix to the work-matrix (@TODO(thynn): This should either be a straight memcpy, or only on the values we need to copy (and weaved in below?)) */
	for( i = 0; i < n; ++i ) {
		for( j = 0; j < n; ++j ) {
			IDX( QR, i, j, n ) = IDX( A, i, j, n );
		}
	}
	/* Crout's LUP decomposition (without pivoting/scaling, which means it's unstable and bad...@TODO(thynn): Pivoting and scaling!) */
	singular = 0;
	for( k = 0; k < n; ++k ) {
		scale = 0.f;
		for( i = k; i < n; ++i ) {
			scale = fmaxf( scale, fabsf( IDX( QR, i, k, n ) ) );
		}
		if( scale == 0.f ) {
			singular = 1;
			c[ k ] = 0.f;
			d[ k ] = 0.f;
		} else {
			for( i = k; i < n; ++i ) {
				IDX( QR, i, k, n ) /= scale;
			}
			sum = 0.f;
			for( i = k; i < n; ++i ) {
				sum += IDX( QR, i, k, n ) * IDX( QR, i, k, n );
			}
			sigma = IDX( QR, k, k, n ) >= 0.f ? sqrtf( sum ) : -sqrtf( sum );
			IDX( QR, k, k, n ) += sigma;
			c[ k ] = sigma * IDX( QR, k, k, n );
			d[ k ] = -scale * sigma;
			for( j = k + 1; j < n; ++j ) {
				sum = 0.f;
				for( i = k; i < n; ++i ) {
					sum += IDX( QR, i, k, n ) * IDX( QR, i, j, n );
				}
				tau = sum / c[ k ];
				for( i = k; i < n; ++i ) {
					IDX( QR, i, j, n ) -= tau * IDX( QR, i, k, n );
				}
			}
		}
	}
	d[ n - 1 ] = IDX( QR, n - 1, n - 1, n );
	if( d[ n - 1 ] == 0.f ) {
		singular = 1;
	}

	
	/* Calculate initial residual */
	vulo__vcopy( x, initial_guess, n );
	vulo__mmul( r, A, x, n );
	vulo__vsub( r, b, r, n );
	rd = vulo__dot( r, r, n );

	for( k = 0; k < max_iterations; ++k ) {
		/* Form Q^T * b in y */
		for( j = 0; j < n; ++j ) {
				sum = 0.f;
				for( i = j; i < n; ++i ) {
					sum += IDX( QR, i, j, n ) * r[ i ];
				}
				tau = sum / c[ j ];
				for( i = j; i < n; ++i ) {
					r[ i ] -= tau * IDX( QR, i, j, n );
				}
			}
			/* Solve R*x = Q^T*b */
			r[ n - 1 ] /= d[ n - 1 ];
			for( i = n - 2; i >= 0; --i ) {
				sum = r[ i ];
				for( j = i + 1; j < n; ++j ) {
					sum -= IDX( QR, i, j, n ) * r[ j ];
				}
				r[ i ] = sum / d[ i ];
			}
		
		/* Subtract the error from the old solution */
		vulo__vsub( x, x, r, n );

		/* Break if within tolerance */
		rd2 = vulo__dot( r, r, n );
		if( fabs( rd2 - rd ) < tolerance * n ) {
			break;
		}
		rd = rd2;
	}
	
	VUL_LINEAR_SOLVERS_FREE( QR );
	VUL_LINEAR_SOLVERS_FREE( d );
	VUL_LINEAR_SOLVERS_FREE( c );
	VUL_LINEAR_SOLVERS_FREE( r );
}
#endif

#ifndef VUL_DEFINE
void vul_solve_successive_over_relaxation_dense( float *out,
												 float *A,
												 float *initial_guess,
												 float *b,
												 float relaxation_factor,
												 int n,
												 int max_iterations,
												 float tolerance );
#else
void vul_solve_successive_over_relaxation_dense( float *out,
												 float *A,
												 float *initial_guess,
												 float *b,
												 float relaxation_factor,
												 int n,
												 int max_iterations,
												 float tolerance )
{
	float *x, *r;
	int i, j, k;
	float omega, rd, rd2;

	r = ( float* )VUL_LINEAR_SOLVERS_ALLOC( sizeof( float ) * n );
		
	x = out;
	/* Calculate initial residual */
	vulo__vcopy( x, initial_guess, n );
	vulo__mmul( r, A, x, n );
	vulo__vsub( r, r, b, n );
	rd = vulo__dot( r, r, n );
		
	for( k = 0; k < max_iterations; ++k ) {
		/* Relax */
		for( i = 0; i < n; ++i ) {
			omega = 0.f;
			for( j = 0; j < n; ++j ) {
				if( i != j ) {
					omega += IDX( A, i, j, n ) * x[ j ];
				}
			}
			x[ i ] = ( 1.f - relaxation_factor ) * x[ i ]
					+ ( relaxation_factor / IDX( A, i, i, n ) ) * ( b[ i ] - omega );
		}
		/* Check for convergence */
		vulo__mmul( r, A, x, n );
		vulo__vsub( r, r, b, n );
		rd2 = vulo__dot( r, r, n );
		if( fabs( rd2 - rd ) < tolerance * n ) {
			break;
		}
		rd = rd2;
	}
	
	VUL_LINEAR_SOLVERS_FREE( r );
}
#endif


#undef IDX
#undef DECLARE_VECTOR_OP
#undef DEFINE_VECTOR_OP
#undef VECTOR_OP

#endif