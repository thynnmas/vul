/*
* Villains' Utility Library - Thomas Martin Schmid, 2016. Public domain¹
*
* This file describes a constraint satisfaciton solver using the an implementation
* of the general arc consistency algorithm and vul_astar for a best-first
* search of the problem space.
*
* It uses the allocators defined in vul_astar, that is VUL_ASTAR_ALLOC to allocate,
* and VUL_ASTAR_FREE to deallocate.
*
* @TODO(thynn): Copy the more full description from the exercise.
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
#ifndef VUL_CSP_H
#define VUL_CSP_H

#include "vul_types.h"
#include "vul_stable_array.h"
#include "vul_queue.h"
#include "vul_astar.h"

/**
* If defined, the functions are defined and not just declared. Only do this in _one_ c/cpp file!
*/
//#define VUL_DEFINE

typedef struct vul_csp_type_t {
	ui32_t size;
	void *data;
} vul_csp_type_t;

typedef struct vul_csp_var_t {
	ui32_t id;					 // Unique identifier. If you're using string names, hash them....
	vul_csp_type_t *bound_value; // This is used in revise only, and is null for the variables in CNET.
	// It is simple here to let us have one less type, and will probably go
	// at the next refactor that touches code involving it...
} vul_csp_var_t;

typedef struct vul_csp_constraint_t {
	ui32_t var_count;

	vul_csp_var_t **vars;
	vul_svector_t **doms; // csp_type_t
	int( *test )( ui32_t count, vul_csp_var_t *vars );
} vul_csp_constraint_t;

typedef struct vul_csp_variable_instance_t {
	vul_csp_var_t *var;
	vul_svector_t *dom_inst; // csp_type_t
} vul_csp_variable_instance_t;

typedef struct vul_csp_constraint_instance_t {
	vul_csp_constraint_t *constraint;

	vul_csp_variable_instance_t **var_insts;
} vul_csp_constraint_instance_t;

/*
* Top level data structure of GAC that keeps all the constraints
* the full variable domains and the variables.
* Everything internally in the search handles instances or
* references to these.
*/
typedef struct vul_gac_cnet_t {
	vul_svector_t *constraints; // csp_constraint_t
	vul_svector_t *variables; // csp_var_t (with bound_Value = NULL)
	vul_svector_t *domains;	// vul_svector_t*< csp_type_t >
} vul_gac_cnet_t;

/*
* Search node data. Contains a reference to the variable we just made
* an asusmption about to get here (NULL in root node), constraint instances
* and variable instances.
*/
typedef struct vul_gac_node_data_t {
	ui32_t assumption_var_index; // 0xffffffff if no assumption was made

	ui32_t constraint_count;
	vul_csp_constraint_instance_t *const_insts; // These have pointers to the instance array below

	vul_svector_t *var_insts; // csp_variable_instance_t
} vul_gac_node_data_t;

/*
* Used in revise and revise_recurse, this holds the index into the constraint instances
* variable instance a binding is for, and the index into its domain that is bound.
*/
typedef struct vul__gac_revise_binding {
	ui32_t index;
	ui32_t value;
} vul__gac_revise_binding;

typedef struct vul_gac_revise_part {
	vul_csp_constraint_instance_t *c;
	ui32_t focal;
} vul_gac_revise_pair;

/**
 * User data for A* nodes
 */
typedef struct vul_gac_astar_node_user_data {
	vul_gac_node_data_t *gac_node;
	void *user_data; // For the user
} vul_gac_astar_node_user_data;

/*
 * User data for the A* graph
 */
typedef struct vul_gac_astar_graph_user_data {
	vul_gac_cnet_t *cnet;
	void *user_data;
} vul_gac_astar_graph_user_data;

#endif

/*
* Creates a initial search node with full domains, no assumption made
* and runs the initial GAC iteration.
*/
#ifndef VUL_DEFINE
void vul_csp_graph_initialize( vul_astar_graph_t *graph );
#else
void vul_csp_graph_initialize( vul_astar_graph_t *graph )
{
	vul_gac_node_data_t *gac_node;
	vul_astar_node_t *anode;
	vul_gac_astar_node_user_data *ndata;

	// Call gac_graph_initialize to start GAC and create the intial node.
	gac_node = vul_gac_graph_initialize( graph );

	// Create the astar_node so we can check if we're done
	anode = ( vul_astar_node_t* )vul_svector_append_empty( graph->nodes );
	anode->user_data = ( vul_gac_astar_node_user_data* )VUL_ASTAR_ALLOC( sizeof( vul_gac_astar_node_user_data ) );
	ndata = ( vul_gac_astar_node_user_data* )anode->user_data;
	ndata->gac_node = gac_node;
	ndata->user_data = NULL;

	anode->f_cost = 0.0;
	anode->g_cost = 0.0;
	anode->state = VUL_ASTAR_NODE_UNDISCOVERED;
	anode->path_parent = NULL;
}
#endif

