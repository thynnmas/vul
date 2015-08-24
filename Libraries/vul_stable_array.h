/*
* Villains' Utility Library - Thomas Martin Schmid, 2015. Public domain¹
*
* This file describes a vector class that never moves elements upon
* resize. It is implemented as an series of exponentially growing buffers
* into which pointers will remain constant. @NOTE: Functionality is
* currently somewhat limited as it's use is quite specific atm.
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
#ifndef VUL_STABLE_RESIZEABLE_ARRAY_H
#define VUL_STABLE_RESIZEABLE_ARRAY_H

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>

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
* @TODO(thynn): We don't currently do this!
*/
//#define VUL_DEBUG

typedef struct vul_svector_t {
	ui32_t buffer_count;
	ui32_t size;
	ui32_t buffer_base_size;
	ui32_t element_size;
	void **buffers;

	/* Memory management functions */
	void *( *allocator )( size_t size );
	void( *deallocator )( void *ptr );
} vul_svector_t;

#endif

/**
* Create a stable vector, given the size of an element and the size of
* the base buffer (the smallest one). Every buffer after is double the size.
* Also takes in memory mamagement functions.
*/
#ifndef VUL_DEFINE
vul_svector_t *vul_svector_create( ui32_t element_size,
								   ui32_t buffer_base_size,
								   void *( *allocator )( size_t size ),
								   void( *deallocator )( void *ptr ) );
#else
vul_svector_t *vul_svector_create( ui32_t element_size,
								   ui32_t buffer_base_size,
								   void *( *allocator )( size_t size ),
								   void( *deallocator )( void *ptr ) )
{
	vul_svector_t *ret;

	ret = ( vul_svector_t* )allocator( sizeof( vul_svector_t ) );
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
#endif

/**
* Destroy the vector.
*/
#ifndef VUL_DEFINE
void vul_svector_destroy( vul_svector_t *vec );
#else
void vul_svector_destroy( vul_svector_t *vec )
{
	ui32_t i;

	assert( vec );
	for( i = 0; i < vec->buffer_count; ++i ) {
		vec->deallocator( vec->buffers[ i ] );
	}
	vec->deallocator( vec->buffers );
	vec->deallocator( vec );
}
#endif

/**
* Frees the memory. Equivalent to destroying the vector
* and creating a new one.
*/
#ifndef VUL_DEFINE
void vul_svector_freemem( vul_svector_t *vec );
#else
void vul_svector_freemem( vul_svector_t *vec )
{
	ui32_t i;

	assert( vec );
	for( i = 0; i < vec->buffer_count; ++i ) {
		vec->deallocator( vec->buffers[ i ] );
	}
	vec->deallocator( vec->buffers );
	vec->buffers = NULL;

	vec->buffer_count = 0;
	vec->size = 0;
}
#endif


/**
* Calculates the index of the buffer this element is in.
*/
#ifndef VUL_DEFINE
ui32_t vul__stable_vector_calculate_buffer_index( ui32_t base_size, ui32_t i );
#else
ui32_t vul__stable_vector_calculate_buffer_index( ui32_t base_size, ui32_t i )
{
	ui32_t bi, base;

	base = base_size;
	bi = 0;
	while( base <= i ) {
		base += ( ui32_t )pow( ( f32_t )base_size, ( i32_t )++bi + 1 );
	}

	return bi;
}
#endif

/**
* Calculates the index within its buffer this element is at.
*/
#ifndef VUL_DEFINE
ui32_t vul__stable_vector_calculate_relative_index( ui32_t base_size, ui32_t bi, ui32_t i );
#else
ui32_t vul__stable_vector_calculate_relative_index( ui32_t base_size, ui32_t bi, ui32_t i )
{
	ui32_t accumulated_base, base;

	accumulated_base = 0;
	base = 0;
	while( base < bi ) {
		accumulated_base += ( ui32_t )pow( ( f32_t )base_size, ( i32_t )base + 1 );
		++base;
	}
	return i - accumulated_base;
}
#endif

/**
* Append an empty object to the vector and return the pointer to it.
*/
#ifndef VUL_DEFINE
void *vul_svector_append_empty( vul_svector_t *vec );
#else
void *vul_svector_append_empty( vul_svector_t *vec )
{
	ui32_t bi, ri;
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
		vec->buffers[ bi ] = vec->allocator( ( ui32_t )pow( ( f32_t )vec->buffer_base_size,
			( i32_t )bi + 1 ) * vec->element_size );
	}
	assert( vec->buffers[ bi ] ); // This should always be valid at this point!

	return ( void* )( ( ui8_t* )vec->buffers[ bi ] + ( ri * vec->element_size ) );
}
#endif

