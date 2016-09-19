/*
 * Villains' Utility Library - Thomas Martin Schmid, 2016. Public domain¹
 *
 * This file contains a number of linear system solvers and functions
 * to perform singular value decomposition:
 *
 * > The following solvers of the linear system Ax=b:
 *    -Iterative:
 *      -Generalized Minimal Residual method
 *      -Conjugate gradient method
 *      -Successive over-relaxation
 *    -Decompositions (iterative refinement):
 *      -QR decomposition
 *      -Cholesky decomposition
 *      -LU decomposition
 * > For iterative solvers, the following preconditioners:
 *    -Jacobi (diagonal)
 *    -Incomplete cholesky
 *    -Incomplete LU(0)
 * > The following SVD methods:
 *    -One-sided Jacobi orthogonalization
 *    -Repeated, alternating QR and LQ decomposition (SLOW and less accurate, but simple)
 * > A Generalized Linear Least Square solver that uses SVD
 * > A function that finds the largest eigenvalue of a matrix (using the power method).
 *
 * All features are supplied both for dense matrices and sparse matrices, except
 * preconditioners, which are only supported for sparse matrices. The library
 * uses a row-major List-of-Lists format for sparse matrices. Complex numbers are not
 * supported!
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
 * Define VUL_LINALG_ROW_MAJOR to use row major dense matrices, otherwise column major
 * is assumed.
 * 
 * If you wish to use a custom allocator for temporaries, define VUL_LINALG_ALLOC
 * and VUL_LINALG_FREE to functions with signatures similar to malloc and free;
 * if no alternative is given malloc and free are used.
 *
 * All solvers leave input intact; only the "out"-argument is altered.
 *
 * Define VUL_DEFINE in exactly _one_ compilation unit to define the implementation.
 *
 * Error reporting: When something goes wrong (divide-by-zeroes, singular matrices encountered
 * etc.) these can be handled in multiple ways. Define any of these to use it (no default):
 *  - VUL_LINALG_ERROR_STDERR: Print an error message to stderr.
 *  - VUL_LINALG_ERROR_STR: Last error message is written to the global vul_linalg_last_error.
 *  - VUL_LINALG_ERROR_ASSERT: Trigger an assert on error.
 *  - VUL_LINALG_ERROR_QUIET: Do no reporting at all, simple fail.
 *  - VUL_LINALG_ERROR_CUSTOM: Define your own macro VUL_LINALG_ERROR_CUSTOM( ... )
 *    and do whatever you want with the error message (printf style formatting/arguments)
 *    before failing.
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
#ifndef VUL_LINALG_H
#define VUL_LINALG_H

#include <math.h>
#include <assert.h>
#include <float.h>
#include <string.h>

#ifndef VUL_LINALG_ALLOC
#include <stdlib.h>
#define VUL_LINALG_ALLOC malloc
#define VUL_LINALG_FREE free
#else
   #ifndef VUL_LINALG_FREE
      vul_linalg.h: You must also specify a deallocation function to go with VUL_LINALG_ALLOC
   #endif
#endif

#ifdef VUL_LINALG_DOUBLE
#define vul_linalg_real double
#else
#define vul_linalg_real float
#endif

#ifdef _cplusplus
extern "C" {
#endif

//--------------------
// Sparse datatypes
//

typedef struct vul_linalg_sparse_entry {
   unsigned int idx;
   vul_linalg_real val;
} vul_linalg_sparse_entry;

typedef struct vul_linalg_vector {
   vul_linalg_sparse_entry *entries;
   unsigned int count;
} vul_linalg_vector;

typedef struct vul_linalg_matrix_row {
   unsigned int idx;
   vul_linalg_vector vec;
} vul_linalg_matrix_row;

typedef struct vul_linalg_matrix {
   vul_linalg_matrix_row *rows;
   unsigned int count;
} vul_linalg_matrix;

//---------------------
// Preconditioners
//

typedef enum vul_linalg_precoditioner_type {
   VUL_LINALG_PRECONDITIONER_NONE,
   VUL_LINALG_PRECONDITIONER_JACOBI,
   VUL_LINALG_PRECONDITIONER_INCOMPLETE_CHOLESKY,
   VUL_LINALG_PRECONDITIONER_INCOMPLETE_LU_0
} vul_linalg_precoditioner_type;

//----------------------------------
// Sparse datatype public functions
//

/*
 * Creates a sparse vector. Takes a list of indices and values to fill it with,
 * or two null pointers to initialize empty.
 */
vul_linalg_vector *vul_linalg_vector_create( unsigned *idxs, vul_linalg_real *vals, unsigned int n );

/*
 * Create a sparse matrix. Takes a list of coordinates and values to fill it with,
 * or three null-pointers to initialize empty.
 */
vul_linalg_matrix *vul_linalg_matrix_create( unsigned int *rows, unsigned int *cols, 
                                             vul_linalg_real *vals, unsigned int init_count );

/* 
 * Destroys a sparse matrix
 */
void vul_linalg_matrix_destroy( vul_linalg_matrix *m );

/* 
 * Overwriting a non-zero value with zero does not reclaim the space!
 */
void vul_linalg_vector_insert( vul_linalg_vector *v, unsigned int idx, vul_linalg_real val );

/*
 * When overwriting a non-zero value with zero, the space is not reclaimed automaticall,
 * so the matrix is not "fully sparse". When rotating/transforming with householder or givens
 * rotations this can lead to full rows of zeroes. Use vulb__mclean on the matrix to reclaim all
 * space lost to zero-over-non-zero insertions.
 */
void vul_linalg_matrix_insert( vul_linalg_matrix *m, unsigned int r, unsigned int c, vul_linalg_real v );

/*
 * Retrieve an element from a sparse vector.
 */
vul_linalg_real vul_linalg_vector_get( vul_linalg_vector *v, unsigned int idx );

/* 
 * Retrieve and element from a sparse matrix.
 */
vul_linalg_real vul_linalg_matrix_get( vul_linalg_matrix *m, unsigned int r, unsigned int c );

/*
 * Destroy a sparse vector
 */
void vul_linalg_vector_destroy( vul_linalg_vector *v );

//--------------------------
// Sparse preconditioners
//

/*
 * Returns the preconditioner matrix for an incomplete sparse LU(0) decomposition.
 * In this incomplete decomposition, only the entries in LU that are non-zero in
 * A are used. Both L and U are returned in a single matrix.
 */
vul_linalg_matrix *vul_linalg_precondition_ilu0( vul_linalg_matrix *A, int c, int r );

/*
 * Returns the preconditioner matrix for an incomplete cholesky decomposition.
 * Only the entries in L that are non-zero in A are used. Only L is returned, L* is
 * constructed on the fly in the solver (L is real, so it's just a transposed index).
 */
vul_linalg_matrix *vul_linalg_precondition_ichol( vul_linalg_matrix *A, int c, int r );

/*
 * Returns the preconditioner matrix for a Jacobi decomposition of A. For faster inversion
 * in the solving step, the inverse of the diagonal entries of A is returned.
 */
vul_linalg_matrix *vul_linalg_precondition_jacobi( vul_linalg_matrix *A, int c, int r );

//--------------------
// Sparse solvers

/*
 * Iterative solver of the linear system Ax = b
 * Uses the Conjugate Gradient Method, and works for matrices that are
 * POSITIVE-DEFINITE and SYMMETRIC.
 *
 * Runs for at most max_iterations, or until the average square error is
 * below the given tolerance.
 *
 * An optional preconditioner can be supplied. If none is wanted, select preconditioner
 * type VUL_LINALG_PRECONDITIONER_NONE, and set P to NULL. Otherwise set P to a
 * precalculated preconditioner matrix (see the vul_linalg_precondition_* functions).
 * The preconditioner is applied on the left.
 */
vul_linalg_vector *vul_linalg_conjugate_gradient_sparse( vul_linalg_matrix *A,
                                                         vul_linalg_vector *initial_guess,
                                                         vul_linalg_vector *b,
                                                         vul_linalg_matrix *P,
                                                         vul_linalg_precoditioner_type ptype,
                                                         int max_iterations,
                                                         vul_linalg_real tolerance );
/*
 * Iterative solver of the linear system Ax = b
 * Uses the Generalized Minimal Residual Method. 
 * 
 * Runs for at most max_iterations, or until the ratio of the average square
 * error vs. the norm of b is below a given tolerance. Restarts the 
 * orthonormal basis construction every restart_interval iterations.
 *
 * An optional preconditioner can be supplied. If none is wanted, select preconditioner
 * type VUL_LINALG_PRECONDITIONER_NONE, and set P to null. Otherwise set P to a
 * precalculated preconditioner matrix (see the vul_linalg_precondition_* functions).
 * If a Jacobi preconditioner is used, it is applied on the right. If an incomplete
 * decomposition is used, the lower diagonal matrix is applied on the left, and the
 * upper diagonal matrix is applied on the right. @NOTE(thynn): This gave the best results
 * in testing, but feel free to let me know why this is fundamentally wrong and I should be
 * ashamed of doing it this way.
 */
vul_linalg_vector *vul_linalg_gmres_sparse( vul_linalg_matrix *A,
                                            vul_linalg_vector *initial_guess,
                                            vul_linalg_vector *b,
                                            vul_linalg_matrix *P,
                                            vul_linalg_precoditioner_type ptype,
                                            int restart_interval,
                                            int max_iterations,
                                            vul_linalg_real tolerance );

/*
 * Iterative solver of the linear system Ax = b
 * Uses the Successive Over-Relaxation method. May converge for any matrix,
 * but may also not converge at all.
 *
 * Runs for at most max_iterations, or until the average square error is
 * below the given tolerance.
 */
vul_linalg_vector *vul_linalg_successive_over_relaxation_sparse( vul_linalg_matrix *A,
                                                                 vul_linalg_vector *initial_guess,
                                                                 vul_linalg_vector *b,
                                                                 vul_linalg_real relaxation_factor,
                                                                 int max_iterations,
                                                                 vul_linalg_real tolerance );

/*
 * LU Decomposition step. Supply with a matrix that is NOT SINGULAR,
 * and it returns a decomposition into a lower triangular matrix L
 * and an upper triangular matrix U, both stored in the output matrix
 * LU. The argument indices is also an output argument, and is required
 * due to pivoting of the decomposed matrix.
 *
 * The decomposition can then be given to vul_linalg_lu_solve_sparse
 * with A and b to solve Ax = b.
 */
void vul_linalg_lu_decomposition_sparse( vul_linalg_matrix **LU,
                                         int *indices,
                                         vul_linalg_matrix *A,
                                         int cols, int rows );
/*
 * Iterative solver of the linear system Ax = b given a LL* decomposition
 * of the matrix A. Use vul_linalg_cholesky_decomposition_sparse to create
 * this decomposition.
 *
 * This method employs at most max_iterations of iterative refinement, or until
 * the average square error is below tolerance.
 */
vul_linalg_vector *vul_linalg_lu_solve_sparse( vul_linalg_matrix *LU,
                                               int *indices,
                                               vul_linalg_matrix *A,
                                               vul_linalg_vector *initial_guess,
                                               vul_linalg_vector *b,
                                               int cols, int rows,
                                               int max_iterations,
                                               vul_linalg_real tolerance );

/*
 * Cholesky Decomposition step. Supply a matrix A that is
 * HERMITIAN and POSITIVE-DEFINITE, and this returns the LL*
 * decomposition of the matrix in the output matrices L and LT.
 */
void vul_linalg_cholesky_decomposition_sparse( vul_linalg_matrix **L,
                                               vul_linalg_matrix **LT,
                                               vul_linalg_matrix *A,
                                               int cols, int rows );
/*
 * Iterative solver of the linear system Ax = b given a LL* decomposition
 * of the matrix A. Use vul_linalg_cholesky_decomposition_sparse to create
 * this decomposition.
 *
 * This method employs at most max_iterations of iterative refinement, or until
 * the average square error is below tolerance.
 */
vul_linalg_vector *vul_linalg_cholesky_solve_sparse( vul_linalg_matrix *L,
                                                     vul_linalg_matrix *LT,
                                                     vul_linalg_matrix *A,
                                                     vul_linalg_vector *initial_guess,
                                                     vul_linalg_vector *b,
                                                     int cols, int rows,
                                                     int max_iterations,
                                                     vul_linalg_real tolerance );
/*
 * QR decomposition step. Supply a matric A and this returns the Q^T and R
 * decomposition into their respective matrix pointers.
 */
void vul_linalg_qr_decomposition_sparse( vul_linalg_matrix **Q,
                                         vul_linalg_matrix **R,
                                         vul_linalg_matrix *A,
                                         int cols, int rows );

/*
 * Iterative solver of the linear system Ax = b given a Q^TR decomposition
 * of the matrix A. Use vul_linalg_qr_decomposition_sparse to create
 * this decomposition.
 *
 * This method employs at most max_iterations of iterative refinement, or until
 * the average square error is below tolerance.
 */
vul_linalg_vector *vul_linalg_qr_solve_sparse( vul_linalg_matrix *Q,
                                               vul_linalg_matrix *R,
                                               vul_linalg_matrix *A,
                                               vul_linalg_vector *initial_guess,
                                               vul_linalg_vector *b,
                                               int cols, int rows,
                                               int max_iterations,
                                               vul_linalg_real tolerance );

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
void vul_linalg_conjugate_gradient_dense( vul_linalg_real *out,
                                          vul_linalg_real *A,
                                          vul_linalg_real *initial_guess,
                                          vul_linalg_real *b,
                                          int n,
                                          int max_iterations,
                                          vul_linalg_real tolerance );
/*
 * Iterative solver of the linear system Ax = b
 * Uses the Generalized Minimal Residual Method.
 *
 * Runs for at most max_iterations, or until the ratio of the average square
 * error vs. the norm of b is below a given tolerance. Restarts the 
 * orthonormal basis construction every restart_interval iterations.
 */
void vul_linalg_gmres_dense( vul_linalg_real *x,
                             vul_linalg_real *A,
                             vul_linalg_real *initial_guess,
                             vul_linalg_real *b,
                             int n,
                             int restart_interval,
                             int max_iterations,
                             vul_linalg_real tolerance );
/*
 * Iterative solver of the linear system Ax = b
 * Uses the Successive Over-Relaxation method. May converge for any matrix,
 * but may also not converge at all.
 *
 * Runs for at most max_iterations, or until the average square error is
 * below the given tolerance.
 */
void vul_linalg_successive_over_relaxation_dense( vul_linalg_real *out,
                                                  vul_linalg_real *A,
                                                  vul_linalg_real *initial_guess,
                                                  vul_linalg_real *b,
                                                  vul_linalg_real relaxation_factor,
                                                  int n,
                                                  int max_iterations,
                                                  vul_linalg_real tolerance );

/*
 * LU Decomposition step. Supply with a square matrix that is NOT SINGULAR,
 * and it returns a decomposition into a lower triangular matrix L
 * and an upper triangular matrix U, both stored in the output matrix
 * LU. The argument indices is also an output argument, and is required
 * due to pivoting of the decomposed matrix.
 *
 * The decomposition can then be given to vul_linalg_lu_solve_dense
 * with A and b to solve AX=b.
 */
void vul_linalg_lu_decomposition_dense( vul_linalg_real *LU,
                                        int *indices,
                                        vul_linalg_real *A,
                                        int n );

/*
 * Iterative solver of the linear system Ax = b given a LU decomposition and
 * pivoting indices of A. Use vul_linalg_lu_decomposition_dense to create this 
 * decomposition/indices pair.
 *
 * This method employs at most max_iterations of iterative refinement, or until
 * the average square error is below tolerance.
 */
void vul_linalg_lu_solve_dense( vul_linalg_real *out,
                                vul_linalg_real *LU,
                                int *indices,
                                vul_linalg_real *A,
                                vul_linalg_real *initial_guess,
                                vul_linalg_real *b,
                                int n,
                                int max_iterations,
                                vul_linalg_real tolerance );
/*
 * Cholesky Decomposition step. Supply a square matrix that is
 * HERMITIAN and POSITIVE-DEFINITE, and this returns the LL*
 * decomposition of the matrix (stored in the LL output argument, a
 * preallocated matrix of size n x n).
 */
void vul_linalg_cholesky_decomposition_dense( vul_linalg_real *LL,
                                              vul_linalg_real *A,
                                              int n );
/*
 * Iterative solver of the linear system Ax = b given a LL* decomposition
 * of the matrix A. Use vul_linalg_cholesky_decomposition_dense to create
 * this decomposition.
 *
 * This method employs at most max_iterations of iterative refinement, or until
 * the average square error is below tolerance.
 */
void vul_linalg_cholesky_solve_dense( vul_linalg_real *out,
                                      vul_linalg_real *LL,
                                      vul_linalg_real *A,
                                      vul_linalg_real *initial_guess,
                                      vul_linalg_real *b,
                                      int n,
                                      int max_iterations,
                                      vul_linalg_real tolerance );
/*
 * QR decomposition step. Supply a square matrix to create the 
 * QR decomposition in the preallocated matrices Q and R (output).
 *
 * Uses Givens rotations.
 */
void vul_linalg_qr_decomposition_dense( vul_linalg_real *Q,
                                        vul_linalg_real *R,
                                        vul_linalg_real *A,
                                        int n );

/*
 * Iterative solver of the linear system Ax = b
 * Uses the precomputd QR Decomposition in matrices Q and R.
 * Use vul_linalg_qr_decomposition_dense to calculate these.
 *
 * This method employs at most max_iterations of iterative refinement, or until
 * the average square error is below tolerance.
 */
void vul_linalg_qr_solve_dense( vul_linalg_real *out,
                                vul_linalg_real *Q,
                                vul_linalg_real *R,
                                vul_linalg_real *A,
                                vul_linalg_real *initial_guess,
                                vul_linalg_real *b,
                                int n,
                                int max_iterations,
                                vul_linalg_real tolerance );

//---------------------------------------
// Dense Singular Value Decomposition

typedef struct vul_linalg_svd_basis {
   vul_linalg_real sigma;
   vul_linalg_real *u, *v;
   unsigned int u_length, v_length;
   unsigned int axis;
} vul_linalg_svd_basis;

/*
 * Reconstruct the matrix M = U S V* from the bases returned from the dense svd functions.
 * Takes a variable number n of bases x.
 */
void vul_linalg_svd_basis_reconstruct_matrix( vul_linalg_real *M, vul_linalg_svd_basis *x, int n );
/*
 * Destroy n svd bases as returned from the svd functions.
 */
void vul_linalg_svd_basis_destroy( vul_linalg_svd_basis *x, int n );