/*
* Helper function that compares a csp_var_t instance with a
* csp_variable_instance_t.
* Simply compares their id. Used to find things in vectors.
*/
#ifndef VUL_DEFINE
int vul__csp_variable_instance_var_comparator( void *a, void *b );
#else
int vul__csp_variable_instance_var_comparator( void *a, void *b )
{
	// This actually compares csp_variable_instance_t (b) with a csp_var_t (a)
	vul_csp_var_t *va, *vb;

	va = ( vul_csp_var_t* )a;
	vb = ( ( vul_csp_variable_instance_t* )b )->var;

	return va->id == vb->id ? 0 : 1;
}
#endif

/*
* Helper function that compares two csp_variable_instance_t instances
* by comraing the id of their variables. Used to ind things in vectors.
*/
#ifndef VUL_DEFINE
int vul__csp_variable_instance_var_inst_comparator( void *a, void *b );
#else
int vul__csp_variable_instance_var_inst_comparator( void *a, void *b )
{
	vul_csp_var_t *va, *vb;

	va = ( ( vul_csp_variable_instance_t* )a )->var;
	vb = ( ( vul_csp_variable_instance_t* )b )->var;

	return va->id == vb->id ? 0 : 1;
}
#endif

/*
* Deallocates a node. Does NOT touch any of the things referenced in CNET.
*/
#ifndef VUL_DEFINE
void vul__csp_graph_finalize_astar_node( vul_astar_node_t *node );
#else
void vul__csp_graph_finalize_astar_node( vul_astar_node_t *node )
{
	ui32_t i;
	vul_gac_astar_node_user_data *ndata;

	ndata = ( vul_gac_astar_node_user_data* )node->user_data;

	vul_gac_node_finalize( ndata->gac_node );
	if( ndata->user_data ) {
		VUL_ASTAR_FREE( ndata->user_data );
	}

	VUL_ASTAR_FREE( ndata );
}
#endif

/*
* Resets an astar graph for a graph coloring problem.
* Destroys the astar_graph, then rebuilds it.
*/
#ifndef VUL_DEFINE
void vul_csp_graph_reset( vul_astar_graph_t *graph );
#else
void vul_csp_graph_reset( vul_astar_graph_t *graph )
{
	ui32_t i;

	// Clean up the astar_graph.
	for( i = 0; i < vul_svector_size( graph->nodes ); ++i ) {
		vul__csp_graph_finalize_astar_node( ( vul_astar_node_t* )vul_svector_get( graph->nodes, i ) );
	}
	vul_svector_destroy( graph->nodes );
	graph->nodes = vul_svector_create( sizeof( vul_astar_node_t ), 32, 
									   VUL_ASTAR_ALLOC, VUL_ASTAR_FREE );

	// Reinitialize
	vul_csp_graph_initialize( graph );
}
#endif

/*
* Recursively binds the first free variable to every possibility in its
* domain and calls itself with it bound. If zero variables are free, it
* performs the revise step and returns, true if for any of the combinations,
* the constraint holds, false otherwise.
* bound is a vector of type gac__revise_binding.
*
* @NOTE(thynn): Yes this contains GOTOs, but they are function internal 
* and simplify the logic quite a bit, so tough...
*/
#ifndef VUL_DEFINE
int vul__gac_revise_recurse( vul_csp_constraint_instance_t *cons, 
							 vul_svector_t *bound );
