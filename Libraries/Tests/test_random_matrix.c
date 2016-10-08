#include <stdio.h>
#include <assert.h>

#define VUL_DEFINE
#define VUL_LINALG_ERROR_ASSERT
#include "../vul_linalg.h"
#include "../vul_timer.h"
#include "../vul_rngs.h"

#ifdef VUL_LINALG_DOUBLE
typedef double real;
#else
typedef float real;
#endif

void generate_square_symmetric_positive_definite_matrix_dense( real *A, int n, real density )
{
   real *A2 = ( real* )malloc( sizeof( real ) * n * n );
   real *A3 = ( real* )malloc( sizeof( real ) * n * n );
   vul_rng_pcg32 *rng = vul_rng_pcg32_create( 0xbeefcafe, 0xdeadf012 );
   
   for( int y = 0; y < n; ++y ) {
      for( int x = 0; x < n; ++x ) {
         real chance = vul_rng_pcg32_next_float( rng );
         if( chance < 0.5 * density ) {
            real val = vul_rng_pcg32_next_float( rng );
            A2[ x * n + y ] = val;
         }
      }
   }
   // A*A'
   vulb__mtranspose( A3, A2, n, n );
   vulb__mmul_matrix( A, A2, A3, n );

   for( int i = 0; i < n; ++i ) {
      A[ i * n + i ] += n;
   }

   vul_rng_pcg32_destroy( rng );
   free( A2 );
   free( A3 );
}

void generate_random_vector_dense( real *v, int n )
{
   vul_rng_pcg32 *rng = vul_rng_pcg32_create( 0xabad1dea, 0xc00010ff );

   for( int i = 0; i < n; ++i ) {
      v[ i ] = vul_rng_pcg32_next_float( rng );
   }

   vul_rng_pcg32_destroy( rng );
}

vul_linalg_matrix *generate_square_symmetric_positive_definite_matrix( int n, real density )
{
   vul_linalg_matrix *A = vul_linalg_matrix_create( 0, 0, 0, 0 );
   vul_linalg_matrix *A2 = vul_linalg_matrix_create( 0, 0, 0, 0 );
   vul_rng_pcg32 *rng = vul_rng_pcg32_create( 0xbeefcafe, 0xdeadf012 );
   
   int c = 0;
   for( int y = 0; y < n; ++y ) {
      for( int x = 0; x < n; ++x ) {
         real chance = vul_rng_pcg32_next_float( rng );
         if( chance < 0.5 * density ) {
            ++c;
            real val = vul_rng_pcg32_next_float( rng );
            vul_linalg_matrix_insert( A, y, x, val );
         }
      }
   }
   // A*A'
   for( int i = 0; i < A->count; ++i ) {
      for( int j = 0; j < A->count; ++j ) {
         real s = 0.f;
         for( int k0 = 0, k1 = 0; k0 < A->rows[ i ].vec.count && k1 < A->rows[ j ].vec.count; ) {
            if( A->rows[ i ].vec.entries[ k0 ].idx == A->rows[ j ].vec.entries[ k1 ].idx ) {
               s += A->rows[ i ].vec.entries[ k0 ].val * A->rows[ j ].vec.entries[ k1 ].val;
               ++k0; ++k1;
            } else if( A->rows[ i ].vec.entries[ k0 ].idx < A->rows[ j ].vec.entries[ k1 ].idx ) {
               ++k0;
            } else {
               ++k1;
            }
         }
         vul_linalg_matrix_insert( A2, A->rows[ i ].idx, A->rows[ j ].idx, s );
      }
   }

   for( int i = 0; i < n; ++i ) {
      vul_linalg_matrix_insert( A2, i, i, vul_linalg_matrix_get( A2, i, i ) + n );
   }

   vul_rng_pcg32_destroy( rng );
   vul_linalg_matrix_destroy( A );
   return A2;
}

vul_linalg_vector *generate_random_vector( int n )
{
   vul_rng_pcg32 *rng = vul_rng_pcg32_create( 0xabad1dea, 0xc00010ff );
   vul_linalg_vector *v = vul_linalg_vector_create( 0, 0, 0 );

   for( int i = 0; i < n; ++i ) {
      vul_linalg_vector_insert( v, i, vul_rng_pcg32_next_float( rng ) );
   }

   vul_rng_pcg32_destroy( rng );
   return v;
}

#define TEST( expr ) if( !( expr ) ) {\
   fprintf( stderr, "Failed assert at %s:%d.", __FILE__, __LINE__ );\
   fprintf( stderr, #expr );\
   exit( 1 );\
}

#define CHECK_WITHIN_EPS_SPARSE( a, b, n, eps )\
   {for( int ppi = 0; ppi < ( n ); ++ppi ) {\
      TEST( fabs( vul_linalg_vector_get( a, ppi ) - vul_linalg_vector_get( b, ppi ) ) < ( eps ) );\
   }}

