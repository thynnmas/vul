#define VUL_DEFINE

#include <stdio.h>

#define TEST_MIN( a, b ) ( ( a ) <= ( b ) ? ( a ) : ( b ) )
#define TEST_MAX( a, b ) ( ( a ) >= ( b ) ? ( a ) : ( b ) )

#define CHECK_WITHIN_EPS( a, b, n, eps )\
	{for( int ppi = 0; ppi < ( n ); ++ppi ) {\
		assert( fabs( ( a )[ ppi ] - ( b )[ ppi ] ) < TEST_MAX( eps, ( b )[ ppi ] * eps ) );\
	}}
#define PRINT_VECTOR( name, v, n )\
	printf("%s [", name);\
	{for( int ppi = 0; ppi < ( n ); ++ppi ) {\
		printf( ( ppi == ( n )- 1 ) ? "%f" : "%f, ", ( v )[ ppi ] );\
	}}\
	printf("]\n");
#define PRINT_MATRIX( name, m, c, r )\
	printf("%s\n", name);\
	{for( int ppi = 0; ppi < ( r ); ++ppi ) {\
		printf("[");\
		{for( int ppj = 0; ppj < ( c ); ++ppj ) {\
			printf( ( ppj == ( c )- 1 ) ? "%f" : "%f, ", ( m )[ ppi * c + ppj ] );\
		}}\
		printf("]\n");\
	}}
#define PRINT_MATRIX_SPARSE( name, m, c, r )\
	printf("%s\n", name);\
	{for( int ppi = 0; ppi < ( r ); ++ppi ) {\
		printf("[");\
		{for( int ppj = 0; ppj < ( c ); ++ppj ) {\
			printf( ( ppj == ( c ) - 1 ) ? "%f" : "%f, ", vul_solve_matrix_get( m, ppi, ppj ) );\
		}}\
		printf("]\n");\
	}}
#define CHECK_WITHIN_EPS_SPARSE( a, b, n, eps )\
	{for( int ppi = 0; ppi < ( n ); ++ppi ) {\
		assert( fabs( vul_solve_vector_get( a, ppi ) - vul_solve_vector_get( b, ppi ) ) < ( eps ) );\
	}}
#define PRINT_VECTOR_SPARSE( name, v, n )\
	printf("%s [", name);\
	{for( int ppi = 0; ppi < ( n ); ++ppi ) {\
		printf( ( ppi == ( n )- 1 ) ? "%f" : "%f, ", vul_solve_vector_get( v, ppi ) );\
	}}\
	printf("]\n");

//#define VUL_SOLVE_DOUBLE
#define VUL_LINEAR_SOLVERS_ROW_MAJOR
//#define VUL_LINEAR_SOLVERS_ALLOC malloc
//#define VUL_LINEAR_SOLVERS_FREE free
#include "../vul_linear_solvers.h"
#ifdef VUL_SOLVE_DOUBLE
typedef double real;
#else
typedef float real;
#endif

void vul__test_linear_solvers_dense( )
{
	real eps = 1e-10f;
	int iters = 32;

	real A[ 3 * 3 ] = { 25.f, 15.f, -5.f,
							  15.f, 18.f,  0.f,
							  -5.f, 0.f,  11.f };
	real b[ 3 ] = { 1.f, 3.f, 5.f };
	real x[ 3 ], guess[ 3 ] = { 0.f, 0.f, 0.f };
	real solution[ 3 ] = { 17.f / 225.f, 14.f / 135.f,  22.f/ 45.f };

	vul_solve_conjugate_gradient_dense( x, A, guess, b, 3, iters, eps );
	CHECK_WITHIN_EPS( x, solution, 3, 1e-7f );

	vul_solve_lu_decomposition_dense( x, A, guess, b, 3, iters, eps );
	CHECK_WITHIN_EPS( x, solution, 3, 1e-8f );

	vul_solve_cholesky_decomposition_dense( x, A, b, 3 );
	CHECK_WITHIN_EPS( x, solution, 3, 1e-7f );

	vul_solve_qr_decomposition_dense( x, A, b, 3 );
	CHECK_WITHIN_EPS( x, solution, 3, 1e-7f );

	vul_solve_successive_over_relaxation_dense( x, A, guess, b, 1.1f, 3, iters, eps );
	CHECK_WITHIN_EPS( x, solution, 3, 1e-5f );
}

