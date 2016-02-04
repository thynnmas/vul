/**
 * This tests vul_astar on a rectangular grid.
 */
#include "../vul_astar.h"
#include "../vul_types.h"

#include <stdio.h>
#include <ctype.h>
#include <string.h>

#ifdef _WIN32
#define snprintf sprintf_s
#endif

#define VISUALIZE VUL_FALSE

char *level = "(20, 20)"
			  "(19, 3)"
			  "(2, 18)"
			  "(5, 5, 10, 10) (1, 2, 4, 1)";


u32 astar__input_read_number_pair( u32 *x, u32 *y, char **p )
{
	u32 num_count;

	num_count = 0;
	while( **p ) {
		/* If character is a number, read the number */
		if( isdigit( **p ) ) {
			if( num_count++ % 2 == 0 ) {
				*x = strtol( *p, p, 10 );
			} else {
				*y = strtol( *p, p, 10 );
				return 1;
			}
		} else {
			++( *p );
		}
	}
	return 0;
}

void astar_free_bitfield( u32 *bitfield )
{
	free( bitfield );
}

void astar__bitfield_mark_block( u32 *bitfield,
								 u32 bitfield_width,
								 u32 x0, u32 y0,
								 u32 w, u32 h )
{
	u32 x, y, i, b;

	for( y = y0; y < y0 + h; ++y ) {
		for( x = x0; x < x0 + w; ++x ) {
			i = ( y * bitfield_width + x ) / 32;
			b = ( y * bitfield_width + x ) % 32;
			bitfield[ i ] |= ( 1 << b );
		}
	}
}
u32 *astar_input_from_string( u32 *w, u32 *h,
								 u32 *sx, u32 *sy,
								 u32 *ex, u32 *ey,
								 char *str )
{
	u32 *bitfield, x0, y0, xw, yh, bitfield_length;
	char *c;

	/* Read the size of the grid and allocate it */
	c = str;
	assert( astar__input_read_number_pair( w, h, &c ) );
	bitfield_length = ( u32 )ceil( ( f32 )( ( *w ) * ( *h ) ) / 32.f );
	bitfield = ( u32* )malloc( bitfield_length * sizeof( u32 ) );
	/* Initialize it to 0 */
	memset( bitfield, 0x00, bitfield_length * sizeof( u32 ) );
	/* Read the start and end positions */
	assert( astar__input_read_number_pair( sx, sy, &c ) );
	assert( astar__input_read_number_pair( ex, ey, &c ) );
	/* Read ll the walls */
	while( 1 ) {
		if( !astar__input_read_number_pair( &x0, &y0, &c ) ) {
			break; /* No more walls */
		}
		/* If we have a single block we have a malformed line! */
		assert( astar__input_read_number_pair( &xw, &yh, &c ) );
		/* Mark the block */
		astar__bitfield_mark_block( bitfield, *w, x0, y0, xw, yh );
	}
	return bitfield;
}

typedef struct astar_node_user_data {
	u32 x, y, blocked;
} astar_node_user_data;

typedef struct astar_graph_user_data {
	u32 width, height;
} astar_graph_user_data;

/*
* Cost between neighbors is infinite if blocked, 1 otherwise (since we know the node's index is
* calculated from it's location with (y * width) + x).
*/
vul_astar_node *astar_get_node( vul_astar_graph *graph,
								void *data )
{
	u32 index;
	astar_graph_user_data *graph_data;
	astar_node_user_data *node_data;
	vul_astar_node *node;

	node_data = ( astar_node_user_data* )data;
	graph_data = ( astar_graph_user_data* )graph->user_data;

	data = ( astar_graph_user_data* )graph->user_data;
	index = ( node_data->y * graph_data->width ) + node_data->x;

	node = ( vul_astar_node* )vul_svector_get( graph->nodes, index );

	return node;
}