#else
int vul__gac_revise_recurse( vul_csp_constraint_instance_t *cons, 
							 vul_svector_t *bound )
{
	ui32_t i, j, unknown, found;
	ui32_t bound_count;
	vul_csp_var_t *lvars;
	vul_svector_t *dom;
	vul__gac_revise_binding *b;

	bound_count = vul_svector_size( bound );

	//Check that we have not bound more variables than we have;
	assert( cons->constraint->var_count - bound_count >= 0 );
	
	if( cons->constraint->var_count - bound_count == 0 ) {
		// We have a trivial case; no more binding to do.
		lvars = ( vul_csp_var_t* )VUL_ASTAR_ALLOC( sizeof( vul_csp_var_t ) 
												   * cons->constraint->var_count );
		for( i = 0; i < bound_count; ++i ) {
			b = ( vul__gac_revise_binding* )vul_svector_get( bound, i );
			lvars[ b->index ].id = cons->var_insts[ b->index ]->var->id;
			lvars[ b->index ].bound_value = ( vul_csp_type_t* )vul_svector_get( cons->var_insts[ b->index ]->dom_inst,
																				b->value );
		}

		// Test if the constraint is satisfied, if it is, return
		if( cons->constraint->test( bound_count, lvars ) ) {
			return 1;
		}
	} else {
		// We have free variables. For all values in its domain, bind
		// the first free variable and call ourselves.
		unknown = 0xffffffff;
		for( i = 0; i < cons->constraint->var_count; ++i ) {
			for( j = 0; j < bound_count; ++j ) {
				if( ( ( vul__gac_revise_binding* )vul_svector_get( bound, j ) )->index == i ) {
					goto GAC_REVISE_FOUND;
				}
			}
			unknown = i;
			goto GAC_REVISE_BIND;
		GAC_REVISE_FOUND:
			// Keep looping
			continue;
		}
		// We claim there are free variables but can't find any, so assert false
		assert( VUL_FALSE );
		return 0;
	GAC_REVISE_BIND:
		dom = cons->var_insts[ unknown ]->dom_inst;
		b = ( vul__gac_revise_binding* )vul_svector_append_empty( bound );
		for( i = 0; i < vul_svector_size( dom ); ++i ) {
			b->index = unknown;
			b->value = i;
			// If it holds for this binding, return
			if( vul__gac_revise_recurse( cons, bound ) ) {
				return 1;
			}
			// Otherwise keep trying
		}
		// Until we've exhausted every possibility, then return false
	}

	return 0;
}
#endif

/*
* Performs a single general arc-consistency revision.
* Returns true if the focal variable's domain was restricted.
* Uses gac__revise_recurse internally.
*/
#ifndef VUL_DEFINE
int vul_gac_revise( vul_csp_constraint_instance_t *cons, 
					ui32_t focal_index );
#else
int vul_gac_revise( vul_csp_constraint_instance_t *cons, 
					ui32_t focal_index )
{
	ui32_t i, ret, del;
	i32_t j;
	vul_svector_t *to_remove, *bound, *dom;
	vul__gac_revise_binding *b;

	bound = vul_svector_create( sizeof( vul__gac_revise_binding ), 
								cons->constraint->var_count,
							    VUL_ASTAR_ALLOC, 
								VUL_ASTAR_FREE );
	ret = 0; // No removes unless we find any
	to_remove = vul_svector_create( sizeof( ui32_t ), 8,
									VUL_ASTAR_ALLOC,
									VUL_ASTAR_FREE );
	dom = cons->var_insts[ focal_index ]->dom_inst;
	for( i = 0; i < vul_svector_size( dom ); ++i ) {
		b = ( vul__gac_revise_binding* )vul_svector_append_empty( bound );
		b->index = focal_index;
		b->value = i;
		// If this binding does not satisfy the constrain for any combination of the other variables' possible
		// bindings, we can delete it
		if( !vul__gac_revise_recurse( cons, bound ) ) {
			// Mark that we should return true and mark the value for removal (don't remove while iterating!)
			ret = 1;
			vul_svector_append( to_remove, &i );
		}
		// Free the bound array since it may have been altered in the recurvise call
		vul_svector_freemem( bound );
	}
	if( ret ) {
		// Remove in descending order (to_remove is indices, and ascending )
		for( j = vul_svector_size( to_remove ) - 1; j >= 0; --j ) {
			vul_svector_remove_swap( dom, *( ui32_t* )vul_svector_get( to_remove, j ) );
		}
	}
	vul_svector_destroy( to_remove );
	vul_svector_destroy( bound );
	return ret;
}
#endif

/*
* This performs the GAC algorithm over all constraints.
* Don't call manually, gets called from gac_initialize or gac_rerun.
*/
#ifndef VUL_DEFINE
void vul_gac_deduction( vul_queue_t *queue,
						ui32_t constraint_count,
						vul_csp_constraint_instance_t *const_insts );
#else
void vul_gac_deduction( vul_queue_t *q, 
						ui32_t constraint_count, 
						vul_csp_constraint_instance_t *const_insts )
{
	ui32_t i, j, k;
	ui32_t add; // boolean
	vul_gac_revise_pair next, pair;
	vul_csp_variable_instance_t *var_inst;

	while( !vul_queue_is_empty( q ) ){
		vul_queue_pop( q, ( void* )&next );
		if( vul_gac_revise( next.c, next.focal ) ) {
			// Domain was reduced, push any constraint that involves focal onto queue again.
			for( i = 0; i < constraint_count; ++i ) {
				if( &const_insts[ i ] == next.c ) continue; // We can skip this since is it was just revised!
				for( j = 0; j < const_insts[ i ].constraint->var_count; ++j ) {
					var_inst = const_insts[ i ].var_insts[ j ];
					// Check if this constraint contains a reference to X*. 
					if( next.c->var_insts[ next.focal ]->var->id == var_inst->var->id ) {
						// For every variable except the one we just checked, add it
						for( k = 0; k < const_insts[ i ].constraint->var_count; ++k ) {
							if( k == j ) continue;
							pair.c = &const_insts[ i ];
							pair.focal = k;
							vul_queue_push( q, &pair );
						}
					}
				}
			}
		}
	}
}
#endif

