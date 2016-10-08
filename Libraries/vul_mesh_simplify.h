/*
 * @TODO(thynn): This, properly
 * Mesh LOD generation/simplification.
 * WIP. Thought I'd make a simple mesh simplification/edge collapse thing that doesn't do voxelization, instead attempting
 * to chose a edges to collapse based on weights applied to mean error weighted by how "important" the feature is.
 */

#include <string.h> // For memset
#include "vul_cmath.h"

#ifndef VUL_MESH_ASSERT
#include <assert.h>
#define VUL_MESH_ASSERT assert
#endif

#if !defined( VUL_MESH_ALLOC ) && !defined( VUL_MESH_FREE )
#include <stdlib.h>
#define VUL_MESH_ALLOC malloc
#define VUL_MESH_FREE free
#elif defined( VUL_MESH_ALLOC ) || defined( VUL_MESH_FREE )
You have to define both alloc and free!
#endif

typedef struct vul__mesh_vertex {
	v3 pos; // @TODO(thynn): We probably want to have more data here, so we can interpolate those as well when we collapse edges. At least have room for texcoords? Normals etc. can be recalculated at the end!
	f32 feature_weight; // @TODO(thynn): If we want multiple scales, do we need room for multiple scales? Or do we som them into this one value?
	size_t idx; // So we can copy over the full vertex afterwards, is -1 if the vertex has been removed
} vul__mesh_vertex;

typedef struct vul__mesh_edge {
	u32 iA, iB;
	f32 error;
} vul__mesh_edge;

typedef struct vul_mesh {
	f32 *vertex_data;
	size_t vertex_stride; // @TODO(thynn): The below assumes this is in floats
	size_t vertex_position_offset; // @TODO(thynn): The below assumes this is in floats
	size_t vertex_count;

	u32 *index_data;
	size_t index_count;
} vul_mesh;

typedef struct vul__mesh_lod {
	vul__mesh_vertex *vertices;
	vul__mesh_edge *edges;

	size_t vertex_count, edge_count, valid_edges;

	size_t *vertex_neighbor_count;
	size_t **vertex_to_edge_map;
} vul__mesh_lod;

f32 vul__mesh_calculate_error( vul__mesh_lod *mesh, size_t iA, size_t iB )
{
	v3 x2 = vadd3( vmuls3( mesh->vertices[ iA ].pos, 0.5f ), vmuls3( mesh->vertices[ iB ], 0.5f ) );
	// @TODO(thynn): For each neighbor n of {iA, iB}, calcualte the new line segment [n, pN]
	// and the distance from {pA, pB} to that line. Sum these for the total error
	f32 err = 0.f;
	size_t cnt = 0;
	for( size_t i = 0; i < mesh->vertex_neighbor_count[ iA ], ++i ) {
		v3 x1 = mesh->vertices[ mesh->vertex_to_edge_map[ iA ][ i ] ].pos;
		v3 p0 = vsub3( x2, x1 );
		err += vnorm3( vcross3( p0 ), vsub3( x1, mesh->vertices[ iA ] ) ) / vnorm3( p0 );
		++cnt;
	}
	for( size_t i = 0; i < mesh->vertex_neighbor_count[ iB ], ++i ) {
		v3 x1 = mesh->vertices[ mesh->vertex_to_edge_map[ iB ][ i ] ].pos;
		v3 p0 = vsub3( x2, x1 );
		err += vnorm3( vcross3( p0 ), vsub3( x1, mesh->vertices[ iB ] ) ) / vnorm3( p0 );
		++cnt;
	}
	return err / ( f32 )cnt;
}

f32 vul__mesh_calculate_vertex_weight( vul__mesh_lod *mesh, size_t idx )
{
	// @TODO(thynn): This is where we should be smart. For now, return 1, thus using root squared distance
	// However, we should use a set of weighted features here, such as:
	// - Number of neighboring vertices/edges it is part of
	// - Distance to neighbors (total and variance)
	// - Angle between normal at vertex and edges (total and variance)
	// - Harris operator??
	// - The above, but for neighbors of neighbors (discounting ourselves and 1st-degree neighbors).
	// - We could then tweak these values, amybe even apply some ML to them, to get better selection

	return 1.f;
}

