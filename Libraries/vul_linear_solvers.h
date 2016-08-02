/*
 * Villains' Utility Library - Thomas Martin Schmid, 2016. Public domain?
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
 * @TODO(thynn): Non-square matrices:
 *	              When r > c, we have extra information, and can do with r = c (so not needed).
 *               When r < c, we don't have a straight forward solution, and must either substitute a
 *               variable, or use SVD or LSF, which are both covered in above TODOs are not implemented yet.
 *               So instead of bothering with this, implement the other two!
 *
 * All solvers are run iteratively until a desired tolerance or a maximum iteration count
 * is reached. Dense [@TODO(thynn): and sparse] matrices are supported. 
 *
 * Define VUL_LINEAR_SOLVERS_ROW_MAJOR to use row major matrices, otherwise column major
 * is assumed (for dense). Sparse matrices are Lists-of-lists, and are always row-major.
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
#ifndef VUL_LINEAR_SOLVERS_H
#define VUL_LINEAR_SOLVERS_H

#include <math.h>
#include <assert.h>
#include <float.h>
#include <string.h>

#ifndef VUL_LINEAR_SOLVERS_ALLOC
#include <stdlib.h>
#define VUL_LINEAR_SOLVERS_ALLOC malloc
#define VUL_LINEAR_SOLVERS_FREE free
#else
	#ifndef VUL_LINEAR_SOLVERS_FREE
		vul_linear_solvers.h: You must also specify a deallocation function to go with VUL_LINEAR_SOLVERS_ALLOC
	#endif
#endif

#ifdef _cplusplus
extern "C" {
#endif

//------------------------
// Math helpers (sparse)

typedef struct vul_solve_sparse_entry {
	unsigned int idx;
	float val;
} vul_solve_sparse_entry;

typedef struct vul_solve_vector {
	vul_solve_sparse_entry *entries;
	unsigned int count;
} vul_solve_vector;

typedef struct vul_solve_matrix_row {
	unsigned int idx;
	vul_solve_vector vec;
} vul_solve_matrix_row;

typedef struct vul_solve_matrix {
	vul_solve_matrix_row *rows;
	unsigned int count;
} vul_solve_matrix;

/*
 * Creates a sparse vector. Takes a list of indices and values to fill it with,
 * or two null pointers to initialize empty.
 */

vul_solve_vector *vul_solve_vector_create( unsigned *idxs, float *vals, unsigned int n );
/*
 * Create a sparse matrix. Takes a list of coordinates and values to fill it with,
 * or three null-pointers to initialize empty.
 */
vul_solve_matrix *vul_solve_matrix_create( unsigned int *rows, unsigned int *cols, float *vals, unsigned int n, unsigned int init_count );

void vul_solve_matrix_destroy( vul_solve_matrix *m );

void vul_solve_vector_insert( vul_solve_vector *v, unsigned int idx, float val );

void vul_solve_matrix_insert( vul_solve_matrix *m, unsigned int r, unsigned int c, float v );

float vul_solve_vector_get( vul_solve_vector *v, unsigned int idx );

float vul_solve_matrix_get( vul_solve_matrix *m, unsigned int r, unsigned int c );

void vul_solve_vector_destroy( vul_solve_vector *v );

#define DECLARE_VECTOR_OP( name, op ) static void name( vul_solve_vector *out, vul_solve_vector *a, vul_solve_vector *b );
DECLARE_VECTOR_OP( vull__sparse_vadd, + )
DECLARE_VECTOR_OP( vull__sparse_vsub, - )
DECLARE_VECTOR_OP( vull__sparse_vmul, * )
#undef DECLARE_VECTOR_OP

static void vull__sparse_vmul_sub( vul_solve_vector *out, vul_solve_vector *a, vul_solve_vector *x, vul_solve_vector *b );
static void vull__sparse_vmul_add( vul_solve_vector *out, vul_solve_vector *a, vul_solve_vector *x, vul_solve_vector *b );
static void vull__sparse_vcopy( vul_solve_vector *out, vul_solve_vector *x );
static float vull__sparse_dot( vul_solve_vector *a, vul_solve_vector *b );
static void vull__sparse_mmul( vul_solve_vector *out, vul_solve_matrix *A, vul_solve_vector *x );
static void vull__sparse_mmul_add( vul_solve_vector *out, vul_solve_matrix *A, vul_solve_vector *x, vul_solve_vector *b );
static void vull__sparse_forward_substitute( vul_solve_vector *out, vul_solve_matrix *A, vul_solve_vector *b );
static void vull__sparse_backward_substitute( vul_solve_vector *out, vul_solve_matrix *A, vul_solve_vector *b );

//--------------------
// Solvers (sparse)


vul_solve_vector *vul_solve_conjugate_gradient_sparse( vul_solve_matrix *A,
																		 vul_solve_vector *initial_guess,
																		 vul_solve_vector *b,
																		 int max_iterations,
																		 float tolerance );

//------------------------
// Math helpers (dense)

#define DECLARE_VECTOR_OP( name, op ) static void name( float *out, float *a, float *b, int n );
DECLARE_VECTOR_OP( vull__vadd, + )
DECLARE_VECTOR_OP( vull__vsub, - )
DECLARE_VECTOR_OP( vull__vmul, * )
#undef DECLARE_VECTOR_OP

static void vull__vmul_sub( float *out, float *a, float x, float *b, int n );
static void vull__vmul_add( float *out, float *a, float x, float *b, int n );
static void vull__vcopy( float *out, float *x, int n );
static float vull__dot( float *a, float *b, int n );
static void vull__mmul( float *out, float *A, float *x, int n );
static void vull__mmul_matrix( float *O, float *A, float *B, int n );
static void vull__mmul_add( float *out, float *A, float *x, float *b, int n );
static void vull__forward_substitute( float *out, float *A, float *b, int n );
static void vull__backward_substitute( float *out, float *A, float *b, int n );