/*
* This initializes the queue with all constraint pairs
* and calls gac_deduction. This works on the CNET constraints,
* but we actually want constraint instances because we generate them
* for the initial search state anyway, and it simlpifies the work
* we do here not to have to recreate them.
*/
#ifndef VUL_DEFINE
void vul_gac_initialize( ui32_t constraint_count, 
						 vul_csp_constraint_instance_t *const_insts );
#else
void vul_gac_initialize( ui32_t constraint_count, 
						 vul_csp_constraint_instance_t *const_insts )
{
	ui32_t i, j;
	vul_gac_revise_pair pair;
	vul_queue_t *q;

	q = vul_queue_create( sizeof( vul_gac_revise_pair ),
						  VUL_ASTAR_ALLOC,
						  VUL_ASTAR_FREE );

	for( i = 0; i < constraint_count; ++i ){
		for( j = 0; j < const_insts[ i ].constraint->var_count; ++j ) {
			pair.c = &const_insts[ i ];
			pair.focal = j;
			vul_queue_push( q, &pair );
		}
	}

	// Now perform deduction
	vul_gac_deduction( q, constraint_count, const_insts );
}
#endif

/*
* This initializes the queue based on a just made assumption,
* and then calls gac_eduction.
* It works on constraint instances.
*/
#ifndef VUL_DEFINE
void vul_gac_rerun( ui32_t constraint_count, 
					vul_csp_constraint_instance_t *const_insts, 
					vul_csp_var_t *assumption );
#else
void vul_gac_rerun( ui32_t constraint_count,
				vul_csp_constraint_instance_t *const_insts,
				vul_csp_var_t *assumption )
{
	ui32_t i, j, k;
	vul_gac_revise_pair pair;
	vul_queue_t *q;
	vul_csp_variable_instance_t *var_inst;

	q = vul_queue_create( sizeof( vul_gac_revise_pair ),
						  VUL_ASTAR_ALLOC,
						  VUL_ASTAR_FREE );

	// Domain was reduced, push any constraint that involves the assumption onto queue again.
	for( i = 0; i < constraint_count; ++i ) {
		for( j = 0; j < const_insts[ i ].constraint->var_count; ++j ) {
			// Check if this constraint contains a reference X*
			if( const_insts[ i ].var_insts[ j ]->var->id == assumption->id ) {
				// For every variable extect the one we just checked, add it
				for( k = 0; k < const_insts[ i ].constraint->var_count; ++k ) {
					if( k == j ) continue;
					pair.c = &const_insts[ i ];
					pair.focal = k;

					vul_queue_push( q, &pair );
				}
			}
		}
	}

	// Then perform deduction
	vul_gac_deduction( q, constraint_count, const_insts );
}
#endif

/*
* Finalizes the CNET.
*/
#ifndef VUL_DEFINE
void vul_gac_cnet_finalize( vul_gac_cnet_t *cnet );
#else
void vul_gac_cnet_finalize( vul_gac_cnet_t *cnet )
{
	ui32_t i, j;
	vul_csp_constraint_t *c;

	// For each constriant, free doms, var, test, then destroy the vector
	for( i = 0; i < vul_svector_size( cnet->constraints ); ++i ) {
		c = ( vul_csp_constraint_t* )vul_svector_get( cnet->constraints, i );
		VUL_ASTAR_FREE( c->vars );
		VUL_ASTAR_FREE( c->doms );
	}
	vul_svector_destroy( cnet->constraints );

	// For each domain, destroy the vector, then destroy the outer vector
	for( i = 0; i < vul_svector_size( cnet->domains ); ++i ) {
		vul_svector_destroy( *( ( vul_svector_t** )vul_svector_get( cnet->domains, i ) ) );
	}
	vul_svector_destroy( cnet->domains );

	// Destroy the variable vector
	vul_svector_destroy( cnet->variables );

	// And free the cnet
	VUL_ASTAR_FREE( cnet );
}
#endif

