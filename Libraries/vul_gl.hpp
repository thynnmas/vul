/*
 * Villains' Utility Library - Thomas Martin Schmid, 2014. Public domain¹
 *
 * This file contains a collection helper functions useful when working with
 * OpenGL.
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
#include "vul_math.hpp"
#include <cmath>

namespace vul {

	/**
	 * OpenGL Helper class, containing mostly static functions.
	 * @TODO: Include a simple immediate mode here for prototyping!
	 */
	class gl_helper {

	public:
		/**
		 * Creates a uniform orthographic projection matrix.
		 * Default, normalized depth parameters are given.
		 * Depth range will be [-1, 1], right handed.
		 */
		template< typename T >
		static Matrix< T, 4, 4 > ortho( const T left,
										const T right,
										const T bottom,
										const T top,
										const T near = 0.f,
										const T far = 1.f );
		
		/**
		 * Creates a uniform perspective projection matrix for the given
		 * vertical FOV and aspect ratio. FOV in radians.
		 * Depth range will be [-1, 1], right handed.
		 * @NOTE: near must be > 0
		 */
		template< typename T >
		static Matrix< T, 4, 4 > perspective( const T fov_y,
											  const T aspect,
											  const T near,
											  const T far );
		
		/**
		 * Creates a uniform perspective projection matrix for the given
		 * vertical FOV and viewport size. FOV in radians.
		 * Depth range will be [-1, 1], right handed.
		 * @NOTE: near must be > 0
		 */
		template< typename T >
		static Matrix< T, 4, 4 > perspective( const T fov_y,
											  const T width,
											  const T height,
											  const T near,
											  const T far );
		/**
		 * Creates a uniform perspective projection matrix for the given
		 * vertical FOV and viewport size and location. FOV in radians.
		 * Depth range will be [-1, 1], right handed.
		 * @NOTE: near must be > 0
		 */
		template< typename T >
		static Matrix< T, 4, 4 > perspective( const T fov_y,
											  const T left,
											  const T right,
											  const T bottom,
											  const T top,
											  const T near,
											  const T far );
	};
	
	template< typename T >
	Matrix< T, 4, 4 > gl_helper::ortho( const T left,
										const T right,
										const T bottom,
										const T top,
										const T near,
										const T far )
	{
		Matrix< T, 4, 4 > mat;
		T A, B, C, D, Q, Qn, O, I;

		O = T( 0.f ); // Just so our matrix below is easier to read
		I = T( 1.f ); // Ditto...
		
		// Scale
		A = 2.f / ( right - left );
		B = 2.f / ( top - bottom );
		Q = -2.f / ( far - near);
		// Translation
		C = -( right + left ) / ( right - left );
		D = -( top + bottom ) / ( top - bottom );
		Qn = -( far + near ) / ( far - near );

		mat = makeMatrix44< T >( A, O, O, C,
								 O, B, O, D,
								 O, O, Q, Qn,
								 O, O, O, I );

		return mat;
		
	}

	template< typename T >
	Matrix< T, 4, 4 > gl_helper::perspective( const T fov_y,
											  const T aspect,
											  const T near,
											  const T far )
	{
		Matrix< T, 4, 4 > mat;
		T A, B, Q, Qn, O, I;
		T range, left, right, bottom, top;

		O = T( 0.f ); // Just so our matrix below is easier to read
		I = T( 1.f ); // Ditto...
		
		range = tan( fov_y / 2.f ) * near;
		left = -range * aspect;
		right = range * aspect;
		bottom = -range;
		top = range;

		// Scale
		A = ( 2.f * near ) / ( right - left );
		B = ( 2.f * near ) / ( top - bottom );
		Q = -( far + near ) / ( far - near );
		
		// Translation
		Qn = -( 2.f * far * near ) / ( far - near );

		mat = makeMatrix44< T >( A, O,  O, O,
								 O, B,  O, O,
								 O, O,  Q, Qn,
								 O, O, -I, O );

		return mat;
	}

	template< typename T >
	Matrix< T, 4, 4 > gl_helper::perspective( const T fov_y,
											  const T width,
											  const T height,
											  const T near,
											  const T far )
	{
		Matrix< T, 4, 4 > mat;
		T A, B, Q, Qn, O, I;
		T w, h, pf;

		O = T( 0.f ); // Just so our matrix below is easier to read
		I = T( 1.f ); // Ditto...

		pf = T( 0.5f );
		h = std::cos( pf * fov_y ) / std::sin( pf * fov_y );
		w = h * height / width;
	
		// Scale
		A = w;
		B = h;
		Q = -( far + near ) / ( far - near );
		
		// Translation
		Qn = -( 2.f * far * near ) / ( far - near );

		mat = makeMatrix44< T >( A, O,  O, O,
								 O, B,  O, O,
								 O, O,  Q, Qn,
								 O, O, -I, O );

		return mat;
	}
	template< typename T >
	Matrix< T, 4, 4 > gl_helper::perspective( const T fov_y,
											  const T left,
											  const T right,
											  const T bottom,
											  const T top,
											  const T near,
											  const T far )
	{
		Matrix< T, 4, 4 > mat;
		T A, B, C, D, Q, Qn, O, I;
		T w, h, pf;

		O = T( 0.f ); // Just so our matrix below is easier to read
		I = T( 1.f ); // Ditto...

		pf = T( 0.5f );
		h = std::cos( pf * fov_y ) / std::sin( pf * fov_y );
		w = h * ( top - bottom ) / ( right - left );

		// Rotation & shear
		C = ( right + left ) / ( right - left );
		D = ( top + bottom ) / ( top - bottom );

		// Scale
		A = w;
		B = h;
		Q = -( far + near ) / ( far - near );
		
		// Translation
		Qn = -( 2.f * far * near ) / ( far - near );

		mat = makeMatrix44< T >( A, O,  C, O,
								 O, B,  D, O,
								 O, O,  Q, Qn,
								 O, O, -I, O );

		return mat;
	}
}