/*
 * Computes the singular value decomposition of A.
 * If rank is set, the maximum of non-zero singular values and rank
 * values and basis vectors are returned.
 * The out array must have room for min(c,r) entries even if less are desired,
 * as the entries are used to sort and select singular values.
 *
 * This function uses Jacobi orthogonalization.
 */
void vul_linalg_svd_dense( vul_linalg_svd_basis *out, int *rank,
                           vul_linalg_real *A,
                           int c, int r, int itermax, vul_linalg_real eps );

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
void vul_linalg_svd_dense_qrlq( vul_linalg_svd_basis *out, int *rank,
                                vul_linalg_real *A,
                                int c, int r, int itermax, vul_linalg_real eps );

/*
 * Solves the generalized linear least squares problem defined by the
 * given singular value decomposition of A, and b.
 */
void vul_linalg_linear_least_squares_dense( vul_linalg_real *x,
                                            vul_linalg_svd_basis *bases,
                                            int rank,
                                            vul_linalg_real *b );

//---------------------------------------
// Sparse Singular Value Decomposition

typedef struct vul_linalg_svd_basis_sparse {
   vul_linalg_real sigma;
   vul_linalg_vector *u, *v;
   unsigned int u_length, v_length;
   unsigned int axis;
} vul_linalg_svd_basis_sparse;

/*
 * Computes the singular value decomposition of A.
 * If rank is set, the maximum of non-zero singular values and rank
 * values and basis vectors are returned.
 * The out array must have room for min(c,r) entries even if less are desired,
 * as the entries are used to sort and select singular values.
 *
 * This function uses Jacobi orthogonalization.
 */
void vul_linalg_svd_sparse( vul_linalg_svd_basis_sparse *out, int *rank,
                            vul_linalg_matrix *A,
                            int c, int r, int itermax, vul_linalg_real eps );
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
void vul_linalg_svd_sparse_qrlq( vul_linalg_svd_basis_sparse *out, int *rank,
                                 vul_linalg_matrix *A,
                                 int c, int r, int itermax, vul_linalg_real eps );

/*
 * Solves the generalized linear least squares problem defined by A and b by
 * a given singular value decomposition of A, and b.
 */
vul_linalg_vector *vul_linalg_linear_least_squares_sparse( vul_linalg_svd_basis_sparse *bases,
                                                           int rank,
                                                           vul_linalg_vector *b );

//--------------------------------
// Miscellanous BLAS functions
//

/*
 * Find the largest eigenvalue in the matrix A of dimentions c,r to the given
 * epsilon or until max_iter iterations have run.
 *
 * Uses the Power method (slow, but simple).
 */
vul_linalg_real vul_linalg_largest_eigenvalue_dense( vul_linalg_real *A, int c, int r, 
                                                     int max_iter, vul_linalg_real eps );

/*
 * Find the condition number of the matrix A. Note that this uses the matrix norm,
 * and computes the condition number as the fraction between largest and smallest
 * non-singluar singular values in the singular value decomposition. This means
 * it is very slow.
 */
vul_linalg_real vul_linalg_condition_number_dense( vul_linalg_real *A, int c, int r, 
                                                   int max_iter, vul_linalg_real eps );

/*
 * Find the largest eigenvalue in the matrix A of dimentions c,r to the given
 * epsilon or until max_iter iterations have run.
 *
 * Uses the Power method (slow, but simple).
 */
vul_linalg_real vul_linalg_largest_eigenvalue_sparse( vul_linalg_matrix *A, int c, int r, 
                                                      int max_iter, vul_linalg_real eps );

/*
 * Find the condition number of the matrix A. Note that this uses the matrix norm,
 * and computes the condition number as the fraction between largest and smallest
 * non-singluar singular values in the singular value decomposition. This means
 * it is very slow.
 */
vul_linalg_real vul_linalg_condition_number_sparse( vul_linalg_matrix *A, int c, int r, 
                                                    int max_iter, vul_linalg_real eps );


#ifdef _cplusplus
}
#endif

#endif // VUL_LINALG_H

#ifdef VUL_DEFINE

#ifdef _cplusplus
extern "C" {
#endif

//----------------------------------
// Sparse datatype local functions
//

#define DECLARE_VECTOR_OP( name, op ) static void name( vul_linalg_vector *out, vul_linalg_vector *a, vul_linalg_vector *b );
DECLARE_VECTOR_OP( vulb__sparse_vadd, + )
DECLARE_VECTOR_OP( vulb__sparse_vsub, - )
DECLARE_VECTOR_OP( vulb__sparse_vmul, * )
#undef DECLARE_VECTOR_OP

static void vulb__sparse_vmul_sub( vul_linalg_vector *out, vul_linalg_vector *a, vul_linalg_vector *x, vul_linalg_vector *b );
static void vulb__sparse_vmul_add( vul_linalg_vector *out, vul_linalg_vector *a, vul_linalg_vector *x, vul_linalg_vector *b );
static void vulb__sparse_vcopy( vul_linalg_vector *out, vul_linalg_vector *x );
static vul_linalg_real vulb__sparse_dot( vul_linalg_vector *a, vul_linalg_vector *b );
static void vulb__sparse_vclear( vul_linalg_vector *v );
static void vulb__sparse_mmul( vul_linalg_vector *out, vul_linalg_matrix *A, vul_linalg_vector *x );
static void vulb__sparse_mmul_submatrix( vul_linalg_vector *out, vul_linalg_matrix *A, vul_linalg_vector *x,
                                         int c, int r );
static void vulb__sparse_mmul_matrix( vul_linalg_matrix *O, vul_linalg_matrix *A, vul_linalg_matrix *B, int n );
static void vulb__sparse_mmul_add( vul_linalg_vector *out, vul_linalg_matrix *A, vul_linalg_vector *x, vul_linalg_vector *b );
static void vulb__sparse_forward_substitute( vul_linalg_vector *out, vul_linalg_matrix *A, vul_linalg_vector *b );
static void vulb__sparse_backward_substitute( vul_linalg_vector *out, vul_linalg_matrix *A, vul_linalg_vector *b );
static void vulb__sparse_backward_substitute_submatrix( vul_linalg_vector *out, vul_linalg_matrix *A, 
                                                        vul_linalg_vector *b, int c, int r );
static void vulb__sparse_mtranspose( vul_linalg_matrix *out, vul_linalg_matrix *A );
static void vulb__sparse_mcopy( vul_linalg_matrix *out, vul_linalg_matrix *A );
static void vulb__sparse_mclear( vul_linalg_matrix *A );
// Helper that returns an empty vector if the row doesn't exist; simplifies some loops
static vul_linalg_vector vulb__linalg_matrix_get_row_by_array_index( vul_linalg_matrix *m, unsigned int r );
/*
 * When inserting zeroes into places that previously held non-zeroes, the memory is not freed.
 * This "compacts" the matrix back into a fully sparse memory pattern.
 */
static void vulb__sparse_mclean( vul_linalg_matrix *A );

/*
 * Apply a Givens rotation to the matrix A at indices i and j
 * with the given sine/cosine. If post_multiply is set,
 * perform A = G * A instead of A = A * G.
 */
static void vul__linalg_givens_rotate_sparse( vul_linalg_matrix *A, int c, int r, 
                                              int i, int j, float cosine, float sine,
                                              int post_multiply );
/*
 * Compute the QR decomposition of A using givens rotations.
 */
static void vul__linalg_qr_decomposition_givens_sparse( vul_linalg_matrix *Q, vul_linalg_matrix *R, 
                                                        vul_linalg_matrix *A, int c, int r );

//------------------------
// Dense local functions
//

#define DECLARE_VECTOR_OP( name, op ) static void name( vul_linalg_real *out, vul_linalg_real *a, vul_linalg_real *b, int n );
DECLARE_VECTOR_OP( vulb__vadd, + )
DECLARE_VECTOR_OP( vulb__vsub, - )
DECLARE_VECTOR_OP( vulb__vmul, * )
#undef DECLARE_VECTOR_OP

static void vulb__vmul_sub( vul_linalg_real *out, vul_linalg_real *a, vul_linalg_real x, vul_linalg_real *b, int n );
static void vulb__vmul_add( vul_linalg_real *out, vul_linalg_real *a, vul_linalg_real x, vul_linalg_real *b, int n );
static void vulb__vcopy( vul_linalg_real *out, vul_linalg_real *x, int n );
static vul_linalg_real vulb__dot( vul_linalg_real *a, vul_linalg_real *b, int n );
static void vulb__mmul( vul_linalg_real *out, vul_linalg_real *A, vul_linalg_real *x, int c, int r );
static void vulb__mmul_matrix( vul_linalg_real *O, vul_linalg_real *A, vul_linalg_real *B, int n );
static void vulb__mmul_add( vul_linalg_real *out, vul_linalg_real *A, vul_linalg_real *x, vul_linalg_real *b, 
                            int c, int r );
static void vulb__forward_substitute( vul_linalg_real *out, vul_linalg_real *A, vul_linalg_real *b, int c, int r );
static void vulb__backward_substitute( vul_linalg_real *out, vul_linalg_real *A, vul_linalg_real *b, int c, int r,
                                       int transpose );
static void vulb__mtranspose( vul_linalg_real *O, vul_linalg_real *A, int c, int r );
static void vulb__mmul_matrix_rect( vul_linalg_real *O, vul_linalg_real *A, vul_linalg_real *B, 
                                    int ca, int ra_cb, int rb );
/*
 * If transposed is set, treat A as A^T.
 * Matrices may not alias.
 * @NOTE(Thynn): Not currently used.
 */ 
static void vul__linalg_qr_decomposition_gram_schmidt( vul_linalg_real *Q, vul_linalg_real *R, vul_linalg_real *A, 
                                                       int c, int r, int transpose );
/*
 * If transposed is set, treat A as A^T.
 * Matrices may alias.
 * @NOTE(Thynn): Not currently used.
 */
static void vul__linalg_qr_decomposition_householder( vul_linalg_real *Q, vul_linalg_real *R, vul_linalg_real *A, 
                                                      int c, int r, int transpose );
/*
 * If transposed is set, treat A as A^T.
 * Matrices may not alias.
 */
static void vul__linalg_qr_decomposition_givens( vul_linalg_real *Q, vul_linalg_real *R, vul_linalg_real *A, 
                                                 int c, int r, int transpose );

/*
 * Apply a Givens rotation to the matrix A at indices i and j
 * with the given sine/cosine. If post_multiply is set,
 * perform A = G * A instead of A = A * G.
 */
static void vul__linalg_givens_rotate( vul_linalg_real *A, int c, int r, 
                                       int i, int j, float cosine, float sine,
                                       int post_multiply );

//-----------------------------------------------------
// Dense Singular Value Decomposition local functions
//

/*
 * Use shell sort to sort the bases.
 */
static void vul__linalg_svd_sort( vul_linalg_svd_basis *x, int n );

/*
 * Qt and u are optional space to perform operations in (and to use the data afterwards, as in QR decomposition.
 * If they are NULL, we allocate room for them inside.
 * If QO and Q are not NULL, Q is postmultiplied by Qt into QO, allowing for accumulation of Q and not just R
 * during QR decomposition. If they (either of them) are NULL, this step is skipped.
 */ 
static void vul__linalg_apply_householder_column( vul_linalg_real *QO, vul_linalg_real *A,
                                                  vul_linalg_real *RO, vul_linalg_real *R,
                                                  int c, int r, int qc, int qr, int k,
                                                  vul_linalg_real *Qt, vul_linalg_real *u, int respect_signbit );
/*
 * Calculate the norm of the matrix' diagonal as a vector.
 */
static vul_linalg_real vul__linalg_matrix_norm_diagonal( vul_linalg_real *A, int c, int r );

/*
 * Only values above the "upper_diag"-th diagonal are used, so for
 * the full matrix, use -min(r,c)/2.
 */
static vul_linalg_real vul__linalg_matrix_norm_as_single_column( vul_linalg_real *A, int c, int r, int upper_diag );

//-----------------------------------------------------
// Dense Singular Value Decomposition local functions
//

/*
 * Calculate the norm of the matrix' diagonal as a vector.
 */
static vul_linalg_real vul__linalg_matrix_norm_diagonal_sparse( vul_linalg_matrix *A );

/*
 * Only values above the "upper_diag"-th diagonal are used, so for
 * the full matrix, use -min(r,c)/2.
 */
static vul_linalg_real vul__linalg_matrix_norm_as_single_column_sparse( vul_linalg_matrix *A, int upper_diag );
/*
 * Use shell sort to sort the bases.
 */
static void vul__linalg_svd_sort_sparse( vul_linalg_svd_basis_sparse *x, int n );

#ifdef _cplusplus
}
#endif
#endif

#ifdef VUL_DEFINE

#ifdef _cplusplus
extern "C" {
#endif

#ifdef VUL_LINALG_ERROR_STDERR
#define ERR( ... )\
   fprintf( stderr, __VA_ARGS__ );
#elif defined( VUL_LINALG_ERROR_STR )
char vul_linalg_last_error[ 1024 ];
#define ERR( ... )\
   snprintf( vul_last_error, 1023, __VA_ARGS__ );\
   vul_linalg_last_error[ 1023 ] = 0;
#elif defined( VUL_LINALG_ERROR_ASSERT )
#define ERR( ... )\
   assert( 0 );
#elif defined( VUL_LINALG_ERROR_QUIET )
#define ERR( ... )\
   {(void)0;}
#elif defined( VUL_LINALG_ERROR_CUSTOM )
#define ERR( ... )\
   VUL_LINALG_ERROR_CUSTOM( __VA_ARGS__)
#endif

//-------------------------------
// Sparse datatype public functions
//

vul_linalg_vector *vul_linalg_vector_create( unsigned *idxs, vul_linalg_real *vals, unsigned int n )
{
   unsigned int i;
   vul_linalg_vector *v;

   v = ( vul_linalg_vector* )VUL_LINALG_ALLOC( sizeof( vul_linalg_vector ) );
   v->count = 0;
   v->entries = 0;
   for( i = 0; i < n; ++i ) {
      vul_linalg_vector_insert( v, idxs[ i ], vals[ i ] );
   }
   return v;
}

vul_linalg_matrix *vul_linalg_matrix_create( unsigned int *rows, unsigned int *cols, vul_linalg_real *vals, unsigned int init_count )
{
   vul_linalg_matrix *m;
   unsigned int i;
   
   m = ( vul_linalg_matrix* )VUL_LINALG_ALLOC( sizeof( vul_linalg_matrix ) );
   m->count = 0;
   m->rows = 0;

   for( i = 0; i < init_count; ++i ) {
      vul_linalg_matrix_insert( m, rows[ i ], cols[ i ], vals[ i ] );
   }
   return m;
}

void vul_linalg_matrix_destroy( vul_linalg_matrix *m )
{
   unsigned int i;
   for( i = 0; i < m->count; ++i ) {
      if( m->rows[ i ].vec.count ) {
         VUL_LINALG_FREE( m->rows[ i ].vec.entries );
      }
   }
   VUL_LINALG_FREE( m->rows );
   m->rows = 0;

   VUL_LINALG_FREE( m );
}

void vul_linalg_vector_insert( vul_linalg_vector *v, unsigned int idx, vul_linalg_real val )
{
   vul_linalg_sparse_entry *e;
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

   e = ( vul_linalg_sparse_entry* )VUL_LINALG_ALLOC( sizeof( vul_linalg_sparse_entry ) * ( v->count + 1 ) );
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
      VUL_LINALG_FREE( v->entries );
   }
   v->entries = e;
   v->count = v->count + 1;
}

void vul_linalg_matrix_insert( vul_linalg_matrix *m, unsigned int r, unsigned int c, vul_linalg_real val )
{
   unsigned int i, j, inserted;
   vul_linalg_matrix_row *e;
   
   // Overwrite, even if zero
   for( i = 0; i < m->count; ++i ) {
      if( m->rows[ i ].idx == r ) {
         vul_linalg_vector_insert( &m->rows[ i ].vec, c, val );
         return;
      }
   }

   // If zero, don't create a new entry
   if( val == 0.f ) {
      return;
   }

   e = ( vul_linalg_matrix_row* )VUL_LINALG_ALLOC( sizeof( vul_linalg_matrix_row ) * ( m->count + 1 ) );
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
            vul_linalg_vector_insert( &e[ i ].vec, c, val );
            inserted = 1;
         }
      }
   } else {
      e[ 0 ].idx = r;
      e[ 0 ].vec.count = 0;
      e[ 0 ].vec.entries = 0;
      vul_linalg_vector_insert( &e[ 0 ].vec, c, val );
   }
   if( m->rows ) {
      VUL_LINALG_FREE( m->rows );
   }
   m->rows = e;
   m->count = m->count + 1;
}

vul_linalg_real vul_linalg_vector_get( vul_linalg_vector *v, unsigned int idx )
{
   unsigned int i;

   for( i = 0; i < v->count; ++i ) {
      if( v->entries[ i ].idx == idx ) {
         return v->entries[ i ].val;
      }
   }
   return 0.f;
}

vul_linalg_real vul_linalg_matrix_get( vul_linalg_matrix *m, unsigned int r, unsigned int c )
{
   unsigned int i;

   for( i = 0; i < m->count; ++i ) {
      if( m->rows[ i ].idx == r ) {
         return vul_linalg_vector_get( &m->rows[ i ].vec, c );
      }
   }
   return 0.f;
}
static vul_linalg_vector vulb__linalg_matrix_get_row_by_array_index( vul_linalg_matrix *m, unsigned int r )
{
   vul_linalg_vector v;

   if( r >= m->count ) {
      v.count = 0;
      v.entries = 0;
      return v;
   }
   return m->rows[ r ].vec;
}

void vul_linalg_vector_destroy( vul_linalg_vector *v )
{
   if( v->entries ) {
      VUL_LINALG_FREE( v->entries );
   }
   VUL_LINALG_FREE( v );
}

//----------------------------------------
// Sparse datatype local functions
//

#define DEFINE_VECTOR_OP( name, op )\
   static void name( vul_linalg_vector *out, vul_linalg_vector *a, vul_linalg_vector *b )\
   {\
      unsigned int ia, ib;\
      ia = 0; ib = 0;\
      while( ia < a->count && ib < b->count ) {\
         if( a->entries[ ia ].idx == b->entries[ ib ].idx ) {\
            vul_linalg_vector_insert( out, a->entries[ ia ].idx, a->entries[ ia ].val op b->entries[ ib ].val );\
            ++ia; ++ib;\
         } else if( a->entries[ ia ].idx < b->entries[ ib ].idx ) {\
            vul_linalg_vector_insert( out, a->entries[ ia ].idx, a->entries[ ia ].val op 0.0 );\
            ++ia;\
         } else {\
            vul_linalg_vector_insert( out, b->entries[ ib ].idx, 0.0 op b->entries[ ib ].val );\
            ++ib;\
         }\
      }\
      while( ia < a->count ) {\
         vul_linalg_vector_insert( out, a->entries[ ia ].idx, a->entries[ ia ].val op 0.0 );\
         ++ia;\
      }\
      while( ib < b->count ) {\
         vul_linalg_vector_insert( out, b->entries[ ib ].idx, 0.0 op b->entries[ ib ].val );\
         ++ib;\
      }\
   }
