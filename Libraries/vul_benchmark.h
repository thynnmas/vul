/*
 * Villains' Utility Library - Thomas Martin Schmid, 2015. Public domain?
 *
 * This file contains auxilliary functions for benchmarking.
 * @TODO: Proper statistics; at the moment this does the programmer's
 * hacky equivalent; specify iteration count and calculate mean,
 * median and standard deviation.
 * @TODO: Plotting (bar diagram of all iterations, histogram, smooth graf)
 * nanovg should be useful for this; hide the whole thing behind a define.
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

#include "vul_types.h"
#include "vul_timer.h"
#include "vul_sort.h"

#include <stdarg.h>

typedef struct vul_benchmark_result {
	ui32_t iterations;
	f64_t mean;
	ui64_t median;
	f64_t std_deviation;
} vul_benchmark_result;


/**
 * Helper function used to find the median. This is the median-of-median
 * algorithm, and all the median-function does is pretend the k-argument doesn't exist.
 */
uint32_t vul__benchmark_select( uint64_t *times, uint32_t left, uint32_t right, uint32_t k )
{
	uint64_t order[ 5 ], time;
	uint32_t i, l, count, median;
	int32_t j;

	count = right - left;
	if( count < 5 )
	{
		// Just find the k-th element the hard way
		l = 1;
		order[ 0 ] = times[ left ];
		for( i = 1; i < count + 1; ++i ) 
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

	for( i = 0; i < ( count / 5 ) + 1; ++i )
	{
		l = 5 * i; // Left bound of new array
		j = ( l + 4 ) > right ? right : ( l + 4 ); // Right bound of new array
		
		median = vul__benchmark_select( times, l, j, 2 ); // 0-indexed k

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
f64_t vul__benchmark_standard_deviation( ui64_t *times, ui32_t left, ui32_t right, f64_t mean )
{
	ui32_t i;
	f64_t d, dev, inv_count;

	inv_count = 1.0 / ( f64_t )( ( right - left ) - 1 );
	dev = 0;
	for( i = left; i < right; ++i )
	{
		d = ( f64_t )times[ i ] - mean;
		d *= d;
		dev += d * inv_count;
	}

	return sqrt( dev );
}

/**
 * Runs the given function the given amount of times, calculating mean, median and
 * std_deviation over the runs. All times in milliseconds.
 */
vul_benchmark_result vul_benchmark_millis( ui32_t repetitions, 
										   void ( *function )( void *data ), void* func_data )
{
	ui32_t r;
	ui64_t *times;
	vul_timer_t *clk;
	vul_benchmark_result res;

	times = ( ui64_t* )malloc( sizeof( ui64_t ) * repetitions );
	clk = vul_timer_create( );

	// Run the benchmark
	for( r = 0; r < repetitions; ++r ) {
		vul_timer_reset( clk );
		function( func_data );
		times[ r ] = vul_timer_get_millis( clk );
	}
	
	res.iterations = repetitions;
	res.mean = vul__benchmark_mean( times, 0, repetitions );
	res.median = vul__benchmark_median( times, 0, repetitions );
	res.std_deviation = vul__benchmark_standard_deviation( times, 0, repetitions, res.mean );

	vul_timer_destroy( clk );

	return res;
}


/**
 * Runs the given function the given amount of times, calculating mean, median and
 * std_deviation over the runs. All times in microseconds.
 */
vul_benchmark_result vul_benchmark_micros( ui32_t repetitions, 
										   void ( *function )( void *data ), void *func_data )
{
	ui32_t r;
	ui64_t *times;
	vul_timer_t *clk;
	vul_benchmark_result res;

	times = ( ui64_t* )malloc( sizeof( ui64_t ) * repetitions );
	clk = vul_timer_create( );

	// Run the benchmark
	for( r = 0; r < repetitions; ++r ) {
		vul_timer_reset( clk );
		function( func_data );
		times[ r ] = vul_timer_get_micros( clk );
	}
	
	res.mean = vul__benchmark_mean( times, 0, repetitions - 1 );
	res.median = vul__benchmark_median( times, 0, repetitions - 1 );
	res.std_deviation = vul__benchmark_standard_deviation( times, 0, repetitions - 1, res.mean );
	res.iterations = repetitions;

	vul_timer_destroy( clk );

	return res;
}

/**
 * Runs the given function, calculating mean, median and
 * std_deviation over the runs, until the given percentage of samples (ci) is expected
 * to lie in the interval mean +- error * mean.
 * It will run at least min_iter iterations, and at most max_iter.
 */
vul_benchmark_result vul_benchmark_millis_confidence( f32_t ci, f32_t error, ui32_t min_iter, ui32_t max_iter,
																		void ( *function )( void *data ), void *func_data )
{
	ui32_t iter, count;
	ui64_t *times;
	vul_timer_t *clk;
	vul_benchmark_result res;

	times = 0;
	iter = 0;
	count = min_iter;
	clk = vul_timer_create( );

	while( count <= max_iter ) {
		// (re)allocate the times array, maintaining the values we already have
		if( times == NULL ) {
			times = ( ui64_t* )malloc( sizeof( ui64_t ) * count );
		} else {
			times = ( ui64_t )realloc( times, sizeof( ui64_t ) * count );
		}
		// Run the benchmark
		while( iter < count ) {
			vul_timer_reset( clk );
			function( func_data );
			times[ iter++ ] = vul_timer_get_millis( clk );
		}
		
		// Calculate important values to determine if finished
		res.mean = vul__benchmark_mean( times, 0, count - 1 );
		res.std_deviation = vul__benchmark_standard_deviation( times, 0, count - 1, res.mean );
		res.iterations = count;

		// Calculate teh size of error relative to the std.dev.
		f64_t z = error / res.std_deviation;
		// The percentage of samples expected to be in the CI is given as erf(z/sqrt(2))
		f64_t eci = erf( z / sqrt( 2.f ) );
		// If we have the desired precision, break
		if( eci <= ci ) {
			break;
		}
		count *= 2;
		if( count > max_iter ) {
			count = max_iter;
		}
	}
	// Calculate the median after, because it's slow and we don't need it each run
	res.median = vul__benchmark_median( times, 0, count - 1 );

	vul_timer_destroy( clk );

	return res;
}

/**
 * Runs the given function, calculating mean, median and
 * std_deviation over the runs, until the given percentage of samples (ci) is expected
 * to lie in the interval mean +- error * mean.
 * It will run at least min_iter iterations, and at most max_iter.
 */
vul_benchmark_result vul_benchmark_micros_confidence( f32_t ci, f32_t error, ui32_t min_iter, ui32_t max_iter,
																		void ( *function )( void *data ), void *func_data )
{
	ui32_t iter, count;
	ui64_t *times;
	vul_timer_t *clk;
	vul_benchmark_result res;

	times = 0;
	iter = 0;
	count = min_iter;
	clk = vul_timer_create( );

	while( count <= max_iter ) {
		// (re)allocate the times array, maintaining the values we already have
		if( times == NULL ) {
			times = ( ui64_t* )malloc( sizeof( ui64_t ) * count );
		} else {
			times = ( ui64_t )realloc( times, sizeof( ui64_t ) * count );
		}
		// Run the benchmark
		while( iter < count ) {
			vul_timer_reset( clk );
			function( func_data );
			times[ iter++ ] = vul_timer_get_micros( clk );
		}
		
		// Calculate important values to determine if finished
		res.mean = vul__benchmark_mean( times, 0, count - 1 );
		res.std_deviation = vul__benchmark_standard_deviation( times, 0, count - 1, res.mean );
		res.iterations = count;

		// Calculate teh size of error relative to the std.dev.
		f64_t z = error / res.std_deviation;
		// The percentage of samples expected to be in the CI is given as erf(z/sqrt(2))
		f64_t eci = erf( z / sqrt( 2.f ) );
		// If we have the desired precision, break
		if( eci <= ci ) {
			break;
		}
		count *= 2;
		if( count > max_iter ) {
			count = max_iter;
		}
	}
	// Calculate the median after, because it's slow and we don't need it each run
	res.median = vul__benchmark_median( times, 0, count - 1 );

	vul_timer_destroy( clk );

	return res;
}
typedef struct vul_benchmark_histogram {
	uint32_t *buckets, bucket_count, bucket_max;
	uint64_t smallest, largest;
} vul_benchmark_histogram;

void vul__benchmark_create_histogram( vul_benchmark_histogram *hist, uint64_t *times, 
												  uint32_t left, uint32_t right, uint32_t buckets )
{
	double s, l, r, t;
	uint32_t i, j;
	hist->bucket_count = buckets;
	
	// Find range
	hist->smallest = -1;
	hist->largest = 0;
	for( i = left; i < right; ++i ) {
		hist->smallest = times[ i ] < hist->smallest ? times[ i ] : hist->smallest;
		hist->largest = times[ i ] > hist->largest ? times[ i ] : hist->largest;
	}
	s = ( double )( hist->largest - hist->smallest ) / ( double )buckets;
	if( hist->largest - hist->smallest < ( uint64_t )buckets ) {
		uint32_t hb = buckets / 2;
		double med = ( double )hist->smallest + ( s * ( double )hb );
		s = 1.0;
		hist->smallest = med - s * ( double )hb;
		hist->largest = med + s * ( double )hb;
	}
	
	// Fill buckets
	hist->buckets = ( uint32_t* )malloc( sizeof( uint32_t ) * buckets );
	l = ( double )hist->smallest;
	r = ( double )hist->smallest + s;
	for( i = 0; i < buckets; ++i ) {
		hist->buckets[ i ] = 0;
		for( j = left; j < right; ++j ) {
			t = ( double )times[ j ];
			if( t >= l && ( t < r || ( i == buckets - 1  && t == ( double )hist->largest ) ) ) { 
				++hist->buckets[ i ];
			}
		}
		l = r;
		r += s;
	}

	// Find largest bucket
	hist->bucket_max = 0;
	for( i = 0; i < buckets; ++i ) {
		hist->bucket_max = hist->buckets[ i ] > hist->bucket_max ? hist->buckets[ i ] : hist->bucket_max;
	}
}

/**
 * Print a histogram to stdout with a given number of buckets.
 */
void vul_benchmark_print_histogram_millis( uint64_t *times, uint32_t left, uint32_t right, uint32_t buckets )
{
	uint32_t i, j, ml;
	uint64_t v, s;
	double l, r, ds;
	vul_benchmark_histogram hist;
	
	// Calcualte it
	vul__benchmark_create_histogram( &hist, times, left, right, buckets );
	
	// Print legend
	printf( "Time (ms) | Count |0" );
	ml = ( uint32_t )log10( ( double )hist.bucket_max );
	for( i = 1; i < buckets - ml; ++i ) printf(" ");
	printf( "%d|\n", hist.bucket_max );
	printf( "----------|-------|" );
	for( i = 0; i < buckets; ++i ) printf("-");
	printf("|\n");

	s = hist.bucket_max / buckets;
	ds = ( double )( hist.largest - hist.smallest ) / ( double )buckets;
	l = ( double )hist.smallest;
	r = l + ds;
	for( i = 0; i < buckets; ++i ) {
		v = 0;
		printf("%02.1f-%02.1f | %d", l, r, hist.buckets[ i ]);
		ml = ( uint32_t )log10( ( double )hist.buckets[ i ] );
		for( j = 0; j < 5 - ml; ++j ) printf(" ");
		printf("|");
		l = r;
		r += ds;
		for( j = 0; j < buckets && hist.buckets[ i ] > v; ++j ) {
			printf("*");
			v += s;
		}
		for( ; j < buckets; ++j ) printf(" ");
		printf("|\n");
	}
	printf("\n");

	// Clean up
	free( hist.buckets );
}

/**
 * Print a histogram to stdout with a given number of buckets.
 */
void vul_benchmark_print_histogram_micros( uint64_t *times, uint32_t left, uint32_t right, uint32_t buckets )
{
	uint32_t i, j, ml;
	uint64_t v, s;
	double l, r, ds;
	vul_benchmark_histogram hist;
	
	// Calcualte it
	vul__benchmark_create_histogram( &hist, times, left, right, buckets );
	
	// Print legend
	printf( "Time (ms)   | Count |0" );
	ml = ( uint32_t )log10( ( double )hist.bucket_max ) + 1;
	for( i = 1; i < buckets - ml; ++i ) printf(" ");
	printf( "%d|\n", hist.bucket_max );
	printf( "------------|-------|" );
	for( i = 0; i < buckets; ++i ) printf("-");
	printf("|\n");

	s = hist.bucket_max / buckets;
	ds = ( double )( hist.largest - hist.smallest ) / ( double )buckets;
	l = ( double )hist.smallest;
	r = l + ds;
	for( i = 0; i < buckets; ++i ) {
		v = 0;
		printf("%02.2f-%02.2f | %d", l / 1000.0, r / 1000.0, hist.buckets[ i ]);
		ml = ( uint32_t )log10( ( double )hist.buckets[ i ] );
		for( j = 0; j < 5 - ml; ++j ) printf(" ");
		printf("|");
		l = r;
		r += ds;
		for( j = 0; j < buckets && hist.buckets[ i ] > v; ++j ) {
			printf("*");
			v += s;
		}
		for( ; j < buckets; ++j ) printf(" ");
		printf("|\n");
	}
	printf("\n");

	// Clean up
	free( hist.buckets );
}