//-------------------
// Solvers (dense)

void vul_solve_conjugate_gradient_dense( float *out,
										 float *A,
										 float *initial_guess,
										 float *b,
										 int n,
										 int max_iterations,
										 float tolerance );
void vul_solve_lu_decomposition_dense( float *out,
									   float *A,
									   float *initial_guess,
									   float *b,
									   int n,
									   int max_iterations,
									   float tolerance );
/**
 * Valid for positive-definite symmetric matrices
 */
void vul_solve_cholesky_decomposition_dense( float *out,
											 float *A,
											 float *initial_guess,
											 float *b,
											 int n,
											 int max_iterations,
											 float tolerance );
void vul_solve_qr_decomposition_dense( float *out,
									   float *A,
									   float *initial_guess,
									   float *b,
									   int n,
									   int max_iterations,
									   float tolerance );
void vul_solve_successive_over_relaxation_dense( float *out,
												 float *A,
												 float *initial_guess,
												 float *b,
												 float relaxation_factor,
												 int n,
												 int max_iterations,
												 float tolerance );

//----------------------------------
// Singular Value Decomposition

typedef struct vul_solve_svd_basis {
	float sigma;
	float *u; int n;
	unsigned int axis;
} vul_solve_svd_basis;

static void vul__solve_svd_sort( vul_solve_svd_basis *x, int n );

/*
 * If transposed is set, treat A as A^T.
 * Matrices may not alias.
 *
 * @TODO(thynn): Faster and more stable version.
 * This currently uses Gram-Schmidt, and has numerical issues as well
 * as speed issues. Should be rewritten to use householder or givens rotations!
 */ 
static void vul__solve_qr_decomposition( float *Q, float *R, float *A, int n, int transposed );

/*
 * Find the largest eigenvalue in the matrix A.
 * Uses the Power method (slow, but simple).
 */
static float vul__solve_largest_eigenvalue( float *A, int n, float eps, int max_iter );

/*
 * Calculate the norm of the matrix' diagonal as a vector.
 */
static float vul__solve_matrix_norm_diagonal( float *A, int n );

/*
 * Only values above the "upper_diag"-th diagonal are used, so for
 * the full matrix, use -min(r,c)/2.
 */
static float vul__solve_matrix_norm_as_single_column( float *A, int n, int upper_diag );

static float vul__solve_matrix_2norm( float *A, int n, float eps, int itermax );

/*
 * Computes the singular value decomposition of A.
 * If rank is set, the maximum of non-zero sigma and rank
 * values and basis vectors are returned.
 *
 * @TODO(thynn): Faster version. This performs repeated QR decomposition
 * of the matrix to reduce it to a diagonal matrix with the singular values,
 * while multiplying out the alternatingly upper and lower triangular matrix
 * produced by the decomposition into U and V.
 */
void vul_svd_dense_slow( vul_solve_svd_basis *out, int *rank,
								 float *A,
								 int r, int c, float eps, int itermax );
/*
 * Computes the singular value decomposition of A.
 * If rank is set, the maximum of non-zero sigma and rank
 * values and basis vectors are returned.
 *
 * This version uses the QR algorithm (or jacobi?)
 * and is faster than the above, but more complex.
 * @TODO(thynn): Actually implement this!
 */
void vul_svd_dense_fast( vul_solve_svd_basis *out, int *rank,
								 float *A,
								 int r, int c, float eps );

#ifdef _cplusplus
}
#endif
#endif

#ifdef VUL_DEFINE

