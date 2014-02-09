/*
 * Villains' Utility Library - Thomas Martin Schmid, 2014. Public domain¹
 *
 * This file describes a general graph as well as a few useful functions on them.
 * @TODO: Dijkstra etc.
 * 
 * ¹ If public domain is not legally valid in your country and or legal area,
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

#ifndef VUL_GRAPH_H
#define VUL_GRAPH_H

#include <malloc.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "vul_types.h"
#include "vul_resizable_array.h"

/**
 * If defined, the functions are defined and not just declared. Only do this in _one_ c/cpp file!
 */
//#define VUL_DEFINE

typedef struct vul_node_t vul_node_t;
struct vul_node_t{
	/**
	 * The content of the graph.
	 */
	void *content;
	/**
	 * Parent node
	 */
	vul_node_t *parent;
	/**
	 * Children
	 */
	vul_vector_t *children;
	/**
	 * Useful for traversal
	 */
	bool visited;
	// @TODO: More things that might be useful; cost, heuristics etc. Repace our children vector with vector_t< vul_edge_t >, where
	// vul_edge_t contains a vul_node_t as well as edge information (weight etc.)
	vul_node_t( )
	{
		content = NULL;
		parent = NULL;
		visited = false;
		children = vul_vector_create( sizeof( vul_node_t ) );
	}
};

/**
 * Strategy to use when DFS iterating over a graph; do we visit
 * nodes before or after visiting their children.
 */
enum vul_graph_dfs_strategy {
	VUL_GRAPH_PRE,
	VUL_GRAPH_POST
};

/**
 * Traverses a tree depth first and executes the given function at each node.
 * Assumes there are no loops in the graph!
 */
#ifndef VUL_DEFINE
void vul_graph_dfs( vul_node_t *root, void (*func)( vul_node_t *root ), vul_graph_dfs_strategy strategy = VUL_GRAPH_POST );
#else
void vul_graph_dfs( vul_node_t *root, void (*func)( vul_node_t *root ), vul_graph_dfs_strategy strategy = VUL_GRAPH_POST )
{
	vul_node_t **n, *c;
	vul_vector_t *stack = vul_vector_create( sizeof( vul_node_t* ) );
	i32_t offset;
	
	// Push the root
	root->visited = false;
	vul_vector_add( stack, &root );

	if( strategy == VUL_GRAPH_POST ) {
		// While stack is not empty
		while( vul_vector_size( stack ) > 0) {
			// Peek
			offset = vul_vector_size( stack ) - 1;
			n = ( vul_node_t** )vul_vector_get( stack, offset );
			// Handle
			if( ( *n )->visited ) {
				// If visited, execute func and pop
				func( *n );
				vul_vector_remove_cascade( stack, vul_vector_size( stack ) - 1 );
			} else {
				// Otherwise push children (the wrong way around)
				for( i32_t i = vul_vector_size( ( *n )->children ) - 1; i >= 0 ; --i ) {
					n = ( vul_node_t** )vul_vector_get( stack, offset );
					c = ( vul_node_t* )vul_vector_get( ( *n )->children, i );
					c->visited = false;
					vul_vector_add( stack, &c );
				}
				// Damn thing might even have changed after the last add. Our vector-implementation being evil, I suppose.
				n = ( vul_node_t** )vul_vector_get( stack, offset );
				// And mark as visited
				( *n )->visited = true;
			}
		}
	} else {
		// While stack is not empty
		while( vul_vector_size( stack ) > 0) {
			// Peek
			offset = vul_vector_size( stack ) - 1;
			n = ( vul_node_t** )vul_vector_get( stack, offset );
			// Handle
			func( *n );
			// Otherwise push children
			for( i32_t i = vul_vector_size( ( *n )->children ) - 1; i >= 0 ; --i ) {
				// Need to refetch n every time because the stack might've been moved, sadly.
				n = ( vul_node_t** )vul_vector_get( stack, offset );
				c = ( vul_node_t* )vul_vector_get( ( *n )->children, i );
				c->visited = false;
				vul_vector_add( stack, &c );
			}
			// And remove it.
			vul_vector_remove_cascade( stack, vul_vector_size( stack ) - ( 1 + vul_vector_size( ( *n )->children ) ) );
		}
	}
	vul_vector_destroy( stack );
}
#endif

/**
 * Traverses a tree depth first and executes the given function at each node.
 * Assumes there are no loops in the graph!
 */
#ifndef VUL_DEFINE
void vul_graph_bfs( vul_node_t *root, void (*func)( vul_node_t *root ) );
#else
void vul_graph_bfs( vul_node_t *root, void (*func)( vul_node_t *root ) )
{
	// @TODO: Rewqrite this with a queue.
	vul_node_t **n, *c;
	vul_vector_t *stack = vul_vector_create( sizeof( vul_node_t* ) );
	
	// Push the root
	root->visited = false;
	vul_vector_add( stack, root );

	// While stack is not empty
	while( vul_vector_size( stack ) > 0) {
		// Peek
		n = ( vul_node_t** )vul_vector_get( stack, vul_vector_size( stack ) - 1 );
		// Do things
		func( *n );
		// Push children
		for( ui32_t i = 0; i < vul_vector_size( ( *n )->children ); ++i ) {
			c = ( vul_node_t* )vul_vector_get( ( *n )->children, i );
			vul_vector_add( stack, c );
		}
		// Pop n, which means top - numchildren.
		vul_vector_remove_cascade( stack, vul_vector_size( stack ) - ( 1 + vul_vector_size( ( *n )->children ) ) );
	}
}
#endif

/**
 * Helper function that frees nodes.
 */
#ifndef VUL_DEFINE
void vul__graph_node_free( vul_node_t *node );
#else
void vul__graph_node_free( vul_node_t *node )
{
	if( node->children != NULL ) {
		vul_vector_destroy( node->children );
	}

	free( node->content );
}
#endif

/**
 * Deletes a tree. DFS traverses the tree and deletes the nodes post-visit.
 */
#ifndef VUL_DEFINE
void vul_graph_delete( vul_node_t *root );
#else
void vul_graph_delete( vul_node_t *root )
{
	vul_graph_dfs( root, vul__graph_node_free );
}
#endif

/**
 * Copies the content pointed to at content to a new node inserted under parent.
 * Returns a pointer to the new node.
 */
#ifndef VUL_DEFINE
vul_node_t *vul_graph_insert( vul_node_t *parent, void *content, ui32_t size_content );
#else
vul_node_t *vul_graph_insert( vul_node_t *parent, void *content, ui32_t size_content )
{
	// Create the node
	vul_node_t n;
	// Store the pointer to the parent
	n.parent = parent;
	// No chilren
	n.children = vul_vector_create( sizeof( vul_node_t ) );
	// Store content
	n.content = malloc( size_content );
	assert( n.content != NULL ); // Make sure malloc didn't fail
	memcpy( n.content, content, size_content );
	// Default to false
	n.visited = false;

	// Store it as a child in the parent
	vul_vector_add( parent->children, &n );

	// And return it
	return ( vul_node_t* )vul_vector_get( parent->children, vul_vector_size( parent->children ) - 1 );
}
#endif

#endif