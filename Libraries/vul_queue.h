/*
 * Villains' Utility Library - Thomas Martin Schmid, 2016. Public domain¹
 *
 * This file describes a queue constructed by dynamically allocated and
 * freed arrays linked together in a list. The size of each block, in bytes
 * can be specified by defining VUL_QUEUE_BUFFER_BYTE_SIZE; it is recommended
 * to make this a multiple of the queue's elements' size to not waste memory.
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
#ifndef VUL_QUEUE_H
#define VUL_QUEUE_H

#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "vul_types.h"
#ifndef VUL_LINKED_LIST_H
#include "vul_linked_list.h"
#endif

#ifndef VUL_QUEUE_BUFFER_BYTE_SIZE
	#define VUL_QUEUE_BUFFER_BYTE_SIZE 1024
#endif

typedef struct vul__queue_buffer_t {
	ui8_t data[ VUL_QUEUE_BUFFER_BYTE_SIZE ];
	ui32_t first, next;
	ui32_t buffer_id; // incrementally unique, used to sort in the dumiy
} vul__queue_buffer_t;

/**
 * If defined, the functions are defined and not just declared. Only do this in _one_ c/cpp file!
 */
//#define VUL_DEFINE

typedef struct vul_queue_t
{
	vul_list_element_t *first_root, *next_root; // List of vul__queue_buffer_t objects
	ui32_t data_size;
	ui32_t first;
	ui32_t next;

	/* Memory management functions */
	void *( *allocator )( size_t size );
	void( *deallocator )( void *ptr );
} vul_queue_t;

#endif

/**
 * Comparison function for the linked list used internally.
 */
#ifndef VUL_DEFINE
int vul__queue_comparator( void *a, void *b );
#else
int vul__queue_comparator( void *a, void *b )
{
	vul__queue_buffer_t *ba, *bb;

	ba = ( vul__queue_buffer_t* )a;
	bb = ( vul__queue_buffer_t* )b;

	return ba->buffer_id - bb->buffer_id;
}
#endif

/**
 * Returns the size of the queue.
 */
#ifndef VUL_DEFINE
unsigned int vul_queue_size( vul_queue_t *q );
#else
unsigned int vul_queue_size( vul_queue_t *q )
{
	vul_list_element_t *e;
	vul__queue_buffer_t *buf;
	ui32_t count;

	assert( q );
	if( !q->first_root || !q->next_root ) {
		return 0;
	}
	count = 0;
	e = q->first_root;
	while( e != q->next_root ) {
		buf = ( vul__queue_buffer_t* )e->data;
		count += buf->next - buf->first;
		e = e->next;
	}
	buf = ( vul__queue_buffer_t* )e->data;
	count += buf->next - buf->first;

	return count;
}
#endif

/**
 * Returns true if the queue is empty
 */
#ifndef VUL_DEFINE
bool32_t vul_queue_is_empty( vul_queue_t *q );
#else
bool32_t vul_queue_is_empty( vul_queue_t *q )
{
	vul__queue_buffer_t *bf, *bn;

	assert( q );
	if( !q->first_root || !q->next_root ) {
		return 1;
	}
	if( q->first_root != q->next_root ) {
		return 0;
	}
	bf = ( vul__queue_buffer_t* )q->first_root->data;
	bn = ( vul__queue_buffer_t* )q->next_root->data;

	return bn->next - bf->first == 0;
}
#endif

/**
 * Creates a new queue. Takes the size of an element as an arguemnt.
 * and the factor by which to grow it when needed.
 */
#ifndef VUL_DEFINE
vul_queue_t *vul_queue_create( unsigned int element_size, 
							   void *( *allocator )( size_t size ),
							   void( *deallocator )( void *ptr ) );
