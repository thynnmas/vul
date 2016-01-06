/*
* Villains' Utility Library - Thomas Martin Schmid, 2016. Public domain¹
*
* This file describes a generic A* implementation for any graph.
* Define VUL_ASTAR_ALLOC and VUL_ASTAR_FREE to your own allocator
* if you don't want to use malloc/free.
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
*
* Example use:
*
*/
#ifndef VUL_ASTAR_H
#define VUL_ASTAR_H

#include <stdio.h>
#include "vul_priority_heap.h"
#include "vul_queue.h"
#include "vul_stack.h"
#include "vul_stable_array.h"
#include "vul_types.h"
#ifndef F64_INF
#define F64_INF 1.79769e+308
#endif

#ifndef VUL_ASTAR_ALLOC
#define VUL_ASTAR_ALLOC malloc
#endif
#ifndef VUL_ASTAR_FREE
#define VUL_ASTAR_FREE free
#endif

/**
* If defined, the functions are defined and not just declared. Only do this in _one_ c/cpp file!
*/
//#define VUL_DEFINE

/* We keep the open and closed set as state within the nodes. */
typedef enum vul__astar_node_state {
	VUL_ASTAR_NODE_OPEN,
	VUL_ASTAR_NODE_CLOSED,
	VUL_ASTAR_NODE_UNDISCOVERED,
	VUL_ASTAR_NODE_STATE_Count
} vul__astar_node_state;

/*
* The nodes contain the f and g costs used during the search, as well as a
* pointer to the node we came from along the shortest path to this node used
* to trace back the path we find during search, and its state.
* It also contains implementation specific data, which must be defined later.
*/
typedef struct vul_astar_node_t {
	void *user_data;
	f64_t f_cost;
	f64_t g_cost;
	vul__astar_node_state state;
	struct vul_astar_node_t *path_parent;
} vul_astar_node_t;

/*
* The graph representation is problem specific, but we move this to the
* specification of the nodes. In general we have two operations on the graph,
* finding a node by a given vul_astar_location, and finding the neighbors of
* a given node. Both of these operations should be fast, so TODO: Use a map!
*/
typedef struct vul_astar_graph_t {
	void *user_data;
	vul_svector_t *nodes;
} vul_astar_graph_t;

/*
* A node on the path from root to end point.
*/
typedef struct vul_astar_path_node_t {
	void *node_data;
	struct vul_astar_path_node_t *next;
} vul_astar_path_node_t;

/*
* A* result struct. Contains statistics on the sizes of the sets
* as well as the path from root to end point. If no path was found
* the root node will be NULL. This path is allcoated in the astar_calculate_path
* function and must be freed once no longer needed with astar_path_finalize.
* Also contains the final node, for problems where finding that node it the goal.
*/
typedef struct vul_astar_result_t {
	ui64_t size_closed_set;
	ui64_t size_open_set;
	vul_astar_path_node_t *root;
	vul_astar_node_t *final_node;
} vul_astar_result_t;


typedef enum vul_astar_strategy {
	VUL_ASTAR_STRATEGY_BEST_FIRST,
	VUL_ASTAR_STRATEGY_DEPTH_FIRST,
	VUL_ASTAR_STRATEGY_BREADTH_FIRST,
	VUL_ASTAR_STRATEGY_Count
} vul_astar_strategy;

#ifndef VUL_DEFINE
/*
 * Performs an A* search on the given graph using the given heuristic.
 * Terminates if the node is found to be final by the given is_final function.
 *
 * - The heuristic must be monotonic for best first.
 * - is_final is to return true if the node is the final node/the end state.
 * - The neighbors function fills the given array of maximum 'max' neighbors with
 *   the reachable states from the current node (root).
 * - neighbor_cost returns the cost between two given nodes.
 *
 * Full comment regarding the neighbor function followes (copied from where it was before):
 * Fills at most max_neighbors neighbors into the neighbors array.
 * Returns the nunmber of neighbroughs filled in.
 * If a neighbor is unreachable (i.e. not a valid path
 * to follow, f.ex. a wall on a walkable grid), it should NOT be returned.
 * Nodes can be constructed here if they aren't constructed prior,
 * and should be added to the graph's node-vector. If they
 * are constructed here, make sure no duplicates are created;
 * search the node array first, only create if not already
 * existing, as we will otherwise have a node with multiple,
 * potentially conflicting states (@TODO(thynn): Which is why the node array should be a map!)
 */
