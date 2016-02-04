/*
 * Villains' Utility Library - Thomas Martin Schmid, 2016. Public domain¹
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
#ifndef VUL_GL_H
#define VUL_GL_H

#include "vul_types.h"
#include <stdio.h>
#include <math.h>

#ifdef _cplusplus
extern "C" {
#endif
/**
 * Creates a uniform orthographic projection matrix.
 * Depth range will be [-1, 1], right handed.
 * Matrix is assumed to be column major: mat44[ 1 ] = column 0, row 1
 */
void vul_gl_ortho( f32 *mat44,
				   const f32 left,
				   const f32 right,
				   const f32 bottom,
				   const f32 top,
				   const f32 vnear,
				   const f32 vfar );
/**
 * Creates a uniform perspective projection matrix for the given
 * vertical FOV and aspect ratio. FOV in radians.
 * Depth range will be [-1, 1], right handed.
 * @NOTE: vnear must be > 0
 * Matrix is assumed to be column major: mat44[ 1 ] = column 0, row 1
 */
void vul_gl_perspective( f32 *mat44, 
						 const f32 fov_y,
						 const f32 aspect,
						 const f32 vnear,
						 const f32 vfar );
/**
 * Creates a uniform perspective projection matrix for the given
 * vertical FOV and viewport width and height. FOV in radians.
 * Depth range will be [-1, 1], right handed.
 * @NOTE: vnear must be > 0
 * Matrix is assumed to be column major: mat44[ 1 ] = column 0, row 1
 */
void vul_gl_perspective_fov( f32 *mat44, 
							 const f32 fov_y,
							 const f32 width,
							 const f32 height,
							 const f32 vnear,
							 const f32 vfar );
/**
 * Creates a uniform perspective projection matrix for the given
 * vertical FOV and viewport size and position. FOV in radians.
 * Depth range will be [-1, 1], right handed.
 * @NOTE: vnear must be > 0
 * Matrix is assumed to be column major: mat44[ 1 ] = column 0, row 1
 */
void vul_gl_perspective_fov_offset( f32 *mat44, 
									const f32 fov_y,
									const f32 left,
									const f32 right,
									const f32 bottom,
									const f32 top,
									const f32 vnear,
									const f32 vfar );
/**
 * Check for errors in OpenGL and print the output to the given string if
 * the string is not null, to stdout otherwise.
 * Returns 0 if no errors were found, 1 otherwise.
 */
int vul_gl_check_error_print( char *out_str );

#ifdef _cplusplus
}
#endif
#endif

#ifdef VUL_DEFINE

