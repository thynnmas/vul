/*
 * Villains' Utility Library - Thomas Martin Schmid, 2014. Public domain¹
 *
 * This file contains auxilliary functions for benchmarking.
 * @TODO: Proper statistics; at the moment this does the programmer's
 * hacky equivalent; specify iteration count and calculate mean,
 * median and standard deviation.
 * @TODO: Plotting.
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

#include "vul_types.h"
#include "vul_timer.h"
#include "vul_sort.h"

#include <stdarg.h>

typedef struct vul_benchmark_result {
	f64_t mean;
	ui64_t median;
	f64_t std_deviation;
} vul_benchmark_result;

/**
 * Helper function used to find the median. In all honesty, this is the median-of-median
 * algorithm, and all the median-function does is pretend the k-argument doesn't exist.
 */
ui32_t vul__benchmark_select( ui64_t *times, ui32_t left, ui32_t right, ui32_t k )
{
	ui64_t order[ 5 ], time;
	ui32_t i, l, count, median;
	i32_t j;

	count = right - left;
	if( count < 5 )
	{
		// Just find the median element the hard way
		l = 1;
		order[ 0 ] = times[ left ];
		for( i = 1; i < count; ++i ) 
		{
			time = times[ left + i ];
			if( time < order[ l - 1 ] ) {
				for( j = l - 1; j >= 0 && time < order[ j ]; --j )
				{
					order[ j + 1 ] = order[ j ];
				}
				order[ ++j ] = time;
				++l;
			} else {
				order[ l++ ] = time;
			}
		}
		for( i = 0; i < 5; ++i ) {
			if ( order[ k ] == times[ left + i ] ) {
				return left + i;
			}
		}
	}

	for( i = 0; i < count / 5; ++i )
	{
		l = 5 * i; // Left bound of new array
		j = ( l + 4 ) > right ? right : ( l + 4 ); // Right bound of new array
		
		median = vul__benchmark_select( times, l, j, 3 );

		time = times[ median ];
		times[ median ] = times[ i ];
		times[ i ] = time;
	}

	return vul__benchmark_select( times, 0, count / 5, count / 10 );
}

/**
 * Finds the median element of an array of times. O(n)
 */
ui64_t vul__benchmark_median( ui64_t *times, ui32_t left, ui32_t right )
{
	return times[ vul__benchmark_select( times, left, right, ( right - left ) / 2 ) ];
}

/**
 * Calculates the mean of an array of times. O(n)
 */
f64_t vul__benchmark_mean( ui64_t *times, ui32_t left, ui32_t right )
{
	ui32_t i;
	f64_t avg, count;

	count = ( f64_t )( right - left );
	avg = 0;
	for( i = left; i < right; ++i )
	{
		avg += ( f64_t )times[ i ] / count;
	}

	return avg;
}

/**
 * Calculates the stad_deviation of an array of times given the mean of the array.  O(n)
 */
f64_t vul__benchmark_standard_deviation( ui64_t *times, ui32_t left, ui32_t right, ui64_t mean )
{
	ui32_t i;
	ui64_t d;
	f64_t dev, inv_count;

	inv_count = 1.0 / ( f64_t )( ( right - left ) - 1 );
	dev = 0;
	for( i = left; i < right; ++i )
	{
		d = times[ i ] - mean;
		d *= d;
		dev += ( f64_t )d * inv_count;
	}

	return sqrt( dev );
}

/**
 * Runs the given function the given amount of times, calculating mean, median and
 * std_deviation over the runs. All times in milliseconds.
 * The given function is of type void function( va_list, element_count_in_va_list ).
 */
vul_benchmark_result vul_benchmark_millis( ui32_t repetitions, void ( *function )( va_list ), ... )
{
	ui32_t r;
	ui64_t *times;
	vul_timer_t *clk;
	vul_benchmark_result res;
	va_list argp;

	times = ( ui64_t* )malloc( sizeof ui64_t * repetitions );
	clk = vul_timer_create( );

	va_start( argp, function );

	// Run the benchmark
	for( r = 0; r < repetitions; ++r ) {
		vul_timer_reset( clk );
		function( argp );
		times[ r ] = vul_timer_get_millis( clk );
	}
	
	va_end( argp );

	res.mean = vul__benchmark_mean( times, 0, repetitions );
	res.median = vul__benchmark_median( times, 0, repetitions );
	res.std_deviation = vul__benchmark_standard_deviation( times, 0, repetitions, res.mean );

	vul_timer_destroy( clk );

	return res;
}

/**
 * Runs the given function the given amount of times, calculating mean, median and
 * std_deviation over the runs. All times in microseconds.
 * The given function is of type void function( va_list ).
 */
vul_benchmark_result vul_benchmark_micros( ui32_t repetitions, void ( *function )( va_list ), ... )
{
	ui32_t r;
	ui64_t *times;
	vul_timer_t *clk;
	vul_benchmark_result res;
	va_list argp;

	times = ( ui64_t* )malloc( sizeof ui64_t * repetitions );
	clk = vul_timer_create( );

	va_start( argp, function );

	// Run the benchmark
	for( r = 0; r < repetitions; ++r ) {
		vul_timer_reset( clk );
		function( argp );
		times[ r ] = vul_timer_get_micros( clk );
	}
	
	va_end( argp );

	res.mean = vul__benchmark_mean( times, 0, repetitions - 1 );
	res.median = vul__benchmark_median( times, 0, repetitions - 1 );
	res.std_deviation = vul__benchmark_standard_deviation( times, 0, repetitions - 1, res.mean );

	vul_timer_destroy( clk );

	return res;
}