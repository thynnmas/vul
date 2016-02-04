#include "../vul_csp.h"

#include <stdio.h>
#include <malloc.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <limits.h>
#include <float.h>

#ifdef _WIN32
#define snprintf sprintf_s
#endif

typedef struct csp_graph_color_t {
	const char *name;
	union {
		struct {
			f32 r, g, b, a;
		} rgba;
		f32 val[ 4 ];
	};
} csp_graph_color_t;

typedef struct csp_graph_pos_t {
	f32 x, y;
} csp_graph_pos_t;

typedef struct csp_graph_node_t {
	csp_graph_pos_t pos;
	vul_vector *neighbors; // index into csp_graph_t->nodes. 
} csp_graph_node_t;

typedef struct csp_graph_t {
	vul_svector *nodes; // csp_graph_node_t (variables in constraints are indices/pointers to these).
	vul_svector *colors; // csp_graph_color_t (domains in constraints are indices/pointers to these).
} csp_graph_t;

static const char *csp_static_color_names[ 6 ] = { "red", "green", "blue", "yellow", "purple", "teal" };
static float csp_static_colors[ 6 ][ 3 ] = { { 1.f, 0.2f, 0.2f },
											 { 0.2f, 1.f, 0.2f },
											 { 0.2f, 0.2f, 1.f },
											 { 0.8f, 0.8f, 0.2f },
											 { 0.8f, 0.2f, 0.8f },
											 { 0.2f, 0.8f, 0.8f } };
void csp_graph_create_k_colors( csp_graph_t *graph, u32 k )
{
	u32 i;
	csp_graph_color_t *c;

	if( k <= 6 )  {
		// Use predefined colors
		for( i = 0; i < k; ++i ) {
			c = ( csp_graph_color_t* )vul_svector_append_empty( graph->colors );
			c->rgba.r = csp_static_colors[ i ][ 0 ];
			c->rgba.g = csp_static_colors[ i ][ 1 ];
			c->rgba.b = csp_static_colors[ i ][ 2 ];
			c->rgba.a = 1.f;
			c->name = csp_static_color_names[ i ];
		}
	} else {
		// Random colors
		for( i = 0; i < k; ++i ) {
			c = ( csp_graph_color_t* )vul_svector_append_empty( graph->colors );
			c->rgba.r = ( f32 )rand( ) / ( f32 )RAND_MAX;
			c->rgba.g = ( f32 )rand( ) / ( f32 )RAND_MAX;
			c->rgba.b = ( f32 )rand( ) / ( f32 )RAND_MAX;
			c->rgba.a = 1.f;
			c->name = "random";
		}
	}
	c = ( csp_graph_color_t* )vul_svector_append_empty( graph->colors );
	c->name = "Unassigned";
	c->rgba.r = 0.04f;
	c->rgba.g = 0.04f;
	c->rgba.b = 0.04f;
	c->rgba.a = 1.f;

}