void vul__test_linear_solvers_sparse( )
{
	real eps = 1e-10f;
	int iters = 32;

	vul_solve_matrix *A = vul_solve_matrix_create( 0, 0, 0, 0 );
	vul_solve_matrix_insert( A, 0, 0, 25.f );
	vul_solve_matrix_insert( A, 0, 1, 15.f );
	vul_solve_matrix_insert( A, 0, 2, -5.f );
	vul_solve_matrix_insert( A, 1, 0, 15.f );
	vul_solve_matrix_insert( A, 1, 1, 18.f );
	vul_solve_matrix_insert( A, 2, 0, -5.f );
	vul_solve_matrix_insert( A, 2, 2, 11.f );

	vul_solve_vector *b = vul_solve_vector_create( 0, 0, 0 );
	vul_solve_vector_insert( b, 0, 1.f );
	vul_solve_vector_insert( b, 1, 3.f );
	vul_solve_vector_insert( b, 2, 5.f );

	vul_solve_vector *guess = vul_solve_vector_create( 0, 0, 0 );

	vul_solve_vector *solution = vul_solve_vector_create( 0, 0, 0 );
	vul_solve_vector_insert( solution, 0, 17.f / 225.f );
	vul_solve_vector_insert( solution, 1, 14.f / 135.f );
	vul_solve_vector_insert( solution, 2, 22.f / 45.f );

	vul_solve_vector *x = vul_solve_conjugate_gradient_sparse( A, guess, b, iters, eps );
	CHECK_WITHIN_EPS_SPARSE( x, solution, 3, 1e-7f );
	vul_solve_vector_destroy( x );
	
	x = vul_solve_cholesky_decomposition_sparse( A, b, 3, 3 );
	CHECK_WITHIN_EPS_SPARSE( x, solution, 3, 1e-7f );
	vul_solve_vector_destroy( x );
#if 0
	vul_solve_lu_decomposition_dense( x, A, guess, b, 3, iters, eps );
	CHECK_WITHIN_EPS( x, solution, 3, 1e-8f );
#endif
	
	x = vul_solve_qr_decomposition_sparse( A, b, 3, 3 );
	CHECK_WITHIN_EPS_SPARSE( x, solution, 3, 1e-7f );
	vul_solve_vector_destroy( x );

	x = vul_solve_successive_over_relaxation_sparse( A, guess, b, 1.1f, iters, eps );
	CHECK_WITHIN_EPS_SPARSE( x, solution, 3, 1e-5f );

	vul_solve_matrix_destroy( A );
	vul_solve_vector_destroy( b );
	vul_solve_vector_destroy( guess );
	vul_solve_vector_destroy( solution );
	vul_solve_vector_destroy( x );
}