#ifdef _cplusplus
extern "C" {
#endif

//---------------
// Math helpers (sparse)

vul_solve_vector *vul_solve_vector_create( unsigned *idxs, float *vals, unsigned int n )
{
	unsigned int i;
	vul_solve_vector *v;

	v = ( vul_solve_vector* )VUL_LINEAR_SOLVERS_ALLOC( sizeof( vul_solve_vector ) );
	v->count = 0;
	v->entries = 0;
	for( i = 0; i < n; ++i ) {
		vul_solve_vector_insert( v, idxs[ i ], vals[ i ] );
	}
	return v;
}

vul_solve_matrix *vul_solve_matrix_create( unsigned int *rows, unsigned int *cols, float *vals, unsigned int n, unsigned int init_count )
{
	vul_solve_matrix *m;
	unsigned int i;
	
	m = ( vul_solve_matrix* )VUL_LINEAR_SOLVERS_ALLOC( sizeof( vul_solve_matrix ) );
	m->count = 0;
	m->rows = 0;

	for( i = 0; i < init_count; ++i ) {
		vul_solve_matrix_insert( m, rows[ i ], cols[ i ], vals[ i ] );
	}
	return m;
}

void vul_solve_matrix_destroy( vul_solve_matrix *m )
{
	unsigned int i;
	for( i = 0; i < m->count; ++i ) {
		if( m->rows[ i ].vec.count ) {
			VUL_LINEAR_SOLVERS_FREE( m->rows[ i ].vec.entries );
		}
	}
	VUL_LINEAR_SOLVERS_FREE( m->rows );
}

void vul_solve_vector_insert( vul_solve_vector *v, unsigned int idx, float val )
{
	vul_solve_sparse_entry *e;
	unsigned int i, j, inserted;

	for( i = 0; i < v->count; ++i ) {
		if( v->entries[ i ].idx == idx ) {
			v->entries[ i ].val = val;
			return;
		}
	}

	e = ( vul_solve_sparse_entry* )VUL_LINEAR_SOLVERS_ALLOC( sizeof( vul_solve_sparse_entry ) * ( v->count + 1 ) );
	inserted = 0;
	if( v->count ) {
		for( i = 0, j = 0; i < v->count + 1; ++i ) {
			if( j < v->count && ( v->entries[ j ].idx < idx || inserted ) ) {
				e[ i ].idx = v->entries[ j ].idx;
				e[ i ].val = v->entries[ j ].val;
				++j;
			} else {
				e[ i ].idx = idx;
				e[ i ].val = val;
				inserted = 1;
			}
		}
	} else {
		e[ 0 ].idx = idx;
		e[ 0 ].val = val;
	}
	if( v->entries ) {
		VUL_LINEAR_SOLVERS_FREE( v->entries );
	}
	v->entries = e;
	v->count = v->count + 1;
}

void vul_solve_matrix_insert( vul_solve_matrix *m, unsigned int r, unsigned int c, float val )
{
	unsigned int i, j, inserted;
	vul_solve_matrix_row *e;

	for( i = 0; i < m->count; ++i ) {
		if( m->rows[ i ].idx == r ) {
			vul_solve_vector_insert( &m->rows[ i ].vec, c, val );
			return;
		}
	}
	e = ( vul_solve_matrix_row* )VUL_LINEAR_SOLVERS_ALLOC( sizeof( vul_solve_matrix_row ) * ( m->count + 1 ) );
	inserted = 0;
	if( m->count ) {
		for( i = 0, j = 0; i < m->count + 1; ++i ) {
			if( j < m->count && ( m->rows[ j ].idx < r || inserted ) ) {
				e[ i ].idx = m->rows[ j ].idx;
				e[ i ].vec = m->rows[ j ].vec;
				++j;
			} else {
				e[ i ].idx = r;
				e[ i ].vec.count = 0;
				e[ i ].vec.entries = 0;
				vul_solve_vector_insert( &e[ i ].vec, c, val );
				inserted = 1;
			}
		}
	} else {
		e[ 0 ].idx = r;
		e[ 0 ].vec.count = 0;
		e[ 0 ].vec.entries = 0;
		vul_solve_vector_insert( &e[ 0 ].vec, c, val );
	}
	if( m->rows ) {
		VUL_LINEAR_SOLVERS_FREE( m->rows );
	}
	m->rows = e;
	m->count = m->count + 1;
}

float vul_solve_vector_get( vul_solve_vector *v, unsigned int idx )
{
	unsigned int i;

	for( i = 0; i < v->count; ++i ) {
		if( v->entries[ i ].idx == idx ) {
			return v->entries[ i ].val;
		}
	}
	return 0.f;
}

float vul_solve_matrix_get( vul_solve_matrix *m, unsigned int r, unsigned int c )
{
	unsigned int i;

	for( i = 0; i < m->count; ++i ) {
		if( m->rows[ i ].idx == r ) {
			return vul_solve_vector_get( &m->rows[ r ].vec, c );
		}
	}
	return 0.f;
}

void vul_solve_vector_destroy( vul_solve_vector *v )
{
	if( v->entries ) {
		VUL_LINEAR_SOLVERS_FREE( v->entries );
	}
}

#define DEFINE_VECTOR_OP( name, op )\
	void name( vul_solve_vector *out, vul_solve_vector *a, vul_solve_vector *b )\
	{\
		unsigned int ia, ib;\
		ia = 0; ib = 0;\
		while( ia < a->count && ib < b->count ) {\
			if( a->entries[ ia ].idx == b->entries[ ib ].idx ) {\
				vul_solve_vector_insert( out, a->entries[ ia ].idx, a->entries[ ia ].val op b->entries[ ib ].val );\
				++ia; ++ib;\
			} else if( a->entries[ ia ].idx < b->entries[ ib ].idx ) {\
				++ia;\
			} else {\
				++ib;\
			}\
		}\
	}
DEFINE_VECTOR_OP( vull__sparse_vadd, + )
DEFINE_VECTOR_OP( vull__sparse_vsub, - )
DEFINE_VECTOR_OP( vull__sparse_vmul, * )

#undef DEFINE_VECTOR_OP

static void vull__sparse_vmul_sub( vul_solve_vector *out, vul_solve_vector *a, vul_solve_vector *x, vul_solve_vector *b )
{
	unsigned int ia, ix, ib;

	ia= 0; ix = 0; ib = 0;
	while( ia < a->count && ix < x->count && ib < b->count ) {
		if( a->entries[ ia ].idx == x->entries[ ix ].idx && a->entries[ ia ].idx == b->entries[ ib ].idx ) {
			vul_solve_vector_insert( out, a->entries[ ia ].idx, a->entries[ ia ].val * x->entries[ ix ].val -
																				  b->entries[ ib ].val );
			++ia; ++ib; ++ix;
		} else if( b->entries[ ib ].idx <= a->entries[ ia ].idx && b->entries[ ib ].idx <= x->entries[ ix ].idx ) {
			vul_solve_vector_insert( out, b->entries[ ib ].idx, -b->entries[ ib ].val );
			++ib;
		} else if( a->entries[ ia ].idx < x->entries[ ix ].idx ) {
			++ia;
		} else {
			++ix;
		}
	}
}
static void vull__sparse_vmul_add( vul_solve_vector *out, vul_solve_vector *a, vul_solve_vector *x, vul_solve_vector *b )
{
	unsigned int ia, ix, ib;

	ia= 0; ix = 0; ib = 0;
	while( ia < a->count && ix < x->count && ib < b->count ) {
		if( a->entries[ ia ].idx == x->entries[ ix ].idx && a->entries[ ia ].idx == b->entries[ ib ].idx ) {
			vul_solve_vector_insert( out, a->entries[ ia ].idx, a->entries[ ia ].val * x->entries[ ix ].val +
																				  b->entries[ ib ].val );
			++ia; ++ib; ++ix;
		} else if( b->entries[ ib ].idx <= a->entries[ ia ].idx && b->entries[ ib ].idx <= x->entries[ ix ].idx ) {
			vul_solve_vector_insert( out, b->entries[ ib ].idx, b->entries[ ib ].val );
			++ib;
		} else if( a->entries[ ia ].idx < x->entries[ ix ].idx ) {
			++ia;
		} else {
			++ix;
		}
	}
}

static void vull__sparse_vcopy( vul_solve_vector *out, vul_solve_vector *x )
{
	unsigned int i;

	if( x->count ) {
		out->entries = ( vul_solve_sparse_entry* )VUL_LINEAR_SOLVERS_ALLOC( sizeof( vul_solve_sparse_entry ) * 
																									x->count );
		for( i = 0; i < x->count; ++i ) {
			out->entries[ i ].idx = x->entries[ i ].idx;
			out->entries[ i ].val = x->entries[ i ].val;
		}
		out->count = x->count;
	} else {
		out->entries = 0;
		out->count = 0;
	}
}
static float vull__sparse_dot( vul_solve_vector *a, vul_solve_vector *b )
{
	float r;
	unsigned int ia, ib;
	
	r = 0.f; ia = 0; ib = 0;
	while( ia < a->count && ib < b->count ) {
		if( a->entries[ ia ].idx == b->entries[ ib ].idx ) {
			r += a->entries[ ia ].val * b->entries[ ib ].val;
			++ia; ++ib;
		} else if( a->entries[ ia ].idx < b->entries[ ib ].idx ) {
			++ia;
		} else {
			++ib;
		}
	}
	return r;
}
static void vull__sparse_mmul( vul_solve_vector *out, vul_solve_matrix *A, vul_solve_vector *x )
{
	unsigned int v, i, ix;
	float sum;

	for( v = 0; v < A->count; ++v ) {
		sum = 0.f;
		i = 0; ix = 0;
		while( i < A->rows[ v ].vec.count && ix < x->count ) {
			if( A->rows[ v ].vec.entries[ i ].idx == x->entries[ ix ].idx ) {
				sum += A->rows[ v ].vec.entries[ i ].val * x->entries[ ix ].val;
				++i; ++ix;
			} else if( A->rows[ v ].vec.entries[ i ].idx < x->entries[ ix ].idx ) {
				++i;
			} else {
				++ix;
			}
		}
		vul_solve_vector_insert( out, A->rows[ v ].idx, sum );
	}
}
static void vull__sparse_mmul_add( vul_solve_vector *out, vul_solve_matrix *A, vul_solve_vector *x, vul_solve_vector *b )
{
	unsigned int v, i, ix, ib, found;
	float sum;

	for( v = 0; v < A->count; ++v ) {
		sum = 0.f;
		i = 0; ix = 0; ib = 0;
		while( i < A->rows[ v ].vec.count && ix < x->count && ib < b->count ) {
			if( A->rows[ v ].vec.entries[ i ].idx == x->entries[ ix ].idx && 
				 A->rows[ v ].vec.entries[ i ].idx == b->entries[ ib ].idx ) {
				sum += A->rows[ v ].vec.entries[ i ].val * x->entries[ ix ].val + b->entries[ ib ].val;
				++i; ++ib; ++ix;
			} else if( b->entries[ ib ].idx <= A->rows[ v ].vec.entries[ i ].idx && 
						  b->entries[ ib ].idx <= x->entries[ ix ].idx ) {
				sum += b->entries[ ib ].val;
				++ib;
			} else if( A->rows[ v ].vec.entries[ i ].idx < x->entries[ ix ].idx ) {
				++i;
			} else {
				++ix;
			}
		}
		vul_solve_vector_insert( out, A->rows[ v ].idx, sum );
	}
}
static void vull__sparse_forward_substitute( vul_solve_vector *out, vul_solve_matrix *A, vul_solve_vector *b )
{
	unsigned int v, i, io;
	float sum;

	for( v = 0; v < A->count; ++v ) {
		sum = vul_solve_vector_get( b, A->rows[ v ].idx );
		i = 0; io = 0;
		if( A->rows[ v ].vec.count && out->count ) {
			while( i < A->rows[ v ].vec.count && io < out->count ) {
				if( A->rows[ v ].vec.entries[ i ].idx > A->rows[ v ].idx ) {
					break;
				}
				if( A->rows[ v ].vec.entries[ i ].idx == out->entries[ io ].idx ) {
					sum -= A->rows[ v ].vec.entries[ i ].val * out->entries[ io ].val;
					++i; ++io;
				} else if( A->rows[ v ].vec.entries[ i ].idx < out->entries[ io ].idx ) {
					++i;
				} else {
					++io;
				}
			}
			if( sum != 0.f ) {
				vul_solve_vector_insert( out, A->rows[ v ].idx, 
												 sum / vul_solve_vector_get( &A->rows[ v ].vec, A->rows[ v ].idx ) );
			}
		}
	}
}
static void vull__sparse_backward_substitute( vul_solve_vector *out, vul_solve_matrix *A, vul_solve_vector *b )
{
	int v;
	unsigned i, io;
	float sum;

	for( v = A->count - 1; v >= 0; ++v ) {
		sum = vul_solve_vector_get( b, A->rows[ v ].idx );
		i = 0; io = 0;
		if( A->rows[ v ].vec.count && out->count ) {
			while( i < A->rows[ v ].vec.count && io < out->count ) {
				if( A->rows[ v ].vec.entries[ i ].idx < A->rows[ v ].idx + 1 ) {
					++i;
					continue;
				}
				if( A->rows[ v ].vec.entries[ i ].idx == out->entries[ io ].idx ) {
					sum -= A->rows[ v ].vec.entries[ i ].val * out->entries[ io ].val;
					++i; ++io;
				} else if( A->rows[ v ].vec.entries[ i ].idx < out->entries[ io ].idx ) {
					++i;
				} else {
					++io;
				}
			}
			if( sum != 0.f ) {
				vul_solve_vector_insert( out, A->rows[ v ].idx, 
												 sum / vul_solve_vector_get( &A->rows[ v ].vec, A->rows[ v ].idx ) );
			}
		}
	}
}

//--------------------
// Solvers (sparse)

vul_solve_vector *vul_solve_conjugate_gradient_sparse( vul_solve_matrix *A,
																		 vul_solve_vector *initial_guess,
																		 vul_solve_vector *b,
																		 int max_iterations,
																		 float tolerance )
{
	vul_solve_vector *x, *r, *Ap, *p;
	float rd, rd2, alpha, beta, tmp;
	int i, j, k, idx, found;

	r = vul_solve_vector_create( 0, 0, 0 );
	p = vul_solve_vector_create( 0, 0, 0 );
	Ap = vul_solve_vector_create( 0, 0, 0 );
	
	x = vul_solve_vector_create( 0, 0, 0 );
	vull__sparse_vcopy( x, initial_guess );
	vull__sparse_mmul( r, A, x );
	vull__sparse_vsub( r, r, b );
	vull__sparse_vcopy( p, r );

	rd = vull__sparse_dot( r, r );
	for( i = 0; i < max_iterations; ++i ) {
		vull__sparse_mmul( Ap, A, p );
		alpha = rd / vull__sparse_dot( p, Ap );
		for( j = 0; j < p->count; ++j ) {
			idx = p->entries[ j ].idx;
			tmp = p->entries[ j ].val * alpha;
			found = 0;
			for( k = 0; k < x->count; ++k ) {
				if( x->entries[ k ].idx == idx ) {
					x->entries[ k ].val -= tmp;
					found = 1;
					break;
				}
			}
			if( !found ) {
				vul_solve_vector_insert( x, idx, -tmp );
			}
		}
		for( j = 0; j < Ap->count; ++j ) {
			idx = Ap->entries[ j ].idx;
			tmp = Ap->entries[ j ].val * alpha;
			found = 0;
			for( k = 0; k < r->count; ++k ) {
				if( r->entries[ k ].idx == idx ) {
					r->entries[ k ].val -= tmp;
					found = 1;
					break;
				}
			}
			if( !found ) {
				vul_solve_vector_insert( r, idx, -tmp );
			}
		}
		rd2 = vull__sparse_dot( r, r );
		if( fabsf( rd2 - rd ) < tolerance * r->count ) {
			break;
		}
		beta = rd2 / rd;
		for( j = 0, k = 0; j < p->count && k < r->count; ) {
			if( p->entries[ j ].idx == r->entries[ k ].idx ) {
				p->entries[ j ].val = r->entries[ k ].val + p->entries[ j ].val * beta;
				++j; ++k;
			} else if( p->entries[ j ].idx < r->entries[ k ].idx ) {
				++j;
			} else {
				vul_solve_vector_insert( p, r->entries[ k ].idx, r->entries[ k ].val );
				++k;
			}
		}
		rd = rd2;
	}

	vul_solve_vector_destroy( p );
	vul_solve_vector_destroy( r );
	vul_solve_vector_destroy( Ap );
	return x;
}

vul_solve_vector *vul_solve_successive_over_relaxation_sparse( vul_solve_matrix *A,
																					vul_solve_vector *initial_guess,
																					vul_solve_vector *b,
																					float relaxation_factor,
																					int max_iterations,
																					float tolerance )
{
	vul_solve_vector *x, *r;
	int i, j, k;
	float omega, rd, rd2, tmp;

	r = vul_solve_vector_create( 0, 0, 0 );
	x = vul_solve_vector_create( 0, 0, 0 );
		
	/* Calculate initial residual */
	vull__sparse_vcopy( x, initial_guess );
	vull__sparse_mmul( r, A, x );
	vull__sparse_vsub( r, r, b );
	rd = vull__sparse_dot( r, r );
		
	for( k = 0; k < max_iterations; ++k ) {
		/* Relax */
		for( i = 0; i < A->count; ++i ) {
			omega = 0.f;
			for( j = 0; j < x->count; ++j ) {
				if( A->rows[ i ].idx != x->entries[ j ].idx ) {
					omega += vul_solve_matrix_get( A, A->rows[ i ].idx, x->entries[ j ].idx ) * x->entries[ j ].val;
				}
			}
			tmp = ( 1.f - relaxation_factor ) * vul_solve_vector_get( x, A->rows[ i ].idx ) 
				 + ( relaxation_factor / vul_solve_matrix_get( A, A->rows[ i ].idx, A->rows[ i ].idx ) )
				 * ( vul_solve_vector_get( b, A->rows[ i ].idx ) - omega );
			if( tmp != 0.f ) {
				vul_solve_vector_insert( x, A->rows[ i ].idx, tmp );
			}
		}
		/* Check for convergence */
		vull__sparse_mmul( r, A, x );
		vull__sparse_vsub( r, r, b );
		rd2 = vull__sparse_dot( r, r );
		if( fabs( rd2 - rd ) < tolerance * x->count ) {
			break;
		}
		rd = rd2;
	}
	
	vul_solve_vector_destroy( r );
	return x;
}

//---------------
// Math helpers (dense)
	
#ifdef VUL_LINEAR_SOLVERS_ROW_MAJOR
#define IDX( A, r, c, n ) A[ ( r ) * ( n ) + ( c ) ]
#else
#define IDX( A, r, c, n ) A[ ( c ) * ( n ) + ( r ) ]
#endif

#define DEFINE_VECTOR_OP( name, op )\
	void name( float *out, float *a, float *b, int n )\
	{\
		int i;\
		for( i = 0; i < n; ++i ) {\
			out[ i ] = a[ i ] op b[ i ];\
		}\
	}
DEFINE_VECTOR_OP( vull__vadd, + )
DEFINE_VECTOR_OP( vull__vsub, - )
DEFINE_VECTOR_OP( vull__vmul, * )

#undef DEFINE_VECTOR_OP

static void vull__vmul_sub( float *out, float *a, float x, float *b, int n )
{
	int i;
	for( i = 0; i < n; ++i ) {
		out[ i ] = a[ i ] * x - b[ i ];
	}
}

static void vull__vmul_add( float *out, float *a, float x, float *b, int n )
{
	int i;
	for( i = 0; i < n; ++i ) {
		out[ i ] = a[ i ] * x + b[ i ];
	}
}
		

static void vull__vcopy( float *out, float *x, int n )
{
	int i;
	for( i = 0; i < n; ++i ) {
		out[ i ] = x[ i ];
	}
}

static float vull__dot( float *a, float *b, int n )
{
	float f;
	int i;

	f = a[ 0 ] * b[ 0 ];
	for( i = 1; i < n; ++i ) {
		f += a[ i ] * b[ i ];
	}
	return f;
}

static void vull__mmul( float *out, float *A, float *x, int n )
{
	int r, c;
	for( r = 0; r < n; ++r ) {
		out[ r ] = 0;
		for( c = 0; c < n; ++c ) {
			out[ r ] += IDX( A, r, c, n ) * x[ c ];
		}
	}
}
static void vull__mmul_matrix( float *O, float *A, float *B, int n )
{
	float s;
	int i, j, k;
	for( i = 0; i < n; ++i ) {
		for( j = 0; j < n; ++j ) {
			s = 0.f;
			for( k = 0; k < n; ++k ) {
				s += IDX( A, i, k, n ) * IDX( B, k, j, n );
			}
			IDX( O, i, j, n ) = s;
		}
	}
}

static void vull__mmul_add( float *out, float *A, float *x, float *b, int n )
{
	int r, c;
	for( r = 0; r < n; ++r ) {
		out[ r ] = b[ r ];
		for( c = 0; c < n; ++c ) {
			out[ r ] += IDX( A, r, c, n )* x[ c ];
		}
	}
}

static void vull__forward_substitute( float *out, float *A, float *b, int n )
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

static void vull__backward_substitute( float *out, float *A, float *b, int n )
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

//---------------
// Solvers

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
	vull__vcopy( out, initial_guess, n );
	vull__mmul( r, A, x, n );
	vull__vsub( r, r, b, n );
	vull__vcopy( p, r, n );

	rd = vull__dot( r, r, n );
	for( i = 0; i < max_iterations; ++i ) {
		vull__mmul( Ap, A, p, n );
		alpha = rd / vull__dot( p, Ap, n );
		for( j = 0; j < n; ++j ) {
			x[ j ] -= p[ j ] * alpha;
			r[ j ] -= Ap[ j ] * alpha;
		}
		rd2 = vull__dot( r, r, n );
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
	vull__vcopy( x, initial_guess, n );
	vull__mmul( r, A, x, n );
	vull__vsub( r, b, r, n );
	rd = vull__dot( r, r, n );

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
		vull__backward_substitute( r, LU, r, n );

		/* Subtract the error from the old solution */
		vull__vsub( x, x, r, n );

		/* Break if within tolerance */
		rd2 = vull__dot( r, r, n );
		if( fabs( rd2 - rd ) < tolerance * n ) {
			break;
		}
		vull__mmul( r, A, x, n );
		vull__vsub( r, r, b, n );
		rd = rd2;
	}
	
	VUL_LINEAR_SOLVERS_FREE( LU );
	VUL_LINEAR_SOLVERS_FREE( r );
	VUL_LINEAR_SOLVERS_FREE( scale );
	VUL_LINEAR_SOLVERS_FREE( indices );
}

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
	vull__vcopy( x, initial_guess, n );
	vull__mmul( r, A, x, n );
	vull__vsub( r, r, b, n );
	rd = vull__dot( r, r, n );

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
		vull__vsub( x, x, r, n );

		/* Break if within tolerance */
		vull__mmul( r, A, x, n );
		vull__vsub( r, r, b, n );
		rd2 = vull__dot( r, r, n );
		if( fabs( rd2 - rd ) < tolerance * n ) {
			break;
		}
		rd = rd2;
	}
	
	VUL_LINEAR_SOLVERS_FREE( D );
	VUL_LINEAR_SOLVERS_FREE( r );
}

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
	vull__vcopy( x, initial_guess, n );
	vull__mmul( r, A, x, n );
	vull__vsub( r, b, r, n );
	rd = vull__dot( r, r, n );

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
		vull__vsub( x, x, r, n );

		/* Break if within tolerance */
		rd2 = vull__dot( r, r, n );
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
	vull__vcopy( x, initial_guess, n );
	vull__mmul( r, A, x, n );
	vull__vsub( r, r, b, n );
	rd = vull__dot( r, r, n );
		
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
		vull__mmul( r, A, x, n );
		vull__vsub( r, r, b, n );
		rd2 = vull__dot( r, r, n );
		if( fabs( rd2 - rd ) < tolerance * n ) {
			break;
		}
		rd = rd2;
	}
	
	VUL_LINEAR_SOLVERS_FREE( r );
}

