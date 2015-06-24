/*
 * Villains' Utility Library - Thomas Martin Schmid, 2015. Public domain¹
 *
 * This file contains tests for vul_gl.hpp
 * 
 * ¹ If public domain is not legally valid in your legal jurisdiction
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
#ifndef VUL_TEST_GL_HPP
#define VUL_TEST_GL_HPP

#include <cassert>

#include "../vul_gl.hpp"

// Test against reference implementations in glm
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

using namespace vul;

namespace vul_test {
	class TestGL {

	public:
		static bool test( );

	private:
		static bool ortho( );
		static bool perspective( );
	};

	bool TestGL::test( )
	{
		assert( ortho( ) );
		assert( perspective( ) );

		return true;
	}

	bool TestGL::ortho( )
	{
		Matrix< f32_t, 4, 4 > m32;
		Matrix< f64_t, 4, 4 > m64;
		glm::mat4x4 gm32;
		glm::dmat4x4 gm64;
		const f32_t f32eps = 1e-5f;
		const f64_t f64eps = 1e-8f;

		gm32 = glm::ortho( -2.f, 0.f, -1.f, 1.f, 0.1f, 100.f );
		gm64 = glm::ortho( -2.0, 0.0, -1.0, 1.0, 0.1, 100.0 );

		m32 = gl_helper::ortho( -2.f, 0.f, -1.f, 1.f, 0.1f, 100.f );
		m64 = gl_helper::ortho( -2.0, 0.0, -1.0, 1.0, 0.1, 100.0 );

		for( ui32_t c = 0; c < 4; ++c ) {
			for( ui32_t r = 0; r < 4; ++r ) {
				assert( m32( c, r ) - gm32[ c ][ r ] < f32eps );
				assert( m64( c, r ) - gm64[ c ][ r ] < f64eps );
			}
		}

		return true;
	}

	bool TestGL::perspective( )
	{
		Matrix< f32_t, 4, 4 > m32, m32o;
		Matrix< f64_t, 4, 4 > m64, m64o;
		glm::mat4x4 gm32;
		glm::dmat4x4 gm64;
		const f32_t f32eps = 1e-5f;
		const f64_t f64eps = 1e-8f;

		// Test aspect version
		gm32 = glm::perspective( 67.5f, 1.6f, 0.1f, 100.0f );
		gm64 = glm::perspective( 67.5, 1.6, 0.1, 100.0 );

		m32 = gl_helper::perspective( 0.375f * ( f32_t )VUL_PI, 1.6f, 0.1f, 100.f );
		m64 = gl_helper::perspective( 0.375 * VUL_PI, 1.6, 0.1, 100.0 );
		
		for( ui32_t c = 0; c < 4; ++c ) {
			for( ui32_t r = 0; r < 4; ++r ) {
				assert( m32( c, r ) - gm32[ c ][ r ] < f32eps );
				assert( m64( c, r ) - gm64[ c ][ r ] < f64eps );
			}
		}

		// Test width/height version
		gm32 = glm::perspectiveFov( 67.5f, 1280.f, 720.f, 0.1f, 100.0f );
		gm64 = glm::perspectiveFov( 67.5, 1280.0, 720.0, 0.1, 100.0 );

		m32 = gl_helper::perspective( 0.375f * ( f32_t )VUL_PI, 1280.f, 720.f, 0.1f, 100.f );
		m64 = gl_helper::perspective( 0.375 * VUL_PI, 1280.0, 720.0, 0.1, 100.0 );
		
		for( ui32_t c = 0; c < 4; ++c ) {
			for( ui32_t r = 0; r < 4; ++r ) {
				assert( m32( c, r ) - gm32[ c ][ r ] < f32eps );
				assert( m64( c, r ) - gm64[ c ][ r ] < f64eps );
			}
		}

		// Test width/height + offset version
		m32o = gl_helper::perspective( 0.375f * ( f32_t )VUL_PI, 0.f, 1280.f, 0.f, 720.f, 0.1f, 100.f );
		m64o = gl_helper::perspective( 0.375 * VUL_PI, 0.0, 1280.0, 0.0, 720.0, 0.1, 100.0 );
		
		for( ui32_t c = 0; c < 4; ++c ) {
			for( ui32_t r = 0; r < 4; ++r ) {
				if( c == 2 && ( r == 0 || r == 1 ) ) {
					printf( "%f %f", m32(c, r), m32o(c, r ) );
					assert( m32( c, r ) - m32o( c, r ) - 1.f < f32eps );
					assert( m64( c, r ) - m64o( c, r ) - 1.0 < f64eps );
				} else {
					assert( m32( c, r ) - m32o( c, r ) < f32eps );
					assert( m64( c, r ) - m64o( c, r ) < f64eps );
				}
			}
		}

		

		return true;
	}
};

#endif
