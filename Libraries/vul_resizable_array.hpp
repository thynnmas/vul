/*
 * Villains' Utility Library - Thomas Martin Schmid, 2014. Public domain¹
 *
 * This file describes an alternative to the STL vector class. This is mostly 
 * based on Tom Forsyth's ArbitraryList found here: https://home.comcast.net/~tom_forsyth/blog.wiki.html
 * This version is templated. Contains a shell sort implementation for sorting.
 * If a faster sort is wanted, vul_sort.h contains multiple sorting algorithms for the
 * non-templated version of this class in vul_resizeable_array.h
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
#ifndef VUL_RESIZEABLE_ARRAY_HPP
#define VUL_RESIZEABLE_ARRAY_HPP

#include <cstddef>
#include <cassert>

/**
 * If enabled, iterators look for undefined/illegal behaviour during loops.
 * Slower, but might help track down ugly bugs!
 */
//#define VUL_DEBUG

namespace vul {
	
	typedef unsigned int ui32_t;

	/**
	 * An alternative to the STL vector class. This is mostly based on Tom Forsyth's ArbitraryList
	 * found here: https://home.comcast.net/~tom_forsyth/blog.wiki.html
	 * It adds a version without templates for a potential compile time decrease at the cost of readability,
	 * a sorting function and a few macros to help portability between templated and non-templated versions (<-@TODO).
	 */
	template < class T >
	class vector_t {
	public:
		/**
		 * Constructor. Takes an optional initial size.
		 * @param initialSize [Optional] Initial reserved size of list.
		 */
		vector_t< T >( ui32_t initialSize = 0 );
		/**
		 * Copy Constructor. Takes an optional initial size.
		 * @param ref The list to copy from
		 */
		vector_t< T >( const vector_t< T > &ref );
		/**
		 * Destructor. Only exists in CPP version, otherwise call finish manually.
		 */
		~vector_t< T >( void );
		/**
		 * Initializes the list. Starting sizes are optional.
		 * @param initialSize [Optional] Initial size of list.
		 * @param initialReservedSize [Optional] Initial size of reserved space.
		 */
		void initialize( ui32_t initialSize = 0, ui32_t initialReservedSize = 0 );
		/**
		 * Deletes the list and cleans up.
		 */
		void finish( );

		/**
		 * Returns the pointer to the item at the given location.
		 * If templates are used, this is typed, if not it is a void*.
		 * @param index Index of the element to get.
		 */		
		T *get( const ui32_t index );
		/**
		 * Returns a const pointer to the item at the given location.
		 * If templates are used, this is typed, if not it is a void*.
		 * @param index
		 */		
		const T *getConst( const ui32_t index ) const;
		/**
		 * Returns the item at the given location.
		 * Only available if templates are used.
		 * @param index
		 */
		T &operator[]( const ui32_t index );
		/**
		 * Returns the const item at the given location.
		 * Only available if templates are used.
		 * @param index
		 */
		const T &operator[] ( const ui32_t index ) const;

		/**
		 * Returns the pointer to the first element of the list.
		 * If templates are used, this is typed, if not it is a void*.
		 */
		T *begin( void );
		/**
		 * Returns the pointer to the last element of the list plus 1.
		 * This is the equivalent of the STL vector.end().
		 * If templates are used, this is typed, if not it is a void*.
		 */
		T *end( void );

		/**
		 * Returns the size of the list.
		 */
		const ui32_t size( void ) const;

		/**
		 * Resizes the list to the given number of items. Preserves items already
		 * in the list up to the new size, others are lost.
		 * @param size Size of the new list
		 * @param freeZero [Optional] Whether the memory should be freed if new size is 0.
		 * @param allocExactly [Optional] Whether reserved size and size should be equal.
		 * @return a pointer to the first element of the new list.
		 */
		T *resize( ui32_t size, bool freeZero = true, bool allocExactly = false );
		/**
		 * Preallocates the list to at least this size.
		 * @param size Minimum size of allocated area.
		 * @param allocExactly [Optional] Whether reserved size and size should be equal.
		 */
		void reserve( ui32_t size, bool allocExactly = false );
		/**
		 * Frees the memory. Equivalent to resize(0, true).
		 */
		void freemem( void );