//------------------------------
// Singular value decomposition
// 

static void vul__solve_svd_sort( vul_solve_svd_basis *x, int n )
{
	vul_solve_svd_basis tmp;
	int gap, gi, i, j;
	int gaps[ ] = { 701, 301, 132, 67, 23, 10, 4, 1 };

	for( gi = 0; gi < sizeof( gaps ) / sizeof( int ); ++gi ) {
		for( i = 0 + gaps[ gi ]; i < n; ++i ) {
			gap = gaps[ gi ];
			memcpy( &tmp, &x[ i ], sizeof( vul_solve_svd_basis ) );
			for( j = i; j >= gap && x[ j - gap ].sigma <= tmp.sigma; j -= gap ) {
				memcpy( &x[ j ], &x[ j - gap ], sizeof( vul_solve_svd_basis ) );
			}
			memcpy( &x[ j ], &tmp, sizeof( vul_solve_svd_basis ) );
		}
	}
}

static void vul__solve_qr_decomposition( float *Q, float *R, float *A, int n, int transposed )
{
	// @TODO(thynn): Do this with householder rotations (or givens rotations) instead of this.

	// Gram-Schmidt; numerically bad and slow, but simple.
	float *u, *a, d, tmp;
	int i, j, k;

	u = ( float* )VUL_LINEAR_SOLVERS_ALLOC( sizeof( float ) * n * n );
	a = ( float* )VUL_LINEAR_SOLVERS_ALLOC( sizeof( float ) * n );

	if( transposed ) {
		// Fill Q
		for( i = 0; i < n; ++i ) {
			for( j = 0; j < n; ++j ) {
				a[ j ] = A[ i * n + j ];
			}
			d = 0.f;
			for( j = 0; j < n; ++j ) {
				u[ i * n + j ] = a[ j ];
				for( k = 0; k < i; ++k ) {
					tmp = vull__dot( &u[ k * n ], &u[ k * n ], n );
					if( tmp != 0.f ) {
						u[ i * n + j ] -= u[ k * n + j ] * ( vull__dot( &u[ k * n ], a, n ) / tmp );
					}
				}
				d += u[ i * n + j ] * u[ i * n + j ];
			}
			if( d != 0.f ) {
				d = 1.f / sqrtf( d );
			}
			for( j = 0; j < n; ++j ) {
				Q[ j * n + i ] = u[ i * n + j ] * d;
			}
		}

		// Fill R
		for( i = 0; i < n; ++i ) {
			for( j = 0; j < n; ++j ) {
				R[ i * n + j ] = 0.f;
				for( k = 0; k < n; ++k ) {
					R[ i * n + j ] += Q[ k * n + i ] * A[ j * n + k ]; // Q^T, A^T
				}
			}
		}
	} else {
		// Fill Q
		for( i = 0; i < n; ++i ) {
			for( j = 0; j < n; ++j ) {
				a[ j ] = A[ j * n + i ];
			}
			d = 0.f;
			for( j = 0; j < n; ++j ) {
				u[ i * n + j ] = a[ j ];
				for( k = 0; k < i; ++k ) {
					tmp = vull__dot( &u[ k * n ], &u[ k * n ], n );
					if( tmp != 0.f ) {
						u[ i * n + j ] -= u[ k * n + j ] * ( vull__dot( &u[ k * n ], a, n ) / tmp );
					}
				}
				d += u[ i * n + j ] * u[ i * n + j ];
			}
			if( d != 0.f ) {
				d = 1.f / sqrtf( d );
			}
			for( j = 0; j < n; ++j ) {
				Q[ j * n + i ] = u[ i * n + j ] * d;
			}
		}

		// Fill R
		for( i = 0; i < n; ++i ) {
			for( j = 0; j < n; ++j ) {
				R[ i * n + j ] = 0.f;
				for( k = 0; k < n; ++k ) {
					R[ i * n + j ] += Q[ k * n + i ] * A[ k * n + j ]; // Q^T
				}
			}
		}
	}

	VUL_LINEAR_SOLVERS_FREE( u );
	VUL_LINEAR_SOLVERS_FREE( a );
}

