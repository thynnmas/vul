/*
 * Villains' Utility Library - Thomas Martin Schmid, 2016. Public domain¹
 *
 * This file contains a number of linear system solvers and functions
 * to perform singular value decomposition:
 *
 * > The following solvers of the linear system Ax=b:
 *    -Iterative:
 *      -Conjugate gradient method
 *      -Successive over-relaxation
 *    -Decompositions (single iteration):
 *      -QR decomposition
 *      -Cholesky decomposition
 *    -Decomposition (iterative refinement):
 *      -LU decomposition [Only for dense matrices] @TODO(thynn): Sparse LU decomposition
 * > The following SVD methods:
 *    -One-sided Jacobi orthogonalization
 *    -Repeated, alternating QR and LQ decomposition (SLOW and less accurate, but simple)
 * > A Generalized Linear Least Square solver that uses SVD
 * > A function that finds the largest eigenvalue of a matrix (using the power method).
 *
 * All features except the LU decomposition solver are supplied both for dense matrices
 * and sparse matrices. The library uses a row-major List-of-Lists format for sparse
 * matrices.
 *
 * Planned future features include:
 *  -@TODO(thynn): SVD general least square that takes an evaluation function for xi^2.
 *                 Let me know if this is actually useful to you, as including it may
 *                 me fairly tricky, and is not on my short term todo-list.
 *  -@TODO(thynn): Performance improvements if desired/needed.
 *                 Most notably, the internal matrix multiplication function
 *                 should use Strassen's algorithm for larger N.
 *
 *
 * Define VUL_BLAS_ROW_MAJOR to use row major dense matrices, otherwise column major
 * is assumed.
 * 
 * If you wish to use a custom allocator for temporaries, define VUL_BLAS_ALLOC
 * and VUL_BLAS_FREE to functions with signatures similar to malloc and free;
 * if no alternative is given malloc and free are used.
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
#ifndef VUL_BLAS_H
#define VUL_BLAS_H

#include <math.h>
#include <assert.h>
#include <float.h>
#include <string.h>

#ifndef VUL_BLAS_ALLOC
#include <stdlib.h>
#define VUL_BLAS_ALLOC malloc
#define VUL_BLAS_FREE free
#else
   #ifndef VUL_BLAS_FREE
      vul_blas.h: You must also specify a deallocation function to go with VUL_BLAS_ALLOC
   #endif
#endif

#ifdef VUL_BLAS_DOUBLE
#define vul_blas_real double
#else
#define vul_blas_real float
#endif

#ifdef _cplusplus
{
#endif

//--------------------
// Sparse datatypes
//

typedef struct vul_blas_sparse_entry {
   unsigned int idx;
   vul_blas_real val;
} vul_blas_sparse_entry;

typedef struct vul_blas_vector {
   vul_blas_sparse_entry *entries;
   unsigned int count;
} vul_blas_vector;

typedef struct vul_blas_matrix_row {
   unsigned int idx;
   vul_blas_vector vec;
} vul_blas_matrix_row;

typedef struct vul_blas_matrix {
   vul_blas_matrix_row *rows;
   unsigned int count;
} vul_blas_matrix;

//----------------------------------
// Sparse datatype public functions
//

/*
 * Creates a sparse vector. Takes a list of indices and values to fill it with,
 * or two null pointers to initialize empty.
 */
vul_blas_vector *vul_blas_vector_create( unsigned *idxs, vul_blas_real *vals, unsigned int n );

/*
 * Create a sparse matrix. Takes a list of coordinates and values to fill it with,
 * or three null-pointers to initialize empty.
 */
vul_blas_matrix *vul_blas_matrix_create( unsigned int *rows, unsigned int *cols, 
                                         vul_blas_real *vals, unsigned int init_count );

/* 
 * Destroys a sparse matrix
 */
void vul_blas_matrix_destroy( vul_blas_matrix *m );

/* 
 * Overwriting a non-zero value with zero does not reclaim the space!
 */
void vul_blas_vector_insert( vul_blas_vector *v, unsigned int idx, vul_blas_real val );

/*
 * When overwriting a non-zero value with zero, the space is not reclaimed automaticall,
 * so the matrix is not "fully sparse". When rotating/transforming with householder or givens
 * rotations this can lead to full rows of zeroes. Use vulb__mclean on the matrix to reclaim all
 * space lost to zero-over-non-zero insertions.
 */
void vul_blas_matrix_insert( vul_blas_matrix *m, unsigned int r, unsigned int c, vul_blas_real v );

/*
 * Retrieve an element from a sparse vector.
 */
vul_blas_real vul_blas_vector_get( vul_blas_vector *v, unsigned int idx );

/* 
 * Retrieve and element from a sparse matrix.
 */
vul_blas_real vul_blas_matrix_get( vul_blas_matrix *m, unsigned int r, unsigned int c );

/*
 * Destroy a sparse vector
 */
void vul_blas_vector_destroy( vul_blas_vector *v );

//--------------------
// Sparse solvers

/*
 * Iterative solver of the linear system Ax = b
 * Uses the Conjugate Gradient Method, and works for matrices that are
 * POSITIVE-DEFINITE and SYMMETRIC.
 *
 * Runs for at most max_iterations, or until the average square error is
 * below the given tolerance.
 */
vul_blas_vector *vul_blas_conjugate_gradient_sparse( vul_blas_matrix *A,
                                                     vul_blas_vector *initial_guess,
                                                     vul_blas_vector *b,
                                                     int max_iterations,
                                                     vul_blas_real tolerance );

/*
 * Iterative solver of the linear system Ax = b
 * Uses the Successive Over-Relaxation method. May converge for any matrix,
 * but may also not converge at all.
 *
 * Runs for at most max_iterations, or until the average square error is
 * below the given tolerance.
 */
vul_blas_vector *vul_blas_successive_over_relaxation_sparse( vul_blas_matrix *A,
                                                             vul_blas_vector *initial_guess,
                                                             vul_blas_vector *b,
                                                             vul_blas_real relaxation_factor,
                                                             int max_iterations,
                                                             vul_blas_real tolerance );

/*
 * Iterative solver of the linear system Ax = b
 * Uses Choleski Decomposition. Works for matrices that are
 * HERMITIAN and POSITIVE-DEFINITE.
 *
 * If your matrix fits, this should be the fastest of the decomposition-based methods.
 * Does not currently emply iterative refinement; @TODO(thynn): Iterative refinement!
 */
vul_blas_vector *vul_blas_cholesky_decomposition_sparse( vul_blas_matrix *A,
                                                         vul_blas_vector *b,
                                                         int cols, int rows );

/*
 * Iterative solver of the linear system Ax = b
 * Uses QR Decomposition. Works for all matrices.
 *
 * Does not currently emply iterative refinement; @TODO(thynn): Iterative refinement!
 */
vul_blas_vector *vul_blas_qr_decomposition_sparse( vul_blas_matrix *A,
                                                   vul_blas_vector *b,
                                                   int cols, int rows );

//-------------------
// Dense solvers

/*
 * Iterative solver of the linear system Ax = b
 * Uses the Conjugate Gradient Method, and works for matrices that are
 * POSITIVE-DEFINITE and SYMMETRIC.
 *
 * Runs for at most max_iterations, or until the average square error is
 * below the given tolerance.
 */
void vul_blas_conjugate_gradient_dense( vul_blas_real *out,
                               vul_blas_real *A,
                               vul_blas_real *initial_guess,
                               vul_blas_real *b,
                               int n,
                               int max_iterations,
                               vul_blas_real tolerance );
/*
 * Iterative solver of the linear system Ax = b
 * Uses the Successive Over-Relaxation method. May converge for any matrix,
 * but may also not converge at all.
 *
 * Runs for at most max_iterations, or until the average square error is
 * below the given tolerance.
 */
void vul_blas_successive_over_relaxation_dense( vul_blas_real *out,
                                     vul_blas_real *A,
                                     vul_blas_real *initial_guess,
                                     vul_blas_real *b,
                                     vul_blas_real relaxation_factor,
                                     int n,
                                     int max_iterations,
                                     vul_blas_real tolerance );

/*
 * Iterative solver of the linear system Ax = b
 * Uses LU Decomposition. Works for all matrices that are NOT SINGULAR.
 *
 * If your matrix fits, this should be the fastest of the stable methods (not SOR).
 * This method employs at most max_iterations of iterative refinement, or until
 * the average square error is below tolerance.
 */
void vul_blas_lu_decomposition_dense( vul_blas_real *out,
                              vul_blas_real *A,
                              vul_blas_real *initial_guess,
                              vul_blas_real *b,
                              int n,
                              int max_iterations,
                              vul_blas_real tolerance );
/*
 * Iterative solver of the linear system Ax = b
 * Uses Choleski Decomposition. Works for matrices that are
 * HERMITIAN and POSITIVE-DEFINITE.
 *
 * If your matrix fits, this should be the fastest of the decomposition-based methods.
 * Does not currently emply iterative refinement; @TODO(thynn): Iterative refinement!
 */
void vul_blas_cholesky_decomposition_dense( vul_blas_real *out,
                                  vul_blas_real *A,
                                  vul_blas_real *b,
                                  int n );
/*
 * Iterative solver of the linear system Ax = b
 * Uses QR Decomposition. Works for all matrices.
 *
 * Does not currently emply iterative refinement; @TODO(thynn): Iterative refinement!
 */
void vul_blas_qr_decomposition_dense( vul_blas_real *out,
                                       vul_blas_real *A,
                                       vul_blas_real *b,
                                       int n );

//---------------------------------------
// Dense Singular Value Decomposition

typedef struct vul_blas_svd_basis {
   vul_blas_real sigma;
   vul_blas_real *u, *v;
   unsigned int u_length, v_length;
   unsigned int axis;
} vul_blas_svd_basis;

/*
 * Reconstruct the matrix M = U S V* from the bases returned from the dense svd functions.
 * Takes a variable number n of bases x.
 */
void vul_blas_svd_basis_reconstruct_matrix( vul_blas_real *M, vul_blas_svd_basis *x, int n );
/*
 * Destroy n svd bases as returned from the svd functions.
 */
void vul_blas_svd_basis_destroy( vul_blas_svd_basis *x, int n );

/*
 * Computes the singular value decomposition of A.
 * If rank is set, the maximum of non-zero singular values and rank
 * values and basis vectors are returned.
 * The out array must have room for min(c,r) entries even if less are desired,
 * as the entries are used to sort and select singular values.
 *
 * This function uses Jacobi orthogonalization.
 */
void vul_blas_svd_dense( vul_blas_svd_basis *out, int *rank,
                         vul_blas_real *A,
                         int c, int r, vul_blas_real eps, int itermax );

/*
 * Computes the singular value decomposition of A.
 * If rank is set, the maximum of non-zero singular values and rank
 * values and basis vectors are returned.
 * The out array must have room for min(c,r) entries even if less are desired,
 * as the entries are used to sort and select singular values.
 *
 * This function uses repeated alternating QR and LQ decomposition.
 * This is VERY SLOW, and usually not recommended over the Jacobi-version,
 * however the implementation is simple, and it was written, so it is
 * available if anyone wants it.
 *
 * If the error increases, the iteration is stopped.
 */
void vul_blas_svd_dense_qrlq( vul_blas_svd_basis *out, int *rank,
                              vul_blas_real *A,
                              int c, int r, vul_blas_real eps, int itermax );

/*
 * Solves the generalized linear least squares problem defined by A and b by
 * singular value decomposition. The SVD runs at most max_iterations, or
 * until the average sqare error is less than tolerance. Matrix size must
 * be given in dimensions c and r.
 */
void vul_blas_linear_least_squares_dense( vul_blas_real *out,
                                          vul_blas_real *A,
                                          vul_blas_real *b,
                                          int c, int r,
                                          int max_iterations,
                                          vul_blas_real tolerance );

//---------------------------------------
// Sparse Singular Value Decomposition

typedef struct vul_blas_svd_basis_sparse {
   vul_blas_real sigma;
   vul_blas_vector *u, *v;
   unsigned int u_length, v_length;
   unsigned int axis;
} vul_blas_svd_basis_sparse;

/*
 * Computes the singular value decomposition of A.
 * If rank is set, the maximum of non-zero singular values and rank
 * values and basis vectors are returned.
 * The out array must have room for min(c,r) entries even if less are desired,
 * as the entries are used to sort and select singular values.
 *
 * This function uses Jacobi orthogonalization.
 */
void vul_blas_svd_sparse( vul_blas_svd_basis_sparse *out, int *rank,
                          vul_blas_matrix *A,
                          int c, int r, vul_blas_real eps, int itermax );
/*
 * Computes the singular value decomposition of A.
 * If rank is set, the maximum of non-zero singular values and rank
 * values and basis vectors are returned.
 * The out array must have room for min(c,r) entries even if less are desired,
 * as the entries are used to sort and select singular values.
 *
 * This function uses repeated alternating QR and LQ decomposition.
 * This is VERY SLOW, and usually not recommended over the Jacobi-version,
 * however the implementation is simple, and it was written, so it is
 * available if anyone wants it.
 *
 * If the error increases, the iteration is stopped.
 */
