/*
 * Villains' Utility Library - Thomas Martin Schmid, 2014. Public domain¹
 *
 * This file describes an alternative to the STL vector class. This is mostly 
 * based on Tom Forsyth's ArbitraryList found here: https://home.comcast.net/~tom_forsyth/blog.wiki.html
 * This version is not templated (so pure C). Sorting of this list is available in vul_sort.h
 * 
 * ¹ If public domain is not legally valid in your country and or legal area,
 *   the MIT licence applies (see the LICENCE file)
 */
#ifndef VUL_RESIZEABLE_ARRAY_H
#define VUL_RESIZEABLE_ARRAY_H

#include <stdlib.h>
#include <malloc.h>
#include <assert.h>
#include <string.h>

#include "vul_types.h"

/**
 * If defined, the functions are defined and not just declared. Only do this in _one_ c/cpp file!
 */
//#define VUL_DEFINE

/**
 * If enabled, iterators look for undefined/illegal behaviour during loops.
 * Slower, but might help track down ugly bugs!
 */
//#define VUL_DEBUG

/**
* An alternative to the STL vector class. This is mostly based on Tom Forsyth's ArbitraryList
* found here: https://home.comcast.net/~tom_forsyth/blog.wiki.html
* It adds a version without templates for a potential compile time decrease at the cost of readability,
* a sorting function and a few macros to help portability between templated and non-templated versions (<-@TODO).
*/
typedef struct {
		
	/**
	* The reserved size of the list
	*/
	unsigned int reserved_size;
	/**
	* The current actual size of the list
	*/
	unsigned int size;
	/**
	* The list itself
	*/
	unsigned char *list;

	/**
	* If we are not using templates, we need the size of an element for easier list handling.
	*/
	unsigned int element_size;

} vul_vector_t;

/**
* Returns the pointer to the first element of the list->
* If templates are used, this is typed, if not it is a void*.
*/
#ifndef VUL_DEFINE
void *vul_vector_begin( vul_vector_t *vec );
#else
void *vul_vector_begin( vul_vector_t *vec )
{
	assert( vec != NULL );
	return vec->list;
}
#endif
/**
* Returns the pointer to the last element of the list plus 1.
* This is the equivalent of the STL vector.end().
* If templates are used, this is typed, if not it is a void*.
*/
#ifndef VUL_DEFINE
void *vul_vector_end( vul_vector_t *vec );
#else
void *vul_vector_end( vul_vector_t *vec )
{
	assert( vec != NULL );
	return ( void* )( &vec->list[ vec->element_size * vec->size ] );
}
#endif
		
/**
* Returns the element size of the type kept. Only for non-templated vectors.
*/
#ifndef VUL_DEFINE
const unsigned int vul_vector_size( const vul_vector_t *vec );
#else
const unsigned int vul_vector_size( const vul_vector_t *vec ) 
{
	assert( vec != NULL );
	return vec->size;
}
#endif
/**
* Returns the pointer to the item at the given location.
* If templates are used, this is typed, if not it is a void*.
* @param index Index of the element to get.
*/	
#ifndef VUL_DEFINE	
void *vul_vector_get( vul_vector_t *vec, const unsigned int index );
#else
void *vul_vector_get( vul_vector_t *vec, const unsigned int index )
{
	assert( vec != NULL );
	assert( index < vec->size );
	return ( void* )( &vec->list[ vec->element_size * index ] );
}
#endif
/**
* Returns a const pointer to the item at the given location.
* If templates are used, this is typed, if not it is a void*.
* @param index
*/	
#ifndef VUL_DEFINE	
const void *vul_vector_get_const( vul_vector_t *vec, const unsigned int index );
#else
const void *vul_vector_get_const( vul_vector_t *vec, const unsigned int index )
{
	assert( vec != NULL );
	assert( index < vec->size );
	return ( void* )( &vec->list[ vec->element_size * index ] );
}
#endif

