/*
 * Villains' Utility Library - Thomas Martin Schmid, 2015. Public domain¹
 *
 * This file describes hash map implementation.
 * Buckets are implemented as linked lists to deal with collissions.
 * The hash function used is given in the constructor, allowing this implementation
 * to be released under the licence (or lack thereof) described above while
 * still being able to take advantage of hash fucntions by smarter people than me.
 * @TODO: Dynamic slot-resizing; like resizable array, if population reaches a threshold,
 *        grow the bucket_count and rehash all elements into new buckets.
 *
 * For fast, good hash functions, these should be equivalent:
 *  - http://www.azillionmonkeys.com/qed/hash.html : LGPL 2.1
 *  - http://burtleburtle.net/bob/c/lookup3.c : Public Domain
 * There is also SpookyHash, which is a lot faster, but 64-bit only:
 *  - http://burtleburtle.net/bob/hash/spooky.html : Publidc Domain
 * 
 * ¹ If public domain is not legally valid in your legal jurisdiction
 *   the MIT licence applies (see the LICENCE file)
 *
 * @TODO(thynn): Keep keys locally (use a stable array to store ll-elements,
 * point the keys to those in those elements; change key-ptr in map-elements
 * upon remove_swap in key-element list). Also possibly rewrite to something 
 * mroe data-oriented...
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#ifndef VUL_HASH_MAP_H
#define VUL_HASH_MAP_H

#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "vul_types.h"
#ifndef VUL_LINKED_LIST_H
#include "vul_linked_list.h"
#endif

/**
 * If defined, the functions are defined and not just declared. Only do this in _one_ c/cpp file!
 */
//#define VUL_DEFINE

typedef ui32_t ( *vul_hash_function )( const ui8_t* data, ui32_t len );

typedef struct vul_hash_map_t {
	ui32_t bucket_count;
	vul_list_element_t **buckets;
	vul_hash_function hash;
	int (*comparator)( void* a, void *b );	// Comparison function

	/* Memory management functions */
	void *( *allocator )( size_t size );
	void  ( *deallocator )( void *ptr );
} vul_hash_map_t;

typedef struct vul_hash_map_element_t {
	void *key;
	void *data;

	ui32_t key_size;
	ui32_t data_size;
} vul_hash_map_element_t;

#endif

/**
 * Creates a new hash map. Takes the number of slots to use, the hash function
 * and a comparison function as arguments.
 * The comparison function takes an entire vul_hash_map_element (casted to a void*) as it's input.
 * but must only compare keys. This is both natural, and demanded by the get/get_const functions, which only
 * take keys but must be able to compare them to elements in the map.
 */
#ifndef VUL_DEFINE
vul_hash_map_t *vul_map_create( ui32_t bucket_count, 
								vul_hash_function hash_function, 
								int (*comparator)( void* a, void *b ), 
								void *( *allocator )( size_t size ), 
								void( *deallocator )( void *ptr ) );
#else
vul_hash_map_t *vul_map_create( ui32_t bucket_count,
								vul_hash_function hash_function,
								int( *comparator )( void* a, void *b ),
								void *( *allocator )( size_t size ),
								void( *deallocator )( void *ptr ) )
{
	vul_hash_map_t *map;
	ui32_t i;

	map = ( vul_hash_map_t* )allocator( sizeof( vul_hash_map_t ) );
	assert( map != NULL ); // Make sure allocation didn't fail
	map->bucket_count = bucket_count;
	map->hash = hash_function;
	map->comparator = comparator;
	map->buckets = ( vul_list_element_t** )allocator( sizeof( vul_list_element_t* ) * bucket_count );
	map->allocator = allocator;
	map->deallocator = deallocator;
	assert( map->buckets != NULL ); // Make sure allocation didn't fail
	for( i = 0; i < bucket_count; ++i )
	{
		map->buckets[ i ] = NULL;
	}

	return map;
}
#endif

/**
 * Copies the given element and inserts it into the given hash map.
 * Returns the element.
 */