u32 graph__input_read_vertex( u32 *i, f32 *x, f32 *y, char **p )
{
	u32 num_count;

	num_count = 0;
	while( **p ) {
		/* If character is a number, read the number */
		if( isdigit( **p ) ) {
			if( num_count++ == 0 ) {
				*i = strtol( *p, p, 10 );
			} else {
				if( num_count == 2 ) {
					if( ( *( *p - 1 ) ) == '-' ) {
						*x = -( f32 )strtod( *p, p );
					} else {
						*x = ( f32 )strtod( *p, p );
					}
				} else {
					if( ( *( *p - 1 ) ) == '-' ) {
						*y = -( f32 )strtod( *p, p );
					} else {
						*y = ( f32 )strtod( *p, p );
					}
					return 1;
				}
			}
		} else {
			++( *p );
		}
	}
	return 0;
}
u32 graph__input_read_edge( u32 *a, u32 *b, char **p )
{
	u32 num_count;

	num_count = 0;
	while( **p ) {
		/* If character is a number, read the number */
		if( isdigit( **p ) ) {
			if( num_count++ % 2 == 0 ) {
				*a = strtol( *p, p, 10 );
			} else {
				*b = strtol( *p, p, 10 );
				return 1;
			}
		} else {
			++( *p );
		}
	}
	return 0;
}
csp_graph_t *graph_input_from_string( u32 *vert_count, u32 *edge_count, u32 K, char *str )
{
	csp_graph_t *graph;
	csp_graph_node_t *node;
	u32 a, b, i;
	f32 x, y;

	/* Read the graph size and allocate it */
	assert( graph__input_read_edge( vert_count, edge_count, &str ) );
	graph = ( csp_graph_t* )malloc( sizeof( csp_graph_t ) );
	graph->nodes = vul_svector_create( sizeof( csp_graph_node_t ), 32, malloc, free );
	graph->colors = vul_svector_create( sizeof( csp_graph_color_t ), 32, malloc, free );
	/* Create the colors */
	csp_graph_create_k_colors( graph, K );

	/* Read the vertex coordinates */
	for( i = 0; i < *vert_count; ++i ) {
		vul_svector_append_empty( graph->nodes );
	} // Create them first since they aren't guaranteed to come in order
	for( a = 0; a < *vert_count; ++a ) {
		assert( graph__input_read_vertex( &i, &x, &y, &str ) );
		node = ( csp_graph_node_t* )vul_svector_get( graph->nodes, i );
		node->pos.x = x;
		node->pos.y = y;
		node->neighbors = vul_vector_create( sizeof( u32 ), K,
											 malloc, free, realloc ); // If solvable, we're bounded by K
	}

	/* Read the edges */
	for( i = 0; i < *edge_count; ++i ) {
		assert( graph__input_read_edge( &a, &b, &str ) );
		node = ( csp_graph_node_t* )vul_svector_get( graph->nodes, a );
		vul_vector_add( node->neighbors, &b );
	}

	return graph;
}

int csp_color_comparator( void *a, void *b )
{
	csp_graph_color_t *ca, *cb;

	ca = ( csp_graph_color_t* )a;
	cb = ( csp_graph_color_t* )b;

	return ( ( ca->rgba.r == cb->rgba.r )
			 && ( ca->rgba.g == cb->rgba.g )
			 && ( ca->rgba.b == cb->rgba.b )
			 && ( ca->rgba.a == cb->rgba.a ) ) ? 0 : 1;
}

int csp_color_test( u32 count, vul_csp_var *vars )
{
	assert( count == 2 );
	assert( vars[ 0 ].bound_value->size == sizeof( csp_graph_color_t ) );
	assert( vars[ 1 ].bound_value->size == sizeof( csp_graph_color_t ) );
	return csp_color_comparator( vars[ 0 ].bound_value->data, vars[ 1 ].bound_value->data );
}