/**
* Resizes the list to the given number of items. Preserves items already
* in the list up to the new size, others are lost.
* @param size Size of the new list
* @param freeZero [Optional] Whether the memory should be freed if new size is 0.
* @param allocExactly [Optional] Whether reserved size and size should be equal.
* @return a pointer to the first element of the new list->
*/
#ifndef VUL_DEFINE
void *vul_vector_resize( vul_vector_t *vec, unsigned int size, unsigned int free_zero = true, unsigned int alloc_exactly = false );
#else
void *vul_vector_resize( vul_vector_t *vec, unsigned int size, unsigned int free_zero = true, unsigned int alloc_exactly = false )
{
	assert( vec != NULL );
	unsigned int oldSize, newSize;
			
	oldSize = vec->size;
	vec->size = size;

	if ( vec->size == vec->reserved_size ) {
		// Size matches exactly. Assert that things are well then leave.
		if ( vec->reserved_size == 0 ) {
			assert( vec->list == NULL );
		} else  {
			assert( vec->list != NULL );
		}
		return vec->list;
	} else if ( vec->size < vec->reserved_size ) {
		// We have enough room.
		if ( ( vec->size == 0) && free_zero ) {
			// New size is zero and freeing is wanted
			free( vec->list );
			vec->list = NULL;
			vec->reserved_size = 0;
			return NULL;
		}
		if ( !alloc_exactly ) {
			// No resizing needed
			return vec->list;
		}
	}
	// We need to resize
	assert( vec->size > 0 );
	newSize = vec->size;
	if ( !alloc_exactly ) {
		// Grow by 50% and make sure it's not too small
		newSize = ( vec->size * 3 ) >> 1;
		if ( newSize < 8 ) {
			newSize = 8;
		}
		assert( newSize > vec->reserved_size );
	}
	if ( vec->list == NULL ) {
		assert( vec->reserved_size == 0 );
		vec->list = ( unsigned char* )malloc( vec->element_size * newSize );
		assert( vec->list != NULL ); // Make sure malloc didn't fail
	} else {
		assert( vec->reserved_size > 0 );
		vec->list = ( unsigned char* )realloc( vec->list, vec->element_size * newSize );
		assert( vec->list != NULL ); // Make sure realloc didn't fail
	}
	assert( vec->list != NULL );
	vec->reserved_size = newSize;
	return vec->list;
}
#endif
/**
* Preallocates the list to at least this size.
* @param size Minimum size of allocated area.
* @param allocExactly [Optional] Whether reserved size and size should be equal.
*/
#ifndef VUL_DEFINE
void vul_vector_reserve( vul_vector_t *vec, unsigned int size, bool allocExactly = false );
#else
void vul_vector_reserve( vul_vector_t *vec, unsigned int size, bool allocExactly = false )
{
	assert( vec != NULL );
	unsigned int oldSize;

	assert( size >= vec->size );
	if ( size <= vec->reserved_size ) {
		return;
	}

	oldSize = vec->size;
	vul_vector_resize( vec, size, false, allocExactly );
	vec->size = oldSize;
}
#endif
/**
* Initializes the list-> Starting sizes are optional.
* @param initialSize [Optional] Initial size of list->
* @param initialReservedSize [Optional] Initial size of reserved space.
*/
#ifndef VUL_DEFINE
void vul_vector_initialize( vul_vector_t *vec, unsigned int initialSize = 0, unsigned int initialReservedSize = 0 );
#else
void vul_vector_initialize( vul_vector_t *vec, unsigned int initialSize = 0, unsigned int initialReservedSize = 0 )
{
	assert( vec != NULL );
	vec->list = NULL;
	vec->size = 0;
	vec->reserved_size = 0;
	if ( initialReservedSize > initialSize ) {
		vul_vector_reserve( vec, initialReservedSize, true );
		vul_vector_resize( vec, initialSize, false );
	} else if ( initialSize > 0 ) {
		vul_vector_resize( vec, initialSize, true, true );
	}
}
#endif
/**
* Constructor. Takes an optional initial size.
* @param sizeOfType Size, in bytes, of the type stored in this list->
* @param initialSize [Optional] Initial size of list->
*/
#ifndef VUL_DEFINE
vul_vector_t *vul_vector_create( unsigned int sizeOfType, unsigned int initialSize = 0 );
#else
vul_vector_t *vul_vector_create( unsigned int sizeOfType, unsigned int initialSize = 0 )
{
	vul_vector_t *vec = ( vul_vector_t* )malloc( sizeof( vul_vector_t ) );
	assert( vec != NULL ); // Make sure malloc didn't fail
	vec->element_size = sizeOfType;
	vul_vector_initialize( vec, initialSize );
	return vec;
}
#endif
/**
* Copy Constructor. Takes an optional initial size.
* @param ref The list to copy from
*/
#ifndef VUL_DEFINE
vul_vector_t *vul_vector_create( const vul_vector_t *ref );
#else
vul_vector_t *vul_vector_create( const vul_vector_t *ref )
{
	assert( ref != NULL );
	vul_vector_t *vec = ( vul_vector_t* )malloc( sizeof( vul_vector_t ) );
	assert( vec != NULL ); // Make sure malloc didn't fail
	unsigned int size = vul_vector_size( ref );
	vec->element_size = ref->element_size;

	vec->list = NULL;
	vec->size = 0;
	vec->reserved_size = 0;

	vul_vector_resize( vec, size );

	memcpy( vec->list, ref->list, size * vec->element_size );
	return vec;
}
#endif
/**
* Deletes the list and cleans up.
*/
#ifndef VUL_DEFINE
void vul_vector_destroy( vul_vector_t *vec );
#else
void vul_vector_destroy( vul_vector_t *vec )
{
	assert( vec != NULL );
	if ( vec->list == NULL ) {
		assert( vec->reserved_size == 0 );
		assert( vec->size == 0 );
	} else {
		assert( vec->reserved_size > 0 );
		assert( vec->size >= 0 );
		free( vec->list );
		vec->reserved_size = 0;
		vec->size = 0;
		vec->list = NULL;
	}
	free( vec );
}
#endif

