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

#ifdef VUL_SOLVE_DOUBLE
#define vul_solve_real double
#else
#define vul_solve_real float
#endif

#ifdef _cplusplus
extern "C" {
#endif

//------------------------
// Math helpers (sparse)

typedef struct vul_solve_sparse_entry {
	unsigned int idx;
	vul_solve_real val;
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

vul_solve_vector *vul_solve_vector_create( unsigned *idxs, vul_solve_real *vals, unsigned int n );
/*
 * Create a sparse matrix. Takes a list of coordinates and values to fill it with,
 * or three null-pointers to initialize empty.
 */
vul_solve_matrix *vul_solve_matrix_create( unsigned int *rows, unsigned int *cols, vul_solve_real *vals, unsigned int n, unsigned int init_count );

void vul_solve_matrix_destroy( vul_solve_matrix *m );

void vul_solve_vector_insert( vul_solve_vector *v, unsigned int idx, vul_solve_real val );

void vul_solve_matrix_insert( vul_solve_matrix *m, unsigned int r, unsigned int c, vul_solve_real v );

vul_solve_real vul_solve_vector_get( vul_solve_vector *v, unsigned int idx );

vul_solve_real vul_solve_matrix_get( vul_solve_matrix *m, unsigned int r, unsigned int c );

void vul_solve_vector_destroy( vul_solve_vector *v );

#define DECLARE_VECTOR_OP( name, op ) static void name( vul_solve_vector *out, vul_solve_vector *a, vul_solve_vector *b );
DECLARE_VECTOR_OP( vull__sparse_vadd, + )
DECLARE_VECTOR_OP( vull__sparse_vsub, - )
DECLARE_VECTOR_OP( vull__sparse_vmul, * )
#undef DECLARE_VECTOR_OP

static void vull__sparse_vmul_sub( vul_solve_vector *out, vul_solve_vector *a, vul_solve_vector *x, vul_solve_vector *b );
static void vull__sparse_vmul_add( vul_solve_vector *out, vul_solve_vector *a, vul_solve_vector *x, vul_solve_vector *b );
static void vull__sparse_vcopy( vul_solve_vector *out, vul_solve_vector *x );
static vul_solve_real vull__sparse_dot( vul_solve_vector *a, vul_solve_vector *b );
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
																		 vul_solve_real tolerance );

//------------------------
// Math helpers (dense)

#define DECLARE_VECTOR_OP( name, op ) static void name( vul_solve_real *out, vul_solve_real *a, vul_solve_real *b, int n );
DECLARE_VECTOR_OP( vull__vadd, + )
DECLARE_VECTOR_OP( vull__vsub, - )
DECLARE_VECTOR_OP( vull__vmul, * )
#undef DECLARE_VECTOR_OP

static void vull__vmul_sub( vul_solve_real *out, vul_solve_real *a, vul_solve_real x, vul_solve_real *b, int n );
static void vull__vmul_add( vul_solve_real *out, vul_solve_real *a, vul_solve_real x, vul_solve_real *b, int n );
static void vull__vcopy( vul_solve_real *out, vul_solve_real *x, int n );
static vul_solve_real vull__dot( vul_solve_real *a, vul_solve_real *b, int n );
static void vull__mmul( vul_solve_real *out, vul_solve_real *A, vul_solve_real *x, int c, int r );
static void vull__mmul_matrix( vul_solve_real *O, vul_solve_real *A, vul_solve_real *B, int n );
static void vull__mmul_add( vul_solve_real *out, vul_solve_real *A, vul_solve_real *x, vul_solve_real *b, 
									 int c, int r );
static void vull__forward_substitute( vul_solve_real *out, vul_solve_real *A, vul_solve_real *b, int c, int r );
static void vull__backward_substitute( vul_solve_real *out, vul_solve_real *A, vul_solve_real *b, int c, int r );
static void vull__mtranspose( vul_solve_real *O, vul_solve_real *A, int c, int r );
static void vull__mmul_matrix_rect( vul_solve_real *O, vul_solve_real *A, vul_solve_real *B, int ca, int ra_cb, int rb );

//-------------------
// Solvers (dense)

void vul_solve_conjugate_gradient_dense( vul_solve_real *out,
										 vul_solve_real *A,
										 vul_solve_real *initial_guess,
										 vul_solve_real *b,
										 int n,
										 int max_iterations,
										 vul_solve_real tolerance );
void vul_solve_lu_decomposition_dense( vul_solve_real *out,
									   vul_solve_real *A,
									   vul_solve_real *initial_guess,
									   vul_solve_real *b,
									   int n,
									   int max_iterations,
									   vul_solve_real tolerance );
/**
 * Valid for positive-definite symmetric matrices
 */
void vul_solve_cholesky_decomposition_dense( vul_solve_real *out,
											 vul_solve_real *A,
											 vul_solve_real *initial_guess,
											 vul_solve_real *b,
											 int n,
											 int max_iterations,
											 vul_solve_real tolerance );
void vul_solve_qr_decomposition_dense( vul_solve_real *out,
									   vul_solve_real *A,
									   vul_solve_real *initial_guess,
									   vul_solve_real *b,
									   int n,
									   int max_iterations,
									   vul_solve_real tolerance );
void vul_solve_successive_over_relaxation_dense( vul_solve_real *out,
												 vul_solve_real *A,
												 vul_solve_real *initial_guess,
												 vul_solve_real *b,
												 vul_solve_real relaxation_factor,
												 int n,
												 int max_iterations,
												 vul_solve_real tolerance );

//----------------------------------
// Singular Value Decomposition

typedef struct vul_solve_svd_basis {
	vul_solve_real sigma;
	vul_solve_real *u, *v;
	unsigned int u_length, v_length;
	unsigned int axis;
} vul_solve_svd_basis;

void vul_solve_svd_basis_reconstruct_matrix( float *M, vul_solve_svd_basis *x, int n );

void vul_solve_svd_basis_destroy( vul_solve_svd_basis *x, int n );

static void vul__solve_svd_sort( vul_solve_svd_basis *x, int n );

/*
 * If transposed is set, treat A as A^T.
 * Matrices may not alias.
 */ 
static void vul__solve_qr_decomposition_gram_schmidt( vul_solve_real *Q, vul_solve_real *R, vul_solve_real *A, 
																		int c, int r, int transpose );
/*
 * If transposed is set, treat A as A^T.
 * Matrices may alias.
 */
static void vul__solve_qr_decomposition_householder( vul_solve_real *Q, vul_solve_real *R, vul_solve_real *A, 
																	  int c, int r, int transpose );
/*
 * If transposed is set, treat A as A^T.
 * Matrices may not alias.
 */
static void vul__solve_qr_decomposition_givens( vul_solve_real *Q, vul_solve_real *R, vul_solve_real *A, 
																int c, int r, int transpose );

/*
 * Find the largest eigenvalue in the matrix A.
 * Uses the Power method (slow, but simple).
 */
static vul_solve_real vul__solve_largest_eigenvalue( vul_solve_real *A, int c, int r, vul_solve_real eps, int max_iter );

/*
 * Calculate the norm of the matrix' diagonal as a vector.
 */
static vul_solve_real vul__solve_matrix_norm_diagonal( vul_solve_real *A, int c, int r );

/*
 * Only values above the "upper_diag"-th diagonal are used, so for
 * the full matrix, use -min(r,c)/2.
 */
static vul_solve_real vul__solve_matrix_norm_as_single_column( vul_solve_real *A, int c, int r, int upper_diag );

static vul_solve_real vul__solve_matrix_2norm( vul_solve_real *A, int c, int r, vul_solve_real eps, int itermax );

/*
 * Computes the singular value decomposition of A.
 * If rank is set, the maximum of non-zero sigma and rank
 * values and basis vectors are returned.
 */
void vul_solve_svd_dense( vul_solve_svd_basis *out, int *rank,
								  vul_solve_real *A,
								  int c, int r, vul_solve_real eps, int itermax );

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

vul_solve_vector *vul_solve_vector_create( unsigned *idxs, vul_solve_real *vals, unsigned int n )
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

vul_solve_matrix *vul_solve_matrix_create( unsigned int *rows, unsigned int *cols, vul_solve_real *vals, unsigned int n, unsigned int init_count )
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
	VUL_LINEAR_SOLVERS_FREE( m );
}

