/*
 * Villains' Utility Library - Thomas Martin Schmid, 2017. Public domain?
 *
 * This file describes a queue constructed by dynamically allocated and
 * freed arrays linked together in a list. The size of each block, in bytes
 * can be specified by defining VUL_QUEUE_BUFFER_BYTE_SIZE; it is recommended
 * to make this a multiple of the queue's elements' size to not waste memory.
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
#ifndef VUL_QUEUE_H
#define VUL_QUEUE_H

#include <stdlib.h>
#include <string.h>

#ifndef VUL_DATATYPES_CUSTOM_ASSERT
#include <assert.h>
#define VUL_DATATYPES_CUSTOM_ASSERT assert
#endif

#ifndef VUL_LINKED_LIST_H
#include "vul_linked_list.h"
#endif

#ifndef VUL_TYPES_H
#include <stdint.h>
#define u32 uint32_t
#define b32 uint32_t
#define s32 int32_t
#define u8 uint8_t
#endif

#ifndef VUL_QUEUE_BUFFER_BYTE_SIZE
   #define VUL_QUEUE_BUFFER_BYTE_SIZE 1024
#endif

typedef struct vul__queue_buffer {
   u8 data[ VUL_QUEUE_BUFFER_BYTE_SIZE ];
   u32 first, next;
   u32 buffer_id; // incrementally unique, used to sort in the dummy
} vul__queue_buffer;

/**
 * If defined, the functions are defined and not just declared. Only do this in _one_ c/cpp file!
 */
//#define VUL_DEFINE

typedef struct vul_queue
{
   vul_list_element *first_root, *next_root; // List of vul__queue_buffer objects
   u32 data_size;
   u32 first;
   u32 next;

   /* Memory management functions */
   void *( *allocator )( size_t size );
   void( *deallocator )( void *ptr );
} vul_queue;

#ifdef __cplusplus
extern "C" {
#endif
/**
 * Comparison function for the linked list used internally.
 */
s32 vul__queue_comparator( void *a, void *b );
/**
 * Returns the size of the queue.
 */
u32 vul_queue_size( vul_queue *q );
/**
 * Returns true if the queue is empty
 */
b32 vul_queue_is_empty( vul_queue *q );
/**
 * Creates a new queue. Takes the size of an element as an arguemnt.
 * and the factor by which to grow it when needed.
 */
vul_queue *vul_queue_create( u32 element_size, 
                             void *( *allocator )( size_t size ),
                             void( *deallocator )( void *ptr ) );
/**
 * Pushes an element to the back of the queue. If the queue is full, this resizes it.
 */
void vul_queue_push( vul_queue *q, void *data );
/**
 * Pops an element from the front of the queue and returns it in
 * out.
 */
void vul_queue_pop( vul_queue *q, void *out );
/**
 * Peeks at the front element of the queue.
 * @NOTE: This does not copy the data back, meaning it is only
 * guaranteed to be unaltered as long as the queue is not modified.
 * If you want the data to persisit beyond the next q->push you must copy
 * the data youself.
 */
void *vul_queue_peek( vul_queue *q );
/**
 * Destroys the given queue and deallocates it's memory
 */
void vul_queue_destroy( vul_queue *q );

#ifdef __cplusplus
}
#endif

#ifndef VUL_TYPES_H
#undef u32
#undef b32
#undef s32
#undef u8
#endif

#endif // VUL_QUEUE_H

#ifdef VUL_DEFINE

#ifndef VUL_TYPES_H
#define u32 uint32_t
#define b32 uint32_t
#define s32 int32_t
#define u8 uint8_t
#endif

