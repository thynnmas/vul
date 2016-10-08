/*
 * Villains' Utility Library - Thomas Martin Schmid, 2016. Public domain?
 *
 * This file describes a robin-hood hash map implementation.
 * The hash function used is given in the constructor, allowing this implementation
 * to be released under the licence (or lack thereof) described above while
 * still being able to take advantage of hash functions by smarter people than me.
 *
 * For fast, good hash functions, these should be equivalent:
 *  - http://www.azillionmonkeys.com/qed/hash.html : LGPL 2.1
 *  - http://burtleburtle.net/bob/c/lookup3.c : Public Domain
 * There is also SpookyHash, which is a lot faster, but 64-bit only:
 *  - http://burtleburtle.net/bob/hash/spooky.html : Public Domain
 * 
 * ? If public domain is not legally valid in your legal jurisdiction
 *   the MIT licence applies (see the LICENCE file)
 *
 * Define VUL_DEFINE in exactly one compilation unit.
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

#ifndef VUL_TYPES_H
#include <stdint.h>
#define s32 int32_t
#define f32 float
#define u32 uint32_t
#define b32 u32
#endif

typedef u32 ( *vul_hash_function )( const void* key, const u32 len );

typedef struct vul_map_element {
   void *key;
   void *value;
} vul_map_element;

typedef struct vul_hash_map {
   vul_map_element *entries;
   u32 *hashes;
   u32 count, size, mask;
   f32 factor;
   u32 key_size, value_size;

   vul_hash_function hash;
   int (*comparator)( const void* a, const void *b ); // Comparison function

   /* Memory management functions */
   void *( *allocator )( size_t size );
   void  ( *deallocator )( void *ptr );
} vul_hash_map;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Creates a new hash map. Takes the initial size (must be a power of two!), the maximum load factor, 
 * the sizes of the key and value elements, the hash function and a comparison function as arguments.
 * The comparison function takes two values (casted to a void*) as it's input.
 * but must only compare keys. This is both natural, and demanded by the get/get_const functions, which only
 * take keys but must be able to compare them to elements in the map.
 */
vul_hash_map *vul_map_create( u32 initial_size, f32 load_factor,
                              u32 key_size, u32 value_size,
                              vul_hash_function hash_function, 
                              int (*comparator)( const void* a, const void *b ), 
                              void *( *allocator )( size_t size ), 
                              void( *deallocator )( void *ptr ) );
/**
 * Inserts the given value and key into the map. Copies both values.
 * Lifetime is as long as it is in the map, and value pointer is constant
 * for lifetime.
 *
 * Entries are not overwritten; if the key is the same as another entry, they
 * are simply both in the map, which is probably not desired. To overwrite a value,
 * alter it by changing the value point to by vul_map_get for the key instead!
 */
void vul_map_insert( vul_hash_map *map, void *key, void *value );
/**
 * Deletes the element for the given key from the map. Returns true if
 * the element was deleted, false if none was found for the given key,
 */
b32 vul_map_remove( vul_hash_map *map, const void *key );
/**
 * Returns a pointer to the value for the given key, or NULL if no
 * element is found.
 */
void *vul_map_get( vul_hash_map *map, void *key );
/**
 * Returns a pointer to the value for the given key, or NULL if no
 * element is found.
 */
const void *vul_map_get_const( vul_hash_map *map, void *key );
/**
 * Destroys the given has map, deallocating all it's used memory.
 * Also free's the memory of the elements within it, thus ending the lifetime
 * of value-pointers gotten through vul_map_get(_const).
 */
void vul_map_destroy( vul_hash_map *map );
/**
 * Iterate over all elements in the map. Useful for destruction etc.
 * @NOTE: If func alters the list (the element e, not the data
 * e contains), behaviour is undefined!
 * The data parameter is passed through to func.
 */
void vul_map_iterate( vul_hash_map *map, void ( *func )( vul_map_element *e, void *data ), void *data );

#ifdef __cplusplus
}
#endif

#ifndef VUL_TYPES_H
#undef u32
#undef f32
#undef s32
#undef b32
#endif

#endif // VUL_HASH_MAP_H

#ifdef VUL_DEFINE