/*
* Finalizes a single gac_node_data_t.
*/
#ifndef VUL_DEFINE
void vul_gac_node_finalize( vul_gac_node_data_t *node );
#else
void vul_gac_node_finalize( vul_gac_node_data_t *node )
{
	ui32_t i;
	vul_csp_variable_instance_t *v;

	// Delete all the variable instances
	for( i = 0; i < vul_svector_size( node->var_insts ); ++i ) {
		v = ( vul_csp_variable_instance_t* )vul_svector_get( node->var_insts, i );
		vul_svector_destroy( v->dom_inst );
	}

	// Delete all the constraint instances
	for( i = 0; i < node->constraint_count; ++i ) {
		VUL_ASTAR_FREE( node->const_insts[ i ].var_insts );
	}

	VUL_ASTAR_FREE( node->const_insts );
	vul_svector_destroy( node->var_insts );

	VUL_ASTAR_FREE( node );
}
#endif


/*
* Counts the number of vertices with non-singular domains.
*/
#ifndef VUL_DEFINE
int vul_gac_count_unassigned( vul_astar_node_t *node );
#else
int vul_gac_count_unassigned( vul_astar_node_t *node )
{
	ui32_t i, j, ret;
	vul_gac_astar_node_user_data *udata;
	vul_gac_node_data_t *gnode;

	udata = ( vul_gac_astar_node_user_data* )node->user_data;
	gnode = udata->gac_node;

	// Test if we are done
	ret = 0;
	for( i = 0; i < gnode->constraint_count; ++i ) {
		for( j = 0; j < gnode->const_insts[ i ].constraint->var_count; ++j ) {
			if( vul_svector_size( gnode->const_insts[ i ].var_insts[ j ]->dom_inst ) != 1 ) {
				++ret;
			}
		}
	}
	return ret;
}
#endif

/*
* Count the number of constraints not satisfied by the given node.
*/
#ifndef VUL_DEFINE
int vul_gac_count_failed( vul_astar_node_t *node );
#else
int vul_gac_count_failed( vul_astar_node_t *node )
{
	ui32_t i, j, k, l, ret;
	vul_gac_astar_node_user_data *udata;
	vul_gac_node_data_t *gnode;

	udata = ( vul_gac_astar_node_user_data* )node->user_data;
	gnode = udata->gac_node;

	// Test if we are done
	ret = 0;
	for( i = 0; i < gnode->constraint_count; ++i ) {
		for( j = 0; j < gnode->const_insts[ i ].constraint->var_count; ++j ) {
			if( vul_svector_size( gnode->const_insts[ i ].var_insts[ j ]->dom_inst ) != 1 ) {
				for( k = 0; k < gnode->constraint_count; ++k ) {
					if( k == i ) continue;
					for( l = 0; l < gnode->const_insts[ k ].constraint->var_count; ++l ) {
						if( gnode->const_insts[ k ].constraint->vars[ l ]->id
							== gnode->const_insts[ i ].var_insts[ j ]->var->id ) {
							++ret;
							goto GAC__COUNT_FAILED_LOOP_END;
						}
					}
				}
			}
		}
	GAC__COUNT_FAILED_LOOP_END:
		continue;
	}
	return ret;
}
#endif