#ifdef __cplusplus
extern "C" {
#endif

s32 vul__queue_comparator( void *a, void *b )
{
   vul__queue_buffer *ba, *bb;

   ba = ( vul__queue_buffer* )a;
   bb = ( vul__queue_buffer* )b;

   return ba->buffer_id - bb->buffer_id;
}

u32 vul_queue_size( vul_queue *q )
{
   vul_list_element *e;
   vul__queue_buffer *buf;
   u32 count;

   VUL_DATATYPES_CUSTOM_ASSERT( q );
   if( !q->first_root || !q->next_root ) {
      return 0;
   }
   count = 0;
   e = q->first_root;
   while( e != q->next_root ) {
      buf = ( vul__queue_buffer* )e->data;
      count += buf->next - buf->first;
      e = e->next;
   }
   buf = ( vul__queue_buffer* )e->data;
   count += buf->next - buf->first;

   return count;
}

b32 vul_queue_is_empty( vul_queue *q )
{
   vul__queue_buffer *bf, *bn;

   VUL_DATATYPES_CUSTOM_ASSERT( q );
   if( !q->first_root || !q->next_root ) {
      return 1;
   }
   if( q->first_root != q->next_root ) {
      return 0;
   }
   bf = ( vul__queue_buffer* )q->first_root->data;
   bn = ( vul__queue_buffer* )q->next_root->data;

   return bn->next - bf->first == 0;
}

vul_queue *vul_queue_create( u32 element_size,
                             void *( *allocator )( size_t size ),
                             void( *deallocator )( void *ptr ) )
{
   vul_queue *ret;

   ret = ( vul_queue* )allocator( sizeof( vul_queue ) );
   ret->data_size = element_size;
   ret->first = 0;
   ret->next = 0;
   ret->first_root = NULL;
   ret->next_root = NULL;

   ret->allocator = allocator;
   ret->deallocator = deallocator;

   return ret;
}

void vul_queue_push( vul_queue *q, void *data )
{
   vul__queue_buffer *buf, nbuf;

   VUL_DATATYPES_CUSTOM_ASSERT( q );
   // If no next buffer, create it
   nbuf.next = 0;
   nbuf.first = 0;
   if( !q->next_root ) {
      // Create the new next_root
      nbuf.buffer_id = 0;
      q->next_root = vul_list_insert( NULL, 
                                      &nbuf, 
                                      sizeof( vul__queue_buffer ), 
                                      vul__queue_comparator,
                                      q->allocator );
      // If first root is null, it should be this
      if( !q->first_root ) {
         q->first_root = q->next_root;
      }
   }
   // If no more room, create a new buffer
   buf = ( vul__queue_buffer* )q->next_root->data;
   if( buf->next * q->data_size > VUL_QUEUE_BUFFER_BYTE_SIZE - q->data_size )
   {
      nbuf.buffer_id = buf->buffer_id + 1;
      q->next_root = vul_list_insert( q->next_root, 
                                      &nbuf, 
                                      sizeof( vul__queue_buffer ), 
                                      vul__queue_comparator,
                                      q->allocator );
      buf = ( vul__queue_buffer* )q->next_root->data;
   }
   // Insert it
   memcpy( buf->data + ( buf->next++ * q->data_size ), data, q->data_size );
}

void vul_queue_pop( vul_queue *q, void *out )
{
   vul__queue_buffer *buf;

   VUL_DATATYPES_CUSTOM_ASSERT( q );
   if( q->first_root ) {
      buf = ( vul__queue_buffer* )q->first_root->data;
      memcpy( out, buf->data + ( buf->first * q->data_size ), q->data_size );
      ++buf->first;
      if( buf->first * q->data_size > VUL_QUEUE_BUFFER_BYTE_SIZE - q->data_size ) {
         // Buffer is empty, free it
         if( q->first_root->next != NULL ) {
            if( q->next_root == q->first_root ) {
               q->next_root = q->first_root->next;
            }
            q->first_root = q->first_root->next;
            q->deallocator( q->first_root->prev->data );
            q->deallocator( q->first_root->prev );
            q->first_root->prev = NULL;
         } else {
            q->deallocator( q->first_root->data );
            q->deallocator( q->first_root );
            if( q->next_root == q->first_root ) {
               q->next_root = NULL;
            }
            q->first_root = NULL;
         }
      }
   }
}

void *vul_queue_peek( vul_queue *q )
{
   vul__queue_buffer *buf;

   VUL_DATATYPES_CUSTOM_ASSERT( q );
   if( q->first_root ) {
      buf = ( vul__queue_buffer* )q->first_root->data;
      return ( buf->data + ( buf->first * q->data_size ) );
   }
   return NULL;
}

void vul_queue_destroy( vul_queue *q )
{
   if( q ) {
      if( q->first_root ) {
         vul_list_destroy( q->first_root, q->deallocator );
      }
      q->deallocator( q );
   }
}

#ifdef __cplusplus
}
#endif

#ifndef VUL_TYPES_H
#undef u32
#undef b32
#undef s32
#undef u8
#endif

#endif // VUL_DEFINE
