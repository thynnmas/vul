/*
 * Villains' Utility Library - Thomas Martin Schmid, 2014. Public domain¹
 *
 * This file describes a ring-buffer implementation of a queue.
 * 
 * ¹ If public domain is not legally valid in your country and or legal area,
 *   the MIT licence applies (see the LICENCE file)
 */
#ifndef VUL_QUEUE_H
#define VUL_QUEUE_H

#include <malloc.h>
#include <assert.h>
#include <string.h>

#include "vul_types.h"

/**
 * If defined, the functions are defined and not just declared. Only do this in _one_ c/cpp file!
 */
//#define VUL_DEFINE

typedef struct
{
	void *data;
	unsigned int element_size;
	unsigned int reserved;
	float growth_factor;
	unsigned int front;
	unsigned int back; // This is actually on the slot _after_ the back.
} vul_queue;

/**
 * Returns the size of the queue.
 */
#ifndef VUL_DEFINE
unsigned int vul_queue_size( vul_queue *q );
#else
unsigned int vul_queue_size( vul_queue *q )
{
	if( q->back >= q->front )
		return ( q->back - q->front ) - 1;
	return q->reserved - ( ( q->front - q->back ) + 1 );
}
#endif

/**
 * Creates a new queue. Takes the size of an element as an arguemnt.
 * Optional arguments include the initial size of the ring buffer,
 * and the factor by which to grow it when needed.
 */
#ifndef VUL_DEFINE
vul_queue *vul_queue_create( unsigned int element_size, unsigned int initial_size = 8, float growth_factor = 1.5f );
#else
vul_queue *vul_queue_create( unsigned int element_size, unsigned int initial_size = 8, float growth_factor = 1.5f )
{
	vul_queue *q = ( vul_queue* )malloc( sizeof( vul_queue ) );
	assert( q != NULL ); // Make sure malloc didn't fail
	q->element_size = element_size;
	q->front = 0;
	q->back = 1;
	q->reserved = initial_size;
	q->data = ( void** )malloc( element_size * initial_size );
	assert( q->data != NULL ); // Make sure malloc didn't fail
	q->growth_factor = growth_factor;

	return q;
}
#endif

/**
 * Helper function used to resize the queue whenever needed.
 * Takes a pointer to the old queue and the new size in number of elements.
 * Due to the resize and the nature of ring buffers, this allocates a new list
 * and copies to it, instead of using realloc. It frees the old list.
 */
#ifndef VUL_DEFINE
void vul__queue_resize( vul_queue *q, unsigned int new_size );
#else
void vul__queue_resize( vul_queue *q, unsigned int new_size )
{
	void **d;
	int back = vul_queue_size( q );

	if( q->reserved == new_size ) return;
	if( new_size == 0 )
	{
		if( q->data != NULL ) {
			free( q->data );
			q->data = NULL;
		}
		return;
	}

	d = ( void** )malloc( q->element_size * new_size );
	assert( d != NULL ); // Make sure malloc didn't fail
	if( q->back >= q->front )
	{
		memcpy( d, 
			&q[ q->front * q->element_size ],
			( q->back - q->front ) * q->element_size );
	} else {
		memcpy( d, 
				&q[ q->front * q->element_size ], 
				( q->reserved - q->front ) * q->element_size );
		memcpy( &d[ ( q->reserved - q->front ) * q->element_size ], 
				q, 
				q->back * q->element_size );
	}
	if( q->data != NULL ) {
		free( q->data );
	}
	q->data = d;
	q->reserved = new_size;
	q->front = 0;
	q->back = back;
}
#endif

/**
 * Pushes an element to the back of the queue. If the queue is full, this resizes it.
 */
#ifndef VUL_DEFINE
void vul_queue_push( vul_queue *q, void *data );
#else
void vul_queue_push( vul_queue *q, void *data )
{
	// Check if full; if so, resize
	if( q->front == q->back )
	{
		vul__queue_resize( q, ( unsigned int )( ( float )q->reserved * q->growth_factor ) );
	}
	// Insert
	void *dst = ( void* )( ( unsigned int )q->data + ( q->back * q->element_size ) );
	memcpy( dst, data, q->element_size );
	// Wrap
	if( ++q->back == q->reserved ) {
		q->back = 0;
	}
}
#endif

/**
 * Pops an element from the front of the queue.
 * @NOTE: This does not copy the data back, meaning it is only
 * guaranteed to be unaltered as long as the queue is not modified.
 * If you want the data to persisit beyond the next q->push you must copy
 * the data youself.
 */
#ifndef VUL_DEFINE
void *vul_queue_pop( vul_queue *q );
#else
void *vul_queue_pop( vul_queue *q )
{
	void *ret;
	
	// Get element.
	ret = ( void* )( ( unsigned int )q->data + ( q->front * q->element_size ) );
	// Wrap
	if( ++q->front == q->reserved ) {
		q->front = 0;
	}
	// Return
	return ret;
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
void *vul_queue_peek( vul_queue *q );
#else
void *vul_queue_peek( vul_queue *q )
{
	void *ret;
	
	// Get element.
	return ( void* )( ( unsigned int )q->data + ( q->front * q->element_size ) );
}
#endif

/**
 * Resizes the given queue to the exact size it currently is.
 */
#ifndef VUL_DEFINE
void vul_queue_tighten( vul_queue *q );
#else
void vul_queue_tighten( vul_queue *q )
{
	vul__queue_resize( q, vul_queue_size( q ) );
}
#endif

/**
 * Creates a copy of the given queue. The new queue will be tightly allocated.
 */
#ifndef VUL_DEFINE
vul_queue *vul_queue_copy( vul_queue *src );
#else
vul_queue *vul_queue_copy( vul_queue *src )
{
	vul_queue *dst;

	dst = ( vul_queue* )malloc( sizeof( vul_queue ) );
	assert( dst != NULL ); // Make sure malloc didn't fail

	dst->element_size = src->element_size;
	dst->growth_factor = src->growth_factor;
	dst->back = vul_queue_size( src );
	dst->front = 0;
	dst->reserved = dst->back + 1;
	dst->data = ( void* )malloc( dst->reserved * dst->element_size );
	assert( dst->data != NULL ); // Make sure malloc didn't fail
	if( src->back >= src->front ) {
		memcpy( dst->data,
				( void* )( ( unsigned int )src->data + ( src->front * src->element_size ) ),
				( src->back - src->front ) * src->element_size );
	} else {
		memcpy( dst->data,
				( void* )( ( unsigned int )src->data + ( src->front * src->element_size ) ),
				( src->reserved - src->front ) * src->element_size );
		memcpy( ( void* )( ( unsigned int )dst->data + ( ( src->reserved - src->front ) * src->element_size ) ),
				src->data,
				src->back * src->element_size );
	}
	
	return dst;
}
#endif

/**
 * Destroys the given queue and frees it's memory
 */
#ifndef VUL_DEFINE
void vul_queue_destroy( vul_queue *q );
#else
void vul_queue_destroy( vul_queue *q )
{
	free( q->data );
	free( q );
	// By setting to null we are much more likely to trigger asserts if used after free.
	q->data = NULL;
	q = NULL;
}
#endif

#endif