/*
* Initializes the graph, creates the first node (without assumption)
* and perform the initial deduction (calls gac_initialize).
* Returns the created first node; no user data allocated.
*/
#ifndef VUL_DEFINE
vul_gac_node_data_t *vul_gac_graph_initialize( vul_astar_graph_t *graph );
#else
vul_gac_node_data_t *vul_gac_graph_initialize( vul_astar_graph_t *graph )
{
	ui32_t i, j, size;
	vul_gac_node_data_t *gac_node;
	vul_csp_constraint_t *c;
	vul_gac_astar_graph_user_data *udata;
	vul_csp_variable_instance_t *v;
	vul_astar_node_t *anode;
	vul_gac_astar_node_user_data *ndata;

	udata = ( vul_gac_astar_graph_user_data* )graph->user_data;

	// Create the first search node
	gac_node = ( vul_gac_node_data_t* )malloc( sizeof( vul_gac_node_data_t ) );
	gac_node->assumption_var_index = 0xffffffff;
	gac_node->constraint_count = vul_svector_size( udata->cnet->constraints );
	gac_node->const_insts = ( vul_csp_constraint_instance_t* )VUL_ASTAR_ALLOC( sizeof( vul_csp_constraint_instance_t )
																			   * gac_node->constraint_count );
	gac_node->var_insts = vul_svector_create( sizeof( vul_csp_variable_instance_t ),
											  vul_svector_size( udata->cnet->variables ),
											  VUL_ASTAR_ALLOC, VUL_ASTAR_FREE);

	// For each variable, create variable isntances
	for( i = 0; i < vul_svector_size( udata->cnet->variables ); ++i ) {
		v = ( vul_csp_variable_instance_t* )vul_svector_append_empty( gac_node->var_insts );
		v->var = ( vul_csp_var_t* )vul_svector_get( udata->cnet->variables, i );
		size = vul_svector_size(
			*( ( vul_svector_t** )vul_svector_get( udata->cnet->domains, i ) ) );
		v->dom_inst = vul_svector_create( sizeof( vul_csp_type_t ), size,
										  VUL_ASTAR_ALLOC, VUL_ASTAR_FREE );
		// And create the full domain
		for( j = 0; j < size; ++j ) {
			vul_svector_append( v->dom_inst,
									  vul_svector_get(
									  *( ( vul_svector_t** )vul_svector_get(
									  udata->cnet->domains, i ) ), j ) );
		}
	}

	// For all constraints, create constraint instances
	for( i = 0; i < gac_node->constraint_count; ++i ) {
		c = ( vul_csp_constraint_t* )vul_svector_get( udata->cnet->constraints, i );
		gac_node->const_insts[ i ].constraint = c;
		// For all variables involved in this constraint, link them from the array above.
		gac_node->const_insts[ i ].var_insts = ( vul_csp_variable_instance_t** )VUL_ASTAR_ALLOC( sizeof( vul_csp_variable_instance_t* )
																								 * c->var_count );
		for( j = 0; j < c->var_count; ++j ) {
			// Find the correct variable instance
			v = ( vul_csp_variable_instance_t* )vul_svector_find( gac_node->var_insts,
																  c->vars[ j ],
																  vul__csp_variable_instance_var_comparator );
			assert( v ); // Just to make sure we catch faults early
			gac_node->const_insts[ i ].var_insts[ j ] = v;
		}
	}

	// Now run a full GAC iteration; we might not need to search!
	vul_gac_initialize( gac_node->constraint_count, gac_node->const_insts );

	return gac_node;
}
#endif

/*
* Checks if a node is in a valid state, i.e.
* if all variable instances have non-empty domain instances.
*/
#ifndef VUL_DEFINE
int vul_gac_is_valid( vul_astar_node_t *anode );
#else
int vul_gac_is_valid( vul_astar_node_t *anode )
{
	ui32_t i;
	vul_gac_node_data_t *node;
	vul_csp_variable_instance_t *v;

	node = ( ( vul_gac_astar_node_user_data* )anode->user_data )->gac_node;

	for( i = 0; i < vul_svector_size( node->var_insts ); ++i ) {
		v = ( vul_csp_variable_instance_t* )vul_svector_get( node->var_insts, i );
		if( vul_svector_size( v->dom_inst ) == 0 ) {
			return 0;
		}
	}
	return 1;
}
#endif

/*
* Internal function used by astar_neighbor to create new nodes.
* It copies the domain instances and variable instances of the
* parent node, restricting the variable at index var_restrict
* to the value in it's domain at index var_dom_restrict,
* and marks that as the made assumption.
* Then it marks the node as undiscovered with 0-costs.
*/
#ifndef VUL_DEFINE
void vul__gac_create_astar_node_copy( vul_astar_node_t **anode,
									  vul_astar_graph_t *graph,
									  vul_astar_node_t *parent,
									  vul_gac_node_data_t *to_copy,
									  ui32_t var_restrict,
									  ui32_t var_dom_restrict );
