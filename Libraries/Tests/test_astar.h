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


ui32_t astar__input_read_number_pair( ui32_t *x, ui32_t *y, char **p )
{
	ui32_t num_count;

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

void astar_free_bitfield( ui32_t *bitfield )
{
	free( bitfield );
}

void astar__bitfield_mark_block( ui32_t *bitfield,
								 ui32_t bitfield_width,
								 ui32_t x0, ui32_t y0,
								 ui32_t w, ui32_t h )
{
	ui32_t x, y, i, b;

	for( y = y0; y < y0 + h; ++y ) {
		for( x = x0; x < x0 + w; ++x ) {
			i = ( y * bitfield_width + x ) / 32;
			b = ( y * bitfield_width + x ) % 32;
			bitfield[ i ] |= ( 1 << b );
		}
	}
}
ui32_t *astar_input_from_string( ui32_t *w, ui32_t *h,
								 ui32_t *sx, ui32_t *sy,
								 ui32_t *ex, ui32_t *ey,
								 char *str )
{
	ui32_t *bitfield, x0, y0, xw, yh, bitfield_length;
	char *c;

	/* Read the size of the grid and allocate it */
	c = str;
	assert( astar__input_read_number_pair( w, h, &c ) );
	bitfield_length = ( ui32_t )ceil( ( f32_t )( ( *w ) * ( *h ) ) / 32.f );
	bitfield = ( ui32_t* )malloc( bitfield_length * sizeof( ui32_t ) );
	/* Initialize it to 0 */
	memset( bitfield, 0x00, bitfield_length * sizeof( ui32_t ) );
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
	ui32_t x, y, blocked;
} astar_node_user_data;

typedef struct astar_graph_user_data {
	ui32_t width, height;
} astar_graph_user_data;

/*
* Cost between neighbors is infinite if blocked, 1 otherwise (since we know the node's index is
* calculated from it's location with (y * width) + x).
*/
vul_astar_node_t *astar_get_node( vul_astar_graph_t *graph,
								  void *data )
{
	ui32_t index;
	astar_graph_user_data *graph_data;
	astar_node_user_data *node_data;
	vul_astar_node_t *node;

	node_data = ( astar_node_user_data* )data;
	graph_data = ( astar_graph_user_data* )graph->user_data;

	data = ( astar_graph_user_data* )graph->user_data;
	index = ( node_data->y * graph_data->width ) + node_data->x;

	node = ( vul_astar_node_t* )vul_svector_get( graph->nodes, index );

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
ui64_t astar_neighbors( vul_astar_node_t **neighbors,
						vul_astar_graph_t *graph,
						vul_astar_node_t *root,
						ui32_t max_neighbors )
{
	astar_node_user_data *node_data, *n_data, new_node;
	astar_graph_user_data *graph_data;
	vul_astar_node_t * n;
	ui32_t i;

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
f64_t astar_cost_neighbors( vul_astar_node_t *s,
							vul_astar_node_t *t )
{
	astar_node_user_data *node_data;

	node_data = ( astar_node_user_data* )t->user_data;
	return node_data->blocked == 0 ? 1.0 : F64_INF;
}

void astar_grid_construct_graph( vul_astar_graph_t *graph,
								 ui32_t width,
								 ui32_t height,
								 ui32_t *bitfield )
{
	ui32_t x, y, base;
	astar_graph_user_data *graph_data;
	astar_node_user_data *node_data;
	vul_astar_node_t *node;

	graph->user_data = malloc( sizeof( astar_graph_user_data ) );
	graph_data = ( astar_graph_user_data* )graph->user_data;
	/* Store sizes */
	graph_data->width = width;
	graph_data->height = height;

	/* Allocate space for the nodes (we make the base buffer big enough for the board for speed) */
	graph->nodes = vul_svector_create( sizeof( vul_astar_node_t ), 8, malloc, free );//width * height );

	/* For all the nodes, set their cost */
	for( y = 0u; y < height; ++y ) {
		base = y * width;
		for( x = 0u; x < width; ++x ) {
			/* Index into our bitfield */
			ui32_t blocked = ( bitfield[ ( base + x ) / 32 ]
							   & ( 1 << ( ( base + x ) % 32 ) ) )
							   == 0u ? 0u : 0xffffffff;
			/* Store whether it is blocked */
			node = ( vul_astar_node_t* ) vul_svector_append_empty( graph->nodes );
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

void astar__grid_finalizer( void *node_ptr, ui32_t index, void *nothing )
{
	vul_astar_node_t *node;

	node = ( vul_astar_node_t* )node_ptr;
	free( node->user_data );
}

int astar_grid_is_final( vul_astar_node_t *c, vul_astar_node_t *e )
{
	return c == e ? 1 : 0;
}

void astar_grid_finalize_graph( vul_astar_graph_t *graph )
{
	vul_svector_iterate( graph->nodes, astar__grid_finalizer, NULL );
	vul_svector_destroy( graph->nodes );

	free( graph->user_data );
}

f64_t astar_grid_manhattan_dist( vul_astar_node_t *s, vul_astar_node_t *t )
{
	astar_node_user_data *node_data_t, *node_data_s;

	node_data_t = ( astar_node_user_data* )t->user_data;
	node_data_s = ( astar_node_user_data* )s->user_data;
	return abs( ( i32_t )node_data_t->x - ( i32_t )node_data_s->x ) + abs( ( i32_t )node_data_t->y - ( i32_t )node_data_s->y );
}

f64_t astar_grid_euclidian_dist( vul_astar_node_t *s, vul_astar_node_t *t )
{
	astar_node_user_data *node_data_t, *node_data_s;
	f64_t dx, dy, d;

	node_data_t = ( astar_node_user_data* )t->user_data;
	node_data_s = ( astar_node_user_data* )s->user_data;

	dx = pow( ( f64_t )( ( f64_t )node_data_t->x - ( f64_t )node_data_s->x ), 2 );
	dy = pow( ( f64_t )( ( f64_t )node_data_t->y - ( f64_t )node_data_s->y ), 2 );

	d = sqrt( dx + dy );
	return d;
}

ui32_t astar_grid_print_path( vul_astar_path_node_t *root, int quiet )
{
	astar_node_user_data *data;
	ui32_t count;

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

void astar_grid_visualize( vul_astar_graph_t *graph, vul_astar_node_t *start, vul_astar_node_t *end, vul_astar_node_t *current )
{
	astar_node_user_data *node_data, *current_data, *start_data, *end_data;
	vul_astar_path_node_t *path_root;
	ui32_t len;

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

	printf( "Current node: %x (%d, %d), path length %d\n", current,
			current_data->x, current_data->y, len );
}

void graph_print( vul_astar_graph_t *graph )
{
	astar_graph_user_data *graph_data;
	astar_node_user_data *node_data;
	vul_astar_node_t *node;
	i32_t x, y;

	graph_data = ( astar_graph_user_data* )graph->user_data;
	for( y = ( i32_t )graph_data->height - 1; y >= 0; --y ) {
		for( x = 0; x < ( i32_t )graph_data->width; ++x ) {
			node = ( vul_astar_node_t* )vul_svector_get( graph->nodes, y * graph_data->width + x );
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
	ui32_t *bitfield, w, h, sx, sy, ex, ey;
	vul_astar_graph_t graph;
	vul_astar_result_t result;
	vul_astar_node_t *start, *end;
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