void vul_astar_search( vul_astar_result_t *result,
					   vul_astar_graph_t *graph,
					   f64_t( *heuristic )( vul_astar_node_t *s, vul_astar_node_t *t ),
					   int( *is_final )( vul_astar_node_t *current, vul_astar_node_t *end ),
					   ui64_t( *neighbors )( vul_astar_node_t **neighbors, vul_astar_graph_t *graph, vul_astar_node_t *root, ui32_t max ),
					   f64_t( *cost )( vul_astar_node_t *s, vul_astar_node_t *t ),
					   vul_astar_node_t *start,
					   vul_astar_node_t *end,
					   vul_astar_strategy strategy,
					   ui32_t max_neighbors,
					   void( *visualize )( vul_astar_graph_t *graph, vul_astar_node_t *start, vul_astar_node_t *end, vul_astar_node_t *current ) );

/*
* Iteration function for the general purpose iterator of vul_stable_vector_t
* that resets the a node.
*/
void vul__astar_node_reseter( void *node_ptr, ui32_t index, void *nothing );

/*
* Reset the state of all nodes in the graph to undiscovered. Allows multiple searches
* on the same graph without reconstruction.
*/
void vul_astar_graph_reset( vul_astar_graph_t *graph );

/*
* Traces back the path from the given end node to the start node (identified
* by a NULL parent node). Allocates ever path node along the way.
* Free the path by passing the root node (returned from this) to 
* vul_astar_path_finalize.
*/
vul_astar_path_node_t *vul_astar_calculate_path( vul_astar_node_t *end );

/*
* Frees the memory allocated by vul_astar_calculate_path in the path from the
* given root node.
*/
void vul_astar_path_finalize( vul_astar_path_node_t *root );

/*
* Comparison function used to sort astar_nodes in the search based on their
* f_cost value.
*/
int vul__astar_node_comparator( void *a, void *b );

//-------------------------------------------------------------------------------
// Helper functions to pop, peek and push that help keep the main function clean

void vul__astar_open_set_pop( vul_astar_strategy strategy, void *set, void *data_out );
vul_astar_node_t *vul_astar__open_set_peek( vul_astar_strategy strategy, void *set );
void vul__astar_open_set_push( vul_astar_strategy strategy, void *set, vul_astar_node_t **node );
void *vul__astar_open_set_create( vul_astar_strategy strategy );
void vul__astar_open_set_finalize( vul_astar_strategy strategy, void *set );
int vul__astar_open_set_is_empty( vul_astar_strategy strategy, void *set );

#endif

#endif

#ifdef VUL_DEFINE