/*
* The neighbors of node (x,y) on a grid are trivial to find, assuming here
* we only mode horizontally and vertically. Their indices are calculated
* as (y * width) + x.
* We only return neighbors that are not blocked (DFS may otherwise find a path
* that goes through a wall, since it makes no guarantees to find the best path
* and a wall is otherwise just a normal node with a very high (INF) cost).
*/
u64 astar_neighbors( vul_astar_node **neighbors,
						vul_astar_graph *graph,
						vul_astar_node *root,
						u32 max_neighbors )
{
	astar_node_user_data *node_data, *n_data, new_node;
	astar_graph_user_data *graph_data;
	vul_astar_node * n;
	u32 i;

	assert( max_neighbors >= 4 ); // No diagonals means 4 neighbors

	/* Handle neighbors, handling edge cases */
	i = 0;
	graph_data = ( astar_graph_user_data* )graph->user_data;
	node_data = ( astar_node_user_data* )root->user_data;
	if( node_data->x > 0 ) {
		new_node.x = node_data->x - 1; new_node.y = node_data->y;
		n = astar_get_node( graph, &new_node );
		n_data = ( astar_node_user_data* )n->user_data;
		if( !n_data->blocked ) {
			neighbors[ i++ ] = astar_get_node( graph, &new_node );
		}
	}
	if( node_data->x < graph_data->width - 1 ) {
		new_node.x = node_data->x + 1; new_node.y = node_data->y;
		n = astar_get_node( graph, &new_node );
		n_data = ( astar_node_user_data* )n->user_data;
		if( !n_data->blocked ) {
			neighbors[ i++ ] = astar_get_node( graph, &new_node );
		}
	}
	if( node_data->y > 0 ) {
		new_node.x = node_data->x; new_node.y = node_data->y - 1;
		n = astar_get_node( graph, &new_node );
		n_data = ( astar_node_user_data* )n->user_data;
		if( !n_data->blocked ) {
			neighbors[ i++ ] = astar_get_node( graph, &new_node );
		}
	}
	if( node_data->y < graph_data->height - 1 ) {
		new_node.x = node_data->x; new_node.y = node_data->y + 1;
		n = astar_get_node( graph, &new_node );
		n_data = ( astar_node_user_data* )n->user_data;
		if( !n_data->blocked ) {
			neighbors[ i++ ] = astar_get_node( graph, &new_node );
		}
	}
	/* Notify how many we've filled in */
	return i;
}


/*
* Cost between neighbors is infinite if blocked, 1 otherwise (since we
* disallow diagonal moves.
*/
f64 astar_cost_neighbors( vul_astar_node *s,
							vul_astar_node *t )
{
	astar_node_user_data *node_data;

	node_data = ( astar_node_user_data* )t->user_data;
	return node_data->blocked == 0 ? 1.0 : F64_INF;
}

void astar_grid_construct_graph( vul_astar_graph *graph,
								 u32 width,
								 u32 height,
								 u32 *bitfield )
{
	u32 x, y, base;
	astar_graph_user_data *graph_data;
	astar_node_user_data *node_data;
	vul_astar_node *node;

	graph->user_data = malloc( sizeof( astar_graph_user_data ) );
	graph_data = ( astar_graph_user_data* )graph->user_data;
	/* Store sizes */
	graph_data->width = width;
	graph_data->height = height;

	/* Allocate space for the nodes (we make the base buffer big enough for the board for speed) */
	graph->nodes = vul_svector_create( sizeof( vul_astar_node ), 8, malloc, free );//width * height );

	/* For all the nodes, set their cost */
	for( y = 0u; y < height; ++y ) {
		base = y * width;
		for( x = 0u; x < width; ++x ) {
			/* Index into our bitfield */
			u32 blocked = ( bitfield[ ( base + x ) / 32 ]
							   & ( 1 << ( ( base + x ) % 32 ) ) )
							   == 0u ? 0u : 0xffffffff;
			/* Store whether it is blocked */
			node = ( vul_astar_node* ) vul_svector_append_empty( graph->nodes );
			node->user_data = malloc( sizeof( astar_node_user_data ) );
			node_data = ( astar_node_user_data* )node->user_data;
			node_data->blocked = blocked;
			/* Store the location */
			node_data->x = x;
			node_data->y = y;
			/* Initialize costs */
			node->f_cost = 0.0;
			node->g_cost = 0.0;
			/* Initialize state */
			node->state = VUL_ASTAR_NODE_UNDISCOVERED;
			node->path_parent = NULL;
		}
	}
}

void astar__grid_finalizer( void *node_ptr, u32 index, void *nothing )
{
	vul_astar_node *node;

	node = ( vul_astar_node* )node_ptr;
	free( node->user_data );
}

int astar_grid_is_final( vul_astar_node *c, vul_astar_node *e )
{
	return c == e ? 1 : 0;
}

void astar_grid_finalize_graph( vul_astar_graph *graph )
{
	vul_svector_iterate( graph->nodes, astar__grid_finalizer, NULL );
	vul_svector_destroy( graph->nodes );

	free( graph->user_data );
}

f64 astar_grid_manhattan_dist( vul_astar_node *s, vul_astar_node *t )
{
	astar_node_user_data *node_data_t, *node_data_s;

	node_data_t = ( astar_node_user_data* )t->user_data;
	node_data_s = ( astar_node_user_data* )s->user_data;
	return abs( ( s32 )node_data_t->x - ( s32 )node_data_s->x ) + abs( ( s32 )node_data_t->y - ( s32 )node_data_s->y );
}