#include <cholmod.h>
vul_linalg_vector *solve_cholmod( vul_linalg_matrix *vA, vul_linalg_vector *vb, int n )
{
   cholmod_sparse *A;
   cholmod_dense *x, *b, *r;
   cholmod_factor *L;
   cholmod_triplet *T;
   cholmod_common c;
   double one[ 2 ] = {1,0}, inv[ 2 ] = {-1,0};
   vul_timer *t;
   uint64_t pre, post;
   int i, j, k;
   cholmod_start( &c );
   t = vul_timer_create( );
   k = 0;
   for( i = 0; i < vA->count; ++i ) {
      for( j = 0; j < vA->rows[ i ].vec.count; ++j ) {
         ++k;
      }
   }
   T = cholmod_allocate_triplet( n, n, k, 1, CHOLMOD_REAL, &c );
   k = 0;
   for( i = 0; i < vA->count; ++i ) {
      for( j = 0; j < vA->rows[ i ].vec.count; ++j ) {
         if( T->itype == CHOLMOD_LONG ) {
            long *ti, *tj;
            ti = T->i;
            tj = T->j;
            ti[ k ] = vA->rows[ i ].idx;
            tj[ k ] = vA->rows[ i ].vec.entries[ j ].idx;
         } else {
            int *ti, *tj;
            ti = T->i;
            tj = T->j;
            ti[ k ] = vA->rows[ i ].idx;
            tj[ k ] = vA->rows[ i ].vec.entries[ j ].idx;
         }
         if( T->dtype == CHOLMOD_DOUBLE ) {
            ( ( double* )T->x )[ k ] = vA->rows[ i ].vec.entries[ j ].val;
         } else {
            ( ( float* )T->x )[ k ] = vA->rows[ i ].vec.entries[ j ].val;
         }
         ++k;
      }
   }
   T->nnz = k;
   A = cholmod_triplet_to_sparse( T, n*n, &c );
   cholmod_free_triplet( &T, &c );
   b = cholmod_zeros( n, 1, A->xtype, &c );
   for( i = 0; i < vb->count; ++i ) {
      if( b->dtype == CHOLMOD_DOUBLE ) {
         ( ( double* )b->x )[ vb->entries[ i ].idx ] = vb->entries[ i ].val;
      } else {
         ( ( float* )b->x )[ vb->entries[ i ].idx ] = vb->entries[ i ].val;
      }
   }
   
   pre = vul_timer_get_micros( t );
   L = cholmod_analyze( A, &c );
   cholmod_factorize( A, L, &c );
   post = vul_timer_get_micros( t );
   printf("Cholmod decomposition %lu.%3lums\n", (post-pre)/1000, (post-pre)%1000);
   
   pre = vul_timer_get_micros( t );
   x = cholmod_solve( CHOLMOD_A, L, b, &c );
   post = vul_timer_get_micros( t );
   printf("Cholmod solve %lu.%3lums\n", (post-pre)/1000, (post-pre)%1000);
   
   r = cholmod_copy_dense( b, &c );
   cholmod_sdmult( A, 0, inv, one, x, r, &c );
   printf("Cholmod residual norm: %e\n", cholmod_norm_dense( r, 0, &c ));
   printf("Cholmod normalized error: %e\n", cholmod_norm_dense( r, 0, &c ) / sqrt( vulb__sparse_dot( vb, vb ) ) );
   printf("\n");fflush(stdout);
   vul_linalg_vector *vx = vul_linalg_vector_create( 0, 0, 0 );
   for( k = 0; k < n; ++k ) {
      if( b->dtype == CHOLMOD_DOUBLE ) {
         vul_linalg_vector_insert( vx, k, ( ( double* )x->x )[ k ] );
      } else {
         vul_linalg_vector_insert( vx, k, ( ( float* )x->x )[ k ] );
      }
   }
   cholmod_free_factor( &L, &c );
   cholmod_free_sparse( &A, &c );
   cholmod_free_dense( &r, &c );
   cholmod_free_dense( &x, &c );
   cholmod_free_dense( &b, &c );
   cholmod_finish( &c );
   return vx;
}

#define TEST_CHOLMOD
#define TEST_SVD
#define TEST_QR
#define TEST_LU
#define TEST_CHOLESKY
#define TEST_SOR
#define TEST_GMRES_NONE
#define TEST_GMRES_JACOBI
#define TEST_GMRES_ILU0
#define TEST_GMRES_ILDTT0
#define TEST_CG_NONE
#define TEST_CG_JACOBI
#define TEST_CG_ILU0
#define TEST_CG_ILDTT0