#ifdef _cplusplus
extern "C" {
#endif

void vul_gl_ortho( f32 *mat44,
				   const f32 left,
				   const f32 right,
				   const f32 bottom,
				   const f32 top,
				   const f32 vnear,
				   const f32 vfar )
{
	// Rotation & shear
	mat44[ 1 ] = mat44[ 2 ] = 0.f; 
	mat44[ 4 ] = mat44[ 6 ] = 0.f;
	mat44[ 8 ] = mat44[ 9 ] = 0.f;
	
	// Scale
	mat44[ 0 ] = 2.f / ( right - left );
	mat44[ 5 ] = 2.f / ( top - bottom );
	mat44[ 10 ] = -2.f / ( vfar - vnear);
	mat44[ 15 ] = 1.f;

	// Homogenous
	mat44[ 3 ] = 0.f;
	mat44[ 7 ] = 0.f;
	mat44[ 11 ] = 0.f;
	
	// Translation
	mat44[ 12 ] = -( right + left ) / ( right - left );
	mat44[ 13 ] = -( top + bottom ) / ( top - bottom );
	mat44[ 14 ] = -( vfar + vnear ) / ( vfar - vnear );
}

void vul_gl_perspective( f32 *mat44, 
						 const f32 fov_y,
						 const f32 aspect,
						 const f32 vnear,
						 const f32 vfar )
{
	f32 range, left, right, bottom, top;

	range = ( f32 )tan( fov_y / 2.f ) * vnear;
	left = -range * aspect;
	right = range * aspect;
	bottom = -range;
	top = range;
	
	// Rotation & shear
	mat44[ 1 ] = mat44[ 2 ] = 0.f; 
	mat44[ 4 ] = mat44[ 6 ] = 0.f;
	mat44[ 8 ] = mat44[ 9 ] = 0.f;

	// Scale
	mat44[ 0 ] = ( 2.f * vnear ) / ( right - left );
	mat44[ 5 ] = ( 2.f * vnear ) / ( top - bottom );
	mat44[ 10 ] = -( vfar + vnear ) / ( vfar - vnear );
	mat44[ 15 ] = 0.f;

	// Homogenous
	mat44[ 3 ] = 0.f;
	mat44[ 7 ] = 0.f;
	mat44[ 11 ] = -1.f;
	
	// Translation
	mat44[ 12 ] = 0.f;
	mat44[ 13 ] = 0.f;
	mat44[ 14 ] = -( 2.f * vfar * vnear ) / ( vfar - vnear );
}

void vul_gl_perspective_fov( f32 *mat44, 
							 const f32 fov_y,
							 const f32 width,
							 const f32 height,
							 const f32 vnear,
							 const f32 vfar )
{
	f32 h, w;
	
	h = ( f32 )cos( 0.5f * fov_y ) / ( f32 )sin( 0.5f * fov_y );
	w = h * ( height / width );
	
	// Rotation & shear
	mat44[ 1 ] = mat44[ 2 ] = 0.f; 
	mat44[ 4 ] = mat44[ 6 ] = 0.f;
	mat44[ 8 ] = mat44[ 9 ] = 0.f;

	// Scale
	mat44[ 0 ] = w;
	mat44[ 5 ] = h;
	mat44[ 10 ] = -( vfar + vnear ) / ( vfar - vnear );
	mat44[ 15 ] = 0.f;

	// Homogenous
	mat44[ 3 ] = 0.f;
	mat44[ 7 ] = 0.f;
	mat44[ 11 ] = -1.f;
	
	// Translation
	mat44[ 12 ] = 0.f;
	mat44[ 13 ] = 0.f;
	mat44[ 14 ] = -( 2.f * vfar * vnear ) / ( vfar - vnear );
}

void vul_gl_perspective_fov_offset( f32 *mat44, 
									const f32 fov_y,
									const f32 left,
									const f32 right,
									const f32 bottom,
									const f32 top,
									const f32 vnear,
									const f32 vfar )
{
	f32 w, h;

	h = ( f32 )cos( 0.5f * fov_y ) / ( f32 )sin( 0.5f * fov_y );
	w = h * ( top - bottom ) / ( right - left );

	// Rotation & shear
	mat44[ 1 ] = mat44[ 2 ] = 0.f; 
	mat44[ 4 ] = mat44[ 6 ] = 0.f;
	mat44[ 8 ] = ( right + left ) / ( right - left );
	mat44[ 9 ] = ( top + bottom ) / ( top - bottom );

	// Scale
	mat44[ 0 ] = w;
	mat44[ 5 ] = h;
	mat44[ 10 ] = -( vfar + vnear ) / ( vfar - vnear );
	mat44[ 15 ] = 0.f;

	// Homogenous
	mat44[ 3 ] = 0.f;
	mat44[ 7 ] = 0.f;
	mat44[ 11 ] = -1.f;
	
	// Translation
	mat44[ 12 ] = 0.f;
	mat44[ 13 ] = 0.f;
	mat44[ 14 ] = -( 2.f * vfar * vnear ) / ( vfar - vnear );
}

int vul_gl_check_error_print( char *out_str )
{
	GLenum error;

	if( ( error = glGetError( ) ) != GL_NO_ERROR ) {
		switch( error )
		{
		case 0x0500: //GL_INVALID_ENUM​
			if( out_str ) {
				sprintf( out_str, "OpenGL reported an invalid enum.\n" );
			} else {
				printf( "OpenGL reported an invalid enum.\n" );
			}
			break;
		case 0x0501: //GL_INVALID_VALUE​
			if( out_str ) {
				sprintf( out_str, "OpenGL reported an invalid value.\n" );
			} else {
				printf( "OpenGL reported an invalid value.\n" );
			}
			break;
		case 0x0502: //GL_INVALID_OPERATION
			if( out_str ) {
				sprintf( out_str, "OpenGL reported an invalid operation.\n" );
			} else {
				printf( "OpenGL reported an invalid operation.\n" );
			}
			break;
		case 0x0503: //GL_STACK_OVERFLOW​
			if( out_str ) {
				sprintf( out_str, "OpenGL reported a stack overflow.\n" );
			} else {
				printf( "OpenGL reported a stack overflow.\n" );
			}
			break;
		case 0x0504: //GL_STACK_UNDERFLOW​
			if( out_str ) {
				sprintf( out_str, "OpenGL reported a stack underflow.\n" );
			} else {
				printf( "OpenGL reported a stack underflow.\n" );
			}
			break;
		case 0x0505: //GL_OUT_OF_MEMORY
			if( out_str ) {
				sprintf( out_str, "OpenGL reported it's out of memory.\n" );
			} else {
				printf( "OpenGL reported it's out of memory.\n" );
			}
			break;
		case 0x0506: //GL_INVALID_FRAMEBUFFER_OPERATION​
			if( out_str ) {
				sprintf( out_str, "OpenGL reported an invalid framebuffer operation.\n" );
			} else {
				printf( "OpenGL reported an invalid framebuffer operation.\n" );
			}
			break;
		case 0x8031: //GL_TABLE_TOO_LARGE​1
			if( out_str ) {
				sprintf( out_str, "OpenGL reported a table too large.\n" );
			} else {
				printf( "OpenGL reported a table too large.\n" );
			}
			break;
		default:
			if( out_str ) {
				sprintf( out_str, "Unknown GL error 0x%d.\n", error );
			} else {
				printf( "Unknown GL error 0x%d.\n", error );
			}
			break;
		}
		return 1;
	}

	return 0;
}

#ifdef _cplusplus
}
#endif

#endif