void csp_graph_create_astar( vul_astar_graph *agraph, csp_graph_t *graph, u32 K )
{
	u32 i, j, idnode2;
	csp_graph_node_t *node;
	vul_csp_var *var;
	vul_csp_constraint *c;
	vul_csp_type *t;
	vul_svector *dom;

	vul_gac_astar_graph_user_data *udata;

	agraph->nodes = vul_svector_create( sizeof( vul_astar_node ), 32, malloc, free );
	agraph->user_data = ( vul_gac_astar_graph_user_data* )malloc( sizeof( vul_gac_astar_graph_user_data ) );
	udata = ( vul_gac_astar_graph_user_data* )agraph->user_data;

	udata->user_data = graph;
	udata->cnet = ( vul_gac_cnet* )malloc( sizeof( vul_gac_cnet ) );

	/* Each node is a variable */
	udata->cnet->variables = vul_svector_create( sizeof( vul_csp_var ),
												 vul_svector_size( graph->nodes ), 
												 malloc, free );
	for( i = 0; i < vul_svector_size( graph->nodes ); ++i ) {
		var = vul_svector_append_empty( udata->cnet->variables );
		var->id = i; // These are from the input and are assumed to be unique
		var->bound_value = NULL; // Start off unbound
	}

	/* Each variable has a full domain at the start */
	udata->cnet->domains = vul_svector_create( sizeof( vul_svector* ),
													 vul_svector_size( graph->nodes ),
													 malloc, free );
	for( i = 0; i < vul_svector_size( graph->nodes ); ++i ) {
		dom = vul_svector_create( sizeof( vul_csp_type ), K, malloc, free );
		// The domains a csp_type_ts pointing to the the actual color values of graph->colors
		for( j = 0; j < K; ++j ) {
			t = vul_svector_append_empty( dom );
			t->size = sizeof( csp_graph_color_t );
			t->data = vul_svector_get( graph->colors, j );
		}
		vul_svector_append( udata->cnet->domains, &dom );
	}

	/* For each edge, we create a constraint (this assumes we get edges A-B only once,
	and not both A-B and B-A). Our size estimate means we only have one allocation in this loop at 3 edges
	per node. */
	udata->cnet->constraints = vul_svector_create( sizeof( vul_csp_constraint ),
														 vul_svector_size( graph->nodes ),
														 malloc, free );
	for( i = 0; i < vul_svector_size( graph->nodes ); ++i ) {
		node = ( csp_graph_node_t* )vul_svector_get( graph->nodes, i );
		for( j = 0; j < vul_vector_size( node->neighbors ); ++j ) {
			idnode2 = *( u32* )vul_vector_get( node->neighbors, j );

			c = vul_svector_append_empty( udata->cnet->constraints );
			// Each constraint has 2 variables, the two nodes, and accompanying domains
			c->var_count = 2;
			c->vars = ( vul_csp_var** )malloc( sizeof( vul_csp_var* ) * 2 );
			c->vars[ 0 ] = ( vul_csp_var* )vul_svector_get( udata->cnet->variables, i );
			c->vars[ 1 ] = ( vul_csp_var* )vul_svector_get( udata->cnet->variables, idnode2 );
			c->doms = ( vul_svector** )malloc( sizeof( vul_svector* ) * 2 );
			c->doms[ 0 ] = ( vul_svector* )vul_svector_get( udata->cnet->domains, i );
			c->doms[ 1 ] = ( vul_svector* )vul_svector_get( udata->cnet->domains, j );
			// Create the constraint test string
			c->test = csp_color_test;
		}
	}
}

void csp_graph_finalize_astar( vul_astar_graph *graph )
{
	u32 i;
	vul_gac_astar_graph_user_data *udata;
	csp_graph_node_t *n;
	csp_graph_t *cg;

	if( !graph ) {
		return;
	}

	udata = ( vul_gac_astar_graph_user_data* )graph->user_data;
	/* CNET */
	vul_gac_cnet_finalize( udata->cnet );

	/* CSP Graph */
	// For each node in csp_graph, destroy neightbor array, destroy vector
	cg = ( csp_graph_t* )udata->user_data;
	for( i = 0; i < vul_svector_size( cg->nodes ); ++i ) {
		n = ( csp_graph_node_t* )vul_svector_get( cg->nodes, i );
		vul_vector_destroy( n->neighbors );
	}
	vul_svector_destroy( cg->nodes );

	// Destroy color vector
	vul_svector_destroy( cg->colors );
	// Free the csp_graph itself
	free( cg );

	/* Astar nodes */
	// For each node created in the graph, delete the gac node (the graph node is already gone), then destroy
	// the node vector.
	for( i = 0; i < vul_svector_size( graph->nodes ); ++i ) {
		vul__csp_graph_finalize_astar_node( ( vul_astar_node* )vul_svector_get( graph->nodes, i ) );
	}
	vul_svector_destroy( graph->nodes );

	// Delete the user data
	free( udata );
	// Finally don't free the graph, it's on the stack
}

