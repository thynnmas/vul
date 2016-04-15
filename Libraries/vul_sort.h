/*
 * Villains' Utility Library - Thomas Martin Schmid, 2016. Public domain?
 *
 * This file contains a collection of sorting algorithms for the data structures
 * included in vul.
 * Current sorting algorithms for vul_vector:
 *  -Insertion sort ( Fastest for very small vectors [0, 10^3] )
 *	-Shell sort	( Fastest for small vectors [10^3, 10^4] )
 *  -Quicksort ( Fastest for medium sized vectors [10^4 ,10^5] )
 *  -Thynnsort ( Fastest for large vectors [10^4, inf] )
 * @TODO: Make a stable version of vul_sort that doesn't use shell sort.
 * @TODO: Bottom-up merge sort
 * @TODO: Heap sort
 * @TODO: Radix sort
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
#ifndef VUL_SORT_H
#define VUL_SORT_H

#include <assert.h>
#include <stdlib.h>
#ifndef VUL_OSX
#include <malloc.h>
#endif

// Define in exactly_one_ C/CPP file.
//#define VUL_DEFINE

#ifndef VUL_DEFINE
	#include "vul_resizable_array.h"
#endif

/**
 * Minimum number of elements per merge run (thynnsort)
 */
#define VUL_SORT_THYNN_MIN_MERGE 64
/**
 * Minimum number of elements to gallop over (thynnsort)
 */
#define VUL_SORT_THYNN_MIN_GALLOP 7

/**
 * Defines the minimum size for vuL_sort_vector() to use Shell sort
 * instead of insertion sort.
 */
#define VUL_SORT_MIN_SIZE_USE_SHELL 16
/**
 * Defines the minimum size for vuL_sort_vector() to use Thynnsort
 * instead of shell sort. Must be larger than VUL_SORT_MIN_SIZE_USE_SHELL
 */
#define VUL_SORT_MIN_SIZE_USE_THYNN 2048

/**
 * Entry in the merge stack. Contains a pair of first entry and run length.
 */
struct vul__sort_merge_stack_pair {
	s32 base;
	s32 length;
};