#else
vul_queue_t *vul_queue_create( unsigned int element_size,
							   void *( *allocator )( size_t size ),
							   void( *deallocator )( void *ptr ) )
{
	vul_queue_t *ret;

	ret = ( vul_queue_t* )allocator( sizeof( vul_queue_t ) );
	ret->data_size = element_size;
	ret->first = 0;
	ret->next = 0;
	ret->first_root = NULL;
	ret->next_root = NULL;

	ret->allocator = allocator;
	ret->deallocator = deallocator;

	return ret;
}
#endif

/**
 * Pushes an element to the back of the queue. If the queue is full, this resizes it.
 */
#ifndef VUL_DEFINE
void vul_queue_push( vul_queue_t *q, void *data );
#else
void vul_queue_push( vul_queue_t *q, void *data )
{
	vul__queue_buffer_t *buf, nbuf;

	assert( q );
	// If no next buffer, create it
	nbuf.next = 0;
	nbuf.first = 0;
	if( !q->next_root ) {
		// Create the new next_root
		nbuf.buffer_id = 0;
		q->next_root = vul_list_insert( NULL, 
										&nbuf, 
										sizeof( vul__queue_buffer_t ), 
										vul__queue_comparator,
										q->allocator );
		// If first root is null, it should be this
		if( !q->first_root ) {
			q->first_root = q->next_root;
		}
	}
	// If no more room, create a new buffer
	buf = ( vul__queue_buffer_t* )q->next_root->data;
	if( buf->next >= ( VUL_QUEUE_BUFFER_BYTE_SIZE / q->data_size ) - q->data_size )
	{
		nbuf.buffer_id = buf->buffer_id + 1;
		q->next_root = vul_list_insert( q->next_root, 
										&nbuf, 
										sizeof( vul__queue_buffer_t ), 
										vul__queue_comparator,
										q->allocator );
		buf = ( vul__queue_buffer_t* )q->next_root->data;
	}
	// Insert it
	memcpy( buf->data + ( buf->next++ * q->data_size ), data, q->data_size );
}
#endif

/**
 * Pops an element from the front of the queue and returns it in
 * out.
 */
#ifndef VUL_DEFINE
void vul_queue_pop( vul_queue_t *q, void *out );
#else
void vul_queue_pop( vul_queue_t *q, void *out )
{
	vul__queue_buffer_t *buf;

	assert( q );
	if( q->first_root ) {
		buf = ( vul__queue_buffer_t* )q->first_root->data;
		memcpy( out, buf->data + ( buf->first * q->data_size ), q->data_size );
		++buf->first;
		if( buf->first >= ( VUL_QUEUE_BUFFER_BYTE_SIZE / q->data_size ) - q->data_size ) {
			// Buffer is empty, free it
			if( q->first_root->next != NULL ) {
				q->first_root = q->first_root->next;
				q->deallocator( q->first_root->prev->data );
				q->deallocator( q->first_root->prev );
				q->first_root->prev = NULL;
			} else {
				q->deallocator( q->first_root->data );
				q->deallocator( q->first_root );
				q->first_root = NULL;
			}
		}
	}
}
#endif

/**
 * Peeks at the front element of the queue.
 * @NOTE: This does not copy the data back, meaning it is only
 * guaranteed to be unaltered as long as the queue is not modified.
 * If you want the data to persisit beyond the next q->push you must copy
 * the data youself.
 */
#ifndef VUL_DEFINE
void *vul_queue_peek( vul_queue_t *q );
#else
void *vul_queue_peek( vul_queue_t *q )
{
	vul__queue_buffer_t *buf;

	assert( q );
	if( q->first_root ) {
		buf = ( vul__queue_buffer_t* )q->first_root->data;
		return ( buf->data + ( buf->first * q->data_size ) );
	}
	return NULL;
}
#endif

/**
 * Destroys the given queue and deallocates it's memory
 */
#ifndef VUL_DEFINE
void vul_queue_destroy( vul_queue_t *q );
#else
void vul_queue_destroy( vul_queue_t *q )
{
	if( q ) {
		if( q->first_root ) {
			vul_list_destroy( q->first_root, q->deallocator );
		}
		q->deallocator( q );
	}
}
#endif