		/**
		 * Removes the item at the given index by copying the last item to it.
		 * @param index Index of the element to remove.
		 */
		void removeSwap( ui32_t index );
		/**
		 * Removes the item at the given index. Copies all following elements one slot forward,
		 * preserving order of the list.
		 * @param index Index of the element to remove.
		 */
		void removeCascade( ui32_t index );
		/**
		 * Adds the slot for an item to teh list and returns a pointer to it.
		 * @return the pointer to the item
		 */
		T *add( void );
		/**
		 * Copies the given item to the list.
		 * @param item item to add
		 */
		void add( T const &item );
		/**
		 * Inserts a slot for an item to the list at the given location
		 * and returns a pointer to it. Cascades everything after index
		 * down one slot.
		 * @param index index to insert the new item
		 * @return pointer to the new item
		 */
		T *insert( ui32_t index );
		/**
		 * Copies the given item to the list. Cascades everything after index
		 * down one slot.
		 * @param item item to add
		 * @param index index to insert the new item
		 */
		void insert( T const &item, ui32_t index );
		/**
		 * Swaps the elemnts at the given indices
		 * @param indexA index of first element
		 * @param indexB index of second element
		 */
		void swap( ui32_t indexA, ui32_t indexB );

		/**
		 * Copies from the given array into this vector. Will overwrite anything after
		 * index with the new items.
		 * @param index The index to copy the first item to
		 * @param list The other vector that is to be copied from
		 * @param count The number of items to copy
		 */
		void copy( ui32_t index, const T* list, ui32_t count );
		/**
		 * Copies from the given vector into this one. Will overwrite anything after
		 * index with the new items.
		 * @param index The index to copy the first item to
		 * @param list The other vector that is to be copied from
		 * @param otherFirstIndex [Optional] The first index in the other vector to copy. Default is 0.
		 * @param otherCount [Optional] The number of items to copy. Default is entire list.
		 */
		void copy( ui32_t index, vector_t< T > &list, ui32_t otherFirstIndex = 0, ui32_t otherCount = 0xffffffff );

		/**
		 * Copies from the given vector onto the end of this one.
		 * @param list The other vector that is to be copied from
		 * @param otherFirstIndex The first index in the other vector to copy
		 * @param otherCount [Optional] The number of items to copy. Default is entire list.
		 */
		void append( vector_t< T > &list, ui32_t otherFirstIndex, ui32_t otherCount = 0xffffffff );
		/**
		 * Finds the index of the given item, or -1 if not found.
		 * @param item The item to find
		 * @return the index of the item, -1 if not found.
		 */
		ui32_t find( const T &item );
		
		/**
		 * Shrinks the vector to fit the current size.
		 */
		void tighten( void );
		
		/**
		 * Sorts the given vector based on the result of the goven comparator function.
		 * Uses shell sort. @TODO: Add vector_t support to vul_sort
		 */
		static void sort( vector_t< T > &list, int (*comparator)( const T *a, const T *b ) );

		
		/**
		* The reserved size of the list
		*/
		ui32_t mReservedSize;
		/**
		* The current actual size of the list
		*/
		ui32_t mSize;
		/**
		* The list itself
		*/
		T *mList;

	};
	
