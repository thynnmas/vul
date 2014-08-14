/*
 * Villains' Utility Library - Thomas Martin Schmid, 2014. Public domain¹
 *
 * This file describes a doubly linked list.
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


typedef struct vul_list_element_t vul_list_element_t;
struct vul_list_element_t
{
	void *data;
	ui32_t data_size;
	vul_list_element_t *prev;
	vul_list_element_t *next;
};

/**
 * Creates a new vul_list_element_t and adds it after the given element. Copies the given data.
 * Should the given element be NULL, this is equivalent to creating a new list.
 */
#ifndef VUL_DEFINE
vul_list_element_t *vul__list_add_after( vul_list_element_t *e, void *data, ui32_t data_size );
#else
vul_list_element_t *vul__list_add_after( vul_list_element_t *e, void *data, ui32_t data_size )
{
	vul_list_element_t *ret;

	ret = ( vul_list_element_t* )malloc( sizeof( vul_list_element_t ) );
	assert( ret != NULL ); // Make sure malloc didn't fail
	ret->data = malloc( data_size );
	assert( ret->data != NULL ); // Make sure malloc didn't fail
	ret->data_size = data_size;
	memcpy( ret->data, data, data_size );

	if ( e != NULL )
	{
		ret->prev = e;
		if ( e->next != NULL ) {
			vul_list_element_t *n = e->next;
			e->next = ret;
			ret->next = n;
			n->prev = ret;
		} else {
			ret->next = NULL;
			e->next = ret;
		}
	} else {
		ret->prev = NULL;
		ret->next = NULL;
	}

	return ret;
}
#endif

/**
 * Removes the given vul_list_element_t from the list, and deletes it.
 */
#ifndef VUL_DEFINE
void vul_list_remove( vul_list_element_t *e );
#else
void vul_list_remove( vul_list_element_t *e )
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
 * a new element with the given data. If the given data is smaller than the entire list,
 * we return NULL.
 */

#ifndef VUL_DEFINE
vul_list_element_t *vul_list_find( vul_list_element_t *head, void *data, int (*comparator)( void *a, void *b ) );
#else
vul_list_element_t *vul_list_find( vul_list_element_t *head, void *data, int (*comparator)( void *a, void *b ) )
{
	assert( head != NULL );
	
	// If the first element is bigger then what we want the spot we wish to return
	// is before the actual list, so we return null.
	if( comparator( data, head->data ) < 0 ) {
		return NULL;
	}

	while( head->next != NULL // Return last element of the list if data is bigger than all elements, not null
		   && comparator( data, head->next->data ) >= 0 )	// And keep moving while data is bigger than or equal to the next element.
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
vul_list_element_t *vul_list_insert( vul_list_element_t *list_head, void *data, ui32_t data_size, int (*comparator)( void *a, void *b ) );
#else
vul_list_element_t *vul_list_insert( vul_list_element_t *list_head, void *data, ui32_t data_size, int (*comparator)( void *a, void *b ) )
{
	vul_list_element_t *before, *ret;
	
	// Find the smallest element smaller than or equal to it
	if( list_head != NULL ) {
		before = vul_list_find( list_head, data, comparator );

		// Insert the data after it (which if before == NULL means create a new head).
		ret = vul__list_add_after( before, data, data_size );

		// If we created a new head, we must manualy link it up
		if( before == NULL )
		{
			ret->next = list_head;
			list_head->prev = ret;
		}
	} else {
		// Create a new head
		ret = vul__list_add_after( NULL, data, data_size );
	}
	return ret;
}
#endif

/**
 * Returns the length of the given list.
 */
#ifndef VUL_DEFINE
ui32_t vul_list_size( vul_list_element_t *list_head );
#else
ui32_t vul_list_size( vul_list_element_t *list_head )
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
void vul_list_iterate( vul_list_element_t *list_head, void (*func)( vul_list_element_t *e ) );
#else
void vul_list_iterate( vul_list_element_t *list_head, void (*func)( vul_list_element_t *e ) )
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
void vul_list_destroy( vul_list_element_t *list_head );
#else
void vul_list_destroy( vul_list_element_t *list_head )
{
	vul_list_element_t *next;

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
vul_list_element_t *vul_list_copy( vul_list_element_t *list_head );
#else
vul_list_element_t *vul_list_copy( vul_list_element_t *list_head )
{
	vul_list_element_t *nhead, *n;

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