void vul__mesh_calculate_edges_vertices( vul__mesh_lod *out, vul_mesh *in )
{
	VUL_MESH_ASSERT( in->index_count % 3 == 0 );
	
	out->vertex_count = in->vertex_count;
	out->edge_count = in->index_count;
	out->vertices = ( vul__mesh_vertex* )VUL_MESH_ALLOC( sizeof( vul__mesh_vertex ) * in->vertex_count );
	out->edges = ( vul__mesh_edge* )VUL_MESH_ALLOC( sizeof( vul__mesh_edge ) * in->index_count ); // @TODO(thynn): Some of these (most of these) are probably shared!
	for( size_t i = 0; i < in->index_count; i +=3 ) {
		out->edges[ e     ].iA = in->indices[ i     ];
		out->edges[ e     ].iB = in->indices[ i + 1 ];
		out->edges[ e + 1 ].iA = in->indices[ i + 1 ];
		out->edges[ e + 1 ].iB = in->indices[ i + 2 ];
		out->edges[ e + 2 ].iA = in->indices[ i + 2 ];
		out->edges[ e + 2 ].iB = in->indices[ i     ];
	}
	for( size_t i = 0; i < in->vertex_count; ++i ) {
		out->vertices[ i ].idx = i;
		out->vertices[ i ].pos = vec3( in->vertices[ in->vertex_position_offset + ( i * in->vertex_stride )     ],
												 in->vertices[ in->vertex_position_offset + ( i * in->vertex_stride ) + 1 ],
												 in->vertices[ in->vertex_position_offset + ( i * in->vertex_stride ) + 2 ] );
		out->vertices[ i ].feature_weight = 0; // @TODO(thynn): This should be some normalized default value, although it should probably 
	}

	// Mark duplicate edges
	out->valid_edges = out->edge_count;
	for( size_t i = 0; i < out->edge_count; ++i ) {
		for( size_t j = i + 1; j < out->edge_count; ++j ) {
			if( out->edges[ i ].iA == out->edges[ j ].iA &&
				 out->edges[ i ].iB == out->edges[ j ].iB ) {
				// Mark the first one as a duplicate (so we encounter it earlier in the loop below, and makes logic here simpler)
				out->edges[ i ].iA = ( size_t )-1;
				out->edges[ i ].iB = ( size_t )-1;
				--out->valid_edges;
			}
		}
	}
	out->vertex_to_edge_map = ( size_t** )VUL_MESH_ALLOC( sizeof( size_t* ) * in->vertex_count );
	out->vertex_neighbor_count = ( size_t* )VUL_MESH_ALLOC( sizeof( size_t ) * in->vertex_count );
	for( size_t i = 0; i < in->vertex_count; ++i ) {
		size_t n = 0;
		for( size_t j = 0; j < out->edge_count; ++j ) {
			if( out->edges[ j ].iA == i || out->edges[ j ].iB == i ) {
				++n;
			}
		}
		out->vertex_neighbor_count[ i ] = n;
		if( n ) {
			out->vertex_to_edge_map[ i ] = ( size_t* )VUL_MESH_ALLOC( sizeof( size_t ) * n );
			for( size_t j = 0, m = 0; j < out->edge_count; ++j ) {
				if( out->edges[ j ].iA == i || out->edges[ j ].iB == i ) {
					out->vertex_to_edge_map[ i ][ m++ ] = j;
				}
			}
		} else {
			out->vertex_to_edge_map[ i ] = 0;
		}
	}

	for( size_t i = 0; i < in->edge_count; ++i ) {
		if( out->edges[ i ].iA == -1 || out->edges[ i ].iB == -1 ) {
			continue;
		}
		out->edges[ i ].error = vul__mesh_calculate_error( out, out->edges[ i ].iA, out->edges[ i ].iB );
	}

	for( size_t i = 0; i < in->vertex_count; ++i ) {
		out->vertices[ i ].feature_weight = vul__mesh_calculate_vertex_weight( out, i );  // @TODO(thynn): We probably need the original mesh here
	}

}