#else
void vul__gac_create_astar_node_copy( vul_astar_node_t **anode,
									  vul_astar_graph_t *graph,
									  vul_astar_node_t *parent,
									  vul_gac_node_data_t *to_copy,
									  ui32_t var_restrict,
									  ui32_t var_dom_restrict )
{
	ui32_t i, j, k;
	vul_gac_astar_graph_user_data *gdata;
	vul_gac_astar_node_user_data *ndata;
	vul_gac_node_data_t *node;
	vul_csp_variable_instance_t *v, *vc;
	vul_csp_type_t *t, *tc;

	gdata = ( vul_gac_astar_graph_user_data* )graph->user_data;

	*anode = ( vul_astar_node_t* )VUL_ASTAR_ALLOC( sizeof( vul_astar_node_t ) );
	( *anode )->user_data = ( vul_gac_astar_node_user_data* )VUL_ASTAR_ALLOC( sizeof( vul_gac_astar_node_user_data ) );

	// Copy the GAC node
	node = ( vul_gac_node_data_t* )VUL_ASTAR_ALLOC( sizeof( vul_gac_node_data_t ) );
	node->assumption_var_index = var_restrict;
	node->constraint_count = to_copy->constraint_count;

	node->var_insts = vul_svector_create( sizeof( vul_csp_variable_instance_t ),
										  vul_svector_size( to_copy->var_insts ),
										  VUL_ASTAR_ALLOC,
										  VUL_ASTAR_FREE );
	for( i = 0; i < vul_svector_size( to_copy->var_insts ); ++i ) {
		v = ( vul_csp_variable_instance_t* )vul_svector_append_empty( node->var_insts );
		vc = ( vul_csp_variable_instance_t* )vul_svector_get( to_copy->var_insts, i );
		v->var = vc->var;
		v->dom_inst = vul_svector_create( sizeof( vul_csp_type_t ),
												i == var_restrict ? 1 :
												vul_svector_size( vc->dom_inst ),
												VUL_ASTAR_ALLOC,
												VUL_ASTAR_FREE );
		if( i == var_restrict ) {
			// Restrict the domain
			t = ( vul_csp_type_t* )vul_svector_append_empty( v->dom_inst );
			tc = ( vul_csp_type_t* )vul_svector_get( vc->dom_inst, var_dom_restrict );
			t->size = tc->size;
			t->data = tc->data;
		} else {
			// Copy domain
			for( j = 0; j < vul_svector_size( vc->dom_inst ); ++j ) {
				t = ( vul_csp_type_t* )vul_svector_append_empty( v->dom_inst );
				tc = ( vul_csp_type_t* )vul_svector_get( vc->dom_inst, j );
				t->size = tc->size;
				t->data = tc->data;
			}
		}
	}

	node->const_insts = ( vul_csp_constraint_instance_t* )VUL_ASTAR_ALLOC( sizeof( vul_csp_constraint_instance_t )
																  * node->constraint_count );
	for( i = 0; i < node->constraint_count; ++i ) {
		// Copy the constraints
		node->const_insts[ i ].constraint = to_copy->const_insts[ i ].constraint;
		// Find the correct variable instances in the above copied array.
		node->const_insts[ i ].var_insts = ( vul_csp_variable_instance_t** )VUL_ASTAR_ALLOC( sizeof( vul_csp_variable_instance_t* )
																							 * node->const_insts[ i ].constraint->var_count );
		for( j = 0; j < to_copy->const_insts[ i ].constraint->var_count; ++j ) {
			// Find the correct variable instance
			v = ( vul_csp_variable_instance_t* )vul_svector_find( node->var_insts,
																  to_copy->const_insts[ i ].var_insts[ j ],
																  vul__csp_variable_instance_var_inst_comparator );
			assert( v ); // Make sure we found a valid one.
			node->const_insts[ i ].var_insts[ j ] = v;
		}
	}

	// Create the user_data of the astar_node
	ndata = ( vul_gac_astar_node_user_data* )( *anode )->user_data;
	ndata->gac_node = node;
	ndata->user_data = NULL; // @TODO: Might want to copy user data here; would require passing it in to function

	// astar_node internal data.
	( *anode )->f_cost = 0.0;
	( *anode )->g_cost = 0.0;
	( *anode )->state = VUL_ASTAR_NODE_UNDISCOVERED;
	( *anode )->path_parent = parent;
}
#endif

/*
* Determines if the current node (c) is final. We don't have a known end
* node in GAC, so e is always NULL. This return true iff all variable
* domains are reduced to size 1.
* This is where we perform deduction! It is called when a search node is closed.
*/
#ifndef VUL_DEFINE
int vul_gac_is_final( vul_astar_node_t *c, vul_astar_node_t *e );
#else
int vul_gac_is_final( vul_astar_node_t *c, vul_astar_node_t *e )
{
	ui32_t i, j, ret;
	vul_gac_astar_node_user_data *udata;
	vul_gac_node_data_t *gnode;
	vul_csp_var_t *var;

	udata = ( vul_gac_astar_node_user_data* )c->user_data;
	gnode = udata->gac_node;

	// Reduce the domain
	if( gnode->assumption_var_index != 0xffffffff ) {
		vul_gac_rerun( gnode->constraint_count, gnode->const_insts,
					   ( ( vul_csp_variable_instance_t* )vul_svector_get( gnode->var_insts,
																		  gnode->assumption_var_index ) )->var );
	}

	// Test if we are done
	ret = 0;
	for( i = 0; i < gnode->constraint_count; ++i ) {
		for( j = 0; j < gnode->const_insts[ i ].constraint->var_count; ++j ) {
			if( vul_svector_size( gnode->const_insts[ i ].var_insts[ j ]->dom_inst ) != 1 ) {
				return 0;
			}
		}
	}
	return 1;
}
#endif

