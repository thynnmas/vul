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
#include "vul_types.h"
#include <math.h>

/**
 * Creates a uniform orthographic projection matrix.
 * Depth range will be [-1, 1], right handed.
 * Matrix is assumed to be column major: mat44[ 1 ] = column 0, row 1
 */
void vul_gl_ortho( f32_t *mat44, 
				   const f32_t left,
				   const f32_t right,
				   const f32_t bottom,
				   const f32_t top,
				   const f32_t near,
				   const f32_t far )
{
	// Rotation & shear
	mat44[ 1 ] = mat44[ 2 ] = 0.f; 
	mat44[ 4 ] = mat44[ 6 ] = 0.f;
	mat44[ 8 ] = mat44[ 9 ] = 0.f;
	
	// Scale
	mat44[ 0 ] = 2.f / ( right - left );
	mat44[ 5 ] = 2.f / ( top - bottom );
	mat44[ 10 ] = -2.f / ( far - near);
	mat44[ 15 ] = 1.f;

	// Homogenous
	mat44[ 3 ] = 0.f;
	mat44[ 7 ] = 0.f;
	mat44[ 11 ] = 0.f;
	
	// Translation
	mat44[ 12 ] = -( right + left ) / ( right - left );
	mat44[ 13 ] = -( top + bottom ) / ( top - bottom );
	mat44[ 14 ] = -( far + near ) / ( far - near );
}

/**
 * Creates a uniform perspective projection matrix for the given
 * vertical FOV and aspect ratio. FOV in radians.
 * Depth range will be [-1, 1], right handed.
 * @NOTE: near must be > 0
 * Matrix is assumed to be column major: mat44[ 1 ] = column 0, row 1
 */
void vul_gl_perspective( f32_t *mat44, 
						 const f32_t fov_y,
						 const f32_t aspect,
						 const f32_t near,
						 const f32_t far )
{
	f32_t range, left, right, bottom, top;

	range = ( f32_t )tan( fov_y / 2.f ) * near;
	left = -range * aspect;
	right = range * aspect;
	bottom = -range;
	top = range;
	
	// Rotation & shear
	mat44[ 1 ] = mat44[ 2 ] = 0.f; 
	mat44[ 4 ] = mat44[ 6 ] = 0.f;
	mat44[ 8 ] = mat44[ 9 ] = 0.f;

	// Scale
	mat44[ 0 ] = ( 2.f * near ) / ( right - left );
	mat44[ 5 ] = ( 2.f * near ) / ( top - bottom );
	mat44[ 10 ] = -( far + near ) / ( far - near );
	mat44[ 15 ] = 0.f;

	// Homogenous
	mat44[ 3 ] = 0.f;
	mat44[ 7 ] = 0.f;
	mat44[ 11 ] = -1.f;
	
	// Translation
	mat44[ 12 ] = 0.f;
	mat44[ 13 ] = 0.f;
	mat44[ 14 ] = -( 2.f * far * near ) / ( far - near );
}

/**
 * Creates a uniform perspective projection matrix for the given
 * vertical FOV and viewport width and height. FOV in radians.
 * Depth range will be [-1, 1], right handed.
 * @NOTE: near must be > 0
 * Matrix is assumed to be column major: mat44[ 1 ] = column 0, row 1
 */
void vul_gl_perspective_fov( f32_t *mat44, 
							 const f32_t fov_y,
							 const f32_t width,
							 const f32_t height,
							 const f32_t near,
							 const f32_t far )
{
	f32_t h, w;
	
	h = ( f32_t )cos( 0.5f * fov_y ) / ( f32_t )sin( 0.5f * fov_y );
	w = h * height / width;
	
	// Rotation & shear
	mat44[ 1 ] = mat44[ 2 ] = 0.f; 
	mat44[ 4 ] = mat44[ 6 ] = 0.f;
	mat44[ 8 ] = mat44[ 9 ] = 0.f;

	// Scale
	mat44[ 0 ] = w;
	mat44[ 5 ] = h;
	mat44[ 10 ] = -( far + near ) / ( far - near );
	mat44[ 15 ] = 0.f;

	// Homogenous
	mat44[ 3 ] = 0.f;
	mat44[ 7 ] = 0.f;
	mat44[ 11 ] = -1.f;
	
	// Translation
	mat44[ 12 ] = 0.f;
	mat44[ 13 ] = 0.f;
	mat44[ 14 ] = -( 2.f * far * near ) / ( far - near );
}


/**
 * Creates a uniform perspective projection matrix for the given
 * vertical FOV and viewport size and position. FOV in radians.
 * Depth range will be [-1, 1], right handed.
 * @NOTE: near must be > 0
 * Matrix is assumed to be column major: mat44[ 1 ] = column 0, row 1
 */
void vul_gl_perspective_fov_offset( f32_t *mat44, 
									const f32_t fov_y,
									const f32_t left,
									const f32_t right,
									const f32_t bottom,
									const f32_t top,
									const f32_t near,
									const f32_t far )
	{
		f32_t w, h;

		h = ( f32_t )cos( 0.5f * fov_y ) / ( f32_t )sin( 0.5f * fov_y );
		w = h * ( top - bottom ) / ( right - left );

		// Rotation & shear
		mat44[ 1 ] = mat44[ 2 ] = 0.f; 
		mat44[ 4 ] = mat44[ 6 ] = 0.f;
		mat44[ 8 ] = ( right + left ) / ( right - left );
		mat44[ 9 ] = ( top + bottom ) / ( top - bottom );

		// Scale
		mat44[ 0 ] = w;
		mat44[ 5 ] = h;
		mat44[ 10 ] = -( far + near ) / ( far - near );
		mat44[ 15 ] = 0.f;

		// Homogenous
		mat44[ 3 ] = 0.f;
		mat44[ 7 ] = 0.f;
		mat44[ 11 ] = -1.f;
	
		// Translation
		mat44[ 12 ] = 0.f;
		mat44[ 13 ] = 0.f;
		mat44[ 14 ] = -( 2.f * far * near ) / ( far - near );
	}