/**
* Frees the memory. Equivalent to resize(0, true).
*/
#ifndef VUL_DEFINE
void vul_vector_freemem( vul_vector_t *vec );
#else
void vul_vector_freemem( vul_vector_t *vec )
{
	assert( vec != NULL );
	vul_vector_resize( vec, 0, true );
}
#endif

/**
* Removes the item at the given index by copying the last item to it.
* @param index Index of the element to remove.
*/
#ifndef VUL_DEFINE
void vul_vector_removeSwap( vul_vector_t *vec, unsigned int index );
#else
void vul_vector_removeSwap( vul_vector_t *vec, unsigned int index )
{
	assert( vec != NULL );
	unsigned int elem, last, i;

	assert( index < vec->size );
	elem = index * vec->element_size;
	last = ( vec->size - 1 ) * vec->element_size;
	for ( i = 0; i < vec->element_size; ++i ) {
		vec->list[ elem + i ] = vec->list[ last + i ];
	}
	vul_vector_resize( vec, vec->size - 1 );
}
#endif
/**
* Removes the item at the given index. Copies all following elements one slot forward,
* preserving order of the list->
* @param index Index of the element to remove.
*/
#ifndef VUL_DEFINE
void vul_vector_remove_cascade( vul_vector_t *vec, unsigned int index );
#else
void vul_vector_remove_cascade( vul_vector_t *vec, unsigned int index )
{
	assert( vec != NULL );
	unsigned int i;

	assert( index < vec->size );
	for( i = ( index + 1 ) * vec->element_size; i < ( vec->size * vec->element_size ); i += vec->element_size ) {
		memcpy( &vec->list[ i - vec->element_size ], &vec->list[ i ], vec->element_size );
	}
	vul_vector_resize( vec, vec->size - 1 );
}
#endif
/**
* Adds the slot for an item to teh list and returns a pointer to it.
* @return the pointer to the item
*/
#ifndef VUL_DEFINE
void *vul_vector_add( vul_vector_t *vec );
#else
void *vul_vector_add( vul_vector_t *vec )
{
	assert( vec != NULL );
	vul_vector_resize( vec, vec->size + 1 );
	return ( void* )( &vec->list[ ( vec->size - 1 ) * vec->element_size ] );
}
#endif
/**
* Copies the given item to the list->
* @param item item to add
*/
#ifndef VUL_DEFINE
void vul_vector_add( vul_vector_t *vec, void *item );
#else
void vul_vector_add( vul_vector_t *vec, void *item )
{
	assert( vec != NULL );
	void *slot = vul_vector_add( vec );
	memcpy( slot, item, vec->element_size );
}
#endif
/**
* Inserts a slot for an item to the list at the given location
* and returns a pointer to it. Cascades everything after index
* down one slot.
* @param index index to insert the new item
* @return pointer to the new item
*/
#ifndef VUL_DEFINE
void *vul_vector_insert( vul_vector_t *vec, unsigned int index );
#else
void *vul_vector_insert( vul_vector_t *vec, unsigned int index )
{
	assert( vec != NULL );
	unsigned int i, last, first;

	assert( index <= vec->size );
	vul_vector_resize( vec, vec->size + 1 );
	last = vec->size - 1;
	first = ( index + 1 ) * vec->element_size;
	for ( i = last; i >= first; --i ) {
		vec->list[ i ] = vec->list[ i - vec->element_size ];
	}
	return ( &vec->list[ index * vec->element_size ] );
}
#endif
/**
* Copies the given item to the list-> Cascades everything after index
* down one slot.
* @param item item to add
* @param index index to insert the new item
*/
#ifndef VUL_DEFINE
void vul_vector_insert( vul_vector_t *vec, void *item, unsigned int index );
#else
void vul_vector_insert( vul_vector_t *vec, void *item, unsigned int index )
{
	assert( vec != NULL );
	void *slot = vul_vector_insert( vec, index );
	memcpy( slot, item, vec->element_size );
}
#endif
/**
* Swaps the elemnts at the given indices
* @param indexA index of first element
* @param indexB index of second element
*/
#ifndef VUL_DEFINE
void vul_vector_swap( vul_vector_t *vec, unsigned int indexA, unsigned int indexB );
#else
void vul_vector_swap( vul_vector_t *vec, unsigned int indexA, unsigned int indexB )
{
	assert( vec != NULL );
	assert( indexA < vec->size );
	assert( indexB < vec->size );
	void *temp = malloc( vec->element_size );
	assert( temp != NULL ); // Make sure malloc didn't fail
	memcpy( temp, &vec->list[ indexA * vec->element_size ], vec->element_size );
	memcpy( &vec->list[ indexA * vec->element_size ], &vec->list[ indexB * vec->element_size ], vec->element_size );
	memcpy( &vec->list[ indexB * vec->element_size ], temp, vec->element_size );
}
#endif