void vul_blas_svd_sparse_qrlq( vul_blas_svd_basis_sparse *out, int *rank,
                               vul_blas_matrix *A,
                               int c, int r, vul_blas_real eps, int itermax );

/*
 * Solves the generalized linear least squares problem defined by A and b by
 * singular value decomposition. The SVD runs at most max_iterations, or
 * until the average sqare error is less than tolerance. Matrix size must
 * be given in dimensions c and r.
 */
vul_blas_vector *vul_blas_linear_least_squares_sparse( vul_blas_matrix *A,
                                                       vul_blas_vector *b,
                                                       int c, int r,
                                                       int max_iterations,
                                                       vul_blas_real tolerance );

//--------------------------------
// Miscellanous BLAS functions
//

/*
 * Find the largest eigenvalue in the matrix A of dimentions c,r to the given
 * epsilon or until max_iter iterations have run.
 *
 * Uses the Power method (slow, but simple).
 */
vul_blas_real vul_blas_largest_eigenvalue( vul_blas_real *A, int c, int r, vul_blas_real eps, int max_iter );


#ifdef _cplusplus
}
#endif

#endif // VUL_BLAS_H

#ifdef VUL_DEFINE

#ifdef _cplusplus
extern "C" {
#endif

//----------------------------------
// Sparse datatype local functions
//

#define DECLARE_VECTOR_OP( name, op ) static void name( vul_blas_vector *out, vul_blas_vector *a, vul_blas_vector *b );
DECLARE_VECTOR_OP( vulb__sparse_vadd, + )
DECLARE_VECTOR_OP( vulb__sparse_vsub, - )
DECLARE_VECTOR_OP( vulb__sparse_vmul, * )
#undef DECLARE_VECTOR_OP

static void vulb__sparse_vmul_sub( vul_blas_vector *out, vul_blas_vector *a, vul_blas_vector *x, vul_blas_vector *b );
static void vulb__sparse_vmul_add( vul_blas_vector *out, vul_blas_vector *a, vul_blas_vector *x, vul_blas_vector *b );
static void vulb__sparse_vcopy( vul_blas_vector *out, vul_blas_vector *x );
static vul_blas_real vulb__sparse_dot( vul_blas_vector *a, vul_blas_vector *b );
static void vulb__sparse_mmul( vul_blas_vector *out, vul_blas_matrix *A, vul_blas_vector *x );
static void vulb__sparse_mmul_matrix( vul_blas_matrix *O, vul_blas_matrix *A, vul_blas_matrix *B, int n );
static void vulb__sparse_mmul_add( vul_blas_vector *out, vul_blas_matrix *A, vul_blas_vector *x, vul_blas_vector *b );
static void vulb__sparse_forward_substitute( vul_blas_vector *out, vul_blas_matrix *A, vul_blas_vector *b );
static void vulb__sparse_backward_substitute( vul_blas_vector *out, vul_blas_matrix *A, vul_blas_vector *b );
static void vulb__sparse_mtranspose( vul_blas_matrix *out, vul_blas_matrix *A );
static void vulb__sparse_mcopy( vul_blas_matrix *out, vul_blas_matrix *A );
static void vulb__sparse_mclear( vul_blas_matrix *A );
// Helper that returns an empty vector if the row doesn't exist; simplifies some loops
static vul_blas_vector vulb__blas_matrix_get_row_by_array_index( vul_blas_matrix *m, unsigned int r );
/*
 * When inserting zeroes into places that previously held non-zeroes, the memory is not freed.
 * This "compacts" the matrix back into a fully sparse memory pattern.
 */
static void vulb__sparse_mclean( vul_blas_matrix *A );

/*
 * Apply a Givens rotation to the matrix A at indices i and j
 * with the given sine/cosine. If post_multiply is set,
 * perform A = G * A instead of A = A * G.
 */
static void vul__blas_givens_rotate_sparse( vul_blas_matrix *A, int c, int r, 
                                            int i, int j, float cosine, float sine,
                                            int post_multiply );
/*
 * Compute the QR decomposition of A using givens rotations.
 */
static void vul__blas_qr_decomposition_givens_sparse( vul_blas_matrix *Q, vul_blas_matrix *R, 
                                                      vul_blas_matrix *A, int c, int r );

//------------------------
// Dense local functions
//

#define DECLARE_VECTOR_OP( name, op ) static void name( vul_blas_real *out, vul_blas_real *a, vul_blas_real *b, int n );
DECLARE_VECTOR_OP( vulb__vadd, + )
DECLARE_VECTOR_OP( vulb__vsub, - )
DECLARE_VECTOR_OP( vulb__vmul, * )
#undef DECLARE_VECTOR_OP

static void vulb__vmul_sub( vul_blas_real *out, vul_blas_real *a, vul_blas_real x, vul_blas_real *b, int n );
static void vulb__vmul_add( vul_blas_real *out, vul_blas_real *a, vul_blas_real x, vul_blas_real *b, int n );
static void vulb__vcopy( vul_blas_real *out, vul_blas_real *x, int n );
static vul_blas_real vulb__dot( vul_blas_real *a, vul_blas_real *b, int n );
static void vulb__mmul( vul_blas_real *out, vul_blas_real *A, vul_blas_real *x, int c, int r );
static void vulb__mmul_matrix( vul_blas_real *O, vul_blas_real *A, vul_blas_real *B, int n );
static void vulb__mmul_add( vul_blas_real *out, vul_blas_real *A, vul_blas_real *x, vul_blas_real *b, 
                            int c, int r );
static void vulb__forward_substitute( vul_blas_real *out, vul_blas_real *A, vul_blas_real *b, int c, int r );
static void vulb__backward_substitute( vul_blas_real *out, vul_blas_real *A, vul_blas_real *b, int c, int r,
                                       int transpose );
static void vulb__mtranspose( vul_blas_real *O, vul_blas_real *A, int c, int r );
static void vulb__mmul_matrix_rect( vul_blas_real *O, vul_blas_real *A, vul_blas_real *B, 
                                    int ca, int ra_cb, int rb );
/*
 * If transposed is set, treat A as A^T.
 * Matrices may not alias.
 * @NOTE(Thynn): Not currently used.
 */ 
static void vul__blas_qr_decomposition_gram_schmidt( vul_blas_real *Q, vul_blas_real *R, vul_blas_real *A, 
                                                     int c, int r, int transpose );
/*
 * If transposed is set, treat A as A^T.
 * Matrices may alias.
 * @NOTE(Thynn): Not currently used.
 */
static void vul__blas_qr_decomposition_householder( vul_blas_real *Q, vul_blas_real *R, vul_blas_real *A, 
                                                    int c, int r, int transpose );
/*
 * If transposed is set, treat A as A^T.
 * Matrices may not alias.
 */
static void vul__blas_qr_decomposition_givens( vul_blas_real *Q, vul_blas_real *R, vul_blas_real *A, 
                                               int c, int r, int transpose );

/*
 * Apply a Givens rotation to the matrix A at indices i and j
 * with the given sine/cosine. If post_multiply is set,
 * perform A = G * A instead of A = A * G.
 */
static void vul__blas_givens_rotate( vul_blas_real *A, int c, int r, 
                                     int i, int j, float cosine, float sine,
                                     int post_multiply );

//-----------------------------------------------------
// Dense Singular Value Decomposition local functions
//

/*
 * Use shell sort to sort the bases.
 */
static void vul__blas_svd_sort( vul_blas_svd_basis *x, int n );

/*
 * Qt and u are optional space to perform operations in (and to use the data afterwards, as in QR decomposition.
 * If they are NULL, we allocate room for them inside.
 * If QO and Q are not NULL, Q is postmultiplied by Qt into QO, allowing for accumulation of Q and not just R
 * during QR decomposition. If they (either of them) are NULL, this step is skipped.
 */ 
static void vul__blas_apply_householder_column( vul_blas_real *QO, vul_blas_real *A,
                                                vul_blas_real *RO, vul_blas_real *R,
                                                int c, int r, int qc, int qr, int k,
                                                vul_blas_real *Qt, vul_blas_real *u, int respect_signbit );
/*
 * Calculate the norm of the matrix' diagonal as a vector.
 */
static vul_blas_real vul__blas_matrix_norm_diagonal( vul_blas_real *A, int c, int r );

/*
 * Only values above the "upper_diag"-th diagonal are used, so for
 * the full matrix, use -min(r,c)/2.
 */
static vul_blas_real vul__blas_matrix_norm_as_single_column( vul_blas_real *A, int c, int r, int upper_diag );

//-----------------------------------------------------
// Dense Singular Value Decomposition local functions
//

/*
 * Calculate the norm of the matrix' diagonal as a vector.
 */
static vul_blas_real vul__blas_matrix_norm_diagonal_sparse( vul_blas_matrix *A );

/*
 * Only values above the "upper_diag"-th diagonal are used, so for
 * the full matrix, use -min(r,c)/2.
 */
static vul_blas_real vul__blas_matrix_norm_as_single_column_sparse( vul_blas_matrix *A, int upper_diag );
/*
 * Use shell sort to sort the bases.
 */
static void vul__blas_svd_sort_sparse( vul_blas_svd_basis_sparse *x, int n );

#ifdef _cplusplus
}
#endif
#endif

#ifdef VUL_DEFINE

#ifdef _cplusplus
extern "C" {
#endif

//-------------------------------
// Sparse datatype public functions
//

vul_blas_vector *vul_blas_vector_create( unsigned *idxs, vul_blas_real *vals, unsigned int n )
{
   unsigned int i;
   vul_blas_vector *v;

   v = ( vul_blas_vector* )VUL_BLAS_ALLOC( sizeof( vul_blas_vector ) );
   v->count = 0;
   v->entries = 0;
   for( i = 0; i < n; ++i ) {
      vul_blas_vector_insert( v, idxs[ i ], vals[ i ] );
   }
   return v;
}

vul_blas_matrix *vul_blas_matrix_create( unsigned int *rows, unsigned int *cols, vul_blas_real *vals, unsigned int init_count )
{
   vul_blas_matrix *m;
   unsigned int i;
   
   m = ( vul_blas_matrix* )VUL_BLAS_ALLOC( sizeof( vul_blas_matrix ) );
   m->count = 0;
   m->rows = 0;

   for( i = 0; i < init_count; ++i ) {
      vul_blas_matrix_insert( m, rows[ i ], cols[ i ], vals[ i ] );
   }
   return m;
}

void vul_blas_matrix_destroy( vul_blas_matrix *m )
{
   unsigned int i;
   for( i = 0; i < m->count; ++i ) {
      if( m->rows[ i ].vec.count ) {
         VUL_BLAS_FREE( m->rows[ i ].vec.entries );
      }
   }
   VUL_BLAS_FREE( m->rows );
   m->rows = 0;

   VUL_BLAS_FREE( m );
}

void vul_blas_vector_insert( vul_blas_vector *v, unsigned int idx, vul_blas_real val )
{
   vul_blas_sparse_entry *e;
   unsigned int i, j, inserted;

   // Overwrite, even if zero
   for( i = 0; i < v->count; ++i ) {
      if( v->entries[ i ].idx == idx ) {
         v->entries[ i ].val = val;
         return;
      }
   }

   // If zero, do not create a new entry
   if( val == 0.f ) {
      return;
   }

   e = ( vul_blas_sparse_entry* )VUL_BLAS_ALLOC( sizeof( vul_blas_sparse_entry ) * ( v->count + 1 ) );
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
      VUL_BLAS_FREE( v->entries );
   }
   v->entries = e;
   v->count = v->count + 1;
}

void vul_blas_matrix_insert( vul_blas_matrix *m, unsigned int r, unsigned int c, vul_blas_real val )
{
   unsigned int i, j, inserted;
   vul_blas_matrix_row *e;
   
   // Overwrite, even if zero
   for( i = 0; i < m->count; ++i ) {
      if( m->rows[ i ].idx == r ) {
         vul_blas_vector_insert( &m->rows[ i ].vec, c, val );
         return;
      }
   }

   // If zero, don't create a new entry
   if( val == 0.f ) {
      return;
   }

   e = ( vul_blas_matrix_row* )VUL_BLAS_ALLOC( sizeof( vul_blas_matrix_row ) * ( m->count + 1 ) );
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
            vul_blas_vector_insert( &e[ i ].vec, c, val );
            inserted = 1;
         }
      }
   } else {
      e[ 0 ].idx = r;
      e[ 0 ].vec.count = 0;
      e[ 0 ].vec.entries = 0;
      vul_blas_vector_insert( &e[ 0 ].vec, c, val );
   }
   if( m->rows ) {
      VUL_BLAS_FREE( m->rows );
   }
   m->rows = e;
   m->count = m->count + 1;
}

vul_blas_real vul_blas_vector_get( vul_blas_vector *v, unsigned int idx )
{
   unsigned int i;

   for( i = 0; i < v->count; ++i ) {
      if( v->entries[ i ].idx == idx ) {
         return v->entries[ i ].val;
      }
   }
   return 0.f;
}