void vul__mesh_collapse_edge( vul__mesh_lod *mesh )
{
	// Find the best edge to collapse
	f32 mn = FLT_MAX;
	size_t idx = ( size_t )-1;

	for( size_t i = 0; i < mesh->edge_count; ++i ) {
		f32 err = mesh->edges[ i ].error 
				  * mesh->vertices[ mesh->edges[ i ].iA ].feature_weight 
				  * mesh->vertices[ mesh->edges[ i ].iB ].feature_weight;
		if( err < mn ) {
			mn = err;
			idx = i;
		}
	}
	
	// Mark the first vertex as removed
	vul__mesh_vertex *remove = &mesh->vertices[ mesh->edges[ idx ].iA ];
	remove->idx = ( size_t )-1;
	vul__mesh_vertex *edit = &mesh->vertices[ mesh->edges[ idx ].iB ];
	// Calculate the new position
	edit->pos = vadd3( vmul3s( remove->pos, 0.5f ), vmul3s( edit->pos, 0.5 ) );
	// For every edge that references the removed vertex, point to the new vertex
	for( size_t i = 0; i < mesh->vertex_neighbor_count[ mesh->edges[ idx ].iA ]; ++i ) {
		vul__mesh_edge *e = &mesh->edges[ mesh->vertex_to_edge_map[ mesh->edges[ idx ].iA ][ i ] ];
		if( e->iA == mesh->edges[ idx ].iA ) {
			e->iA = mesh->edges[ idx ].iB;
		}
		if( e->iB == mesh->edges[ idx ].iA ) {
			e->iB = mesh->edges[ idx ].iB;
		}
	}
	// Update the edge map for the edited vertex, and update/remove the removed one
	// @TODO(thynn): We need to make sure we don't duplicate shared neighbors here!
	size_t ncount = mesh->vertex_neighbor_count[ mesh->edges[ idx ].iA ]
					  + mesh->vertex_neighbor_count[ mesh->edges[ idx ].iB ];
	size_t *nbs = ( size_t* )VUL_MESH_ALLOC( sizeof( size_t ) * ncount );
	memcpy( &nbs[ 0 ], 
			  mesh->vertex_to_edge_map[ mesh->edges[ idx ].iA ], 
			  mesh->vertex_neighbor_count[ mesh->edges[ idx ].iA ] * sizeof( size_t ) );
	memcpy( &nbs[ mesh->vertex_neighbor_count[ mesh->edges[ idx ].iA ] ], 
			  mesh->vertex_to_edge_map[ mesh->edges[ idx ].iB ], 
			  mesh->vertex_neighbor_count[ mesh->edges[ idx ].iB ] * sizeof( size_t ) );
	mesh->vertex_neighbor_count[ mesh->edges[ idx ].iB ] = ncount;
	mesh->vertex_neighbor_count[ mesh->edges[ idx ].iA ] = 0;
	VUL_MESH_FREE( mesh->vertex_to_edge_map[ mesh->edges[ idx ].iA ] );
	VUL_MESH_FREE( mesh->vertex_to_edge_map[ mesh->edges[ idx ].iB ] );
	mesh->vertex_to_edge_map[ mesh->edges[ idx ].iA ] = 0;
	mesh->vertex_to_edge_map[ mesh->edges[ idx ].iB ] = nbs;

	// For every edge that references the moved vertex, recalculate the error
	for( size_t i = 0; mesh->vertex_neighbor_count[ mesh->edges[ idx ].iB ]; ++i ) {
		vul__mesh_edge *e = &mesh->edges[ mesh->vertex_to_edge_map[ mesh->edges[ idx ].iB ][ i ] ];
		e->error = vul__mesh_calculate_error( mesh, e->iA, e->iB );
	}
	// Recalculate the feature weight of the moved vertex. @TODO(thynn): We might need a reference to LOD0 / the original mesh here.
	mesh->vertices[ mesh->edges[ idx ].iB ].feature_weight = vul__mesh_calculate_vertex_weight( mesh, mesh->edges[ idx ].iB ] ); // @TODO(thynn): Should we really recalculate this, or just sum the two old ones? Make a setting?

	// @TODO(thynn): We probably want to recalculate the number of triangles here (calculate how many we eliminated).
}