f64 astar_grid_euclidian_dist( vul_astar_node *s, vul_astar_node *t )
{
	astar_node_user_data *node_data_t, *node_data_s;
	f64 dx, dy, d;

	node_data_t = ( astar_node_user_data* )t->user_data;
	node_data_s = ( astar_node_user_data* )s->user_data;

	dx = pow( ( f64 )( ( f64 )node_data_t->x - ( f64 )node_data_s->x ), 2 );
	dy = pow( ( f64 )( ( f64 )node_data_t->y - ( f64 )node_data_s->y ), 2 );

	d = sqrt( dx + dy );
	return d;
}

u32 astar_grid_print_path( vul_astar_path_node *root, int quiet )
{
	astar_node_user_data *data;
	u32 count;

	if( !quiet ) {
		printf( "Path: " );
	}
	count = 0;
	while( root )
	{
		if( !quiet ) {
			data = ( astar_node_user_data* )root->node_data;
			printf( "(%d, %d)", data->x, data->y );
		}
		root = root->next;
		++count;
	}
	if( !quiet ) {
		printf( "\n" );
	}
	return count;
}

void astar_grid_visualize( vul_astar_graph *graph, vul_astar_node *start, vul_astar_node *end, vul_astar_node *current )
{
	astar_node_user_data *node_data, *current_data, *start_data, *end_data;
	vul_astar_path_node *path_root;
	u32 len;

	current_data = ( astar_node_user_data* )current->user_data;
	start_data = ( astar_node_user_data* )start->user_data;
	end_data = ( astar_node_user_data* )end->user_data;
	
	// Count path length
	path_root = vul_astar_calculate_path( current );
	len = 1;
	while( path_root->next )
	{
		node_data = ( astar_node_user_data* )path_root->node_data;
		++len;
		path_root = path_root->next;
	}
	vul_astar_path_finalize( path_root );

	printf( "Current node: %p (%d, %d), path length %d\n", current,
			current_data->x, current_data->y, len );
}

void graph_print( vul_astar_graph *graph )
{
	astar_graph_user_data *graph_data;
	astar_node_user_data *node_data;
	vul_astar_node *node;
	s32 x, y;

	graph_data = ( astar_graph_user_data* )graph->user_data;
	for( y = ( s32 )graph_data->height - 1; y >= 0; --y ) {
		for( x = 0; x < ( s32 )graph_data->width; ++x ) {
			node = ( vul_astar_node* )vul_svector_get( graph->nodes, y * graph_data->width + x );
			node_data = ( astar_node_user_data* )node->user_data;
			printf( "%d", node_data->blocked ? 1 : 0 );
		}
		printf( "\n" );
	}
}
void astar_strategy_print( vul_astar_strategy strategy )
{
	printf( "Testing with " );
	if( strategy == VUL_ASTAR_STRATEGY_BEST_FIRST ) {
		printf( "best first\n" );
	} else if( strategy == VUL_ASTAR_STRATEGY_BREADTH_FIRST ) {
		printf( "breadth first\n" );
	} else if( strategy == VUL_ASTAR_STRATEGY_DEPTH_FIRST ) {
		printf( "depth first\n" );
	} else {
		assert( 0 && "Invalid search strategy" );
	}
}

void vul_test_astar( )
{
	u32 *bitfield, w, h, sx, sy, ex, ey;
	vul_astar_graph graph;
	vul_astar_result result;
	vul_astar_node *start, *end;
	astar_node_user_data user_data;
	vul_astar_strategy strat;

	/* Create the graph */
	bitfield = astar_input_from_string( &w, &h, &sx, &sy, &ex, &ey, level );
	graph.nodes = NULL;
	graph.user_data = NULL;
	astar_grid_construct_graph( &graph, w, h, bitfield );

	/* Get the start and end nodes */
	user_data.x = sx;
	user_data.y = sy;
	start = astar_get_node( &graph, &user_data );
	user_data.x = ex;
	user_data.y = ey;
	end = astar_get_node( &graph, &user_data );

	for( strat = VUL_ASTAR_STRATEGY_BEST_FIRST; strat < VUL_ASTAR_STRATEGY_Count; ++strat ) {
		/* Find the path */
		vul_astar_search( &result, &graph,
						  astar_grid_manhattan_dist,
						  astar_grid_is_final,
						  astar_neighbors,
						  astar_cost_neighbors,
						  start, end,
						  strat, 4,
						  ( VISUALIZE ? astar_grid_visualize : NULL ) );

		/* Print results */
		astar_strategy_print( strat );
		printf( "Modes considered: %llu\n", result.size_closed_set );
		printf( "Nodes still open: %llu\n", result.size_open_set );
		printf( "Length: %d\n", astar_grid_print_path( result.root, 0 ) - 1 );

		/* Clean up */
		vul_astar_path_finalize( result.root );

		/* Reset graph */
		vul_astar_graph_reset( &graph );
	}
	astar_grid_finalize_graph( &graph );
}