vul_blas_real vul_blas_matrix_get( vul_blas_matrix *m, unsigned int r, unsigned int c )
{
   unsigned int i;

   for( i = 0; i < m->count; ++i ) {
      if( m->rows[ i ].idx == r ) {
         return vul_blas_vector_get( &m->rows[ i ].vec, c );
      }
   }
   return 0.f;
}
static vul_blas_vector vulb__blas_matrix_get_row_by_array_index( vul_blas_matrix *m, unsigned int r )
{
   vul_blas_vector v;

   if( r >= m->count ) {
      v.count = 0;
      v.entries = 0;
      return v;
   }
   return m->rows[ r ].vec;
}

void vul_blas_vector_destroy( vul_blas_vector *v )
{
   if( v->entries ) {
      VUL_BLAS_FREE( v->entries );
   }
   VUL_BLAS_FREE( v );
}

//----------------------------------------
// Sparse datatype local functions
//

#define DEFINE_VECTOR_OP( name, op )\
   static void name( vul_blas_vector *out, vul_blas_vector *a, vul_blas_vector *b )\
   {\
      unsigned int ia, ib;\
      ia = 0; ib = 0;\
      while( ia < a->count && ib < b->count ) {\
         if( a->entries[ ia ].idx == b->entries[ ib ].idx ) {\
            vul_blas_vector_insert( out, a->entries[ ia ].idx, a->entries[ ia ].val op b->entries[ ib ].val );\
            ++ia; ++ib;\
         } else if( a->entries[ ia ].idx < b->entries[ ib ].idx ) {\
            vul_blas_vector_insert( out, a->entries[ ia ].idx, 0.f op a->entries[ ia ].val );\
            ++ia;\
         } else {\
            vul_blas_vector_insert( out, b->entries[ ib ].idx, 0.f op b->entries[ ib ].val );\
            ++ib;\
         }\
      }\
      while( ia < a->count ) {\
         vul_blas_vector_insert( out, a->entries[ ia ].idx, 0.f op a->entries[ ia ].val );\
         ++ia;\
      }\
      while( ib < b->count ) {\
         vul_blas_vector_insert( out, b->entries[ ib ].idx, 0.f op b->entries[ ib ].val );\
         ++ib;\
      }\
   }
DEFINE_VECTOR_OP( vulb__sparse_vadd, + )
DEFINE_VECTOR_OP( vulb__sparse_vsub, - )
DEFINE_VECTOR_OP( vulb__sparse_vmul, * )

#undef DEFINE_VECTOR_OP

static void vulb__sparse_vmul_sub( vul_blas_vector *out, vul_blas_vector *a, vul_blas_vector *x, vul_blas_vector *b )
{
   unsigned int ia, ix, ib;

   ia= 0; ix = 0; ib = 0;
   while( ia < a->count && ix < x->count && ib < b->count ) {
      if( a->entries[ ia ].idx == x->entries[ ix ].idx && a->entries[ ia ].idx == b->entries[ ib ].idx ) {
         vul_blas_vector_insert( out, a->entries[ ia ].idx, a->entries[ ia ].val * x->entries[ ix ].val -
                                                             b->entries[ ib ].val );
         ++ia; ++ib; ++ix;
      } else if( b->entries[ ib ].idx <= a->entries[ ia ].idx && b->entries[ ib ].idx <= x->entries[ ix ].idx ) {
         vul_blas_vector_insert( out, b->entries[ ib ].idx, -b->entries[ ib ].val );
         ++ib;
      } else if( a->entries[ ia ].idx < x->entries[ ix ].idx ) {
         ++ia;
      } else {
         ++ix;
      }
   }
}
static void vulb__sparse_vmul_add( vul_blas_vector *out, vul_blas_vector *a, vul_blas_vector *x, vul_blas_vector *b )
{
   unsigned int ia, ix, ib;

   ia= 0; ix = 0; ib = 0;
   while( ia < a->count && ix < x->count && ib < b->count ) {
      if( a->entries[ ia ].idx == x->entries[ ix ].idx && a->entries[ ia ].idx == b->entries[ ib ].idx ) {
         vul_blas_vector_insert( out, a->entries[ ia ].idx, a->entries[ ia ].val * x->entries[ ix ].val +
                                                             b->entries[ ib ].val );
         ++ia; ++ib; ++ix;
      } else if( b->entries[ ib ].idx <= a->entries[ ia ].idx && b->entries[ ib ].idx <= x->entries[ ix ].idx ) {
         vul_blas_vector_insert( out, b->entries[ ib ].idx, b->entries[ ib ].val );
         ++ib;
      } else if( a->entries[ ia ].idx < x->entries[ ix ].idx ) {
         ++ia;
      } else {
         ++ix;
      }
   }
}

static void vulb__sparse_vcopy( vul_blas_vector *out, vul_blas_vector *x )
{
   unsigned int i;

   if( x->count ) {
      out->entries = ( vul_blas_sparse_entry* )VUL_BLAS_ALLOC( sizeof( vul_blas_sparse_entry ) * 
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
static vul_blas_real vulb__sparse_dot( vul_blas_vector *a, vul_blas_vector *b )
{
   vul_blas_real r;
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
static void vulb__sparse_mmul( vul_blas_vector *out, vul_blas_matrix *A, vul_blas_vector *x )
{
   unsigned int v, i, ix;
   vul_blas_real sum;

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
      vul_blas_vector_insert( out, A->rows[ v ].idx, sum );
   }
}
static void vulb__sparse_mmul_matrix( vul_blas_matrix *O, vul_blas_matrix *A, vul_blas_matrix *B, int n )
{
   vul_blas_real s;
   int i, j, k;
   
   //@TODO(thynn): Strassen instead of this naïve approach.
   for( i = 0; i < A->count; ++i ) {
      for( j = 0; j < n; ++j ) {
         s = 0.f;
         for( k = 0; k < A->rows[ i ].vec.count; ++k ) {
            s += A->rows[ i ].vec.entries[ k ].val 
               * vul_blas_matrix_get( B, A->rows[ i ].vec.entries[ k ].idx, j );
         }
         vul_blas_matrix_insert( O, i, j, s );
      }
   }
}
static void vulb__sparse_mmul_add( vul_blas_vector *out, vul_blas_matrix *A, vul_blas_vector *x, vul_blas_vector *b )
{
   unsigned int v, i, ix, ib, found;
   vul_blas_real sum;

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
      vul_blas_vector_insert( out, A->rows[ v ].idx, sum );
   }
}
static void vulb__sparse_forward_substitute( vul_blas_vector *out, vul_blas_matrix *A, vul_blas_vector *b )
{
   int i, j;
   vul_blas_real sum;

   for( i = 0; i < A->count; ++i ) {
      sum = vul_blas_vector_get( b, A->rows[ i ].idx );
      for( j = A->rows[ i ].vec.count - 1; 
           j >= 0 && A->rows[ i ].vec.entries[ j ].idx >= A->rows[ i ].idx ; 
           --j )
           ; // Find j = i - 1
      for( ; j >= 0 ; --j ) {
         sum -= A->rows[ i ].vec.entries[ j ].val * vul_blas_vector_get( out, A->rows[ i ].vec.entries[ j ].idx );
      }
      vul_blas_vector_insert( out, i, sum / vul_blas_matrix_get( A, A->rows[ i ].idx, A->rows[ i ].idx ) );
   }
}

static void vulb__sparse_backward_substitute( vul_blas_vector *out, vul_blas_matrix *A, vul_blas_vector *b )
{
   int i, j;
   vul_blas_real sum;

   for( i = A->count - 1; i >= 0; --i ) {
      sum = vul_blas_vector_get( b, A->rows[ i ].idx );
      for( j = 0; j < A->rows[ i ].vec.count && A->rows[ i ].vec.entries[ j ].idx <= A->rows[ i ].idx; ++j )
         ; // Find index of first column > i
      while( j < A->rows[ i ].vec.count ) {
         sum -= A->rows[ i ].vec.entries[ j ].val * vul_blas_vector_get( out, A->rows[ i ].vec.entries[ j ].idx );
         ++j;
      }
      vul_blas_vector_insert( out, i, sum / vul_blas_matrix_get( A, A->rows[ i ].idx, A->rows[ i ].idx ) );
   }
}
static void vulb__sparse_mtranspose( vul_blas_matrix *out, vul_blas_matrix *A )
{
   int i, j;
   
   vulb__sparse_mclear( out );
   for( i = 0; i < A->count; ++i ) {
      for( j = 0; j < A->rows[ i ].vec.count; ++j ) {
         vul_blas_matrix_insert( out, A->rows[ i ].vec.entries[ j ].idx, 
                                       A->rows[ i ].idx, 
                                       A->rows[ i ].vec.entries[ j ].val );
      }
   }
}
static void vulb__sparse_mcopy( vul_blas_matrix *out, vul_blas_matrix *A )
{
   int i, j;

   vulb__sparse_mclear( out );
   for( i = 0; i < A->count; ++i ) {
      for( j = 0; j < A->rows[ i ].vec.count; ++j ) {
         vul_blas_matrix_insert( out, A->rows[ i ].idx, 
                                       A->rows[ i ].vec.entries[ j ].idx, 
                                       A->rows[ i ].vec.entries[ j ].val );
      }
   }
}

static void vulb__sparse_mclear( vul_blas_matrix *A )
{
   int i;

   for( i = 0; i < A->count; ++i ) {
      if( A->rows[ i ].vec.count ) {
         VUL_BLAS_FREE( A->rows[ i ].vec.entries );
      }
      A->rows[ i ].vec.count = 0;
   }
   VUL_BLAS_FREE( A->rows );
   A->rows = 0;
   A->count = 0;
}
static void vulb__sparse_mclean( vul_blas_matrix *A )
{
   int i, j, k, c;
   vul_blas_sparse_entry *n;

   for( i = 0; i < A->count; ++i ) {
      c = 0;
      for( j = 0; j < A->rows[ i ].vec.count; ++j ) {
         if( A->rows[ i ].vec.entries[ j ].val != 0.f ) {
            ++c;
         }
      }
      if( c != A->rows[ i ].vec.count ) {
         if( c != 0 ) {
            n = ( vul_blas_sparse_entry* )VUL_BLAS_ALLOC( sizeof( vul_blas_sparse_entry ) * c );
            for( j = 0, k = 0; j < A->rows[ i ].vec.count; ++j ) {
               if( A->rows[ i ].vec.entries[ j ].val != 0.f ) {
                  n[ k ].val = A->rows[ i ].vec.entries[ j ].val;
                  n[ k ].idx = A->rows[ i ].vec.entries[ j ].idx;
                  ++k;
               }
            }
         } else {
            n = 0;
         }
         VUL_BLAS_FREE( A->rows[ i ].vec.entries );
         A->rows[ i ].vec.entries = n;
         A->rows[ i ].vec.count = c;
      }
   }
}

//---------------------
// Sparse solvers
//

vul_blas_vector *vul_blas_conjugate_gradient_sparse( vul_blas_matrix *A,
                                                       vul_blas_vector *initial_guess,
                                                       vul_blas_vector *b,
                                                       int max_iterations,
                                                       vul_blas_real tolerance )
{
   vul_blas_vector *x, *r, *Ap, *p;
   vul_blas_real rd, rd2, alpha, beta, tmp;
   int i, j, k, idx, found;

   r = vul_blas_vector_create( 0, 0, 0 );
   p = vul_blas_vector_create( 0, 0, 0 );
   Ap = vul_blas_vector_create( 0, 0, 0 );
   
   x = vul_blas_vector_create( 0, 0, 0 );
   vulb__sparse_vcopy( x, initial_guess );
   vulb__sparse_mmul( r, A, x );
   vulb__sparse_vsub( r, r, b );
   vulb__sparse_vcopy( p, r );

   rd = vulb__sparse_dot( r, r );
   for( i = 0; i < max_iterations; ++i ) {
      vulb__sparse_mmul( Ap, A, p );
      alpha = rd / vulb__sparse_dot( p, Ap );
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
            vul_blas_vector_insert( x, idx, -tmp );
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
            vul_blas_vector_insert( r, idx, -tmp );
         }
      }
      rd2 = vulb__sparse_dot( r, r );
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
            vul_blas_vector_insert( p, r->entries[ k ].idx, r->entries[ k ].val );
            ++k;
         }
      }
      rd = rd2;
   }

   vul_blas_vector_destroy( p );
   vul_blas_vector_destroy( r );
   vul_blas_vector_destroy( Ap );
   return x;
}

