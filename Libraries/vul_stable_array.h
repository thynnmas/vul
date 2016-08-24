/*
* Villains' Utility Library - Thomas Martin Schmid, 2016. Public domain?
*
* This file describes a vector class that never moves elements upon
* resize. It is implemented as an series of exponentially growing buffers
* into which pointers will remain constant. 
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
#ifndef VUL_STABLE_RESIZEABLE_ARRAY_H
#define VUL_STABLE_RESIZEABLE_ARRAY_H

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>

#ifndef VUL_TYPES_H
#include <stdint.h>
#define u8 uint8_t
#define u32 uint32_t
#define s32 int32_t
#define b32 uint32_t
#define f32 float
#endif

#define VUL_TRUE 1
#define VUL_FALSE 0

/**
* If enabled, iterate checks that the buffer address of each accessed
* element is unchanged (helps tracking down remove-during-iteration bugs).
* Slightly slower.
*/
//#define VUL_DEBUG

typedef struct vul_svector {
   u32 buffer_count;
   u32 size;
   u32 buffer_base_size;
   u32 element_size;
   void **buffers;

   /* Memory management functions */
   void *( *allocator )( size_t size );
   void ( *deallocator )( void *ptr );
} vul_svector;

#ifdef _cplusplus
extern "C" {
#endif
/**
* Create a stable vector, given the size of an element and the size of
* the base buffer (the smallest one). Every buffer after is double the size.
* Also takes in memory mamagement functions.
*/
vul_svector *vul_svector_create( u32 element_size,
                                 u32 buffer_base_size,
                                 void *( *allocator )( size_t size ),
                                 void ( *deallocator )( void *ptr ) );
/**
* Destroy the vector.
*/
void vul_svector_destroy( vul_svector *vec );
/**
* Frees the memory. Equivalent to destroying the vector
* and creating a new one.
*/
void vul_svector_freemem( vul_svector *vec );
/**
* Calculates the index of the buffer this element is in.
*/
static u32 vul__stable_vector_calculate_buffer_index( u32 base_size, u32 i );
/**
* Calculates the index within its buffer this element is at.
*/
static u32 vul__stable_vector_calculate_relative_index( u32 base_size, u32 bi, u32 i );
/**
* Append an empty object to the vector and return the pointer to it.
*/
void *vul_svector_append_empty( vul_svector *vec );
/**
* Append the given object to the vector by copying it into place
* and return the pointer to the new element.
*/
void *vul_svector_append( vul_svector *vec, void *element );
/**
* Return the element in buffer 'buffer' at index relative to that buffer 'index'.
*/
static void *vul__stable_vector_get_tiered( vul_svector *vec, u32 buffer, u32 index );
/**
* Get the element at the given index.
*/
void *vul_svector_get( vul_svector *vec, u32 index );
/**
* Removes an element, swapping the last element into it's place.
*/
void vul_svector_remove_swap( vul_svector *vec, u32 index );
/**
* General purpose iterator. Iterates over every element in the vector
* calling the given function for each one. The function takes a pointer to the element
* and it's absolute index in the vector as its first two arguments, then the func_data
* given to this function as a third parameter, allowing you to pass an environment or
* data into it.
*/
void vul_svector_iterate( vul_svector *vec, 
                          void( *func )( void *data, u32 index, void *func_data ), 
                          void *func_data );
/**
* Find the element equal to the given element according to the given
* comparison function.
*/
void *vul_svector_find( vul_svector *vec, 
                        void *element, s32( *comparator )( void *a, void *b ) );
/**
* Returns the number of elements in the vector.
*/
u32 vul_svector_size( vul_svector *vec );
/**
* Returns true if the vector is empty
*/
b32 vul_svector_is_empty( vul_svector *vec );
#ifdef _cplusplus
}
#endif

#ifndef VUL_TYPES_H
#undef u8
#undef u32
#undef s32
#undef b32
#undef f32
#endif

#endif // VUL_STABLE_RESIZEABLE_ARRAY_H

#ifdef VUL_DEFINE

#ifndef VUL_TYPES_H
#define u8 uint8_t
#define u32 uint32_t
#define s32 int32_t
#define b32 uint32_t
#define f32 float
#endif