	template< class T >
	vector_t< T >::vector_t( ui32_t initialSize )
	{
		initialize( 0, initialSize );
	}
	template< class T >
	vector_t< T >::vector_t( const vector_t< T > &ref )
	{
		ui32_t size = ref.size( );

		mList = NULL;
		mSize = 0;
		mReservedSize = 0;

		resize( size );

		for ( ui32_t i = 0; i < size; ++i ) {
			*( get( i ) ) = *( ref.getConst( i ) );
		}
	}
	template< class T >
	vector_t< T >::~vector_t( void )
	{
		finish( );
	}
	template< class T >
	void vector_t< T >::initialize( ui32_t initialSize = 0, ui32_t initialReservedSize = 0 )
	{
		mList = NULL;
		mSize = 0;
		mReservedSize = 0;
		if ( initialReservedSize > initialSize ) {
			reserve( initialReservedSize, true );
			resize( initialSize, false );
		} else if ( initialSize > 0 ) {
			resize( initialSize, true, true );
		}
	}
	template< class T >
	void vector_t< T >::finish( )
	{
		if ( mList == NULL ) {
			assert( mReservedSize == 0 );
			assert( mSize == 0 );
		} else {
			assert( mReservedSize > 0 );
			assert( mSize >= 0 );
			free( mList );
			mReservedSize = 0;
			mSize = 0;
			mList = NULL;
		}
	}
	template< class T >
	T *vector_t< T >::get( const ui32_t index )
	{
		assert( index < mSize );
		return ( &mList[ index ] );
	}
	template< class T >
	const T *vector_t< T >::getConst( const ui32_t index ) const
	{
		assert( this != NULL );
		assert( index < mSize );
		return ( &mList[ index ] );
	}
	template< class T >
	T &vector_t< T >::operator[]( const ui32_t index )
	{
		return *get( index );
	}
	template< class T >
	const T &vector_t< T >::operator[] ( const ui32_t index ) const
	{
		return *getConst( index );
	}
	template< class T >
	T *vector_t< T >::begin( void )
	{
		return mList;
	}
	template< class T >
	T *vector_t< T >::end( void )
	{
		return ( mList + mSize );
	}
	template< class T >
	const ui32_t vector_t< T >::size( void ) const
	{
		return mSize;
	}
	template< class T >
	T *vector_t< T >::resize( ui32_t size, bool freeZero = true, bool allocExactly = false )
	{
		ui32_t oldSize, newSize;
			
		oldSize = mSize;
		mSize = size;

		if ( mSize == mReservedSize ) {
			// Size matches exactly. Assert that things are well then leave.
			if ( mReservedSize == 0 ) {
				assert( mList == NULL );
			} else  {
				assert( mList != NULL );
			}
			return mList;
		} else if ( mSize < mReservedSize ) {
			// We have enough room.
			if ( ( mSize == 0) && freeZero ) {
				// New size is zero and freeing is wanted
				free( mList );
				mList = NULL;
				mReservedSize = 0;
				return NULL;
			}
			if ( !allocExactly ) {
				// No resizing needed
				return mList;
			}
		}
		// We need to resize
		assert( mSize > 0 );
		newSize = mSize;
		if ( !allocExactly ) {
			// Grow by 50% and make sure it's not too small
			newSize = ( mSize * 3 ) >> 1;
			if ( newSize < 8 ) {
				newSize = 8;
			}
			assert( newSize > mReservedSize );
		}
		if ( mList == NULL ) {
			assert( mReservedSize == 0 );
			mList = static_cast< T* >( malloc( sizeof( T ) * newSize ) );
			assert( mList != NULL ); // Make sure malloc didn't fail
		} else {
			assert( mReservedSize > 0 );
			mList = static_cast< T* >( realloc( mList, sizeof( T ) * newSize ) );
			assert( mList != NULL ); // Make sure realloc didn't fail
		}
		assert( mList != NULL );
		mReservedSize = newSize;
		return mList;
	}
	template< class T >
	void vector_t< T >::reserve( ui32_t size, bool allocExactly = false )
	{
		ui32_t oldSize;

		assert( size >= mSize );
		if ( size <= mReservedSize ) {
			return;
		}

		oldSize = mSize;
		resize( size, false, allocExactly );
		mSize = oldSize;
	}
	template< class T >
	void vector_t< T >::freemem( void )
	{
		resize( 0, true );
	}
	template< class T >
	void vector_t< T >::removeSwap( ui32_t index )
	{
		assert( index < mSize );
		mList[ index ] = mList[ mSize - 1 ];
		resize( mSize - 1 );
	}
	template< class T >
	void vector_t< T >::removeCascade( ui32_t index )
	{
		ui32_t i;

		assert( index < mSize );
		for ( i = index; i < mSize - 1; ++i ) {
			mList[ i ] = mList[ i + 1 ];
		}
		resize( mSize - 1 );
	}
	template< class T >
	T *vector_t< T >::add( void )
	{
		resize( mSize + 1 );
		return ( &mList[ mSize - 1 ] );
	}
	template< class T >
	void vector_t< T >::add( T const &item )
	{
		*( add( ) ) = item;
	}
	template< class T >
	T *vector_t< T >::insert( ui32_t index )
	{
		ui32_t i, last, first;

		assert( index <= mSize );
		resize( mSize + 1 );
		for ( i = mSize - 1; i >= index + 1; --i ) {
			mList[ i ] = mList[ i - 1 ];
		}
		return ( &mList[ index ] );
	}
	template< class T >
	void vector_t< T >::insert( T const &item, ui32_t index )
	{
		*( insert( index ) ) = item;
	}
	template< class T >
	void vector_t< T >::swap( ui32_t indexA, ui32_t indexB )
	{
		assert( indexA < mSize );
		assert( indexB < mSize );
		
		T temp = mList[ indexA ];
		
		mList[ indexA ] = mList[ indexB ];
		mList[ indexB ] = temp;
	}
	template< class T >
	void vector_t< T >::copy( ui32_t index, const T* list, ui32_t count )
	{
		ui32_t i;
		void *first;

		if ( mSize < ( index + count ) ) {
			resize( index + count );
		}
		for ( i = 0; i < count; ++i ) {
			*( get( i + index ) ) = list[ i ];
		}
	}
	template< class T >
	void vector_t< T >::copy( ui32_t index, vector_t< T > &list, ui32_t otherFirstIndex = 0, ui32_t otherCount = 0xffffffff )
	{
		ui32_t i;
		void *firstLocal;
		const void *firstOther;

		if ( otherCount == 0xffffffff ) {
			otherCount = list.size( ); - otherFirstIndex;
		}
		if ( mSize < ( index + count ) ) {
			resize( index + count );
		}
		assert( list.size( ) >= ( otherFirstIndex + otherCount ) );
		for ( i = 0; i < count; ++i ) {
			*( get( i + index ) ) = *( list.getConst( i + otherFirstIndex ) );
		}
	}
	template< class T >
	void vector_t< T >::append( vector_t< T > &list, ui32_t otherFirstIndex, ui32_t otherCount = 0xffffffff )
	{
		ui32_t first;

		if ( otherCount == 0xffffffff ) {
			otherCount = list.size( ) - otherFirstIndex;
		}
		first = mSize;
		resize( first + otherCount );
		copy( first, list, otherFirstIndex, otherCount );
	}
	template< class T >
	ui32_t vector_t< T >::find( const T &item )
	{
		ui32_t i;

		for ( i = 0; i < mSize; ++i ) {
			if ( mList[ i ] == item ) {
				return i;
			}
		}
		return mSize;
	}
	template< class T >
	void vector_t< T >::tighten( void )
	{
		resize( mSize, true, true );
	}
	template< class T >
	void vector_t< T >::sort( vector_t< T > &list, int (*comparator)( const T *a, const T *b ) )
	{
		ui32_t gaps[] = { 701, 301, 132, 67, 23, 10, 4, 1 };
		ui32_t gap, gi, i, j;
		T temp;

		for ( gi = 0; gi < sizeof( gaps ); ++gi ) {
			// Insertion sort for each gap size
			for ( i = gaps[ gi ]; i < list.size( ); ++i ) {
				gap = gaps[ gi ];
				temp = *list.get( i );
				for ( j = i; j >= gap && comparator( list.getConst( j - gap ), &temp ) > 0; j-= gap ) {
					*list.get( j ) = *list.get( j - gap );
				}
				*list.get( j ) = temp;
			}
		}
	}
	
}