void vul_solve_vector_insert( vul_solve_vector *v, unsigned int idx, vul_solve_real val )
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

void vul_solve_matrix_insert( vul_solve_matrix *m, unsigned int r, unsigned int c, vul_solve_real val )
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

vul_solve_real vul_solve_vector_get( vul_solve_vector *v, unsigned int idx )
{
	unsigned int i;

	for( i = 0; i < v->count; ++i ) {
		if( v->entries[ i ].idx == idx ) {
			return v->entries[ i ].val;
		}
	}
	return 0.f;
}

vul_solve_real vul_solve_matrix_get( vul_solve_matrix *m, unsigned int r, unsigned int c )
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
	VUL_LINEAR_SOLVERS_FREE( v );
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
static vul_solve_real vull__sparse_dot( vul_solve_vector *a, vul_solve_vector *b )
{
	vul_solve_real r;
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
	vul_solve_real sum;

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
	vul_solve_real sum;

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
	vul_solve_real sum;

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
	vul_solve_real sum;

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
																		 vul_solve_real tolerance )
{
	vul_solve_vector *x, *r, *Ap, *p;
	vul_solve_real rd, rd2, alpha, beta, tmp;
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
		if( fabs( rd2 - rd ) < tolerance * r->count ) {
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
																					vul_solve_real relaxation_factor,
																					int max_iterations,
																					vul_solve_real tolerance )
{
	vul_solve_vector *x, *r;
	int i, j, k;
	vul_solve_real omega, rd, rd2, tmp;

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

// @NOTE(thynn): The reason we index y over x is that I fucked up, and initially had them mixed up,
// and once I discovered the bug I had been consistent enough that simply swapping indices meant it all worked.
// So now we index (y,x) instead of (x,y). Oops. But it's only internal in this file anyway.
#ifdef VUL_LINEAR_SOLVERS_ROW_MAJOR
#define IDX( A, y, x, c, r ) A[ ( y ) * ( c ) + ( x ) ]
#else
#define IDX( A, y, x, c, r ) A[ ( x ) * ( r ) + ( y ) ]
#endif

#define DEFINE_VECTOR_OP( name, op )\
	void name( vul_solve_real *out, vul_solve_real *a, vul_solve_real *b, int n )\
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

static inline void vull__swap_ptr( vul_solve_real **a, vul_solve_real **b )
{
	vul_solve_real *t = *a;
	*a = *b;
	*b = t;
}

static void vull__vmul_sub( vul_solve_real *out, vul_solve_real *a, vul_solve_real x, vul_solve_real *b, int n )
{
	int i;
	for( i = 0; i < n; ++i ) {
		out[ i ] = a[ i ] * x - b[ i ];
	}
}

static void vull__vmul_add( vul_solve_real *out, vul_solve_real *a, vul_solve_real x, vul_solve_real *b, int n )
{
	int i;
	for( i = 0; i < n; ++i ) {
		out[ i ] = a[ i ] * x + b[ i ];
	}
}
		

static void vull__vcopy( vul_solve_real *out, vul_solve_real *x, int n )
{
	int i;
	for( i = 0; i < n; ++i ) {
		out[ i ] = x[ i ];
	}
}

static vul_solve_real vull__dot( vul_solve_real *a, vul_solve_real *b, int n )
{
	vul_solve_real f;
	int i;

	f = a[ 0 ] * b[ 0 ];
	for( i = 1; i < n; ++i ) {
		f += a[ i ] * b[ i ];
	}
	return f;
}

static void vull__mmul( vul_solve_real *out, vul_solve_real *A, vul_solve_real *x, int c, int r )
{
	int i, j;
	for( i = 0; i < r; ++i ) {
		out[ i ] = 0;
		for( j = 0; j < c; ++j ) {
			out[ i ] += IDX( A, i, j, c, r ) * x[ j ];
		}
	}
}
static void vull__mmul_matrix( vul_solve_real *O, vul_solve_real *A, vul_solve_real *B, int n )
{
	vul_solve_real s;
	int i, j, k;
	for( i = 0; i < n; ++i ) {
		for( j = 0; j < n; ++j ) {
			s = 0.f;
			for( k = 0; k < n; ++k ) {
				s += IDX( A, i, k, n, n ) * IDX( B, k, j, n, n );
			}
			IDX( O, i, j, n, n ) = s;
		}
	}
}

static void vull__mmul_add( vul_solve_real *out, vul_solve_real *A, vul_solve_real *x, vul_solve_real *b, 
									 int c, int r )
{
	int i, j;
	for( i = 0; i < r; ++i ) {
		out[ i ] = b[ i ];
		for( j = 0; j < c; ++j ) {
			out[ i ] += IDX( A, i, j, c, r )* x[ j ];
		}
	}
}

static void vull__forward_substitute( vul_solve_real *out, vul_solve_real *A, vul_solve_real *b, int c, int r )
{
	int i, j;

	for( i = 0; i < r; ++i ) {
		vul_solve_real sum = b[ i ];
		for( j = 0; j < c; ++j ) {
			sum -= IDX( A, i, j, c, r ) * out[ j ];
		}
		out[ i ] = sum / IDX( A, i, i, c, r );
	}
}

static void vull__backward_substitute( vul_solve_real *out, vul_solve_real *A, vul_solve_real *b, int c, int r )
{
	int i, j;

	for( i = r - 1; i >= 0; --i ) {
		vul_solve_real sum = b[ i ];
		for( j = i + 1; j < c; ++j ) {
			sum -= IDX( A, i, j, c, r ) * out[ j ];
		}
		out[ i ] = sum / IDX( A, i, i, c, r );
	}
}
static void vull__mtranspose( vul_solve_real *O, vul_solve_real *A, int c, int r )
{
	int i, j, k;

	if( r == c ) {
		// Square is trivial and cache friendly
		for( i = 0; i < r; ++i ) {
			for( j = 0; j < c; ++j ) {
				IDX( O, i, j, c, r ) = IDX( A, j, i, c, r );
			}
		}
	} else {
		// We can't just iterate over the columns and rows; will lead to duplication/leaving some out.
		// Instead, iterate over the elements and calculate their index.
		for( k = 0; k < r * c; ++k ) {
			i = k % c;
			j = k / c;
			IDX( O, i, j, r, c ) = IDX( A, j, i, c, r );
		}
	}
}
static void vull__mmul_matrix_rect( vul_solve_real *O, vul_solve_real *A, vul_solve_real *B, int ra, int rb_ca, int cb )
{
	int i, j, k;
	vul_solve_real d;

	for( i = 0; i < ra; ++i ) {
		for( j = 0; j < cb; ++j ) {
			d = 0.f;
			for( k = 0; k < rb_ca; ++k ) {
				d += IDX( A, i, k, rb_ca, ra ) * IDX( B, k, j, cb, rb_ca );
			}
			IDX( O, i, j, cb, ra ) = d;
		}
	}
}

//---------------
// Solvers

void vul_solve_conjugate_gradient_dense( vul_solve_real *out,
										 vul_solve_real *A,
										 vul_solve_real *initial_guess,
										 vul_solve_real *b,
										 int n,
										 int max_iterations,
										 vul_solve_real tolerance )
{
	vul_solve_real *x, *r, *Ap, *p;
	vul_solve_real rd, rd2, alpha, beta;
	int i, j;

	r = ( vul_solve_real* )VUL_LINEAR_SOLVERS_ALLOC( sizeof( vul_solve_real ) * n );
	p = ( vul_solve_real* )VUL_LINEAR_SOLVERS_ALLOC( sizeof( vul_solve_real ) * n );
	Ap = ( vul_solve_real* )VUL_LINEAR_SOLVERS_ALLOC( sizeof( vul_solve_real ) * n );
	
	x = out;
	vull__vcopy( out, initial_guess, n );
	vull__mmul( r, A, x, n, n );
	vull__vsub( r, r, b, n );
	vull__vcopy( p, r, n );

	rd = vull__dot( r, r, n );
	for( i = 0; i < max_iterations; ++i ) {
		vull__mmul( Ap, A, p, n, n );
		alpha = rd / vull__dot( p, Ap, n );
		for( j = 0; j < n; ++j ) {
			x[ j ] -= p[ j ] * alpha;
			r[ j ] -= Ap[ j ] * alpha;
		}
		rd2 = vull__dot( r, r, n );
		if( fabs( rd2 - rd ) < tolerance * n ) {
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

void vul_solve_lu_decomposition_dense( vul_solve_real *out,
									   vul_solve_real *A,
									   vul_solve_real *initial_guess,
									   vul_solve_real *b,
									   int n,
									   int max_iterations,
									   vul_solve_real tolerance )
{
	vul_solve_real *x, *LU, *r, *scale;
	vul_solve_real sum, rd, rd2, tmp, largest;
	int i, j, k, imax, iold, *indices;

	LU = ( vul_solve_real* )VUL_LINEAR_SOLVERS_ALLOC( sizeof( vul_solve_real ) * n * n );
	r = ( vul_solve_real* )VUL_LINEAR_SOLVERS_ALLOC( sizeof( vul_solve_real ) * n );
	scale = ( vul_solve_real* )VUL_LINEAR_SOLVERS_ALLOC( sizeof( vul_solve_real ) * n );
	indices = ( int* )VUL_LINEAR_SOLVERS_ALLOC( sizeof( int ) * n );
	x = out;
	
	/* Crout's LUP decomposition (without pivoting/scaling, which means it's unstable and bad...@TODO(thynn): Pivoting and scaling!) */
	for( i = 0; i < n; ++i ) {
			largest = 0.f;
			for( j = 0; j < n; ++ j ) {
				if( ( tmp = fabs( IDX( A, i, j, n, n ) ) ) > largest ) {
					largest = tmp;
				}
			}
			assert( largest != 0.f ); // LU decomposition is not valid for singular matrices
			scale[ i ] = 1.f / largest;
		}
		for( j = 0; j < n; ++j ) {
			for( i = 0; i < j; ++i ) {
				sum = IDX( A, i, j, n, n );
				for( k = 0; k < i; ++k ) {
					sum -= IDX( LU, i, k, n, n ) * IDX( LU, k, j, n, n );
				}
				IDX( LU, i, j, n, n ) = sum;
			}

			largest = 0.f;
			for( i = j; i < n; ++i ) {
				sum = IDX( A, j, i, n, n );
				for( k = 0; k < j; ++k ) {
					sum -= IDX( LU, i, k, n, n ) * IDX( LU, k, j, n, n );
				}
				IDX( LU, i, j, n, n ) = sum;
				if( ( tmp = scale[ i ] * fabs( sum ) ) >= largest ) {
					largest = tmp;
					imax = i;
				}
			}
			if( j != imax ) { 
				for( k = 0; k < n; ++k ) {
					tmp = IDX( LU, imax, k, n, n );
					IDX( LU, imax, k, n, n ) = IDX( LU, j, k, n, n );
					IDX( LU, j, k ,n, n ) = tmp;
				}
				scale[ imax ] = scale[ j ];
			}
			indices[ j ] = imax;
			assert( IDX( LU, j, j, n, n ) != 0.f ); // Pivot element is close enough to zero that we're singular
			if( j != n - 1 ) {
				tmp = 1.f / IDX( LU, j, j, n, n );
				for( i = j + 1; i < n; ++i ) {
					IDX( LU, i, j, n, n ) *= tmp;
				}
			}
		}
	
	/* Calculate initial residual */
	vull__vcopy( x, initial_guess, n );
	vull__mmul( r, A, x, n, n );
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
					sum -= IDX( A, i, j, n, n ) * r[ j ];
				}
			} else if( sum ) {
				iold = i;
			}
			r[ i ] = sum;
		}
		/* Solve Ue = y (reuse r as e) */
		vull__backward_substitute( r, LU, r, n, n );

		/* Subtract the error from the old solution */
		vull__vsub( x, x, r, n );

		/* Break if within tolerance */
		rd2 = vull__dot( r, r, n );
		if( fabs( rd2 - rd ) < tolerance * n ) {
			break;
		}
		vull__mmul( r, A, x, n, n );
		vull__vsub( r, r, b, n );
		rd = rd2;
	}
	
	VUL_LINEAR_SOLVERS_FREE( LU );
	VUL_LINEAR_SOLVERS_FREE( r );
	VUL_LINEAR_SOLVERS_FREE( scale );
	VUL_LINEAR_SOLVERS_FREE( indices );
}

void vul_solve_cholesky_decomposition_dense( vul_solve_real *out,
											 vul_solve_real *A,
											 vul_solve_real *initial_guess,
											 vul_solve_real *b,
											 int n,
											 int max_iterations,
											 vul_solve_real tolerance )
{
	vul_solve_real *x, *D, *r;
	vul_solve_real sum, rd, rd2;
	int i, j, k;

	D = ( vul_solve_real* )VUL_LINEAR_SOLVERS_ALLOC( sizeof( vul_solve_real ) * n * n );
	r = ( vul_solve_real* )VUL_LINEAR_SOLVERS_ALLOC( sizeof( vul_solve_real ) * n );
	x = out;
	
	/* Copy the matrix to the work-matrix (@TODO(thynn): This should either be a straight memcpy, or only on the values we need to copy (and weaved in below?)) */
		for( i = 0; i < n; ++i ) {
			for( j = 0; j < n; ++j ) {
				IDX( D, i, j, n, n ) = IDX( A, i, j, n, n );
				assert( IDX( A, i, j, n, n ) == IDX( A, j, i, n, n ) ); // Cholesky decomposition is only valid for positive-definite SYMMETRIC matrices
			}
		}
	/* Decomposition */
	for( i = 0; i < n; ++i ) {
		for( j = i; j < n; ++j ) {
			sum = IDX( D, i, j, n, n );
			for( k = i - 1; k >= 0; --k ) {
				sum -= IDX( D, i, k, n, n ) * IDX( D, j, k, n, n );
			}
			if( i == j ) {
				assert( sum > 0.f ); // Cholesky decomposition is only valid for POSITIVE-DEFINITE symmetric matrices
				IDX( D, i, i, n, n ) = sqrt( sum );
			} else {
				assert( IDX( D, i, i, n, n ) != 0.f ); // Determinant is sufficiently small that a divide-by-zero is imminent
				IDX( D, j, i, n, n ) = sum / IDX( D, i, i, n, n );
			}
		}
	}
	
	/* Calculate initial residual */
	vull__vcopy( x, initial_guess, n );
	vull__mmul( r, A, x, n, n );
	vull__vsub( r, r, b, n );
	rd = vull__dot( r, r, n );

	for( k = 0; k < max_iterations; ++k ) {
		/* Solve Ly = r (solve for the residual error, not b)*/
		for( i = 0; i < n; ++i ) {
			sum = r[ i ];
			for( j = i - 1; j >= 0; --j ) {
				sum -= IDX( D, i, j, n, n ) * r[ j ];
			}
			r[ i ] = sum / IDX( D, i, i, n, n );
		}
		/* Solve L^Te = y (reuse r as e) */
		for( i = n - 1; i >= 0; --i ) {
			sum = r[ i ];
			for( j = i + 1; j < n; ++j ) {
				sum -= IDX( D, j, i, n, n ) * r[ j ];
			}
			r[ i ] = sum / IDX( D, i, i, n, n );
		}
		
		/* Subtract the error from the old solution */
		vull__vsub( x, x, r, n );

		/* Break if within tolerance */
		vull__mmul( r, A, x, n, n );
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

void vul_solve_qr_decomposition_dense( vul_solve_real *out,
									   vul_solve_real *A,
									   vul_solve_real *initial_guess,
									   vul_solve_real *b,
									   int n,
									   int max_iterations,
									   vul_solve_real tolerance )
{
	vul_solve_real *x, *QR, *c, *d, *r;
	vul_solve_real scale, sigma, tau, sum, rd, rd2;
	int i, j, k, singular;

	QR = ( vul_solve_real* )VUL_LINEAR_SOLVERS_ALLOC( sizeof( vul_solve_real ) * n * n );
	d = ( vul_solve_real* )VUL_LINEAR_SOLVERS_ALLOC( sizeof( vul_solve_real ) * n );
	c = ( vul_solve_real* )VUL_LINEAR_SOLVERS_ALLOC( sizeof( vul_solve_real ) * n );
	r = ( vul_solve_real* )VUL_LINEAR_SOLVERS_ALLOC( sizeof( vul_solve_real ) * n );
	x = out;
	
	/* Copy the matrix to the work-matrix (@TODO(thynn): This should either be a straight memcpy, or only on the values we need to copy (and weaved in below?)) */
	for( i = 0; i < n; ++i ) {
		for( j = 0; j < n; ++j ) {
			IDX( QR, i, j, n, n ) = IDX( A, i, j, n, n );
		}
	}
	/* Crout's LUP decomposition (without pivoting/scaling, which means it's unstable and bad...@TODO(thynn): Pivoting and scaling!) */
	singular = 0;
	for( k = 0; k < n; ++k ) {
		scale = 0.f;
		for( i = k; i < n; ++i ) {
			scale = fmaxf( scale, fabs( IDX( QR, i, k, n, n ) ) );
		}
		if( scale == 0.f ) {
			singular = 1;
			c[ k ] = 0.f;
			d[ k ] = 0.f;
		} else {
			for( i = k; i < n; ++i ) {
				IDX( QR, i, k, n, n ) /= scale;
			}
			sum = 0.f;
			for( i = k; i < n; ++i ) {
				sum += IDX( QR, i, k, n, n ) * IDX( QR, i, k, n, n );
			}
			sigma = IDX( QR, k, k, n, n ) >= 0.f ? sqrt( sum ) : -sqrt( sum );
			IDX( QR, k, k, n, n ) += sigma;
			c[ k ] = sigma * IDX( QR, k, k, n, n );
			d[ k ] = -scale * sigma;
			for( j = k + 1; j < n; ++j ) {
				sum = 0.f;
				for( i = k; i < n; ++i ) {
					sum += IDX( QR, i, k, n, n ) * IDX( QR, i, j, n, n );
				}
				tau = sum / c[ k ];
				for( i = k; i < n; ++i ) {
					IDX( QR, i, j, n, n ) -= tau * IDX( QR, i, k, n, n );
				}
			}
		}
	}
	d[ n - 1 ] = IDX( QR, n - 1, n - 1, n, n );
	if( d[ n - 1 ] == 0.f ) {
		singular = 1;
	}

	
	/* Calculate initial residual */
	vull__vcopy( x, initial_guess, n );
	vull__mmul( r, A, x, n, n );
	vull__vsub( r, b, r, n );
	rd = vull__dot( r, r, n );

	for( k = 0; k < max_iterations; ++k ) {
		/* Form Q^T * b in y */
		for( j = 0; j < n; ++j ) {
			sum = 0.f;
			for( i = j; i < n; ++i ) {
				sum += IDX( QR, i, j, n, n ) * r[ i ];
			}
			tau = sum / c[ j ];
			for( i = j; i < n; ++i ) {
				r[ i ] -= tau * IDX( QR, i, j, n, n );
			}
		}
		/* Solve R*x = Q^T*b */
		r[ n - 1 ] /= d[ n - 1 ];
		for( i = n - 2; i >= 0; --i ) {
			sum = r[ i ];
			for( j = i + 1; j < n; ++j ) {
				sum -= IDX( QR, i, j, n, n ) * r[ j ];
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

void vul_solve_successive_over_relaxation_dense( vul_solve_real *out,
												 vul_solve_real *A,
												 vul_solve_real *initial_guess,
												 vul_solve_real *b,
												 vul_solve_real relaxation_factor,
												 int n,
												 int max_iterations,
												 vul_solve_real tolerance )
{
	vul_solve_real *x, *r;
	int i, j, k;
	vul_solve_real omega, rd, rd2;

	r = ( vul_solve_real* )VUL_LINEAR_SOLVERS_ALLOC( sizeof( vul_solve_real ) * n );
		
	x = out;
	/* Calculate initial residual */
	vull__vcopy( x, initial_guess, n );
	vull__mmul( r, A, x, n, n );
	vull__vsub( r, r, b, n );
	rd = vull__dot( r, r, n );
		
	for( k = 0; k < max_iterations; ++k ) {
		/* Relax */
		for( i = 0; i < n; ++i ) {
			omega = 0.f;
			for( j = 0; j < n; ++j ) {
				if( i != j ) {
					omega += IDX( A, i, j, n, n ) * x[ j ];
				}
			}
			x[ i ] = ( 1.f - relaxation_factor ) * x[ i ]
					+ ( relaxation_factor / IDX( A, i, i, n, n ) ) * ( b[ i ] - omega );
		}
		/* Check for convergence */
		vull__mmul( r, A, x, n, n );
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

void vul_solve_svd_basis_reconstruct_matrix( float *M, vul_solve_svd_basis *x, int n )
{
	int i, j, k, l;

	if( !n ) {
		return;
	}

	memset( M, 0, sizeof( float ) * x[ 0 ].u_length * x[ 0 ].v_length );
	for( k = 0; k < n; ++k ) {
		for( i = 0; i < x[ k ].u_length; ++i ) {
			for( j = 0; j < x[ k ].v_length; ++j ) {
				IDX( M, i, j, x[ k ].v_length, x[ k ].u_length ) += x[ k ].sigma * x[ k ].u[ i ] * x[ k ].v[ j ];
			}
		}
	}
}

void vul_solve_svd_basis_destroy( vul_solve_svd_basis *x, int n )
{
	int i;

	for( i = 0; i < n; ++i ) {
		if( x[ i ].u ) {
			VUL_LINEAR_SOLVERS_FREE( x[ i ].u );
			x[ i ].u = 0;
		}
		if( x[ i ].v ) {
			VUL_LINEAR_SOLVERS_FREE( x[ i ].v );
			x[ i ].v = 0;
		}
	}
}

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

static void vul__solve_qr_decomposition_gram_schmidt( vul_solve_real *Q, vul_solve_real *R, vul_solve_real *A, 
																		int c, int r, int transpose )
{
	// Gram-Schmidt; numerically bad and slow, but simple.
	vul_solve_real *u, *a, d, tmp, *At;
	int i, j, k;
	
	if( transpose ) {
		i = c; c = r; r = i;
	}

	u = ( vul_solve_real* )VUL_LINEAR_SOLVERS_ALLOC( sizeof( vul_solve_real ) * c * r );
	a = ( vul_solve_real* )VUL_LINEAR_SOLVERS_ALLOC( sizeof( vul_solve_real ) * r );

	At = 0;
	if( transpose ) {
		At = ( vul_solve_real* )VUL_LINEAR_SOLVERS_ALLOC( sizeof( vul_solve_real ) * c * r );
		vull__mtranspose( At, A, r, c );
		A = At;

	}
	// Fill Q
	for( i = 0; i < c; ++i ) {
		for( j = 0; j < r; ++j ) {
			a[ j ] = IDX( A, j, i, c, r );
		}
		d = 0.f;
		for( j = 0; j < r; ++j ) {
			u[ i * r + j ] = a[ j ];
			for( k = 0; k < i; ++k ) {
				tmp = vull__dot( &u[ k * r ], &u[ k * r ], r );
				if( tmp != 0.f ) {
					u[ i * r + j ] -= u[ k * r + j ] * ( vull__dot( &u[ k * r ], a, r ) / tmp );
				}
			}
			d += u[ i * r + j ] * u[ i * r + j ];
		}
		if( d != 0.f ) {
			d = 1.f / sqrt( d );
		}
		for( j = 0; j < r; ++j ) {
			IDX( Q, j, i, r, r ) = u[ i * r + j] * d;
		}
	}

	// Fill R
	for( i = 0; i < r; ++i ) {
		for( j = 0; j < c; ++j ) {
			R[ i * c + j ] = 0.f;
			for( k = 0; k < r; ++k ) {
				IDX( R, i, j, c, r ) += IDX( Q, k, i, r, r ) * IDX( A, k, j, c, r ); // Q^T
			}
		}
	}

	if( At ) {
		VUL_LINEAR_SOLVERS_FREE( A );
	}
	VUL_LINEAR_SOLVERS_FREE( u );
	VUL_LINEAR_SOLVERS_FREE( a );
}

static void vul__solve_qr_decomposition_householder( vul_solve_real *Q, vul_solve_real *R, vul_solve_real *A, 
																	  int c, int r, int transpose )
{
	vul_solve_real *u, *Qt, *Q0, *Q1, *R0, *R1, alpha, d;
	int i, j, k, l, n;

	if( transpose ) {
		i = c; c = r; r = i;
	}

	u = ( vul_solve_real* )VUL_LINEAR_SOLVERS_ALLOC( sizeof( vul_solve_real ) * r );

	n = r > c ? r : c;
	Qt = ( vul_solve_real* )VUL_LINEAR_SOLVERS_ALLOC( sizeof( vul_solve_real ) * r * r );
	Q0 = ( vul_solve_real* )VUL_LINEAR_SOLVERS_ALLOC( sizeof( vul_solve_real ) * r * r );
	Q1 = ( vul_solve_real* )VUL_LINEAR_SOLVERS_ALLOC( sizeof( vul_solve_real ) * r * r );
	R0 = ( vul_solve_real* )VUL_LINEAR_SOLVERS_ALLOC( sizeof( vul_solve_real ) * n * n );
	R1 = ( vul_solve_real* )VUL_LINEAR_SOLVERS_ALLOC( sizeof( vul_solve_real ) * n * n );

	if( r == 0 ) return;
	if( r == 1 ) {
		Q[ 0 ] = A[ 0 ]; // Just copy it?
		R[ 0 ] = 1.f;
	}
	memset( R0, 0, sizeof( vul_solve_real ) * n * n );
	memset( R1, 0, sizeof( vul_solve_real ) * n * n );
	if( transpose ) {
		vull__mtranspose( R0, A, r, c );
	} else {
		memcpy( R0, A, sizeof( vul_solve_real ) * r * c );
	}

	memset( Q0, 0, sizeof( vul_solve_real ) * r * r );
	for( i = 0; i < r; ++i ) {
		IDX( Q0, i, i, r, r ) = 1.f;
	}

	for( k = 0; k < r - 1; ++k ) {
		// Calculate alpha
		alpha = 0.f;
		for( i = k; i < r; ++i ) {
			alpha += IDX( R0, i, k, c, r ) * IDX( R0, i, k, c, r );
		}
		// k-th element dictates sign to avoid loss of significance TODO(thynn): Check why we get wrong results when doing so!
		alpha = copysign( sqrt( alpha ), IDX( R0, k, k, c, r ) );

		// Construct u
		for( i = 0; i < ( r - k ); ++i ) {
			u[ i ] = IDX( R0, ( i + k ), k, c, r );
		}
		// Construct v
		u[ 0 ] += alpha;
		d = 0.f;
		for( i = 0; i < r - k; ++i ) {
			d += u[ i ] * u[ i ];
		}
		if( d != 0.f ) {
			d = 1.f / sqrt( d );
		}
		for( i = 0; i < r - k; ++i ) {
			u[ i ] *= d;
		}
		// Construct Q
		memset( Qt, 0, sizeof( vul_solve_real ) * r * r );
		for( i = 0; i < k; ++i ) {
			IDX( Qt, i, i, r, r ) = 1.f;
		}
		for( i = k; i < r; ++i ) {
			for( j = k; j < r; ++j ) {
				IDX( Qt, i, j, r, r ) = ( ( i == j ) ? 1.f : 0.f ) - 2.f * u[ i - k ] * u[ j - k ];
			}
		}
		// Calcualte new A to keep going
		for( i = 0; i < r; ++i ) {
			for( j = 0; j < c; ++j ) {
				d = 0.f;
				for( l = 0; l < r; ++l ) {
					d += IDX( Qt, i, l, r, r ) * IDX( R0, l, j, c, r );
				}
				IDX( R1, i, j, c, r ) = d;
			}
		}
		// Accumulate Q
		for( i = 0; i < r; ++i ) { // i == row
			for( j = 0; j < r; ++j ) { // j == column
				d = 0.f;
				for( l = 0; l < r; ++l ) {
					d += IDX( Q0, i, l, r, r ) * IDX( Qt, l, j, r, r ); // Q is symmetric, so Q^T == Q in the real case
				}
				IDX( Q1, i, j, r, r ) = d;
			}
		}
		vull__swap_ptr( &Q0, &Q1 );
		vull__swap_ptr( &R0, &R1 );
	}
	memcpy( Q, Q0, sizeof( vul_solve_real ) * r * r );
	memcpy( R, R0, sizeof( vul_solve_real ) * r * c );

	VUL_LINEAR_SOLVERS_FREE( Q0 );
	VUL_LINEAR_SOLVERS_FREE( Q1 );
	VUL_LINEAR_SOLVERS_FREE( Qt );
	VUL_LINEAR_SOLVERS_FREE( R0 );
	VUL_LINEAR_SOLVERS_FREE( R1 );
	VUL_LINEAR_SOLVERS_FREE( u );
}

// @TODO(thynn): I think we should use givens for sparse, householder for dense.
// Always use givens when we have more columns than rows??
static void vul__solve_qr_decomposition_givens( vul_solve_real *Q, vul_solve_real *R, vul_solve_real *A,	
																int c, int r, int transpose )
{
	// @TODO(thynn): svd solver misbehaves with this when non-square matrices are used (due to transposes
	// and mismatching/misindexing col/row sizes.
	int i, j, k;
	vul_solve_real theta, st, ct, G[ 4 ], v0, v1, *RA;
	
	if( transpose ) {
		i = c; c = r; r = i;
	}

	RA = ( vul_solve_real* )VUL_LINEAR_SOLVERS_ALLOC( sizeof( vul_solve_real ) * r * c );

	memset( RA, 0, sizeof( vul_solve_real ) * r * c );
	if( transpose ) {
		vull__mtranspose( RA, A, r, c );
	} else {
		memcpy( RA, A, sizeof( vul_solve_real ) * r * c );
	}
	for( i = 0; i < r; ++i ) {
		for( j = 0; j < r; ++j ) {
			IDX( Q, i, j, r, r ) = ( i == j ) ? 1.f : 0.f;
		}
	}
	
	for( j = 0; j < c; ++j ) {
		for( i = r - 1; i > j; --i ) {
			v0 = IDX( RA, ( i - 1 ), j, c, r );
			if( v0 == 0.f ) {
				theta = RA[ i * c + j ] > 0 ? M_PI * 0.5f : M_PI * -0.5f;
			} else {
				theta = atanf( -IDX( RA, i, j, c, r ) / v0 );
			}
			// Create givens rotation matrix
			st = sin( theta );
			ct = cos( theta );
			G[ 0 ] = ct;
			G[ 1 ] = st; // @TODO(thynn): This should be positive, and 2 negative, but this produces correct results, so idk.
			G[ 2 ] = -st;
			G[ 3 ] = ct;
			// Apply it to the two affected rows, that is calculate the below for those rows that change
			// R = G^T * R
			for( k = 0; k < c; ++k ) {
				v0 = G[ 0 ] * IDX( RA, ( i - 1 ), k, c, r ) + G[ 2 ] * IDX( RA, i, k, c, r ); // G[ 2 ]: T is transposed
				v1 = G[ 1 ] * IDX( RA, ( i - 1 ), k, c, r ) + G[ 3 ] * IDX( RA, i, k, c, r ); // G[ 1 ]: T is transposed
				IDX( RA, ( i - 1 ), k, c, r ) = v0;
				IDX( RA,   i      , k, c, r ) = v1;
			}
			// Calculate new Q = Q * G
			for( k = 0; k < r; ++k ) {
				v0 = G[ 0 ] * IDX( Q, k, i - 1, r, r ) + G[ 2 ] * IDX( Q, k, i, r, r );
				v1 = G[ 1 ] * IDX( Q, k, i - 1, r, r ) + G[ 3 ] * IDX( Q, k, i, r, r );
				IDX( Q, k, i - 1, r, r ) = v0;
				IDX( Q, k, i    , r, r ) = v1;
			}
		}
	}
	for( i = 0; i < r; ++i ) {
		for( j = 0; j < c; ++j ) {
			IDX( R, i, j, c, r ) = IDX( RA, i, j, c, r );
		}
	}

	VUL_LINEAR_SOLVERS_FREE( RA );
}

static vul_solve_real vul__solve_largest_eigenvalue( vul_solve_real *A, int c, int r, vul_solve_real eps, int max_iter )
{
	int iter, axis, normaxis, i, j;
	vul_solve_real *v, *y, lambda, norm, err;

	// Power method. Slow, but simple
	v = ( vul_solve_real* )VUL_LINEAR_SOLVERS_ALLOC( sizeof( vul_solve_real ) * r );
	y = ( vul_solve_real* )VUL_LINEAR_SOLVERS_ALLOC( sizeof( vul_solve_real ) * r );
	memset( v, 0, sizeof( vul_solve_real ) * r );
	v[ 0 ] = 1.f;

	err = eps * 2;
	iter = 0;
	axis = 0;
	lambda = 0.f;
	while( err > eps && iter++ < max_iter ) {
		vull__mmul( y, A, v, c, r );
		for( i = 0; i < r; ++i ) {
			y[ i ] = 0;
			for( j = 0; j < c; ++j ) {
				y[ i ] += IDX( A, i, j, c, r ) * v[ j ];
			}
		}
		err = fabs( lambda - y[ axis ] );
		lambda = y[ axis ];
		norm = -FLT_MAX;
		for( i = 0; i < r; ++i ) {
			if( y[ i ] > norm ) {
				norm = y[ i ];
				normaxis = i;
			}
		}
		axis = normaxis;
		for( i = 0; i < r; ++i ) {
			v[ i ] = y[ i ] / norm;
		}
	}

	VUL_LINEAR_SOLVERS_FREE( v );
	VUL_LINEAR_SOLVERS_FREE( y );

	return lambda;
}

static vul_solve_real vul__solve_matrix_norm_diagonal( vul_solve_real *A, int c, int r )
{
	vul_solve_real v;
	int i, n;
	
	v = 0.f;
	n = c < r ? c : r;
	for( i = 0; i < n; ++i ) {
		v += IDX( A, i, i, c, r ) * IDX( A, i, i, c, r );
	}
	return sqrt( v );
}

static vul_solve_real vul__solve_matrix_norm_as_single_column( vul_solve_real *A, int c, int r, int upper_diag )
{
	vul_solve_real v;
	int i, j;

	v = 0.f;
	for( i = 0; i < r; ++i ) {
		for( j = i + upper_diag < 0 ? 0 : i + upper_diag;
			  j < c; ++j ) {
			v += IDX( A, i, j, c, r ) * IDX( A, i, j, c, r );
		}
	}
	return v;
}

static vul_solve_real vul__solve_matrix_2norm( vul_solve_real *A, int c, int r, vul_solve_real eps, int itermax )
{
	// @TODO(thynn): Needs testing if useful, removal otherwise!
	vul_solve_real v, *U, *T, *AA, e;
	int i, j, k, n;

	n = r >= c ? r : c;
	U = ( vul_solve_real* )VUL_LINEAR_SOLVERS_ALLOC( sizeof( vul_solve_real ) * r * c );
	T = ( vul_solve_real* )VUL_LINEAR_SOLVERS_ALLOC( sizeof( vul_solve_real ) * r * c );
	AA = ( vul_solve_real* )VUL_LINEAR_SOLVERS_ALLOC( sizeof( vul_solve_real ) * n * n );
	memcpy( U, A, sizeof( vul_solve_real ) * r * c );
	vull__mtranspose( T, A, c, r );
	vull__mmul_matrix_rect( AA, T, U, n, c, n );
	// Find the larges eigenvalue!
	e = vul__solve_largest_eigenvalue( AA, c, r, eps, itermax ); 
	return sqrt( e * e );
}

void vul_solve_svd_dense( vul_solve_svd_basis *out, int *rank,
								  vul_solve_real *A,
								  int c, int r, vul_solve_real eps, int itermax )
{
	vul_solve_real *U0, *U1, *V0, *V1, *S0, *S1, *Q, err, e, f;
	int iter, n, i, j, k, ri, ci;

	n = r > c ? r : c;
	U0 = ( vul_solve_real* )VUL_LINEAR_SOLVERS_ALLOC( sizeof( vul_solve_real ) * r * r );
	U1 = ( vul_solve_real* )VUL_LINEAR_SOLVERS_ALLOC( sizeof( vul_solve_real ) * r * r );
	V0 = ( vul_solve_real* )VUL_LINEAR_SOLVERS_ALLOC( sizeof( vul_solve_real ) * c * c );
	V1 = ( vul_solve_real* )VUL_LINEAR_SOLVERS_ALLOC( sizeof( vul_solve_real ) * c * c );
	S0 = ( vul_solve_real* )VUL_LINEAR_SOLVERS_ALLOC( sizeof( vul_solve_real ) * r * c );
	S1 = ( vul_solve_real* )VUL_LINEAR_SOLVERS_ALLOC( sizeof( vul_solve_real ) * r * c );
	Q = ( vul_solve_real* )VUL_LINEAR_SOLVERS_ALLOC( sizeof( vul_solve_real ) * n * n );
	memset( U0, 0, sizeof( vul_solve_real ) * r * r );
	memset( U1, 0, sizeof( vul_solve_real ) * r * r );
	memset( V0, 0, sizeof( vul_solve_real ) * c * c );
	memset( V1, 0, sizeof( vul_solve_real ) * c * c );
	memset( S0, 0, sizeof( vul_solve_real ) * r * c );
	memset( S1, 0, sizeof( vul_solve_real ) * r * c );
	memset( Q, 0, sizeof( vul_solve_real ) * n * n );
	iter = 0;
	err = FLT_MAX;

	// Initialize to empty for U, V, Q. Set S to A^T
	vull__mtranspose( S0, A, c, r );

	// Initialize as identity matrix
	for( i = 0; i < r; ++i ) {
		IDX( U0, i, i, r, r ) = 1.f;
	}
	for( i = 0; i < c; ++i ) {
		IDX( V0, i, i, c, c ) = 1.f;
	}
	while( err > eps && iter++ < itermax ) {
		// Decompose
		vul__solve_qr_decomposition_householder( Q, S1, S0, r, c, 1 );
		vull__mmul_matrix( U1, U0, Q, r );
		vul__solve_qr_decomposition_householder( Q, S0, S1, c, r, 1 );
		vull__mmul_matrix( V1, V0, Q, c );

		vull__swap_ptr( &U0, &U1 );
		vull__swap_ptr( &V0, &V1 );

		// Calculate error
		e = vul__solve_matrix_norm_as_single_column( S0, r, c, 1 );
		f = vul__solve_matrix_norm_diagonal( S0, r, c );
		if( f == 0.f ) {
			f = 1.f;
		}
		err = e / f;
	}

	// Grap sigmas and rank, sort decreasing
	k = r < c ? r : c;
	for( j = 0, i = 0; i < k; ++i ) { // Since we're transposed, S is indexed with r and not c
		out[ i ].sigma = fabs( IDX( S0, i, i, r, c ) );
		out[ i ].axis = i;
		if( out[ i ].sigma > eps ) {
			++j;
		}
	}
	if( *rank == 0 || j < *rank ) {
		*rank = j;
	}
	vul__solve_svd_sort( out, k );

	// Fix signs and copy U
	for( i = 0; i < *rank; ++i ) {
		out[ i ].u_length = r;
		out[ i ].v_length = c;
		out[ i ].u = ( vul_solve_real* )VUL_LINEAR_SOLVERS_ALLOC( sizeof( vul_solve_real ) * r );
		out[ i ].v = ( vul_solve_real* )VUL_LINEAR_SOLVERS_ALLOC( sizeof( vul_solve_real ) * c );
		f = IDX( S0, out[ i ].axis, out[ i ].axis , r, c ) < 0.f ? -1.f : 1.f;
		for( j = 0; j < r; ++j ) {
			out[ i ].u[ j ] = IDX( U0, j, out[ i ].axis, r, r ) * f;
		}
		for( j = 0; j < c; ++j ) {
			out[ i ].v[ j ] = IDX( V0, j, out[ i ].axis, c, c );
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

#undef IDX

#ifdef _cplusplus
}
#endif

#undef vul_solve_real

#endif