void vul__test_svd_sparse( )
{
	vul_solve_svd_basis_sparse res[ 15 ];
	memset( res, 0, sizeof( vul_solve_svd_basis_sparse ) * 15 );
	int rank = 0;
	vul_solve_matrix *A = vul_solve_matrix_create( 0, 0, 0, 0 );
	vul_solve_matrix_insert( A, 0, 0, 2.f );
	vul_solve_matrix_insert( A, 0, 2, 8.f );
	vul_solve_matrix_insert( A, 0, 3, 6.f );
	
	vul_solve_matrix_insert( A, 1, 0, 1.f );
	vul_solve_matrix_insert( A, 1, 1, 6.f );
	vul_solve_matrix_insert( A, 1, 3, 1.f );
	vul_solve_matrix_insert( A, 1, 4, 7.f );

	vul_solve_matrix_insert( A, 2, 0, 5.f );
	vul_solve_matrix_insert( A, 2, 2, 7.f );
	vul_solve_matrix_insert( A, 2, 3, 4.f );

	vul_solve_matrix_insert( A, 3, 0, 7.f );
	vul_solve_matrix_insert( A, 3, 2, 8.f );
	vul_solve_matrix_insert( A, 3, 3, 5.f );

	vul_solve_matrix_insert( A, 4, 1, 10.f );
	vul_solve_matrix_insert( A, 4, 4, 7.f );

	vul_solve_svd_sparse( res, &rank, A, 5, 5, 1e-7, 32 );
	assert( rank == 5 );
	assert( fabs( res[ 0 ].sigma - 17.9173f ) < 1e-2 );
	assert( fabs( res[ 1 ].sigma - 15.1722f ) < 1e-2 );
	assert( fabs( res[ 2 ].sigma - 3.5639f ) < 1e-2 );
	assert( fabs( res[ 3 ].sigma - 1.9843f ) < 1e-2 );
	assert( fabs( res[ 4 ].sigma - 0.3496f ) < 1e-2 );
	vul_solve_matrix *R0 = vul_solve_svd_basis_reconstruct_matrix_sparse( res, rank );
	for( int k = 0; k < 5; ++k ) {
		CHECK_WITHIN_EPS_SPARSE( &R0->rows[ k ].vec, &A->rows[ k ].vec, 5, 1e-1 );
	}
	vul_solve_matrix_destroy( R0 );
	vul_solve_svd_basis_destroy_sparse( res, rank );

	vul_solve_matrix *A2 = vul_solve_matrix_create( 0, 0, 0, 0 );
	vul_solve_matrix_insert( A2, 0, 0, 1.f );
	vul_solve_matrix_insert( A2, 0, 4, 2.f );
	vul_solve_matrix_insert( A2, 1, 2, 3.f );
	vul_solve_matrix_insert( A2, 3, 1, 2.f );
	vul_solve_matrix *A3 = vul_solve_matrix_create( 0, 0, 0, 0 );
	vull__sparse_mtranspose( A3, A2 );
	vul_solve_svd_sparse( res, &rank, A2, 5, 4, 1e-10, 32 );
	assert( rank == 3 ); // Check that we got back the rank we wanted
	assert( fabs( res[ 0 ].sigma - 3.f ) < 1e-5 );
	assert( fabs( res[ 1 ].sigma - sqrtf( 5.f ) ) < 1e-5 );
	assert( fabs( res[ 2 ].sigma - 2.f ) < 1e-5 );

	R0 = vul_solve_svd_basis_reconstruct_matrix_sparse( res, rank );
	assert( R0->count == A2->count );
	for( int k = 0; k < R0->count; ++k ) {
		CHECK_WITHIN_EPS_SPARSE( &R0->rows[ k ].vec, &A2->rows[ k ].vec, 5, 1e-1 );
	}
	vul_solve_matrix_destroy( R0 );
	vul_solve_svd_basis_destroy_sparse( res, rank );

	vul_solve_matrix_destroy( A );
	vul_solve_matrix_destroy( A2 );
	vul_solve_matrix_destroy( A3 );
}

