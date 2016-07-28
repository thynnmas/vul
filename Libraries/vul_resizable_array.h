/*
 * Villains' Utility Library - Thomas Martin Schmid, 2016. Public domain?
 *
 * This file describes an alternative to the STL vector class. This is mostly 
 * based on Tom Forsyth's ArbitraryList found here: https://home.comcast.net/~tom_forsyth/blog.wiki.html
 * This version is not templated (so pure C). Sorting of this list is available in vul_sort.h
 * 
 * ? If public domain is not legally valid in your legal jurisdiction
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
#ifndef VUL_RESIZEABLE_ARRAY_H
#define VUL_RESIZEABLE_ARRAY_H

#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "vul_types.h"

#define VUL_TRUE 1
#define VUL_FALSE 0

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
typedef struct vul_vector {
		
	/**
	* The reserved size of the list
	*/
	u32 reserved_size;
	/**
	* The current actual size of the list
	*/
	u32 size;
	/**
	* The list itself
	*/
	u8 *list;

	/**
	* If we are not using templates, we need the size of an element for easier list handling.
	*/
	u32 element_size;

	/* Memory management fucntions */
	void *( *allocator )( size_t size );
	void( *deallocator )( void *ptr );
	void *( *reallocator )( void *ptr, size_t size );
} vul_vector;

