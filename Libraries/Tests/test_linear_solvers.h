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

void vul_test_linear_solvers( ) {
	vul__test_linear_solvers_dense( );
	puts("Dense solvers work.");
	vul__test_linear_solvers_sparse( );
	puts("Sparse solvers work.");
}