DEFINE_VECTOR_OP( vulb__sparse_vadd, + )
DEFINE_VECTOR_OP( vulb__sparse_vsub, - )
DEFINE_VECTOR_OP( vulb__sparse_vmul, * )

#undef DEFINE_VECTOR_OP

static void vulb__sparse_vmul_sub( vul_linalg_vector *out, vul_linalg_vector *a, vul_linalg_vector *x, vul_linalg_vector *b )
{
   unsigned int ia, ix, ib;

   ia= 0; ix = 0; ib = 0;
   while( ia < a->count && ix < x->count && ib < b->count ) {
      if( a->entries[ ia ].idx == x->entries[ ix ].idx && a->entries[ ia ].idx == b->entries[ ib ].idx ) {
         vul_linalg_vector_insert( out, a->entries[ ia ].idx, a->entries[ ia ].val * x->entries[ ix ].val -
                                                              b->entries[ ib ].val );
         ++ia; ++ib; ++ix;
      } else if( b->entries[ ib ].idx <= a->entries[ ia ].idx && b->entries[ ib ].idx <= x->entries[ ix ].idx ) {
         vul_linalg_vector_insert( out, b->entries[ ib ].idx, -b->entries[ ib ].val );
         ++ib;
      } else if( a->entries[ ia ].idx < x->entries[ ix ].idx ) {
         ++ia;
      } else {
         ++ix;
      }
   }
}
static void vulb__sparse_vmul_add( vul_linalg_vector *out, vul_linalg_vector *a, vul_linalg_vector *x, vul_linalg_vector *b )
{
   unsigned int ia, ix, ib;

   ia= 0; ix = 0; ib = 0;
   while( ia < a->count && ix < x->count && ib < b->count ) {
      if( a->entries[ ia ].idx == x->entries[ ix ].idx && a->entries[ ia ].idx == b->entries[ ib ].idx ) {
         vul_linalg_vector_insert( out, a->entries[ ia ].idx, a->entries[ ia ].val * x->entries[ ix ].val +
                                                              b->entries[ ib ].val );
         ++ia; ++ib; ++ix;
      } else if( b->entries[ ib ].idx <= a->entries[ ia ].idx && b->entries[ ib ].idx <= x->entries[ ix ].idx ) {
         vul_linalg_vector_insert( out, b->entries[ ib ].idx, b->entries[ ib ].val );
         ++ib;
      } else if( a->entries[ ia ].idx < x->entries[ ix ].idx ) {
         ++ia;
      } else {
         ++ix;
      }
   }
}

static void vulb__sparse_vcopy( vul_linalg_vector *out, vul_linalg_vector *x )
{
   unsigned int i;

   if( out->count ) {
      out->count = 0;
      if( out->entries ) {
         VUL_LINALG_FREE( out->entries );
      }
   }
   if( x->count ) {
      out->entries = ( vul_linalg_sparse_entry* )VUL_LINALG_ALLOC( sizeof( vul_linalg_sparse_entry ) * 
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
static vul_linalg_real vulb__sparse_dot( vul_linalg_vector *a, vul_linalg_vector *b )
{
   vul_linalg_real r;
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
static void vulb__sparse_vclear( vul_linalg_vector *v )
{
   if( v->entries ) {
      VUL_LINALG_FREE( v->entries );
   }
   v->entries = 0;
   v->count = 0;
}
static void vulb__sparse_mmul( vul_linalg_vector *out, vul_linalg_matrix *A, vul_linalg_vector *x )
{
   unsigned int v, i, ix;
   vul_linalg_real sum;

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
      vul_linalg_vector_insert( out, A->rows[ v ].idx, sum );
   }
}
static void vulb__sparse_mmul_submatrix( vul_linalg_vector *out, vul_linalg_matrix *A, vul_linalg_vector *x,
                                         int c, int r )
{
   unsigned int v, i, ix;
   vul_linalg_real sum;

   for( v = 0; v < A->count && A->rows[ v ].idx < r; ++v ) {
      sum = 0.f;
      i = 0; ix = 0;
      while( i < A->rows[ v ].vec.count && ix < x->count && 
             A->rows[ v ].idx < c && x->entries[ ix ].idx < c ) {
         if( A->rows[ v ].vec.entries[ i ].idx == x->entries[ ix ].idx ) {
            sum += A->rows[ v ].vec.entries[ i ].val * x->entries[ ix ].val;
            ++i; ++ix;
         } else if( A->rows[ v ].vec.entries[ i ].idx < x->entries[ ix ].idx ) {
            ++i;
         } else {
            ++ix;
         }
      }
      vul_linalg_vector_insert( out, A->rows[ v ].idx, sum );
   }
}
static void vulb__sparse_mmul_matrix( vul_linalg_matrix *O, vul_linalg_matrix *A, vul_linalg_matrix *B, int n )
{
   vul_linalg_real s;
   int i, j, k;
   
   //@TODO(thynn): Strassen instead of this naïve approach.
   for( i = 0; i < A->count; ++i ) {
      for( j = 0; j < n; ++j ) {
         s = 0.f;
         for( k = 0; k < A->rows[ i ].vec.count; ++k ) {
            s += A->rows[ i ].vec.entries[ k ].val 
               * vul_linalg_matrix_get( B, A->rows[ i ].vec.entries[ k ].idx, j );
         }
         vul_linalg_matrix_insert( O, A->rows[ i ].idx, j, s );
      }
   }
}
static void vulb__sparse_mmul_add( vul_linalg_vector *out, vul_linalg_matrix *A, vul_linalg_vector *x, vul_linalg_vector *b )
{
   unsigned int v, i, ix, ib, found;
   vul_linalg_real sum;

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
      vul_linalg_vector_insert( out, A->rows[ v ].idx, sum );
   }
}
static void vulb__sparse_forward_substitute( vul_linalg_vector *out, vul_linalg_matrix *A, vul_linalg_vector *b )
{
   int i, j;
   vul_linalg_real sum;

   for( i = 0; i < A->count; ++i ) {
      sum = vul_linalg_vector_get( b, A->rows[ i ].idx );
      for( j = A->rows[ i ].vec.count - 1; 
           j >= 0 && A->rows[ i ].vec.entries[ j ].idx >= A->rows[ i ].idx ; 
           --j )
           ; // Find j = i - 1
      for( ; j >= 0 ; --j ) {
         sum -= A->rows[ i ].vec.entries[ j ].val * vul_linalg_vector_get( out, A->rows[ i ].vec.entries[ j ].idx );
      }
      vul_linalg_vector_insert( out, A->rows[ i ].idx, 
                                sum / vul_linalg_matrix_get( A, A->rows[ i ].idx, A->rows[ i ].idx ) );
   }
}

static void vulb__sparse_backward_substitute( vul_linalg_vector *out, vul_linalg_matrix *A, vul_linalg_vector *b )
{
   int i, j;
   vul_linalg_real sum;

   for( i = A->count - 1; i >= 0; --i ) {
      sum = vul_linalg_vector_get( b, A->rows[ i ].idx );
      for( j = 0; j < A->rows[ i ].vec.count && A->rows[ i ].vec.entries[ j ].idx <= A->rows[ i ].idx; ++j )
         ; // Find index of first column > i
      while( j < A->rows[ i ].vec.count ) {
         sum -= A->rows[ i ].vec.entries[ j ].val * vul_linalg_vector_get( out, A->rows[ i ].vec.entries[ j ].idx );
         ++j;
      }
      vul_linalg_vector_insert( out, A->rows[ i ].idx, 
                                sum / vul_linalg_matrix_get( A, A->rows[ i ].idx, A->rows[ i ].idx ) );
   }
}
static void vulb__sparse_backward_substitute_submatrix( vul_linalg_vector *out, vul_linalg_matrix *A, 
                                                        vul_linalg_vector *b, int c, int r )
{
   int i, j;
   vul_linalg_real sum;

   for( i = A->count - 1; i >= 0 && A->rows[ i ].idx >= r; --i )
      ; // Find last row to use
   while( i >= 0 ) {
      sum = vul_linalg_vector_get( b, A->rows[ i ].idx );
      for( j = 0; j < A->rows[ i ].vec.count && 
                  A->rows[ i ].vec.entries[ j ].idx <= A->rows[ i ].idx &&
                  A->rows[ i ].vec.entries[ j ].idx < c; ++j )
         ; // Find index of first column > i
      while( j < A->rows[ i ].vec.count && A->rows[ i ].vec.entries[ j ].idx < c ) {
         sum -= A->rows[ i ].vec.entries[ j ].val * vul_linalg_vector_get( out, A->rows[ i ].vec.entries[ j ].idx );
         ++j;
      }
      vul_linalg_vector_insert( out, A->rows[ i ].idx, 
                                sum / vul_linalg_matrix_get( A, A->rows[ i ].idx, A->rows[ i ].idx ) );
      --i;
   }
}
static void vulb__sparse_mtranspose( vul_linalg_matrix *out, vul_linalg_matrix *A )
{
   int i, j;
   
   vulb__sparse_mclear( out );
   for( i = 0; i < A->count; ++i ) {
      for( j = 0; j < A->rows[ i ].vec.count; ++j ) {
         vul_linalg_matrix_insert( out, A->rows[ i ].vec.entries[ j ].idx, 
                                        A->rows[ i ].idx, 
                                        A->rows[ i ].vec.entries[ j ].val );
      }
   }
}
static void vulb__sparse_mcopy( vul_linalg_matrix *out, vul_linalg_matrix *A )
{
   int i, j;

   vulb__sparse_mclear( out );
   for( i = 0; i < A->count; ++i ) {
      for( j = 0; j < A->rows[ i ].vec.count; ++j ) {
         vul_linalg_matrix_insert( out, A->rows[ i ].idx, 
                                        A->rows[ i ].vec.entries[ j ].idx, 
                                        A->rows[ i ].vec.entries[ j ].val );
      }
   }
}

static void vulb__sparse_mclear( vul_linalg_matrix *A )
{
   int i;

   for( i = 0; i < A->count; ++i ) {
      if( A->rows[ i ].vec.count ) {
         VUL_LINALG_FREE( A->rows[ i ].vec.entries );
      }
      A->rows[ i ].vec.count = 0;
   }
   VUL_LINALG_FREE( A->rows );
   A->rows = 0;
   A->count = 0;
}
static void vulb__sparse_mclean( vul_linalg_matrix *A )
{
   int i, j, k, c;
   vul_linalg_sparse_entry *n;

   for( i = 0; i < A->count; ++i ) {
      c = 0;
      for( j = 0; j < A->rows[ i ].vec.count; ++j ) {
         if( A->rows[ i ].vec.entries[ j ].val != 0.f ) {
            ++c;
         }
      }
      if( c != A->rows[ i ].vec.count ) {
         if( c != 0 ) {
            n = ( vul_linalg_sparse_entry* )VUL_LINALG_ALLOC( sizeof( vul_linalg_sparse_entry ) * c );
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
         VUL_LINALG_FREE( A->rows[ i ].vec.entries );
         A->rows[ i ].vec.entries = n;
         A->rows[ i ].vec.count = c;
      }
   }
}

//--------------------------
// Sparse preconditioners
//

vul_linalg_matrix *vul_linalg_precondition_ilu0( vul_linalg_matrix *A, int c, int r )
{
   // @TODO(thynn): Use this algorithm for sparse LU as well!
   vul_linalg_matrix *P, *Ap;
   vul_linalg_real pivot, sum;
   int i, j, k, n;

   P = vul_linalg_matrix_create( 0, 0, 0, 0 );

   vul_linalg_matrix *LT, *U;
   n = r < c ? r : c;
   LT = vul_linalg_matrix_create( 0, 0, 0, 0 );
   U = vul_linalg_matrix_create( 0, 0, 0, 0 );
   for( i = 0; i < n; ++i ) {
      vul_linalg_matrix_insert( U, i, i, 1.0 );
      vul_linalg_matrix_insert( LT, i, i, 1.0 );
   }
   vul_linalg_vector *s = vul_linalg_vector_create( 0, 0, 0 );
   for( i = 0; i < A->count; ++i ) {
      // Solve Lx = A(i:)^T (This is me testing if something works!)
      if( i != 0 ) {
         vulb__sparse_vclear( s );
      }
      vulb__sparse_vcopy( s, &A->rows[ i ].vec );
      for( j = 0; j < s->count; ++j ) {
         for( k = 0; 
              k < LT->rows[ s->entries[ j ].idx ].vec.count && 
              LT->rows[ s->entries[ j ].idx ].vec.entries[ k ].idx <= s->entries[ j ].idx; 
              ++k )
            ; // Find k == j
         while( k < LT->rows[ s->entries[ j ].idx ].vec.count ) {
            vul_linalg_real v = vul_linalg_vector_get( s, LT->rows[ s->entries[ j ].idx ].vec.entries[ k ].idx );
            v -= LT->rows[ s->entries[ j ].idx ].vec.entries[ k ].val * s->entries[ j ].val;
            vul_linalg_vector_insert( s, LT->rows[ s->entries[ j ].idx ].vec.entries[ k ].idx, v );
            ++k;
         }
      }
      // @TODO(thynn): This can be improved with partial pivoting on s
      // Form U
      for( j = 0; j < s->count && s->entries[ j ].idx < A->rows[ i ].idx; ++j )
         ; // Find j == k
      while( j < s->count ) {
         if( vul_linalg_matrix_get( A, A->rows[ i ].idx, s->entries[ j ].idx ) != 0.0 ) {
            vul_linalg_matrix_insert( U, A->rows[ i ].idx, s->entries[ j ].idx, s->entries[ j ].val );
         }
         ++j;
      }
      // Form L^T
      for( j = 0; j < s->count && s->entries[ j ].idx < A->rows[ i ].idx; ++j )
         ; // Find j == i
      while( j < s->count ) {
         if( vul_linalg_matrix_get( A, s->entries[ j ].idx, A->rows[ i ].idx ) != 0.0 ) {
            vul_linalg_matrix_insert( LT, A->rows[ i ].idx, s->entries[ j ].idx, s->entries[ j ].val
                                                             / vul_linalg_matrix_get( U, A->rows[ i ].idx,
                                                                                         A->rows[ i ].idx ) );
         }
         ++j;
      }
   }
   // @TODO(thynn): Instead of forming P here, make preconditioner a struct with more than one matrix if needed?
   vulb__sparse_mcopy( P, U );
   for( i = 0; i < LT->count; ++i ) {
      for( j = 0; j < LT->rows[ i ].vec.count; ++j ) {
         if( LT->rows[ i ].idx != LT->rows[ i ].vec.entries[ j ].idx && vul_linalg_matrix_get( A, LT->rows[ i ].idx, LT->rows[ i ].vec.entries[ j ].idx ) != 0.0 ) {
            vul_linalg_matrix_insert( P, LT->rows[ i ].vec.entries[ j ].idx, LT->rows[ i ].idx,
                                      LT->rows[ i ].vec.entries[ j ].val );
         }
      }
   }
   
   vul_linalg_vector_destroy( s );
   vul_linalg_matrix_destroy( LT );
   vul_linalg_matrix_destroy( U );

   return P;
}

vul_linalg_matrix *vul_linalg_precondition_ichol( vul_linalg_matrix *A, int c, int r )
{
   vul_linalg_matrix *P;
   vul_linalg_vector rowi, rowj;
   vul_linalg_real d, v;
   int i, j, ki, kj;
   
   P = vul_linalg_matrix_create( 0, 0, 0, 0 );
   for( i = 0; i < A->count; ++i ) {
      for( j = 0; j < A->rows[ i ].vec.count; ++j ) {
         if( A->rows[ i ].vec.entries[ j ].idx <= A->rows[ i ].idx ) {
            vul_linalg_matrix_insert( P, A->rows[ i ].idx, 
                                         A->rows[ i ].vec.entries[ j ].idx, 
                                         A->rows[ i ].vec.entries[ j ].val );
         }
      }
   }
   
   for( i = 0; i < A->count; ++i ) {
      for( j = 0; j < A->rows[ i ].vec.count && A->rows[ i ].vec.entries[ j ].idx < A->rows[ i ].idx; ++j )
         ; // Find first entry that is above or on the diagonal
      while( j < A->rows[ i ].vec.count ) {
         d = vul_linalg_matrix_get( A, A->rows[ i ].vec.entries[ j ].idx, A->rows[ i ].idx );
         // We store in the lower half so we can easily iterate over row members here, instead of columns
         // When solving we need to backsub instead of fwdsub on the left due to this!
         rowi = vulb__linalg_matrix_get_row_by_array_index( P, A->rows[ i ].vec.entries[ j ].idx );
         rowj = vulb__linalg_matrix_get_row_by_array_index( P, A->rows[ i ].idx );
         for( ki = rowi.count - 1; ki >= 0 && rowi.entries[ ki ].idx >= A->rows[ i ].idx; --ki )
            ; // Search backwards until ki < i
         for( kj = rowj.count - 1; kj >= 0 && rowj.entries[ kj ].idx >= A->rows[ i ].idx; --kj )
            ; // Search backwards until kj < i
         while( ki >= 0 && kj >= 0 ) {
            if( rowi.entries[ ki ].idx == rowj.entries[ kj ].idx ) {
               d -= rowi.entries[ ki ].val * rowj.entries[ kj ].val;
               --ki; --kj;
            } else if( rowi.entries[ ki ].idx >= rowj.entries[ kj ].idx ) {
               --ki;
            } else {
               --kj;
            }
         }
         if( A->rows[ i ].idx == A->rows[ i ].vec.entries[ j ].idx ) {
            if( d <= 0.f ) {
               ERR( "Cholesky decomposition is only valid for POSITIVE-DEFINITE symmetric matrices." );
               return NULL;
            }
            vul_linalg_matrix_insert( P, A->rows[ i ].idx, A->rows[ i ].idx, sqrt( d ) );
         } else {
            v = vul_linalg_matrix_get( P, A->rows[ i ].idx, A->rows[ i ].idx );
            if( v == 0.f ) {
               ERR( "Determinant is sufficiently small that a divide-by-zero is imminent." );
               return NULL;
            }
            vul_linalg_matrix_insert( P, A->rows[ i ].vec.entries[ j ].idx,A->rows[ i ].idx,  d / v );
         }
         ++j;
      }
   }

   return P;
}

vul_linalg_matrix *vul_linalg_precondition_jacobi( vul_linalg_matrix *A, int c, int r )
{
   vul_linalg_matrix *P;
   int i, n;
   
   P = vul_linalg_matrix_create( 0, 0, 0, 0 );
   n = r > c ? c : r;
   for( i = 0; i < n; ++i ) {
      vul_linalg_matrix_insert( P, i, i, 1.0 / vul_linalg_matrix_get( A, i, i ) );
   }

   return P;
}

enum vul__linalg_precondition_apply_side {
   VUL__LINALG_PRECONDITION_LEFT = 1,
   VUL__LINALG_PRECONDITION_RIGHT = 2,
   VUL__LINALG_PRECONDITION_BOTH = VUL__LINALG_PRECONDITION_LEFT | VUL__LINALG_PRECONDITION_RIGHT
} vul__linalg_precondition_apply_side;

void vul__linalg_precondition_solve( vul_linalg_precoditioner_type type,
                                     vul_linalg_vector *x,
                                     vul_linalg_matrix *P, vul_linalg_vector *b,
                                     enum vul__linalg_precondition_apply_side side )
{
   int i, k, j, set;

   vulb__sparse_vclear( x );
   set = 0;

   if( side & VUL__LINALG_PRECONDITION_RIGHT ) {
      switch( type ) {
      case VUL_LINALG_PRECONDITIONER_JACOBI: {
         for( i = 0; i < b->count; ++i ) {
            vul_linalg_vector_insert( x, b->entries[ i ].idx, 
                                      vul_linalg_matrix_get( P, b->entries[ i ].idx, b->entries[ i ].idx ) *
                                      b->entries[ i ].val );
         }
         set = 1;
      } break;
      case VUL_LINALG_PRECONDITIONER_INCOMPLETE_CHOLESKY: {
         // Backward because we store it in the lower part
         vulb__sparse_backward_substitute( x, P, b );
         set = 1;
      } break;
      case VUL_LINALG_PRECONDITIONER_INCOMPLETE_LU_0: {
         vul_linalg_real sum;
         
         /* Solve Lx = b */
         vulb__sparse_vcopy( x, b );
         for( i = 0; i < P->count; ++i ) {
            sum = vul_linalg_vector_get( x, P->rows[ i ].idx );
            for( j = 0; j < P->rows[ i ].vec.count && P->rows[ i ].vec.entries[ j ].idx < P->rows[ i ].idx; ++j ){
               sum -= P->rows[ i ].vec.entries[ j ].val 
                    * vul_linalg_vector_get( x, P->rows[ i ].vec.entries[ j ].idx );
            }
            vul_linalg_vector_insert( x, P->rows[ i ].idx, sum );
         }
         set = 1;
      } break;
      case VUL_LINALG_PRECONDITIONER_NONE: {
         // Do nothing
      } break;
      default:
         ERR( "Unknown preconditioner, can't solve for it!" );
      }
   }
   if( side & VUL__LINALG_PRECONDITION_LEFT ) {
      switch( type ) {
      case VUL_LINALG_PRECONDITIONER_INCOMPLETE_CHOLESKY: {
         vul_linalg_vector *y;

         y = vul_linalg_vector_create( 0, 0, 0 );
         if( side == VUL__LINALG_PRECONDITION_BOTH ) {
            vulb__sparse_vcopy( y, x );
         } else {
            vulb__sparse_vcopy( y, b );
         }

         /* Backward substitute */
         for( i = P->count - 1; i >= 0; --i ) {
            vul_linalg_real pivot, sum = vul_linalg_vector_get( y, P->rows[ i ].idx );
            for( j = 0; j < P->rows[ i ].vec.count && P->rows[ i ].vec.entries[ j ].idx < P->rows[ i ].idx; ++j )
               ; // Find j = i
            pivot = P->rows[ i ].vec.entries[ j++ ].val; // Store P[i][i]
            while( j < P->rows[ i ].vec.count ) {
               sum -= P->rows[ i ].vec.entries[ j ].val 
                    * vul_linalg_vector_get( x, P->rows[ i ].vec.entries[ j ].idx );
               ++j;
            }
            vul_linalg_vector_insert( x, P->rows[ i ].idx, sum / pivot );
         }
         vul_linalg_vector_destroy( y );
         set = 1;
      } break;
      case VUL_LINALG_PRECONDITIONER_INCOMPLETE_LU_0: {
         vul_linalg_vector *y;
         
         y = vul_linalg_vector_create( 0, 0, 0 );
         if( side == VUL__LINALG_PRECONDITION_BOTH ) {
            vulb__sparse_vcopy( y, x );
         } else {
            vulb__sparse_vcopy( y, b );
         }
         /* Solve Ux = y */
         vulb__sparse_backward_substitute( x, P, y );
         vul_linalg_vector_destroy( y );
         set = 1;
      } break;
      case VUL_LINALG_PRECONDITIONER_JACOBI:
      case VUL_LINALG_PRECONDITIONER_NONE: {
         // Do nothing
      } break;
      default:
         ERR( "Unknown preconditioner, can't solve for it!" );
      }
   }

   if( !set ) {
      vulb__sparse_vcopy( x, b );
   }
}

//---------------------
// Sparse solvers
//

vul_linalg_vector *vul_linalg_conjugate_gradient_sparse( vul_linalg_matrix *A,
                                                         vul_linalg_vector *initial_guess,
                                                         vul_linalg_vector *b,
                                                         vul_linalg_matrix *P,
                                                         vul_linalg_precoditioner_type ptype,
                                                         int max_iterations,
                                                         vul_linalg_real tolerance )
{
   vul_linalg_vector *x, *r, *Ap, *p, *z;
   vul_linalg_real rd, rd2, alpha, beta, tmp;
   int i, j, k, idx, found;

   r = vul_linalg_vector_create( 0, 0, 0 );
   z = vul_linalg_vector_create( 0, 0, 0 );
   p = vul_linalg_vector_create( 0, 0, 0 );
   Ap = vul_linalg_vector_create( 0, 0, 0 );
   
   x = vul_linalg_vector_create( 0, 0, 0 );
   vulb__sparse_vcopy( x, initial_guess );
   vulb__sparse_mmul( r, A, x );
   vulb__sparse_vsub( r, b, r );
   vul__linalg_precondition_solve( ptype, p, P, r, VUL__LINALG_PRECONDITION_BOTH );
   vulb__sparse_vcopy( p, r );

   rd = vulb__sparse_dot( p, r );
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
            vul_linalg_vector_insert( x, idx, -tmp );
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
            vul_linalg_vector_insert( r, idx, -tmp );
         }
      }
      rd2 = vulb__sparse_dot( r, r );
      if( fabs( rd2 - rd ) < tolerance * r->count ) {
         break;
      }
      vul__linalg_precondition_solve( ptype, z, P, r, VUL__LINALG_PRECONDITION_BOTH );
      rd2 = vulb__sparse_dot( z, r );
      beta = rd2 / rd;
      for( j = 0, k = 0; j < p->count && k < z->count; ) {
         if( p->entries[ j ].idx == z->entries[ k ].idx ) {
            p->entries[ j ].val = z->entries[ k ].val + p->entries[ j ].val * beta;
            ++j; ++k;
         } else if( p->entries[ j ].idx < z->entries[ k ].idx ) {
            ++j;
         } else {
            vul_linalg_vector_insert( p, z->entries[ k ].idx, z->entries[ k ].val );
            ++k;
         }
      }
      rd = rd2;
   }

   vul_linalg_vector_destroy( z );
   vul_linalg_vector_destroy( p );
   vul_linalg_vector_destroy( r );
   vul_linalg_vector_destroy( Ap );
   return x;
}