// @TODO(thynn): Write "find Nth largest eigenvalue" version that sorts a copy of the y-vector and
// selects axis and norm for the Nth element instead of the largest.
static float vul__solve_largest_eigenvalue( float *A, int n, float eps, int max_iter )
{
	int iter, axis, normaxis, i;
	float *v, *y, lambda, norm, err;

	// Power method. Slow, but simple
	v = ( float* )VUL_LINEAR_SOLVERS_ALLOC( sizeof( float ) * n );
	y = ( float* )VUL_LINEAR_SOLVERS_ALLOC( sizeof( float ) * n );
	memset( v, 0, sizeof( float ) * n );
	v[ 0 ] = 1.f;

	err = eps * 2;
	iter = 0;
	axis = 0;
	lambda = 0.f;
	while( err > eps && iter++ < max_iter ) {
		vull__mmul( y, A, v, n );
		err = fabs( lambda - y[ axis ] );
		lambda = y[ axis ];
		norm = -FLT_MAX;
		for( i = 0; i < n; ++i ) {
			if( y[ i ] > norm ) {
				norm = y[ i ];
				normaxis = i;
			}
		}
		axis = normaxis;
		for( i = 0; i < n; ++i ) {
			v[ i ] = y[ i ] / norm;
		}
	}
	return lambda;
}

