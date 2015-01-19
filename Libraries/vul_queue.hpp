/*
 * Villains' Utility Library - Thomas Martin Schmid, 2015. Public domain¹
 *
 * This file describes a templated ring-buffer implementation of a queue.
 * @TODO: Copy constructor.
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
#ifndef VUL_queue_t_HPP
#define VUL_queue_t_HPP

#include <cassert>
#include <cstring>

namespace vul {

	typedef unsigned int ui32_t;
	typedef float f32_t;

	/**
	 * A templated ring-buffer implementation of a queue. Automatically resizes
	 * when full. The operation of resizing is slow, as it copies the entire buffer.
	 */
	template< typename T >
	class queue_t
	{
	private:
		/**
		 * The buffer.
		 */
		T *data;
		/**
		 * The reserved size of the buffer.
		 */
		ui32_t reserved;
		/**
		 * The index of the first element of the queue_t.
		 */
		ui32_t front;
		/**
		 * The index of the first slot after the last element of the queue_t.
		 */
		ui32_t back;
		/**
		 * The growth factor of the buffer; the factor by which we grow
		 * the buffer when we run out of size.
		 */
		f32_t growth_factor;

		/**
		 * Resizes the queue_t. Called when needed from push.
		 * Due to the nature of reallocation and ring buffers, we 
		 * need to copy the data to a new buffer and delete the old one.
		 * This makes this slow!
		 */
		void resize( ui32_t new_size );

	public:
		/**
		 * Creates a new queue_t. Optionally takes the initial size to allocate the buffer
		 * and the growth factor by which we grow that buffer.
		 */
		queue_t( ui32_t initial_size = 8, f32_t growth_factor = 1.5f );
		/**
		 * Deletes the queue_t.
		 */
		~queue_t();

		/**
		 * Resizes the buffer to the exact size of the queue_t at the time of calling.
		 * This uses resize, so is slow.
		 */
		void tighten( );

		/**
		 * Returnes the size of the queue_t.
		 */
		ui32_t size( );
		
		/**
		 * Pushes a new element into the back of queue_t.
		 */
		void push( const T &data );
		/**
		 * Pops an element from the front of the queue_t.
		 */
		T &pop( );
		/**
		 * Peeks at the first element of the queue_t.
		 */
		T &peek( );
	};

	template< typename T >
	void queue_t< T >::resize( ui32_t new_size )
	{
		T *buffer = new T[ new_size ];
		assert( buffer != NULL );

		ui32_t back = this->size( );

		if( this->back >= this->front )
		{
			memcpy( buffer,
					&this->data[ this->front ],
					( this->back - this->front ) * sizeof( T ) );
		} else {
			memcpy( buffer,
					&this->data[ this->front ],
					( this->reserved - this->front ) * sizeof( T ) );
			memcpy( &buffer[ this->reserved - this->front ],
					this->data,
					this->back * sizeof( T ) );
		}
		if( this->data != NULL ) {
			delete [] this->data;
		}
		this->data = buffer;
		this->reserved = new_size;
		this->front = 0;
		this->back = back;
	}

	template< typename T >
	queue_t< T >::queue_t( ui32_t initial_size = 8, f32_t growth_factor = 1.5f )
	{
		this->data = new T[ initial_size ];
		assert( this->data );

		this->front = 0;
		this->back = 1;
		this->reserved = initial_size;
		this->growth_factor = growth_factor;
	}
	template< typename T >
	queue_t< T >::~queue_t( )
	{
		delete [] this->data;
	}

	template< typename T >
	void queue_t< T >::tighten( )
	{
		this->resize( this->size( ) );
	}
	template< typename T >
	ui32_t queue_t< T >::size( )
	{
		if( this->back >= this->front )
		{
			return ( this->back - this->front ) - 1 ;
		}
		return this->reserved - ( ( this->front - this->back ) + 1 );
	}
	
	template< typename T >
	void queue_t< T >::push( const T &data )
	{
		// If full, resize
		if( this->front == this->back )
		{
			this->resize( ( ui32_t )( ( f32_t )this->reserved * this->growth_factor ) );
		}
		// Insert it
		this->data[ this->back++ ] = data;
		// Wrap if needed
		if( this->back == this->reserved ) {
			this->back = 0;
		}
	}
	template< typename T >
	T &queue_t< T >::pop( )
	{
		// Grab element
		T &ret = this->data[ this->front++ ];

		// Wrap if needed
		if( this->front == this->reserved ) {
			this->front = 0;
		}
		return ret;
	}
	template< typename T >
	T &queue_t< T >::peek( )
	{
		return this->data[ this->front ];
	}
}

#endif