#ifdef _cplusplus
extern "C" {
#endif

vul_svector *vul_svector_create( u32 element_size,
                                 u32 buffer_base_size,
                                 void *( *allocator )( size_t size ),
                                 void (*deallocator )( void *ptr ) )
{
   vul_svector *ret;

   assert( buffer_base_size != 0 );

   ret = ( vul_svector* )allocator( sizeof( vul_svector ) );
   assert( ret );
   ret->buffer_count = 0;
   ret->size = 0;
   ret->buffer_base_size = buffer_base_size;
   ret->element_size = element_size;
   ret->buffers = NULL;
   ret->allocator = allocator;
   ret->deallocator = deallocator;

   return ret;
}

void vul_svector_destroy( vul_svector *vec )
{
   u32 i;

   assert( vec );
   for( i = 0; i < vec->buffer_count; ++i ) {
      vec->deallocator( vec->buffers[ i ] );
   }
   vec->deallocator( vec->buffers );
   vec->deallocator( vec );
}

void vul_svector_freemem( vul_svector *vec )
{
   u32 i;

   assert( vec );
   for( i = 0; i < vec->buffer_count; ++i ) {
      vec->deallocator( vec->buffers[ i ] );
   }
   vec->deallocator( vec->buffers );
   vec->buffers = NULL;

   vec->buffer_count = 0;
   vec->size = 0;
}

static u32 vul__stable_vector_calculate_buffer_index( u32 base_size, u32 i )
{
   u32 bi, base;

   base = base_size;
   bi = 0;
   while( base <= i ) {
      base += ( u32 )pow( ( f32 )base_size, ( s32 )++bi + 1 );
   }

   return bi;
}

static u32 vul__stable_vector_calculate_relative_index( u32 base_size, u32 bi, u32 i )
{
   u32 accumulated_base, base;

   accumulated_base = 0;
   base = 0;
   while( base < bi ) {
      accumulated_base += ( u32 )pow( ( f32 )base_size, ( s32 )base + 1 );
      ++base;
   }
   return i - accumulated_base;
}

void *vul_svector_append_empty( vul_svector *vec )
{
   u32 bi, ri;
   void **ret;

   assert( vec );

   // Calculate index
   bi = vul__stable_vector_calculate_buffer_index( vec->buffer_base_size, vec->size );
   ri = vul__stable_vector_calculate_relative_index( vec->buffer_base_size, bi, vec->size++ );
   assert( bi <= vec->buffer_count ); // We can't skip a step...  

   // Make sure we have room for the buffer
   if( bi == vec->buffer_count ) {
      ret = ( void** )realloc( vec->buffers, sizeof( void* ) * ++vec->buffer_count );
      assert( ret );
      vec->buffers = ret;
      vec->buffers[ vec->buffer_count - 1 ] = NULL;
   }
   // Make sure the buffer exists
   if( !vec->buffers[ bi ] ) {
      vec->buffers[ bi ] = vec->allocator( ( u32 )pow( ( f32 )vec->buffer_base_size,
         ( s32 )bi + 1 ) * vec->element_size );
   }
   assert( vec->buffers[ bi ] ); // This should always be valid at this point!

   return ( void* )( ( u8* )vec->buffers[ bi ] + ( ri * vec->element_size ) );
}

void *vul_svector_append( vul_svector *vec, void *element )
{
   void *data;

   data = vul_svector_append_empty( vec );
   memcpy( data, element, vec->element_size );

   return data;
}

static void *vul__stable_vector_get_tiered( vul_svector *vec, u32 buffer, u32 index )
{
   return ( void* )( ( u8* )vec->buffers[ buffer ] + ( vec->element_size * index ) );
}

void *vul_svector_get( vul_svector *vec, u32 index )
{
   u32 bi, ri;

   assert( vec );
   assert( index < vec->size );

   bi = vul__stable_vector_calculate_buffer_index( vec->buffer_base_size, index );
   ri = vul__stable_vector_calculate_relative_index( vec->buffer_base_size, bi, index );

   return vul__stable_vector_get_tiered( vec, bi, ri );
}

void vul_svector_remove_swap( vul_svector *vec, u32 index )
{
   void *rem, *rep;

   assert( vec );
   if( vec->size == 1 ) {
      --vec->size;
      return; // We don't deallocate down to zero.
   }
   rem = vul_svector_get( vec, index );
   rep = vul_svector_get( vec, vec->size - 1 );
   // Copy last element over, unless this is the last element
   if( rem != rep ) {
      memcpy( rem, rep, vec->element_size );
   }
   // Decrese size
   --vec->size;
   // Check if we can deallocate a buffer. We only deallocate a buffer if the next smaller 
   // one is empty, so only if we have _two_ empty buffer at the end. Memory is cheaper
   // than alloc<->dealloc oscillations at borders.
   if( vec->buffer_count > 1 &&
      vul__stable_vector_calculate_buffer_index( vec->buffer_base_size, vec->size )
      < vec->buffer_count - 1 ) {
      vec->deallocator( vec->buffers[ --vec->buffer_count ] );
   }
}

void vul_svector_iterate( vul_svector *vec,
                          void( *func )( void *data, u32 index, void *func_data ),
                          void *func_data )
{
   u32 i, ai, bi, bs;

   assert( vec );

   ai = 0;
   for( bi = 0; bi < vec->buffer_count; ++bi ) {
      bs = ( u32 )pow( ( f32 )vec->buffer_base_size, ( s32 )bi + 1 );
      for( i = 0; i < bs && ai < vec->size; ++i, ++ai ) {
#ifdef VUL_DEBUG
         void *addr = vec->buffers[ bi ];
#endif
         func( ( void* )( ( u8* )vec->buffers[ bi ] + ( vec->element_size * i ) ), ai, func_data );
#ifdef VUL_DEBUG
         assert( addr == vec->buffers[ bi ] ); // Make sure we didn't alter the buffer (reallocate or free it)
#endif
      }
   }
}

void *vul_svector_find( vul_svector *vec,
                        void *element, s32( *comparator )( void *a, void *b ) )
{
   u32 i, ai, bi, bs;
   void *current;

   assert( vec );

   ai = 0;
   for( bi = 0; bi < vec->buffer_count; ++bi ) {
      bs = ( u32 )pow( ( f32 )vec->buffer_base_size, ( s32 )bi + 1 );
      for( i = 0; i < bs; ++i, ++ai ) {
         current = ( void* )( ( u8* )vec->buffers[ bi ] + ( vec->element_size * i ) );
         if( comparator( element, current ) == 0 ) {
            return current;
         }
      }
   }
   return NULL;
}

u32 vul_svector_size( vul_svector *vec )
{
   assert( vec );
   return vec->size;
}

b32 vul_svector_is_empty( vul_svector *vec )
{
   assert( vec );
   return vec->size == 0;
}

#ifdef _cplusplus
}
#endif

#ifndef VUL_TYPES_H
#undef u8
#undef u32
#undef s32
#undef b32
#undef f32
#endif

#endif // VUL_DEFINE

