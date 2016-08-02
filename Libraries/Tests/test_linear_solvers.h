#define VUL_DEFINE
//#define VUL_LINEAR_SOLVERS_ROW_MAJOR
//#define VUL_LINEAR_SOLVERS_ALLOC malloc
//#define VUL_LINEAR_SOLVERS_FREE free

#include <stdio.h>

#define CHECK_WITHIN_EPS( a, b, n, eps )\
	{for( int i = 0; i < ( n ); ++i ) {\
		assert( fabs( ( a )[ i ] - ( b )[ i ] ) < ( eps ) );\
	}}
#define PRINT_VECTOR( v, n )\
	printf("[");\
	{for( int i = 0; i < ( n ); ++i ) {\
		printf( ( i == ( n )- 1 ) ? "%f" : "%f, ", ( v )[ i ] );\
	}}\
	printf("]\n");
#define PRINT_MATRIX( m, n )\
	{for( int i = 0; i < ( n ); ++i ) {\
		printf("[");\
		{for( int j = 0; j < ( n ); ++j ) {\
			printf( ( j == ( n )- 1 ) ? "%f" : "%f, ", ( m )[ i * n + j ] );\
		}}\
		printf("]\n");\
	}}

#define CHECK_WITHIN_EPS_SPARSE( a, b, n, eps )\
	{for( int i = 0; i < ( n ); ++i ) {\
		assert( fabs( vul_solve_vector_get( a, i ) - vul_solve_vector_get( b, i ) ) < ( eps ) );\
	}}
#define PRINT_VECTOR_SPARSE( v, n )\
	printf("[");\
	{for( int i = 0; i < ( n ); ++i ) {\
		printf( ( i == ( n )- 1 ) ? "%f" : "%f, ", vul_solve_vector_get( v, i ) );\
	}}\
	printf("]\n");

#include "../vul_linear_solvers.h"

void vul__test_linear_solvers_dense( )
{
	float eps = 1e-10f;
	int iters = 32;

	float A[ 3 * 3 ] = { 25.f, 15.f, -5.f,
						 15.f, 18.f, 0.f,
						 -5.f, 0.f, 11.f };
	float b[ 3 ] = { 1.f, 3.f, 5.f };
	float x[ 3 ], guess[ 3 ] = { 0.f, 0.f, 0.f };
	float solution[ 3 ] = { 17.f / 225.f, 14.f / 135.f,  22.f/ 45.f };

	vul_solve_conjugate_gradient_dense( x, A, guess, b, 3, iters, eps );
	CHECK_WITHIN_EPS( x, solution, 3, 1e-7f );
#if 0
	vul_solve_lu_decomposition_dense( x, A, guess, b, 3, iters, eps );
	CHECK_WITHIN_EPS( x, solution, 3, 1e-8f );

	vul_solve_cholesky_decomposition_dense( x, A, guess, b, 3, iters, eps );
	CHECK_WITHIN_EPS( x, solution, 3, 1e-7f );
	
	vul_solve_qr_decomposition_dense( x, A, guess, b, 3, iters, eps );
	CHECK_WITHIN_EPS( x, solution, 3, 1e-8f );
#endif
	vul_solve_successive_over_relaxation_dense( x, A, guess, b, 1.1f, 3, iters, eps );
	CHECK_WITHIN_EPS( x, solution, 3, 1e-5f );
}

void vul__test_linear_solvers_sparse( )
{
	float eps = 1e-10f;
	int iters = 32;

	vul_solve_matrix *A = vul_solve_matrix_create( 0, 0, 0, 3, 0 );
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
#if 0
	vul_solve_lu_decomposition_dense( x, A, guess, b, 3, iters, eps );
	CHECK_WITHIN_EPS( x, solution, 3, 1e-8f );

	vul_solve_cholesky_decomposition_dense( x, A, guess, b, 3, iters, eps );
	CHECK_WITHIN_EPS( x, solution, 3, 1e-7f );
	
	vul_solve_qr_decomposition_dense( x, A, guess, b, 3, iters, eps );
	CHECK_WITHIN_EPS( x, solution, 3, 1e-8f );
#endif
	x = vul_solve_successive_over_relaxation_sparse( A, guess, b, 1.1f, iters, eps );
	CHECK_WITHIN_EPS_SPARSE( x, solution, 3, 1e-5f );
	vul_solve_vector_destroy( x );

	vul_solve_matrix_destroy( A );
	vul_solve_vector_destroy( b );
	vul_solve_vector_destroy( guess );
	vul_solve_vector_destroy( solution );
}

