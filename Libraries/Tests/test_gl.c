/*
 * Villains' Utility Library - Thomas Martin Schmid, 2016. Public domain?
 *
 * This file contains tests for vul_gl.h
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
#ifndef VUL_TEST_GL_H
#define VUL_TEST_GL_H

#include <stdlib.h>

#define TEST( expr ) if( !( expr ) ) {\
   fprintf( stderr, #expr );\
   exit( 1 );\
}

#define VUL_DEFINE
#include "GL/gl.h"
#include "../vul_gl.h"
#include "../vul_types.h"

#define VUL_TEST_PI 3.1415926535897932384626433832795f

// Test against reference implementation. @TODO: Find a C library to test against!


static int vul__test_gl_ortho( )
{
	f32 m32[ 16 ], f32eps;
	u32 c, r;
	
	//glm::mat4x4 gm32;
	
	f32eps = 1e-5f;

	//gm32 = glm::ortho( -2.f, 0.f, -1.f, 1.f, 0.1f, 100.f );
	
	vul_gl_ortho( m32, -2.f, 0.f, -1.f, 1.f, 0.1f, 100.f );
	
	for( c = 0; c < 4; ++c ) {
		for( r = 0; r < 4; ++r ) {
	//		TEST( m32( c, r ) - gm32[ r ][ c ] < f32eps );
		}
	}

	return 1;
}

static int vul__test_gl_perspective( )
{
	f32 m32[ 16 ], m32o[ 16 ], f32eps;
	u32 c, r;
	//glm::mat4x4 gm32;
	
	f32eps = 1e-5f;

	// Test aspect version
	//gm32 = glm::perspective( 67.5f, 1.6f, 0.1f, 100.0f );
	vul_gl_perspective( m32, 0.375f * ( f32 )VUL_TEST_PI, 1.6f, 0.1f, 100.f );
		
	for( c = 0; c < 4; ++c ) {
		for( r = 0; r < 4; ++r ) {
			//TEST( m32( c, r ) - gm32[ c ][ r ] < f32eps );
		}
	}

	// Test width/height version
	//gm32 = glm::perspectiveFov( 67.5f, 1280.f, 720.f, 0.1f, 100.0f );

	vul_gl_perspective_fov( m32, 0.375f * ( f32 )VUL_TEST_PI, 1280.f, 720.f, 0.1f, 100.f );
		
	for( c = 0; c < 4; ++c ) {
		for( r = 0; r < 4; ++r ) {
			//TEST( m32( c, r ) - gm32[ c ][ r ] < f32eps );
		}
	}

	// Test width/height + offset version
	vul_gl_perspective_fov_offset( m32o, 0.375f * ( f32 )VUL_TEST_PI, 0.f, 1280.f, 0.f, 720.f, 0.1f, 100.f );
		
	for( c = 0; c < 4; ++c ) {
		for( r = 0; r < 4; ++r ) {
			if( c == 3 && ( r == 0 || r == 1 ) ) {
				//TEST( m32( c, r ) - m32o( c, r ) < ... + f32eps );
			} else {
				//TEST( m32( c, r ) - m32o( c, r ) < f32eps );
			}
		}
	}

	return 1;
}

int main( )
{
	TEST( vul__test_gl_ortho( ) );
	TEST( vul__test_gl_perspective( ) );

	return 0;
}

#undef VUL_TEST_PI

#endif