/*
* Print usage help
*/
void print_usage_gac( )
{
	printf( "Usage:\tgraph [option]\n"
			"Options:\t-f [file path] K\n"
			"\t\t-r [raw data] K\n"
			"\t\t\tWhere K is the number of colors allowed.\n"
			"\t\t-q"
			"\t\t\tQuiet mode, doesn't output the final path to stdout.\n"
			"\t\t-g"
			"\t\t\tGui mode. Display the work ina  GUI during execution.\n"
			"\t\t-a"
			"\t\t\tRuns all three search types. Default is best-first only.\n" );
}


/*
* Concatenate together all arguments from argv[ 2 ] onwards;
* we don't want them separate.
*/
char *concat_strings_gac( u32 count, char **args )
{
	size_t c, len, *lens;
	char *str, *strp;

	lens = ( size_t* )malloc( sizeof( size_t ) * count );
	len = 0;
	for( c = 0; c < count; ++c ) {
		lens[ c ] = strlen( args[ c + 2 ] );
		len += lens[ c ];
	}
	str = ( char* )malloc( len + 1 );
	strp = str;
	for( c = 0; c < count; ++c ) {
		snprintf( strp, lens[ c ], "%s", args[ c + 2 ] );
		strp += lens[ c ];
	}
	str[ len ] = 0;
	free( lens );

	return str;
}

void print_solution( vul_gac_node_data *node, u32 K )
{
	u32 i;
	csp_graph_color_t *c;

	printf( "Solution\t(vertex: color ):\n" );
	if( K <= 6 ) {
		for( i = 0; i < vul_svector_size( node->var_insts ); ++i ) {
			printf( "\t\t(%d: %s)\n", i,
					( ( csp_graph_color_t* )( ( vul_csp_type* )vul_svector_get(
					( ( vul_csp_variable_instance* )vul_svector_get( node->var_insts, i ) )->dom_inst,
					0 ) )->data )->name );
		}
	} else {
		for( i = 0; i < vul_svector_size( node->var_insts ); ++i ) {
			c = ( csp_graph_color_t* )( ( vul_csp_type* )vul_svector_get(
				( ( vul_csp_variable_instance* )vul_svector_get( node->var_insts, i ) )->dom_inst,
				0 ) )->data;
			printf( "\t\t(%d: %s(%f,%f,%f))\n", i,
					c->name, c->rgba.r, c->rgba.g, c->rgba.b );
		}
	}
}

void astar_strategy_print( vul_astar_strategy strategy );