/**
* Append the given object to the vector by copying it into place
* and return the pointer to the new element.
*/
#ifndef VUL_DEFINE
void *vul_svector_append( vul_svector_t *vec, void *element );
#else
void *vul_svector_append( vul_svector_t *vec, void *element )
{
	void *data;

	data = vul_svector_append_empty( vec );
	memcpy( data, element, vec->element_size );

	return data;
}
#endif

/**
* Return the element in buffer 'buffer' at index relative to that buffer 'index'.
*/
#ifndef VUL_DEFINE
void *vul__stable_vector_get_tiered( vul_svector_t *vec, ui32_t buffer, ui32_t index );
#else
void *vul__stable_vector_get_tiered( vul_svector_t *vec, ui32_t buffer, ui32_t index )
{
	return ( void* )( ( ui8_t* )vec->buffers[ buffer ] + ( vec->element_size * index ) );
}
#endif

/**
* Get the element at the given index.
*/
#ifndef VUL_DEFINE
void *vul_svector_get( vul_svector_t *vec, ui32_t index );
#else
void *vul_svector_get( vul_svector_t *vec, ui32_t index )
{
	ui32_t bi, ri;

	assert( vec );
	assert( index < vec->size );

	bi = vul__stable_vector_calculate_buffer_index( vec->buffer_base_size, index );
	ri = vul__stable_vector_calculate_relative_index( vec->buffer_base_size, bi, index );

	return vul__stable_vector_get_tiered( vec, bi, ri );
}
#endif

/**
* Removes an element, swapping the last element into it's place.
*/
#ifndef VUL_DEFINE
void vul_svector_remove_swap( vul_svector_t *vec, ui32_t index );
#else
void vul_svector_remove_swap( vul_svector_t *vec, ui32_t index )
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
#endif

/**
* General purpose iterator. Iterates over every element in the vector
* calling the given function for each one. The function takes a pointer to the element
* and it's absolute index in the vector as its first two arguments, then the func_data
* given to this function as a third parameter, allowing you to pass an environment or
* data into it.
*/
#ifndef VUL_DEFINE
void vul_svector_iterate( vul_svector_t *vec, 
						  void( *func )( void *data, ui32_t index, void *func_data ), 
						  void *func_data );
#else
void vul_svector_iterate( vul_svector_t *vec,
						  void( *func )( void *data, ui32_t index, void *func_data ),
						  void *func_data )
{
	ui32_t i, ai, bi, bs;

	assert( vec );

	ai = 0;
	for( bi = 0; bi < vec->buffer_count; ++bi ) {
		bs = ( ui32_t )pow( ( f32_t )vec->buffer_base_size, ( i32_t )bi + 1 );
		for( i = 0; i < bs && ai < vec->size; ++i, ++ai ) {
			func( ( void* )( ( ui8_t* )vec->buffers[ bi ] + ( vec->element_size * i ) ), ai, func_data );
		}
	}
}
#endif

/**
* Find the element equal to the given element according to the given
* comparison function.
*/
#ifndef VUL_DEFINE
void *vul_svector_find( vul_svector_t *vec, 
						void *element, int( *comparator )( void *a, void *b ) );
#else
void *vul_svector_find( vul_svector_t *vec,
						void *element, int( *comparator )( void *a, void *b ) )
{
	ui32_t i, ai, bi, bs;
	void *current;

	assert( vec );

	ai = 0;
	for( bi = 0; bi < vec->buffer_count; ++bi ) {
		bs = ( ui32_t )pow( ( f32_t )vec->buffer_base_size, ( i32_t )bi + 1 );
		for( i = 0; i < bs; ++i, ++ai ) {
			current = ( void* )( ( ui8_t* )vec->buffers[ bi ] + ( vec->element_size * i ) );
			if( comparator( element, current ) == 0 ) {
				return current;
			}
		}
	}
	return NULL;
}
#endif

/**
* Returns the number of elements in the vector.
*/
#ifndef VUL_DEFINE
ui32_t vul_svector_size( vul_svector_t *vec );
#else
ui32_t vul_svector_size( vul_svector_t *vec )
{
	assert( vec );
	return vec->size;
}
#endif

/**
* Returns true if the vector is empty
*/
#ifndef VUL_DEFINE
bool32_t vul_svector_is_empty( vul_svector_t *vec );
#else
bool32_t vul_svector_is_empty( vul_svector_t *vec )
{
	assert( vec );
	return vec->size == 0;
}
#endif