void vul__mesh_populate_from_lod( vul_mesh *out, vul_mesh *in, vul__mesh_lod *lod )
{
	// @TODO(thynn): This!
	// Calculate the number of valid vertices
	size_t vert_count = 0;
	for( size_t i = 0; i < in->vertex_count; ++i ) {
		if( lod->vertices[ i ].idx != -1 ) {
			++vert_count;
		}
	}
	out->vertex_count = vert_count;
	out->vertex_stride = in->vertex_stride;
	out->vertex_position_offset = in->vertex_position_offset;
	out->vertices = ( f32* )VUL_MESH_ALLOC( sizeof( f32 ) * in->vertex_stride );
	for( size_t i = 0, o = 0; i < in->vertex_count; ++i ) {
		if( lod->vertices[ i ].idx == -1 ) {
			continue;
		}
		memcpy( &out->vertices[ o * in->vertex_stride ],
				  &in->vertices[ i * in->vertex_stride ],
				  sizeof( f32 ) * in->vertex_stride );
		out->vertices[ o * in->vertex_stride + in->vertex_position_offset     ] = lod->vertices[ i ].pos.x;
		out->vertices[ o * in->vertex_stride + in->vertex_position_offset + 1 ] = lod->vertices[ i ].pos.y;
		out->vertices[ o * in->vertex_stride + in->vertex_position_offset + 2 ] = lod->vertices[ i ].pos.z;
		lod->vertices[ i ].idx = o++; // Store away the new index for later
	}

	// @TODO(thynn): Calculate the new triangles, although I think we probably want to keep track of this during the collapse instead (so we can use
	// triangle count to stop, and remove them as we go)
	out->index_count = 0;
	out->index_data = 0;
}

void vul__mesh_lod_destroy( vul__mesh_lod *lod )
{
	VUL_MESH_FREE( lod->vertices );
	VUL_MESH_FREE( lod->edges );

	for( size_t i = 0; i < lod->vertex_count; ++i ) {
		if( lod->vertex_neighbor_count[ i ] ) {
			VUL_MESH_FREE( lod->vertex_neighbor_count[ i ] );
		}
		if( lod->vertex_to_edge_map[ i ] ) {
			VUL_MESH_FREE( lod->vertex_to_edge_map[ i ] );
		}
	}
	VUL_MESH_FREE( lod->vertex_neighbor_count );
	VUL_MESH_FREE( lod->vertex_to_edge_map );
}

void vul_mesh_simplify( vul_mesh *out, vul_mesh *mesh, size_t target_tri_count, void ( *callback )( vul_mesh *mesh, vul__mesh_lod *lod ) )
{
	/* Calculate edges */
	vul__mesh_lod lod;
	vul__mesh_calculate_edges_vertices( &lod, mesh );

	while( lod->valid_edges > target_tri_count * 3 ) { // @TODO(thynn): This is just flat out not correct, we need a number of triangles from the lod-mesh somewhere
		vul__mesh_collapse_edge( &lod );
		// @TODO(thynn): Allow the user to supply a callback after every one of these, so they can render the progress etc?
		if( callback ) {
			callback( mesh, &lod );
		}
	}
	// @TODO(thynn): Copy the final mesh data over into a new one; we should define a vul_mesh struct that contains the parameters above, and take in a "in" parameter and an "out" parameter.
	vul__mesh_populate_from_lod( out, mesh, &lod );

	// @TODO(thynn): Free lod0
	vul__mesh_lod_destroy( &lod );
}
