/* Mock immediate mode for GL. Allows for both fire-and-forget begin/end
   which is highly innefficient (creates a VAO for every call, deletes 
   after render) and persistent variations; create your objects with 
   begin/end syntax, then use them with draw */

#include "vul_datatypes.h"

#define NULL 0

typedef struct
{
	vul_vector *positions;
	vul_vector *normals;
	vul_vector *texcoords;
} vulgl__immediate_state;

static vulgl__immediate_state vulgl__immediate_current_state;

void vul_gl_begin( GLenum e )
{
	if ( vulgl__immediate_current_state.positions ) 
		free ( vulgl__immediate_current_state.positions );
	vulgl__immediate_current_state.positions = vul_vector_create( sizeof( float ) * 3 * 8 );

	if ( vulgl__immediate_current_state.normals )
		free ( vulgl__immediate_current_state.normals );
	vulgl__immediate_current_state.normals = vul_vector_create( sizeof( float ) * 3 * 8 );

	if ( vulgl__immediate_current_state.texcoords )
		free ( vulgl__immediate_current_state.texcoords );
	vulgl__immediate_current_state.texcoords = vul_vector_create( sizeof( float ) * 2 * 8 );
}

void vul_gl_vertex3f( float x, float y, float z )
{
	vul_vector_add( vulgl__immediate_current_state.positions, &x );
	vul_vector_add( vulgl__immediate_current_state.positions, &y );
	vul_vector_add( vulgl__immediate_current_state.positions, &z );
}

void vul_gl_normal3f( float x, float y, float z )
{
	vul_vector_add( vulgl__immediate_current_state.normals, &x );
	vul_vector_add( vulgl__immediate_current_state.normals, &y );
	vul_vector_add( vulgl__immediate_current_state.normals, &z );
}

void vul_gl_texcoord2f( float u, float v )
{
	vul_vector_add( vulgl__immediate_current_state.texcoords, &u );
	vul_vector_add( vulgl__immediate_current_state.texcoords, &v );
}

void vul_gl_end( )
{
	// @TODO: Draw things
	// Create the VAO
	// Create the VBO
	// DrawArrays

	if ( vulgl__immediate_current_state.positions ) free ( vulgl__immediate_current_state.positions );
	if ( vulgl__immediate_current_state.normals ) free ( vulgl__immediate_current_state.normals );
	if ( vulgl__immediate_current_state.texcoords ) free ( vulgl__immediate_current_state.texcoords );
}