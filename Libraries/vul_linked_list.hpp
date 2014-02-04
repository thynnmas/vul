/*
 * Villains' Utility Library - Thomas Martin Schmid, 2014. Public domain¹
 *
 * This file describes a doubly linked list.
 * 
 * ¹ If public domain is not legally valid in your country and or legal area,
 *   the MIT licence applies (see the LICENCE file)
 */
#ifndef VUL_LINKED_LIST_HPP
#define VUL_LINKED_LIST_HPP

#include <malloc.h>
#include <assert.h>
#include <string.h>

/**
 * If defined, the functions are defined and not just declared. Only do this in _one_ c/cpp file!
 */
//#define VUL_DEFINE

namespace vul
{
	typedef unsigned int ui32_t;
	
	/**
	 * Define the comparator function.
	 */
	template< typename T >
	typedef int ( *compare_function )( const T &a, const T &b );

	/**
	 * A doubly linked list. A linked list is just a root element.
	 */
	template< typename T >
	class list_element_t {
	
	private:
		/** 
		 * The data
		 */
		T data;
		/**
		 * The preivous list element
		 */
		list_element_t *prev;
		/**
		 * The next list element
		 */
		list_element_t *next;
		
		/**
		 * Inserts a new element after this element.
		 */
		list_element_t *insert_after( const T &data );
		
	public:
		/** 
		 * Creates a new list element. Default arguments create a new empty list.
		 */
		list_element_t( const T &data, list_element_t< T > *prev = NULL, list_element_t< T > *next = NULL );
		/**
		 * Creates a copy of the given list.
		 */
		list_element_t( const list_element_t< T > &ref );
		/**
		 * Deletes a list element. This properly removes the element from the list.
		 */
		~list_element_t( );

		/**
		 * Finds the given data in the list, returning the element containing it.
		 * If no_match_null is true:
		 *		-if no exact match is found null is returned.
		 *		-the first exact match is returned.
		 * If no_match_null is false:
		 *		-if no exact match is found, the last smaller element is returned
		 *		-the last exact match is returned.
		 * The no_match_null == false option is very useful for finding the spot
		 * to insert a new element.
		 */
		list_element_t *find( const T &data, compare_function comparator, bool no_match_null = false );
		
		/**
		 * Inserts the given data into the list while keeping the list sorted.
		 */
		list_element_t *insert( const T &data, compare_function comparator );

		/*
		 * Returnes the length of the list from this element to the end.
		 */
		ui32_t size( );
	};

	template< typename T >
	list_element_t< T >::list_element_t( const T &data, list_element_t< T > *prev = NULL, list_element_t< T > *next = NULL )
	{
		this->data = data;
		this->prev = prev;
		this->next = next;
	}

	template< typename T >
	list_element_t< T >::list_element_t( const list_element_t< T > &ref )
	{
		this->data = ref.data;
		
		list_element_t< T > *o = &ref;
		list_element_t< T > *n = this;
		while( o )
		{
			n->next = new list_element_t< T >( o->next->data, n );
			assert( n->next != NULL );
			n->next->pref = n;

			n = n->next;
			o = o->next;
		}
		
	}

	template< typename T >
	list_element_t< T >::~list_element_t( )
	{
		if( this->next != NULL ) {
			this->next->prev = this->prev;
		}
		if( this->prev != NULL ) {
			this->prev->next = this->next;
		}
	}

	template< typename T >
	list_element_t *list_element_t< T >::insert_after( const T &data )
	{
		list_element_t< T > *n = new list_element_t< T >( data, this->prev, this->next );
		if( this->prev != NULL ) {
			this->prev->next = n;
		}
		if( this->next != NULL ) {
			this->next->prev = n;
		}
	}

	template< typename T >
	list_element_t *list_element_t< T >::find( const T &data, compare_function comparator, bool no_match_null = false )
	{
		list_element_t< T > *head = this;

		if( so_match_null ) {
			while( head != NULL && comparator( data, head->data ) <= 0 )
			{
				head = head->next;
			}
		} else {
			while( head != NULL && comparator( data, head->data ) < 0 )
			{
				head = head->next;
			}
			if( comparator( data, head->data ) != 0 ) {
				return NULL;
			}
		}
		return head;
	}

	template< typename T >
	list_element_t *list_element_t< T >::insert( const T &data, compare_function comparator )
	{
		// Find the spot
		list_element_t< T > *before = this->find( data );
		// Insert after it
		return before->insert_after( data, comparator, false );
	}

	template< typename T >
	ui32_t size( )
	{
		ui32_t length = 1;
		list_element_t< T > *head = this;

		while( head->next != NULL ) {
			head = head->next;
			++length;
		}

		return length;
	}
}