void vul_astar_search( vul_astar_result_t *result,
					   vul_astar_graph_t *graph,
					   f64_t( *heuristic )( vul_astar_node_t *s, vul_astar_node_t *t ),
					   int( *is_final )( vul_astar_node_t *current, vul_astar_node_t *end ),
					   ui64_t( *neighbors )( vul_astar_node_t **neighbors, vul_astar_graph_t *graph, vul_astar_node_t *root, ui32_t max ),
					   f64_t( *cost )( vul_astar_node_t *s, vul_astar_node_t *t ),
					   vul_astar_node_t *start,
					   vul_astar_node_t *end,
					   vul_astar_strategy strategy,
					   ui32_t max_neighbors,
					   void( *visualize )( vul_astar_graph_t *graph, vul_astar_node_t *start, vul_astar_node_t *end, vul_astar_node_t *current ) )
	{
	void *open_set;
	vul_astar_node_t *n, **nb, *best;
	ui64_t c, i;
	f64_t g;

	/* Initialize the open set */
	open_set = vul__astar_open_set_create( strategy );

	/* Initialize the neighborood */
	nb = ( vul_astar_node_t** )VUL_ASTAR_ALLOC( sizeof( vul_astar_node_t* ) * max_neighbors );
	n = NULL;
	best = NULL; // Store the best node we've seen so far by our heuristic, so we can
				 // return something if we don't solve the problem.

	/* Mark the start node as open */
	start->state = VUL_ASTAR_NODE_OPEN;
	vul__astar_open_set_push( strategy, open_set, &start );
	start->path_parent = NULL;
	start->g_cost = 0;
	start->f_cost = heuristic( start, end );

	result->size_closed_set = 0;
	result->size_open_set = 1;

	while( !vul__astar_open_set_is_empty( strategy, open_set ) )
	{
		/* Pop the node with the lowest f_cost */
		vul__astar_open_set_pop( strategy, open_set, ( void* )&n );

		/* If is is the best so far, store it */
		if( best == NULL || n->f_cost < best->f_cost ) {
			best = n;
		}

		/* Visualize the current state, if a visualization-function is given */
		if( visualize ) {
			visualize( graph, start, end, n );
		}
		/* Close the node */
		n->state = VUL_ASTAR_NODE_CLOSED;
		++result->size_closed_set;
		/* If n is the end node, we are done */
		if( is_final( n, end ) ) {
			/* Calculate the path */
			result->root = vul_astar_calculate_path( n );
			result->final_node = n;
			return;
		}
		/* Fetch the neighbors */
		c = neighbors( nb, graph, n, max_neighbors );
		for( i = 0; i < c; ++i )
		{
			/* We have a monotonic heuristic, so we can skip closed nodes */
			if( nb[ i ]->state == VUL_ASTAR_NODE_CLOSED ) {
				continue;
			}
			/* Calculate tentative g */
			g = n->g_cost + cost( n, nb[ i ] );
			/* Since h is monotonic we only care if not already open */
			if( nb[ i ]->state != VUL_ASTAR_NODE_OPEN || g < nb[ i ]->g_cost )
			{
				/* Store parent node */
				nb[ i ]->path_parent = n;
				/* Store costs */
				nb[ i ]->g_cost = g;
				nb[ i ]->f_cost = g + heuristic( nb[ i ], end );
				if( nb[ i ]->state != VUL_ASTAR_NODE_OPEN ) {
					nb[ i ]->state = VUL_ASTAR_NODE_OPEN;
					vul__astar_open_set_push( strategy, open_set, &nb[ i ] );
					++result->size_open_set;
				}
			}
		}
	}
	result->root = NULL;
	result->final_node = best; // root being NULL indicates we failed to solve it, but our closest
	// node is returned as the final because it might be of interest

	// Clean up
	vul__astar_open_set_finalize( strategy, open_set );
	free( nb );
}

void astar__node_reseter( void *node_ptr, ui32_t index, void *nothing )
{
	vul_astar_node_t *node;

	node = ( vul_astar_node_t* )node_ptr;
	node->state = VUL_ASTAR_NODE_UNDISCOVERED;
	node->path_parent = NULL;
	node->g_cost = 0.0;
	node->f_cost = 0.0;
}

void vul_astar_graph_reset( vul_astar_graph_t *graph )
{
	// Our stable vector has a GP iterator...
	vul_svector_iterate( graph->nodes, astar__node_reseter, NULL );
}

/*
* Comparison function used to sort astar_nodes in the search based on their
* f_cost value.
*/
int astar__node_comparator( void *a, void *b )
{
	f64_t diff, eps;
	vul_astar_node_t *na, *nb;

	na = *( vul_astar_node_t** )a;
	nb = *( vul_astar_node_t** )b;

	diff = na->f_cost - nb->f_cost;
	eps = 1e-8;
	if( diff < eps ) {
		return -1;
	} else if( diff > eps ) {
		return 1;
	} else {
		return 0;
	}
}


/*
* Traces back the path from the given end node to the start node (identified
* by a NULL parent node). Allocates ever path node along the way.
*/
vul_astar_path_node_t *vul_astar_calculate_path( vul_astar_node_t *end )
{
	vul_astar_path_node_t *cur, *last;

	last = NULL;
	while( end ) {
		cur = ( vul_astar_path_node_t* )VUL_ASTAR_ALLOC( sizeof( vul_astar_path_node_t ) );
		cur->next = last;
		cur->node_data = end->user_data;

		last = cur;
		end = end->path_parent;
	}

	return cur;
}

void vul_astar_path_finalize( vul_astar_path_node_t *root )
{
	vul_astar_path_node_t *next;

	while( root ) {
		next = root->next;
		VUL_ASTAR_FREE( root );
		root = next;
	}
}