void test_dense( vul_linalg_matrix *vA, vul_linalg_vector *vb, int n, real d, real eps, int iters, int gmres_restart, int gmres_iters, vul_timer *t )
{
   uint64_t pre, post;
   int rank = 0;

   //-----------------------------------
   // Dense
   //
   printf("------------------------\n");
   printf("DENSE MATRICES\n\n");

   pre = vul_timer_get_micros( t );
   real *A = ( real* )malloc( sizeof( real * ) * n * n );
   real *b = ( real* )malloc( sizeof( real * ) * n );
   memset( A, 0, sizeof( real ) * n * n );
   memset( b, 0, sizeof( real ) * n );
   for( int y = 0; y < n; ++y ) {
      for( int x = 0; x < n; ++x ) {
         A[ x * n + y ] = vul_linalg_matrix_get( vA, x, y );
      }
      b[ y ] = vul_linalg_vector_get( vb, y );
   }
   post = vul_timer_get_micros( t );
   printf("Generation step %lu.%3lums for (%d^2 matrix @ %f density)\n\n", (post-pre)/1000, (post-pre)%1000, n, d);

   real *x = ( real* )malloc( sizeof( real * ) * n );
   real *r = ( real* )malloc( sizeof( real * ) * n );
   real *e = ( real* )malloc( sizeof( real * ) * n );
   real *guess = ( real* )malloc( sizeof( real * ) * n );
   real *P = ( real* )malloc( sizeof( real * ) * n * n );
   real *P2 = ( real* )malloc( sizeof( real * ) * n * n );
   memset( P, 0, sizeof( real ) * n * n );
   memset( P2, 0, sizeof( real ) * n * n );
   memset( guess, 0, sizeof( real ) * n );
   memset( x, 0, sizeof( real ) * n );
   memset( r, 0, sizeof( real ) * n );
   memset( e, 0, sizeof( real ) * n );

#ifdef TEST_SVD
   vul_linalg_svd_basis *res = ( vul_linalg_svd_basis* )malloc( n * sizeof( vul_linalg_svd_basis ) );
   pre = vul_timer_get_micros( t );
   vul_linalg_svd_dense( res, &rank, A, n, n, 32, eps );
   post = vul_timer_get_micros( t );
   printf("SVD decompose %lu.%3lums\n", (post-pre)/1000, (post-pre)%1000);
   pre = vul_timer_get_micros( t );
   vul_linalg_linear_least_squares_dense( r, res, rank, b );
   post = vul_timer_get_micros( t );
   printf("SVD solve %lu.%3lums\n", (post-pre)/1000, (post-pre)%1000);
   vulb__mmul( e, A, r, n, n );
   vulb__vsub( e, b, e, n );
   printf("Residual: %e\n", sqrt( vulb__dot( e, e, n ) ) );
   vul_linalg_svd_basis_destroy( res, rank );
   free( res );
   printf("\n");fflush( stdout );
#endif

#ifdef TEST_QR
   // QR decompose
   pre = vul_timer_get_micros( t );
   vul_linalg_qr_decomposition_dense( P, P2, A, n );
   post = vul_timer_get_micros( t );
   printf("QR decompose %lu.%3lums\n", (post-pre)/1000, (post-pre)%1000);
   pre = vul_timer_get_micros( t );
   vul_linalg_qr_solve_dense( x, P, P2, A, guess, b, n, iters, eps );
   post = vul_timer_get_micros( t );
   printf("QR solve %lu.%3lums\n", (post-pre)/1000, (post-pre)%1000);
   memset( e, 0, sizeof( real ) * n );
   vulb__mmul( e, A, x, n, n );
   vulb__vsub( e, b, e, n );
   printf("Residual: %e\n", sqrt( vulb__dot( e, e, n ) ) );
   if( r ) {
      for( int i = 0; i < n; ++i ) {
         if( fabs( x[ i ] - r[ i ] ) > 1e-2 ) {
            printf( "At least one error is large: %f != %f (idx %d)\n", x[ i ], r[ i ], i );
         }
      }
      vulb__vsub( x, x, r, n );
      printf( "Metrics QR: err2: %e, normed %e\n", vulb__dot( x, x, n ), sqrt( vulb__dot( x, x, n ) ) / sqrt( vulb__dot( r, r, n ) ) );
   } else {
      r = x;
   }
   printf("\n");fflush( stdout );
#endif

#ifdef TEST_LU
   // LU decompose
   memset( x, 0, sizeof( real ) * n );
   pre = vul_timer_get_micros( t );
   memset( P, 0, sizeof( real ) * n * n );
   int *indices = ( int* )malloc( sizeof( int ) * n );
   vul_linalg_lu_decomposition_dense( P, indices, A, n );
   post = vul_timer_get_micros( t );
   printf("LU decompose %lu.%3lums\n", (post-pre)/1000, (post-pre)%1000);
   pre = vul_timer_get_micros( t );
   vul_linalg_lu_solve_dense( x, P, indices, A, guess, b, n, iters, eps );
   post = vul_timer_get_micros( t );
   printf("LU solve %lu.%3lums\n", (post-pre)/1000, (post-pre)%1000);
   memset( e, 0, sizeof( real ) * n );
   vulb__mmul( e, A, x, n, n );
   vulb__vsub( e, b, e, n );
   printf("Residual: %e\n", sqrt( vulb__dot( e, e, n ) ) );
   if( r ) {
      for( int i = 0; i < n; ++i ) {
         if( fabs( x[ i ] - r[ i ] ) > 1e-2 ) {
            printf( "At least one error is large: %f != %f (idx %d)\n", x[ i ], r[ i ], i );
         }
      }
      vulb__vsub( x, x, r, n );
      printf( "Metrics LU: err2: %e, normed %e\n", vulb__dot( x, x, n ), sqrt( vulb__dot( x, x, n ) ) / sqrt( vulb__dot( r, r, n ) ) );
   } else {
      r = x;
   }
   free( indices );
   printf("\n");fflush( stdout );
#endif

#ifdef TEST_CHOLESKY
   // Cholesky decompose
   memset( x, 0, sizeof( real ) * n );
   pre = vul_timer_get_micros( t );
   memset( P, 0, sizeof( real ) * n * n );
   vul_linalg_cholesky_decomposition_dense( P, A, n );
   post = vul_timer_get_micros( t );
   printf("Cholesky decompose %lu.%3lums\n", (post-pre)/1000, (post-pre)%1000);
   pre = vul_timer_get_micros( t );
   vul_linalg_cholesky_solve_dense( x, P, A, guess, b, n, iters, eps );
   post = vul_timer_get_micros( t );
   printf("Cholesky solve %lu.%3lums\n", (post-pre)/1000, (post-pre)%1000);
   memset( e, 0, sizeof( real ) * n );
   vulb__mmul( e, A, x, n, n );
   vulb__vsub( e, b, e, n );
   printf("Residual: %e\n", sqrt( vulb__dot( e, e, n ) ) );
   if( r ) {
      for( int i = 0; i < n; ++i ) {
         if( fabs( x[ i ] - r[ i ] ) > 1e-2 ) {
            printf( "At least one error is large: %f != %f (idx %d)\n", x[ i ], r[ i ], i );
         }
      }
      vulb__vsub( x, x, r, n );
      printf( "Metrics Cholesky: err2: %e, normed %e\n", vulb__dot( x, x, n ), sqrt( vulb__dot( x, x, n ) ) / sqrt( vulb__dot( r, r, n ) ) );
   } else {
      r = x;
   }
   printf("\n");fflush( stdout );
#endif

#ifdef TEST_SOR
   // SOR
   memset( x, 0, sizeof( real ) * n );
   pre = vul_timer_get_micros( t );
   vul_linalg_successive_over_relaxation_dense( x, A, guess, b, 1.05, n, iters, eps );
   post = vul_timer_get_micros( t );
   printf("SOR solve %lu.%3lums\n", (post-pre)/1000, (post-pre)%1000);
   memset( e, 0, sizeof( real ) * n );
   vulb__mmul( e, A, x, n, n );
   vulb__vsub( e, b, e, n );
   printf("Residual: %e\n", sqrt( vulb__dot( e, e, n ) ) );
   if( r ) {
      for( int i = 0; i < n; ++i ) {
         if( fabs( x[ i ] - r[ i ] ) > 1e-2 ) {
            printf( "At least one error is large: %f != %f (idx %d)\n",x[ i ], r[ i ], i );
         }
      }
      vulb__vsub( x, x, r, n );
      printf( "Metrics SOR: err2: %e, normed %e\n", vulb__dot( x, x, n ), sqrt( vulb__dot( x, x, n ) ) / sqrt( vulb__dot( r, r, n ) ) );
   } else {
      r = x;
   }
   printf("\n");fflush( stdout );
#endif

#ifdef TEST_GMRES_NONE
   // GMRES - no preconditioner
   memset( x, 0, sizeof( real ) * n );
   pre = vul_timer_get_micros( t );
   vul_linalg_gmres_dense( x, A, guess, b, n, gmres_restart, gmres_iters, eps );
   post = vul_timer_get_micros( t );
   printf("GMRES - NONE solve %lu.%3lums\n", (post-pre)/1000, (post-pre)%1000);
   memset( e, 0, sizeof( real ) * n );
   vulb__mmul( e, A, x, n, n );
   vulb__vsub( e, b, e, n );
   printf("Residual: %e\n", sqrt( vulb__dot( e, e, n ) ) );
   if( r ) {
      for( int i = 0; i < n; ++i ) {
         if( fabs( x[ i ] - r[ i ] ) > 1e-2 ) {
            printf( "At least one error is large: %f != %f (idx %d)\n",x[ i ], r[ i ], i );
         }
      }
      vulb__vsub( x, x, r, n );
      printf( "Metrics GMRES - NONE: err2: %e, normed %e\n", vulb__dot( x, x, n ), sqrt( vulb__dot( x, x, n ) ) / sqrt( vulb__dot( r, r, n ) ) );
   } else {
      r = x;
   }
   printf("\n");fflush( stdout );
#endif

#ifdef TEST_CG_NONE
   // CG - no preconditioner
   memset( x, 0, sizeof( real ) * n );
   pre = vul_timer_get_micros( t );
   vul_linalg_conjugate_gradient_dense( x, A, guess, b, n, iters, eps );
   post = vul_timer_get_micros( t );
   printf("CG - NONE solve %lu.%3lums\n", (post-pre)/1000, (post-pre)%1000);
   memset( e, 0, sizeof( real ) * n );
   vulb__mmul( e, A, x, n, n );
   vulb__vsub( e, b, e, n );
   printf("Residual: %e\n", sqrt( vulb__dot( e, e, n ) ) );
   if( r ) {
      for( int i = 0; i < n; ++i ) {
         if( fabs( x[ i ] - r[ i ] ) > 1e-2 ) {
            printf( "At least one error is large: %f != %f (idx %d)\n",x[ i ], r[ i ], i );
         }
      }
      vulb__vsub( x, x, r, n );
      printf( "Metrics CG - NONE: err2: %e, normed %e\n", vulb__dot( x, x, n ), sqrt( vulb__dot( x, x, n ) ) / sqrt( vulb__dot( r, r, n ) ) );
   } else {
      r = x;
   }
   printf("\n");fflush( stdout );
#endif

   free( x );
   free( r );
   free( guess );
   free( b );
   free( e );
   free( P2 );
   free( P );
   free( A );
}

