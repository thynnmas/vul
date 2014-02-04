/*
 * Villains' Utility Library - Thomas Martin Schmid, 2014. Public domain¹
 *
 * This file includes the various datatypes of vul. At the moment, this includes:
 *	-General graph
 *	-Linked list (doubly linked)
 *	-Skip list
 *	-Queue
 *	-Resizable array (vector)
 *	-Hash map (albeit without hash functions)
 * While not including:
 *	-Fibonacci heap (@TODO)
 *	-Simpler heap (@TODO)
 *	-Stack; since the vector can be used for this with no extra work.
 * As of now proper iteration is not supported. This is however on my todo list, see:
 * @TODO: Proper iterators for all data types, see http://nothings.org/computer/iterate.html
 *		  Might want to write fast, simple iterators as well, expose them as constIterate
 *		  and require that the underlying data structure is unaltered by the iteration.
 *
 * @TODO: Properly test all of these!
 * 
 * ¹ If public domain is not legally valid in your country and or legal area,
 *   the MIT licence applies (see the LICENCE file)
 */

#include "vul_graph.hpp"
#include "vul_linked_list.hpp"
#include "vul_skip_list.hpp"
#include "vul_queue.hpp"
#include "vul_resizable_array.hpp"
#include "vul_hash_map.hpp"