void vul__test_svd_dense( )
{
	vul_solve_svd_basis res[ 15 ];
	memset( res, 0, sizeof( vul_solve_svd_basis ) * 15 );
	int rank = 0;
	real A[ 15 * 25 ] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
								 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
								 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
								 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
								 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
								 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
								 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
								 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
								 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1,
								 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1,
								 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1,
								 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1,
								 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1,
								 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1,
								 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1,
								 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1,
								 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1,
								 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
								 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
								 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
								 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
								 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
								 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
								 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
								 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
	rank = 3;
	vul_solve_svd_dense( res, &rank, A, 15, 25, 1e-7, 32 );
	assert( rank == 3 );
	assert( fabs( res[ 0 ].sigma - 14.72f ) < 1e-2 );
	assert( fabs( res[ 1 ].sigma - 5.22f ) < 1e-2 );
	assert( fabs( res[ 2 ].sigma - 3.31f ) < 1e-2 );
	real RA1[ 15 * 25 ];
	vul_solve_svd_basis_reconstruct_matrix( RA1, res, rank );
	CHECK_WITHIN_EPS( RA1, A, 15 * 25, 1e-3 );
	vul_solve_svd_basis_destroy( res, rank );
	
	real A2[ 5 * 5 ] = { 2,  0, 8, 6, 0,
								1,  6, 0, 1, 7,
								5,  0, 7, 4, 0,
								7,  0, 8, 5, 0,
								0, 10, 0, 0, 7 };
	rank = 0;
	vul_solve_svd_dense( res, &rank, A2, 5, 5, 1e-7, 8 ); // Higher iteration count introduces error with givens rotations!
	assert( rank == 5 );
	assert( fabs( res[ 0 ].sigma - 17.9173f ) < 1e-2 );
	assert( fabs( res[ 1 ].sigma - 15.1722f ) < 1e-2 );
	assert( fabs( res[ 2 ].sigma - 3.5639f ) < 1e-2 );
	assert( fabs( res[ 3 ].sigma - 1.9843f ) < 1e-2 );
	assert( fabs( res[ 4 ].sigma - 0.3496f ) < 1e-2 );
	real R0[ 5 * 5 ];
	vul_solve_svd_basis_reconstruct_matrix( R0, res, rank );
	CHECK_WITHIN_EPS( R0, A2, 5 * 5, 1e-1 );
	vul_solve_svd_basis_destroy( res, rank );
	
	real A3[ 4 * 5 ] = { 1, 0, 0, 0, 2,
								 0, 0, 3, 0, 0,
								 0, 0, 0, 0, 0,
								 0, 2, 0, 0, 0 };
	vul_solve_svd_dense( res, &rank, A3, 5, 4, 1e-10, 32 );
	assert( rank == 3 ); // Check that we got back the rank we wanted
	assert( fabs( res[ 0 ].sigma - 3.f ) < 1e-5 );
	assert( fabs( res[ 1 ].sigma - sqrtf( 5.f ) ) < 1e-5 );
	assert( fabs( res[ 2 ].sigma - 2.f ) < 1e-5 );

	vul_solve_svd_basis_reconstruct_matrix( R0, res, rank );
	CHECK_WITHIN_EPS( R0, A3, 5 * 4, 1e-1 );
	vul_solve_svd_basis_destroy( res, rank );

	// Test jacobi
	rank = 3;
	vul_solve_svd_dense( res, &rank, A, 15, 25, 1e-7, 32 );
	assert( rank == 3 );
	assert( fabs( res[ 0 ].sigma - 14.72f ) < 1e-2 );
	assert( fabs( res[ 1 ].sigma - 5.22f ) < 1e-2 );
	assert( fabs( res[ 2 ].sigma - 3.31f ) < 1e-2 );
	vul_solve_svd_basis_reconstruct_matrix( RA1, res, rank );
	CHECK_WITHIN_EPS( RA1, A, 15 * 25, 1e-3 );
	vul_solve_svd_basis_destroy( res, rank );

	rank = 0;
	vul_solve_svd_dense_jacobi( res, &rank, A2, 5, 5, 1e-7, 8 );
	assert( rank == 5 );
	assert( fabs( res[ 0 ].sigma - 17.9173f ) < 1e-2 );
	assert( fabs( res[ 1 ].sigma - 15.1722f ) < 1e-2 );
	assert( fabs( res[ 2 ].sigma - 3.5639f ) < 1e-2 );
	assert( fabs( res[ 3 ].sigma - 1.9843f ) < 1e-2 );
	assert( fabs( res[ 4 ].sigma - 0.3496f ) < 1e-2 );
	vul_solve_svd_basis_reconstruct_matrix( R0, res, rank );
	CHECK_WITHIN_EPS( R0, A2, 5 * 5, 1e-1 );
	vul_solve_svd_basis_destroy( res, rank );

	rank = 0;
	vul_solve_svd_dense_jacobi( res, &rank, A3, 5, 4, 1e-10, 32 );
	assert( rank == 3 ); // Check that we got back the rank we wanted
	assert( fabs( res[ 0 ].sigma - 3.f ) < 1e-2 );
	assert( fabs( res[ 1 ].sigma - sqrtf( 5.f ) ) < 1e-2 );
	assert( fabs( res[ 2 ].sigma - 2.f ) < 1e-2 );

	vul_solve_svd_basis_reconstruct_matrix( R0, res, rank );
	CHECK_WITHIN_EPS( R0, A3, 5 * 4, 1e-1 );
	vul_solve_svd_basis_destroy( res, rank );
}

