/*
 * Villains' Utility Library - Thomas Martin Schmid, 2016. Public domain?
 *
 * This file describes a singly linked skip list. The current implementation is
 * not indexable.
 *
 * Define VUL_DEFINE in exactly one compilation unit.
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
#ifndef VUL_SKIP_LIST_H
#define VUL_SKIP_LIST_H

#include <stdlib.h>
#include <assert.h>
#include <string.h>

#ifndef VUL_TYPES_H
#include <stdint.h>
#define u32 uint32_t
#define s32 int32_t
#define f32 float
#endif

typedef struct vul_skip_list_element {
   void *data;
   u32 levels;
   struct vul_skip_list_element** nexts;
} vul_skip_list_element;

typedef struct vul_skip_list {
   u32 levels;
   vul_skip_list_element **heads;
   u32 data_size;
   s32 (*comparator)( void *a, void *b );

   /* Memory management functions */
   void *( *allocator )( size_t size );
   void( *deallocator )( void *ptr );
} vul_skip_list;

#ifdef __cplusplus
extern "C" {
#endif
/**
 * Simulates a coin flip; .5 probability of 0, same for 1.
 * Currently uses rand, because we need it to be fast, not secure.
 */
static s32 vul__skip_list_coin_flip( );
/**
 * Creates a new skip list. Takes the size of an element and a comparison function
 * as arguments and returns an empty list.
 */
vul_skip_list *vul_skip_list_create( u32 data_size, 
                                     s32( *comparator )( void *a, void *b ),
                                     void *( *allocator )( size_t size ),
                                     void( *deallocator )( void *ptr ) );
/**
 * Finds the element in the list which matches the given data key. If
 * no match is found, returns NULL.
 */
vul_skip_list_element *vul_skip_list_find( vul_skip_list *list, void *data );
/**
 * Removes the given element from the list, and deletes it.
 */
void vul_skip_list_remove( vul_skip_list *list, vul_skip_list_element *e );
/**
 * Inserts the given data into the list, while keeping it sorted and stable.
 * Copies the given data (of given size) to the element.
 * Returns a pointer to the new element.
 */
vul_skip_list_element *vul_skip_list_insert( vul_skip_list *list, void *data );
/**
 * Returns the length of the given skip list.
 */
unsigned int vul_skip_list_size( vul_skip_list *list );
/**
 * General purpose iteration over a list.
 * Executes the given function for each element in the list.
 * @NOTE: If func alters the list, behaviour is undefined!
 */
void vul_skip_list_iterate( vul_skip_list *list, void( *func )( vul_skip_list_element *e ) );
/**
 * Deletes a the given list and all elements in it.
 */
void vul_skip_list_destroy( vul_skip_list *list );
/**
 * Creates a copy of the given skip list. It does NOT maintain the same lanes,
 * meaning it copies the data and maintains the sorting invariant, but it may look
 * differently internally, and thus may perform slightly different.
 */
vul_skip_list *vul_skip_list_copy( vul_skip_list *src,
                                   void *( *allocator )( size_t size ),
                                   void( *deallocator )( void *ptr ) );
#ifdef __cplusplus
}
#endif

#ifndef VUL_TYPES_H
#undef u32
#undef f32
#undef s32
#endif

#endif // VUL_SKIP_LIST_H

#ifdef VUL_DEFINE

#ifndef VUL_TYPES_H
#define u32 uint32_t
#define s32 int32_t
#define f32 float
#endif