#ifndef VUL_TYPES_H
#define u32 uint32_t
#define s32 int32_t
#define f32 float
#define b32 u32
#endif

#ifdef __cplusplus
extern "C" {
#endif

//--------------------
// Internal helpers

static u32 vul__map_hash_internal( vul_hash_map *map, const void *key )
{
   u32 h;
   
   // Hash
   h = map->hash( key, map->key_size );

   // Use most significant bit to indicate dead elements
   h &= 0x7fffffff;

   // Use 0 to indicate never-used slot
   h |= h == 0;

   return h;
}

static u32 vul__map_probe_distance( u32 hash, u32 slot, u32 size, u32 mask ) {
   return ( u32 )( ( s32 )slot + ( s32 )size - ( s32 )( hash & mask ) ) & mask;
}

static void vul__map_insert_helper( vul_hash_map *map, u32 hash, void *key, void *value )
{
   u32 pos, dist, probe_dist;
   void *tkey, *tvalue;
   u32 thash;

   pos = hash & map->mask;
   while( 1 ) {
      if( map->hashes[ pos ] == 0 ) {
         // Slot was never used, use it
         map->entries[ pos ].key = key;
         map->entries[ pos ].value = value;
         map->hashes[ pos ] = hash;
         return;
      }

      // If the item in its place has probed less than
      probe_dist = vul__map_probe_distance( map->hashes[ pos ], pos, map->size, map->mask );
      if( probe_dist < dist ) {
         if( ( map->hashes[ pos ] >> 31 ) != 0 ) {
            // Slot has been vacated, use it
            map->entries[ pos ].key = key;
            map->entries[ pos ].value = value;
            map->hashes[ pos ] = hash;
            return;
         }

         // The other element has probed less, swap and keep going
         tkey = map->entries[ pos ].key;
         tvalue = map->entries[ pos ].value;
         thash = map->hashes[ pos ];
         map->entries[ pos ].key = key;
         map->entries[ pos ].value = value;
         map->hashes[ pos ] = hash;
         key = tkey;
         value = tvalue;
         hash = thash;

         dist = probe_dist;
      }
      pos = ( pos + 1 ) & map->mask;
      ++dist;
   }
}

static s32 vul__map_lookup_index( vul_hash_map *map, const void *key )
{
   u32 hash, pos, dist;
   
   hash = vul__map_hash_internal( map, key );
   pos = hash & map->mask;
   dist = 0;

   while( 1 ) {
      if( map->hashes[ pos ] == 0 ) {
         return -1;
      } else if( dist < vul__map_probe_distance( map->hashes[ pos ], pos, map->size, map->mask ) ) {
         return -1;
      } else if( map->hashes[ pos ] == hash && 
                 map->comparator( map->entries[ pos ].key, key ) == 0 ) {
         return pos;
      }
      pos = ( pos + 1 ) & map->mask;
      ++dist;
   }
}

static void vul__map_grow( vul_hash_map *map )
{
   vul_map_element *oentries;
   u32 osize, i, h;
   u32 *ohashes;

   oentries = map->entries;
   osize = map->size;
   ohashes = map->hashes;

   map->size *= 2;

   map->entries = ( vul_map_element* )malloc( sizeof( vul_map_element ) * map->size );
   map->hashes = ( u32* )malloc( sizeof( u32 ) * map->size );

   assert( map->entries );
   assert( map->hashes );

   map->mask = map->size - 1;

   // Mark all as free
   memset( map->hashes, 0, sizeof( u32 ) * map->size );

   // Copy over old elements
   for( i = 0; i < osize; ++i ) {
      h = ohashes[ i ];
      if( h != 0 && ( h >> 31 ) == 0 ) {
         vul__map_insert_helper( map, h, oentries[ i ].key, oentries[ i ].value ); // Copy the pointers internally
      }
   }

   map->deallocator( oentries );
   map->deallocator( ohashes );
}

//-------------
// Public API

vul_hash_map *vul_map_create( u32 initial_size, f32 load_factor,
                              u32 key_size, u32 value_size,
                              vul_hash_function hash_function, 
                              int (*comparator)( const void* a, const void *b ), 
                              void *( *allocator )( size_t size ), 
                              void( *deallocator )( void *ptr ) )
{
   vul_hash_map *map;

   map = ( vul_hash_map* )allocator( sizeof( vul_hash_map ) );
   assert( map != NULL ); // Make sure allocation didn't fail
   map->hash = hash_function;
   map->comparator = comparator;
   // @TODO(thynn): Support alignement in allocators?
   map->entries = ( vul_map_element* )malloc( sizeof( vul_map_element ) * initial_size );
   map->hashes = ( u32* )malloc( sizeof( u32 ) * initial_size );
   assert( map->entries != NULL );
   assert( map->hashes != NULL );

   map->key_size = key_size;
   map->value_size = value_size;

   map->count = 0;
   map->size = initial_size;
   map->mask = map->size - 1;
   map->factor = load_factor;

   map->allocator = allocator;
   map->deallocator = deallocator;
   
   memset( map->hashes, 0, sizeof( u32 ) * initial_size );
#ifdef VUL_DEBUG
   // If debug, zero the entries as well
   memset( map->entries, 0, sizeof( vul_map_element ) * initial_size );
#endif

   return map;
}

void vul_map_insert( vul_hash_map *map, void *key, void *value )
{
   u32 threshold;
   void *keycopy, *valuecopy;

   keycopy = malloc( map->key_size );
   valuecopy = malloc( map->value_size );

   assert( keycopy );
   assert( valuecopy );

   memcpy( keycopy, key, map->key_size );
   memcpy( valuecopy, value, map->value_size );

   threshold = ( u32 )( ( f32 )map->size * map->factor );
   if( ++map->count >= threshold ) {
      vul__map_grow( map );
   }
   vul__map_insert_helper( map, vul__map_hash_internal( map, key ), keycopy, valuecopy );
}

b32 vul_map_remove( vul_hash_map *map, const void *key )
{
   u32 idx;

   idx = vul__map_lookup_index( map, key );
   
   if( idx == -1 ) {
      // No element to delete, signal failure
      return 0;
   }

   // Delete the entry
   map->deallocator( map->entries[ idx ].key );
   map->deallocator( map->entries[ idx ].value );
#ifdef VUL_DEBUG
   // Set memory to zero to make use-after-free bugs easier to find
   memset( &map->entries[ idx ], 0, sizeof( vul_map_element ) );
#endif
 
   // Mark as deleted
   map->hashes[ idx ] |= 0x80000000;
   --map->count;
   return 1;
}

void *vul_map_get( vul_hash_map *map, void *key )
{
   u32 idx;

   idx = vul__map_lookup_index( map, key );
   return idx != -1 ? map->entries[ idx ].value : NULL;
}

const void *vul_map_get_const( vul_hash_map *map, void *key )
{
   return vul_map_get( map, key );
}

void vul_map_destroy( vul_hash_map *map )
{
   u32 i;

   for( i = 0; i < map->size; ++i ) {
      if( map->hashes[ i ] != 0 && ( map->hashes[ i ] >> 31 ) == 0 ) {
         map->deallocator( map->entries[ i ].key );
         map->deallocator( map->entries[ i ].value );
      }
   }
#ifdef VUL_DEBUG
   memset( map->entries, 0, sizeof( vul_map_element ) * map->size );
   memset( map->hashes, 0, sizeof( u32 ) * map->size );
#endif
   map->deallocator( map->entries );
   map->deallocator( map->hashes );
#ifdef VUL_DEBUG
   map->entries = 0;
   map->hashes = 0;
#endif
   map->deallocator( map );
#ifdef VUL_DEBUG
   map = 0;
#endif
}

void vul_map_iterate( vul_hash_map *map, void ( *func )( vul_map_element *e, void *data ), void *data )
{
   u32 i;
   for( i = 0; i < map->size; ++i )
   {
      // @TODO(thynn): We need to make sure it's not zero and not deleted before entering it!
      // whcih means we need to iterate over size, not count!
      if( map->hashes[ i ] != 0 && ( map->hashes[ i ] >> 31 ) == 0 ) {
         func( &map->entries[ i ], data );
      }
   }
}

#ifdef __cplusplus
}
#endif

#ifndef VUL_TYPES_H
#undef b32
#undef f32
#undef s32
#undef u32
#endif

#endif // VUL_DEFINE