void vul__test_eigenvalues( ) {
	real H[ 4 * 4 ] = { 1, 2, 3, 4,
								2, 6, 7, 8,
								3, 7, 0, 0,
								4, 8, 0, 1 };
	real solution = 15.756757465243327;
	real eps = 1e-6;
	real v = vul_solve_largest_eigenvalue( H, 4, 4, 1e-7, 32 );
	assert( fabs( v - solution ) < eps );
}

void vul__test_qr_decomposition( ) {
	// Square
	real A[ 3 * 3 ] = { 12, -51,   4,
								 6, 167, -68,
								-4,  24, -41 };
	real B[ 3 * 3 ] = { 12,   6,  -4,
							  -51, 167,  24,
								 4, -68, -41 };
	real Q[ 3 * 3 ], R[ 3 * 3 ];
	real sQ[ 3 * 3 ] = { 6.f / 7.f, -69.f / 175.f, -58.f / 175.f,
								 3.f / 7.f, 158.f / 175.f,   6.f / 175.f,
								-2.f / 7.f,   6.f /  35.f, -33.f /  35.f };
	real sR[ 3 * 3 ] = { 14,  21, -14,
								  0, 175, -70,
								  0,   0,  35 };
	real M[ 3 * 3 ];
	
	vul__solve_qr_decomposition_gram_schmidt( Q, R, A, 3, 3, 0 ); 
	CHECK_WITHIN_EPS( sQ, Q, 3 * 3, 1e-7 );
	CHECK_WITHIN_EPS( sR, R, 3 * 3, 1e-5 );
	vull__mmul_matrix( M, Q, R, 3 );
	CHECK_WITHIN_EPS( M, A, 3 * 3, 1e-5 );

	vul__solve_qr_decomposition_gram_schmidt( Q, R, B, 3, 3, 1 ); 
	CHECK_WITHIN_EPS( sQ, Q, 3 * 3, 1e-7 );
	CHECK_WITHIN_EPS( sR, R, 3 * 3, 1e-5 );
	vull__mmul_matrix( M, Q, R, 3 );
	CHECK_WITHIN_EPS( M, A, 3 * 3, 1e-5 );

	real sQH[ 3 * 3 ] = { -6.f / 7.f,   69.f / 175.f,  58.f / 175.f,
								  -3.f / 7.f, -158.f / 175.f,  -6.f / 175.f,
								   2.f / 7.f,   -6.f /  35.f,  33.f /  35.f };
	real sRH[ 3 * 3 ] = { -14,  -21,  14,
								    0, -175,  70,
								    0,    0, -35 };
	vul__solve_qr_decomposition_householder( Q, R, A, 3, 3, 0 );
	CHECK_WITHIN_EPS( sQH, Q, 3 * 3, 1e-3 );
	CHECK_WITHIN_EPS( sRH, R, 3 * 3, 1e-1 );
	vull__mmul_matrix( M, Q, R, 3 );
	CHECK_WITHIN_EPS( M, A, 3 * 3, 1e-3 );

	vul__solve_qr_decomposition_householder( Q, R, B, 3, 3, 1 );
	CHECK_WITHIN_EPS( sQH, Q, 3 * 3, 1e-3 );
	CHECK_WITHIN_EPS( sRH, R, 3 * 3, 1e-1 );
	vull__mmul_matrix( M, Q, R, 3 );
	CHECK_WITHIN_EPS( M, A, 3 * 3, 1e-3 );

	vul__solve_qr_decomposition_givens( Q, R, A, 3, 3, 0 );
	vull__mmul_matrix( M, Q, R, 3 );	
	CHECK_WITHIN_EPS( M, A, 3 * 3, 1e-4 );

	vul__solve_qr_decomposition_givens( Q, R, B, 3, 3, 1 );
	vull__mmul_matrix( M, Q, R, 3 );
	CHECK_WITHIN_EPS( M, A, 3 * 3, 1e-4 );

	// Non-square
	real C[ 4 * 5 ] = { 1, 0, 0, 0, 2,
								0, 0, 3, 0, 0,
								0, 0, 0, 0, 0,
								0, 2, 0, 0, 0 };
	real sCQ[ 4 * 4 ] = { 1,  0,  0, 0,
								  0,  0, -1, 0,
								  0,  0,  0, 1,
								  0, -1, 0, 0 };
	real sCR[ 4 * 5 ] = { 1,  0,  0, 0, 2,
								  0, -2,  0, 0, 0,
								  0,  0, -3, 0, 0,
								  0,  0,  0, 0, 0 };
	real CQ[ 5 * 5 ], CR[ 5 * 4 ], CM[ 4 * 5 ];
	vul__solve_qr_decomposition_gram_schmidt( CQ, CR, C, 5, 4, 0 );
	vull__mmul_matrix_rect( CM, CQ, CR, 4, 4, 5 );
	CHECK_WITHIN_EPS( CM, C, 4 * 5, 1e-3 );
	vul__solve_qr_decomposition_householder( CQ, CR, C, 5, 4, 0 );
	vull__mmul_matrix_rect( CM, CQ, CR, 4, 4, 5 );
	CHECK_WITHIN_EPS( CM, C, 4 * 5, 1e-3 );
	vul__solve_qr_decomposition_givens( CQ, CR, C, 5, 4, 0 );
	vull__mmul_matrix_rect( CM, CQ, CR, 4, 4, 5 );
	CHECK_WITHIN_EPS( CM, C, 4 * 5, 1e-3 );

	real CT[ 4 * 5 ];
	vull__mtranspose( CT, C, 5, 4 );
	memset( CQ, 0, sizeof( real ) * 5 * 5 );
	memset( CR, 0, sizeof( real ) * 5 * 4 );
	memset( CM, 0, sizeof( real ) * 5 * 4 );
	vul__solve_qr_decomposition_gram_schmidt( CQ, CR, C, 5, 4, 1 );
	vull__mmul_matrix_rect( CM, CQ, CR, 5, 5, 4 );
	CHECK_WITHIN_EPS( CM, CT, 4 * 5, 1e-3 );
	vul__solve_qr_decomposition_householder( CQ, CR, C, 5, 4, 1 );
	vull__mmul_matrix_rect( CM, CQ, CR, 5, 5, 4 );
	CHECK_WITHIN_EPS( CM, CT, 4 * 5, 1e-3 );
	vul__solve_qr_decomposition_givens( CQ, CR, C, 5, 4, 1 );
	vull__mmul_matrix_rect( CM, CQ, CR, 5, 5, 4 );
	CHECK_WITHIN_EPS( CM, CT, 4 * 5, 1e-3 );
}