#ifdef __cplusplus
extern "C" {
#endif

static s32 vul__skip_list_coin_flip( )
{
   return ( ( f32 )rand( ) / ( f32 )RAND_MAX ) > 0.5;
}

vul_skip_list *vul_skip_list_create( u32 data_size,
                                     s32( *comparator )( void *a, void *b ),
                                     void *( *allocator )( size_t size ),
                                     void( *deallocator )( void *ptr ) )
{
   vul_skip_list *ret;
   s32 l;

   ret = ( vul_skip_list* )allocator( sizeof( vul_skip_list ) );
   assert( ret != NULL ); // Make sure allocation didn't fail
   ret->levels = 1;
   ret->heads = ( vul_skip_list_element** )allocator( sizeof( vul_skip_list_element* ) );
   assert( ret->heads != NULL ); // Make sure allocation didn't fail
   ret->comparator = comparator;
   ret->data_size = data_size;
   ret->allocator = allocator;
   ret->deallocator = deallocator;

   for( l = ret->levels - 1; l >= 0; --l )
   {
      ret->heads[ l ] = NULL;
   }

   return ret;
}

vul_skip_list_element *vul_skip_list_find( vul_skip_list *list, void *data )
{
   s32 l;
   vul_skip_list_element *e = NULL;

   if( list == NULL || list->heads == NULL ) return NULL;

   for( l = list->levels - 1; l >= 0; --l )
   {
      e = list->heads[ l ];
      while( list->comparator( e->data, data ) < 0 )
      {
         e = e->nexts[ l ];
      }
   }

   if( e && list->comparator( e->data, data ) == 0 ) return e;
   return NULL;
}

void vul_skip_list_remove( vul_skip_list *list, vul_skip_list_element *e )
{
   s32 l;
   vul_skip_list_element *p, *h;

   assert( e != NULL );

   // For every level
   for( l = list->levels - 1; l >= 0; --l )
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

   list->deallocator( e->data );
   list->deallocator( e->nexts );
   list->deallocator( e );
   // By setting to null we are much more likely to trigger asserts if used after deallocation
   e->data = NULL;
   e->nexts = NULL;
   e = NULL;
}

vul_skip_list_element *vul_skip_list_insert( vul_skip_list *list, void *data )
{
   s32 l;
   vul_skip_list_element **el, *e, *ret;

   if( list == NULL || list->heads == NULL ) return NULL;

   el = ( vul_skip_list_element** )list->allocator( list->levels * sizeof( vul_skip_list_element* ) );
   assert( el != NULL ); // Make sure allocation didn't fail

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
   ret = ( vul_skip_list_element* )list->allocator( sizeof( vul_skip_list_element ) );
   assert( ret != NULL ); // Make sure allocation didn't fail
   ret->data = list->allocator( list->data_size );
   assert( ret->data != NULL ); // Make sure allocation didn't fail
   memcpy( ret->data, data, list->data_size );
   ret->levels = 1;
   while( vul__skip_list_coin_flip( ) ) ++ret->levels;
   ret->nexts = ( vul_skip_list_element** )list->allocator( ret->levels * sizeof( vul_skip_list_element* ) );
   assert( ret->nexts != NULL ); // Make sure allocation didn't fail
   /* @TODO: THIS IS NOT WORKING YET! */ assert( false && "Skip lists aren't correctly implemented here." );
   // Insert it at bottom level, and each above if coinflip is false
   for( l = ret->levels - 1; l >= 0; --l )
   {
      ret->nexts[ l ] = el[ l ]->nexts[ l ];
   }
   return NULL; // @NOTE: never reached due to the above todo: we assert false!
}

unsigned int vul_skip_list_size( vul_skip_list *list )
{
   s32 c;
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

void vul_skip_list_iterate( vul_skip_list *list, void( *func )( vul_skip_list_element *e ) )
{
   vul_skip_list_element *e;

   e = list->heads[ 0 ];
   while( e != NULL ) {
      func( e );
      e = e->nexts[ 0 ];
   }
}

void vul_skip_list_destroy( vul_skip_list *list )
{
   vul_skip_list_element *e, *n;

   // Destroy all elements from the list at level 0
   e = list->heads[ 0 ];
   while( e != NULL )
   {
      n = e->nexts[ 0 ];

      list->deallocator( e->data );
      list->deallocator( e->nexts );
      list->deallocator( e );
      // By setting to null we are much more likely to trigger asserts if used after deallocation.
      e->data = NULL;
      e->nexts = NULL;
      e = NULL;

      e = n;
   }

   // Then deallocate the other heads
   list->deallocator( list->heads );
   // And finally, the list
   list->deallocator( list );

   // By setting to null we are much more likely to trigger asserts if used after deallocation.
   list->heads = NULL;
   list = NULL;
}

vul_skip_list *vul_skip_list_copy( vul_skip_list *src,
                                   void *( *allocator )( size_t size ),
                                   void( *deallocator )( void *ptr ) )
{
   vul_skip_list *dst;
   vul_skip_list_element *e;

   dst = vul_skip_list_create( src->data_size, src->comparator, allocator, deallocator );

   e = src->heads[ 0 ];
   while( e != NULL )
   {
      vul_skip_list_insert( dst, e->data );
      e = e->nexts[ 0 ];
   }

   return dst;
}

#ifdef __cplusplus
}
#endif

#ifndef VUL_TYPES_H
#undef u32
#undef f32
#undef s32
#endif

#endif // VUL_DEFINE
