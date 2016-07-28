/*
 * Villains' Utility Library - Thomas Martin Schmid, 2016. Public domain?
 *
 * This file contains tests for the linear solvers in vul_linear.hpp
 * It compares results against expected results based on offline math,
 * @TODO(thynn): as well as reference implementations (ceres, eigen).
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
#ifndef VUL_TEST_LINEAR_HPP
#define VUL_TEST_LINEAR_HPP

#include <cassert>

#include "../vul_linear.hpp"

#define VUL_TEST_RNG ( float )( ( float )rand( ) / ( float )RAND_MAX )

using namespace vul;

namespace vul_test {
	class TestLinear {

	public:
		static bool test( );

	private:
		static bool conjugateGradient( );
		static bool luDecomposition( );
		static bool choleskyDecomposition( );
		static bool qrDecomposition( );
		static bool successiveOverRelaxation( );
	};

	bool TestLinear::test( )
	{
		assert( conjugateGradient( ) );
		assert( luDecomposition( ) );
		assert( choleskyDecomposition( ) );
		assert( qrDecomposition( ) );
		assert( successiveOverRelaxation( ) );
		
		return true;
	}
	

	#define testWithinEpsilon( x, s, n, eps )\
		for( int i = 0; i < n; ++i ) {\
			printf("%f = %f, %f\n", x[i], s[i], fabs(x[i]-s[i]));\
			assert( fabs( x[ i ] - s[ i ] ) < eps );\
		}\

	bool TestLinear::conjugateGradient( )
	{
		Matrix< f32, 3, 3 > A = makeMatrix33< f32 >( 25, 15, -5,
														 15, 18, 0,
														 -5, 0, 11 );
		Vector< f32, 3 > b = makeVector< f32 >( 1, 3, 5 );
		f32 eps = 1e-10f;
		s32 iters = 32;
		Vector< f32, 3 > init = makeVector< f32, 3 >( 0.f );

		Vector< f32, 3 > x = solveConjugateGradient( A, init, b, iters, eps );
		
		Vector< f32, 3 > solution = makeVector< f32 >( 17.f/ 255.f, 14.f / 135.f, 22.f / 45.f );
		testWithinEpsilon( x, solution, 3, 1e-7f );

		return true;
	}
	bool TestLinear::luDecomposition( )
	{
		Matrix< f32, 3, 3 > A = makeMatrix33< f32 >( 25, 15, -5,
														 15, 18, 0,
														 -5, 0, 11 );
		Vector< f32, 3 > b = makeVector< f32 >( 1, 3, 5 );
		f32 eps = 1e-10f;
		s32 iters = 32;
		Vector< f32, 3 > init = makeVector< f32, 3 >( 0.f );

		Vector< f32, 3 > x = solveLUDecomposition( A, init, b, iters, eps );
		
		Vector< f32, 3 > solution = makeVector< f32 >( 17.f/ 255.f, 14.f / 135.f, 22.f / 45.f );
		testWithinEpsilon( x, solution, 3, 1e-8f );
		
		return true;
	}
	bool TestLinear::choleskyDecomposition( )
	{
		Matrix< f32, 3, 3 > A = makeMatrix33< f32 >( 25, 15, -5,
														 15, 18, 0,
														 -5, 0, 11 );
		Vector< f32, 3 > b = makeVector< f32 >( 1, 3, 5 ); // @TODO(thynn): This should make sense <--
		f32 eps = 1e-10f;
		s32 iters = 32;
		Vector< f32, 3 > init = makeVector< f32, 3 >( 0.f );

		Vector< f32, 3 > x = solveCholeskyDecomposition( A, init, b, iters, eps );
		
		Vector< f32, 3 > solution = makeVector< f32 >( 17.f/ 255.f, 14.f / 135.f, 22.f / 45.f );
		testWithinEpsilon( x, solution, 3, 1e-7f );
		
		return true;
	}
	bool TestLinear::qrDecomposition( )
	{
		Matrix< f32, 3, 3 > A = makeMatrix33< f32 >( 25, 15, -5,
														 15, 18, 0,
														 -5, 0, 11 );
		Vector< f32, 3 > b = makeVector< f32 >( 1, 3, 5 );
		f32 eps = 1e-10f;
		s32 iters = 32;
		Vector< f32, 3 > init = makeVector< f32, 3 >( 0.f );

		Vector< f32, 3 > x = solveQRDecomposition( A, init, b, iters, eps );
		
		Vector< f32, 3 > solution = makeVector< f32 >( 17.f/ 255.f, 14.f / 135.f, 22.f / 45.f );
		testWithinEpsilon( x, solution, 3, 1e-8f );
		
		return true;
	}
	bool TestLinear::successiveOverRelaxation( )
	{
		Matrix< f32, 3, 3 > A = makeMatrix33< f32 >( 25, 15, -5,
														 15, 18, 0,
														 -5, 0, 11 );
		Vector< f32, 3 > b = makeVector< f32 >( 1, 3, 5 );
		f32 eps = 1e-10f;
		s32 iters = 32;
		Vector< f32, 3 > init = makeVector< f32, 3 >( 0.f );

		Vector< f32, 3 > x = solveSOR( A, init, b, 1.1f, iters, eps );
		
		Vector< f32, 3 > solution = makeVector< f32 >( 17.f/ 255.f, 14.f / 135.f, 22.f / 45.f );
		testWithinEpsilon( x, solution, 3, 1e-5 );
		
		return true;
	}
};

#undef VUL_TEST_RNG

#endif
