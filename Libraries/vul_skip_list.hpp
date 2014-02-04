/*
 * Villains' Utility Library - Thomas Martin Schmid, 2014. Public domain¹
 *
 * This file describes a templated singly linked skip list. The current
 * implementation is not indexable.
 * @TODO: Copy constructor
 * 
 * ¹ If public domain is not legally valid in your country and or legal area,
 *   the MIT licence applies (see the LICENCE file)
 */
#ifndef VUL_skip_list_t_HPP
#define VUL_skip_list_t_HPP

#include <cassert>
#include <cstring>

/**
 * If defined, the functions are defined and not just declared. Only do this in _one_ c/cpp file!
 */
//#define VUL_DEFINE

namespace vul {

	typedef unsigned int ui32_t;

	/** 
	 * Comparison function
	 */
	template< typename T >
	typedef int ( *compare_function )( const T &a, const T &b );

	template< typename T >
	class skip_list_t_element_t {
		/**
		 * The data of the elment.
		 */
		T data;
		/**
		 * Number of levels this stop reaches.
		 */
		ui32_t levels;
		/**
		 * Next element at each level it reaches.
		 */
		skip_list_t_element_t *nexts[ ];

		/**
		 * Creates a new element, allocates the nexts-array.
		 */
		skip_list_t_element_t( const T &data, ui32_t levels );
		/**
		 * Deletes an element, deallocates the nexts-array.
		 */
		~skip_list_t_element_t( );
	};

	template< typename T >
	skip_list_t_element_t< T >::skip_list_t_element_t( const T &data, ui32_t levels )
	{
		this->data = data;
		this->levels = levels;
		this->nexts = new skip_list_t_element_t*[ levels ];
		for( ui32_t i = 0; i < levels; ++i )
		{
			this->nexts[ i ] = NULL;
		}
	}
	template< typename T >
	skip_list_t_element_t< T >::~skip_list_t_element_t( )
	{
		delete [] this->nexts;
	}

	template< typename T >
	class skip_list_t {
	private:
		/**
		 * Number of levels this list has.
		 */
		ui32_t levels;
		/**
		 * Head of the list at each level.
		 */
		skip_list_t_element_t *heads[ ];
		/**
		 * Comparison function.
		 */
		compare_function comparator;

		/**
		 * Flips a coin. .5 probability of false, same for true.
		 */
		bool coin_flip( );

	public:
		/**
		 * Creates a new skip list.
		 */
		skip_list_t( compare_function comparator );
		/**
		 * Destroys a list.
		 */
		~skip_list_t( );

		/**
		 * Finds the first element in the list that matches the given data.
		 */
		skip_list_t_element_t *find( const T &data );
		/**
		 * Removes the given element from the list.
		 */
		void remove( skip_list_t_element_t *e );
		/**
		 * Inserts the given data into the list. Returns the newly created element.
		 */
		skip_list_t_element_t *insert( const T &data );
		/** 
		 * Returns the size of the list.
		 */
		ui32_t size( );
	};

	template< T >
	bool skip_list_t< T >::coin_flip( )
	{
		return ( rand( ) / RAND_MAX ) > 0.5f );
	}

	template< T >
	skip_list_t< T >::skip_list_t( compare_function comparator )
	{
		this->comparator = comparator;
		this->levels = 1;
		this->heads = new skip_list_t_element_t*[ 0 ];
		assert( this->heads );
		this->heads[ 0 ] = NULL;
	}
	template< T >
	skip_list_t< T >::~skip_list_t( )
	{
		// Iterate through the bottom level, destroy all elements
		skip_list_t_element_t *e = this->heads[ 0 ], *n;

		while( e != NULL )
		{
			n = e->nexts[ 0 ];
			delete e;
			e = n;
		}

		// Deallocate the heads array
		delete [] this->heads;
	}

	template< T >
	skip_list_t_element_t *skip_list_t< T >::find( const T &data )
	{
		skip_list_t_element_t *e = this->heads[ this->levels - 1 ];

		for( ui32_t l = this->levels - 1; l >= 0; --l )
		{
			while( e != NULL && this->comparator( e->data, data ) )
			{
				e = e->nexts[ l ];
			}
		}

		if( e != NULL && this->comparator( e->data, data ) == 0 ) {
			return e;
		}
		return NULL;
	}
	template< T >
	void skip_list_t< T >::remove( skip_list_t_element_t *e )
	{
		assert( e != NULL );
		skip_list_t_element_t *h, *ph;

		// For every level
		for( ui32_t l = this->levels - 1; l >= 0; --l )
		{
			h = this->heads[ l ];
			ph = NULL;
			// See if e exists on this level
			while( h != NULL && h != e )
			{
				ph = h;
				h = h->nexts[ l ];
			}
			// And if it does, remove it's link
			if( h == e ) {
				ph->nexts[ l ] = e->nexts[ l ];
			}
		}
		// Then delete e
		delete e;
	}
	template< T >
	skip_list_t_element_t *skip_list_t< T >::insert( const T &data )
	{
		// Stores the last element smaller than data at each level
		skip_list_t_element_t ** el = new skip_list_t_element_t*[ this->levels ];
		for( ui32_t l = this->levels - 1; l >= 0; --l )
		{
			skip_list_t_element_t *e = this->heads[ l ];
			while( this->comparator( e->data, data ) < 0 )
			{
				if( e->nexts[ l ] == NULL ) {
					break;
				}
				e = e->nexts[ l ]
			}
			el[ l ] = e;
		}

		// Create the new element.
		ui32_t lvls = 1;
		while( coin_flip( ) ) {
			++lvls;
		}
		skip_list_t_element_t *n = new skip_list_t_element_t( data, lvls );
		assert( n != NULL );
		// If lvls is greater than the list's levelcount, grow heads.
		if( lvls > this->levels )
		{
			skip_list_t_element_t **tmp = new skip_list_t_element_t*[ lvl ];
			skip_list_t_element_t **tmpel = new skip_list_t_element_t*[ lvl ];
			ui32_t i;
			for( i = 0; i < this->levels;  ++i )
			{
				tmp[ i ] = heads[ i ];
				tmpel[ i ] = el[ i ];
			}
			for( ; i < lvls; ++i )
			{
				tmp[ i ] = NULL;
				tmpel[ i ] = NULL;
			}
			delete [] heads;
			delete [] el;
			el = tmpel;
			this->heads = tmp;
		}
		// Insert n at every level
		for( ui32_t i = 0; i < lvls; ++i )
		{
			if( el[ i ] == NULL ) {
				heads[ i ] = n;
			} else {
				n->nexts[ i ] = el[ i ]->nexts[ i ];
				el[ i ]->nexts[ i ] = n;
			}
		}

		delete [] el;
	}
	
	template< T >
	ui32_t skip_list_t< T >::size( )
	{
		ui32_t c = 0;
		skip_list_t_element_t *e = this->heads[ 0 ];

		while( c != NULL ) 
		{
			++c;
			e = e->nexts[ 0 ];
		}

		return c;
	}
}