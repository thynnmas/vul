/*
 * Villains' Utility Library - Thomas Martin Schmid, 2014. Public domain�
 *
 * A scalar linear math library containing Vectors, Matrices, Points, Quaternions, AABBs
 * general affine transformations as well as fixed and half precision floating point types.
 *
 * The library contains both a C++11 version and a c++98-version, the primary difference
 * being the way we construct our types. The C++11 style uses normal constructors
 * and initializer lists:
 *		Vector< f32_t, 3 > foo( a ), bar{ b, c, d };
 *		foo = Vector< f32_t, 3 >( a );
 * while the c++98-version uses a function style:
 *		Vector< f32_t, 3 > foo = makeVector< f32_t >( a, b, c );
 *		foo = makeVector< f32_t, 3 >( a );
 *
 * For SIMD work on our vectors we use an AOSOA architecture; we pack 2-8 vectors into
 * vectors of simd types, f.e. 4 Vector< f32_t, 3 >s into 1 Vector< __m128, 3 >, then operate
 * on those. Both 128-bit (SSE) and 256-bit (AVX) vectors are supported, behind
 * guards VUL_AOSOA_SSE and VUL_AOSOA_AVX respectively.
 *
 * Since templates need to be redefined for every compilation unit anyway, we only need a
 * definition guard for functions with no varied template arguments, which is constrained
 * to vul_aabb.hpp's transform3D functions and the extensions to numeric_limits found in
 * vul_fixed.hpp and vul_half.hpp. Define VUL_DEFINE in a _single_ c/cpp file to define 
 * these functions in that file.
 *
 * Library format inspired by http://www.reedbeta.com/blog/2013/12/28/on-vector-math-libraries/
 * 
 * @TODO: Test C++11 version. Test other compilers!
 * @TODO: The todos in vul_half.hpp
 * @TODO: Run melton, lint & other static analysis tools.
 * @TODO: Planned features:
 *		-SIMD functions for more things
 *		-Linear solvers
 *		-64-bit fixed type
 *		-Faster matrix multiplication for large matrices.
 *		-Specializations for 2-, 3- & 4-vectors with swizzles and
 *		 .x-style access. Need to duplicate all general vector code per
 *		 specialization though, so do this in separate files for cleanliness,
 *		 and use opportunity to write faster functions for these versions.
 *		 Possibly write full specializations instead for the normal types;
 *		 consider which will give less code duplication.
 *		-2x2, 3x3 & 4x4 matrix inversions as special cases for speed.
 * 
 * � If public domain is not legally valid in your legal jurisdiction
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
#ifndef VUL_MATH_HPP
#define VUL_MATH_HPP

/**
 * Define this for the c++11 version
 */
//#define VUL_CPLUSPLUS11

#include "vul_types.hpp"

#include "vul_aabb.hpp"
#include "vul_affine.hpp"
#if defined( VUL_AOSOA_SSE ) || defined( VUL_AOSOA_AVX )
	#include "vul_aosoa.hpp"		// This is the file where we pack scalar data into SIMD data.
#endif
#include "vul_fixed.hpp"
#include "vul_half.hpp"
#include "vul_matrix.hpp"
#include "vul_point.hpp"
#include "vul_quaternion.hpp"
#include "vul_vector.hpp"

#endif