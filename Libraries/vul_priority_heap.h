/*
* Villains' Utility Library - Thomas Martin Schmid, 2015. Public domain¹
*
* This file describes a generic priority queue. The current implementation
* is a dumb-as-all-hell linked list implementation, but
* @TODO(thynn): Implement a Heap or Fibonacci Heap version of this
* should increase the performance substantially!
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
#ifndef VUL_PRIORITY_HEAP_H
#define VUL_PRIORITY_HEAP_H

#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "vul_types.h"
#include "vul_linked_list.h"

/**
* If defined, the functions are defined and not just declared. Only do this in _one_ c/cpp file!
*/
//#define VUL_DEFINE

/*
* Defines a linked list version of a priority heap.
*/
typedef struct vul_priority_heap_t {
	vul_list_element_t *root;
	ui32_t data_size;
	int( *comparator )( void *a, void *b );
	void *( *allocator )( size_t size );
	void( *deallocator )( void *ptr );
} vul_priority_heap_t;

#endif

/**
 * Create a new priority heap.
 * Takes a comparison function for elements and
 * memory management functions.
 */
#ifndef VUL_DEFINE
vul_priority_heap_t *vul_priority_heap_create( ui32_t data_size, 
												 int( *comparison_func )( void* a, void* b ),
												 void *( *allocator )( size_t size ),
												 void( *deallocator )( void *ptr ) );
#else
vul_priority_heap_t *vul_priority_heap_create( ui32_t data_size,
											   int( *comparison_func )( void* a, void* b ),
											   void *( *allocator )( size_t size ),
											   void( *deallocator )( void *ptr ) )
{
	vul_priority_heap_t *q;

	q = ( vul_priority_heap_t* )allocator( sizeof( vul_priority_heap_t ) );
	assert( q );
	q->data_size = data_size;
	q->comparator = comparison_func;
	q->allocator = allocator;
	q->deallocator = deallocator;
	q->root = NULL;

	return q;
}
#endif

/**
 * Destroys a priority heap
 */
#ifndef VUL_DEFINE
void vul_priority_heap_destroy( vul_priority_heap_t *q );
#else
void vul_priority_heap_destroy( vul_priority_heap_t *q )
{
	assert( q );
	if( q->root ) {
		vul_list_destroy( q->root, q->deallocator );
	}
	free( q );
	}
#endif

/**
 * Pushes an element into the heap.
 */
#ifndef VUL_DEFINE
void vul_priority_heap_push( vul_priority_heap_t *q, void *data );
#else
void vul_priority_heap_push( vul_priority_heap_t *q, void *data )
{
	vul_list_element_t *e;

	e = vul_list_insert( q->root, data, q->data_size, q->comparator, q->allocator );
	if( e->prev == NULL ) {
		q->root = e;
}
}
#endif

/**
 * Pops the next element out of the heap and copies it to data_out
 */
#ifndef VUL_DEFINE
void vul_priority_heap_pop( vul_priority_heap_t *q, void *data_out );
#else
void vul_priority_heap_pop( vul_priority_heap_t *q, void *data_out )
{
	vul_list_element_t *e;

	e = NULL;
	if( q->root ) {
		e = q->root->next;
		// Copy the data to the outbuffer
		memcpy( data_out, q->root->data, q->data_size );
}
	// Then remove the element
	vul_list_remove( q->root, q->deallocator );

	q->root = e;
}
#endif

/**
 * Peeks at the next element of the heap
 */
#ifndef VUL_DEFINE
void *vul_priority_heap_peek( vul_priority_heap_t *q );
#else
void *vul_priority_heap_peek( vul_priority_heap_t *q )
{
	if( q->root ) {
		return q->root->data;
	}
	return NULL;
	}
#endif

/**
 * Returns true if the heap is empty 
 */
#ifndef VUL_DEFINE
bool32_t vul_priority_heap_is_empty( vul_priority_heap_t *q );
#else
bool32_t vul_priority_heap_is_empty( vul_priority_heap_t *q )
{
	return q->root ? 0 : 1;
}
#endif

/**
 * Returns the number of elements in the heap.
 */
#ifndef VUL_DEFINE
ui32_t vul_priority_heap_size( vul_priority_heap_t *q );
#else
ui32_t vul_priority_heap_size( vul_priority_heap_t *q )
{
	return vul_list_size( q->root );
}
#endif