/*
 * Villains' Utility Library - Thomas Martin Schmid, 2015. Public domain¹
 *
 * This file describes a singly linked skip list. The current implementation is
 * not indexable.
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
#ifndef VUL_SKIP_LIST_H
#define VUL_SKIP_LIST_H

#include <malloc.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "vul_types.h"

/**
 * If defined, the functions are defined and not just declared. Only do this in _one_ c/cpp file!
 */
//#define VUL_DEFINE


typedef struct vul_skip_list_element
{
	void *data;
	unsigned int levels;
	vul_skip_list_element** nexts;
} vul_skip_list_element;

typedef struct vul_skip_list
{
	unsigned int levels;
	vul_skip_list_element **heads;
	int (*comparator)( void *a, void *b );
	unsigned int data_size;
} vul_skip_list;

/**
 * Simulates a coin flip; .5 probability of 0, same for 1.
 */
#ifndef VUL_DEFINE
int vul__skip_list_coin_flip( );
#else
int vul__skip_list_coin_flip( )
{
	return ( rand() / RAND_MAX ) > 0.5;
	// @TODO: Uniform distribution you said? Halton series!
}
#endif

/**
 * Creates a new skip list. Takes the size of an element and a comparison function
 * as arguments and returns an empty list.
 */
#ifndef VUL_DEFINE
vul_skip_list *vul_skip_list_create(unsigned int data_size, int (*comparator)( void *a, void *b ) );
#else
vul_skip_list *vul_skip_list_create(unsigned int data_size, int (*comparator)( void *a, void *b ) )
{
	vul_skip_list *ret;
	int l;

	ret = ( vul_skip_list* )malloc( sizeof( vul_skip_list ) );
	assert( ret != NULL ); // Make sure malloc didn't fail
	ret->levels = 1;
	ret->heads = ( vul_skip_list_element** )malloc( sizeof( vul_skip_list_element* ) );
	assert( ret->heads != NULL ); // Make sure malloc didn't fail
	ret->comparator = comparator;
	ret->data_size = data_size;

	for( l = ret->levels - 1; l >= 0; --l )
	{
		ret->heads[ l ] = NULL;
	}

	return ret;
}
#endif

/**
 * Finds the element in the list which matches the given data key. If
 * no match is found, returns NULL.
 */
#ifndef VUL_DEFINE
vul_skip_list_element *vul_skip_list_find( vul_skip_list *list, void *data );
#else
vul_skip_list_element *vul_skip_list_find( vul_skip_list *list, void *data )
{
	int l;
	vul_skip_list_element *e = NULL;

	if ( list == NULL || list->heads == NULL ) return NULL;
	
	for( l = list->levels - 1; l >= 0; --l )
	{
		e = list->heads[ l ];
		while( list->comparator( e->data, data ) < 0 )
		{
			e = e->nexts[ l ];
		}
	}

	if ( e && list->comparator( e->data, data ) == 0 ) return e;
	return NULL;
}
#endif

/**
 * Removes the given element from the list, and deletes it.
 */
#ifndef VUL_DEFINE
void vul_skip_list_remove( vul_skip_list *list, vul_skip_list_element *e );
#else
void vul_skip_list_remove( vul_skip_list *list, vul_skip_list_element *e )
{
	int l;
	vul_skip_list_element *p, *h;

	assert( e != NULL );
	
	// For every level
	for ( l = list->levels - 1; l >= 0; --l )
	{
		p = NULL;
		h = list->heads[ l ];
		// See if e exists on this level
		while( h != NULL && h != e ) {
			p = h;
			h = h->nexts[ l ];
		}
		// And if it does, remove it's link
		if( h == e ) {
			p->nexts[ l ] = e->nexts[ l ];
		}
	}

	free( e->data );
	free( e->nexts );
	free( e );
	// By setting to null we are much more likely to trigger asserts if used after free.
	e->data = NULL;
	e->nexts = NULL;
	e = NULL;
}
#endif

/**
 * Inserts the given data into the list, while keeping it sorted and stable.
 * Copies the given data (of given size) to the element.
 * Returns a pointer to the new element.
 */
