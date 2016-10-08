/*
* Villains' Utility Library - Thomas Martin Schmid, 2016. Public domain?
*
* This file describes a generic priority queue, implemented as a
* fibonacci heap.
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
#ifndef VUL_PRIORITY_HEAP_H
#define VUL_PRIORITY_HEAP_H

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>

#ifndef VUL_TYPES_H
#include <stdint.h>
#define u32 uint32_t
#define b32 uint32_t
#endif

/**
 * The internal node representation.
 */
typedef struct vul__fheap_element {
   u32 degree;
   b32 marked;

   struct vul__fheap_element *next;
   struct vul__fheap_element *prev;

   struct vul__fheap_element *parent;

   struct vul__fheap_element *child;

   void *data;
} vul__fheap_element;

/**
 * The priotity heap structure.
 */
typedef struct vul_priority_heap {
   vul__fheap_element *min_element;

   u32 size;
   u32 element_size;

   int ( *comparator )( void *a, void *b );

   void *( *allocator )( size_t size );
   void ( *deallocator )( void *ptr );
} vul_priority_heap;


#ifdef __cplusplus
extern "C" {
#endif
//-----------------------
// The external API
//

/**
 * Create a new priority heap.
 * Takes a comparison function for elements and
 * memory management functions.
 */
vul_priority_heap *vul_priority_heap_create( u32 element_size, 
                                             int ( *comparison_func )( void* a, void* b ),
                                             void *( *allocator )( size_t size ),
                                             void ( *deallocator )( void *ptr ) );
/**
 * Returns true if the heap is empty 
 */
b32 vul_priority_heap_is_empty( vul_priority_heap *heap );
/**
 * Destroys a priority heap
 */
void vul_priority_heap_destroy( vul_priority_heap *heap );
/**
 * Pushes an element into the heap. Copies the data.
 */
void vul_priority_heap_push( vul_priority_heap *heap, void *data );
/**
 * Pops the next element out of the heap and copies it to data_out
 */
void vul_priority_heap_pop( vul_priority_heap *heap, void *data_out );
/**
 * Peeks at the next element of the heap
 */
void *vul_priority_heap_peek( vul_priority_heap *heap );
/**
 * Returns the number of elements in the heap.
 */
u32 vul_priority_heap_size( vul_priority_heap *heap );
/**
 * Merges two heaps into a new one. Destroys the old heaps!
 */
vul_priority_heap *vul_priority_heap_merge( vul_priority_heap *heap1, vul_priority_heap *heap2 );


//----------------------------------
// Internal helper functions
//

/**
 * Merge two parts of the heap.
 */
static vul__fheap_element *vul__fheap_merge_lists( vul_priority_heap *heap, 
                                                   vul__fheap_element *e1, 
                                                   vul__fheap_element *e2 );
/**
 * Adds an element to the heap, while maintaining the sorted invariant.
 */
static void *vul__fheap_enqueue( vul_priority_heap *heap, void *data );
/**
 * Pops the first element from the heap and reorders the heap as needed
 * to maintain the sorted invariant.
 */
static vul__fheap_element *vul__fheap_dequeue_min( vul_priority_heap *heap );
/**
 * Cuts the given node from the heap.
 */
static void vul__fheap_cut_node( vul_priority_heap *heap, vul__fheap_element *element );
/**
 * Delete an arbitraty element from the heap.
 * @NOTE(thynn): Not part of the API since the heal element type isn't exposed,
 * but may be useful, so left in. To use, just remove the static to expose it.
 */
static void vul__fheap_delete( vul_priority_heap *heap, vul__fheap_element *element );

#ifdef __cplusplus
}
#endif

#ifndef VUL_TYPES_H
#undef u32
#undef b32
#endif

#endif // VUL_PRIORITY_HEAP_H

#ifdef VUL_DEFINE

#ifndef VUL_TYPES_H
#define u32 uint32_t
#define b32 uint32_t
#endif

