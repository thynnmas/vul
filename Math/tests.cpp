/*
 * Villains' Utility Library - Thomas Martin Schmid, 2014. Public domain¹
 *
 * @TODO: Proper tests:
 * We try to test all categories for all cases, and all edge cases 
 * against static, precalculated results. In  addition, we use the
 * current system time as a seed and test a not insignificant number
 * of random cases against a reference implementation (glm as of now).
 * The hope is that everyone runs the tests a few runs before trusting 
 * our library and that we get good coverage this way.
 * If the fuzzing fails, the seed is provided in output; please supply
 * this with the bug report so we can reproduce.
 * 
 * ¹ If public domain is not legally valid in your country and or legal area,
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

#ifdef VUL_TEST

#define NOMINMAX

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#define VUL_DEFINE
#include "vul_math.hpp"

#include "Tests/test_aabb.hpp"
#include "Tests/test_affine.hpp"
#include "Tests/test_aosoa.hpp"
#include "Tests/test_fixed.hpp"
#include "Tests/test_half.hpp"
#include "Tests/test_matrix.hpp"
#include "Tests/test_point.hpp"
#include "Tests/test_quaternion.hpp"
#include "Tests/test_vector.hpp"

using namespace vul;
using namespace vul_test;

int main( int argc, char **argv )
{
	/*
	printf( "\nTesting half precision format\n" );
	test_half( );
	printf( "\nTesting fixed point formats\n" );
	test_fixed( );
	printf( "\nTesting vectors\n" );
	test_vector( );
	printf( "\nTesting points\n" );
	test_point( );
	printf( "\nTesting matrices\n" );
	test_matrix( );
	printf( "\nTesting affine transformations\n" );
	test_affine( );
	printf( "\nTesting AABBs\n" );
	test_aabb( );
	// @TODO: Test quaternions
	printf( "\nTesting quaternions\n" );
	test_quats( );

	printf( "\nBenchmarking AABB transforms\n" );
	bench_aabbs( );
	*/
	TestAABB::test( );
	TestAffine::test( );
	TestAOSOA::test( );
	TestFixed::test( );
	TestHalf::test( );
	TestMatrix::test( );
	TestPoint::test( );
	TestQuaternion::test( );
	TestVector::test( );

	printf( "\nDone. Enter to close.\n" );
	// Wait for input
	char buffer[ 1024 ];
	gets_s( buffer, 1024 );

	return 0;
}

#endif