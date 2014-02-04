/*
 * Villains' Utility Library - Thomas Martin Schmid, 2014. Public domain�
 *
 * This file describes a doubly linked list.
 * 
 * � If public domain is not legally valid in your country and or legal area,
 *   the MIT licence applies (see the LICENCE file)
 */
#ifndef VUL_LINKED_LIST_H
#define VUL_LINKED_LIST_H

#include <stdlib.h>
#include <malloc.h>
#include <assert.h>
#include <string.h>

#include "vul_types.h"

/**
 * If defined, the functions are defined and not just declared. Only do this in _one_ c/cpp file!
 */
//#define VUL_DEFINE


typedef struct vul_list_element vul_list_element;
struct vul_list_element
{
	void *data;
	ui32_t data_size;
	vul_list_element *prev;
	vul_list_element *next;
};

/**
 * Creates a new vul_list_element and adds it after the given element. Copies the given data.
 * Should the given element be NULL, this is equivalent to creating a new list.
 */
#ifndef VUL_DEFINE
vul_list_element *vul__list_add_after( vul_list_element *e, void *data, ui32_t data_size );
#else
vul_list_element *vul__list_add_after( vul_list_element *e, void *data, ui32_t data_size )
{
	vul_list_element *ret, n;

	ret = ( vul_list_element* )malloc( sizeof( vul_list_element ) );
	assert( ret != NULL ); // Make sure malloc didn't fail
	ret->data = malloc( data_size);
	assert( ret->data != NULL ); // Make sure malloc didn't fail
	ret->data_size = data_size;
	memcpy( ret->data, data, data_size );

	if ( e != NULL )
	{
		ret->prev = e;
		if ( e->next != NULL ) {
			vul_list_element *n = e->next;
			e->next = ret;
			ret->next = n;
			n->prev = ret;
		}
	}

	return ret;
}
#endif

/**
 * Removes the given vul_list_element from the list, and deletes it.
 */
#ifndef VUL_DEFINE
void vul_list_remove( vul_list_element *e );
#else
void vul_list_remove( vul_list_element *e )
{
	assert( e != NULL );

	if ( e->prev != NULL )
	{
		e->prev->next = e->next;
	}
	if ( e->next != NULL )
	{
		e->next->prev = e->prev;
	}
	free( e->data );
	free( e );
	// By setting to null we are much more likely to trigger asserts if used after free.
	e->data = NULL;
	e = NULL;
}
#endif

/**
 * Finds the last element in the list that is equal to the given data, or if not available, 
 * the last one that is smaller. This is the element after which we would want to insert
 * a new element with the given data.
 */

#ifndef VUL_DEFINE
vul_list_element *vul_list_find( vul_list_element *head, void *data, int (*comparator)( void *a, void *b ) );
#else
vul_list_element *vul_list_find( vul_list_element *head, void *data, int (*comparator)( void *a, void *b ) )
{
	assert( head != NULL );

	while( head->next != NULL // Return last element of the list if data is bigger than all elements, not null
		   && comparator( data, head->data ) <= 0 )	// And keep moving while data is smaller or equal.
	{
		head = head->next;
	}
	return head;
}
#endif

/** 
 * Inserts the given data into the list while keeping the list sorted and stable.
 * If list_head is NULL this creates a new list!
 */
#ifndef VUL_DEFINE
vul_list_element *vul_list_insert( vul_list_element *list_head, void *data, ui32_t data_size, int (*comparator)( void *a, void *b ) );
#else
vul_list_element *vul_list_insert( vul_list_element *list_head, void *data, ui32_t data_size, int (*comparator)( void *a, void *b ) )
{
	vul_list_element *ret, *before;
	
	// Find the smallest element smaller than or equal to it
	before = vul_list_find( list_head, data, comparator );

	// Insert the data after it
	return vul__list_add_after( before, data, data_size );
}
#endif

/**
 * Returns the length of the given list.
 */
#ifndef VUL_DEFINE
ui32_t vul_list_size( vul_list_element *list_head );
#else
ui32_t vul_list_size( vul_list_element *list_head )
{
	int c;

	c = 0;
	while( list_head != NULL )
	{
		++c;
		list_head = list_head->next;
	}

	return c;
}
#endif

/**
 * General purpose iteration over a list.
 * Executes the given function for each element in the list.
 * @NOTE: If func alters the list, behaviour is undefined!
 */
#ifndef VUL_DEFINE
void vul_list_iterate( vul_list_element *list_head, void (*func)( vul_list_element *e ) );
#else
void vul_list_iterate( vul_list_element *list_head, void (*func)( vul_list_element *e ) )
{
	while( list_head != NULL ) {
		func( list_head );
		list_head = list_head->next;
	}
}
#endif

/**
 * Deletes a the given list and all elements in it.
 */
#ifndef VUL_DEFINE
void vul_list_destroy( vul_list_element *list_head );
#else
void vul_list_destroy( vul_list_element *list_head )
{
	vul_list_element *next;

	while( list_head != NULL )
	{
		next = list_head->next;

		free( list_head->data );
		free( list_head );
		// By setting to null we are much more likely to trigger asserts if used after free.
		list_head->data = NULL;
		list_head = NULL;
		
		list_head = next;
	}
}
#endif

/**
 * Creates a copy of the given list.
 */
#ifndef VUL_DEFINE
vul_list_element *vul_list_copy( vul_list_element *list_head );
#else
vul_list_element *vul_list_copy( vul_list_element *list_head )
{
	vul_list_element *nhead, *n;

	nhead = vul__list_add_after( NULL, list_head->data, list_head->data_size );
	n = nhead;
	while( list_head->next )
	{
		list_head = list_head->next;
		n = vul__list_add_after( n, list_head->data, list_head->data_size );
	}

	return nhead;
}
#endif

#endif