vul_linalg_vector *vul_linalg_gmres_sparse( vul_linalg_matrix *A,
                                            vul_linalg_vector *initial_guess,
                                            vul_linalg_vector *b,
                                            vul_linalg_matrix *P,
                                            vul_linalg_precoditioner_type ptype,
                                            int restart_interval,
                                            int max_iterations,
                                            vul_linalg_real tolerance )
{
   vul_linalg_matrix *V, *H;
   vul_linalg_vector *r, *x, *e, *y, *s, *w;
   vul_linalg_real bd, rd, err, tmp, *cosines, *sines, v0, v1;
   int i, j, k, l, m;

   x = vul_linalg_vector_create( 0, 0, 0 );
   r = vul_linalg_vector_create( 0, 0, 0 );
   w = vul_linalg_vector_create( 0, 0, 0 );
   vulb__sparse_vcopy( x, initial_guess );
   vulb__sparse_mmul( r, A, x );
   vulb__sparse_vsub( w, b, r );
   vul__linalg_precondition_solve( ptype, r, P, w, VUL__LINALG_PRECONDITION_LEFT );
   bd = vulb__sparse_dot( b, b ); bd = sqrt( bd );
   rd = vulb__sparse_dot( r, r ); rd = sqrt( rd );

   err = rd / bd;
   if( err <= tolerance ) {
      vul_linalg_vector_destroy( r );
      vul_linalg_vector_destroy( w );
      return x; // Initial guess is close enough!
   }

   V = vul_linalg_matrix_create( 0, 0, 0, 0 ); // V is cols * restart_interval
   H = vul_linalg_matrix_create( 0, 0, 0, 0 ); // H is restart_interval+1 * restart_interval
   e = vul_linalg_vector_create( 0, 0, 0 );
   s = vul_linalg_vector_create( 0, 0, 0 );
   y = vul_linalg_vector_create( 0, 0, 0 );
   cosines = ( vul_linalg_real* )VUL_LINALG_ALLOC( sizeof( vul_linalg_real ) * restart_interval );
   sines   = ( vul_linalg_real* )VUL_LINALG_ALLOC( sizeof( vul_linalg_real ) * restart_interval );
   memset( cosines, 0, sizeof( vul_linalg_real ) * restart_interval );
   memset( sines,   0, sizeof( vul_linalg_real ) * restart_interval );

   vul_linalg_vector_insert( e, 0, 1.0 );

   for( k = 0; k < max_iterations; ++k ) {
      printf("iter %d, %e\n", k, rd);
      // v_1 = r / norm( r )
      for( i = 0; i < r->count; ++i ) {
         vul_linalg_matrix_insert( V, 0, r->entries[ i ].idx, r->entries[ i ].val / rd );
      }
      // s = norm(r)*e
      vulb__sparse_vclear( s );
      for( i = 0; i < e->count; ++i ) {
         vul_linalg_vector_insert( s, e->entries[ i ].idx, e->entries[ i ].val * rd );
      }

      for( i = 0; i < restart_interval; ++i ) {
         for( j = 0, l = -1; j < V->count; ++j ) {
            if( V->rows[ j ].idx == i ) {
               l = j;
               break;
            }
         }
         if( l == -1 ) {
            ERR( "GMRES has encountered an all-zero orthonormal basis, which isn't really possible. Is the"
                 " matrix singular? Returning current estimate (likely wrong)." );
            return x;
         }
         vulb__sparse_vclear( w );
         vulb__sparse_vclear( y );
         vul__linalg_precondition_solve( ptype, y, P, &V->rows[ l ].vec, VUL__LINALG_PRECONDITION_RIGHT );
         vulb__sparse_mmul( w, A, y );

         // Construct orthonormal basis using Gram-Schmidt
         for( j = 0; j <= i; ++j ) {
            tmp = 0.0;
            for( l = 0; l < w->count; ++l ) {
               tmp += w->entries[ l ].val * vul_linalg_matrix_get( V, j, w->entries[ l ].idx );
            }
            vul_linalg_matrix_insert( H, j, i, tmp );
            for( l = 0; l < V->rows[ j ].vec.count; ++l ) {
               vul_linalg_vector_insert( w, V->rows[ j ].vec.entries[ l ].idx,
                                         vul_linalg_vector_get( w, V->rows[ j ].vec.entries[ l ].idx )
                                       - tmp * V->rows[ j ].vec.entries[ l ].val );
            }
         }
         tmp = vulb__sparse_dot( w, w ); tmp = sqrt( tmp );
         vul_linalg_matrix_insert( H, i + 1, i, tmp );
         for( j = 0; j < w->count; ++j ) {
            vul_linalg_matrix_insert( V, i + 1, w->entries[ j ].idx, w->entries[ j ].val / tmp );
         }

         // Apply givens rotation to H to form R part of QR factorization in H
         for( j = 0; j < i; ++j ) {
            tmp = cosines[ j ] * vul_linalg_matrix_get( H, j,     i )
                + sines[ j ]   * vul_linalg_matrix_get( H, j + 1, i );
            vul_linalg_matrix_insert( H, j + 1, i, cosines[ j ] * vul_linalg_matrix_get( H, j,     i )
                                                 + sines[ j ]   * vul_linalg_matrix_get( H, j + 1, i ) );
            vul_linalg_matrix_insert( H, j, i, tmp );
         }

         // Calculate rotation matrix (and thus update the Q part of the QR factorization)
         v0 = vul_linalg_matrix_get( H, i,     i );
         v1 = vul_linalg_matrix_get( H, i + 1, i );
         if( v1 == 0.0 ) {
            cosines[ i ] = 1.0;
            sines[ i ] = 0.0;
         } else if( fabs( v1 ) > fabs( v0 ) ) {
            tmp = v0 / v1;
            sines[ i ] = 1.0 / sqrt( 1.0 + tmp * tmp );
            cosines[ i ] = tmp * sines[ i ];
         } else {
            tmp = v1 / v0;
            cosines[ i ] = 1.0 / sqrt( 1.0 + tmp * tmp );
            sines[ i ] = tmp * cosines[ i ];
         }

         // Approximate residual norm
         tmp = cosines[ i ] * vul_linalg_vector_get( s, i );
         vul_linalg_vector_insert( s, i + 1, -sines[ i ] * vul_linalg_vector_get( s, i ) );
         vul_linalg_vector_insert( s, i, tmp );
         vul_linalg_matrix_insert( H, i, i, cosines[ i ] * vul_linalg_matrix_get( H, i,     i )
                                          + sines[ i ]   * vul_linalg_matrix_get( H, i + 1, i ) );
         vul_linalg_matrix_insert( H, i + 1, i, 0.0 );
         err = fabs( vul_linalg_vector_get( s, i ) ) / bd;
         printf("r-iter %d, err %e\n", i, err);
         if( err <= tolerance ) {
            // Update x by solving Hy=s and adding y to x
            vulb__sparse_backward_substitute_submatrix( y, H, s, i+1, i+1 );
            for( j = 0, l = -1; j < V->count; ++j ) {
               if( V->rows[ j ].idx == i ) {
                  l = j;
                  break;
               }
            }
            if( l == -1 ) {
               ERR( "GMRES has encountered an all-zero orthonormal basis, which isn't really possible. Is the"
                    " matrix singular? Returning current estimate (likely wrong)." );
               return x;
            }
            for( j = 0; j < V->rows[ l ].vec.count && V->rows[ l ].vec.entries[ j ].idx < i; ++j ) {
               vul_linalg_vector_insert( x, V->rows[ l ].vec.entries[ j ].idx, 
                                         vul_linalg_vector_get( x, V->rows[ l ].vec.entries[ j ].idx )
                                       - V->rows[ l ].vec.entries[ j ].val 
                                       * vul_linalg_vector_get( y, V->rows[ l ].vec.entries[ j ].idx ) );
            }
            break;
         }
      }

      // Check if done!
      if( err <= tolerance ) {
         break; // We converged!
      }

      // Update x by solving Hy=s and adding y to x
      vulb__sparse_backward_substitute_submatrix( y, H, s, restart_interval, restart_interval );
      vulb__sparse_mmul_submatrix( r, V, y, restart_interval, restart_interval );
      vul__linalg_precondition_solve( ptype, y, P, r, VUL__LINALG_PRECONDITION_RIGHT );
      vulb__sparse_vadd( x, x, y );

      // Update residual
      vulb__sparse_mmul( r, A, x );
      vulb__sparse_vsub( w, b, r );
      vul__linalg_precondition_solve( ptype, r, P, w, VUL__LINALG_PRECONDITION_LEFT );
      rd = vulb__sparse_dot( r, r ); rd = sqrt( rd );
      vul_linalg_vector_insert( s, i + 1, rd );
      err = rd / bd;
      if( err <= tolerance ) {
         break; // We converged!
      }
   }
   if( err > tolerance ) {
      printf("Filed to converge to tolerance in GMRES\n");
      //ERR( "Failed to converge in GMRES!" ); // @TODO(thynn): This is the wrong way to signal this; find a better way!
   }

   vul_linalg_matrix_destroy( V );
   vul_linalg_matrix_destroy( H );
   vul_linalg_vector_destroy( r );
   vul_linalg_vector_destroy( s );
   vul_linalg_vector_destroy( w );
   vul_linalg_vector_destroy( e );
   vul_linalg_vector_destroy( y );
   VUL_LINALG_FREE( cosines );
   VUL_LINALG_FREE( sines );

   return x;
}

vul_linalg_vector *vul_linalg_successive_over_relaxation_sparse( vul_linalg_matrix *A,
                                                                 vul_linalg_vector *initial_guess,
                                                                 vul_linalg_vector *b,
                                                                 vul_linalg_real relaxation_factor,
                                                                 int max_iterations,
                                                                 vul_linalg_real tolerance )
{
   vul_linalg_vector *x, *r;
   int i, j, k;
   vul_linalg_real omega, rd, rd2, tmp;

   r = vul_linalg_vector_create( 0, 0, 0 );
   x = vul_linalg_vector_create( 0, 0, 0 );
      
   /* Calculate initial residual */
   vulb__sparse_vcopy( x, initial_guess );
   vulb__sparse_mmul( r, A, x );
   vulb__sparse_vsub( r, b, r );
   rd = vulb__sparse_dot( r, r );
      
   for( k = 0; k < max_iterations; ++k ) {
      /* Relax */
      for( i = 0; i < A->count; ++i ) {
         omega = 0.f;
         for( j = 0; j < x->count; ++j ) {
            if( A->rows[ i ].idx != x->entries[ j ].idx ) {
               omega += vul_linalg_matrix_get( A, A->rows[ i ].idx, x->entries[ j ].idx ) * x->entries[ j ].val;
            }
         }
         tmp = ( 1.f - relaxation_factor ) * vul_linalg_vector_get( x, A->rows[ i ].idx ) 
             + ( relaxation_factor / vul_linalg_matrix_get( A, A->rows[ i ].idx, A->rows[ i ].idx ) )
             * ( vul_linalg_vector_get( b, A->rows[ i ].idx ) - omega );
         if( tmp != 0.f ) {
            vul_linalg_vector_insert( x, A->rows[ i ].idx, tmp );
         }
      }
      /* Check for convergence */
      vulb__sparse_mmul( r, A, x );
      vulb__sparse_vsub( r, b, r );
      rd2 = vulb__sparse_dot( r, r );
      if( fabs( rd2 - rd ) < tolerance * x->count ) {
         break;
      }
      rd = rd2;
   }
   
   vul_linalg_vector_destroy( r );
   return x;
}