void vul__astar_open_set_pop( vul_astar_strategy strategy, void *set, void *data_out )
{
	if( strategy == VUL_ASTAR_STRATEGY_BEST_FIRST ) {
		vul_priority_heap_pop( ( vul_priority_heap_t* )set, data_out );
	} else if( strategy == VUL_ASTAR_STRATEGY_BREADTH_FIRST ) {
		vul_queue_pop( ( vul_queue_t* )set, data_out );
	} else if( strategy == VUL_ASTAR_STRATEGY_DEPTH_FIRST ) {
		vul_stack_pop( ( vul_stack_t* )set, data_out );
	} else {
		assert( 0 && "Invalid search strategy" );
	}
}
vul_astar_node_t *vul__astar_open_set_peek( vul_astar_strategy strategy, void *set )
{
	if( strategy == VUL_ASTAR_STRATEGY_BEST_FIRST ) {
		return ( vul_astar_node_t* )vul_priority_heap_peek( ( vul_priority_heap_t* )set );
	} else if( strategy == VUL_ASTAR_STRATEGY_BREADTH_FIRST ) {
		return ( vul_astar_node_t* )vul_queue_peek( ( vul_queue_t* )set );
	} else if( strategy == VUL_ASTAR_STRATEGY_DEPTH_FIRST ) {
		return ( vul_astar_node_t* )vul_stack_peek( ( vul_stack_t* )set );
	} else {
		assert( 0 && "Invalid search strategy" );
	}
	return NULL;
}
void vul__astar_open_set_push( vul_astar_strategy strategy, void *set, vul_astar_node_t **node )
{
	if( strategy == VUL_ASTAR_STRATEGY_BEST_FIRST ) {
		vul_priority_heap_push( ( vul_priority_heap_t* )set, node );
	} else if( strategy == VUL_ASTAR_STRATEGY_BREADTH_FIRST ) {
		vul_queue_push( ( vul_queue_t* )set, node );
	} else if( strategy == VUL_ASTAR_STRATEGY_DEPTH_FIRST ) {
		vul_stack_push( ( vul_stack_t* )set, node );
	} else {
		assert( 0 && "Invalid search strategy" );
	}
}

void *vul__astar_open_set_create( vul_astar_strategy strategy )
{
	if( strategy == VUL_ASTAR_STRATEGY_BEST_FIRST ) {
		return vul_priority_heap_create( sizeof( vul_astar_node_t* ), 
										 astar__node_comparator,
										 VUL_ASTAR_ALLOC,
										 VUL_ASTAR_FREE );
	} else if( strategy == VUL_ASTAR_STRATEGY_BREADTH_FIRST ) {
		return vul_queue_create( sizeof( vul_astar_node_t* ), 
								 VUL_ASTAR_ALLOC, 
								 VUL_ASTAR_FREE );
	} else if( strategy == VUL_ASTAR_STRATEGY_DEPTH_FIRST ) {
		return vul_stack_create( sizeof( vul_astar_node_t* ), 
								 8, 
								 VUL_ASTAR_ALLOC, 
								 VUL_ASTAR_FREE );
	} else {
		assert( 0 && "Invalid search strategy" );
	}
	return NULL;
}

void vul__astar_open_set_finalize( vul_astar_strategy strategy, void *set )
{
	if( strategy == VUL_ASTAR_STRATEGY_BEST_FIRST ) {
		vul_priority_heap_destroy( ( vul_priority_heap_t* )set );
	} else if( strategy == VUL_ASTAR_STRATEGY_BREADTH_FIRST ) {
		vul_queue_destroy( ( vul_queue_t* )set );
	} else if( strategy == VUL_ASTAR_STRATEGY_DEPTH_FIRST ) {
		vul_stack_destroy( ( vul_stack_t* )set );
	} else {
		assert( 0 && "Invalid search strategy" );
	}
}

int vul__astar_open_set_is_empty( vul_astar_strategy strategy, void *set )
{
	if( strategy == VUL_ASTAR_STRATEGY_BEST_FIRST ) {
		return vul_priority_heap_is_empty( ( vul_priority_heap_t* )set );
	} else if( strategy == VUL_ASTAR_STRATEGY_BREADTH_FIRST ) {
		return vul_queue_is_empty( ( vul_queue_t* )set );
	} else if( strategy == VUL_ASTAR_STRATEGY_DEPTH_FIRST ) {
		return vul_stack_is_empty( ( vul_stack_t* )set );
	} else {
		assert( 0 && "Invalid search strategy" );
	}
	return 1;
}


#endif