// ----------------
// Macros
//

// Iterators
#ifdef VUL_DEBUG
	// @Note: Debug versions attempt to spot if you alter elemtns in the middle of a loop. That might cause a resize and in turn mayhem.
	// Normal iterator
	#define vul_foreach( T, list ) for ( T *it = list.begin( ), *first = list.begin( ), *last = list.end( ); assert( first == list.begin( ) ), assert( last == list.end( ) ), it != last; ++it )
	// Easier for list of values; copies
	#define vul_foreachval( T, list ) for ( T *it = list.begin( ), *first = list.begin( ), *last = list.end( ), ref = ( ( it != last ) ? *it : T( ) ); assert( first == list.begin( ) ), assert( last == list.end( ) ), it != last; ref = *( ++it ) )
	// Easier for list of pointers
	#define vul_foreachptr( T, list ) for ( T **it = list.begin( ), **first = list.begin( ), **last = list.end( ), *ref = ( ( it != last ) ? *it : NULL ); assert( first == list.begin( ) ), assert( last == list.end( ) ), it != last; ref = *( ++it ) )
#else
	// Normal iterator
	#define vul_foreach( T, list ) for ( T *it = list.begin( ), *last = list.end( ); it != last; ++it )
	// Easier for list of values; copies
	#define vul_foreachval( T, list ) for ( T *it = list.begin( ), *last = list.end( ), ref = ( ( it != last ) ? *it : T( ) ); it != last; ref = *( ++it ) )
	// Easier for list of pointers
	#define vul_foreachptr( T, list ) for ( T **it = list.begin( ), **last = list.end( ), *ref = ( ( it != last ) ? *it : NULL ); it != last; ref = *( ++it ) )
#endif

#endif