/*
 * Villains' Utility Library - Thomas Martin Schmid, 2015. Public domain¹
 *
 * This file describes a templated general graph as well as a few useful functions on them.
 * @TODO: Functions on it (BFS, DFS, Dijkstra etc.)
 *		  This will require user-data in edge_t.
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

#ifndef VUL_GRAPH_HPP
#define VUL_GRAPH_HPP

#include <cassert>
#include <cstring>

#include "vul_resizable_array.hpp"

namespace vul {

	template< typename T >
	class node_t;
	/**
	 * Class that holds edge information.
	 */
	template< typename T >
	class edge_t {
		node_t< T > source;
		node_t< T > destination;
		
		/** 
		 * Create a new edge.
		 */
		edge_t( const node_t< T > &src, const node_t< T > &dst );
	};

	template< typename T >
	edge_t< T >::edge_t( const node_t< T > &src, const node_t< T > &dst )
	{
		source = src;
		destination = dst;
	}

	/**
	 * Nodes in a general graph; data and a list of edges.
	 */
	template< typename T >
	class node_t {
		/**
		 * Contains the content of the node.
		 */
		T data;
		/**
		 * Contains all the edges of the node.
		 */
		vector_t< edge_t< T > > edges;

		/**
		 * Useful meta for graph traversal.
		 */
		bool visited;

		/**
		 * Creates a new node.
		 */
		node_t( const T &data );
		/**
		 * Deletes a node.
		 */
		~node_t( );
		
		/**
		 * Adds an edge from this node to the given one. Edges are one-way.
		 */
		void add_edge( const node_t< T > &dst );

	};

	template< typename T >
	node_t< T >::node_t( const T &data )
	{
		this->data = data;
		this->edges.initialize( );
	}
	template< typename T >
	node_t< T >::~node_t( )
	{
		this->edges.finish( );
	}
	template< typename T >
	void node_t< T >::add_edge( const node_t< T > &dst )
	{
		this->edges.add( edge_t( *this, dst ) );
	}
}

#endif