vul_blas_vector *vul_blas_successive_over_relaxation_sparse( vul_blas_matrix *A,
                                                               vul_blas_vector *initial_guess,
                                                               vul_blas_vector *b,
                                                               vul_blas_real relaxation_factor,
                                                               int max_iterations,
                                                               vul_blas_real tolerance )
{
   vul_blas_vector *x, *r;
   int i, j, k;
   vul_blas_real omega, rd, rd2, tmp;

   r = vul_blas_vector_create( 0, 0, 0 );
   x = vul_blas_vector_create( 0, 0, 0 );
      
   /* Calculate initial residual */
   vulb__sparse_vcopy( x, initial_guess );
   vulb__sparse_mmul( r, A, x );
   vulb__sparse_vsub( r, r, b );
   rd = vulb__sparse_dot( r, r );
      
   for( k = 0; k < max_iterations; ++k ) {
      /* Relax */
      for( i = 0; i < A->count; ++i ) {
         omega = 0.f;
         for( j = 0; j < x->count; ++j ) {
            if( A->rows[ i ].idx != x->entries[ j ].idx ) {
               omega += vul_blas_matrix_get( A, A->rows[ i ].idx, x->entries[ j ].idx ) * x->entries[ j ].val;
            }
         }
         tmp = ( 1.f - relaxation_factor ) * vul_blas_vector_get( x, A->rows[ i ].idx ) 
             + ( relaxation_factor / vul_blas_matrix_get( A, A->rows[ i ].idx, A->rows[ i ].idx ) )
             * ( vul_blas_vector_get( b, A->rows[ i ].idx ) - omega );
         if( tmp != 0.f ) {
            vul_blas_vector_insert( x, A->rows[ i ].idx, tmp );
         }
      }
      /* Check for convergence */
      vulb__sparse_mmul( r, A, x );
      vulb__sparse_vsub( r, r, b );
      rd2 = vulb__sparse_dot( r, r );
      if( fabs( rd2 - rd ) < tolerance * x->count ) {
         break;
      }
      rd = rd2;
   }
   
   vul_blas_vector_destroy( r );
   return x;
}

vul_blas_vector *vul_blas_cholesky_decomposition_sparse( vul_blas_matrix *A,
                                                           vul_blas_vector *b,
                                                           int cols, int rows )
{
   vul_blas_vector *x, *r, rowi, rowj;
   vul_blas_matrix *D, *DT;
   vul_blas_real sum, rd, rd2;
   int i, j, ki, kj, k;

   D = vul_blas_matrix_create( 0, 0, 0, 0 );
   DT = vul_blas_matrix_create( 0, 0, 0, 0 );
   r = vul_blas_vector_create( 0, 0, 0 );
   x = vul_blas_vector_create( 0, 0, 0 );

   for( i = 0; i < A->count; ++i ) {
      for( j = 0; j < A->rows[ i ].vec.count; ++j ) {
         vul_blas_matrix_insert( D, A->rows[ i ].idx, 
                                     A->rows[ i ].vec.entries[ j ].idx,
                                     A->rows[ i ].vec.entries[ j ].val );
      }
   }

   // Decomposition into L and L^T
   for( i = 0; i < cols; ++i ) {
      for( j = i; j < rows; ++j ) {
         sum = vul_blas_matrix_get( A, i, j );
         rowi = vulb__blas_matrix_get_row_by_array_index( D, i );
         rowj = vulb__blas_matrix_get_row_by_array_index( D, j );
         for( ki = rowi.count - 1; ki >= 0 && rowi.entries[ ki ].idx >= i; --ki )
            ; // Search backwards until ki < i
         for( kj = rowj.count - 1; kj >= 0 && rowj.entries[ kj ].idx >= i; --kj )
            ; // Search backwards until kj < i
         while( ki >= 0 && kj >= 0 ) {
            if( rowi.entries[ ki ].idx == rowj.entries[ kj ].idx ) {
               sum -= rowi.entries[ ki ].val * rowj.entries[ kj ].val;
               --ki; --kj;
            } else if( rowi.entries[ ki ].idx >= rowj.entries[ kj ].idx ) {
               --ki;
            } else {
               --kj;
            }
         }
         if( i == j ) {
            assert( sum > 0.f ); // Cholesky decomposition is only valid for POSITIVE-DEFINITE symmetric matrices
            vul_blas_matrix_insert( D, i, i, sqrt( sum ) );
         } else {
            rd = vul_blas_matrix_get( D, i, i );
            assert( rd != 0.f ); // Determinant is sufficiently small that a divide-by-zero is imminent
            vul_blas_matrix_insert( D, j, i, sum / rd );
         }
      }
   }
   
   // Solve Lr = b
   vulb__sparse_forward_substitute( r, D, b );
   // Solve L^Tx = r
   vulb__sparse_mtranspose( DT, D );
   vulb__sparse_backward_substitute( x, DT, r );

   vul_blas_matrix_destroy( D );
   vul_blas_matrix_destroy( DT );
   vul_blas_vector_destroy( r );

   return x;
}

static void vul__blas_givens_rotate_sparse( vul_blas_matrix *A, int c, int r, 
                                            int i, int j, float cosine, float sine,
                                            int post_multiply )
{
   int k;
   vul_blas_real G[ 4 ], v0, v1;

   G[ 0 ] = cosine;
   G[ 1 ] = sine;
   G[ 2 ] = -sine;
   G[ 3 ] = cosine;

   if( post_multiply ) {
      // R = G^T * R
      for( k = 0; k < c; ++k ) {
         v0 = G[ 0 ] * vul_blas_matrix_get( A, i, k ) 
            + G[ 2 ] * vul_blas_matrix_get( A, j, k ); // G[ 2 ]: T is transposed
         v1 = G[ 1 ] * vul_blas_matrix_get( A, i, k ) 
            + G[ 3 ] * vul_blas_matrix_get( A, j, k ); // G[ 1 ]: T is transposed
         vul_blas_matrix_insert( A, i, k, v0 );
         vul_blas_matrix_insert( A, j, k, v1 );
      }
   } else {
      // Q = Q * G
      for( k = 0; k < r; ++k ) {
         v0 = G[ 0 ] * vul_blas_matrix_get( A, k, i ) 
            + G[ 2 ] * vul_blas_matrix_get( A, k, j );
         v1 = G[ 1 ] * vul_blas_matrix_get( A, k, i) 
            + G[ 3 ] * vul_blas_matrix_get( A, k, j );
         vul_blas_matrix_insert( A, k, i, v0 );
         vul_blas_matrix_insert( A, k, j, v1 );
      }
   }
}

static void vul__blas_qr_decomposition_givens_sparse( vul_blas_matrix *Q, vul_blas_matrix *R, 
                                                      vul_blas_matrix *A, int c, int r )
{
   int i, j, k;
   vul_blas_real theta, st, ct, G[ 4 ], v0, v1, *RA;

   vulb__sparse_mcopy( R, A );
   vulb__sparse_mclear( Q );

   for( i = 0; i < r; ++i ) {
      vul_blas_matrix_insert( Q, i, i, 1.f );
   }
   
   for( j = 0; j < c; ++j ) {
      for( i = r - 2; i >= j; --i ) {
         v0 = vul_blas_matrix_get( R, i, j );
         v1 = vul_blas_matrix_get( R, i + 1, j );
         theta = v0 * v0 + v1 * v1;
         if( theta != 0.f ) {
            theta = sqrt( theta );
            ct =  v0 / theta;
            st = -v1 / theta;
         } else {
            st = 0.f;
            ct = 1.f;
         }
         vul__blas_givens_rotate_sparse( R, c, r, i, i + 1, ct, st, 1 );
         vul__blas_givens_rotate_sparse( Q, r, r, i, i + 1, ct, st, 0 );
         vulb__sparse_mclean( R );
         vulb__sparse_mclean( Q );
      }
   }
}

vul_blas_vector *vul_blas_qr_decomposition_sparse( vul_blas_matrix *A,
                                                   vul_blas_vector *b,
                                                   int cols, int rows )
{
   vul_blas_matrix *Q, *QT, *R;
   vul_blas_vector *x, *d;

   Q = vul_blas_matrix_create( 0, 0, 0, 0 );
   QT = vul_blas_matrix_create( 0, 0, 0, 0 );
   R = vul_blas_matrix_create( 0, 0, 0, 0 );
   d = vul_blas_vector_create( 0, 0, 0 );
   x = vul_blas_vector_create( 0, 0, 0 );

   vul__blas_qr_decomposition_givens_sparse( Q, R, A, cols, rows );
   vulb__sparse_mtranspose( QT, Q );
   vulb__sparse_mmul( d, QT, b );
   vulb__sparse_backward_substitute( x, R, d );

   vul_blas_matrix_destroy( Q );
   vul_blas_matrix_destroy( QT );
   vul_blas_matrix_destroy( R );
   vul_blas_vector_destroy( d );

   return x;
}

//-------------------------------------------------------------
// Sparse singular value decomposition local functions
//

static vul_blas_real vul__blas_matrix_norm_diagonal_sparse( vul_blas_matrix *A )
{
   vul_blas_real v;
   int i, j;
   
   v = 0.f;
   for( i = 0; i < A->count; ++i ) {
      for( j = 0; j < A->rows[ i ].vec.count; ++j ) {
         if( A->rows[ i ].vec.entries[ j ].idx == A->rows[ i ].idx ) {
            v += A->rows[ i ].vec.entries[ j ].val * A->rows[ i ].vec.entries[ j ].val;
         }
      }
   }
   return sqrt( v );
}

static vul_blas_real vul__blas_matrix_norm_as_single_column_sparse( vul_blas_matrix *A, int upper_diag )
{
   vul_blas_real v;
   int i, j, s;

   v = 0.f;
   for( i = 0; i < A->count; ++i ) {
      s = ( int )A->rows[ i ].idx + upper_diag < 0 ? 0 : ( int )A->rows[ i ].idx + upper_diag;
      for( j = 0; j < A->rows[ i ].vec.count && A->rows[ i ].vec.entries[ j ].idx <= s; ++j )
         ; // Advance to the upper diagonal
      for( ; j < A->rows[ i ].vec.count; ++j ) {
         v += A->rows[ i ].vec.entries[ j ].val * A->rows[ i ].vec.entries[ j ].val;
      }
   }
   return v;
}

vul_blas_matrix *vul_blas_svd_basis_reconstruct_matrix_sparse( vul_blas_svd_basis_sparse *x, int n )
{
   int i, j, k, l;
   vul_blas_matrix *M;

   M = vul_blas_matrix_create( 0, 0, 0, 0 );
   if( !n ) {
      return M;
   }

   // We sum from smallest to largest sigma for numerical stability
   for( k = n - 1; k >= 0; --k ) {
      for( i = 0; i < x[ k ].u_length; ++i ) {
         for( j = 0; j < x[ k ].v_length; ++j ) {
            vul_blas_matrix_insert( M, i, j, vul_blas_matrix_get( M, i, j ) + 
                                              x[ k ].sigma * vul_blas_vector_get( x[ k ].u, i )
                                                           * vul_blas_vector_get( x[ k ].v, j ) );
         }
      }
   }
   return M;
}

void vul_blas_svd_basis_destroy_sparse( vul_blas_svd_basis_sparse *x, int n )
{
   int i;

   for( i = 0; i < n; ++i ) {
      if( x[ i ].u ) {
         vul_blas_vector_destroy( x[ i ].u );
         x[ i ].u = 0;
      }
      if( x[ i ].v ) {
         vul_blas_vector_destroy( x[ i ].v );
         x[ i ].v = 0;
      }
   }
}

static void vul__blas_svd_sort_sparse( vul_blas_svd_basis_sparse *x, int n )
{
   // Shell sort, gaps for up to about a 10M singular values. Good luck exceeding that with this solver!
   vul_blas_svd_basis_sparse tmp;
   int gap, gi, i, j;
   int gaps[ ] = { 4071001, 1170001, 237001, 67001, 17001, 5001, 1701, 701, 301, 132, 67, 23, 10, 4, 1 };

   for( gi = 0; gi < sizeof( gaps ) / sizeof( int ); ++gi ) {
      for( i = 0 + gaps[ gi ]; i < n; ++i ) {
         gap = gaps[ gi ];
         memcpy( &tmp, &x[ i ], sizeof( vul_blas_svd_basis_sparse ) );
         for( j = i; j >= gap && x[ j - gap ].sigma <= tmp.sigma; j -= gap ) {
            memcpy( &x[ j ], &x[ j - gap ], sizeof( vul_blas_svd_basis_sparse ) );
         }
         memcpy( &x[ j ], &tmp, sizeof( vul_blas_svd_basis_sparse ) );
      }
   }
}

