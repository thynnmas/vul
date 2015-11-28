#define VUL_DEFINE
//#define VUL_LINEAR_SOLVERS_ROW_MAJOR
//#define VUL_LINEAR_SOLVERS_ALLOC malloc
//#define VUL_LINEAR_SOLVERS_FREE free

#include "../vul_linear_solvers.h"

#define CHECK_WITHIN_EPS( a, b, n, eps )\
	{for( int i = 0; i < ( n ); ++i ) {\
		assert( fabs( ( a )[ i ] - ( b )[ i ] ) < ( eps ) );\
	}}

void vul_test_linear_solvers( ) {

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
	
	vul_solve_lu_decomposition_dense( x, A, guess, b, 3, iters, eps );
	CHECK_WITHIN_EPS( x, solution, 3, 1e-8f );
	
	vul_solve_cholesky_decomposition_dense( x, A, guess, b, 3, iters, eps );
	CHECK_WITHIN_EPS( x, solution, 3, 1e-7f );
	
	vul_solve_qr_decomposition_dense( x, A, guess, b, 3, iters, eps );
	CHECK_WITHIN_EPS( x, solution, 3, 1e-8f );
	
	vul_solve_successive_over_relaxation_dense( x, A, guess, b, 1.1f, 3, iters, eps );
	CHECK_WITHIN_EPS( x, solution, 3, 1e-5f );
}