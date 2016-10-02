/*
 * Villains' Utility Library - Thomas Martin Schmid, 2016. Public domain?
 *
 * This file contains tests for vul_priority_heap.h
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
#ifndef VUL_TEST_HASH_MAP_H
#define VUL_TEST_HASH_MAP_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>

//----------------------
// The actual tests
//

#define TEST( expr ) if( !( expr ) ) {\
   fprintf( stderr, #expr );\
   assert( 0 );\
   exit( 1 );\
}

#define VUL_DEFINE
#include "../vul_map_robin_hood.h"

typedef struct vul_test_map_key {
   char *str;
   size_t len;
} vul_test_map_key;

int vul_test_map_compare_string( const void *a, const void *b )
{
   const vul_test_map_key *ca, *cb;
   size_t diff;

   ca = ( const vul_test_map_key* )a;
   cb = ( const vul_test_map_key* )b;

   diff = ca->len - cb->len;

   return diff != 0 ? diff : strcmp( ca->str, cb->str );
}

uint32_t vul_test_map_hash( const void *key, uint32_t len )
{
   vul_test_map_key *k;
   size_t i;
   uint32_t h;

   h = 0;
   k = ( vul_test_map_key* )key;
   for( i = 0; i < k->len; ++i ) {
      h += k->str[ i ];
   }

   return h;
}

void vul_test_map_iterate( vul_map_element *e )
{
   vul_test_map_key*k;
   size_t *v;

   k = ( vul_test_map_key* )e->key;
   v = ( size_t* )e->value;

   TEST( k->len == *v );
}

int main( )
{
   vul_hash_map *map;
   vul_test_map_key k, k2;
   size_t v, i;

   map = vul_map_create( 4, 0.8, sizeof( vul_test_map_key), sizeof( size_t ), 
                         vul_test_map_hash, vul_test_map_compare_string,
                         malloc, free );

   // Test the most basic insert
   k.len = 4;
   k.str = "test";
   v = 127;
   vul_map_insert( map, &k, &v );

   // Test that the get works (and that we actually copied the value)
   v = 0;
   TEST( *( ( size_t* )vul_map_get( map, &k ) ) == 127 );
   TEST( *( ( const size_t* )vul_map_get_const( map, &k ) ) == 127 );

   // Test that get returns null if not found
   k.len = 0;
   k.str = 0;
   TEST( vul_map_get( map, &k ) == NULL );
   TEST( vul_map_get_const( map, &k ) == NULL );
   
   // Overwrite the elmenent by writing to the pointer given by get
   k.len = 4;
   k.str = "test";
   *( ( size_t* )vul_map_get( map, &k ) ) = 208;
   TEST( *( ( size_t* )vul_map_get( map, &k ) ) == 208 );
   
   // Remove the element
   vul_map_remove( map, &k );
   // Check that it was removed
   TEST( vul_map_get( map, &k ) == NULL );
   TEST( vul_map_get_const( map, &k ) == NULL );
   
   // Insert 10 elements
   for( i = 0; i < 10; ++i ) {
      k.len = i+1;
      k.str = "abcdefghijk";
      v = i+1;
      vul_map_insert( map, &k, &v );
   }
   
   // Test iteration
   vul_map_iterate( map, vul_test_map_iterate );

   // Finally, test collissions!
   vul_map_destroy( map );

   map = vul_map_create( 4, 0.8, sizeof( vul_test_map_key), sizeof( size_t ), 
                         vul_test_map_hash, vul_test_map_compare_string,
                         malloc, free );

   k.len = 2;
   k.str = "ab";
   v = 1;
   vul_map_insert( map, &k, &v );
   k2.len = 2;
   k2.str = "ba";
   v = 2;
   vul_map_insert( map, &k2, &v );

   TEST( *( ( size_t* )vul_map_get( map, &k ) ) == 1 );
   TEST( *( ( size_t* )vul_map_get( map, &k2 ) ) == 2 );

   vul_map_remove( map, &k2 );
   TEST( *( ( size_t* )vul_map_get( map, &k ) ) == 1 );
   TEST( vul_map_get( map, &k2 ) == NULL );

   vul_map_destroy( map );

   return 0;
}
#endif