#ifdef __cplusplus
extern "C" {
#endif

static vul__fheap_element *vul__fheap_merge_lists( vul_priority_heap *heap, 
                                                   vul__fheap_element *e1, 
                                                   vul__fheap_element *e2 )
{
   if( e1 == NULL && e2 == NULL ) {
      return NULL;
   } else if( e1 != NULL && e2 == NULL ) {
      return e1;
   } else if( e1 == NULL && e2 != NULL ) {
      return e2;
   } else {
      vul__fheap_element *e1n = e1->next;
      e1->next = e2->next;
      e1->next->prev = e1;
      e2->next = e1n;
      e2->next->prev = e2;

      return heap->comparator( e1->data, e2->data ) < 0 ? e1 : e2;
   }
}

static void *vul__fheap_enqueue( vul_priority_heap *heap, void *data )
{
   vul__fheap_element *element = ( vul__fheap_element* )heap->allocator( sizeof( vul__fheap_element ) );
   
   memset( element, 0, sizeof( vul__fheap_element ) );
   element->data = data;
   element->next = element;
   element->next = element;

   ++heap->size;
   
   heap->min_element = vul__fheap_merge_lists( heap, heap->min_element, element );

   return element;
}

static vul__fheap_element *vul__fheap_dequeue_min( vul_priority_heap *heap )
{
   u32 tovisitsize, treesize;
   vul__fheap_element *min_element, *current, **tovisit, **tree, *first;

   assert( heap->size != 0 );
   
   heap->size -= 1;

   min_element = heap->min_element;

   if( min_element->next == min_element ) {
      heap->min_element = NULL;
   } else {
      heap->min_element->prev->next = heap->min_element->next;
      heap->min_element->next->prev = heap->min_element->prev;
      heap->min_element = heap->min_element->next;
   }

   if( min_element->child != NULL ) {
      vul__fheap_element *current = min_element->child;
      do {
         current->parent = NULL;
         current = current->next;
      } while( current != min_element->child );
   }

   heap->min_element = vul__fheap_merge_lists( heap, heap->min_element, min_element->child );

   if( heap->min_element == NULL ) return min_element;

   /* Count tovisit and calculate tree sizes */
   current = heap->min_element;
   tovisitsize = current == NULL ? 0 : 1;
   treesize = ( u32 )log2( heap->size ) + 1; // The guaranteed upper bound on degree is log( n )
   first = current;
   while( current->next != first ) {
      ++tovisitsize;
      current = current->next;
    }

   tovisit = ( vul__fheap_element** )heap->allocator( tovisitsize * sizeof( vul__fheap_element* ) );
   tree = ( vul__fheap_element** )heap->allocator( treesize * sizeof( vul__fheap_element* ) );

   /* Fill the tovisit array */
   current = heap->min_element;
   for( u32 i = 0; i < tovisitsize; ++i ) {     
      tovisit[ i ] = current;
      current = current->next;
   }

   /* Initialize the tree array to empty */
   for( u32 i = 0; i < treesize; ++i ) {
      tree[ i ] = NULL;
   }

    for( u32 i = 0; i < tovisitsize; ++i ) {
      current = tovisit[ i ];
      while( 1 ) {
         if( tree[ current->degree ] == NULL ) {
            tree[ current->degree ] = current;
            break;
         }

         vul__fheap_element *other = tree[ current->degree ];
         tree[ current->degree ] = NULL;

         vul__fheap_element *mn = ( heap->comparator( other->data, current->data ) < 0 ) ? other : current;
         vul__fheap_element *mx = ( mn == other ) ? current : other;

         mx->next->prev = mx->prev;
         mx->prev->next = mx->next;

         mx->next = mx;
         mx->prev = mx;
         mn->child = vul__fheap_merge_lists( heap, mn->child, mx );

         mx->parent = mn;
         mx->marked = 0;
         ++mn->degree;

         current = mn;
      }

      if( heap->comparator( current->data, heap->min_element->data ) <= 0 ) {
         heap->min_element = current;
      }
    }

    heap->deallocator( tree );
    heap->deallocator( tovisit );

    return min_element;
}

static void vul__fheap_cut_node( vul_priority_heap *heap, vul__fheap_element *element )
{
   element->marked = 0;

   if( element->parent == NULL ) return;

   if( element->next != element ) {
      element->next->prev = element->prev;
      element->prev->next = element->next;
   }

   if( element->parent->child == element ) {
      if( element->next != element ) {
         element->parent->child = element->next;
      } else {
         element->parent->child = NULL;
      }
   }

   --element->parent->degree;

   element->prev = element;
   element->next = element;

   heap->min_element = vul__fheap_merge_lists( heap, heap->min_element, element );

   if( element->parent->marked ) {
      vul__fheap_cut_node( heap, element->parent );
   } else {
      element->parent->marked = 1;
   }

   element->parent = NULL;
}

static void vul__fheap_delete( vul_priority_heap *heap, vul__fheap_element *element )
{
   /* If there's a parent, cut it */
   if( element->parent != NULL ) {
      vul__fheap_cut_node( heap, element );
   }

   /* Make it the smallest node */
   heap->min_element = element;

   /* Then dequeue the smallest element, and don't return it */
   vul__fheap_dequeue_min( heap );
}

vul_priority_heap *vul_priority_heap_create( u32 element_size,
                                             int ( *comparison_func )( void* a, void* b ),
                                             void *( *allocator )( size_t size ),
                                             void ( *deallocator )( void *ptr ) )
{
   vul_priority_heap *heap;

   heap = ( vul_priority_heap* )allocator( sizeof( vul_priority_heap ) );
   assert( heap );
   heap->element_size = element_size;
   heap->comparator = comparison_func;
   heap->allocator = allocator;
   heap->deallocator = deallocator;
   heap->min_element = NULL;
   heap->size = 0;

   return heap;
}

b32 vul_priority_heap_is_empty( vul_priority_heap *heap )
{
   return heap->size == 0 ? 1 : 0;
}

void vul_priority_heap_destroy( vul_priority_heap *heap )
{
   // @TODO(thynn): Do this more efficiently:
   // We have no need of preseved order on every delete and should
   // just march each heap, deleting everything we meet along the way!
   while( !vul_priority_heap_is_empty( heap ) ) {
      vul__fheap_element *e = vul__fheap_dequeue_min( heap );
      heap->deallocator( e->data );
      heap->deallocator( e );
   }
   heap->deallocator( heap );
}

void vul_priority_heap_push( vul_priority_heap *heap, void *data )
{
   void *data_copy = heap->allocator( heap->element_size );
   memcpy( data_copy, data, heap->element_size );

   vul__fheap_enqueue( heap, data_copy );
}

void vul_priority_heap_pop( vul_priority_heap *heap, void *data_out )
{
   vul__fheap_element *el = vul__fheap_dequeue_min( heap );
   
   memcpy( data_out, el->data, heap->element_size );

   heap->deallocator( el->data );
   heap->deallocator( el );
}

void *vul_priority_heap_peek( vul_priority_heap *heap )
{
   return heap->min_element == NULL ? NULL : heap->min_element->data;
}

u32 vul_priority_heap_size( vul_priority_heap *heap )
{
   return heap->size;
}

vul_priority_heap *vul_priority_heap_merge( vul_priority_heap *heap1, vul_priority_heap *heap2 )
{
   vul_priority_heap *res = ( vul_priority_heap* )heap1->allocator( sizeof( vul_priority_heap ) );

   res->min_element = vul__fheap_merge_lists( heap1, heap1->min_element, heap2->min_element );
   res->size = heap1->size + heap2->size;

   heap1->min_element = NULL;
   heap1->size = 0;
   heap1->deallocator( heap1 );

   heap2->min_element = NULL;
   heap2->size = 0;
   heap2->deallocator( heap2 );

   return res;
}

#ifdef __cplusplus
}
#endif

#ifndef VUL_TYPES_H
#undef u32
#undef b32
#endif

#endif
