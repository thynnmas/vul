/*
 * Villains' Utility Library - Thomas Martin Schmid, 2016. Public domain¹
 *
 * This file contains tests for vul_priority_heap.h
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
#ifndef VUL_TEST_PRIORITY_HEAP_H
#define VUL_TEST_PRIORITY_HEAP_H

/* This is a reference implementation for testing! */
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#ifndef VUL_LINKED_LIST_H
#include "../vul_linked_list.h"
#endif

/*
* Defines a linked list version of a priority heap.
*/
typedef struct vul_priority_heap_reference_t {
	vul_list_element_t *root;
	ui32_t data_size;
	int( *comparator )( void *a, void *b );
	void *( *allocator )( size_t size );
	void( *deallocator )( void *ptr );
} vul_priority_heap_reference_t;

#endif

/**
 * Create a new priority heap.
 * Takes a comparison function for elements and
 * memory management functions.
 */
#ifndef VUL_DEFINE
vul_priority_heap_reference_t *vul_priority_heap_reference_create( ui32_t data_size, 
												 int( *comparison_func )( void* a, void* b ),
												 void *( *allocator )( size_t size ),
												 void( *deallocator )( void *ptr ) );
#else
vul_priority_heap_reference_t *vul_priority_heap_reference_create( ui32_t data_size,
											   int( *comparison_func )( void* a, void* b ),
											   void *( *allocator )( size_t size ),
											   void( *deallocator )( void *ptr ) )
{
	vul_priority_heap_reference_t *q;

	q = ( vul_priority_heap_reference_t* )allocator( sizeof( vul_priority_heap_reference_t ) );
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
void vul_priority_heap_reference_destroy( vul_priority_heap_reference_t *q );
#else
void vul_priority_heap_reference_destroy( vul_priority_heap_reference_t *q )
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
void vul_priority_heap_reference_push( vul_priority_heap_reference_t *q, void *data );
#else
void vul_priority_heap_reference_push( vul_priority_heap_reference_t *q, void *data )
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
void vul_priority_heap_reference_pop( vul_priority_heap_reference_t *q, void *data_out );
#else
void vul_priority_heap_reference_pop( vul_priority_heap_reference_t *q, void *data_out )
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
void *vul_priority_heap_reference_peek( vul_priority_heap_reference_t *q );
#else
void *vul_priority_heap_reference_peek( vul_priority_heap_reference_t *q )
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
bool32_t vul_priority_heap_reference_is_empty( vul_priority_heap_reference_t *q );
#else
bool32_t vul_priority_heap_reference_is_empty( vul_priority_heap_reference_t *q )
{
	return q->root ? 0 : 1;
}
#endif

/**
 * Returns the number of elements in the heap.
 */	
#ifndef VUL_DEFINE
ui32_t vul_priority_heap_reference_size( vul_priority_heap_reference_t *q );
#else
ui32_t vul_priority_heap_reference_size( vul_priority_heap_reference_t *q )
{
	return vul_list_size( q->root );
}
#endif

//----------------------
// The actual tests
//

#include "../vul_priority_heap.h"

int vul_test_priority_heap_compare_floats( void *a, void *b )
{
	float *fa, *fb, r;

	fa = ( float* )a;
	fb = ( float* )b;
	r = *fa - *fb;

	return ( r < 0.f ) ? -1 : ( r > 0.f ) ? 1 : 0;
}

void vul_test_priority_heap( )
{
	vul_priority_heap_t *heap;
	vul_priority_heap_reference_t *reference;

	heap = vul_priority_heap_create( sizeof( float ), vul_test_priority_heap_compare_floats, malloc, free );
	reference = vul_priority_heap_reference_create( sizeof( float ), vul_test_priority_heap_compare_floats, malloc, free );
	
	/* Insert a bunch of things */
	float minimum = 1.f; // We're generating normalized floats, so 1 is max!
	for( ui32_t i = 0; i < 1000; ++i ) {
		float f = ( float )rand( ) / ( float )RAND_MAX;
		minimum = f < minimum ? f : minimum;
		vul_priority_heap_push( heap, &f );
		vul_priority_heap_reference_push( reference, &f );
	}
	/* Check that at the end, minimum is correct */
	assert( *( ( float* )vul_priority_heap_reference_peek( reference ) ) == minimum );
	assert( *( ( float* )vul_priority_heap_peek( heap ) ) == minimum );

	/* Check that sizes are correct */
	assert( vul_priority_heap_size( heap ) == 1000 );
	assert( vul_priority_heap_reference_size( reference ) == 1000 );

	/* Check that they are not empty... */
	assert( !vul_priority_heap_is_empty( heap ) );
	assert( !vul_priority_heap_reference_is_empty( reference ) );

	/* Check that we agree on the rest */
	for( ui32_t i = 0; i < 1000; ++i ) {
		float a, b;
		vul_priority_heap_pop( heap, &a );
		vul_priority_heap_reference_pop( reference, &b );
		assert( a == b ); // We can use equality, since the data should be exactly the same!
	}

	/* Check they are now empty */
	assert( vul_priority_heap_is_empty( heap ) );
	assert( vul_priority_heap_reference_is_empty( reference ) );

	/* Delete them to check that that works... */
	vul_priority_heap_destroy( heap );
	vul_priority_heap_reference_destroy( reference );
}