void vul__test_svd_dense( )
{
	float eps = 1e-10;
	int rank = 0;/*
	float A[ 15 * 35 ] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
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

	vul_solve_svd_basis res[ 15 ];
	rank = 3;
	vul_svd_dense_slow( res, &rank, A, 35, 15, eps, 32 );
	printf("Rank %d\n", rank );
	for( int i = 0; i < 15; ++i ) {
		printf("%d : [%d, %f]\n", i, res[ i ].axis, res[ i ].sigma );
	}
	PRINT_VECTOR( res[ 0 ].u, res[ 0 ].n );
	PRINT_VECTOR( res[ 1 ].u, res[ 1 ].n );
	PRINT_VECTOR( res[ 2 ].u, res[ 2 ].n );
	assert( rank == 3 );
	assert( fabs( res[ 0 ].sigma - 14.72f ) < eps );
	assert( fabs( res[ 1 ].sigma - 5.22f ) < eps );
	assert( fabs( res[ 2 ].sigma - 3.31f ) < eps );*/
	/*
	float A[ 4 * 5 ] = { 1, 0, 0, 0, 2,
								0, 0, 3, 0, 0,
								0, 0, 0, 0, 0,
								0, 2, 0, 0, 0 };
	vul_solve_svd_basis res[ 4 ];
	vul_svd_dense_slow( res, &rank, A, 4, 5, eps, 32 );
	printf("Rank %d\n", rank );
	for( int i = 0; i < 4; ++i ) {
		printf("%d : [%d, %f]\n", i, res[ i ].axis, res[ i ].sigma );
	}
	assert( rank == 3 );
	assert( fabs( res[ 0 ].sigma - 3.f ) < eps );
	assert( fabs( res[ 1 ].sigma - sqrtf( 5.f ) ) < eps );
	assert( fabs( res[ 2 ].sigma - 2.f ) < eps );
	PRINT_VECTOR( res[ 0 ].u, res[ 0 ].n );
	PRINT_VECTOR( res[ 1 ].u, res[ 1 ].n );
	PRINT_VECTOR( res[ 2 ].u, res[ 2 ].n );
*/
	float A[ 5 * 5 ] = { 2,  0, 8, 6, 0,
								1,  6, 0, 1, 7,
								5,  0, 7, 4, 0,
								7,  0, 8, 5, 0,
								0, 10, 0, 0, 7 };
	vul_solve_svd_basis res[ 5 ];
	vul_svd_dense_slow( res, &rank, A, 5, 5, eps, 3 );
	printf("Rank %d\n", rank );
	for( int i = 0; i < 5; ++i ) {
		printf("%d : [%d, %f]\n", i, res[ i ].axis, res[ i ].sigma );
	}
	for( int k = 0; k < rank; ++k ) {
		PRINT_VECTOR( res[ k ].u, res[ k ].n );
	}
}

void vul__test_eigenvalues( ) {
	float H[ 4 * 4 ] = { 1, 2, 3, 4,
								2, 6, 7, 8,
								3, 7, 0, 0,
								4, 8, 0, 1 };
	float solution = 15.756757465243;
	float eps = 1e-7;
	float v = vul__solve_largest_eigenvalue( H, 4, 1e-7, 32 );
	assert( fabs( v - solution ) < eps );
}

void vul__test_qr_decomposition( ) {
	float eps = 1e-5;
	float A[ 3 * 3 ] = { 12, -51,   4,
								 6, 167, -68,
								-4,  24, -41 };
	float sQ[ 3 * 3 ] = { 6.f / 7.f, -69.f / 175.f, -58.f / 175.f,
								 3.f / 7.f, 158.f / 175.f,   6.f / 175.f,
								-2.f / 7.f,   6.f /  35.f, -33.f /  35.f };
	float sR[ 3 * 3 ] = { 14,  21, -14,
								  0, 175, -70,
								  0,   0,  35 };
	float Q[ 3 * 3 ], R[ 3 * 3 ];
	vul__solve_qr_decomposition( Q, R, A, 3, 0 ); 
	CHECK_WITHIN_EPS( sQ, Q, 3 * 3, eps );
	CHECK_WITHIN_EPS( sR, R, 3 * 3, eps );
	float B[ 3 * 3 ] = { 12,   6,  -4,
							  -51, 167,  24,
								 4, -68, -41 };
	vul__solve_qr_decomposition( Q, R, B, 3, 1 ); 
	CHECK_WITHIN_EPS( sQ, Q, 3 * 3, eps );
	CHECK_WITHIN_EPS( sR, R, 3 * 3, eps );
}

void vul_test_linear_solvers( ) {
	vul__test_linear_solvers_dense( );
	puts("Dense solvers work.");
	vul__test_linear_solvers_sparse( );
	puts("Sparse solvers work.");
	vul__test_eigenvalues( );
	puts("Eigenvalue finding works.");
	vul__test_qr_decomposition( );
	puts("QR decomposition works.");
	vul__test_svd_dense( );
	puts("Dense SVD works.");
}