void vul_blas_svd_sparse_qrlq( vul_blas_svd_basis_sparse *out, int *rank,
                               vul_blas_matrix *A,
                               int c, int r, vul_blas_real eps, int itermax )
{
   vul_blas_matrix *U0, *U1, *V0, *V1, *S0, *S1, *Sb, *Q, *tmp;
   vul_blas_real err, e, f, scale;
   int iter, n, i, j, k, ri, ci;

   n = r > c ? r : c;
   U0 = vul_blas_matrix_create( 0, 0, 0, 0 );
   U1 = vul_blas_matrix_create( 0, 0, 0, 0 );
   V0 = vul_blas_matrix_create( 0, 0, 0, 0 );
   V1 = vul_blas_matrix_create( 0, 0, 0, 0 );
   S0 = vul_blas_matrix_create( 0, 0, 0, 0 );
   S1 = vul_blas_matrix_create( 0, 0, 0, 0 );
   Sb = vul_blas_matrix_create( 0, 0, 0, 0 );
   Q  = vul_blas_matrix_create( 0, 0, 0, 0 );
   iter = 0;
   err = FLT_MAX;

   // Initialize to empty for U, V, Q. Set S to A^T
   vulb__sparse_mtranspose( S0, A );
   // Scale the matrix by the inverse of the largest coefficient for numerical
   // stability. We multiply the scale back into the singular values.
   scale = -FLT_MAX;
   for( i = 0; i < S0->count; ++i ) {
      for( j = 0; j < S0->rows[ i ].vec.count; ++j ) {
         scale = fabs( S0->rows[ i ].vec.entries[ j ].val ) > scale 
               ? fabs( S0->rows[ i ].vec.entries[ j ].val ) : scale;
      }
   }
   f = 1.0 / scale;
   for( i = 0; i < S0->count; ++i ) {
      for( j = 0; j < S0->rows[ i ].vec.count; ++j ) {
         S0->rows[ i ].vec.entries[ j ].val *= f;
      }
   }

   // Initialize as identity matrix
   for( i = 0; i < r; ++i ) {
      vul_blas_matrix_insert( U0, i, i, 1.f );
   }
   for( i = 0; i < c; ++i ) {
      vul_blas_matrix_insert( V0, i, i, 1.f );
   }
   while( err > eps && iter++ < itermax ) {
      // Store away old S
      vulb__sparse_mcopy( Sb, S0 );

      // Decompose
      vulb__sparse_mtranspose( S1, S0 );
      vul__blas_qr_decomposition_givens_sparse( Q, S0, S1, c, r );
      vulb__sparse_mmul_matrix( U1, U0, Q, r );

      vulb__sparse_mtranspose( S1, S0 );
      vul__blas_qr_decomposition_givens_sparse( Q, S0, S1, r, c );
      vulb__sparse_mmul_matrix( V1, V0, Q, c );

      tmp = U0; U0 = U1; U1 = tmp;
      tmp = V0; V0 = V1; V1 = tmp;

      // Calculate error
      e = vul__blas_matrix_norm_as_single_column_sparse( S0, 1 );
      f = vul__blas_matrix_norm_diagonal_sparse( S0 );
      if( f == 0.f ) {
         f = 1.f;
      }
      // Break if error increases
      if( ( e / f ) > err ) {
         tmp = Sb; Sb = S0; S0 = tmp;
         tmp = U0; U0 = U1; U1 = tmp;
         tmp = V0; V0 = V1; V1 = tmp;
         break;
      }
      err = e / f;
   }

   // Grap sigmas and rank, sort decreasing
   k = r < c ? r : c;
   for( j = 0, i = 0; i < k; ++i ) { // Since we're transposed, S is indexed with r and not c
      out[ i ].sigma = fabs( vul_blas_matrix_get( S0, i, i ) ) * scale;
      out[ i ].axis = i;
      if( out[ i ].sigma > eps ) {
         ++j;
      }
   }
   if( *rank == 0 || j < *rank ) {
      *rank = j;
   }
   vul__blas_svd_sort_sparse( out, k );

   // Fix signs and copy U
   for( i = 0; i < *rank; ++i ) {
      out[ i ].u_length = r;
      out[ i ].v_length = c;
      out[ i ].u = vul_blas_vector_create( 0, 0, 0 );
      out[ i ].v = vul_blas_vector_create( 0, 0, 0 );
      f = vul_blas_matrix_get( S0, out[ i ].axis, out[ i ].axis ) < 0.f ? -1.f : 1.f;
      for( j = 0; j < r; ++j ) {
         vul_blas_vector_insert( out[ i ].u, j, vul_blas_matrix_get( U0, j, out[ i ].axis ) * f );
      }
      for( j = 0; j < c; ++j ) {
         vul_blas_vector_insert( out[ i ].v, j, vul_blas_matrix_get( V0, j, out[ i ].axis ) );
      }
   }

   vul_blas_matrix_destroy( U0 );
   vul_blas_matrix_destroy( U1 );
   vul_blas_matrix_destroy( V0 );
   vul_blas_matrix_destroy( V1 );
   vul_blas_matrix_destroy( S0 );
   vul_blas_matrix_destroy( S1 );
   vul_blas_matrix_destroy( Sb );
   vul_blas_matrix_destroy( Q );
}

void vul_blas_svd_sparse( vul_blas_svd_basis_sparse *out, int *rank,
                          vul_blas_matrix *A,
                          int c, int r, vul_blas_real eps, int itermax )
{
   vul_blas_matrix *J, *U, *V, *G;
   vul_blas_vector *omegas;
   vul_blas_real f, t, vik, vjk, scale;
   int iter, n, m, i, j, k, ri, ci, nonzero;

   n = r > c ? r : c;
   m = r < c ? r : c;
   U = vul_blas_matrix_create( 0, 0, 0, 0 );
   V = vul_blas_matrix_create( 0, 0, 0, 0 );
   G = vul_blas_matrix_create( 0, 0, 0, 0 );
   omegas = vul_blas_vector_create( 0, 0, 0 );
   iter = 0;
   nonzero = c;

   // Initialize S to A scaled by the maximum coefficient of A for numerical stability
   scale = -FLT_MAX;
   for( i = 0; i < A->count; ++i ) {
      for( j = 0; j < A->rows[ i ].vec.count; ++j ) {
         scale = fabs( A->rows[ i ].vec.entries[ j ].val ) > scale 
               ? fabs( A->rows[ i ].vec.entries[ j ].val ) : scale;
      }
   }
   f = 1.0 / scale;
   for( i = 0; i < A->count; ++i ) {
      for( j = 0; j < A->rows[ i ].vec.count; ++j ) {
         vul_blas_matrix_insert( G, A->rows[ i ].idx, A->rows[ i ].vec.entries[ j ].idx, 
                                     A->rows[ i ].vec.entries[ j ].val * f );
      }
   }
   
   // Initialize U and V as identity matrices
   for( i = 0; i < r; ++i ) {
      vul_blas_matrix_insert( U, i, i, 1.f );
   }
   for( i = 0; i < c; ++i ) {
      vul_blas_matrix_insert( V, i, i, 1.f );
   }

   while( nonzero && iter++ < itermax ) {
      nonzero = 0;
      for( i = 0; i < r - 1; ++i ) {
         for( j = i + 1; j < r; ++j ) {
            vul_blas_real aii, aij, ajj;
            
            // Compute a_ij, a_ji, a_ii
            aii = 0.0; aij = 0.0; ajj = 0.0;
            for( k = 0; k < c; ++k ) {
               vik = vul_blas_matrix_get( G, i, k );
               vjk = vul_blas_matrix_get( G, j, k );

               aii += vik * vik;
               ajj += vjk * vjk;
               aij += vik * vjk;
            }
            if( fabs( aij ) > eps ) {
               nonzero += 1;
               vul_blas_real tau, t, ct, st;
               tau = ( aii - ajj ) / ( 2.0 * aij );
               t = copysign( 1.0 / ( fabs( tau ) + sqrt( 1.0 + tau * tau ) ), tau );
               ct = 1.0 / sqrt( 1.0 + t * t );
               st = ct * t;
               vul__blas_givens_rotate_sparse( G, c, r, j, i, ct, st, 1 );
               if( j < r ) {
                  vul__blas_givens_rotate_sparse( U, r, r, j, i, ct, st, 0 );
               }
            }
         }
      }
   }

   // Calculate the singular values (2-norm of the columns of G)
   for( i = 0; i < r; ++i ) {
      t = 0.0;
      for( j = 0; j < c; ++j ) {
         f = vul_blas_matrix_get( G, i, j );
         t += f * f;
      }
      vul_blas_vector_insert( omegas, i, sqrt( t ) );
   }

   // Calculate V
   for( i = 0; i < c; ++i ) { // The rest is zero
      if( fabs( vul_blas_vector_get( omegas, i ) ) > eps ) { // Ignore zero singular values
         for( j = 0; j < c; ++j ) {
            vul_blas_matrix_insert( V, j, i, vul_blas_matrix_get( G, i, j ) / 
                                              vul_blas_vector_get( omegas, i ) );
         }
      }
   }

   // Grap sigmas and rank, sort decreasing
   k = r < c ? r : c;
   for( j = 0, i = 0; i < c; ++i ) {
      out[ i ].sigma = fabs( vul_blas_vector_get( omegas, i ) ) * scale;
      out[ i ].axis = i;
      if( out[ i ].sigma > eps ) {
         ++j;
      }
   }
   if( *rank == 0 || j < *rank ) {
      *rank = j;
   }
   vul__blas_svd_sort_sparse( out, k );

   // Fix signs and copy U
   for( i = 0; i < *rank; ++i ) {
      out[ i ].u_length = r;
      out[ i ].v_length = c;
      out[ i ].u = vul_blas_vector_create( 0, 0, 0 );
      out[ i ].v = vul_blas_vector_create( 0, 0, 0 );
      f = vul_blas_vector_get( omegas, out[ i ].axis ) < 0.f ? -1.f : 1.f;
      for( j = 0; j < r; ++j ) {
         vul_blas_vector_insert( out[ i ].u, j, vul_blas_matrix_get( U, j, out[ i ].axis ) * f );
      }
      for( j = 0; j < c; ++j ) {
         vul_blas_vector_insert( out[ i ].v, j, vul_blas_matrix_get( V, j, out[ i ].axis ) );
      }
   }

   vul_blas_matrix_destroy( U );
   vul_blas_matrix_destroy( V );
   vul_blas_matrix_destroy( G );
   vul_blas_vector_destroy( omegas );
}

vul_blas_vector *vul_blas_linear_least_squares_sparse( vul_blas_matrix *A,
                                                         vul_blas_vector *b,
                                                         int c, int r,
                                                         int max_iterations,
                                                         vul_blas_real tolerance )
{
   vul_blas_svd_basis_sparse *res;
   vul_blas_matrix *U, *V;
   vul_blas_vector *S, *d, *out;
   vul_blas_real v;
   int rank, i, j, k, m;

   res = ( vul_blas_svd_basis_sparse* )VUL_BLAS_ALLOC( sizeof( vul_blas_svd_basis_sparse ) * c );
   rank = 0;
   vul_blas_svd_sparse( res, &rank, A, c, r, tolerance, max_iterations );
   
   U = vul_blas_matrix_create( 0, 0, 0, 0 );
   V = vul_blas_matrix_create( 0, 0, 0, 0 );
   S = vul_blas_vector_create( 0, 0, 0 );
   d = vul_blas_vector_create( 0, 0, 0 );
   out = vul_blas_vector_create( 0, 0, 0 );

   // Reconstruct U^T
   for( i = 0; i < rank; ++i ) {
      for( j = 0; j < res[ i ].u->count; ++j ) {
         vul_blas_matrix_insert( U, res[ i ].axis, res[ i ].u->entries[ j ].idx, res[ i ].u->entries[ j ].val );
      }
   }
   // Reconstruct V
   for( i = 0; i < rank; ++i ) {
      for( j = 0; j < res[ i ].v->count; ++j ) {
         vul_blas_matrix_insert( V, res[ i ].v->entries[ j ].idx, res[ i ].axis, res[ i ].v->entries[ j ].val );
      }
   }
   // Reconstruct the diagonal of S
   for( i = 0; i < rank; ++i ) {
      vul_blas_vector_insert( S, res[ i ].axis, res[ i ].sigma );
   }
   // Free the basis'
   vul_blas_svd_basis_destroy_sparse( res, rank );
   VUL_BLAS_FREE( res );

   // Calculate d = U^T * b
   for( i = 0; i < U->count; ++i ) {
      v = 0.0;
      for( j = 0; j < U->rows[ i ].vec.count; ++j ) {
         v += U->rows[ i ].vec.entries[ j ].val * vul_blas_vector_get( b, U->rows[ i ].vec.entries[ j ].idx );
      }
      vul_blas_vector_insert( d, U->rows[ i ].idx, v );
   }

   // Calculate x = V * S^+ * d
   for( i = 0; i < V->count; ++i ) {
      v = 0.0;
      for( j = 0; j < V->rows[ i ].vec.count; ++j ) {
         v += V->rows[ i ].vec.entries[ j ].val
            * vul_blas_vector_get( d, V->rows[ i ].vec.entries[ j ].idx )
            / vul_blas_vector_get( S, V->rows[ i ].vec.entries[ j ].idx );
      }
      vul_blas_vector_insert( out, V->rows[ i ].idx, v );
   }
   
   vul_blas_matrix_destroy( U );
   vul_blas_matrix_destroy( V );
   vul_blas_vector_destroy( S );
   vul_blas_vector_destroy( d );

   return out;
}


//------------------------
// Dense local functions
//

// @NOTE(thynn): The reason we index y over x is that I fucked up, and initially had them mixed up,
// and once I discovered the bug I had been consistent enough that simply swapping indices meant it all worked.
// So now we index (y,x) instead of (x,y). Oops. But it's only internal in this file anyway.
#ifdef VUL_BLAS_ROW_MAJOR
#define IDX( A, y, x, c, r ) A[ ( y ) * ( c ) + ( x ) ]
#else
#define IDX( A, y, x, c, r ) A[ ( x ) * ( r ) + ( y ) ]
#endif