static float vul__solve_matrix_norm_diagonal( float *A, int n )
{
	float v;
	int i;
	
	v = 0.f;
	for( i = 0; i < n; ++i ) {
		v += A[ i * n + i ] * A[ i * n + i ];
	}
	return sqrtf( v );
}

static float vul__solve_matrix_norm_as_single_column( float *A, int n, int upper_diag )
{
	float v;
	int i, j;

	v = 0.f;
	for( i = 0; i < n; ++i ) {
		for( j = i + upper_diag < 0 ? 0 : i + upper_diag;
			  j < n; ++j ) {
			v += A[ i * n + j ] * A[ i * n + j ];
		}
	}
	return v;
}

static float vul__solve_matrix_2norm( float *A, int n, float eps, int itermax )
{
	float v, *U, *T, *AA, e;
	int i, j, k;

	U = ( float* )VUL_LINEAR_SOLVERS_ALLOC( sizeof( float ) * n * n );
	T = ( float* )VUL_LINEAR_SOLVERS_ALLOC( sizeof( float ) * n * n );
	AA = ( float* )VUL_LINEAR_SOLVERS_ALLOC( sizeof( float ) * n * n );
	memset( U, 0, sizeof( float ) * n * n );
	memset( T, 0, sizeof( float ) * n * n );
	for( i = 0; i < n; ++i ) {
		for( j = 0;j < n; ++j ) {
			U[ j * n + i ] = A[ j * n + i ];
			T[ i * n + j ] = A[ j * n + i ];
		}
	}
	for( i = 0; i < n; ++i ) {
		for( j = 0; j < n; ++j ) {
			v = 0;
			for( k = 0; k < n; ++k ) {
				v += T[ i * n + k ] * U[ k * n + j ];
			}
			AA[ i * n + j ] = v;
		}
	}
	// Find the larges eigenvalue!
	e = vul__solve_largest_eigenvalue( A, n, eps, itermax ); 
	return sqrtf( e * e );
}