#ifdef _cplusplus
extern "C" {
#endif
/**
 * Sorts the given vector based on the result of the given comparator function.
 * Sorts only the part defined by the low and high arguments.
 * For array size: s < VUL_SORT_MIN_SIZE_USE_SHELL | s < VUL_SORT_MIN_SIZE_USE_THYNN |		else
 * we use sort:	           insertion sort		   |			shell sort			 |   thynn sort
 */
void vul_sort_vector( vul_vector *list, s32 (*comparator)( const void *a, const void *b ), s32 low, s32 high );
/*
 * Sorts the given vector based on the result of the given comparator function.
 * Sorts only the part defined by the low and high arguments.
 * Uses shell sort. Marcin Ciura's gap squence with inner insertion sort.
 */
void vul_sort_vector_shell( vul_vector *list, s32 (*comparator)( const void *a, const void *b ), s32 low, s32 high );
/*
 * Sorts the given vector based on the result of the given comparator function.
 * Sorts only the part defined by the low and high arguments.
 * Uses iterative quicksort with an auxiliary stack.
 */
void vul_sort_vector_quick( vul_vector *list, s32 (*comparator)( const void *a, const void *b ), s32 low, s32 high );
/**
 * Sorts the given vector based on  the result of the given comparator function.
 * Sorts only the part defined by the low and high arguments.
 * Takes an optional start parameter, where start dictates where to start sorting;
 * this indicates that the list is already sorted up to the start point.
 * Uses insertion sort
 */
void vul_sort_vector_insertion( vul_vector *list, s32 (*comparator)( const void *a, const void *b ), s32 low, s32 high, s32 start );
/**
 * Sorts the given vector based on the result of the given comparator function.
 * Uses a variation of timsort (http://bugs.python.org/file4451/timsort.txt) dubbed thynnsort.
 * @NOTE: Since shell sort is unstable, this is an unstable sort. Probably want a normal TimSort
 * version around that IS stable!
 */
void vul_sort_vector_thynn( vul_vector *list, s32 (*comparator)( const void *a, const void *b ), s32 low, s32 high );

//------------------
// Helpers

/**
 * Checks that low and high are in range for the given list
 */
static s32 vul__sort_vector_check_range( vul_vector *list, s32 low, s32 high );
/**
 * Helper function for vul_sort_vector_quick.
 * Partitions the part of the array defined by low and high into two subarrays.
 */
static s32 vul__sort_vector_quick_partition( vul_vector *list, s32 (*comparator)( const void *a, const void *b ), s32 low, s32 high );
/**
 * Helper function for vul_sort_vector_tim
 * Reverses a range within a list.
 */
static void vul__sort_reverse_range( vul_vector *list, s32 low, s32 high );
/**
 * Helper function for vul_sort_vector_thynn.
 * Counts the number of elements starting from low that are already sorted.
 * If the first two elemnts are in reverse order, it counts the number
 * of elements that are reversely sorted, then reverses the list.
 */
static s32 vul__sort_count_ascend_run( vul_vector *list, s32 (*comparator)( const void *a, const void *b ), s32 low, s32 high );
/**
 * Helper function for vul_sort_vector_thynn.
 * Computes a sensible minimum run length for the merge sort
 * step in thynnsort based on the list's mength.
 */
static s32 vul__sort_compute_minrun( s32 length );
/**
 * Helper function for vul_sort_vector_thynn.
 * Finds the spot in list[base, base + length] where key should be inserted
 * or the rightmost equal element. Hint indicates where to start searching.
 */
static s32 vul__sort_gallop_right( const void *key, vul_vector *list, s32 (*comparator)( const void *a, const void *b ), s32 base, s32 length, s32 hint );
/**
 * Helper function for vul_sort_vector_thynn.
 * Finds the spot in list[base, base + length] where key should be inserted
 * or the leftmost equal element. Hint indicates where to start searching.
 */
static s32 vul__sort_gallop_left( const void *key, vul_vector *list, s32 (*comparator)( const void *a, const void *b ), s32 base, s32 length, s32 hint );
/**
 * Helper function for vul_sort_vector_thynn.
 * Merges two adjacent runs, in a stable fashion. list[ base1 ] > list[ base2 ] must
 * be true, as well as list[ base1 + length1 - 1 ] > all elements in list [ base2, base2 + length2 ].
 * Also demands length1 <= length2
 */
static void vul__sort_merge_low( vul_vector *list, s32 (*comparator)( const void *a, const void *b ), s32 base1, s32 length1, s32 base2, s32 length2 );
/**
 * Helper function for vul_sort_vector_thynn.
 * Merges two adjacent runs, in a stable fashion. list[ base1 ] > list[ base2 ] must
 * be true, as well as list[ base1 + length1 - 1 ] > all elements in list [ base2, base2 + length2 ].
 * Also demands length1 >= length2
 */
static void vul__sort_merge_high( vul_vector *list, s32 (*comparator)( const void *a, const void *b ), s32 base1, s32 length1, s32 base2, s32 length2 );
/**
 * Helper function for vul_sort_vector_thynn.
 * Takes a vector of vul__sort_merge_stack_pair elements.
 * and the index if the lower element of the two to merge.
 * Then merges elements i and i+1.
 */
static void vul__sort_merge_at( vul_vector *list, s32 (*comparator)( const void *a, const void *b ), s32 i, vul_vector *stack );
/**
 * Helper function for vul_sort_vector_thynn.
 * Takes a vector of vul__sort_merge_stack_pair elements.
 * Merges runs until the sort invariant is fulfilled.
 * @NOTE(thynn): This includes the fix described here: 
 * http://envisage-project.eu/proving-android-java-and-python-sorting-algorithm-is-broken-and-how-to-fix-it/
 */
static void vul__sort_merge_collapse( vul_vector *list, s32 (*comparator)( const void *a, const void *b ), vul_vector *stack );
/**
 * Helper function for vul_sort_vector_thynn.
 * Takes a vector of vul__sort_merge_stack_pair elements.
 * Merges runs until only one remains
 */
static void vul__sort_force_merge_collapse( vul_vector *list, s32 (*comparator)( const void *a, const void *b ), vul_vector *stack );
#ifdef _cplusplus
}
#endif
#endif

#ifdef VUL_DEFINE