#ifndef VUL_DEFINE
vul_hash_map_element_t *vul_map_insert( vul_hash_map_t *map, const vul_hash_map_element_t *ref );
#else
vul_hash_map_element_t *vul_map_insert( vul_hash_map_t *map, const vul_hash_map_element_t *ref )
{
	ui32_t bucket;
	vul_list_element_t *e;

	// Find the bucket
	bucket = map->hash( ( ui8_t* )ref->key, ref->key_size ) % map->bucket_count;

	// Insert it into the list at that bucket.
	// vul_list_insert copies entire element, so both data and key.
	if( map->buckets[ bucket ] == NULL ) {
		map->buckets[ bucket ] = vul_list_insert( NULL,
												  ( void* )ref, sizeof( vul_hash_map_element_t ),
												  map->comparator, map->allocator );
		return ( vul_hash_map_element_t* )map->buckets[ bucket ]->data;
	} else {
		e = vul_list_insert( map->buckets[ bucket ],
							 ( void* )ref, sizeof( vul_hash_map_element_t ),
							 map->comparator, map->allocator );
		if( e->prev == NULL ) {
			// We have changed head of the list!
			map->buckets[ bucket ] = e;
		}
		return ( vul_hash_map_element_t* )e->data;
	}
}
#endif

/**
 * Deletes the given element from the hash map.
 */
#ifndef VUL_DEFINE
void vul_map_remove( vul_hash_map_t *map, const vul_hash_map_element_t *ref );
#else
void vul_map_remove( vul_hash_map_t *map, const vul_hash_map_element_t *ref )
{
	ui32_t bucket;
	vul_list_element_t *e;

	// Find the bucket
	bucket = map->hash( ( ui8_t* )ref->key, ref->key_size ) % map->bucket_count;

	// Find the element
	e = vul_list_find( map->buckets[ bucket ], ( void* )ref, map->comparator );
	// Delete if an exact match; vul_list_find will return NULL if what we search for is smaller than
	// all elements in the list and the biggest smaller element if no match is found, so check it matches.
	if( e != NULL && map->comparator( e->data, ( void* )ref ) == 0 )
	{
		if( e->prev == NULL ) {
			map->buckets[ bucket ] = e->next;
		}
		vul_list_remove( e, map->deallocator );
	}
}
#endif

/**
 * Retrieves a pointer to the element matching the given key from the hash map.
 * Returns NULL if no matching element is found.
 */
#ifndef VUL_DEFINE
vul_hash_map_element_t *vul_map_get( vul_hash_map_t *map, void *key, ui32_t key_size );
#else
vul_hash_map_element_t *vul_map_get( vul_hash_map_t *map, void *key, ui32_t key_size )
{
	vul_hash_map_element_t e;
	vul_list_element_t *le;
	ui32_t bucket;

	e.key = key;
	e.key_size = key_size;

	// Find bucket
	bucket = map->hash( ( ui8_t* )key, key_size ) % map->bucket_count;

	// Find an element
	if( map->buckets[ bucket ] != NULL ) {
		le = vul_list_find( map->buckets[ bucket ], &e, map->comparator );
	} else {
		return NULL;
	}

	// Check that it's an actual match
	if( le != NULL && map->comparator( le->data, &e ) == 0 ) {
		return ( vul_hash_map_element_t* )le->data;
	} else {
		return NULL;
	}
}
#endif

/**
 * Retrieves a const pointer to the element matching the given key from the hash map.
 * Returns NULL if no matching element is found.
 */
#ifndef VUL_DEFINE
const vul_hash_map_element_t *vul_map_get_const( vul_hash_map_t *map, void *key, ui32_t key_size );
#else
const vul_hash_map_element_t *vul_map_get_const( vul_hash_map_t *map, void *key, ui32_t key_size )
{
	return vul_map_get( map, key, key_size );
}
#endif

/**
 * Destroys the given has map, deallocating all it's used memory.
 */
#ifndef VUL_DEFINE
void vul_map_destroy( vul_hash_map_t *map );
#else
void vul_map_destroy( vul_hash_map_t *map )
{
	ui32_t b;

	for( b = 0; b < map->bucket_count; ++b )
	{
		if( map->buckets[ b ] != NULL )
		{
			vul_list_destroy( map->buckets[ b ], map->deallocator );
			map->buckets[ b ] = NULL;
		}
	}

	map->deallocator( map->buckets );
	map->buckets = NULL;
	map->deallocator( map );
	// By setting to null we are much more likely to trigger asserts if used after decallocation.
	map = NULL;
}
#endif
