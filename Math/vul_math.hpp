/*
 * Villains' Utility Library - Thomas Martin Schmid, 2017. Public domain?
 *
 * A scalar linear math library containing Vectors, Matrices, Points, Quaternions, AABBs
 * general affine transformations as well as fixed point and half precision floating 
 * point types.
 *
 * The library contains both a C++11 version and a C++98-version, the primary difference
 * being the way we construct our types. The C++11 style uses normal constructors
 * and initializer lists:
 *		Vector< f32, 3 > foo( a ), bar{ b, c, d };
 *		foo = Vector< f32, 3 >( a );
 * while the C++98-version uses a function style:
 *		Vector< f32, 3 > foo = makeVector< f32 >( a, b, c );
 *		foo = makeVector< f32, 3 >( a );
 *
 * For SIMD work on our vectors we use an AOSOA architecture; we pack 2-8 vectors into
 * vectors of simd types, f.e. 4 Vector< f32, 3 >s into 1 Vector< __m128, 3 >, then operate
 * on those. 128-bit (SSE, NEON) and 256-bit (AVX) vectors are supported, behind
 * guards VUL_AOSOA_SSE, VUL_AOSOA_NEON and VUL_AOSOA_AVX respectively. VUL_AOSOA_NEON only 
 * supports 32-bit floats.
 *
 * Since templates need to be redefined for every compilation unit anyway, we only need a
 * definition guard for functions with no varied template arguments, which is constrained
 * to vul_aabb.hpp's transform3D functions and the extensions to numeric_limits found in
 * vul_fixed.hpp and vul_half.hpp. Define VUL_DEFINE in a _single_ c/cpp file to define 
 * these functions in that file.
 * 
 * Any type that can be cast to and from a float can be used. Be aware that if integer types
 * are used, f.e. norm will work with integer operations; use fnorm for accurate results.
 *
 * Library format inspired by http://www.reedbeta.com/blog/2013/12/28/on-vector-math-libraries/
 * 
 * Tested compilers											|	Notes
 *		-MS VC 2015 Community								|
 *		-gcc (Debian 4.7.2-5) 4.7.2							|	C++98 & C++11. AVX builds, is not tested
 *		-MS VC 2010 Express									|	C++98 only
 *		-clang 3.0-6.2										|	C++98 only; C++11 requires initializer_list 
 *															|	which is introduced in clang 3.1
 * 
 * @TODO: Planned features:
 *		-Linear solvers
 *		-64-bit fixed type
 *		-Faster matrix multiplication for large matrices (which ties into sparse matrices)
 *		-2x2, 3x3 & 4x4 matrix inversions as special cases for speed.
 *		-Bezier curves, B-splines (as extensions, behind guards to avoid bloat?)
 *			-vul_bezier exists, incomplete
 *		-Potentially rewrite with SIMD-types as primitives, as this is apparently better now.
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
#ifndef VUL_MATH_HPP
#define VUL_MATH_HPP

#ifndef NOMINMAX
	#define NOMINMAX
	#define VUL_UNDEF_NOMINMAX
#endif

/**
 * Define this for the c++11 version
 */
//#define VUL_CPLUSPLUS11

#include "vul_types.hpp"

#include "vul_aabb.hpp"
#include "vul_affine.hpp"
#if defined( VUL_AOSOA_SSE ) || defined( VUL_AOSOA_AVX ) || defined( VUL_AOSOA_NEON )
	#include "vul_aosoa.hpp"		// This is the file where we pack scalar data into SIMD data.
#endif
#include "vul_fixed.hpp"
#include "vul_half.hpp"
#include "vul_matrix.hpp"
#include "vul_point.hpp"
#include "vul_quaternion.hpp"
#include "vul_vector.hpp"

#ifdef VUL_UNDEF_NOMINMAX
	#undef NOMINMAX
#endif

namespace vul {
   template< typename T >
   T clamp( T v, T mn, T mx );

   template< typename T >
   T saturate( T v );

   template< typename T, typename T_t >
   T lerp( T a, T b, T_t t );
}
#endif

#ifdef VUL_DEFINE
namespace vul {
   template< typename T >
   T clamp( T v, T mn, T mx )
   {
      return v < mn ? mn
           : v > mx ? mx
           : v;
   }

   template< typename T >
   T saturate( T v )
   {
      return v < static_cast< T >( 0.f ) ? static_cast< T >( 0.f )
           : v > static_cast< T >( 1.f ) ? static_cast< T >( 1.f )
           : v;
   }

   template< typename T, typename T_t >
   T lerp( T a, T b, T_t t )
   {
      T_t t1 = static_cast< T_t >( 1.f ) - t;
      return a * t + b * t1;
   }
}

#endif