void vul_linalg_lu_decomposition_sparse( vul_linalg_matrix **LU,
                                         int *indices,
                                         vul_linalg_matrix *A,
                                         int cols, int rows )
{
   vul_linalg_vector *scale;
   vul_linalg_real sum, tmp, largest;
   int i, j, k, imax;

   *LU = vul_linalg_matrix_create( 0, 0, 0, 0 );
   scale = vul_linalg_vector_create( 0, 0, 0 );
   
   /* Crout's LUP decomposition with pivoting and scaling */
   for( i = 0; i < A->count; ++i ) {
      largest = 0.f;
      for( j = 0; j < A->rows[ i ].vec.count; ++j ) {
         if( ( tmp = fabs( A->rows[ i ].vec.entries[ j ].val ) ) > largest ) {
            largest = tmp;
         }
      }
      if( largest == 0.f ) {
         ERR( "LU decomposition is not valid for singular matrices." );
         return;
      }
      vul_linalg_vector_insert( scale, A->rows[ i ].idx, 1.f / largest );
   }
   for( j = 0; j < cols; ++j ) {
      for( i = 0; i < j; ++i ) {
         sum = vul_linalg_matrix_get( A, i, j );
         for( k = 0; k < i; ++k ) {
            sum -= vul_linalg_matrix_get( *LU, i, k ) * vul_linalg_matrix_get( *LU, k, j );
         }
         vul_linalg_matrix_insert( *LU, i, j, sum );
      }

      largest = 0.f;
      for( i = j; i < rows; ++i ) {
         sum = vul_linalg_matrix_get( A, j, i );
         for( k = 0; k < j; ++k ) {
            sum -= vul_linalg_matrix_get( *LU, i, k ) * vul_linalg_matrix_get( *LU, k, j );
         }
         vul_linalg_matrix_insert( *LU, i, j, sum );
         if( ( tmp = vul_linalg_vector_get( scale, i ) * fabs( sum ) ) >= largest ) {
            largest = tmp;
            imax = i;
         }
      }
      if( j != imax ) { 
         for( k = 0; k < rows; ++k ) {
            tmp = vul_linalg_matrix_get( *LU, imax, k );
            vul_linalg_matrix_insert( *LU, imax, k, vul_linalg_matrix_get( *LU, j, k ) );
            vul_linalg_matrix_insert( *LU, j, k, tmp );
         }
         vul_linalg_vector_insert( scale, imax, vul_linalg_vector_get( scale, j ) );
      }
      indices[ j ] = imax;
      if( vul_linalg_matrix_get( *LU, j, j ) == 0.f ) {
         ERR( "Pivot element is close enough to zero that we're singular." );
         return;
      }
      if( j != cols - 1 ) {
         tmp = 1.f / vul_linalg_matrix_get( *LU, j, j );
         for( i = j + 1; i < rows; ++i ) {
            vul_linalg_matrix_insert( *LU, i, j, vul_linalg_matrix_get( *LU, i, j ) * tmp );
         }
      }
   }
   
   vul_linalg_vector_destroy( scale );
}

vul_linalg_vector *vul_linalg_lu_solve_sparse( vul_linalg_matrix *LU,
                                               int *indices,
                                               vul_linalg_matrix *A,
                                               vul_linalg_vector *initial_guess,
                                               vul_linalg_vector *b,
                                               int cols, int rows,
                                               int max_iterations,
                                               vul_linalg_real tolerance )
{
   vul_linalg_vector *x, *r, *y;
   vul_linalg_real rd, rd2, sum;
   int k, i, j, imax, iold;

   r = vul_linalg_vector_create( 0, 0, 0 );
   x = vul_linalg_vector_create( 0, 0, 0 );
   
   /* Calculate initial residual */
   vulb__sparse_vcopy( x, initial_guess );
   vulb__sparse_mmul( r, A, x );
   vulb__sparse_vsub( r, b, r );
   rd = vulb__sparse_dot( r, r );

   for( k = 0; k < max_iterations; ++k ) {
      /* Solve Ly = r (solve for the residual error, not b) */
      for( i = 0, iold = 0; i < cols; ++i ) {
         imax = indices[ i ];
         sum = vul_linalg_vector_get( r, imax );
         vul_linalg_vector_insert( r, imax, vul_linalg_vector_get( r, i ) );
         if( iold ) {
            for( j = iold; j < i - 1; ++j ) {
               sum -= vul_linalg_matrix_get( LU, i, j ) * vul_linalg_vector_get( r, j );
            }
         } else if( sum ) {
            iold = i;
         }
         vul_linalg_vector_insert( r, i, sum );
      }
      /* Solve Ue = y (reuse r as e) */
      vulb__sparse_backward_substitute( r, LU, r );

      /* Add the error from the old solution */
      vulb__sparse_vadd( x, x, r );

      /* Break if within tolerance */
      rd2 = vulb__sparse_dot( r, r );
      if( fabs( rd2 - rd ) < tolerance * rows ) {
         break;
      }
      /* Calculate new residual */
      vulb__sparse_mmul( r, A, x );
      vulb__sparse_vsub( r, b, r );
      rd = rd2;
   }
   
   vul_linalg_vector_destroy( r );

   return x;
}

void vul_linalg_cholesky_decomposition_sparse( vul_linalg_matrix **L,
                                               vul_linalg_matrix **LT,
                                               vul_linalg_matrix *A,
                                               int cols, int rows )
{
   vul_linalg_vector rowi, rowj;
   vul_linalg_real sum, rd, rd2;
   int i, j, ki, kj, k;

   *L = vul_linalg_matrix_create( 0, 0, 0, 0 );
   *LT = vul_linalg_matrix_create( 0, 0, 0, 0 );

   for( i = 0; i < A->count; ++i ) {
      for( j = 0; j < A->rows[ i ].vec.count; ++j ) {
         vul_linalg_matrix_insert( *L, A->rows[ i ].idx, 
                                       A->rows[ i ].vec.entries[ j ].idx,
                                       A->rows[ i ].vec.entries[ j ].val );
      }
   }

   // Decomposition into L and L^T
   for( i = 0; i < cols; ++i ) {
      for( j = i; j < rows; ++j ) {
         sum = vul_linalg_matrix_get( A, i, j );
         rowi = vulb__linalg_matrix_get_row_by_array_index( *L, i );
         rowj = vulb__linalg_matrix_get_row_by_array_index( *L, j );
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
            if( sum <= 0.f ) {
               ERR( "Cholesky decomposition is only valid for POSITIVE-DEFINITE symmetric matrices." );
               return;
            }
            vul_linalg_matrix_insert( *L, i, i, sqrt( sum ) );
         } else {
            rd = vul_linalg_matrix_get( *L, i, i );
            if( rd == 0.f ) {
               ERR( "Determinant is sufficiently small that a divide-by-zero is imminent." );
               return;
            }
            vul_linalg_matrix_insert( *L, j, i, sum / rd );
         }
      }
   }
   vulb__sparse_mtranspose( *LT, *L );
}
vul_linalg_vector *vul_linalg_cholesky_solve_sparse( vul_linalg_matrix *L,
                                                     vul_linalg_matrix *LT,
                                                     vul_linalg_matrix *A,
                                                     vul_linalg_vector *initial_guess,
                                                     vul_linalg_vector *b,
                                                     int cols, int rows,
                                                     int max_iterations,
                                                     vul_linalg_real tolerance )
{
   vul_linalg_vector *x, *r, *y;
   vul_linalg_real rd, rd2;
   int k;

   r = vul_linalg_vector_create( 0, 0, 0 );
   y = vul_linalg_vector_create( 0, 0, 0 );
   x = vul_linalg_vector_create( 0, 0, 0 );
   
   /* Calculate initial residual */
   vulb__sparse_vcopy( x, initial_guess );
   vulb__sparse_mmul( r, A, x );
   vulb__sparse_vsub( r, b, r );
   rd = vulb__sparse_dot( r, r );

   for( k = 0; k < max_iterations; ++k ) {
      /* Solve Ly = r */
      vulb__sparse_forward_substitute( y, L, r );
      /* Solve L^Tr = y */
      vulb__sparse_backward_substitute( r, LT, y );

      /* Subtract the error from the old solution */
      vulb__sparse_vadd( x, x, r );

      /* Break if within tolerance */
      rd2 = vulb__sparse_dot( r, r );
      if( fabs( rd2 - rd ) < tolerance * rows ) {
         break;
      }
      /* Calculate new residual */
      vulb__sparse_mmul( r, A, x );
      vulb__sparse_vsub( r, b, r );
      rd = rd2;
   }
   
   vul_linalg_vector_destroy( r );
   vul_linalg_vector_destroy( y );

   return x;
}

static void vul__linalg_givens_rotate_sparse( vul_linalg_matrix *A, int c, int r, 
                                              int i, int j, float cosine, float sine,
                                              int post_multiply )
{
   int k;
   vul_linalg_real G[ 4 ], v0, v1;

   G[ 0 ] = cosine;
   G[ 1 ] = sine;
   G[ 2 ] = -sine;
   G[ 3 ] = cosine;

   if( post_multiply ) {
      // R = G^T * R
      for( k = 0; k < c; ++k ) {
         v0 = G[ 0 ] * vul_linalg_matrix_get( A, i, k ) 
            + G[ 2 ] * vul_linalg_matrix_get( A, j, k ); // G[ 2 ]: T is transposed
         v1 = G[ 1 ] * vul_linalg_matrix_get( A, i, k ) 
            + G[ 3 ] * vul_linalg_matrix_get( A, j, k ); // G[ 1 ]: T is transposed
         vul_linalg_matrix_insert( A, i, k, v0 );
         vul_linalg_matrix_insert( A, j, k, v1 );
      }
   } else {
      // Q = Q * G
      for( k = 0; k < r; ++k ) {
         v0 = G[ 0 ] * vul_linalg_matrix_get( A, k, i ) 
            + G[ 2 ] * vul_linalg_matrix_get( A, k, j );
         v1 = G[ 1 ] * vul_linalg_matrix_get( A, k, i) 
            + G[ 3 ] * vul_linalg_matrix_get( A, k, j );
         vul_linalg_matrix_insert( A, k, i, v0 );
         vul_linalg_matrix_insert( A, k, j, v1 );
      }
   }
}

static void vul__linalg_qr_decomposition_givens_sparse( vul_linalg_matrix *Q, vul_linalg_matrix *R, 
                                                        vul_linalg_matrix *A, int c, int r )
{
   int i, j, k;
   vul_linalg_real theta, st, ct, G[ 4 ], v0, v1, *RA;

   vulb__sparse_mcopy( R, A );
   vulb__sparse_mclear( Q );

   for( i = 0; i < r; ++i ) {
      vul_linalg_matrix_insert( Q, i, i, 1.f );
   }
   
   for( j = 0; j < c; ++j ) {
      for( i = r - 2; i >= j; --i ) {
         v0 = vul_linalg_matrix_get( R, i, j );
         v1 = vul_linalg_matrix_get( R, i + 1, j );
         theta = v0 * v0 + v1 * v1;
         if( theta != 0.f ) {
            theta = sqrt( theta );
            ct =  v0 / theta;
            st = -v1 / theta;
         } else {
            st = 0.f;
            ct = 1.f;
         }
         vul__linalg_givens_rotate_sparse( R, c, r, i, i + 1, ct, st, 1 );
         vul__linalg_givens_rotate_sparse( Q, r, r, i, i + 1, ct, st, 0 );
         vulb__sparse_mclean( R );
         vulb__sparse_mclean( Q );
      }
   }
}

void vul_linalg_qr_decomposition_sparse( vul_linalg_matrix **Q,
                                         vul_linalg_matrix **R,
                                         vul_linalg_matrix *A,
                                         int cols, int rows )
{
   vul_linalg_matrix *QT;
   vul_linalg_vector *x, *d;

   *Q = vul_linalg_matrix_create( 0, 0, 0, 0 );
   *R = vul_linalg_matrix_create( 0, 0, 0, 0 );
   QT = vul_linalg_matrix_create( 0, 0, 0, 0 );

   vul__linalg_qr_decomposition_givens_sparse( QT, *R, A, cols, rows );
   vulb__sparse_mtranspose( *Q, QT );

   vul_linalg_matrix_destroy( QT );
}

vul_linalg_vector *vul_linalg_qr_solve_sparse( vul_linalg_matrix *Q,
                                               vul_linalg_matrix *R,
                                               vul_linalg_matrix *A,
                                               vul_linalg_vector *initial_guess,
                                               vul_linalg_vector *b,
                                               int cols, int rows,
                                               int max_iterations,
                                               vul_linalg_real tolerance )
{
   vul_linalg_vector *x, *d, *r;
   vul_linalg_real rd, rd2;
   int k;

   r = vul_linalg_vector_create( 0, 0, 0 );
   d = vul_linalg_vector_create( 0, 0, 0 );
   x = vul_linalg_vector_create( 0, 0, 0 );
   
   /* Calculate initial residual */
   vulb__sparse_vcopy( x, initial_guess );
   vulb__sparse_mmul( r, A, x );
   vulb__sparse_vsub( r, b, r );
   rd = vulb__sparse_dot( r, r );

   for( k = 0; k < max_iterations; ++k ) {
      /* Calculate d = Qr */
      vulb__sparse_mmul( d, Q, r );
      
      /* Solve Rr = d */
      vulb__sparse_backward_substitute( r, R, d );

      /* Subtract the error from the old solution */
      vulb__sparse_vadd( x, x, r );

      /* Break if within tolerance */
      rd2 = vulb__sparse_dot( r, r );
      if( fabs( rd2 - rd ) < tolerance * rows ) {
         break;
      }
      /* Calculate new residual */
      vulb__sparse_mmul( r, A, x );
      vulb__sparse_vsub( r, b, r );
      rd = rd2;
   }
   
   vul_linalg_vector_destroy( r );
   vul_linalg_vector_destroy( d );

   return x;
}

//-------------------------------------------------------------
// Sparse singular value decomposition local functions
//