void vul__test_householder( )
{
	real B[ 3 * 3 ] = {  12, -51,   4,
								 6, 167, -68,
								-4,  24, -41 };
	real SB[ 3 * 3 ] = { 14,  21, -14,
								 0, -49, -14,
								 0, 168, -77 };
	real S[ 3 * 3 ];
	vul__solve_apply_householder_column( S, B, 0, 0, 3, 3, 3, 3, 0, NULL, NULL, 0 );
	CHECK_WITHIN_EPS( S, SB, 3 * 3, 1e-3 );
}

void vul__test_transpose( )
{
	real A[ 2 * 3 ] = { 1, 2, 3,
							  4, 5, 6 };
	real B[ 2 * 3 ] = { 1, 4, 
							  2, 5,
							  3, 6 };
	real C[ 2 * 3 ];
	vull__mtranspose( C, A, 3, 2 );
	CHECK_WITHIN_EPS( C, B, 3 * 2, FLT_EPSILON );
	vull__mtranspose( C, B, 2, 3 );
	CHECK_WITHIN_EPS( C, A, 3 * 2, FLT_EPSILON );
}

void vul_test_linear_solvers( ) {
	// @TODO(thynn): Test all the helpers, not just the transpose
	vul__test_transpose( );

	vul__test_linear_solvers_dense( );
	puts("Dense solvers work.");
	vul__test_linear_solvers_sparse( );
	puts("Sparse solvers work.");
	vul__test_eigenvalues( );
	puts("Eigenvalue finding works.");
	vul__test_householder( );
	puts("Householder reflection works.");
	vul__test_qr_decomposition( );
	puts("QR decomposition works.");
	vul__test_svd_sparse( );
	puts("Sparse SVD works.");
	vul__test_svd_dense( );
	puts("Dense SVD works.");
}