#ifndef VUL_DEFINE
vul_skip_list_element *vul_skip_list_insert( vul_skip_list *list, void *data );
#else
vul_skip_list_element *vul_skip_list_insert( vul_skip_list *list, void *data )
{
	int l;
	vul_skip_list_element **el, *e, *ret;
	
	if ( list == NULL || list->heads == NULL ) return NULL;

	el = ( vul_skip_list_element** )malloc( list->levels * sizeof( vul_skip_list_element* ) );
	assert( el != NULL ); // Make sure malloc didn't fail
	
	for( l = list->levels - 1; l >= 0; --l )
	{
		e = list->heads[ l ];
		while( list->comparator( e->data, data ) < 0 )
		{
			if( e->nexts[ l ] == NULL ) {
				break;
			}
			e = e->nexts[ l ];
		}
		el[ l ] = e; // Store the last elements in each level
	}

	// Create our element
	ret = ( vul_skip_list_element* )malloc( sizeof( vul_skip_list_element ) );
	assert( ret != NULL ); // Make sure malloc didn't fail
	ret->data = malloc( list->data_size );
	assert( ret->data != NULL ); // Make sure malloc didn't fail
	memcpy( ret->data, data, list->data_size );
	ret->levels = 1;
	while( vul__skip_list_coin_flip( ) ) ++ret->levels;
	ret->nexts = ( vul_skip_list_element** )malloc( ret->levels * sizeof( vul_skip_list_element* ) );
	assert( ret->nexts != NULL ); // Make sure malloc didn't fail
	/* @TODO: THIS IS NOT WORKING YET! */ assert(false && "Skip lists aren't correctly implemented here.");
	// Insert it at bottom level, and each above if coinflip is false
	for ( l = ret->levels - 1; l >= 0; --l )
	{
		ret->nexts[ l ] = el[ l ]->nexts[ l ];
	}
	return NULL; // @NOTE: never reached due to the above todo: we assert false!
}
#endif

/**
 * Returns the length of the given skip list.
 */
#ifndef VUL_DEFINE
unsigned int vul_skip_list_size( vul_skip_list *list );
#else
unsigned int vul_skip_list_size( vul_skip_list *list )
{
	int c;
	vul_skip_list_element *e;
	
	// Count elements in the lowest list.
	e = list->heads[ 0 ];
	c = 0;
	while( e != NULL )
	{
		++c;
		e = e->nexts[ 0 ];
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
void vul_skip_list_iterate( vul_skip_list *list, void (*func)( vul_skip_list_element *e ) );
#else
void vul_skip_list_iterate( vul_skip_list *list, void (*func)( vul_skip_list_element *e ) )
{
	vul_skip_list_element *e;

	e = list->heads[ 0 ];
	while( e != NULL ) {
		func( e );
		e = e->nexts[ 0 ];
	}
}
#endif

/**
 * Deletes a the given list and all elements in it.
 */
#ifndef VUL_DEFINE
void vul_skip_list_destroy( vul_skip_list *list );
#else
void vul_skip_list_destroy( vul_skip_list *list )
{
	vul_skip_list_element *e, *n;
	
	// Destroy all elements from the list at level 0
	e = list->heads[ 0 ];
	while( e != NULL )
	{
		n = e->nexts[ 0 ];

		free( e->data );
		free( e->nexts );
		free( e );
		// By setting to null we are much more likely to trigger asserts if used after free.
		e->data = NULL;
		e->nexts = NULL;
		e = NULL;
		
		e = n;
	}

	// Then free the other heads
	free( list->heads );
	// And finally, the list
	free( list );
	
	// By setting to null we are much more likely to trigger asserts if used after free.
	list->heads = NULL;
	list = NULL;
}
#endif

/**
 * Creates a copy of the given skip list. It does NOT maintain the same lanes,
 * meaning it copies the data and maintains the sorting invariant, but it may look
 * differently internally, and thus may perform slightly different.
 */
#ifndef VUL_DEFINE
vul_skip_list *vul_skip_list_copy( vul_skip_list *src );
#else
vul_skip_list *vul_skip_list_copy( vul_skip_list *src )
{
	vul_skip_list *dst;
	vul_skip_list_element *e;

	dst = vul_skip_list_create( src->data_size, src->comparator );

	e = src->heads[ 0 ];
	while( e != NULL )
	{
		vul_skip_list_insert( dst, e->data );
		e = e->nexts[ 0 ];
	}

	return dst;
}
#endif

#endif