#ifdef _cplusplus
extern "C" {
#endif
/**
* Returns the pointer to the first element of the list->
* If templates are used, this is typed, if not it is a void*.
*/
void *vul_vector_begin( vul_vector *vec );
/**
* Returns the pointer to the last element of the list plus 1.
* This is the equivalent of the STL vector.end().
* If templates are used, this is typed, if not it is a void*.
*/
void *vul_vector_end( vul_vector *vec );
/**
* Returns the element size of the type kept. Only for non-templated vectors.
*/
const u32 vul_vector_size( const vul_vector *vec );
/**
* Returns the pointer to the item at the given location.
* If templates are used, this is typed, if not it is a void*.
* @param index Index of the element to get.
*/	
void *vul_vector_get( vul_vector *vec, const u32 index );
/**
* Returns a const pointer to the item at the given location.
* If templates are used, this is typed, if not it is a void*.
* @param index
*/	
const void *vul_vector_get_const( vul_vector *vec, const u32 index );
/**
* Resizes the list to the given number of items. Preserves items already
* in the list up to the new size, others are lost.
* @param size Size of the new list
* @param freeZero Whether the memory should be freed if new size is 0.
* @param allocExactly Whether reserved size and size should be equal.
* @return a pointer to the first element of the new list->
*/
void *vul_vector_resize( vul_vector *vec, u32 size,
						 u32 free_zero, u32 alloc_exactly );
/**
* Preallocates the list to at least this size.
* @param size Minimum size of allocated area.
* @param allocExactly Whether reserved size and size should be equal.
*/
void vul_vector_reserve( vul_vector *vec, u32 size, s32 allocExactly );
/**
* Initializes the list
* @param initialSize Initial size of list
* @param initialReservedSize Initial size of reserved space.
*/
void vul_vector_initialize( vul_vector *vec, u32 initialSize, 
							u32 initialReservedSize );
/**
* Constructor. Takes initial size.
* @param sizeOfType Size, in bytes, of the type stored in this list.
* @param initialSize Initial reseurved size of list.
* @param allocator Allocation function (f.ex. malloc)
* @param deallocator Deallocation function (f.ex. free)
* @param reallocator Reallocation function (f.ex. realloc)
*/
vul_vector *vul_vector_create( u32 sizeOfType, 
							   u32 initialSize,
							   void *( *allocator )( size_t size ),
							   void  ( *deallocator )( void *ptr ),
							   void *( *reallocator )( void *ptr, size_t size ) );
/**
* Deletes the list and cleans up.
*/
void vul_vector_destroy( vul_vector *vec );
/**
* Frees the memory. Equivalent to resize(0, VUL_TRUE).
*/
void vul_vector_freemem( vul_vector *vec );
/**
* Removes the item at the given index by copying the last item to it.
* @param index Index of the element to remove.
*/
void vul_vector_remove_swap( vul_vector *vec, u32 index );
/**
* Removes the item at the given index. Copies all following elements one slot forward,
* preserving order of the list->
* @param index Index of the element to remove.
*/
void vul_vector_remove_cascade( vul_vector *vec, u32 index );
/**
* Adds the slot for an item to teh list and returns a pointer to it.
* @return the pointer to the item
*/
void *vul_vector_add_empty( vul_vector *vec );
/**
* Copies the given item to the list->
* @param item item to add
*/
void vul_vector_add( vul_vector *vec, void *item );
/**
* Inserts a slot for an item to the list at the given location
* and returns a pointer to it. Cascades everything after index
* down one slot.
* @param index index to insert the new item
* @return pointer to the new item
*/
void *vul_vector_insert_empty( vul_vector *vec, u32 index );
/**
* Copies the given item to the list-> Cascades everything after index
* down one slot.
* @param item item to add
* @param index index to insert the new item
*/
void vul_vector_insert( vul_vector *vec, void *item, u32 index );
/**
* Swaps the elemnts at the given indices
* @param indexA index of first element
* @param indexB index of second element
*/
void vul_vector_swap( vul_vector *vec, u32 indexA, u32 indexB );
/**
* Copies from the given array into this vector. Will overwrite anything after
* index with the new items.
* @param index The index to copy the first item to
* @param list The other vector that is to be copied from
* @param count The number of items to copy
*/
void vul_vector_copy( vul_vector *dest, u32 index, 
					  const void* list, u32 count );
/**
* Copies from the given vector into this one. Will overwrite anything after
* index with the new items.
* @param index The index to copy the first item to
* @param list The other vector that is to be copied from
* @param otherFirstIndex The first index in the other vector to copy. Default is 0.
* @param otherCount The number of items to copy. 
*/
void vul_vector_copy_partial( vul_vector *dest, u32 index, 
							  vul_vector *list, u32 otherFirstIndex, 
							  u32 otherCount );
/**
* Copies from the given vector onto the end of this one.
* @param list The other vector that is to be copied from
* @param otherFirstIndex The first index in the other vector to copy
* @param otherCount The number of items to copy. 
*/
void vul_vector_append( vul_vector *dest, vul_vector *list, 
						u32 otherFirstIndex, u32 otherCount );
/**
* Finds the index of the given item, or -1 if not found.
* @param item The item to find
* @return the index of the item, -1 if not found.
*/
u32 vul_vector_find( vul_vector *vec, void *item );
/**
* Finds the index of the given item, or -1 if not found.
* This compares value of entire element, not pointers.
* @param item The item to find
* @return the index of the item, -1 if not found.
*/
u32 vul_vector_find_val( vul_vector *vec, void *item );
/**
* Finds the index of the given item, or -1 if not found.
* This uses a given comparator to compare the items
*/
u32 vul_vector_find_comparator( vul_vector *vec, void *item, s32 ( *comparator )( void* a, void *b ) );
/**
* Shrinks the vector to fit the current size.
*/
void vul_vector_tighten( vul_vector *vec );


// ----------------
// Macros
//

// Undefine them if we're redefining them, so we don't get complaining compilers...
#ifdef vul_foreachptr
#undef vul_foreachptr
#endif
#ifdef vul_foreachval
#undef vul_foreachval
#endif
#ifdef vul_foreach
#undef vul_foreach
#endif
#ifdef VUL_DEBUG
	// @Note: Debug versions attempt to spot if you alter the list in the middle of a loop. That might cause a resize and in turn mayhem.
	// Normal iterator
#define vul_foreach( T, list ) for ( T *it = ( T* )vul_vector_begin( list ), *first = ( T* )vul_vector_begin( list ), *last = ( T* )vul_vector_end( list ); assert( first == ( T* )vul_vector_begin( list ) ), assert( last == ( T* )vul_vector_end( list ) ), it != last; ++it )
	// Easier for list of values; copies
#define vul_foreachval( T, ref, list ) for ( T *it = ( T* )vul_vector_begin( list ), *first = ( T* )vul_vector_begin( list ), *last = ( T* )vul_vector_end( list ), ref = ( ( it != last ) ? *it : T( ) ); assert( first == ( T* )vul_vector_begin( list ) ), assert( last == ( T* )vul_vector_end( list ) ), it != last; ref = *( ++it ) )
	// Easier for list of pointers
#define vul_foreachptr( T, ref, list ) for ( T **it = ( T** )vul_vector_begin( list ), **first = ( T** )vul_vector_begin( list ), **last = ( T** )vul_vector_end( list ), *ref = ( ( it != last ) ? *it : NULL ); assert( first == ( T* )vul_vector_begin( list ) ), assert( last == ( T* )vul_vector_end( list ) ), it != last; ref = *( ++it ) )
#else
	// Normal iterator
	#define vul_foreach( T, list ) for ( T *it = ( T* )vul_vector_begin( list ), *last = ( T* )vul_vector_end( list ); it != last; ++it )
	// Easier for list of values; copies
	#define vul_foreachval( T, ref, list ) for ( T *it = ( T* )vul_vector_begin( list ), *last = ( T* )vul_vector_end( list ), ref = ( ( it != last ) ? *it : T( ) ); it != last; ref = *( ++it ) )
	// Easier for list of pointers
	#define vul_foreachptr( T, ref, list ) for ( T **it = ( T** )vul_vector_begin( list ), **last = ( T** )vul_vector_end( list ), *ref = ( ( it != last ) ? *it : NULL ); it != last; ref = *( ++it ) )
#endif

#ifdef VUL_VECTOR_C89_ITERATORS
	// @NOTE: These iterators support C89 (because MSVC doesn't support C99...). They require all their arguments to be preinitialized.
	#undef vul_foreachptr
	#undef vul_foreachval
	#undef vul_foreach
	#ifdef VUL_DEBUG
		// @Note: Debug versions attempt to spot if you alter the list in the middle of a loop. That might cause a resize and in turn mayhem.
		// Normal iterator
		#define vul_foreach( T, it, first, last, list ) for ( it = ( T* )vul_vector_begin( list ), first = ( T* )vul_vector_begin( list ), last = ( T* )vul_vector_end( list ); internal_functional_assert( first == ( T* )vul_vector_begin( list ) ), internal_functional_assert( last == ( T* )vul_vector_end( list ) ), it != last; ++it )
		// Easier for list of values; copies
		#define vul_foreachval( T, ref, it, first, last, list ) for ( it = ( T* )vul_vector_begin( list ), first = ( T* )vul_vector_begin( list ), last = ( T* )vul_vector_end( list ), ref = ( ( it != last ) ? *it : T( ) ); internal_functional_assert( first == ( T* )vul_vector_begin( list ) ), internal_functional_assert( last == ( T* )vul_vector_end( list ) ), it != last; ref = *( ++it ) )
		// Easier for list of pointers
		#define vul_foreachptr( T, ref, it, first, last, list ) for ( it = ( T** )vul_vector_begin( list ), first = ( T** )vul_vector_begin( list ), last = ( T** )vul_vector_end( list ), *ref = ( ( it != last ) ? *it : NULL ); internal_functional_assert( first == ( T* )vul_vector_begin( list ) ), internal_functional_assert( last == ( T* )vul_vector_end( list ) ), it != last; ref = *( ++it ) )
	#else
		// Normal iterator
		#define vul_foreach( T, it, last, list ) for ( it = ( T* )vul_vector_begin( list ), last = ( T* )vul_vector_end( list ); it != last; ++it )
		// Easier for list of values; copies
		#define vul_foreachval( T, ref, it, last, list ) for ( it = ( T* )vul_vector_begin( list ), last = ( T* )vul_vector_end( list ), ref = ( ( it != last ) ? *it : T( ) ); it != last; ref = *( ++it ) )
		// Easier for list of pointers
		#define vul_foreachptr( T, ref, it, last, list ) for ( it = ( T** )vul_vector_begin( list ), last = ( T** )vul_vector_end( list ), *ref = ( ( it != last ) ? *it : NULL ); it != last; ref = *( ++it ) )
	#endif
#endif

#ifdef _cplusplus
}
#endif
#endif

