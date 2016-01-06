/*
 * Villains' Utility Library - Thomas Martin Schmid, 2016. Public domain?
 *
 * Bezier curves. We support linear, quadratic and cubic bezier curves of
 * all data types that can be cast to/from floats. This file supplies
 * construction and drawing of such curves both from straight arrays of
 * points and VUL linked lists and vectors at user defined granularities.
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
#ifndef VUL_BEZIER_HPP
#define VUL_BEZIER_HPP

#include "vul_point.hpp"
#include <cstring>
#include <assert.h>

namespace vul {

	//----------------
	// Declarations
	//

	/* 
	 * Trace a given bezier curve at a given interval and call callback for each point.
	 */
	template< typename T, i32_t n >
	void bezier_trace_linear( void ( *callback )( Vector< T, n > pt ), 
									  Point< T, n > *pts, i32_t pt_count,
									  T step );
	template< typename T, i32_t n >
	void bezier_trace_quad( void ( *callback )( Vector< T, n > pt ), 
									Point< T, n > *pts, i32_t pt_count,
									T step );
	template< typename T, i32_t n >
	void bezier_trace_cubic( void ( *callback )( Vector< T, n > pt ), 
									 Point< T, n > *pts, i32_t pt_count,
									 T step );

	//---------------
	// Definitions
	//
	template< typename T, i32_t n >
	void bezier_trace_linear( void ( *callback )( Vector< T, n > pt ), 
									  Point< T, n > *pts, i32_t pt_count,
									  T step )
	{
		assert( pts );
		for( i32_t i = 0; i < pt_count - 1; ++i ) {
			T d = norm( pts[ i + 1 ] - pts[ i ] );
			for( T t = 0; t < d; t += step ) {
				callback( lerp( pts[ i ], pts[ i + 1 ], t / d ) );
			}
		}
	}

	template< typename T, i32_t n >
	void bezier_trace_quad( void ( *callback )( Vector< T, n > pt ), 
									Point< T, n > *pts, i32_t pt_count,
									T step )
	{
		assert( pts );
		for( i32_t i = 0; i < pt_count - 2; i += 2 ) {
			T d = norm( pts[ i + 2 ] - pts[ i ] );
			for( T t = 0; t < d; t += step ) {
				T ft = t / d;
				callback( lerp( lerp( pts[ i ], pts[ i + 1 ], ft ),
									 lerp( pts[ i + 1 ], pts[ i + 2 ], ft ),
									 ft ) );
			}
		}
	}

	template< typename T, i32_t n >
	void bezier_trace_cubic( void ( *callback )( Vector< T, n > pt ), 
									 Point< T, n > *pts, i32_t pt_count,
									 T step )
	{
		assert( pts );
		for( i32_t i = 0; i < pt_count - 3; i += 3 ) {
			T d = norm( pts[ i + 3 ] - pts[ i ] );
			for( T t = 0; t < d; t += step ) {
				T ft = t / d;
				callback( lerp( lerp( lerp( pts[ i ], pts[ i + 1 ], ft ),
											 lerp( pts[ i + 1 ], pts[ i + 2 ], ft ),
											 ft ),
									 lerp( lerp( pts[ i + 1 ], pts[ i + 2 ], ft ),
											 lerp( pts[ i + 2 ], pts[ i + 3 ], ft ),
											 ft ),
									 ft ) );
			}
		}
	}
}

#endif