char *TEST_CSP_GRAPH = "40 94\n"
"0 0 5\n"
"1 3 3\n"
"2 3 7\n"
"3 4 5\n"
"4 5 4\n"
"5 5 6\n"
"6 6 5\n"
"7 7 3\n"
"8 7 7\n"
"9 10 5\n"
"10 0 25\n"
"11 3 23\n"
"12 3 27\n"
"13 4 25\n"
"14 5 24\n"
"15 5 26\n"
"16 6 25\n"
"17 7 23\n"
"18 7 27\n"
"19 10 25\n"
"20 20 5\n"
"21 23 3\n"
"22 23 7\n"
"23 24 5\n"
"24 25 4\n"
"25 25 6\n"
"26 26 5\n"
"27 27 3\n"
"28 27 7\n"
"29 30 5\n"
"30 20 25\n"
"31 23 23\n"
"32 23 27\n"
"33 24 25\n"
"34 25 24\n"
"35 25 26\n"
"36 26 25\n"
"37 27 23\n"
"38 27 27\n"
"39 30 25\n"
"0 1\n"
"0 2\n"
"1 2\n"
"2 8\n"
"1 7\n"
"7 8\n"
"8 9\n"
"7 9\n"
"3 4\n"
"3 5\n"
"3 6\n"
"4 5\n"
"4 6\n"
"5 6\n"
"4 7\n"
"6 7\n"
"5 8\n"
"6 8\n"
"2 3\n"
"2 5\n"
"1 3\n"
"1 4\n"
"10 11\n"
"10 12\n"
"11 12\n"
"12 18\n"
"11 17\n"
"17 18\n"
"18 19\n"
"17 19\n"
"13 14\n"
"13 15\n"
"13 16\n"
"14 15\n"
"14 16\n"
"15 16\n"
"14 17\n"
"16 17\n"
"15 18\n"
"16 18\n"
"12 13\n"
"12 15\n"
"11 13\n"
"11 14\n"
"20 21\n"
"20 22\n"
"21 22\n"
"22 28\n"
"21 27\n"
"27 28\n"
"28 29\n"
"27 29\n"
"23 24\n"
"23 25\n"
"23 26\n"
"24 25\n"
"24 26\n"
"25 26\n"
"24 27\n"
"26 27\n"
"25 28\n"
"26 28\n"
"22 23\n"
"22 25\n"
"21 23\n"
"21 24\n"
"30 31\n"
"30 32\n"
"31 32\n"
"32 38\n"
"31 37\n"
"37 38\n"
"38 39\n"
"37 39\n"
"33 34\n"
"33 35\n"
"33 36\n"
"34 35\n"
"34 36\n"
"35 36\n"
"34 37\n"
"36 37\n"
"35 38\n"
"36 38\n"
"32 33\n"
"32 35\n"
"31 33\n"
"31 34\n"
"9 19\n"
"9 20\n"
"9 30\n"
"19 20\n"
"19 30\n"
"20 30\n";
int TEST_CSP_K = 4;

void vul_test_csp( )
{
	u32 vc, ec, K, i, j, quiet, gui, all;
	csp_graph_t *csp_graph;
	vul_astar_graph graph;
	vul_astar_result result;
	vul_astar_node *start, *end;
	vul_gac_astar_node_user_data user_data;
	vul_astar_strategy strat;
	char *str, *p, in[ 2 ];

	
	K = TEST_CSP_K;
	csp_graph = graph_input_from_string( &vc, &ec, K, TEST_CSP_GRAPH );

	csp_graph_create_astar( &graph, csp_graph, K );

	vul_csp_graph_initialize( &graph );
	
	for( strat = VUL_ASTAR_STRATEGY_BEST_FIRST;
		 strat < VUL_ASTAR_STRATEGY_Count + 1;
		 ++strat ) {

		start = vul_svector_get( graph.nodes, 0 ); // First and only node is start
		end = NULL; // End node is unknown

		// @TODO: Some timing...
		vul_astar_search( &result,
						  &graph,
						  vul_gac_heuristic,
						  vul_gac_is_final,
						  vul_gac_neighbors,
						  vul_gac_cost_neighbors,
						  start, end,
						  strat,
						  K, // Each node has exactly as many potential neighbors as we can make
							 // assumptions, for each node we can only assume a valid color, <= K
							 // This is a restriction we've made to make the tree not flat, see comment
							 // for vul_astar_neighbors
						  NULL );

		astar_strategy_print( strat );
		if( result.root ) {
			print_solution( ( ( vul_gac_astar_node_user_data* )result.final_node->user_data )->gac_node, K );
		}

		printf( "Failed constraint count: %d\n", vul_gac_count_failed( result.final_node ) );
		printf( "Vertices without color assignment: %d\n", vul_gac_count_unassigned( result.final_node ) );
		printf( "Modes considered: %llu\n", result.size_closed_set );
		printf( "Nodes still open: %llu\n", result.size_open_set );
		printf( "Length: %d\n", astar_grid_print_path( result.root, 1 ) - 1 );

		if( strat + 1 < VUL_ASTAR_STRATEGY_Count ) {
			vul_csp_graph_reset( &graph );
		}
	}

	csp_graph_finalize_astar( &graph );
}