/**
* Copies from the given array into this vector. Will overwrite anything after
* index with the new items.
* @param index The index to copy the first item to
* @param list The other vector that is to be copied from
* @param count The number of items to copy
*/
#ifndef VUL_DEFINE
void vul_vector_copy( vul_vector_t *dest, unsigned int index, const void* list, unsigned int count );
#else
void vul_vector_copy( vul_vector_t *dest, unsigned int index, const void* list, unsigned int count )
{
	assert( dest != NULL );
	void *first;

	if ( dest->size < ( index + count ) ) {
		vul_vector_resize( dest, index + count );
	}
	first = vul_vector_get( dest, index );
	memcpy( first, list, count * dest->element_size );
}
#endif
/**
* Copies from the given vector into this one. Will overwrite anything after
* index with the new items.
* @param index The index to copy the first item to
* @param list The other vector that is to be copied from
* @param otherFirstIndex [Optional] The first index in the other vector to copy. Default is 0.
* @param otherCount [Optional] The number of items to copy. Default is entire list->
*/
#ifndef VUL_DEFINE
void vul_vector_copy( vul_vector_t *dest, unsigned int index, vul_vector_t *list, unsigned int otherFirstIndex = 0, unsigned int otherCount = 0xffffffff );
#else
void vul_vector_copy( vul_vector_t *dest, unsigned int index, vul_vector_t *list, unsigned int otherFirstIndex = 0, unsigned int otherCount = 0xffffffff )
{
	assert( dest != NULL );
	assert( list != NULL );
	void *firstLocal;
	const void *firstOther;

	if ( otherCount == 0xffffffff ) {
		otherCount = vul_vector_size( list )- otherFirstIndex;
	}
	if ( dest->size < ( index + otherCount ) ) {
		vul_vector_resize( dest, index + otherCount );
	}
	assert( vul_vector_size( list ) >= ( otherFirstIndex + otherCount ) );
	firstLocal = vul_vector_get( dest, index );
	firstOther = vul_vector_get_const( list, otherFirstIndex );
	memcpy( firstLocal, firstOther, otherCount * dest->element_size );
}
#endif