int main( int argc, char **argv )
{
   uint64_t pre, post;
   int n = 256;
   real d = 0.01;
   vul_timer *t = vul_timer_create( );
   pre = vul_timer_get_micros( t );
   vul_linalg_matrix *A = generate_square_symmetric_positive_definite_matrix( n, d );
   vul_linalg_vector *b = generate_random_vector( n );
   post = vul_timer_get_micros( t );
   printf("Generation step %lu.%3lums for (%d^2 matrix @ %f density)\n\n", (post-pre)/1000, (post-pre)%1000, n, d);

   vul_linalg_vector *x, *r = 0, *e, *guess = vul_linalg_vector_create( 0, 0, 0 );
   vul_linalg_matrix *P, *P2, *ILU = 0;

   float eps = 1e-10;
   int iters = 512, gmres_iters = 64, gmres_restart = 64;

   e = vul_linalg_vector_create( 0, 0, 0 );

#ifdef TEST_CHOLMOD
   r = solve_cholmod( A, b, n );
#endif

#ifdef TEST_SVD
   vul_linalg_svd_basis_sparse *res = ( vul_linalg_svd_basis_sparse* )malloc( n * sizeof( vul_linalg_svd_basis_sparse ) );
   int rank = 0;
   pre = vul_timer_get_micros( t );
   vul_linalg_svd_sparse( res, &rank, A, n, n, 32, eps );
   post = vul_timer_get_micros( t );
   printf("SVD decompose %lu.%3lums\n", (post-pre)/1000, (post-pre)%1000);
   pre = vul_timer_get_micros( t );
   r = vul_linalg_linear_least_squares_sparse( res, rank, b );
   post = vul_timer_get_micros( t );
   printf("SVD solve %lu.%3lums\n", (post-pre)/1000, (post-pre)%1000);
   vulb__sparse_vclear( e );
   vulb__sparse_mmul( e, A, r );
   vulb__sparse_vsub( e, b, e );
   printf("Residual: %e\n", sqrt( vulb__sparse_dot( e, e ) ) );
   vul_linalg_svd_basis_destroy_sparse( res, rank );
   printf("\n");fflush( stdout );
#endif

#ifdef TEST_QR
   // QR decompose
   pre = vul_timer_get_micros( t );
   vul_linalg_qr_decomposition_sparse( &P, &P2, A, n, n );
   post = vul_timer_get_micros( t );
   printf("QR decompose %lu.%3lums\n", (post-pre)/1000, (post-pre)%1000);
   pre = vul_timer_get_micros( t );
   x = vul_linalg_qr_solve_sparse( P, P2, A, guess, b, n, n, iters, eps );
   post = vul_timer_get_micros( t );
   printf("QR solve %lu.%3lums\n", (post-pre)/1000, (post-pre)%1000);
   vulb__sparse_vclear( e );
   vulb__sparse_mmul( e, A, x );
   vulb__sparse_vsub( e, b, e );
   printf("Residual: %e\n", sqrt( vulb__sparse_dot( e, e ) ) );
   if( r ) {
      for( int i = 0; i < n; ++i ) {
         if( fabs( vul_linalg_vector_get( x, i ) - vul_linalg_vector_get( r, i ) ) > 1e-2 ) {
            printf( "At least one error is large: %f != %f (idx %d)\n", vul_linalg_vector_get( x, i ), vul_linalg_vector_get( r, i ), i );
         }
      }
      vulb__sparse_vsub( x, x, r );
      printf( "Metrics QR: err2: %e, normed %e\n", vulb__sparse_dot( x, x ), sqrt( vulb__sparse_dot( x, x ) ) / sqrt( vulb__sparse_dot( r, r ) ) );
      vul_linalg_vector_destroy( x );
   } else {
      r = x;
   }
   vul_linalg_matrix_destroy( P );
   vul_linalg_matrix_destroy( P2 );
   printf("\n");fflush( stdout );
#endif

#ifdef TEST_LU
   // LU decompose
   pre = vul_timer_get_micros( t );
   vul_linalg_lu_decomposition_sparse( &P, A, n, n );
   post = vul_timer_get_micros( t );
   printf("LU decompose %lu.%3lums\n", (post-pre)/1000, (post-pre)%1000);
   pre = vul_timer_get_micros( t );
   x = vul_linalg_lu_solve_sparse( P, A, guess, b, n, n, iters, eps );
   post = vul_timer_get_micros( t );
   printf("LU solve %lu.%3lums\n", (post-pre)/1000, (post-pre)%1000);
   vulb__sparse_vclear( e );
   vulb__sparse_mmul( e, A, x );
   vulb__sparse_vsub( e, b, e );
   printf("Residual: %e\n", sqrt( vulb__sparse_dot( e, e ) ) );
   if( r ) {
      for( int i = 0; i < n; ++i ) {
         if( fabs( vul_linalg_vector_get( x, i ) - vul_linalg_vector_get( r, i ) ) > 1e-2 ) {
            printf( "At least one error is large: %f != %f (idx %d)\n", vul_linalg_vector_get( x, i ), vul_linalg_vector_get( r, i ), i );
         }
      }
      vulb__sparse_vsub( x, x, r );
      printf( "Metrics LU: err2: %e, normed %e\n", vulb__sparse_dot( x, x ), sqrt( vulb__sparse_dot( x, x ) ) / sqrt( vulb__sparse_dot( r, r ) ) );
      vul_linalg_vector_destroy( x );
   } else {
      r = x;
   }
   vul_linalg_matrix_destroy( P );
   printf("\n");fflush( stdout );
#endif

#ifdef TEST_CHOLESKY
   // Cholesky decompose
   pre = vul_timer_get_micros( t );
   vul_linalg_cholesky_decomposition_sparse( &P, &P2, A, n, n );
   post = vul_timer_get_micros( t );
   printf("Cholesky decompose %lu.%3lums\n", (post-pre)/1000, (post-pre)%1000);
   pre = vul_timer_get_micros( t );
   x = vul_linalg_cholesky_solve_sparse( P, P2, A, guess, b, n, n, iters, eps );
   post = vul_timer_get_micros( t );
   printf("Cholesky solve %lu.%3lums\n", (post-pre)/1000, (post-pre)%1000);
   vulb__sparse_vclear( e );
   vulb__sparse_mmul( e, A, x );
   vulb__sparse_vsub( e, b, e );
   printf("Residual: %e\n", sqrt( vulb__sparse_dot( e, e ) ) );
   if( r ) {
      for( int i = 0; i < n; ++i ) {
         if( fabs( vul_linalg_vector_get( x, i ) - vul_linalg_vector_get( r, i ) ) > 1e-2 ) {
            printf( "At least one error is large: %f != %f (idx %d)\n", vul_linalg_vector_get( x, i ), vul_linalg_vector_get( r, i ), i );
         }
      }
      vulb__sparse_vsub( x, x, r );
      printf( "Metrics Cholesky: err2: %e, normed %e\n", vulb__sparse_dot( x, x ), sqrt( vulb__sparse_dot( x, x ) ) / sqrt( vulb__sparse_dot( r, r ) ) );
      vul_linalg_vector_destroy( x );
   } else {
      r = x;
   }
   vul_linalg_matrix_destroy( P );
   vul_linalg_matrix_destroy( P2 );
   printf("\n");fflush( stdout );
#endif

#ifdef TEST_SOR
   // SOR
   pre = vul_timer_get_micros( t );
   x = vul_linalg_successive_over_relaxation_sparse( A, guess, b, 1.05, iters, eps );
   post = vul_timer_get_micros( t );
   printf("SOR solve %lu.%3lums\n", (post-pre)/1000, (post-pre)%1000);
   vulb__sparse_vclear( e );
   vulb__sparse_mmul( e, A, x );
   vulb__sparse_vsub( e, b, e );
   printf("Residual: %e\n", sqrt( vulb__sparse_dot( e, e ) ) );
   if( r ) {
      for( int i = 0; i < n; ++i ) {
         if( fabs( vul_linalg_vector_get( x, i ) - vul_linalg_vector_get( r, i ) ) > 1e-2 ) {
            printf( "At least one error is large: %f != %f (idx %d)\n", vul_linalg_vector_get( x, i ), vul_linalg_vector_get( r, i ), i );
         }
      }
      vulb__sparse_vsub( x, x, r );
      printf( "Metrics SOR: err2: %e, normed %e\n", vulb__sparse_dot( x, x ), sqrt( vulb__sparse_dot( x, x ) ) / sqrt( vulb__sparse_dot( r, r ) ) );
      vul_linalg_vector_destroy( x );
   } else {
      r = x;
   }
   printf("\n");fflush( stdout );
#endif

#ifdef TEST_GMRES_NONE
   // GMRES - no preconditioner
   pre = vul_timer_get_micros( t );
   x = vul_linalg_gmres_sparse( A, guess, b, NULL, VUL_LINALG_PRECONDITIONER_NONE, gmres_restart, gmres_iters, eps );
   post = vul_timer_get_micros( t );
   printf("GMRES - NONE solve %lu.%3lums\n", (post-pre)/1000, (post-pre)%1000);
   vulb__sparse_vclear( e );
   vulb__sparse_mmul( e, A, x );
   vulb__sparse_vsub( e, b, e );
   printf("Residual: %e\n", sqrt( vulb__sparse_dot( e, e ) ) );
   if( r ) {
      for( int i = 0; i < n; ++i ) {
         if( fabs( vul_linalg_vector_get( x, i ) - vul_linalg_vector_get( r, i ) ) > 1e-2 ) {
            printf( "At least one error is large: %f != %f (idx %d)\n", vul_linalg_vector_get( x, i ), vul_linalg_vector_get( r, i ), i );
         }
      }
      vulb__sparse_vsub( x, x, r );
      printf( "Metrics GMRES - NONE: err2: %e, normed %e\n", vulb__sparse_dot( x, x ), sqrt( vulb__sparse_dot( x, x ) ) / sqrt( vulb__sparse_dot( r, r ) ) );
      vul_linalg_vector_destroy( x );
   } else {
      r = x;
   }
   printf("\n");fflush( stdout );
#endif

#ifdef TEST_GMRES_JACOBI
   // GMRES - Jacobi
   pre = vul_timer_get_micros( t );
   P = vul_linalg_precondition_jacobi( A, n, n );
   post = vul_timer_get_micros( t );
   printf("GMRES - Jacobi decompose %lu.%3lums\n", (post-pre)/1000, (post-pre)%1000);
   pre = vul_timer_get_micros( t );
   x = vul_linalg_gmres_sparse( A, guess, b, P, VUL_LINALG_PRECONDITIONER_JACOBI, gmres_restart, gmres_iters, eps );
   post = vul_timer_get_micros( t );
   printf("GMRES - Jacobi solve %lu.%3lums\n", (post-pre)/1000, (post-pre)%1000);
   vulb__sparse_vclear( e );
   vulb__sparse_mmul( e, A, x );
   vulb__sparse_vsub( e, b, e );
   printf("Residual: %e\n", sqrt( vulb__sparse_dot( e, e ) ) );
   if( r ) {
      for( int i = 0; i < n; ++i ) {
         if( fabs( vul_linalg_vector_get( x, i ) - vul_linalg_vector_get( r, i ) ) > 1e-2 ) {
            printf( "At least one error is large: %f != %f (idx %d)\n", vul_linalg_vector_get( x, i ), vul_linalg_vector_get( r, i ), i );
         }
      }
      vulb__sparse_vsub( x, x, r );
      printf( "Metrics GMRES - Jacobi: err2: %e, normed %e\n", vulb__sparse_dot( x, x ), sqrt( vulb__sparse_dot( x, x ) ) / sqrt( vulb__sparse_dot( r, r ) ) );
      vul_linalg_vector_destroy( x );
   } else {
      r = x;
   }
   vul_linalg_matrix_destroy( P );
   printf("\n");fflush( stdout );
#endif

   // GMRES - ILU
#ifdef TEST_GMRES_ILU0
   pre = vul_timer_get_micros( t );
   if( !ILU ) {
      ILU = vul_linalg_precondition_ilu0( A, n, n );
   }
   post = vul_timer_get_micros( t );
   printf("GMRES - ILU(0) decompose %lu.%3lums\n", (post-pre)/1000, (post-pre)%1000);
   pre = vul_timer_get_micros( t );
   x = vul_linalg_gmres_sparse( A, guess, b, ILU, VUL_LINALG_PRECONDITIONER_INCOMPLETE_LU_0, gmres_restart, gmres_iters, eps );
   post = vul_timer_get_micros( t );
   printf("GMRES - ILU(0) solve %lu.%3lums\n", (post-pre)/1000, (post-pre)%1000);
   vulb__sparse_vclear( e );
   vulb__sparse_mmul( e, A, x );
   vulb__sparse_vsub( e, b, e );
   printf("Residual: %e\n", sqrt( vulb__sparse_dot( e, e ) ) );
   if( r ) {
      for( int i = 0; i < n; ++i ) {
         if( fabs( vul_linalg_vector_get( x, i ) - vul_linalg_vector_get( r, i ) ) > 1e-2 ) {
            printf( "At least one error is large: %f != %f (idx %d)\n", vul_linalg_vector_get( x, i ), vul_linalg_vector_get( r, i ), i );
         }
      }
      vulb__sparse_vsub( x, x, r );
      printf( "Metrics GMRES - ILU(0): err2: %e, normed %e\n", vulb__sparse_dot( x, x ), sqrt( vulb__sparse_dot( x, x ) ) / sqrt( vulb__sparse_dot( r, r ) ) );
      vul_linalg_vector_destroy( x );
   } else {
      r = x;
   }
   printf("\n");fflush( stdout );
#endif

   // GMRES - ILDDT(0)
#ifdef TEST_GMRES_ILDTT0
   pre = vul_timer_get_micros( t );
   P = vul_linalg_precondition_ichol( A, n, n );
   post = vul_timer_get_micros( t );
   printf("GMRES - ILDDT(0) decompose %lu.%3lums\n", (post-pre)/1000, (post-pre)%1000);
   pre = vul_timer_get_micros( t );
   x = vul_linalg_gmres_sparse( A, guess, b, P, VUL_LINALG_PRECONDITIONER_INCOMPLETE_CHOLESKY, gmres_restart, gmres_iters, eps );
   post = vul_timer_get_micros( t );
   printf("GMRES - ILDDT(0) solve %lu.%3lums\n", (post-pre)/1000, (post-pre)%1000);
   vulb__sparse_vclear( e );
   vulb__sparse_mmul( e, A, x );
   vulb__sparse_vsub( e, b, e );
   printf("Residual: %e\n", sqrt( vulb__sparse_dot( e, e ) ) );
   if( r ) {
      for( int i = 0; i < n; ++i ) {
         if( fabs( vul_linalg_vector_get( x, i ) - vul_linalg_vector_get( r, i ) ) > 1e-2 ) {
            printf( "At least one error is large: %f != %f (idx %d)\n", vul_linalg_vector_get( x, i ), vul_linalg_vector_get( r, i ), i );
         }
      }
      vulb__sparse_vsub( x, x, r );
      printf( "Metrics GMRES - ILDDT(0): err2: %e, normed %e\n", vulb__sparse_dot( x, x ), sqrt( vulb__sparse_dot( x, x ) ) / sqrt( vulb__sparse_dot( r, r ) ) );
      vul_linalg_vector_destroy( x );
   } else {
      r = x;
   }
   vul_linalg_matrix_destroy( P );
   printf("\n");fflush( stdout );
#endif

#ifdef TEST_CG_NONE
   // CG - no preconditioner
   pre = vul_timer_get_micros( t );
   x = vul_linalg_conjugate_gradient_sparse( A, guess, b, NULL, VUL_LINALG_PRECONDITIONER_NONE, iters, eps );
   post = vul_timer_get_micros( t );
   printf("CG - NONE solve %lu.%3lums\n", (post-pre)/1000, (post-pre)%1000);
   vulb__sparse_vclear( e );
   vulb__sparse_mmul( e, A, x );
   vulb__sparse_vsub( e, b, e );
   printf("Residual: %e\n", sqrt( vulb__sparse_dot( e, e ) ) );
   if( r ) {
      for( int i = 0; i < n; ++i ) {
         if( fabs( vul_linalg_vector_get( x, i ) - vul_linalg_vector_get( r, i ) ) > 1e-2 ) {
            printf( "At least one error is large: %f != %f (idx %d)\n", vul_linalg_vector_get( x, i ), vul_linalg_vector_get( r, i ), i );
         }
      }
      vulb__sparse_vsub( x, x, r );
      printf( "Metrics CG - NONE: err2: %e, normed %e\n", vulb__sparse_dot( x, x ), sqrt( vulb__sparse_dot( x, x ) ) / sqrt( vulb__sparse_dot( r, r ) ) );
      vul_linalg_vector_destroy( x );
   } else {
      r = x;
   }
   printf("\n");fflush( stdout );
#endif

   // CG - Jacobi
#ifdef TEST_CG_JACOBI
   pre = vul_timer_get_micros( t );
   P = vul_linalg_precondition_jacobi( A, n, n );
   post = vul_timer_get_micros( t );
   printf("CG - Jacobi decompose %lu.%3lums\n", (post-pre)/1000, (post-pre)%1000);
   pre = vul_timer_get_micros( t );
   x = vul_linalg_conjugate_gradient_sparse( A, guess, b, P, VUL_LINALG_PRECONDITIONER_JACOBI, iters, eps );
   post = vul_timer_get_micros( t );
   printf("CG - Jacobi solve %lu.%3lums\n", (post-pre)/1000, (post-pre)%1000);
   vulb__sparse_vclear( e );
   vulb__sparse_mmul( e, A, x );
   vulb__sparse_vsub( e, b, e );
   printf("Residual: %e\n", sqrt( vulb__sparse_dot( e, e ) ) );
   if( r ) {
      for( int i = 0; i < n; ++i ) {
         if( fabs( vul_linalg_vector_get( x, i ) - vul_linalg_vector_get( r, i ) ) > 1e-2 ) {
            printf( "At least one error is large: %f != %f (idx %d)\n", vul_linalg_vector_get( x, i ), vul_linalg_vector_get( r, i ), i );
         }
      }
      vulb__sparse_vsub( x, x, r );
      printf( "Metrics CG - Jacobi: err2: %e, normed %e\n", vulb__sparse_dot( x, x ), sqrt( vulb__sparse_dot( x, x ) ) / sqrt( vulb__sparse_dot( r, r ) ) );
      vul_linalg_vector_destroy( x );
   } else {
      r = x;
   }
   vul_linalg_matrix_destroy( P );
   printf("\n");fflush( stdout );
#endif

   // CG - ILU
#ifdef TEST_CG_ILU0
   pre = vul_timer_get_micros( t );
   if( !ILU ) {
      ILU = vul_linalg_precondition_ilu0( A, n, n );
   }
   post = vul_timer_get_micros( t );
   printf("CG - ILU(0) decompose %lu.%3lums\n", (post-pre)/1000, (post-pre)%1000);
   pre = vul_timer_get_micros( t );
   x = vul_linalg_conjugate_gradient_sparse( A, guess, b, ILU, VUL_LINALG_PRECONDITIONER_INCOMPLETE_LU_0, iters, eps );
   post = vul_timer_get_micros( t );
   printf("CG - ILU(0) solve %lu.%3lums\n", (post-pre)/1000, (post-pre)%1000);
   vulb__sparse_vclear( e );
   vulb__sparse_mmul( e, A, x );
   vulb__sparse_vsub( e, b, e );
   printf("Residual: %e\n", sqrt( vulb__sparse_dot( e, e ) ) );
   if( r ) {
      for( int i = 0; i < n; ++i ) {
         if( fabs( vul_linalg_vector_get( x, i ) - vul_linalg_vector_get( r, i ) ) > 1e-2 ) {
            printf( "At least one error is large: %f != %f (idx %d)\n", vul_linalg_vector_get( x, i ), vul_linalg_vector_get( r, i ), i );
         }
      }
      vulb__sparse_vsub( x, x, r );
      printf( "Metrics CG - ILU(0): err2: %e, normed %e\n", vulb__sparse_dot( x, x ), sqrt( vulb__sparse_dot( x, x ) ) / sqrt( vulb__sparse_dot( r, r ) ) );
      vul_linalg_vector_destroy( x );
   } else {
      r = x;
   }
   printf("\n");fflush( stdout );
#endif

   // CG - ILDDT(0)
#ifdef TEST_CG_ILDTT0
   pre = vul_timer_get_micros( t );
   P = vul_linalg_precondition_ichol( A, n, n );
   post = vul_timer_get_micros( t );
   printf("CG - ILDDT(0) decompose %lu.%3lums\n", (post-pre)/1000, (post-pre)%1000);
   pre = vul_timer_get_micros( t );
   x = vul_linalg_conjugate_gradient_sparse( A, guess, b, P, VUL_LINALG_PRECONDITIONER_INCOMPLETE_CHOLESKY, iters, eps );
   post = vul_timer_get_micros( t );
   printf("CG - ILDTT(0) solve %lu.%3lums\n", (post-pre)/1000, (post-pre)%1000);
   vulb__sparse_vclear( e );
   vulb__sparse_mmul( e, A, x );
   vulb__sparse_vsub( e, b, e );
   printf("Residual: %e\n", sqrt( vulb__sparse_dot( e, e ) ) );
   if( r ) {
      for( int i = 0; i < n; ++i ) {
         if( fabs( vul_linalg_vector_get( x, i ) - vul_linalg_vector_get( r, i ) ) > 1e-2 ) {
            printf( "At least one error is large: %f != %f (idx %d)\n", vul_linalg_vector_get( x, i ), vul_linalg_vector_get( r, i ), i );
         }
      }
      vulb__sparse_vsub( x, x, r );
      printf( "Metrics CG - ILDDT(0): err2: %e, normed %e\n", vulb__sparse_dot( x, x ), sqrt( vulb__sparse_dot( x, x ) ) / sqrt( vulb__sparse_dot( r, r ) ) );
      vul_linalg_vector_destroy( x );
   } else {
      r = x;
   }
   vul_linalg_matrix_destroy( P );
   printf("\n");fflush( stdout );

#endif

   if( ILU ) {
      vul_linalg_matrix_destroy( ILU );
   }
   vul_linalg_vector_destroy( e );
   vul_linalg_vector_destroy( guess );
   test_dense( A, b, n, d, eps, iters, gmres_restart, gmres_iters, t );
   vul_linalg_matrix_destroy( A );
   vul_linalg_vector_destroy( b );

   vul_timer_destroy( t );
}
