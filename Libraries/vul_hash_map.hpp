/*
 * Villains' Utility Library - Thomas Martin Schmid, 2014. Public domain¹
 *
 * This file describes hash map implementation.
 * Buckets are implemented as linked lists to deal with collissions.
 * The hash function used is given in the constructor, allowing this implementation
 * to be released under the licence (or lack thereof) described above while
 * still being able to take advantage of hash fucntions by smarter people than me.
 * @TODO: Dynamic slot-resizing; like resizable array, if population reaches a threshold,
 *        grow the bucket_count and rehash all elements into new buckets.
 *
 * For fast, good hash functions, these should be equivalent:
 *  - http://www.azillionmonkeys.com/qed/hash.html : LGPL 2.1
 *  - http://burtleburtle.net/bob/c/lookup3.c : Public Domain
 * There is also SpookyHash, which is a faster, but 64-bit only:
 *  - http://burtleburtle.net/bob/hash/spooky.html : Public Domain
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
#ifndef VUL_hash_map_t_HPP
#define VUL_hash_map_t_HPP

#include <cstddef>
#include <cassert>

/**
 * If enabled, iterators look for undefined/illegal behaviour during loops.
 * Slower, but might help track down ugly bugs!
 */
//#define VUL_DEBUG

#include "vul_linked_list.hpp"

namespace vul {

	typedef unsigned int ui32_t;
	/**
	 * Define the hash function type
	 */
	template< typename K >
	typedef ui32_t ( *hash_function )( const K &data, ui32_t len );
	/**
	 * Define the comparator function. While it takes hash_map_t_element_t 
	 * arguments, only keys should be compared.
	 */
	template< typename K, typename V >
	typedef int ( *compare_function )( const hash_map_t_element_t< K, V > &a, const hash_map_t_element_t< K, V > &b );

	/**
	 * Define the elements; the key-value pairs that make up the
	 * content of our map.
	 */
	template< typename K, typename V >
	class hash_map_t_element_t {
		K key;
		V value;
		hash_map_t_element_t( const K &key, const V &value );
	};

	template< typename K, typename V >
	hash_map_t_element_t::hash_map_t_element_t( const K &key, const V &value )
	{
		this->key = key;
		this->value = value;
	}

	/** 
	 * A hash map implementation with linked list buckets.
	 */
	template< typename K, typename V >
	class hash_map_t {

	private:
		/**
		 * Number of buckets
		 */
		ui32_t bucket_count;
		/**
		 * The head of each bucket. Uses the linked list implementation in vul_linked_list.hpp
		 */
		list_element_t< hash_map_t_element_t< K, V > > *buckets[ ];
		/** 
		 * The hash function.
		 */
		hash_function< K > hash;
		/** 
		 * The comparison function
		 */
		compare_function< K > comparator;

	public:
		/** 
		 * Creates a new hash map.
		 */
		hash_map_t( ui32_t bucket_count, hash_function hash, compare_function comparison_func );
		/**
		 * Deletes a hash map.
		 */
		~hash_map_t( );

		/**
		 * Insert a key-value pair into the hash map. Returns a pointer to the new hash_map_t_element_t.
		 */
		const hash_map_t_element_t &insert( const K &key, const V &value );		
		/**
		 * Removes the element matching the key from the hash map. Returnes the matching Value.
		 */
		const V &remove( const K &key );
		/**
		 * Returnes the element matching the given key.
		 */
		hash_map_t_element_t &get( const K &key );
		/**
		 * Returnes a constant referenct to the element matching the given key.
		 */
		const hash_map_t_element_t &get_const( const K &key ) const;
		/**
		 * Returnes the value matching the given key.
		 */
		const V &get_value( const K &key );
	};

	template< typename K, typename V >
	hash_map_t< K, V >::hash_map_t( ui32_t bucket_count, hash_function hash, compare_function comparison_func )
	{
		this->bucket_count = bucket_count;
		this->hash = hash;
		this->comparator = comparison_func;
		this->buckets = new list_element_t< hash_map_t_element_t< K, V > >*[ bucket_count ];
		assert( this->buckets != NULL );
		memset( this->buckets, this->bucket_count * sizeof( list_element_t< hash_map_t_element_t< K, V > >* ), 0 );
	}
	
	template< typename K, typename V >
	hash_map_t< K, V >::~hash_map_t( )
	{
		// For every bucket
		for( ui32_t b = 0; b < this->bucket_count; ++b )
		{
			// Delete the entire list
			list_element_t< hash_map_t_element_t< K, V > > *head = this->buckets[ b ], *next;
			while( head ) {
				next = head->next;
				delete head;
				head = next;
			}
		}
		// Delete the bucket pointer array.
		delete [] this->buckets;
	}
	
	template< typename K, typename V >
	const hash_map_t_element_t &hash_map_t< K, V >::insert( const K &key, const V &value )
	{
		// Find the gucket
		ui32_t bucket = this->hash( key ) % this->bucket_count;

		// Insert it into the bucket
		if( this->buckets[ bucket ] == NULL ) {
			// Create the bucket, since it doesn't yet exist
			this->buckets[ bucket ] = new list_element_t( hash_map_t_element_t< K, V >( key, value ) );
			return this->buckets[ bucket ];
		} else {
			return this->buckets[ bucket ]->insert( hash_map_t_element_t< K, V >( key, value ), this->comparator );
		}
	}
	template< typename K, typename V >
	const V &hash_map_t< K, V >::remove( const K &key )
	{
		// Find the gucket
		ui32_t bucket = this->hash( key ) % this->bucket_count;
		
		// Find the element
		list_element_t< hash_map_t_element_t< K, V > > *e = this->buckets[ bucket ]->find( hash_map_t_element_t< K, V >( key, V( ) ), this->comparator, true );
		if( e != NULL  {
			V ret = e->value;
			delete e;
			return ret;
		} else {
			return V( );
		}
	}
	template< typename K, typename V >
	hash_map_t_element_t &hash_map_t< K, V >::get( const K &key )
	{
		// Find the gucket
		ui32_t bucket = this->hash( key ) % this->bucket_count;
		
		// Find the element and return it
		return *this->buckets[ bucket ]->find( hash_map_t_element_t< K, V >( key, V( ) ), this->comparator, true );
		
	}
	template< typename K, typename V >
	const hash_map_t_element_t &hash_map_t< K, V >::get_const( const K &key ) const
	{
		// Find the gucket
		ui32_t bucket = this->hash( key ) % this->bucket_count;
		
		// Find the element and return it
		return *this->buckets[ bucket ]->find( hash_map_t_element_t< K, V >( key, V( ) ), this->comparator, true );
	}
	template< typename K, typename V >
	const V &hash_map_t< K, V >::get_value( const K &key )
	{
		// Find the gucket
		ui32_t bucket = this->hash( key ) % this->bucket_count;
		
		// Find the element
		list_element_t< hash_map_t_element_t< K, V > > *e = this->buckets[ bucket ]->find( hash_map_t_element_t< K, V >( key, V( ) ), this->comparator, true );

		// If the element is not null, return the value
		if( e != NULL ) {
			return e->value;
		} else {
			return V( ); // Return an empty value otherwise
		}
	}
}

#endif