/*
* Create neighbor-function: Make assumptions; order is important here: return the best candidate first!
* best candidate is the one that constrains the most. Perform deduction and don't return illegal states.
*/
#ifndef VUL_DEFINE
ui64_t vul_gac_neighbors( vul_astar_node_t **neighbors,
						  vul_astar_graph_t *graph,
						  vul_astar_node_t *root,
						  ui32_t max_neighbors );
#else
ui64_t vul_gac_neighbors( vul_astar_node_t **neighbors,
						  vul_astar_graph_t *graph,
						  vul_astar_node_t *root,
						  ui32_t max_neighbors )
{
	// Any assumption for any variable without an assignemnt is a valid neighbor. 
	// However this would lead to a flat structure (since all potential assignments are
	// expanded in the first step). Instead, we define our neighbors to be any valid
	// assignment for _some_ open variable. Thus, first, we select a variable to assign a value to
	// and then create all potential neighbors for that variable (by assuming all variables in the domain).
	// We can do both these things in smart ways (select variable in most constraints of the ones with the 
	// smallest domain).
	// This way we get size(domain) children at each level, and the tree is size(variables) tall; a much more
	// useful distribution.
	// However, since we are not doing deductions here (we do those when we test if a node is the solution, i.e.
	// when we close the search node), we can return invalid neighbors here. We don't want to keep
	// expanding past those, so before we do any expansion, check that this search node is valid; if it is not,
	// return nothing.

	ui32_t i, j, k, vi, vc, vcmax, vdmin, vd;
	vul_gac_node_data_t *node;
	vul_csp_variable_instance_t *v;

	// If not valid, make a dead end
	if( !vul_gac_is_valid( root ) ) {
		return 0;
	}

	node = ( ( vul_gac_astar_node_user_data* )root->user_data )->gac_node;

	// Select variable to make assumption about: select the one involved in most 
	// constraints of the ones with the smallest domain
	vcmax = 0;
	vdmin = 0x7fffffff;
	for( i = 0; i < vul_svector_size( node->var_insts ); ++i ) {
		vc = 0;
		v = ( vul_csp_variable_instance_t* )vul_svector_get( node->var_insts, i );
		vd = vul_svector_size( v->dom_inst );
		if( vd <= 1 ) {
			continue; // Already constriced, no reason to assume anything further
		}
		if( vd < vdmin ) {
			for( j = 0; j < node->constraint_count; ++j ) {
				for( k = 0; k < node->const_insts[ j ].constraint->var_count; ++k ) {
					if( node->const_insts[ j ].var_insts[ k ]->var->id == v->var->id ) {
						vc += 1;
						break;
					}
				}
			}
			if( vc > vcmax ) {
				vi = i;
				vcmax = vc;
				vdmin = vd;
			}
		}
	}

	// Create the neighbors
	v = ( vul_csp_variable_instance_t* )vul_svector_get( node->var_insts, vi );
	assert( vul_svector_size( v->dom_inst ) <= max_neighbors ); // Make sure we have room
	for( i = 0; i < vul_svector_size( v->dom_inst ); ++i ) {
		vul__gac_create_astar_node_copy( &neighbors[ i ], graph, root, node, vi, i );
	}
	return ( ui64_t )i;
}
#endif

/*
* The heuristic we use is number of free variables.
* End node is always null, and not used.
*/
#ifndef VUL_DEFINE
f64_t vul_gac_heuristic( vul_astar_node_t *s, vul_astar_node_t *e );
#else
f64_t vul_gac_heuristic( vul_astar_node_t *s, vul_astar_node_t *e )
{
	ui32_t i;
	vul_gac_node_data_t *node;
	vul_csp_variable_instance_t *v;
	f64_t h;

	node = ( ( vul_gac_astar_node_user_data* )s->user_data )->gac_node;

	h = 0;
	for( i = 0; i < vul_svector_size( node->var_insts ); ++i ) {
		v = ( vul_csp_variable_instance_t* )vul_svector_get( node->var_insts, i );
		if( vul_svector_size( v->dom_inst ) > 1 ) { // A variable is free, increase the estimated cost.
			h += 1;
		}
	}

	return h;
}
#endif

/*
* Cost function. Always 1, since we cannot determine if it is valid at this point
* as it is called before we've done the deduction step. If we did the deduction here,
* we would always be performing breath first expasion; not wanted!
*/
#ifndef VUL_DEFINE
f64_t vul_gac_cost_neighbors( vul_astar_node_t *s, vul_astar_node_t *t );
#else
f64_t vul_gac_cost_neighbors( vul_astar_node_t *s, vul_astar_node_t *t )
{
	return 1.0;
}
#endif