static vul_linalg_real vul__linalg_matrix_norm_diagonal_sparse( vul_linalg_matrix *A )
{
   vul_linalg_real v;
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

static vul_linalg_real vul__linalg_matrix_norm_as_single_column_sparse( vul_linalg_matrix *A, int upper_diag )
{
   vul_linalg_real v;
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

vul_linalg_matrix *vul_linalg_svd_basis_reconstruct_matrix_sparse( vul_linalg_svd_basis_sparse *x, int n )
{
   int i, j, k, l;
   vul_linalg_matrix *M;

   M = vul_linalg_matrix_create( 0, 0, 0, 0 );
   if( !n ) {
      return M;
   }

   // We sum from smallest to largest sigma for numerical stability
   for( k = n - 1; k >= 0; --k ) {
      for( i = 0; i < x[ k ].u_length; ++i ) {
         for( j = 0; j < x[ k ].v_length; ++j ) {
            vul_linalg_matrix_insert( M, i, j, vul_linalg_matrix_get( M, i, j ) + 
                                               x[ k ].sigma * vul_linalg_vector_get( x[ k ].u, i )
                                                            * vul_linalg_vector_get( x[ k ].v, j ) );
         }
      }
   }
   return M;
}

void vul_linalg_svd_basis_destroy_sparse( vul_linalg_svd_basis_sparse *x, int n )
{
   int i;

   for( i = 0; i < n; ++i ) {
      if( x[ i ].u ) {
         vul_linalg_vector_destroy( x[ i ].u );
         x[ i ].u = 0;
      }
      if( x[ i ].v ) {
         vul_linalg_vector_destroy( x[ i ].v );
         x[ i ].v = 0;
      }
   }
}

static void vul__linalg_svd_sort_sparse( vul_linalg_svd_basis_sparse *x, int n )
{
   // Shell sort, gaps for up to about a 10M singular values. Good luck exceeding that with this solver!
   vul_linalg_svd_basis_sparse tmp;
   int gap, gi, i, j;
   int gaps[ ] = { 4071001, 1170001, 237001, 67001, 17001, 5001, 1701, 701, 301, 132, 67, 23, 10, 4, 1 };

   for( gi = 0; gi < sizeof( gaps ) / sizeof( int ); ++gi ) {
      for( i = 0 + gaps[ gi ]; i < n; ++i ) {
         gap = gaps[ gi ];
         memcpy( &tmp, &x[ i ], sizeof( vul_linalg_svd_basis_sparse ) );
         for( j = i; j >= gap && x[ j - gap ].sigma <= tmp.sigma; j -= gap ) {
            memcpy( &x[ j ], &x[ j - gap ], sizeof( vul_linalg_svd_basis_sparse ) );
         }
         memcpy( &x[ j ], &tmp, sizeof( vul_linalg_svd_basis_sparse ) );
      }
   }
}

vul_linalg_real vul_linalg_largest_eigenvalue_sparse( vul_linalg_matrix *A, int c, int r, 
                                                      int max_iter, vul_linalg_real eps )
{
   int iter, axis, normaxis, i, j;
   vul_linalg_vector *v, *y;
   vul_linalg_real lambda, norm, err, tmp;

   // Power method. Slow, but simple
   v = vul_linalg_vector_create( 0, 0, 0 );
   y = vul_linalg_vector_create( 0, 0, 0 );
   vul_linalg_vector_insert( v, 0, 1.0 );

   err = eps * 2;
   iter = 0;
   axis = 0;
   lambda = 0.f;
   while( err > eps && iter++ < max_iter ) {
      vulb__sparse_mmul( y, A, v );
      for( i = 0; i < r; ++i ) {
         tmp = 0;
         for( j = 0; j < c; ++j ) {
            vul_linalg_vector_insert( y, i, 0 );
            tmp += vul_linalg_matrix_get( A, i, j ) 
                 * vul_linalg_vector_get( v, j );
         }
         vul_linalg_vector_insert( y, i, tmp );
      }
      err = fabs( lambda - vul_linalg_vector_get( y, axis ) );
      lambda = vul_linalg_vector_get( y, axis );
      norm = -FLT_MAX;
      for( i = 0; i < r; ++i ) {
         if( vul_linalg_vector_get( y, i ) > norm ) {
            norm = vul_linalg_vector_get( y, i );
            normaxis = i;
         }
      }
      axis = normaxis;
      for( i = 0; i < r; ++i ) {
         vul_linalg_vector_insert( v, i, vul_linalg_vector_get( y, i ) / norm );
      }
   }

   vul_linalg_vector_destroy( v );
   vul_linalg_vector_destroy( y );

   return lambda;
}

vul_linalg_real vul_linalg_condition_number_sparse( vul_linalg_matrix *A, int c, int r, 
                                                    int max_iter, vul_linalg_real eps )
{
   vul_linalg_svd_basis_sparse *bases;
   vul_linalg_real ret;
   int n;
   
   n = c > r ? r : c;
   bases = ( vul_linalg_svd_basis_sparse* )VUL_LINALG_ALLOC( sizeof( vul_linalg_svd_basis_sparse ) * n );

   n = 0;
   vul_linalg_svd_sparse( bases, &n, A, c, r, max_iter, eps );
   if( n < 2 ) {
      ERR( "Can't compute condition number, not enough non-zero singular values (need 2)." );
      return 0.0;
   }
   ret = bases[ 0 ].sigma / bases[ n - 1 ].sigma;
   vul_linalg_svd_basis_destroy_sparse( bases, n );
   VUL_LINALG_FREE( bases );

   return ret;
}


void vul_linalg_svd_sparse_qrlq( vul_linalg_svd_basis_sparse *out, int *rank,
                                 vul_linalg_matrix *A,
                                 int c, int r, int itermax, vul_linalg_real eps )
{
   vul_linalg_matrix *U0, *U1, *V0, *V1, *S0, *S1, *Sb, *Q, *tmp;
   vul_linalg_real err, e, f, scale;
   int iter, n, i, j, k, ri, ci;

   n = r > c ? r : c;
   U0 = vul_linalg_matrix_create( 0, 0, 0, 0 );
   U1 = vul_linalg_matrix_create( 0, 0, 0, 0 );
   V0 = vul_linalg_matrix_create( 0, 0, 0, 0 );
   V1 = vul_linalg_matrix_create( 0, 0, 0, 0 );
   S0 = vul_linalg_matrix_create( 0, 0, 0, 0 );
   S1 = vul_linalg_matrix_create( 0, 0, 0, 0 );
   Sb = vul_linalg_matrix_create( 0, 0, 0, 0 );
   Q  = vul_linalg_matrix_create( 0, 0, 0, 0 );
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
      vul_linalg_matrix_insert( U0, i, i, 1.f );
   }
   for( i = 0; i < c; ++i ) {
      vul_linalg_matrix_insert( V0, i, i, 1.f );
   }
   while( err > eps && iter++ < itermax ) {
      // Store away old S
      vulb__sparse_mcopy( Sb, S0 );

      // Decompose
      vulb__sparse_mtranspose( S1, S0 );
      vul__linalg_qr_decomposition_givens_sparse( Q, S0, S1, c, r );
      vulb__sparse_mmul_matrix( U1, U0, Q, r );

      vulb__sparse_mtranspose( S1, S0 );
      vul__linalg_qr_decomposition_givens_sparse( Q, S0, S1, r, c );
      vulb__sparse_mmul_matrix( V1, V0, Q, c );

      tmp = U0; U0 = U1; U1 = tmp;
      tmp = V0; V0 = V1; V1 = tmp;

      // Calculate error
      e = vul__linalg_matrix_norm_as_single_column_sparse( S0, 1 );
      f = vul__linalg_matrix_norm_diagonal_sparse( S0 );
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
      out[ i ].sigma = fabs( vul_linalg_matrix_get( S0, i, i ) ) * scale;
      out[ i ].axis = i;
      if( out[ i ].sigma > eps ) {
         ++j;
      }
   }
   if( *rank == 0 || j < *rank ) {
      *rank = j;
   }
   vul__linalg_svd_sort_sparse( out, k );

   // Fix signs and copy U
   for( i = 0; i < *rank; ++i ) {
      out[ i ].u_length = r;
      out[ i ].v_length = c;
      out[ i ].u = vul_linalg_vector_create( 0, 0, 0 );
      out[ i ].v = vul_linalg_vector_create( 0, 0, 0 );
      f = vul_linalg_matrix_get( S0, out[ i ].axis, out[ i ].axis ) < 0.f ? -1.f : 1.f;
      for( j = 0; j < r; ++j ) {
         vul_linalg_vector_insert( out[ i ].u, j, vul_linalg_matrix_get( U0, j, out[ i ].axis ) * f );
      }
      for( j = 0; j < c; ++j ) {
         vul_linalg_vector_insert( out[ i ].v, j, vul_linalg_matrix_get( V0, j, out[ i ].axis ) );
      }
   }

   vul_linalg_matrix_destroy( U0 );
   vul_linalg_matrix_destroy( U1 );
   vul_linalg_matrix_destroy( V0 );
   vul_linalg_matrix_destroy( V1 );
   vul_linalg_matrix_destroy( S0 );
   vul_linalg_matrix_destroy( S1 );
   vul_linalg_matrix_destroy( Sb );
   vul_linalg_matrix_destroy( Q );
}

void vul_linalg_svd_sparse( vul_linalg_svd_basis_sparse *out, int *rank,
                            vul_linalg_matrix *A,
                            int c, int r, int itermax, vul_linalg_real eps )
{
   vul_linalg_matrix *J, *U, *V, *G;
   vul_linalg_vector *omegas;
   vul_linalg_real f, t, vik, vjk, scale, max_diag, threshold;
   int iter, n, m, i, j, k, ri, ci, nonzero;

   n = r > c ? r : c;
   m = r < c ? r : c;
   U = vul_linalg_matrix_create( 0, 0, 0, 0 );
   V = vul_linalg_matrix_create( 0, 0, 0, 0 );
   G = vul_linalg_matrix_create( 0, 0, 0, 0 );
   omegas = vul_linalg_vector_create( 0, 0, 0 );
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
         vul_linalg_matrix_insert( G, A->rows[ i ].idx, A->rows[ i ].vec.entries[ j ].idx, 
                                      A->rows[ i ].vec.entries[ j ].val * f );
      }
   }
   
   // Initialize U and V as identity matrices
   for( i = 0; i < r; ++i ) {
      vul_linalg_matrix_insert( U, i, i, 1.f );
   }
   for( i = 0; i < c; ++i ) {
      vul_linalg_matrix_insert( V, i, i, 1.f );
   }
   max_diag = 1.0; // Matrix is scaled

   while( nonzero && iter++ < itermax ) {
      nonzero = 0;
      for( i = 0; i < r - 1; ++i ) {
         for( j = i + 1; j < r; ++j ) {
            vul_linalg_real aii, aij, ajj;
            
            // Skip if already diagonal (or as close as we can get)
#ifdef __FLT_DENORM_MIN__
            threshold = eps * max_diag < __FLT_DENORM_MIN__ ? __FLT_DENORM_MIN__ : eps * max_diag;
#else
            threshold = eps * max_diag < FLT_MIN ? FLT_MIN : eps * max_diag;
#endif
            if( !( fabs( vul_linalg_matrix_get( G, i, j ) ) > threshold
                || fabs( vul_linalg_matrix_get( G, j, i ) ) > threshold ) ) {
               continue;
            }

            // Compute a_ij, a_ji, a_ii
            aii = 0.0; aij = 0.0; ajj = 0.0;
            for( k = 0; k < c; ++k ) {
               vik = vul_linalg_matrix_get( G, i, k );
               vjk = vul_linalg_matrix_get( G, j, k );

               aii += vik * vik;
               ajj += vjk * vjk;
               aij += vik * vjk;
            }
            if( fabs( aij ) > eps ) {
               nonzero += 1;
               vul_linalg_real tau, t, ct, st;
               tau = ( aii - ajj ) / ( 2.0 * aij );
               t = copysign( 1.0 / ( fabs( tau ) + sqrt( 1.0 + tau * tau ) ), tau );
               ct = 1.0 / sqrt( 1.0 + t * t );
               st = ct * t;
               vul__linalg_givens_rotate_sparse( G, c, r, j, i, ct, st, 1 );
               if( j < r ) {
                  vul__linalg_givens_rotate_sparse( U, r, r, j, i, ct, st, 0 );
               }

               // Keep track of largest diagonal entry
               aii = vul_linalg_matrix_get( G, i, i );
               ajj = vul_linalg_matrix_get( G, j, j );
               threshold = aii > ajj ? aii : ajj;
               max_diag = max_diag > threshold ? max_diag : threshold;
            }
         }
      }
   }

   // Calculate the singular values (2-norm of the columns of G)
   for( i = 0; i < r; ++i ) {
      t = 0.0;
      for( j = 0; j < c; ++j ) {
         f = vul_linalg_matrix_get( G, i, j );
         t += f * f;
      }
      vul_linalg_vector_insert( omegas, i, sqrt( t ) );
   }

   // Calculate V
   for( i = 0; i < c; ++i ) { // The rest is zero
      if( fabs( vul_linalg_vector_get( omegas, i ) ) > eps ) { // Ignore zero singular values
         for( j = 0; j < c; ++j ) {
            vul_linalg_matrix_insert( V, j, i, vul_linalg_matrix_get( G, i, j ) / 
                                               vul_linalg_vector_get( omegas, i ) );
         }
      }
   }

   // Grap sigmas and rank, sort decreasing
   k = r < c ? r : c;
   for( j = 0, i = 0; i < c; ++i ) {
      out[ i ].sigma = fabs( vul_linalg_vector_get( omegas, i ) ) * scale;
      out[ i ].axis = i;
      if( out[ i ].sigma > eps ) {
         ++j;
      }
   }
   if( *rank == 0 || j < *rank ) {
      *rank = j;
   }
   vul__linalg_svd_sort_sparse( out, k );

   // Fix signs and copy U
   for( i = 0; i < *rank; ++i ) {
      out[ i ].u_length = r;
      out[ i ].v_length = c;
      out[ i ].u = vul_linalg_vector_create( 0, 0, 0 );
      out[ i ].v = vul_linalg_vector_create( 0, 0, 0 );
      f = vul_linalg_vector_get( omegas, out[ i ].axis ) < 0.f ? -1.f : 1.f;
      for( j = 0; j < r; ++j ) {
         vul_linalg_vector_insert( out[ i ].u, j, vul_linalg_matrix_get( U, j, out[ i ].axis ) * f );
      }
      for( j = 0; j < c; ++j ) {
         vul_linalg_vector_insert( out[ i ].v, j, vul_linalg_matrix_get( V, j, out[ i ].axis ) );
      }
   }

   vul_linalg_matrix_destroy( U );
   vul_linalg_matrix_destroy( V );
   vul_linalg_matrix_destroy( G );
   vul_linalg_vector_destroy( omegas );
}

vul_linalg_vector *vul_linalg_linear_least_squares_sparse( vul_linalg_svd_basis_sparse *bases,
                                                           int rank,
                                                           vul_linalg_vector *b )
{
   vul_linalg_vector *d, *out;
   vul_linalg_real v;
   int i, j;

   d = vul_linalg_vector_create( 0, 0, 0 );
   out = vul_linalg_vector_create( 0, 0, 0 );

   // Calculate d = U^T * b
   for( i = 0; i < rank; ++i ) {
      v = 0.0;
      for( j = 0; j < bases[ i ].u->count; ++j ) {
         v += bases[ i ].u->entries[ j ].val * vul_linalg_vector_get( b, bases[ i ].u->entries[ j ].idx );
      }
      vul_linalg_vector_insert( d, bases[ i ].axis, v );
   }

   // Calculate x = V * S^+ * d
   for( i = 0; i < rank; ++i ) {
      for( j = 0; j < bases[ i ].v->count; ++j ) {
         vul_linalg_vector_insert( out, bases[ i ].v->entries[ j ].idx, 
                                   vul_linalg_vector_get( out, bases[ i ].v->entries[ j ].idx )
                                 + bases[ i ].v->entries[ j ].val 
                                 * vul_linalg_vector_get( d, bases[ i ].axis )
                                 / bases[ i ].sigma );
      }
   }
   
   vul_linalg_vector_destroy( d );

   return out;
}


//------------------------
// Dense local functions
//

// @NOTE(thynn): The reason we index y over x is that I fucked up, and initially had them mixed up,
// and once I discovered the bug I had been consistent enough that simply swapping indices meant it all worked.
// So now we index (y,x) instead of (x,y). Oops. But it's only internal in this file anyway.
#ifdef VUL_LINALG_ROW_MAJOR
#define IDX( A, y, x, c, r ) A[ ( y ) * ( c ) + ( x ) ]
#else
#define IDX( A, y, x, c, r ) A[ ( x ) * ( r ) + ( y ) ]
#endif

#define DEFINE_VECTOR_OP( name, op )\
   static void name( vul_linalg_real *out, vul_linalg_real *a, vul_linalg_real *b, int n )\
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

static inline void vulb__swap_ptr( vul_linalg_real **a, vul_linalg_real **b )
{
   vul_linalg_real *t = *a;
   *a = *b;
   *b = t;
}

static void vulb__vmul_sub( vul_linalg_real *out, vul_linalg_real *a, vul_linalg_real x, vul_linalg_real *b, int n )
{
   int i;
   for( i = 0; i < n; ++i ) {
      out[ i ] = a[ i ] * x - b[ i ];
   }
}

static void vulb__vmul_add( vul_linalg_real *out, vul_linalg_real *a, vul_linalg_real x, vul_linalg_real *b, int n )
{
   int i;
   for( i = 0; i < n; ++i ) {
      out[ i ] = a[ i ] * x + b[ i ];
   }
}
      

static void vulb__vcopy( vul_linalg_real *out, vul_linalg_real *x, int n )
{
   int i;
   for( i = 0; i < n; ++i ) {
      out[ i ] = x[ i ];
   }
}

static vul_linalg_real vulb__dot( vul_linalg_real *a, vul_linalg_real *b, int n )
{
   vul_linalg_real f;
   int i;

   f = a[ 0 ] * b[ 0 ];
   for( i = 1; i < n; ++i ) {
      f += a[ i ] * b[ i ];
   }
   return f;
}