#define DEFINE_VECTOR_OP( name, op )\
   static void name( vul_blas_real *out, vul_blas_real *a, vul_blas_real *b, int n )\
   {\
      int i;\
      for( i = 0; i < n; ++i ) {\
         out[ i ] = a[ i ] op b[ i ];\
      }\
   }
DEFINE_VECTOR_OP( vulb__vadd, + )
DEFINE_VECTOR_OP( vulb__vsub, - )
DEFINE_VECTOR_OP( vulb__vmul, * )

#undef DEFINE_VECTOR_OP

static inline void vulb__swap_ptr( vul_blas_real **a, vul_blas_real **b )
{
   vul_blas_real *t = *a;
   *a = *b;
   *b = t;
}

static void vulb__vmul_sub( vul_blas_real *out, vul_blas_real *a, vul_blas_real x, vul_blas_real *b, int n )
{
   int i;
   for( i = 0; i < n; ++i ) {
      out[ i ] = a[ i ] * x - b[ i ];
   }
}

static void vulb__vmul_add( vul_blas_real *out, vul_blas_real *a, vul_blas_real x, vul_blas_real *b, int n )
{
   int i;
   for( i = 0; i < n; ++i ) {
      out[ i ] = a[ i ] * x + b[ i ];
   }
}
      

static void vulb__vcopy( vul_blas_real *out, vul_blas_real *x, int n )
{
   int i;
   for( i = 0; i < n; ++i ) {
      out[ i ] = x[ i ];
   }
}

static vul_blas_real vulb__dot( vul_blas_real *a, vul_blas_real *b, int n )
{
   vul_blas_real f;
   int i;

   f = a[ 0 ] * b[ 0 ];
   for( i = 1; i < n; ++i ) {
      f += a[ i ] * b[ i ];
   }
   return f;
}

static void vulb__mmul( vul_blas_real *out, vul_blas_real *A, vul_blas_real *x, int c, int r )
{
   int i, j;
   for( i = 0; i < r; ++i ) {
      out[ i ] = 0;
      for( j = 0; j < c; ++j ) {
         out[ i ] += IDX( A, i, j, c, r ) * x[ j ];
      }
   }
}
static void vulb__mmul_matrix( vul_blas_real *O, vul_blas_real *A, vul_blas_real *B, int n )
{
   vul_blas_real s;
   int i, j, k;
   // @TODO(thynn): Strassen instead of this naïve approach.
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

static void vulb__mmul_add( vul_blas_real *out, vul_blas_real *A, vul_blas_real *x, vul_blas_real *b, 
                            int c, int r )
{
   int i, j;
   for( i = 0; i < r; ++i ) {
      out[ i ] = b[ i ];
      for( j = 0; j < c; ++j ) {
         out[ i ] += IDX( A, i, j, c, r ) * x[ j ];
      }
   }
}

static void vulb__forward_substitute( vul_blas_real *out, vul_blas_real *A, vul_blas_real *b, int c, int r )
{
   int i, j;

   for( i = 0; i < r; ++i ) {
      vul_blas_real sum = b[ i ];
      for( j = i - 1; j >= 0; --j ) {
         sum -= IDX( A, i, j, c, r ) * out[ j ];
      }
      out[ i ] = sum / IDX( A, i, i, c, r );
   }
}

static void vulb__backward_substitute( vul_blas_real *out, vul_blas_real *A, vul_blas_real *b, int c, int r,
                                       int transpose )
{
   int i, j;

   if( transpose ) {
      for( i = c - 1; i >= 0; --i ) {
         vul_blas_real sum = b[ i ];
         for( j = i + 1; j < r; ++j ) {
            sum -= IDX( A, j, i, c, r ) * out[ j ];
         }
         out[ i ] = sum / IDX( A, i, i, c, r );
      }
   } else {
      for( i = r - 1; i >= 0; --i ) {
         vul_blas_real sum = b[ i ];
         for( j = i + 1; j < c; ++j ) {
            sum -= IDX( A, i, j, c, r ) * out[ j ];
         }
         out[ i ] = sum / IDX( A, i, i, c, r );
      }
   }
}
static void vulb__mtranspose( vul_blas_real *O, vul_blas_real *A, int c, int r )
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
static void vulb__mmul_matrix_rect( vul_blas_real *O, vul_blas_real *A, vul_blas_real *B, int ra, int rb_ca, int cb )
{
   int i, j, k;
   vul_blas_real d;

   // @TODO(thynn): Strassen instead of this naïve approach.
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
// Dense solvers
//

void vul_blas_conjugate_gradient_dense( vul_blas_real *out,
                                        vul_blas_real *A,
                                        vul_blas_real *initial_guess,
                                        vul_blas_real *b,
                                        int n,
                                        int max_iterations,
                                        vul_blas_real tolerance )
{
   vul_blas_real *x, *r, *Ap, *p;
   vul_blas_real rd, rd2, alpha, beta;
   int i, j;

   r = ( vul_blas_real* )VUL_BLAS_ALLOC( sizeof( vul_blas_real ) * n );
   p = ( vul_blas_real* )VUL_BLAS_ALLOC( sizeof( vul_blas_real ) * n );
   Ap = ( vul_blas_real* )VUL_BLAS_ALLOC( sizeof( vul_blas_real ) * n );
   
   x = out;
   vulb__vcopy( out, initial_guess, n );
   vulb__mmul( r, A, x, n, n );
   vulb__vsub( r, r, b, n );
   vulb__vcopy( p, r, n );

   rd = vulb__dot( r, r, n );
   for( i = 0; i < max_iterations; ++i ) {
      vulb__mmul( Ap, A, p, n, n );
      alpha = rd / vulb__dot( p, Ap, n );
      for( j = 0; j < n; ++j ) {
         x[ j ] -= p[ j ] * alpha;
         r[ j ] -= Ap[ j ] * alpha;
      }
      rd2 = vulb__dot( r, r, n );
      if( fabs( rd2 - rd ) < tolerance * n ) {
         break;
      }
      beta = rd2 / rd;
      for( j = 0; j < n; ++j ) {
         p[ j ] = r[ j ] + p[ j ] * beta;
      }
      rd = rd2;
   }

   VUL_BLAS_FREE( p );
   VUL_BLAS_FREE( r );
   VUL_BLAS_FREE( Ap );
}

void vul_blas_lu_decomposition_dense( vul_blas_real *out,
                                      vul_blas_real *A,
                                      vul_blas_real *initial_guess,
                                      vul_blas_real *b,
                                      int n,
                                      int max_iterations,
                                      vul_blas_real tolerance )
{
   vul_blas_real *x, *LU, *r, *scale;
   vul_blas_real sum, rd, rd2, tmp, largest;
   int i, j, k, imax, iold, *indices;

   LU = ( vul_blas_real* )VUL_BLAS_ALLOC( sizeof( vul_blas_real ) * n * n );
   r = ( vul_blas_real* )VUL_BLAS_ALLOC( sizeof( vul_blas_real ) * n );
   scale = ( vul_blas_real* )VUL_BLAS_ALLOC( sizeof( vul_blas_real ) * n );
   indices = ( int* )VUL_BLAS_ALLOC( sizeof( int ) * n );
   x = out;
   
   /* Crout's LUP decomposition with pivoting and scaling */
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
   vulb__vcopy( x, initial_guess, n );
   vulb__mmul( r, A, x, n, n );
   vulb__vsub( r, b, r, n );
   rd = vulb__dot( r, r, n );

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
      vulb__backward_substitute( r, LU, r, n, n, 0 );

      /* Subtract the error from the old solution */
      vulb__vsub( x, x, r, n );

      /* Break if within tolerance */
      rd2 = vulb__dot( r, r, n );
      if( fabs( rd2 - rd ) < tolerance * n ) {
         break;
      }
      vulb__mmul( r, A, x, n, n );
      vulb__vsub( r, r, b, n );
      rd = rd2;
   }
   
   VUL_BLAS_FREE( LU );
   VUL_BLAS_FREE( r );
   VUL_BLAS_FREE( scale );
   VUL_BLAS_FREE( indices );
}

void vul_blas_cholesky_decomposition_dense( vul_blas_real *out,
                                            vul_blas_real *A,
                                            vul_blas_real *b,
                                            int n )
{
   vul_blas_real *x, *D, *r;
   vul_blas_real sum, rd, rd2;
   int i, j, k;

   D = ( vul_blas_real* )VUL_BLAS_ALLOC( sizeof( vul_blas_real ) * n * n );
   r = ( vul_blas_real* )VUL_BLAS_ALLOC( sizeof( vul_blas_real ) * n );
   x = out;
   
   // Copy work matrix
   memcpy( D, A, sizeof( vul_blas_real ) * n * n );

   // Decomposition
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
   // Solve Lr = b
   vulb__forward_substitute( r, D, b, n, n );
   // Solve L^Tx = r
   vulb__backward_substitute( out, D, r, n, n, 1 );

   VUL_BLAS_FREE( D );
   VUL_BLAS_FREE( r );
}

void vul_blas_qr_decomposition_dense( vul_blas_real *out,
                                      vul_blas_real *A,
                                      vul_blas_real *b,
                                      int n )
{
   vul_blas_real *d, *Q, *R, sum, rd, rd2;
   int i, j, k;

   d = ( vul_blas_real* )VUL_BLAS_ALLOC( sizeof( vul_blas_real ) * n );
   Q = ( vul_blas_real* )VUL_BLAS_ALLOC( sizeof( vul_blas_real ) * n * n );
   R = ( vul_blas_real* )VUL_BLAS_ALLOC( sizeof( vul_blas_real ) * n * n );

   vul__blas_qr_decomposition_givens( Q, R, A, n, n, 0 );

   for( i = 0; i < n; ++i ) {
      sum = 0;
      for( j = 0; j < n; ++j ) {
         sum += IDX( Q, j, i, n, n ) * b[ j ];
      }
      d[ i ] = sum;
   }

   vulb__backward_substitute( out, R, d, n, n, 0 );

   VUL_BLAS_FREE( d );
   VUL_BLAS_FREE( Q );
   VUL_BLAS_FREE( R );
}

void vul_blas_successive_over_relaxation_dense( vul_blas_real *out,
                                                vul_blas_real *A,
                                                vul_blas_real *initial_guess,
                                                vul_blas_real *b,
                                                vul_blas_real relaxation_factor,
                                                int n,
                                                int max_iterations,
                                                vul_blas_real tolerance )
{
   vul_blas_real *x, *r;
   int i, j, k;
   vul_blas_real omega, rd, rd2;

   r = ( vul_blas_real* )VUL_BLAS_ALLOC( sizeof( vul_blas_real ) * n );
      
   x = out;
   /* Calculate initial residual */
   vulb__vcopy( x, initial_guess, n );
   vulb__mmul( r, A, x, n, n );
   vulb__vsub( r, r, b, n );
   rd = vulb__dot( r, r, n );
      
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
      vulb__mmul( r, A, x, n, n );
      vulb__vsub( r, r, b, n );
      rd2 = vulb__dot( r, r, n );
      if( fabs( rd2 - rd ) < tolerance * n ) {
         break;
      }
      rd = rd2;
   }
   
   VUL_BLAS_FREE( r );
}

//---------------------------------------
// Dense singular value decomposition
// 

void vul_blas_svd_basis_reconstruct_matrix( vul_blas_real *M, vul_blas_svd_basis *x, int n )
{
   int i, j, k, l;

   if( !n ) {
      return;
   }

   memset( M, 0, sizeof( vul_blas_real ) * x[ 0 ].u_length * x[ 0 ].v_length );

   // Sum from smallest to largest sigma for numerical stability
   for( k = n - 1; k >= 0; --k ) {
      for( i = 0; i < x[ k ].u_length; ++i ) {
         for( j = 0; j < x[ k ].v_length; ++j ) {
            IDX( M, i, j, x[ k ].v_length, x[ k ].u_length ) += x[ k ].sigma * x[ k ].u[ i ] * x[ k ].v[ j ];
         }
      }
   }
}

void vul_blas_svd_basis_destroy( vul_blas_svd_basis *x, int n )
{
   int i;

   for( i = 0; i < n; ++i ) {
      if( x[ i ].u ) {
         VUL_BLAS_FREE( x[ i ].u );
         x[ i ].u = 0;
      }
      if( x[ i ].v ) {
         VUL_BLAS_FREE( x[ i ].v );
         x[ i ].v = 0;
      }
   }
}

static void vul__blas_svd_sort( vul_blas_svd_basis *x, int n )
{
   // Shell sort, gaps for up to about a 1000 singular values.
   vul_blas_svd_basis tmp;
   int gap, gi, i, j;
   int gaps[ ] = { 4071001, 1170001, 237001, 67001, 17001, 5001, 1701, 701, 301, 132, 67, 23, 10, 4, 1 };

   for( gi = 0; gi < sizeof( gaps ) / sizeof( int ); ++gi ) {
      for( i = 0 + gaps[ gi ]; i < n; ++i ) {
         gap = gaps[ gi ];
         memcpy( &tmp, &x[ i ], sizeof( vul_blas_svd_basis ) );
         for( j = i; j >= gap && x[ j - gap ].sigma <= tmp.sigma; j -= gap ) {
            memcpy( &x[ j ], &x[ j - gap ], sizeof( vul_blas_svd_basis ) );
         }
         memcpy( &x[ j ], &tmp, sizeof( vul_blas_svd_basis ) );
      }
   }
}