/**
* Copies from the given vector onto the end of this one.
* @param list The other vector that is to be copied from
* @param otherFirstIndex The first index in the other vector to copy
* @param otherCount [Optional] The number of items to copy. Default is entire list->
*/
#ifndef VUL_DEFINE
void vul_vector_append( vul_vector_t *dest, vul_vector_t *list, unsigned int otherFirstIndex, unsigned int otherCount = 0xffffffff );
#else
void vul_vector_append( vul_vector_t *dest, vul_vector_t *list, unsigned int otherFirstIndex, unsigned int otherCount = 0xffffffff )
{
	assert( dest != NULL );
	assert( list != NULL );
	unsigned int first;

	if ( otherCount == 0xffffffff ) {
		otherCount = vul_vector_size( list ) - otherFirstIndex;
	}
	first = dest->size;
	vul_vector_resize( dest, first + otherCount );
	vul_vector_copy( dest, first, list, otherFirstIndex, otherCount );
}
#endif
/**
* Finds the index of the given item, or -1 if not found.
* @param item The item to find
* @return the index of the item, -1 if not found.
*/
#ifndef VUL_DEFINE
unsigned int vul_vector_find( vul_vector_t *vec, void *item );
#else
unsigned int vul_vector_find( vul_vector_t *vec, void *item )
{
	assert( vec != NULL );
	unsigned int i;

	for ( i = 0; i < vec->size; i += vec->element_size ) {
		if ( &vec->list[ i ] == item ) {
			return i;
		}
	}
	return vec->size;
}
#endif
		
/**
* Shrinks the vector to fit the current size.
*/
#ifndef VUL_DEFINE
void vul_vector_tighten( vul_vector_t *vec );
#else
void vul_vector_tighten( vul_vector_t *vec )
{
	assert( vec != NULL );
	vul_vector_resize( vec, vec->size, true, true );
}
#endif


// ----------------
// Macros
//

// Iterators
#ifdef VUL_DEBUG
	// @Note: Debug versions attempt to spot if you alter elemtns in the middle of a loop. That might cause a resize and in turn mayhem.
	// Normal iterator
	#define vul_foreach( T, list ) for ( T *it = list->begin( ), *first = list->begin( ), *last = list->end( ); internal_functional_assert( first == list->begin( ) ), internal_functional_assert( last == list->end( ) ), it != last; ++it )
	// Easier for list of values; copies
	#define vul_foreachval( T, ref, list ) for ( T *it = list->begin( ), *first = list->begin( ), *last = list->end( ), ref = ( ( it != last ) ? *it : T( ) ); internal_functional_assert( first == list->begin( ) ), internal_functional_assert( last == list->end( ) ), it != last; ref = *( ++it ) )
	// Easier for list of pointers
	#define vul_foreachptr( T, ref, list ) for ( T **it = list->begin( ), **first = list->begin( ), **last = list->end( ), *ref = ( ( it != last ) ? *it : NULL ); internal_functional_assert( first == list->begin( ) ), internal_functional_assert( last == list->end( ) ), it != last; ref = *( ++it ) )
#else
	// Normal iterator
	#define vul_foreach( T, list ) for ( T *it = list->begin( ), *last = list->end( ); it != last; ++it )
	// Easier for list of values; copies
	#define vul_foreachval( T, ref, list ) for ( T *it = list->begin( ), *last = list->end( ), ref = ( ( it != last ) ? *it : T( ) ); it != last; ref = *( ++it ) )
	// Easier for list of pointers
	#define vul_foreachptr( T, ref, list ) for ( T **it = list->begin( ), **last = list->end( ), *ref = ( ( it != last ) ? *it : NULL ); it != last; ref = *( ++it ) )
#endif

#endif