void vul_svd_dense_slow( vul_solve_svd_basis *out, int *rank,
								 float *A,
								 int r, int c, float eps, int itermax )
{
	float *U0, *U1, *V0, *V1, *S0, *S1, *Q, err, e, f, *tp;
	int iter, n, i, j, ri, ci;

	n = r > c ? r : c;
	U0 = ( float* )VUL_LINEAR_SOLVERS_ALLOC( sizeof( float ) * r * r );
	U1 = ( float* )VUL_LINEAR_SOLVERS_ALLOC( sizeof( float ) * r * r );
	V0 = ( float* )VUL_LINEAR_SOLVERS_ALLOC( sizeof( float ) * c * c );
	V1 = ( float* )VUL_LINEAR_SOLVERS_ALLOC( sizeof( float ) * c * c );
	S0 = ( float* )VUL_LINEAR_SOLVERS_ALLOC( sizeof( float ) * n * n );
	S1 = ( float* )VUL_LINEAR_SOLVERS_ALLOC( sizeof( float ) * n * n );
	Q = ( float* )VUL_LINEAR_SOLVERS_ALLOC( sizeof( float ) * n * n );
	memset( U0, 0, sizeof( float ) * r * r );
	memset( U1, 0, sizeof( float ) * r * r );
	memset( V0, 0, sizeof( float ) * c * c );
	memset( V1, 0, sizeof( float ) * c * c );
	memset( S0, 0, sizeof( float ) * n * n );
	memset( Q, 0, sizeof( float ) * n * n );
	iter = 0;
	err = FLT_MAX;

	// Initialize to empty for U, V, Q. Set S to A^T
	for( ci = 0; ci < c; ++ci ) {
		for( ri = 0; ri < r; ++ri ) {
			S0[ ci * c + ri ] = A[ ri * c + ci ];
		}
	}

	// Initialize as identity matrix
	for( i = 0; i < r; ++i ) {
		U0[ i * r + i ] = 1.f;
	}
	for( i = 0; i < c; ++i ) {
		V0[ i * c + i ] = 1.f;
	}
	while( err > eps && iter++ < itermax ) {
		// Decompose
		vul__solve_qr_decomposition( Q, S1, S0, n, 1 );
		vull__mmul_matrix( U1, U0, Q, r );
		vul__solve_qr_decomposition( Q, S0, S1, n, 1 );
		vull__mmul_matrix( V1, V0, Q, c );
		tp = U0;
		U0 = U1;
		U1 = tp;
		tp = V0;
		V0 = V1;
		V1 = tp;

		// Calculate error
		e = vul__solve_matrix_norm_as_single_column( S0, n, 1 );
		f = vul__solve_matrix_norm_diagonal( S0, n );
		if( f == 0.f ) {
			f = 1.f;
		}
		err = e / f;
	}

	// Grap sigmas and rank, sort decreasing
	for( j = 0, i = 0; i < n; ++i ) {
		out[ i ].sigma = fabs( S0[ i * n + i ] );
		out[ i ].axis = i;
		if( out[ i ].sigma != 0.f ) {
			++j;
		}
	}
	if( *rank == 0 || j < *rank ) {
		*rank = j;
	}
	vul__solve_svd_sort( out, n );

	// Fix signs and copy U
	for( i = 0; i < *rank; ++i ) {
		out[ i ].n = r;
		out[ i ].u = ( float* )VUL_LINEAR_SOLVERS_ALLOC( sizeof( float ) * r );
		f = S0[ out[ i ].axis * n + out[ i ].axis ] < 0.f ? -1.f : 1.f;
		for( j = 0; j < r; ++j ) {
			out[ i ].u[ j ] = U0[ j * r + out[ i ].axis ] * f;
		}
	}

	VUL_LINEAR_SOLVERS_FREE( U0 );
	VUL_LINEAR_SOLVERS_FREE( U1 );
	VUL_LINEAR_SOLVERS_FREE( V0 );
	VUL_LINEAR_SOLVERS_FREE( V1 );
	VUL_LINEAR_SOLVERS_FREE( S0 );
	VUL_LINEAR_SOLVERS_FREE( S1 );
	VUL_LINEAR_SOLVERS_FREE( Q );
}

void vul_svd_dense_fast( vul_solve_svd_basis *out, int *rank,
								 float *A,
								 int r, int c, float eps )
{
	assert( 0 && "Stub!" );
	// If r >> c, perform QR decomposition

	// Use householder reflections to reduce to bidiagonal matrix
	
	// Finally, compute eigenvalues (either with the QR algorithm, or with jacobi orthoganalization)
}

#undef IDX

#ifdef _cplusplus
}
#endif

#endif