static void vul__blas_qr_decomposition_gram_schmidt( vul_blas_real *Q, vul_blas_real *R, vul_blas_real *A, 
                                                     int c, int r, int transpose )
{
   // Gram-Schmidt; numerically bad and slow, but simple.
   vul_blas_real *u, *a, d, tmp, *At;
   int i, j, k;
   
   if( transpose ) {
      i = c; c = r; r = i;
   }

   u = ( vul_blas_real* )VUL_BLAS_ALLOC( sizeof( vul_blas_real ) * c * r );
   a = ( vul_blas_real* )VUL_BLAS_ALLOC( sizeof( vul_blas_real ) * r );

   At = 0;
   if( transpose ) {
      At = ( vul_blas_real* )VUL_BLAS_ALLOC( sizeof( vul_blas_real ) * c * r );
      vulb__mtranspose( At, A, r, c );
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
            tmp = vulb__dot( &u[ k * r ], &u[ k * r ], r );
            if( tmp != 0.f ) {
               u[ i * r + j ] -= u[ k * r + j ] * ( vulb__dot( &u[ k * r ], a, r ) / tmp );
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
      VUL_BLAS_FREE( A );
   }
   VUL_BLAS_FREE( u );
   VUL_BLAS_FREE( a );
}

static void vul__blas_apply_householder_column( vul_blas_real *O, vul_blas_real *A,
                                                vul_blas_real *QO, vul_blas_real *Q,
                                                int c, int r, int qc, int qr, int k,
                                                vul_blas_real *Qt, vul_blas_real *u, int respect_signbit )
{
   int i, j, l, free_u, free_Qt;
   vul_blas_real alpha, d;

   free_u = 0; free_Qt = 0;
   if( u == NULL ) {
      u = ( vul_blas_real* )VUL_BLAS_ALLOC( sizeof( vul_blas_real ) * r );
      free_u = 1;
   }
   if( Qt == NULL ) {
      Qt = ( vul_blas_real* )VUL_BLAS_ALLOC( sizeof( vul_blas_real ) * r * r );
      free_Qt = 1;
   }
   
   // Construct u
   for( i = 0; i < ( r - k ); ++i ) {
      u[ i ] = IDX( A, ( i + k ), k, c, r );
   }
   // Calculate and add alpha
   alpha = 0.f;
   for( i = 0; i < ( r - k ); ++i ) {
      alpha += u[ i ] * u[ i ];
   }
   // k-th element dictates sign to avoid loss of significance
   if( respect_signbit ) {
      alpha = copysign( sqrt( alpha ), u[ 0 ] );
   } else {
      alpha = -sqrt( alpha );
   }
   u[ 0 ] += alpha;
   // Normalize u 
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
   memset( Qt, 0, sizeof( vul_blas_real ) * r * r );
   for( i = 0; i < r - k; ++i ) {
      for( j = 0; j < r - k; ++j ) {
         IDX( Qt, i, j, r - k, r - k ) = ( ( i == j ) ? 1.f : 0.f ) - 2.f * u[ i ] * u[ j ];
      }
   }
   // Calcualte new A into O
   memcpy( O, A, c * r * sizeof( vul_blas_real ) );
   for( i = 0; i < r - k; ++i ) {
      for( j = 0; j < c; ++j ) {
         d = 0.f;
         for( l = 0; l < r - k; ++l ) {
            d += IDX( Qt, i, l, r - k, r - k ) * IDX( A, l + k, j, c, r );
         }
         IDX( O, i + k, j, c, r ) = d;
      }
   }
   if( Q && QO ) {
      memcpy( QO, Q, qc * qr * sizeof( vul_blas_real ) );
      for( i = 0; i < qr; ++i ) {
         for( j = 0; j < qc - k; ++j ) {
            d = 0.f;
            for( l = 0; l < r - k; ++l ) {
               d += IDX( Q, i, l + k, qc, qr ) * IDX( Qt, l, j, r - k, r - k );
            }
            IDX( QO, i, j + k, qc, qr ) = d;
         }
      }
   }

   if( free_Qt ) {
      VUL_BLAS_FREE( Qt );
   }
   if( free_u ) {
      VUL_BLAS_FREE( u );
   }
}

static void vul__blas_qr_decomposition_householder( vul_blas_real *Q, vul_blas_real *R, vul_blas_real *A, 
                                                    int c, int r, int transpose )
{
   vul_blas_real *u, *Qt, *Q0, *Q1, *R0, *R1, alpha, d;
   int i, j, k, l, n;

   if( transpose ) {
      i = c; c = r; r = i;
   }

   u = ( vul_blas_real* )VUL_BLAS_ALLOC( sizeof( vul_blas_real ) * r );

   n = r > c ? r : c;
   Qt = ( vul_blas_real* )VUL_BLAS_ALLOC( sizeof( vul_blas_real ) * r * r );
   Q0 = ( vul_blas_real* )VUL_BLAS_ALLOC( sizeof( vul_blas_real ) * r * r );
   Q1 = ( vul_blas_real* )VUL_BLAS_ALLOC( sizeof( vul_blas_real ) * r * r );
   R0 = ( vul_blas_real* )VUL_BLAS_ALLOC( sizeof( vul_blas_real ) * n * n );
   R1 = ( vul_blas_real* )VUL_BLAS_ALLOC( sizeof( vul_blas_real ) * n * n );

   if( r == 0 ) return;
   if( r == 1 ) {
      Q[ 0 ] = A[ 0 ]; // Just copy it?
      R[ 0 ] = 1.f;
   }
   memset( R0, 0, sizeof( vul_blas_real ) * n * n );
   memset( R1, 0, sizeof( vul_blas_real ) * n * n );
   if( transpose ) {
      vulb__mtranspose( R0, A, r, c );
   } else {
      memcpy( R0, A, sizeof( vul_blas_real ) * r * c );
   }

   memset( Q0, 0, sizeof( vul_blas_real ) * r * r );
   for( i = 0; i < r; ++i ) {
      IDX( Q0, i, i, r, r ) = 1.f;
   }

   for( k = 0; k < r - 1; ++k ) {
      // Apply householder transformation to row k
      vul__blas_apply_householder_column( R1, R0, Q1, Q0, c, r, r, r, k, Qt, u, 1 );

      vulb__swap_ptr( &Q0, &Q1 );
      vulb__swap_ptr( &R0, &R1 );
   }
   memcpy( Q, Q0, sizeof( vul_blas_real ) * r * r );
   memcpy( R, R0, sizeof( vul_blas_real ) * r * c );

   VUL_BLAS_FREE( Q0 );
   VUL_BLAS_FREE( Q1 );
   VUL_BLAS_FREE( Qt );
   VUL_BLAS_FREE( R0 );
   VUL_BLAS_FREE( R1 );
   VUL_BLAS_FREE( u );
}

static void vul__blas_givens_rotate( vul_blas_real *A, int c, int r, 
                                     int i, int j, float cosine, float sine,
                                     int post_multiply )
{
   int k;
   vul_blas_real G[ 4 ], v0, v1, a, b;

   G[ 0 ] = cosine;
   G[ 1 ] = sine;
   G[ 2 ] = -sine;
   G[ 3 ] = cosine;
   if( post_multiply ) {
      // Apply it to the two affected rows, that is calculate the below for those rows that change
      // R = G^T * R
      for( k = 0; k < c; ++k ) {
         a = i < r ? IDX( A, i, k, c, r ) : 0.0;
         b = j < r ? IDX( A, j, k, c, r ) : 0.0;
         v0 = G[ 0 ] * a + G[ 2 ] * b; // G[ 2 ]: T is transposed
         v1 = G[ 1 ] * a + G[ 3 ] * b; // G[ 1 ]: T is transposed
         if( i < r ) IDX( A, i, k, c, r ) = v0;
         if( j < r ) IDX( A, j, k, c, r ) = v1;
      }
   } else {
      // Calculate new Q = Q * G
      for( k = 0; k < r; ++k ) {
         a = i < c ? IDX( A, k, i, c, r ) : 0.0;
         b = j < c ? IDX( A, k, j, c, r ) : 0.0;
         v0 = G[ 0 ] * a + G[ 2 ] * b; // G[ 2 ]: T is transposed
         v1 = G[ 1 ] * a + G[ 3 ] * b; // G[ 1 ]: T is transposed
         if( i < c ) IDX( A, k, i, c, r ) = v0;
         if( j < c ) IDX( A, k, j, c, r ) = v1;
      }
   }
}

static void vul__blas_qr_decomposition_givens( vul_blas_real *Q, vul_blas_real *R, vul_blas_real *A, 
                                               int c, int r, int transpose )
{
   int i, j, k;
   vul_blas_real theta, st, ct, G[ 4 ], v0, v1, *RA;
   
   RA = ( vul_blas_real* )VUL_BLAS_ALLOC( sizeof( vul_blas_real ) * r * c );

   memset( RA, 0, sizeof( vul_blas_real ) * r * c );
   if( transpose ) {
      i = r; r = c; c = i;
      vulb__mtranspose( RA, A, r, c );
   } else {
      memcpy( RA, A, sizeof( vul_blas_real ) * r * c );
   }
   for( i = 0; i < r; ++i ) {
      for( j = 0; j < r; ++j ) {
         IDX( Q, i, j, r, r ) = ( i == j ) ? 1.f : 0.f;
      }
   }
   
   for( j = 0; j < c; ++j ) {
      for( i = r - 2; i >= j; --i ) {
         v0 = IDX( RA, i, j, c, r );
         v1 = IDX( RA, i + 1, j, c, r );
         theta = v0 * v0 + v1 * v1;
         if( theta != 0.f ) {
            theta = sqrt( theta );
            ct =  v0 / theta;
            st = -v1 / theta;
         } else {
            st = 0.f;
            ct = 1.f;
         }
         // Create givens rotation matrix
         vul__blas_givens_rotate( RA, c, r, i, i + 1, ct, st, 1 );
         vul__blas_givens_rotate( Q, r, r, i, i + 1, ct, st, 0 );
      }
   }
   memcpy( R, RA, c * r * sizeof( vul_blas_real ) );

   VUL_BLAS_FREE( RA );
}

static vul_blas_real vul__blas_matrix_norm_diagonal( vul_blas_real *A, int c, int r )
{
   vul_blas_real v;
   int i, n;
   
   v = 0.f;
   n = c < r ? c : r;
   for( i = 0; i < n; ++i ) {
      v += IDX( A, i, i, c, r ) * IDX( A, i, i, c, r );
   }
   return sqrt( v );
}

static vul_blas_real vul__blas_matrix_norm_as_single_column( vul_blas_real *A, int c, int r, int upper_diag )
{
   vul_blas_real v;
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

vul_blas_real vul_blas_largest_eigenvalue( vul_blas_real *A, int c, int r, vul_blas_real eps, int max_iter )
{
   int iter, axis, normaxis, i, j;
   vul_blas_real *v, *y, lambda, norm, err;

   // Power method. Slow, but simple
   v = ( vul_blas_real* )VUL_BLAS_ALLOC( sizeof( vul_blas_real ) * r );
   y = ( vul_blas_real* )VUL_BLAS_ALLOC( sizeof( vul_blas_real ) * r );
   memset( v, 0, sizeof( vul_blas_real ) * r );
   v[ 0 ] = 1.f;

   err = eps * 2;
   iter = 0;
   axis = 0;
   lambda = 0.f;
   while( err > eps && iter++ < max_iter ) {
      vulb__mmul( y, A, v, c, r );
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

   VUL_BLAS_FREE( v );
   VUL_BLAS_FREE( y );

   return lambda;
}

void vul_blas_svd_dense_qrlq( vul_blas_svd_basis *out, int *rank,
                              vul_blas_real *A,
                              int c, int r, vul_blas_real eps, int itermax )
{
   vul_blas_real *U0, *U1, *V0, *V1, *S0, *S1, *Sb, *Q, err, e, f, scale;
   int iter, n, i, j, k, ri, ci;

   n = r > c ? r : c;
   U0 = ( vul_blas_real* )VUL_BLAS_ALLOC( sizeof( vul_blas_real ) * r * r );
   U1 = ( vul_blas_real* )VUL_BLAS_ALLOC( sizeof( vul_blas_real ) * r * r );
   V0 = ( vul_blas_real* )VUL_BLAS_ALLOC( sizeof( vul_blas_real ) * c * c );
   V1 = ( vul_blas_real* )VUL_BLAS_ALLOC( sizeof( vul_blas_real ) * c * c );
   S0 = ( vul_blas_real* )VUL_BLAS_ALLOC( sizeof( vul_blas_real ) * r * c );
   S1 = ( vul_blas_real* )VUL_BLAS_ALLOC( sizeof( vul_blas_real ) * r * c );
   Sb = ( vul_blas_real* )VUL_BLAS_ALLOC( sizeof( vul_blas_real ) * r * c );
   Q = ( vul_blas_real* )VUL_BLAS_ALLOC( sizeof( vul_blas_real ) * n * n );
   memset( U0, 0, sizeof( vul_blas_real ) * r * r );
   memset( U1, 0, sizeof( vul_blas_real ) * r * r );
   memset( V0, 0, sizeof( vul_blas_real ) * c * c );
   memset( V1, 0, sizeof( vul_blas_real ) * c * c );
   memset( S0, 0, sizeof( vul_blas_real ) * r * c );
   memset( S1, 0, sizeof( vul_blas_real ) * r * c );
   memset( Sb, 0, sizeof( vul_blas_real ) * r * c );
   memset( Q, 0, sizeof( vul_blas_real ) * n * n );
   iter = 0;
   err = FLT_MAX;

   // Initialize S0 to A^T
   vulb__mtranspose( S0, A, c, r );
   // Scale by inverse of largest coefficient (and multiply it back
   // into the singular values) for numerical stability.
   scale = -FLT_MAX;
   for( i = 0; i < r; ++i ) {
      for( j = 0; j < c; ++j ) {
         scale = fabs( IDX( S0, i, j, c, r ) ) > scale 
               ? fabs( IDX( S0, i, j, c, r ) ) : scale;
      }
   }
   f = 1.0 / scale;
   for( i = 0; i < r; ++i ) {
      for( j = 0; j < c; ++j ) {
         IDX( S0, i, j, c, r ) *= f;
      }
   }

   // Initialize U and V as identity matrix
   for( i = 0; i < r; ++i ) {
      IDX( U0, i, i, r, r ) = 1.f;
   }
   for( i = 0; i < c; ++i ) {
      IDX( V0, i, i, c, c ) = 1.f;
   }
   while( err > eps && iter++ < itermax ) {
      // Store away last S
      memcpy( Sb, S0, r * c * sizeof( vul_blas_real ) );

      // Decompose
      vul__blas_qr_decomposition_givens( Q, S1, S0, r, c, 1 );
      vulb__mmul_matrix( U1, U0, Q, r );
      vul__blas_qr_decomposition_givens( Q, S0, S1, c, r, 1 );
      vulb__mmul_matrix( V1, V0, Q, c );

      vulb__swap_ptr( &U0, &U1 );
      vulb__swap_ptr( &V0, &V1 );

      // Calculate error
      e = vul__blas_matrix_norm_as_single_column( S0, r, c, 1 );
      f = vul__blas_matrix_norm_diagonal( S0, r, c );
      if( f == 0.f ) {
         f = 1.f;
      }
      // Break if error increases
      if( ( e / f ) > err ) {
         vulb__swap_ptr( &Sb, &S0 );
         vulb__swap_ptr( &U0, &U1 );
         vulb__swap_ptr( &V0, &V1 );
         break;
      }
      err = e / f;
   }

   // Grap sigmas and rank, sort decreasing
   k = r < c ? r : c;
   for( j = 0, i = 0; i < k; ++i ) { // Since we're transposed, S is indexed with r and not c
      out[ i ].sigma = fabs( IDX( S0, i, i, r, c ) ) * scale;
      out[ i ].axis = i;
      if( out[ i ].sigma > eps ) {
         ++j;
      }
   }
   if( *rank == 0 || j < *rank ) {
      *rank = j;
   }
   vul__blas_svd_sort( out, k );

   // Fix signs and copy U
   for( i = 0; i < *rank; ++i ) {
      out[ i ].u_length = r;
      out[ i ].v_length = c;
      out[ i ].u = ( vul_blas_real* )VUL_BLAS_ALLOC( sizeof( vul_blas_real ) * r );
      out[ i ].v = ( vul_blas_real* )VUL_BLAS_ALLOC( sizeof( vul_blas_real ) * c );
      f = IDX( S0, out[ i ].axis, out[ i ].axis , r, c ) < 0.f ? -1.f : 1.f;
      for( j = 0; j < r; ++j ) {
         out[ i ].u[ j ] = IDX( U0, j, out[ i ].axis, r, r ) * f;
      }
      for( j = 0; j < c; ++j ) {
         out[ i ].v[ j ] = IDX( V0, j, out[ i ].axis, c, c );
      }
   }

   VUL_BLAS_FREE( U0 );
   VUL_BLAS_FREE( U1 );
   VUL_BLAS_FREE( V0 );
   VUL_BLAS_FREE( V1 );
   VUL_BLAS_FREE( S0 );
   VUL_BLAS_FREE( S1 );
   VUL_BLAS_FREE( Sb );
   VUL_BLAS_FREE( Q );
}

void vul_blas_svd_dense( vul_blas_svd_basis *out, int *rank,
                         vul_blas_real *A,
                         int c, int r, vul_blas_real eps, int itermax )
{
   vul_blas_real *J, *U, *V, *G, *omegas, f, scale;
   int iter, n, m, i, j, k, ri, ci, nonzero;

   n = r > c ? r : c;
   m = r < c ? r : c;
   U = ( vul_blas_real* )VUL_BLAS_ALLOC( sizeof( vul_blas_real ) * r * r );
   V = ( vul_blas_real* )VUL_BLAS_ALLOC( sizeof( vul_blas_real ) * c * c );
   G = ( vul_blas_real* )VUL_BLAS_ALLOC( sizeof( vul_blas_real ) * r * c );
   omegas = ( vul_blas_real* )VUL_BLAS_ALLOC( sizeof( vul_blas_real ) * r );
   memset( U, 0, sizeof( vul_blas_real ) * r * r );
   memset( V, 0, sizeof( vul_blas_real ) * c * c );
   memset( omegas, 0, sizeof( vul_blas_real ) * r );
   iter = 0;
   nonzero = c;

   // Initialize S to A / max_coefficient_in_A for numerical stability
   scale = -FLT_MAX;
   for( i = 0; i < r; ++i ) {
      for( j = 0; j < c; ++j ) {
         scale = fabs( IDX( A, i, j, c, r ) ) > scale 
               ? fabs( IDX( A, i, j, c, r ) ) : scale;
      }
   }
   f = 1.0 / scale;
   for( i = 0; i < r; ++i ) {
      for( j = 0; j < c; ++j ) {
         IDX( G, i, j, c, r ) = IDX( A, i, j, c, r ) * f;
      }
   }
   
   // Initialize U and V as identity matrices
   for( i = 0; i < r; ++i ) {
      IDX( U, i, i, r, r ) = 1.f;
   }
   for( i = 0; i < c; ++i ) {
      IDX( V, i, i, c, c ) = 1.f;
   }

   while( nonzero && iter++ < itermax ) {
      nonzero = 0;
      for( i = 0; i < r - 1; ++i ) {
         for( j = i + 1; j < r; ++j ) {
            vul_blas_real aii, aij, ajj;
            
            // Compute a_ij, a_ji, a_ii
            aii = 0.0; aij = 0.0; ajj = 0.0;
            for( k = 0; k < c; ++k ) {
               aii += IDX( G, i, k, c, r ) * IDX( G, i, k, c, r );
               ajj += IDX( G, j, k, c, r ) * IDX( G, j, k, c, r );
               aij += IDX( G, i, k, c, r ) * IDX( G, j, k, c, r );
            }
            if( fabs( aij ) > eps ) {
               nonzero += 1;
               vul_blas_real tau, t, ct, st;
               tau = ( aii - ajj ) / ( 2.0 * aij );
               t = copysign( 1.0 / ( fabs( tau ) + sqrt( 1.0 + tau * tau ) ), tau );
               ct = 1.0 / sqrt( 1.0 + t * t );
               st = ct * t;
               vul__blas_givens_rotate( G, c, r, j, i, ct, st, 1 );
               vul__blas_givens_rotate( U, r, r, j, i, ct, st, 0 );
            }
         }
      }
   }

   // Calculate the singular values (2-norm of the columns of G)
   for( i = 0; i < r; ++i ) {
      vul_blas_real t = 0.0;
      for( j = 0; j < c; ++j ) {
         t += IDX( G, i, j, c, r ) * IDX( G, i, j, c, r );
      }
      omegas[ i ] = sqrt( t );
   }

   // Calculate V
   for( i = 0; i < c; ++i ) { // The rest is zero
      if( i < r && fabs( omegas[ i ] ) > eps ) { // Ignore zero singular values
         for( j = 0; j < c; ++j ) {
            IDX( V, j, i, c, c ) = IDX( G, i, j, c, r ) / omegas[ i ];
         }
      }
   }

   // Grap sigmas and rank, sort decreasing
   k = r < c ? r : c;
   for( j = 0, i = 0; i < k; ++i ) {
      out[ i ].sigma = fabs( omegas[ i ] ) * scale; // Multiply in the maximal coefficient (scale) again.
      out[ i ].axis = i;
      if( out[ i ].sigma > eps ) {
         ++j;
      }
   }
   if( *rank == 0 || j < *rank ) {
      *rank = j;
   }
   vul__blas_svd_sort( out, k );

   // Fix signs and copy U
   for( i = 0; i < *rank; ++i ) {
      out[ i ].u_length = r;
      out[ i ].v_length = c;
      out[ i ].u = ( vul_blas_real* )VUL_BLAS_ALLOC( sizeof( vul_blas_real ) * r );
      out[ i ].v = ( vul_blas_real* )VUL_BLAS_ALLOC( sizeof( vul_blas_real ) * c );
      f = omegas[ out[ i ].axis ] < 0.f ? -1.f : 1.f;
      for( j = 0; j < r; ++j ) {
         out[ i ].u[ j ] = IDX( U, j, out[ i ].axis, r, r ) * f;
      }
      for( j = 0; j < c; ++j ) {
         out[ i ].v[ j ] = IDX( V, j, out[ i ].axis, c, c );
      }
   }

   VUL_BLAS_FREE( U );
   VUL_BLAS_FREE( V );
   VUL_BLAS_FREE( G );
   VUL_BLAS_FREE( omegas );
}

void vul_blas_linear_least_squares_dense( vul_blas_real *out,
                                           vul_blas_real *A,
                                           vul_blas_real *b,
                                           int c, int r,
                                           int max_iterations,
                                           vul_blas_real tolerance )
{
   vul_blas_svd_basis *res;
   vul_blas_real *U, *V, *S, *d, v;
   int rank, i, j, m;

   res = ( vul_blas_svd_basis* )VUL_BLAS_ALLOC( sizeof( vul_blas_svd_basis ) * c );
   rank = 0;
   vul_blas_svd_dense( res, &rank, A, c, r, tolerance, max_iterations );
   
   U = ( vul_blas_real* )VUL_BLAS_ALLOC( sizeof( vul_blas_real ) * r * r );
   V = ( vul_blas_real* )VUL_BLAS_ALLOC( sizeof( vul_blas_real ) * c * c );
   S = ( vul_blas_real* )VUL_BLAS_ALLOC( sizeof( vul_blas_real ) * c );
   d = ( vul_blas_real* )VUL_BLAS_ALLOC( sizeof( vul_blas_real ) * r );

   // Reconstruct U
   memset( U, 0, sizeof( vul_blas_real ) * r * r );
   for( i = 0; i < rank; ++i ) {
      for( j = 0; j < res[ i ].u_length; ++j ) {
         IDX( U, j, res[ i ].axis, r, r ) = res[ i ].u[ j ];
      }
   }
   // Reconstruct V
   memset( V, 0, sizeof( vul_blas_real ) * c * c );
   for( i = 0; i < rank; ++i ) {
      for( j = 0; j < res[ i ].v_length; ++j ) {
         IDX( V, res[ i ].axis, j, c, c ) = res[ i ].v[ j ];
      }
   }
   // Reconstruct the diagonal of S
   memset( S, 0, sizeof( vul_blas_real ) * c );
   for( i = 0; i < rank; ++i ) {
      S[ res[ i ].axis ] = res[ i ].sigma;
   }
   // Free the basis'
   vul_blas_svd_basis_destroy( res, rank );
   VUL_BLAS_FREE( res );
   
   // Calculate d = U^T * b
   memset( d, 0, sizeof( vul_blas_real ) * r );
   for( i = 0; i < r; ++i ) {
      v = 0.0;
      for( j = 0; j < r; ++j ) {
         v += IDX( U, j, i, r, r ) * b[ j ]; // Transposed
      }
      d[ i ] = v;
   }

   // Calculate x = V * S^+ * d
   m = r < c ? r : c;
   memset( out, 0, sizeof( vul_blas_real ) * m );
   for( i = 0; i < m; ++i ) {
      v = 0.0;
      for( j = 0; j < rank; ++j ) {
         v += IDX( V, j, i, c, c ) * ( d[ j ] / S[ j ] );
      }
      out[ i ] = v;
   }

   VUL_BLAS_FREE( U );
   VUL_BLAS_FREE( V );
   VUL_BLAS_FREE( S );
   VUL_BLAS_FREE( d );
}

#undef IDX

#ifdef _cplusplus
}
#endif

#undef vul_blas_real

#endif