#ifdef _cplusplus
extern "C" {
#endif

static s32 vul__sort_vector_check_range( vul_vector *list, s32 low, s32 high )
{
	return low <= high && low >= 0 && high <= ( s32 )vul_vector_size( list );
}

void vul_sort_vector_shell( vul_vector *list, s32 (*comparator)( const void *a, const void *b ), s32 low, s32 high )
{
	void *temp, *left, *right, *item;
	s32 gap, gi, i, j;
	s32 gaps[ ] = { 4071001, 1170001, 237001, 67001, 17001, 5001, 1701, 701, 301, 132, 67, 23, 10, 4, 1 }; // @TODO(thynn): Larger numbers!
	
	// Check range
	assert( vul__sort_vector_check_range( list, low, high ) && "vul_sort_vector_shell: Range check failed" );

	temp = malloc( list->element_size );
	assert( temp != NULL ); // Make sure malloc didn't fail

	for ( gi = 0; gi < sizeof( gaps ) / sizeof( s32 ); ++gi ) {
		// Insertion sort for each gap size
		for ( i = low + gaps[ gi ]; i <= high; ++i ) {
			gap = gaps[ gi ];
			item = vul_vector_get( list, i );
			memcpy( temp, item, list->element_size );
			for ( j = i; j >= gap && comparator( vul_vector_get_const( list, j - gap), temp ) > 0; j-= gap ) {
				left = vul_vector_get( list, j );
				right = vul_vector_get( list, j - gap );
				memcpy( left, right, list->element_size );
			}
			item = vul_vector_get( list, j );
			memcpy( item, temp, list->element_size );
		}
	}
}

static s32 vul__sort_vector_quick_partition( vul_vector *list, s32 (*comparator)( const void *a, const void *b ), s32 low, s32 high )
{
	const void *pivot;
	s32 i, j, p;
		
	// Random pivot
	p = ( s32 )( rand( ) % ( high - low ) ) + low;
	// Move pivot to end and grab value
	vul_vector_swap( list, p, high );
	pivot = vul_vector_get_const( list, high );
	
	i = low;
	
	for( j = low; j < high; ++j )
	{
		if( comparator( vul_vector_get_const( list, j ), pivot ) <= 0 )
		{
			vul_vector_swap( list, j , i++ );
		}
	}

	vul_vector_swap( list, i, high );

	return i;
}

void vul_sort_vector_quick( vul_vector *list, s32 (*comparator)( const void *a, const void *b ), s32 low, s32 high )
{
	s32 *stack;
	s32 top, pivot;
	
	// Check range
	assert( vul__sort_vector_check_range( list, low, high ) && "vul_sort_vector_quick: Range check failed" );
	stack = ( s32* )malloc( sizeof( s32 ) * ( high - low + 1 ) );
	assert( stack != NULL && "vul_sort_vector_quick: Could not allocate memory for the stack" );
    top = -1;
	
	stack[ ++top ] = low;
    stack[ ++top ] = high;

    while ( top >= 0 )
    {
        high = stack[ top-- ];
        low = stack[ top-- ];

        pivot = vul__sort_vector_quick_partition( list, comparator, low, high );
		if ( pivot - 1 > low )
        {
            stack[ ++top ] = low;
            stack[ ++top ] = pivot - 1;
        }
		if ( pivot + 1 < high )
        {
            stack[ ++top ] = pivot + 1;
            stack[ ++top ] = high;
        }
    }

	free( stack );

}

void vul_sort_vector_insertion( vul_vector *list, s32 (*comparator)( const void *a, const void *b ), s32 low, s32 high, s32 start )
{
	s32 left, right, mid, n;
	void *pivot;
	
	// Corerct auto-arguments
	if ( start <= low ) {
		start = low + 1;
	} else if ( start > high ) {
		start = high;
	}
	// Check range
	assert( vul__sort_vector_check_range( list, low, high ) && "vul_sort_vector_insertion: Range check failed" );
	
	// Alloc pivot
	pivot = malloc( list->element_size );
	assert( pivot != NULL ); // Make sure malloc didn't fail
	while( start <= high )
	{
		memcpy( pivot, vul_vector_get_const( list, start ), list->element_size );
		left = low;
		right = start;
		if( left > right ) {
			break;
		}
		while( left < right )
		{
			mid = ( left + right ) >> 1;
			if( comparator( pivot, vul_vector_get_const( list, mid ) ) < 0 )
			{
				right = mid;
			} else {
				left = mid + 1;
			}
		}
		assert( left == right && "vul_sort_vector_insertion: Something is wrong in binary sort" );

		for( n = start - left; n > 0; --n ) {
			memcpy( vul_vector_get( list, left + n ), vul_vector_get( list, left + ( n - 1 ) ), list->element_size );
		}
		memcpy( vul_vector_get( list, left ), pivot, list->element_size );
		++start;
	}
	// Free pivot
	free( pivot );
}

static void vul__sort_reverse_range( vul_vector *list, s32 low, s32 high )
{
	--high;
	while( low < high )
	{
		vul_vector_swap( list, low++, high-- );
	}
}

static s32 vul__sort_count_ascend_run( vul_vector *list, s32 (*comparator)( const void *a, const void *b ), s32 low, s32 high )
{
	s32 i;

	assert( low < high && "vul__sort_count_ascend_run: Cannot count a 0-length run." );
	if ( high - low == 1 ) {
		return 1; // Trivially
	}

	i = low + 1;
	if( comparator( vul_vector_get_const( list, i++ ), vul_vector_get_const( list, low ) ) < 0 )
	{
		// List is descending, count descending elements
		while( i < high && comparator( vul_vector_get_const( list, i ), vul_vector_get_const( list, i - 1 ) ) < 0 ) {
			++i;
		}
		// And reverse those elements
		vul__sort_reverse_range( list, low, i );
	} else {
		// List is ascending, count ascending elements
		while( i < high && comparator( vul_vector_get_const( list, i ), vul_vector_get_const( list, i - 1 ) ) <= 0 ) {
			++i;
		}
	}

	return i - low;
}

static s32 vul__sort_compute_minrun( s32 length )
{
	s32 r;

	assert( length >= 0 && "vul__sort_compute_minrun: Length is less than 0" );
	r = 0;
	while( length >= VUL_SORT_THYNN_MIN_MERGE )
	{
		r |= length & 1;
		length >>= 1;
	}
	return length + r;
}

static s32 vul__sort_gallop_right( const void *key, vul_vector *list, s32 (*comparator)( const void *a, const void *b ), s32 base, s32 length, s32 hint )
{
	s32 last_ofs, ofs, max_ofs, tmp;

	// Check range
	assert( length > 0
			&& hint >= 0
			&& hint < length
			&& "vul__sort_gallop_right: Range is invalid" );

	last_ofs = 0;
	ofs = 1;
	if( comparator( key, vul_vector_get_const( list, base + hint ) ) >= 0 )
	{
		// Gallop right
		max_ofs = length - hint;
		while( ofs < max_ofs && comparator( key, vul_vector_get_const( list, base + hint + ofs ) ) >= 0 )
		{
			last_ofs = ofs;
			ofs = ( ofs << 1 ) + 1;
			if( ofs <= 0 ) { // Overflow
				ofs = max_ofs;
			}
		}
		if( ofs > max_ofs ) {
			ofs = max_ofs; // It's the last element
		}
		last_ofs += hint;
		ofs += hint;
	} else {
		// Gallop left
		max_ofs = 1 + hint;
		while( ofs < max_ofs && comparator( key, vul_vector_get_const( list, base + hint - ofs ) ) < 0 )
		{
			last_ofs = ofs;
			ofs = ( ofs << 1 ) + 1;
			if( ofs <= 0 ) { // Overflow
				ofs = max_ofs;
			}
		}
		if( ofs > max_ofs ) {
			ofs = max_ofs; // It's the last element
		}
		tmp = last_ofs;
		last_ofs = hint - ofs;
		ofs = hint - tmp;
	}
	
	// Check that ofs and last_ofs are sane
	assert( -1 <= last_ofs
			&& last_ofs < ofs
			&& ofs <= length
			&& "vul__sort_gallop_right: Offset calculation has gone a-stray." );
	
	// list[ base + last_ofs ] < key <= list[ base + ofs ]. Binary search the rest of the way
	++last_ofs;
	while( last_ofs < ofs )
	{
		tmp = last_ofs + ( ( ofs - last_ofs ) >> 1 );
		if( comparator( key, vul_vector_get_const( list, base + tmp ) ) < 0 ) {
			ofs = tmp;
		} else {
			last_ofs = tmp + 1;
		}
	}
	assert( last_ofs == ofs && "vul__sort_gallop_right: Binary search has failed." );

	return ofs;
}

static s32 vul__sort_gallop_left( const void *key, vul_vector *list, s32 (*comparator)( const void *a, const void *b ), s32 base, s32 length, s32 hint )
{
	s32 last_ofs, ofs, max_ofs, tmp;

	// Check range
	assert( length > 0
			&& hint >= 0
			&& hint < length
			&& "vul__sort_gallop_left: Range is invalid" );

	last_ofs = 0;
	ofs = 1;
	if( comparator( key, vul_vector_get_const( list, base + hint ) ) > 0 )
	{
		// Gallop right
		max_ofs = length - hint;
		while( ofs < max_ofs && comparator( key, vul_vector_get_const( list, base + hint + ofs ) ) > 0 )
		{
			last_ofs = ofs;
			ofs = ( ofs << 1 ) + 1;
			if( ofs <= 0 ) { // Overflow
				ofs = max_ofs;
			}
		}
		if( ofs > max_ofs ) {
			ofs = max_ofs; // It's the last element
		}
		last_ofs += hint;
		ofs += hint;
	} else {
		// Gallop left
		max_ofs = 1 + hint;
		while( ofs < max_ofs && comparator( key, vul_vector_get_const( list, base + hint - ofs ) ) <= 0 )
		{
			last_ofs = ofs;
			ofs = ( ofs << 1 ) + 1;
			if( ofs <= 0 ) { // Overflow
				ofs = max_ofs;
			}
		}
		if( ofs > max_ofs ) {
			ofs = max_ofs; // It's the last element
		}
		tmp = last_ofs;
		last_ofs = hint - ofs;
		ofs = hint - tmp;
	}
	
	// Check that ofs and last_ofs are sane
	assert( -1 <= last_ofs
			&& last_ofs < ofs
			&& ofs <= length
			&& "vul__sort_gallop_left: Offset calculation has gone a-stray." );
	
	// list[ base + last_ofs ] < key <= list[ base + ofs ]. Binary search the rest of the way
	++last_ofs;
	while( last_ofs < ofs )
	{
		tmp = last_ofs + ( ( ofs - last_ofs ) >> 1 );
		if( comparator( key, vul_vector_get_const( list, base + tmp ) ) > 0 ) {
			last_ofs = tmp + 1;
		} else {
			ofs = tmp;
		}
	}
	assert( last_ofs == ofs && "vul__sort_gallop_left: Binary search has failed." );

	return ofs;
}

static void vul__sort_merge_low( vul_vector *list, s32 (*comparator)( const void *a, const void *b ), s32 base1, s32 length1, s32 base2, s32 length2 )
{
	vul_vector *temp_list = vul_vector_create( list->element_size, length1, 
												 list->allocator,
												 list->deallocator,
												 list->reallocator );
	s32 c1, c2, dest, minG, running, count1, count2;

	// Range checks
	assert( length1 > 0
			&& length2 > 0
			&& ( base1 + length1 ) == base2
			&& "vul__sort_merge_low: Ranges are invalid." );

	// Copy 1st list to a temporary list
	vul_vector_copy_partial( temp_list, 0, list, base1, length1 );
	
	// Move the first element of second run and deal with degenerate cases
	c1 = 0;
	c2 = base2;
	dest = base1;
	memcpy( vul_vector_get( list, dest++ ), vul_vector_get( list, c2++ ), list->element_size );
	if( --length2 == 0 ) {
		memcpy( vul_vector_get( list, dest ), vul_vector_get( temp_list, c1 ), length1 * list->element_size );
		return;
	}
	if( length1 == 1 ) {
		memcpy( vul_vector_get( list, dest ), vul_vector_get( list, c2 ), length2 * list->element_size );
		memcpy( vul_vector_get( list, dest + length2 ), vul_vector_get( temp_list, c1 ), list->element_size );
		return;
	}

	// Gallop
	minG = VUL_SORT_THYNN_MIN_GALLOP;
	running = 1;
	while( running )
	{
		count1 = 0;
		count2 = 0;

		// Simple until one run wins
		while( count1 < minG && count2 < minG ) 
		{
			// Check sanity of lengths
			assert( length1 > 1
					&& length2 > 0
					&& "vul__sort_merge_low: Length problems in traversal stage." );

			if( comparator( vul_vector_get_const( list, c2 ), vul_vector_get_const( temp_list, c1 ) ) < 0 )
			{
				memcpy( vul_vector_get( list, dest++ ), vul_vector_get( list, c2++ ), list->element_size );
				++count2;
				count1 = 0;
				if( --length2 == 0 ) {
					running = 0;
					break;
				}
			} else {
				memcpy( vul_vector_get( list, dest++ ), vul_vector_get( temp_list, c1++ ), list->element_size );
				++count1;
				count2 = 0;
				if( --length1 == 1 ) {
					running = 0;
					break;
				}
			}
		}

		// If done, break
		if ( !running ) {
			break;
		}
		
		// Now we have one run that is dominating, so gallop instead until domination stops
		while( count1 >= VUL_SORT_THYNN_MIN_GALLOP || count2 >= VUL_SORT_THYNN_MIN_GALLOP )
		{
			// Check sanity of lengths
			assert( length1 > 1
					&& length2 > 0
					&& "vul__sort_merge_low: Length problems in gallop stage." );

			count1 = vul__sort_gallop_right( vul_vector_get_const( list, c2 ), temp_list, comparator, c1, length1, 0 );
			if( count1 != 0 )
			{
				memcpy( vul_vector_get( list, dest ), vul_vector_get( temp_list, c1 ), count1 * list->element_size );
				dest += count1;
				c1 += count1;
				length1 -= count1;
				if( length1 <= 1 ) {
					running = 0;
					break;
				}
			}
			memcpy( vul_vector_get( list, dest++ ), vul_vector_get( list, c2++ ), list->element_size );
			if( --length2 == 0 ) {
				running = 0;
				break;
			}

			count2 = vul__sort_gallop_left( vul_vector_get_const( temp_list, c1 ), list, comparator, c2, length2, 0 );
			if( count2 != 0 )
			{
				memcpy( vul_vector_get( list, dest ), vul_vector_get( list, c2 ), count2 * list->element_size );
				dest += count2;
				c2 += count2;
				length2 -= count2;
				if( length2 == 0 ) {
					running = 0;
					break;
				}
			}
			memcpy( vul_vector_get( list, dest++ ), vul_vector_get( temp_list, c1++ ), list->element_size );
			if( --length1 == 1 ) {
				running = 0;
				break;
			}
			--minG;
		}
		// Break if done
		if( !running ) {
			break;
		}
		// If gallopsize is sub-zero, set it to 0
		if( minG < 0 ) {
			minG = 0;
		}
		minG += 2;
	}

	if( length1 == 1 ) {
		assert( length2 > 0 && "vul__sort_merge_low: Length2 is too small." );
		memcpy( vul_vector_get( list, dest ), vul_vector_get( list, c2 ), length2 * list->element_size );
		memcpy( vul_vector_get( list, dest + length2 ), vul_vector_get( temp_list, c1 ), list->element_size );
	} else if ( length1 == 0 ) {
		
	} else {
		assert( length2 == 0
				&& length1 > 1
				&& "vul__sort_merge_low: Lengths are not sane at the end of function." );
		memcpy( vul_vector_get( list, dest ), vul_vector_get( temp_list, c1 ), length1 * list->element_size );
	}
	vul_vector_destroy( temp_list );
}

static void vul__sort_merge_high( vul_vector *list, s32 (*comparator)( const void *a, const void *b ), s32 base1, s32 length1, s32 base2, s32 length2 )
{
	vul_vector *temp_list = vul_vector_create( list->element_size, length2 + 1,
												 list->allocator,
												 list->deallocator,
												 list->reallocator );
	s32 c1, c2, dest, minG, running, count1, count2;

	// Range checks
	assert( length1 > 0
			&& length2 > 0
			&& ( base1 + length1 ) == base2
			&& "vul__sort_merge_high: Ranges are invalid." );

	// Copy 1st list to a temporary list
	vul_vector_copy_partial( temp_list, 0, list, base2, length2 );
	
	// Move the first element of second run and deal with degenerate cases
	c1 = base1 + length1 - 1;
	c2 = length2 - 1;
	dest = base2 + length2 - 1;
	memcpy( vul_vector_get( list, dest-- ), vul_vector_get( list, c1-- ), list->element_size );
	if( --length1 == 0 ) {
		memcpy( vul_vector_get( list, dest - ( length2 - 1 ) ), vul_vector_get( temp_list, 0 ), length2 * list->element_size );
		return;
	}
	if( length2 == 1 ) {
		dest -= length1;
		c1 -= length1;
		memcpy( vul_vector_get( list, dest + 1 ), vul_vector_get( list, c1 + 1 ), length1 * list->element_size );
		memcpy( vul_vector_get( list, dest ), vul_vector_get( temp_list, c2 ), list->element_size );
		return;
	}

	// Gallop
	minG = VUL_SORT_THYNN_MIN_GALLOP;
	running = 1;
	while( running )
	{
		count1 = 0;
		count2 = 0;

		// Simple until one run wins
		while( count1 < minG && count2 < minG ) 
		{
			// Check sanity of lengths
			assert( length1 > 0
					&& length2 > 1
					&& "vul__sort_merge_high: Length problems in traversal stage." );

			if( comparator( vul_vector_get_const( temp_list, c2 ), vul_vector_get_const( list, c1 ) ) < 0 )
			{
				memcpy( vul_vector_get( list, dest-- ), vul_vector_get( list, c1-- ), list->element_size );
				++count1;
				count2 = 0;
				if( --length1 == 0 ) {
					running = 0;
					break;
				}
			} else {
				memcpy( vul_vector_get( list, dest-- ), vul_vector_get( temp_list, c2-- ), list->element_size );
				++count2;
				count1 = 0;
				if( --length2 == 1 ) {
					running = 0;
					break;
				}
			}
		}

		// If done, break
		if ( !running ) {
			break;
		}
		
		// Now we have one run that is dominating, so gallop instead until domination stops
		while( count1 >= VUL_SORT_THYNN_MIN_GALLOP || count2 >= VUL_SORT_THYNN_MIN_GALLOP )
		{
			// Check sanity of lengths
			assert( length1 > 0
					&& length2 > 1
					&& "vul__sort_merge_high: Length problems in gallop stage." );

			count1 = length1 - vul__sort_gallop_right( vul_vector_get_const( temp_list, c2 ), list, comparator, base1, length1, length1 - 1 );
			if( count1 != 0 )
			{
				dest -= count1;
				c1 -= count1;
				length1 -= count1;
				memcpy( vul_vector_get( list, dest + 1 ), vul_vector_get( list, c1 + 1 ), count1 * list->element_size );
				if( length1 == 0 ) {
					running = 0;
					break;
				}
			}
			memcpy( vul_vector_get( list, dest-- ), vul_vector_get( temp_list, c2-- ), list->element_size );
			if( --length2 == 1 ) {
				running = 0;
				break;
			}

			count2 = length2 - vul__sort_gallop_left( vul_vector_get_const( list, c1 ), temp_list, comparator, 0, length2, length2 - 1 );
			if( count2 != 0 )
			{
				dest -= count2;
				c2 -= count2;
				length2 -= count2;
				memcpy( vul_vector_get( list, dest + 1 ), vul_vector_get( temp_list, c2 + 1 ), count2 * list->element_size );
				if( length2 <= 1 ) {
					running = 0;
					break;
				}
			}
			memcpy( vul_vector_get( list, dest-- ), vul_vector_get( list, c1-- ), list->element_size );
			if( --length1 == 0 ) {
				running = 0;
				break;
			}
			--minG;
		}
		// Break if done
		if( !running ) {
			break;
		}
		// If gallopsize is sub-zero, set it to 0
		if( minG < 0 ) {
			minG = 0;
		}
		minG += 2;
	}

	if( length2 == 1 ) {
		assert( length1 > 0 && "vul__sort_merge_high: Length1 is too small." );
		dest -= length1;
		c1 -= length1;
		memcpy( vul_vector_get( list, dest + 1 ), vul_vector_get( list, c1 + 1 ), length1 * list->element_size );
		memcpy( vul_vector_get( list, dest ), vul_vector_get( temp_list, c2 ), list->element_size );
	} else if ( length2 == 0 ) {
	} else {
		assert( length1 == 0
				&& length2 > 1
				&& "vul__sort_merge_high: Lengths are not sane at the end of function." );
		memcpy( vul_vector_get( list, dest - ( length2 - 1 ) ), vul_vector_get( temp_list, 0 ), length2 * list->element_size );
	}
	vul_vector_destroy( temp_list );
}

static void vul__sort_merge_at( vul_vector *list, s32 (*comparator)( const void *a, const void *b ), s32 i, vul_vector *stack )
{
	struct vul__sort_merge_stack_pair *r1, *r2;
	const struct vul__sort_merge_stack_pair *r3;
	s32 k, b1, b2, l1, l2;

	// Check that the merge is legal
	assert( vul_vector_size( stack ) >= 2 
			&& i >= 0
			&& ( i == vul_vector_size( stack ) - 2 || i == vul_vector_size( stack ) - 3 )
			&& "vul__sort_merge_at: Invalid stack index given for merge." );
	
	// Get run information
	r1 = ( struct vul__sort_merge_stack_pair* )vul_vector_get( stack, i );
	b1 = r1->base;
	l1 = r1->length;
	r2 = ( struct vul__sort_merge_stack_pair* )vul_vector_get( stack, i + 1 );
	b2 = r2->base;
	l2 = r2->length;

	// Check that things are sane
	assert( r1->length > 0 
			&& r2->length > 0
			&& ( r1->base + r1->length ) == r2->base
			&& "vul__sort_merge_at: Runs given for merge don't align." );

	// Get combined run length and if i is 3rd last run, slide over the last run (which is not involved in this merge).
	r1->length = r1->length + r2->length;
	if( i == vul_vector_size( stack ) - 3 ) {
		r3 = ( const struct vul__sort_merge_stack_pair* )vul_vector_get_const( stack, i + 2 );
		r2->base = r3->base;
		r2->length = r3->length;
	}
	// Pop last elemnt on stack.
	vul_vector_remove_cascade( stack, vul_vector_size( stack ) - 1 );

	// Find out where first element of second run goes in first run
	k = vul__sort_gallop_right( vul_vector_get_const( list, b2 ), list, comparator, b1, l1, 0 );
	assert( k >= 0 && "vul__sort_merge_at: vul__sort_gallop_right returned a sub-zero value" );
	b1 += k;
	l1 -= k;
	if( l1 == 0 ) {
		return; // Already in the right order
	}
	
	// Find out where the last element of the first run goes in second run
	l2 = vul__sort_gallop_left( vul_vector_get_const( list, b1 + l1 - 1 ), list, comparator, b2, l2, l2 - 1 );
	assert( l2 >= 0 && "vul__sort_merge_at: vul__sort_gallop_left returned a sub-zero value" );
	if( l2 == 0 ) {
		return; // Already in correct order
	}

	// Merge the remaining runs
	if( l1 <= l2 ) {
		vul__sort_merge_low( list, comparator, b1, l1, b2, l2 );
	} else {
		vul__sort_merge_high( list, comparator, b1, l1, b2, l2 );
	}
}

static void vul__sort_merge_collapse( vul_vector *list, s32 (*comparator)( const void *a, const void *b ), vul_vector *stack )
{
	const struct vul__sort_merge_stack_pair *en, *en1, *enm1, *enm2;
	s32 n;

	while( vul_vector_size( stack ) > 1 )
	{
		n = vul_vector_size( stack ) - 2;
		
		en = ( const struct vul__sort_merge_stack_pair* )vul_vector_get_const( stack, n );
		en1 = ( const struct vul__sort_merge_stack_pair* )vul_vector_get_const( stack, n + 1 );
		
		// Some compiler really want this, so give it to them, even though it's not needed
		enm1 = NULL; enm2 = NULL;
		if(	   ( n     > 0 && ( enm1 = ( const struct vul__sort_merge_stack_pair* )vul_vector_get_const( stack, n - 1 ) )->length <= en->length +  en1->length )
			|| ( n - 1 > 0 && ( enm2 = ( const struct vul__sort_merge_stack_pair* )vul_vector_get_const( stack, n - 2 ) )->length <= en->length + enm1->length ) ) {
			if( enm1->length < en1->length ) {
				--n;
			}
		} else if( n < 0 || en->length > en1->length ) {
			break;
		}
		vul__sort_merge_at( list, comparator, n, stack );
	}
}

static void vul__sort_force_merge_collapse( vul_vector *list, s32 (*comparator)( const void *a, const void *b ), vul_vector *stack )
{
	const struct vul__sort_merge_stack_pair *en1, *enm1;
	s32 n;

	while( vul_vector_size( stack ) > 1 )
	{
		n = vul_vector_size( stack ) - 2;
		
		en1 = ( const struct vul__sort_merge_stack_pair * )vul_vector_get_const( stack, n + 1 );
			
		if( n > 0 && 
			( enm1 = ( const struct vul__sort_merge_stack_pair * )vul_vector_get_const( stack, n - 1 ) )->length
				< en1->length ) {
			--n;
		}
		vul__sort_merge_at( list, comparator, n, stack );
	}
}

void vul_sort_vector_thynn( vul_vector *list, s32 (*comparator)( const void *a, const void *b ), s32 low, s32 high )
{
	// Setup
	s32 n, f, run_length, min_run_length;
	struct vul__sort_merge_stack_pair *current;
	vul_vector *merge_stack = vul_vector_create( sizeof( struct vul__sort_merge_stack_pair ), 0,
												 list->allocator,
												 list->deallocator,
												 list->reallocator );

	// Check range
	assert( vul__sort_vector_check_range( list, low, high ) && "vul_sort_vector_thynn: Range check failed" );

	// Trivial case
	n = high - low;
	if( n < 2 ) {		
		// Free the stack
		vul_vector_destroy( merge_stack );
		return;
	}

	// Short runs, no need to merge sort
	if( n < VUL_SORT_THYNN_MIN_MERGE ) {
		// Find run length
		run_length = vul__sort_count_ascend_run( list, comparator, low, high );
		
		if( run_length > ( VUL_SORT_THYNN_MIN_MERGE >> 2 ) ) {
			// Run is pretty long; go with shell sort
			vul_sort_vector_shell( list, comparator, low, high );
		} else {
			// Short run; insertion sort. Ignore the run
			vul_sort_vector_insertion( list, comparator, low, high, low + run_length );
		}

		// Free the stack
		vul_vector_destroy( merge_stack );
		return;
	}

	// Long run, merge sort with gallop
	// Traverse the array (low to high) to find natural runs. Extend short ones to
	// a minimum length (see vul__sort_compute_minrun) and merge to maintain invariant.
	min_run_length = vul__sort_compute_minrun( n );
	while( n > 0 )
	{
		// Find run length
		run_length = vul__sort_count_ascend_run( list, comparator, low, high );
		// Extend it if it's too short
		if( run_length < min_run_length )
		{
			f = ( n <= min_run_length ) ? n : min_run_length;
			
			// Sort the run
			if( f > ( VUL_SORT_THYNN_MIN_MERGE >> 2 ) ) {
				// Run is pretty long; go with shell sort
				vul_sort_vector_shell( list, comparator, low, low + f );
			} else {
				// Short run; insertion sort. Ignore the run
				vul_sort_vector_insertion( list, comparator, low, low + f, low + run_length );
			}
			// Update run length
			run_length = f;
		}

		// Push to the merge stack
		current = ( struct vul__sort_merge_stack_pair* )vul_vector_add_empty( merge_stack );
		current->base = low;
		current->length = run_length;

		// Merge if time is right
		vul__sort_merge_collapse( list, comparator, merge_stack );

		// Move on to next run
		low += run_length;
		n -= run_length;
	}

	// Check that low has converged to high
	assert( low == high && "vul_sort_vector_thynn: Low and high have not converged." );

	// Force the remaining merges
	vul__sort_force_merge_collapse( list, comparator, merge_stack );

	// Check that stack has signle entry
	assert( vul_vector_size( merge_stack ) == 1 && "vul_sort_vector_thynn: Stack size is not 1, merge not complete." );

	// Free the stack
	vul_vector_destroy( merge_stack );
}

void vul_sort_vector( vul_vector *list, s32 (*comparator)( const void *a, const void *b ), s32 low, s32 high )
{
	s32 s;

	s = vul_vector_size( list );
	if( s < 2 ) {
		return; // Trivial case
	} else if( s > VUL_SORT_MIN_SIZE_USE_THYNN ) {
		vul_sort_vector_thynn( list, comparator, low, high );
	} else if ( s > VUL_SORT_MIN_SIZE_USE_SHELL ) {
		vul_sort_vector_shell( list, comparator, low, high );
	} else {
		vul_sort_vector_insertion( list, comparator, low, high, 0 );
	}
}

#ifdef _cplusplus
}
#endif

#endif