static void vulb__mmul( vul_linalg_real *out, vul_linalg_real *A, vul_linalg_real *x, int c, int r )
{
   int i, j;
   for( i = 0; i < r; ++i ) {
      out[ i ] = 0;
      for( j = 0; j < c; ++j ) {
         out[ i ] += IDX( A, i, j, c, r ) * x[ j ];
      }
   }
}
static void vulb__mmul_matrix( vul_linalg_real *O, vul_linalg_real *A, vul_linalg_real *B, int n )
{
   vul_linalg_real s;
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

static void vulb__mmul_add( vul_linalg_real *out, vul_linalg_real *A, vul_linalg_real *x, vul_linalg_real *b, 
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

static void vulb__forward_substitute( vul_linalg_real *out, vul_linalg_real *A, vul_linalg_real *b, int c, int r )
{
   int i, j;

   for( i = 0; i < r; ++i ) {
      vul_linalg_real sum = b[ i ];
      for( j = i - 1; j >= 0; --j ) {
         sum -= IDX( A, i, j, c, r ) * out[ j ];
      }
      out[ i ] = sum / IDX( A, i, i, c, r );
   }
}

static void vulb__backward_substitute( vul_linalg_real *out, vul_linalg_real *A, vul_linalg_real *b, int c, int r,
                                       int transpose )
{
   int i, j;

   if( transpose ) {
      for( i = c - 1; i >= 0; --i ) {
         vul_linalg_real sum = b[ i ];
         for( j = i + 1; j < r; ++j ) {
            sum -= IDX( A, j, i, c, r ) * out[ j ];
         }
         out[ i ] = sum / IDX( A, i, i, c, r );
      }
   } else {
      for( i = r - 1; i >= 0; --i ) {
         vul_linalg_real sum = b[ i ];
         for( j = i + 1; j < c; ++j ) {
            sum -= IDX( A, i, j, c, r ) * out[ j ];
         }
         out[ i ] = sum / IDX( A, i, i, c, r );
      }
   }
}
static void vulb__mtranspose( vul_linalg_real *O, vul_linalg_real *A, int c, int r )
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
#ifndef VUL_LINALG_ROW_MAJOR
      k = c; c = r; r = k; // Swap the row and column dimensions if column major so we can share logic.
#endif
      for( k = 0; k < r * c; ++k ) {
         i = k % c;
         j = k / c;
         IDX( O, i, j, r, c ) = IDX( A, j, i, c, r );
      }
   }
}
static void vulb__mmul_matrix_rect( vul_linalg_real *O, vul_linalg_real *A, vul_linalg_real *B, int ra, int rb_ca, int cb )
{
   int i, j, k;
   vul_linalg_real d;

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

void vul_linalg_conjugate_gradient_dense( vul_linalg_real *out,
                                          vul_linalg_real *A,
                                          vul_linalg_real *initial_guess,
                                          vul_linalg_real *b,
                                          int n,
                                          int max_iterations,
                                          vul_linalg_real tolerance )
{
   vul_linalg_real *x, *r, *Ap, *p;
   vul_linalg_real rd, rd2, alpha, beta;
   int i, j;

   r = ( vul_linalg_real* )VUL_LINALG_ALLOC( sizeof( vul_linalg_real ) * n );
   p = ( vul_linalg_real* )VUL_LINALG_ALLOC( sizeof( vul_linalg_real ) * n );
   Ap = ( vul_linalg_real* )VUL_LINALG_ALLOC( sizeof( vul_linalg_real ) * n );
   
   x = out;
   vulb__vcopy( x, initial_guess, n );
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

   VUL_LINALG_FREE( p );
   VUL_LINALG_FREE( r );
   VUL_LINALG_FREE( Ap );
}

void vul_linalg_gmres_dense( vul_linalg_real *x,
                             vul_linalg_real *A,
                             vul_linalg_real *initial_guess,
                             vul_linalg_real *b,
                             int n,
                             int restart_interval,
                             int max_iterations,
                             vul_linalg_real tolerance )
{
   vul_linalg_real *V, *H, *r, *e, *y, *s, *w;
   vul_linalg_real bd, rd, err, tmp, *cosines, *sines, v0, v1;
   int i, j, k, l, m;

   r = ( vul_linalg_real* )VUL_LINALG_ALLOC( sizeof( vul_linalg_real ) * n );
   memset( x, 0, sizeof( vul_linalg_real ) * n );
   memset( r, 0, sizeof( vul_linalg_real ) * n );
   vulb__vcopy( x, initial_guess, n );
   vulb__mmul( r, A, x, n, n );
   vulb__vsub( r, b, r, n );
   bd = vulb__dot( b, b, n ); bd = sqrt( bd );
   rd = vulb__dot( r, r, n ); rd = sqrt( rd );

   err = rd / bd;
   if( err <= tolerance ) {
      VUL_LINALG_FREE( r );
      return; // Initial guess is close enough!
   }

   w = ( vul_linalg_real* )VUL_LINALG_ALLOC( sizeof( vul_linalg_real ) * n );
   e = ( vul_linalg_real* )VUL_LINALG_ALLOC( sizeof( vul_linalg_real ) * n );
   s = ( vul_linalg_real* )VUL_LINALG_ALLOC( sizeof( vul_linalg_real ) * ( restart_interval + 2 ) );
   y = ( vul_linalg_real* )VUL_LINALG_ALLOC( sizeof( vul_linalg_real ) * n );
   V = ( vul_linalg_real* )VUL_LINALG_ALLOC( sizeof( vul_linalg_real ) * n * ( restart_interval + 1 ) );
   H = ( vul_linalg_real* )VUL_LINALG_ALLOC( sizeof( vul_linalg_real ) * restart_interval * ( restart_interval + 1 ) );
   cosines = ( vul_linalg_real* )VUL_LINALG_ALLOC( sizeof( vul_linalg_real ) * restart_interval );
   sines   = ( vul_linalg_real* )VUL_LINALG_ALLOC( sizeof( vul_linalg_real ) * restart_interval );
   memset( w, 0, sizeof( vul_linalg_real ) * n );
   memset( e, 0, sizeof( vul_linalg_real ) * n );
   memset( s, 0, sizeof( vul_linalg_real ) * ( restart_interval + 2 ) );
   memset( y, 0, sizeof( vul_linalg_real ) * n );
   memset( V, 0, sizeof( vul_linalg_real ) * n * ( restart_interval + 1 ) );
   memset( H, 0, sizeof( vul_linalg_real ) * restart_interval * ( restart_interval + 1 ) );
   memset( cosines, 0, sizeof( vul_linalg_real ) * restart_interval );
   memset( sines,   0, sizeof( vul_linalg_real ) * restart_interval );

   e[ 0 ] = 1.0;

   for( k = 0; k < max_iterations; ++k ) {
      // v_1 = r / norm( r )
      for( i = 0; i < n; ++i ) {
         V[ i ] = r[ i ] / rd;
      }
      // s = norm(r)*e
      for( i = 0; i < n; ++i ) {
         s[ i ] = e[ i ] * rd;
      }

      for( i = 0; i < restart_interval; ++i ) {
         vulb__mmul( w, A, &V[ i * n ], n, n );

         // Construct orthonormal basis using Gram-Schmidt
         for( j = 0; j <= i; ++j ) {
            tmp = 0.0;
            for( l = 0; l < n; ++l ) {
               tmp += w[ l ] * V[ j * n + l ];
            }
            H[ j * restart_interval + i ] = tmp;
            for( l = 0; l < n; ++l ) {
               w[ l ] -= tmp * V[ j * n + l ];
            }
         }
         tmp = vulb__dot( w, w, n ); tmp = sqrt( tmp );
         H[ ( i + 1 ) * restart_interval + i ] = tmp;
         for( j = 0; j < n; ++j ) {
            V[ ( i + 1 ) * n + j ] = w[ j ] / tmp;
         }

         // Apply givens rotation to H to form R part of QR factorization in H
         for( j = 0; j < i; ++j ) {
            tmp = cosines[ j ] * H[   j       * restart_interval + i ]
                + sines[ j ]   * H[ ( j + 1 ) * restart_interval + i ];
            H[ ( j + 1 ) * restart_interval + i ] = 
                  cosines[ j ] * H[   j       * restart_interval + i ]
                + sines[ j ]   * H[ ( j + 1 ) * restart_interval + i ];
            H[ j * restart_interval + i ] = tmp;
         }

         // Calculate rotation matrix (and thus update the Q part of the QR factorization)
         v0 = H[   i       * restart_interval + i ];
         v1 = H[ ( i + 1 ) * restart_interval + i ];
         if( v1 == 0.0 ) {
            cosines[ i ] = 1.0;
            sines[ i ] = 0.0;
         } else if( fabs( v1 ) > fabs( v0 ) ) {
            tmp = v0 / v1;
            sines[ i ] = 1.0 / sqrt( 1.0 + tmp * tmp );
            cosines[ i ] = tmp * sines[ i ];
         } else {
            tmp = v1 / v0;
            cosines[ i ] = 1.0 / sqrt( 1.0 + tmp * tmp );
            sines[ i ] = tmp * cosines[ i ];
         }

         // Approximate residual norm
         tmp = cosines[ i ] * s[ i ];
         s[ i + 1 ] = -sines[ i ] * s[ i ];
         s[ i ] = tmp;
         H[ i * restart_interval + i ] = 
               cosines[ i ] * H[   i       * restart_interval + i ]
             + sines[ i ]   * H[ ( i + 1 ) * restart_interval + i ];
         H[ ( i + 1 ) * restart_interval + i ] = 0.0;
         err = fabs( s[ i ] ) / bd;
         if( err <= tolerance ) {
            // Update x by solving Hy=s and adding y to x
            // we do this by backward substitution (without the helper functions since H is always ordered
            // our way, and has stride different from it's maximal size.
            for( l = i; l >= 0; --l ) {
               tmp = s[ l ];
               for( m = l + 1; m < i + 1; ++m ) {
                  tmp -= H[ l * restart_interval + m ] * y[ m ];
               }
               y[ l ] = tmp / H[ l * restart_interval + l ];
            }
            for( j = 0; j < i; ++j ) {
               x[ j ] -= V[ i * n + j ] * y[ j ];
            }
            break;
         }
      }

      // Check if done!
      if( err <= tolerance ) {
         break; // We converged!
      }

      // Update x by solving Hy=s and adding y to x
      // we do this by backward substitution (without the helper functions since H is always ordered
      // our way, and has stride different from it's maximal size.
      for( l = restart_interval - 1; l >= 0; --l ) {
         tmp = s[ l ];
         for( m = l + 1; m < restart_interval; ++m ) {
            tmp -= H[ l * restart_interval + m ] * y[ m ];
         }
         y[ l ] = tmp / H[ l * restart_interval + l ];
      }
      // Multiply out results, again not using the helper function because V's layout is fixed
      for( i = 0; i < restart_interval; ++i ) {
         r[ i ] = 0;
         for( j = 0; j < restart_interval; ++j ) {
            r[ i ] += V[ i * n + j ] * y[ j ];
         }
      }
      vulb__vadd( x, x, r, restart_interval );


      // Update residual
      vulb__mmul( r, A, x, n, n );
      vulb__vsub( r, b, r, n );
      rd = vulb__dot( r, r, n ); rd = sqrt( rd );
      s[ i + 1 ] = rd;
      err = rd / bd;
      if( err <= tolerance ) {
         break; // We converged!
      }
   }

   if( err > tolerance ) {
      printf("Filed to converge to tolerance in GMRES\n");
      //ERR( "Failed to converge in GMRES!" ); // @TODO(thynn): This is the wrong way to signal this; find a better way!
   }

   VUL_LINALG_FREE( V );
   VUL_LINALG_FREE( H );
   VUL_LINALG_FREE( r );
   VUL_LINALG_FREE( s );
   VUL_LINALG_FREE( w );
   VUL_LINALG_FREE( e );
   VUL_LINALG_FREE( y );
   VUL_LINALG_FREE( cosines );
   VUL_LINALG_FREE( sines );
}

void vul_linalg_lu_decomposition_dense( vul_linalg_real *LU,
                                        int *indices,
                                        vul_linalg_real *A,
                                        int n )
{
   vul_linalg_real *scale;
   vul_linalg_real sum, tmp, largest;
   int i, j, k, imax;

   scale = ( vul_linalg_real* )VUL_LINALG_ALLOC( sizeof( vul_linalg_real ) * n );
   
   /* Crout's LUP decomposition with pivoting and scaling */
   for( i = 0; i < n; ++i ) {
      largest = 0.f;
      for( j = 0; j < n; ++ j ) {
         if( ( tmp = fabs( IDX( A, i, j, n, n ) ) ) > largest ) {
            largest = tmp;
         }
      }
      if( largest == 0.f ) {
         ERR( "LU decomposition is not valid for singular matrices." );
         return;
      }
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
      if( IDX( LU, j, j, n, n ) == 0.f ) {
         ERR( "Pivot element is close enough to zero that we're singular." );
         return;
      }
      if( j != n - 1 ) {
         tmp = 1.f / IDX( LU, j, j, n, n );
         for( i = j + 1; i < n; ++i ) {
            IDX( LU, i, j, n, n ) *= tmp;
         }
      }
   }
   
   VUL_LINALG_FREE( scale );
}

void vul_linalg_lu_solve_dense( vul_linalg_real *out,
                                vul_linalg_real *LU,
                                int *indices,
                                vul_linalg_real *A,
                                vul_linalg_real *initial_guess,
                                vul_linalg_real *b,
                                int n,
                                int max_iterations,
                                vul_linalg_real tolerance )
{
   vul_linalg_real *x, *r, *scale;
   vul_linalg_real sum, rd, rd2;
   int i, j, k, imax, iold;

   r = ( vul_linalg_real* )VUL_LINALG_ALLOC( sizeof( vul_linalg_real ) * n );
   x = out;
   
   /* Calculate initial residual */
   vulb__vcopy( x, initial_guess, n );
   vulb__mmul( r, A, x, n, n );
   vulb__vsub( r, b, r, n );
   rd = vulb__dot( r, r, n );

   for( k = 0; k < max_iterations; ++k ) {
      /* Solve Ly = r (solve for the residual error, not b) */
      for( i = 0, iold = 0; i < n; ++i ) {
         imax = indices[ i ];
         sum = r[ imax ];
         r[ imax ] = r[ i ];
         if( iold ) {
            for( j = iold; j < i - 1; ++j ) {
               sum -= IDX( LU, i, j, n, n ) * r[ j ];
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
      /* Calculate new residual */
      vulb__mmul( r, A, x, n, n );
      vulb__vsub( r, r, b, n );
      rd = rd2;
   }

   VUL_LINALG_FREE( r );
}

void vul_linalg_cholesky_decomposition_dense( vul_linalg_real *LL,
                                              vul_linalg_real *A,
                                              int n )
{
   vul_linalg_real sum;
   int i, j, k;

   // Copy work matrix
   memcpy( LL, A, sizeof( vul_linalg_real ) * n * n );

   // Decomposition
   for( i = 0; i < n; ++i ) {
      for( j = i; j < n; ++j ) {
         sum = IDX( LL, i, j, n, n );
         for( k = i - 1; k >= 0; --k ) {
            sum -= IDX( LL, i, k, n, n ) * IDX( LL, j, k, n, n );
         }
         if( i == j ) {
            if( sum <= 0.f ) {
               ERR( "Cholesky decomposition is only valid for POSITIVE-DEFINITE symmetric matrices." );
               return;
            }
            IDX( LL, i, i, n, n ) = sqrt( sum );
         } else {
            if( IDX( LL, i, i, n, n ) == 0.f ) {
               ERR( "Determinant is sufficiently small that a divide-by-zero is imminent." );
               return;
            }
            IDX( LL, j, i, n, n ) = sum / IDX( LL, i, i, n, n );
         }
      }
   }
}

void vul_linalg_cholesky_solve_dense( vul_linalg_real *out,
                                      vul_linalg_real *LL,
                                      vul_linalg_real *A,
                                      vul_linalg_real *initial_guess,
                                      vul_linalg_real *b,
                                      int n,
                                      int max_iterations,
                                      vul_linalg_real tolerance )
{
   vul_linalg_real *x, *r, *y;
   vul_linalg_real rd, rd2;
   int i, j, k;

   r = ( vul_linalg_real* )VUL_LINALG_ALLOC( sizeof( vul_linalg_real ) * n );
   y = ( vul_linalg_real* )VUL_LINALG_ALLOC( sizeof( vul_linalg_real ) * n );
   x = out;

   /* Calculate initial residual */
   vulb__vcopy( x, initial_guess, n );
   vulb__mmul( r, A, x, n, n );
   vulb__vsub( r, b, r, n );
   rd = vulb__dot( r, r, n );

   for( k = 0; k < max_iterations; ++k ) {
      /* Solve Ly = r */
      vulb__forward_substitute( y, LL, r, n, n );
      /* Solve L^Tr = y */
      vulb__backward_substitute( r, LL, y, n, n, 1 );

      /* Subtract the error from the old solution */
      vulb__vsub( x, x, r, n );

      /* Break if within tolerance */
      rd2 = vulb__dot( r, r, n );
      if( fabs( rd2 - rd ) < tolerance * n ) {
         break;
      }
      /* Calculate new residual */
      vulb__mmul( r, A, x, n, n );
      vulb__vsub( r, r, b, n );
      rd = rd2;
   }
   
   VUL_LINALG_FREE( r );
   VUL_LINALG_FREE( y );
}

void vul_linalg_qr_decomposition_dense( vul_linalg_real *Q,
                                        vul_linalg_real *R,
                                        vul_linalg_real *A,
                                        int n )
{
   vul__linalg_qr_decomposition_givens( Q, R, A, n, n, 0 );
}

void vul_linalg_qr_solve_dense( vul_linalg_real *out,
                                vul_linalg_real *Q,
                                vul_linalg_real *R,
                                vul_linalg_real *A,
                                vul_linalg_real *initial_guess,
                                vul_linalg_real *b,
                                int n,
                                int max_iterations,
                                vul_linalg_real tolerance )
{
   vul_linalg_real *x, *d, *r, sum, rd, rd2;
   int i, j, k;

   d = ( vul_linalg_real* )VUL_LINALG_ALLOC( sizeof( vul_linalg_real ) * n );
   r = ( vul_linalg_real* )VUL_LINALG_ALLOC( sizeof( vul_linalg_real ) * n );
   x = out;
   
   /* Calculate initial residual */
   vulb__vcopy( x, initial_guess, n );
   vulb__mmul( r, A, x, n, n );
   vulb__vsub( r, b, r, n );
   rd = vulb__dot( r, r, n );

   for( k = 0; k < max_iterations; ++k ) {
      /* Solve Qd = r */
      for( i = 0; i < n; ++i ) {
         sum = 0;
         for( j = 0; j < n; ++j ) {
            sum += IDX( Q, j, i, n, n ) * r[ j ];
         }
         d[ i ] = sum;
      }

      /* Solve Rr = d */
      vulb__backward_substitute( r, R, d, n, n, 0 );

      /* Subtract the error from the old solution */
      vulb__vsub( x, x, r, n );

      /* Break if within tolerance */
      rd2 = vulb__dot( r, r, n );
      if( fabs( rd2 - rd ) < tolerance * n ) {
         break;
      }
      /* Calculate new residual */
      vulb__mmul( r, A, x, n, n );
      vulb__vsub( r, r, b, n );
      rd = rd2;
   }

   VUL_LINALG_FREE( d );
   VUL_LINALG_FREE( r );
}

void vul_linalg_successive_over_relaxation_dense( vul_linalg_real *out,
                                                  vul_linalg_real *A,
                                                  vul_linalg_real *initial_guess,
                                                  vul_linalg_real *b,
                                                  vul_linalg_real relaxation_factor,
                                                  int n,
                                                  int max_iterations,
                                                  vul_linalg_real tolerance )
{
   vul_linalg_real *x, *r;
   int i, j, k;
   vul_linalg_real omega, rd, rd2;

   r = ( vul_linalg_real* )VUL_LINALG_ALLOC( sizeof( vul_linalg_real ) * n );
      
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
   
   VUL_LINALG_FREE( r );
}

//---------------------------------------
// Dense singular value decomposition
// 

void vul_linalg_svd_basis_reconstruct_matrix( vul_linalg_real *M, vul_linalg_svd_basis *x, int n )
{
   int i, j, k, l;

   if( !n ) {
      return;
   }

   memset( M, 0, sizeof( vul_linalg_real ) * x[ 0 ].u_length * x[ 0 ].v_length );

   // Sum from smallest to largest sigma for numerical stability
   for( k = n - 1; k >= 0; --k ) {
      for( i = 0; i < x[ k ].u_length; ++i ) {
         for( j = 0; j < x[ k ].v_length; ++j ) {
            IDX( M, i, j, x[ k ].v_length, x[ k ].u_length ) += x[ k ].sigma * x[ k ].u[ i ] * x[ k ].v[ j ];
         }
      }
   }
}

void vul_linalg_svd_basis_destroy( vul_linalg_svd_basis *x, int n )
{
   int i;

   for( i = 0; i < n; ++i ) {
      if( x[ i ].u ) {
         VUL_LINALG_FREE( x[ i ].u );
         x[ i ].u = 0;
      }
      if( x[ i ].v ) {
         VUL_LINALG_FREE( x[ i ].v );
         x[ i ].v = 0;
      }
   }
}

static void vul__linalg_svd_sort( vul_linalg_svd_basis *x, int n )
{
   // Shell sort, gaps for up to about a 10M singular values.
   vul_linalg_svd_basis tmp;
   int gap, gi, i, j;
   int gaps[ ] = { 4071001, 1170001, 237001, 67001, 17001, 5001, 1701, 701, 301, 132, 67, 23, 10, 4, 1 };

   for( gi = 0; gi < sizeof( gaps ) / sizeof( int ); ++gi ) {
      for( i = 0 + gaps[ gi ]; i < n; ++i ) {
         gap = gaps[ gi ];
         memcpy( &tmp, &x[ i ], sizeof( vul_linalg_svd_basis ) );
         for( j = i; j >= gap && x[ j - gap ].sigma <= tmp.sigma; j -= gap ) {
            memcpy( &x[ j ], &x[ j - gap ], sizeof( vul_linalg_svd_basis ) );
         }
         memcpy( &x[ j ], &tmp, sizeof( vul_linalg_svd_basis ) );
      }
   }
}

static void vul__linalg_qr_decomposition_gram_schmidt( vul_linalg_real *Q, vul_linalg_real *R, vul_linalg_real *A, 
                                                       int c, int r, int transpose )
{
   // Gram-Schmidt; numerically bad and slow, but simple.
   vul_linalg_real *u, *a, d, tmp, *At;
   int i, j, k;
   
   if( transpose ) {
      i = c; c = r; r = i;
   }

   u = ( vul_linalg_real* )VUL_LINALG_ALLOC( sizeof( vul_linalg_real ) * c * r );
   a = ( vul_linalg_real* )VUL_LINALG_ALLOC( sizeof( vul_linalg_real ) * r );

   At = 0;
   if( transpose ) {
      At = ( vul_linalg_real* )VUL_LINALG_ALLOC( sizeof( vul_linalg_real ) * c * r );
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
      VUL_LINALG_FREE( A );
   }
   VUL_LINALG_FREE( u );
   VUL_LINALG_FREE( a );
}

static void vul__linalg_apply_householder_column( vul_linalg_real *O, vul_linalg_real *A,
                                                  vul_linalg_real *QO, vul_linalg_real *Q,
                                                  int c, int r, int qc, int qr, int k,
                                                  vul_linalg_real *Qt, vul_linalg_real *u, int respect_signbit )
{
   int i, j, l, free_u, free_Qt;
   vul_linalg_real alpha, d;

   free_u = 0; free_Qt = 0;
   if( u == NULL ) {
      u = ( vul_linalg_real* )VUL_LINALG_ALLOC( sizeof( vul_linalg_real ) * r );
      free_u = 1;
   }
   if( Qt == NULL ) {
      Qt = ( vul_linalg_real* )VUL_LINALG_ALLOC( sizeof( vul_linalg_real ) * r * r );
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
   memset( Qt, 0, sizeof( vul_linalg_real ) * r * r );
   for( i = 0; i < r - k; ++i ) {
      for( j = 0; j < r - k; ++j ) {
         IDX( Qt, i, j, r - k, r - k ) = ( ( i == j ) ? 1.f : 0.f ) - 2.f * u[ i ] * u[ j ];
      }
   }
   // Calcualte new A into O
   memcpy( O, A, c * r * sizeof( vul_linalg_real ) );
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
      memcpy( QO, Q, qc * qr * sizeof( vul_linalg_real ) );
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
      VUL_LINALG_FREE( Qt );
   }
   if( free_u ) {
      VUL_LINALG_FREE( u );
   }
}

static void vul__linalg_qr_decomposition_householder( vul_linalg_real *Q, vul_linalg_real *R, vul_linalg_real *A, 
                                                      int c, int r, int transpose )
{
   vul_linalg_real *u, *Qt, *Q0, *Q1, *R0, *R1, alpha, d;
   int i, j, k, l, n;

   if( transpose ) {
      i = c; c = r; r = i;
   }

   u = ( vul_linalg_real* )VUL_LINALG_ALLOC( sizeof( vul_linalg_real ) * r );

   n = r > c ? r : c;
   Qt = ( vul_linalg_real* )VUL_LINALG_ALLOC( sizeof( vul_linalg_real ) * r * r );
   Q0 = ( vul_linalg_real* )VUL_LINALG_ALLOC( sizeof( vul_linalg_real ) * r * r );
   Q1 = ( vul_linalg_real* )VUL_LINALG_ALLOC( sizeof( vul_linalg_real ) * r * r );
   R0 = ( vul_linalg_real* )VUL_LINALG_ALLOC( sizeof( vul_linalg_real ) * n * n );
   R1 = ( vul_linalg_real* )VUL_LINALG_ALLOC( sizeof( vul_linalg_real ) * n * n );

   if( r == 0 ) return;
   if( r == 1 ) {
      Q[ 0 ] = A[ 0 ]; // Just copy it?
      R[ 0 ] = 1.f;
   }
   memset( R0, 0, sizeof( vul_linalg_real ) * n * n );
   memset( R1, 0, sizeof( vul_linalg_real ) * n * n );
   if( transpose ) {
      vulb__mtranspose( R0, A, r, c );
   } else {
      memcpy( R0, A, sizeof( vul_linalg_real ) * r * c );
   }

   memset( Q0, 0, sizeof( vul_linalg_real ) * r * r );
   for( i = 0; i < r; ++i ) {
      IDX( Q0, i, i, r, r ) = 1.f;
   }

   for( k = 0; k < r - 1; ++k ) {
      // Apply householder transformation to row k
      vul__linalg_apply_householder_column( R1, R0, Q1, Q0, c, r, r, r, k, Qt, u, 1 );

      vulb__swap_ptr( &Q0, &Q1 );
      vulb__swap_ptr( &R0, &R1 );
   }
   memcpy( Q, Q0, sizeof( vul_linalg_real ) * r * r );
   memcpy( R, R0, sizeof( vul_linalg_real ) * r * c );

   VUL_LINALG_FREE( Q0 );
   VUL_LINALG_FREE( Q1 );
   VUL_LINALG_FREE( Qt );
   VUL_LINALG_FREE( R0 );
   VUL_LINALG_FREE( R1 );
   VUL_LINALG_FREE( u );
}

static void vul__linalg_givens_rotate( vul_linalg_real *A, int c, int r, 
                                       int i, int j, float cosine, float sine,
                                       int post_multiply )
{
   int k;
   vul_linalg_real G[ 4 ], v0, v1, a, b;

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

static void vul__linalg_qr_decomposition_givens( vul_linalg_real *Q, vul_linalg_real *R, vul_linalg_real *A, 
                                                 int c, int r, int transpose )
{
   int i, j, k;
   vul_linalg_real theta, st, ct, G[ 4 ], v0, v1, *RA;
   
   RA = ( vul_linalg_real* )VUL_LINALG_ALLOC( sizeof( vul_linalg_real ) * r * c );

   memset( RA, 0, sizeof( vul_linalg_real ) * r * c );
   if( transpose ) {
      i = r; r = c; c = i;
      vulb__mtranspose( RA, A, r, c );
   } else {
      memcpy( RA, A, sizeof( vul_linalg_real ) * r * c );
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
         vul__linalg_givens_rotate( RA, c, r, i, i + 1, ct, st, 1 );
         vul__linalg_givens_rotate( Q, r, r, i, i + 1, ct, st, 0 );
      }
   }
   memcpy( R, RA, c * r * sizeof( vul_linalg_real ) );

   VUL_LINALG_FREE( RA );
}

static vul_linalg_real vul__linalg_matrix_norm_diagonal( vul_linalg_real *A, int c, int r )
{
   vul_linalg_real v;
   int i, n;
   
   v = 0.f;
   n = c < r ? c : r;
   for( i = 0; i < n; ++i ) {
      v += IDX( A, i, i, c, r ) * IDX( A, i, i, c, r );
   }
   return sqrt( v );
}

static vul_linalg_real vul__linalg_matrix_norm_as_single_column( vul_linalg_real *A, int c, int r, int upper_diag )
{
   vul_linalg_real v;
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

static vul_linalg_real vul__linalg_matrix_norm_one( vul_linalg_real *A, int c, int r )
{
   vul_linalg_real v, m;
   int i, j;

   m = 0.0;
   for( i = 0; i < c; ++i ) {
      v = 0.0;
      for( j = 0; j < r; ++j ) {
         v += fabs( IDX( A, i, j, c, r ) );
      }
      m = v > m ? v : m;
   }
   return m;
}

static vul_linalg_real vul__linalg_matrix_norm_inf( vul_linalg_real *A, int c, int r )
{
   vul_linalg_real v, m;
   int i, j;

   m = 0.0;
   for( i = 0; i < r; ++i ) {
      v = 0.0;
      for( j = 0; j < c; ++j ) {
         v += fabs( IDX( A, i, j, c, r ) );
      }
      m = v > m ? v : m;
   }
   return m;
}

vul_linalg_real vul_linalg_largest_eigenvalue_dense( vul_linalg_real *A, int c, int r, 
                                                     int max_iter, vul_linalg_real eps )
{
   int iter, axis, normaxis, i, j;
   vul_linalg_real *v, *y, lambda, norm, err;

   // Power method. Slow, but simple
   v = ( vul_linalg_real* )VUL_LINALG_ALLOC( sizeof( vul_linalg_real ) * r );
   y = ( vul_linalg_real* )VUL_LINALG_ALLOC( sizeof( vul_linalg_real ) * r );
   memset( v, 0, sizeof( vul_linalg_real ) * r );
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

   VUL_LINALG_FREE( v );
   VUL_LINALG_FREE( y );

   return lambda;
}

vul_linalg_real vul_linalg_condition_number_dense( vul_linalg_real *A, int c, int r, 
                                                   int max_iter, vul_linalg_real eps )
{
   vul_linalg_svd_basis *bases;
   vul_linalg_real ret;
   int n;
   
   n = c > r ? r : c;
   bases = ( vul_linalg_svd_basis* )VUL_LINALG_ALLOC( sizeof( vul_linalg_svd_basis ) * n );

   n = 0;
   vul_linalg_svd_dense( bases, &n, A, c, r, max_iter, eps );
   if( n < 2 ) {
      ERR( "Can't compute condition number, not enough non-zero singular values (need 2)." );
      return 0.0;
   }
   ret = bases[ 0 ].sigma / bases[ n - 1 ].sigma;
   vul_linalg_svd_basis_destroy( bases, n );
   VUL_LINALG_FREE( bases );

   return ret;
}

void vul_linalg_svd_dense_qrlq( vul_linalg_svd_basis *out, int *rank,
                                vul_linalg_real *A,
                                int c, int r, int itermax, vul_linalg_real eps )
{
   vul_linalg_real *U0, *U1, *V0, *V1, *S0, *S1, *Sb, *Q, err, e, f, scale;
   int iter, n, i, j, k, ri, ci;

   n = r > c ? r : c;
   U0 = ( vul_linalg_real* )VUL_LINALG_ALLOC( sizeof( vul_linalg_real ) * r * r );
   U1 = ( vul_linalg_real* )VUL_LINALG_ALLOC( sizeof( vul_linalg_real ) * r * r );
   V0 = ( vul_linalg_real* )VUL_LINALG_ALLOC( sizeof( vul_linalg_real ) * c * c );
   V1 = ( vul_linalg_real* )VUL_LINALG_ALLOC( sizeof( vul_linalg_real ) * c * c );
   S0 = ( vul_linalg_real* )VUL_LINALG_ALLOC( sizeof( vul_linalg_real ) * r * c );
   S1 = ( vul_linalg_real* )VUL_LINALG_ALLOC( sizeof( vul_linalg_real ) * r * c );
   Sb = ( vul_linalg_real* )VUL_LINALG_ALLOC( sizeof( vul_linalg_real ) * r * c );
   Q = ( vul_linalg_real* )VUL_LINALG_ALLOC( sizeof( vul_linalg_real ) * n * n );
   memset( U0, 0, sizeof( vul_linalg_real ) * r * r );
   memset( U1, 0, sizeof( vul_linalg_real ) * r * r );
   memset( V0, 0, sizeof( vul_linalg_real ) * c * c );
   memset( V1, 0, sizeof( vul_linalg_real ) * c * c );
   memset( S0, 0, sizeof( vul_linalg_real ) * r * c );
   memset( S1, 0, sizeof( vul_linalg_real ) * r * c );
   memset( Sb, 0, sizeof( vul_linalg_real ) * r * c );
   memset( Q, 0, sizeof( vul_linalg_real ) * n * n );
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
      memcpy( Sb, S0, r * c * sizeof( vul_linalg_real ) );

      // Decompose
      vul__linalg_qr_decomposition_givens( Q, S1, S0, r, c, 1 );
      vulb__mmul_matrix( U1, U0, Q, r );
      vul__linalg_qr_decomposition_givens( Q, S0, S1, c, r, 1 );
      vulb__mmul_matrix( V1, V0, Q, c );

      vulb__swap_ptr( &U0, &U1 );
      vulb__swap_ptr( &V0, &V1 );

      // Calculate error
      e = vul__linalg_matrix_norm_as_single_column( S0, r, c, 1 );
      f = vul__linalg_matrix_norm_diagonal( S0, r, c );
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
   vul__linalg_svd_sort( out, k );

   // Fix signs and copy U
   for( i = 0; i < *rank; ++i ) {
      out[ i ].u_length = r;
      out[ i ].v_length = c;
      out[ i ].u = ( vul_linalg_real* )VUL_LINALG_ALLOC( sizeof( vul_linalg_real ) * r );
      out[ i ].v = ( vul_linalg_real* )VUL_LINALG_ALLOC( sizeof( vul_linalg_real ) * c );
      f = IDX( S0, out[ i ].axis, out[ i ].axis , r, c ) < 0.f ? -1.f : 1.f;
      for( j = 0; j < r; ++j ) {
         out[ i ].u[ j ] = IDX( U0, j, out[ i ].axis, r, r ) * f;
      }
      for( j = 0; j < c; ++j ) {
         out[ i ].v[ j ] = IDX( V0, j, out[ i ].axis, c, c );
      }
   }

   VUL_LINALG_FREE( U0 );
   VUL_LINALG_FREE( U1 );
   VUL_LINALG_FREE( V0 );
   VUL_LINALG_FREE( V1 );
   VUL_LINALG_FREE( S0 );
   VUL_LINALG_FREE( S1 );
   VUL_LINALG_FREE( Sb );
   VUL_LINALG_FREE( Q );
}

void vul_linalg_svd_dense( vul_linalg_svd_basis *out, int *rank,
                           vul_linalg_real *A,
                           int c, int r, int itermax, vul_linalg_real eps )
{
   vul_linalg_real *J, *U, *V, *G, *omegas, f, scale, max_diag;
   int iter, n, m, i, j, k, ri, ci, nonzero;

   n = r > c ? r : c;
   m = r < c ? r : c;
   U = ( vul_linalg_real* )VUL_LINALG_ALLOC( sizeof( vul_linalg_real ) * r * r );
   V = ( vul_linalg_real* )VUL_LINALG_ALLOC( sizeof( vul_linalg_real ) * c * c );
   G = ( vul_linalg_real* )VUL_LINALG_ALLOC( sizeof( vul_linalg_real ) * r * c );
   omegas = ( vul_linalg_real* )VUL_LINALG_ALLOC( sizeof( vul_linalg_real ) * r );
   memset( U, 0, sizeof( vul_linalg_real ) * r * r );
   memset( V, 0, sizeof( vul_linalg_real ) * c * c );
   memset( omegas, 0, sizeof( vul_linalg_real ) * r );
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
   max_diag = 1.0; // Matrix is scaled

   while( nonzero && iter++ < itermax ) {
      nonzero = 0;
      for( i = 0; i < r - 1; ++i ) {
         for( j = i + 1; j < r; ++j ) {
            vul_linalg_real aii, aij, ajj, threshold;
            
            // Skip if already diagonal (or as close as we can get)
#ifdef __FLT_DENORM_MIN__
            threshold = eps * max_diag < __FLT_DENORM_MIN__ ? __FLT_DENORM_MIN__ : eps * max_diag;
#else
            threshold = eps * max_diag < FLT_MIN ? FLT_MIN : eps * max_diag;
#endif
            aii = ( i < r && j < c ) ? IDX( G, i, j, c, r ) : 0.0;
            ajj = ( i < c && j < r ) ? IDX( G, j, i, c, r ) : 0.0;
            if( !( fabs( aii ) > threshold || fabs( ajj ) > threshold ) ) {
               continue;
            }

            // Compute a_ij, a_ji, a_ii
            aii = 0.0; aij = 0.0; ajj = 0.0;
            for( k = 0; k < c; ++k ) {
               aii += IDX( G, i, k, c, r ) * IDX( G, i, k, c, r );
               ajj += IDX( G, j, k, c, r ) * IDX( G, j, k, c, r );
               aij += IDX( G, i, k, c, r ) * IDX( G, j, k, c, r );
            }
            if( fabs( aij ) > threshold ) {
               nonzero += 1;
               vul_linalg_real tau, t, ct, st;
               tau = ( aii - ajj ) / ( 2.0 * aij );
               t = copysign( 1.0 / ( fabs( tau ) + sqrt( 1.0 + tau * tau ) ), tau );
               ct = 1.0 / sqrt( 1.0 + t * t );
               st = ct * t;
               vul__linalg_givens_rotate( G, c, r, j, i, ct, st, 1 );
               vul__linalg_givens_rotate( U, r, r, j, i, ct, st, 0 );
               aii = IDX( G, i, i, c, r );
               if( j < r && j < c ) {
                  ajj = IDX( G, j, j, c, r );
               } else {
                  ajj = 0.0;
               }
               
               // Keep track of largest diagonal entry
               threshold = aii > ajj ? aii : ajj;
               max_diag = max_diag > threshold ? max_diag : threshold;
            }
         }
      }
   }

   // Calculate the singular values (2-norm of the columns of G)
   for( i = 0; i < r; ++i ) {
      vul_linalg_real t = 0.0;
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
   vul__linalg_svd_sort( out, k );

   // Fix signs and copy U and V
   for( i = 0; i < *rank; ++i ) {
      out[ i ].u_length = r;
      out[ i ].v_length = c;
      out[ i ].u = ( vul_linalg_real* )VUL_LINALG_ALLOC( sizeof( vul_linalg_real ) * r );
      out[ i ].v = ( vul_linalg_real* )VUL_LINALG_ALLOC( sizeof( vul_linalg_real ) * c );
      f = omegas[ out[ i ].axis ] < 0.f ? -1.f : 1.f;
      for( j = 0; j < r; ++j ) {
         out[ i ].u[ j ] = IDX( U, j, out[ i ].axis, r, r ) * f;
      }
      for( j = 0; j < c; ++j ) {
         out[ i ].v[ j ] = IDX( V, j, out[ i ].axis, c, c );
      }
   }

   VUL_LINALG_FREE( U );
   VUL_LINALG_FREE( V );
   VUL_LINALG_FREE( G );
   VUL_LINALG_FREE( omegas );
}

void vul_linalg_linear_least_squares_dense( vul_linalg_real *x,
                                            vul_linalg_svd_basis *bases,
                                            int rank,
                                            vul_linalg_real *b )
{
   vul_linalg_real *d, v;
   int i, j, m;

   m = bases[ 0 ].u_length < bases[ 0 ].v_length ? bases[ 0 ].u_length : bases[ 0 ].v_length;
   d = ( vul_linalg_real* )VUL_LINALG_ALLOC( sizeof( vul_linalg_real ) * bases[ 0 ].u_length );
   memset( d, 0, sizeof( vul_linalg_real ) * bases[ 0 ].u_length );
   memset( x, 0, sizeof( vul_linalg_real ) * m );

   // Calculate d = U^T * b
   for( i = 0; i < rank; ++i ) {
      v = 0.0;
      for( j = 0; j < bases[ i ].u_length; ++j ) {
         v += bases[ i ].u[ j ] * b[ j ];
      }
      d[ bases[ i ].axis ] = v;
   }

   // Calculate x = V * S^+ * d
   for( i = 0; i < rank; ++i ) {
      for( j = 0; j < m; ++j ) {
         x[ j ] += bases[ i ].v[ j ] * d[ bases[ i ].axis ] / bases[ i ].sigma;
      }
   }

   VUL_LINALG_FREE( d );
}

#undef IDX
#undef ERR

#ifdef _cplusplus
}
#endif

#undef vul_linalg_real

#endif