#ifdef VUL_DEFINE

#ifdef _cplusplus
extern "C" {
#endif

void *vul_vector_begin( vul_vector *vec )
{
	assert( vec != NULL );
	return vec->list;
}

void *vul_vector_end( vul_vector *vec )
{
	assert( vec != NULL );
	return ( void* )( &vec->list[ vec->element_size * vec->size ] );
}
const u32 vul_vector_size( const vul_vector *vec )
{
	assert( vec != NULL );
	return vec->size;
}

void *vul_vector_get( vul_vector *vec, const u32 index )
{
	assert( vec != NULL );
	assert( index < vec->size );
	return ( void* )( &vec->list[ vec->element_size * index ] );
}

const void *vul_vector_get_const( vul_vector *vec, const u32 index )
{
	assert( vec != NULL );
	assert( index < vec->size );
	return ( void* )( &vec->list[ vec->element_size * index ] );
}

void *vul_vector_resize( vul_vector *vec, u32 size, u32 free_zero, u32 alloc_exactly )
{
	u32 new_size;

	assert( vec != NULL );

	vec->size = size;

	if( vec->size == vec->reserved_size ) {
		// Size matches exactly. Assert that things are well then leave.
		if( vec->reserved_size == 0 ) {
			assert( vec->list == NULL );
		} else  {
			assert( vec->list != NULL );
		}
		return vec->list;
	} else if( vec->size < vec->reserved_size ) {
		// We have enough room.
		if( ( vec->size == 0 ) && free_zero ) {
			// New size is zero and freeing is wanted
			vec->deallocator( vec->list );
			vec->list = NULL;
			vec->reserved_size = 0;
			return NULL;
		}
		if( !alloc_exactly ) {
			// No resizing needed
			return vec->list;
		}
	}
	// We need to resize
	assert( vec->size > 0 );
	new_size = vec->size;
	if( !alloc_exactly ) {
		// Grow by 50% and make sure it's not too small
		new_size = ( vec->size * 3 ) >> 1;
		if( new_size < 8 ) {
			new_size = 8;
		}
		assert( new_size > vec->reserved_size );
	}
	if( vec->list == NULL ) {
		assert( vec->reserved_size == 0 );
		vec->list = ( u8* )vec->allocator( vec->element_size * new_size );
		assert( vec->list != NULL ); // Make sure allocation didn't fail
	} else {
		assert( vec->reserved_size > 0 );
#pragma warning(suppress: 6308) // We know it might leak, but the assert will trigger if it does!
		vec->list = ( u8* )vec->reallocator( vec->list, vec->element_size * new_size );
		assert( vec->list != NULL ); // Make sure reallocation didn't fail
	}
	assert( vec->list != NULL );
	vec->reserved_size = new_size;
	return vec->list;
}

void vul_vector_reserve( vul_vector *vec, u32 size, s32 allocExactly )
{
	unsigned int old_size;
	assert( vec != NULL );

	assert( size >= vec->size );
	if( size <= vec->reserved_size ) {
		return;
	}

	old_size = vec->size;
	vul_vector_resize( vec, size, VUL_FALSE, allocExactly );
	vec->size = old_size;
}

void vul_vector_initialize( vul_vector *vec, u32 initialSize, u32 initialReservedSize )
{
	assert( vec != NULL );
	vec->list = NULL;
	vec->size = 0;
	vec->reserved_size = 0;
	if( initialReservedSize > initialSize ) {
		vul_vector_reserve( vec, initialReservedSize, VUL_TRUE );
		vul_vector_resize( vec, initialSize, VUL_FALSE, VUL_FALSE );
	} else if( initialSize > 0 ) {
		vul_vector_resize( vec, initialSize, VUL_TRUE, VUL_TRUE );
	}
}

vul_vector *vul_vector_create( unsigned int sizeOfType,
							   unsigned int initialSize,
							   void *( *allocator )( size_t size ),
							   void( *deallocator )( void *ptr ),
							   void *( *reallocator )( void *ptr, size_t size ) )
{
	vul_vector *vec = ( vul_vector* )allocator( sizeof( vul_vector ) );
	assert( vec != NULL ); // Make sure allocation didn't fail
	vec->element_size = sizeOfType;
	vec->allocator = allocator;
	vec->deallocator = deallocator;
	vec->reallocator = reallocator;
	vul_vector_initialize( vec, 0, initialSize );
	return vec;
}

void vul_vector_destroy( vul_vector *vec )
{
	assert( vec != NULL );
	if( vec->list == NULL ) {
		assert( vec->reserved_size == 0 );
		assert( vec->size == 0 );
	} else {
		assert( vec->reserved_size > 0 );
		assert( vec->size >= 0 );
		vec->deallocator( vec->list );
		vec->reserved_size = 0;
		vec->size = 0;
		vec->list = NULL;
	}
	vec->deallocator( vec );
}

void vul_vector_freemem( vul_vector *vec )
{
	assert( vec != NULL );
	vul_vector_resize( vec, 0, VUL_TRUE, VUL_FALSE );
}

void vul_vector_remove_swap( vul_vector *vec, u32 index )
{
	u32 elem, last, i;
	assert( vec != NULL );

	assert( index < vec->size );
	elem = index * vec->element_size;
	last = ( vec->size - 1 ) * vec->element_size;
	for( i = 0; i < vec->element_size; ++i ) {
		vec->list[ elem + i ] = vec->list[ last + i ];
	}
	vul_vector_resize( vec, vec->size - 1, VUL_TRUE, VUL_FALSE );
}

void vul_vector_remove_cascade( vul_vector *vec, u32 index )
{
	u32 i;
	assert( vec != NULL );

	assert( index < vec->size );
	for( i = ( index + 1 ) * vec->element_size; i < ( vec->size * vec->element_size ); i += vec->element_size ) {
		memcpy( &vec->list[ i - vec->element_size ], &vec->list[ i ], vec->element_size );
	}
	vul_vector_resize( vec, vec->size - 1, VUL_TRUE, VUL_FALSE );
}

void *vul_vector_add_empty( vul_vector *vec )
{
	assert( vec != NULL );
	vul_vector_resize( vec, vec->size + 1, VUL_TRUE, VUL_FALSE );
	return ( void* )( &vec->list[ ( vec->size - 1 ) * vec->element_size ] );
}

void vul_vector_add( vul_vector *vec, void *item )
{
	void *slot;

	assert( vec != NULL );

	slot = vul_vector_add_empty( vec );
	memcpy( slot, item, vec->element_size );
}

void *vul_vector_insert_empty( vul_vector *vec, u32 index )
{
	u32 i, last, first;

	assert( vec != NULL );
	assert( index <= vec->size );

	vul_vector_resize( vec, vec->size + 1, VUL_TRUE, VUL_FALSE );
	last = vec->size - 1;
	first = ( index + 1 ) * vec->element_size;
	for( i = last; i >= first; --i ) {
		vec->list[ i ] = vec->list[ i - vec->element_size ];
	}
	return ( &vec->list[ index * vec->element_size ] );
}

void vul_vector_insert( vul_vector *vec, void *item, u32 index )
{
	void *slot;

	assert( vec != NULL );

	slot = vul_vector_insert_empty( vec, index );
	memcpy( slot, item, vec->element_size );
}

void vul_vector_swap( vul_vector *vec, u32 indexA, u32 indexB )
{
	void *temp;

	assert( vec != NULL );
	assert( indexA < vec->size );
	assert( indexB < vec->size );

	temp = vec->allocator( vec->element_size );
	assert( temp != NULL ); // Make sure allocation didn't fail
	memcpy( temp, &vec->list[ indexA * vec->element_size ], vec->element_size );
	memcpy( &vec->list[ indexA * vec->element_size ], &vec->list[ indexB * vec->element_size ], vec->element_size );
	memcpy( &vec->list[ indexB * vec->element_size ], temp, vec->element_size );
}

void vul_vector_copy( vul_vector *dest, u32 index, 
					  const void* list, u32 count )
{
	void *first;

	assert( dest != NULL );

	if( dest->size < ( index + count ) ) {
		vul_vector_resize( dest, index + count, VUL_TRUE, VUL_FALSE );
	}
	first = vul_vector_get( dest, index );
	memcpy( first, list, count * dest->element_size );
}

void vul_vector_copy_partial( vul_vector *dest, u32 index, 
							  vul_vector *list, u32 otherFirstIndex, 
							  u32 otherCount )
{
	void *firstLocal;
	const void *firstOther;

	assert( dest != NULL );
	assert( list != NULL );

	if( dest->size < ( index + otherCount ) ) {
		vul_vector_resize( dest, index + otherCount, VUL_TRUE, VUL_FALSE );
	}
	assert( vul_vector_size( list ) >= ( otherFirstIndex + otherCount ) );
	firstLocal = vul_vector_get( dest, index );
	firstOther = vul_vector_get_const( list, otherFirstIndex );
	memcpy( firstLocal, firstOther, otherCount * dest->element_size );
}

void vul_vector_append( vul_vector *dest, vul_vector *list, 
						u32 otherFirstIndex, u32 otherCount )
{
	u32 first;

	assert( dest != NULL );
	assert( list != NULL );

	first = dest->size;
	vul_vector_resize( dest, first + otherCount, VUL_TRUE, VUL_FALSE );
	vul_vector_copy_partial( dest, first, list, otherFirstIndex, otherCount );
}

u32 vul_vector_find( vul_vector *vec, void *item )
{
	u32 i;

	assert( vec != NULL );

	for( i = 0; i < vec->size; ++i ) {
		if( &vec->list[ i * vec->element_size ] == item ) {
			return i;
		}
	}
	return vec->size;
}

u32 vul_vector_find_val( vul_vector *vec, void *item )
{
	u32 i;

	assert( vec != NULL );

	for( i = 0; i < vec->size; ++i ) {
		if( memcmp( &vec->list[ i * vec->element_size ], item, vec->element_size ) == 0 ) {
			return i;
		}
	}
	return -1;
}

u32 vul_vector_find_comparator( vul_vector *vec, void *item, s32 ( *comparator )( void* a, void *b ) )
{
	u32 i;

	assert( vec != NULL );

	for( i = 0; i < vec->size; ++i ) {
		if( comparator( &vec->list[ i * vec->element_size ], item ) == 0 ) {
			return i;
		}
	}
	return -1;
}

void vul_vector_tighten( vul_vector *vec )
{
	assert( vec != NULL );
	vul_vector_resize( vec, vec->size, VUL_TRUE, VUL_TRUE );
}

#ifdef _cpluspluc
}
#endif

#endif

