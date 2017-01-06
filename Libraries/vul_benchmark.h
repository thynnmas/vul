/*
 * Villains' Utility Library - Thomas Martin Schmid, 2017. Public domain?
 *
 * This file contains auxilliary functions for benchmarking.
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
#ifndef VUL_BENCHMARK_H
#define VUL_BENCHMARK_H

#include "vul_timer.h"
#include "vul_sort.h"

#include <stdarg.h>
#include <stdio.h>

#ifndef VUL_TYPES_H
#include <stdint.h>
#define f32 float
#define f64 double
#define s32 uint32_t
#define u32 uint32_t
#define u64 uint64_t
#endif

typedef struct vul_benchmark_result {
   u32 iterations;
   f64 mean;
   u64 median;
   f64 std_deviation;
} vul_benchmark_result;

typedef struct vul_benchmark_histogram {
   u32 *buckets, bucket_count, bucket_max;
   u64 smallest, largest;
} vul_benchmark_histogram;

#ifdef __cplusplus
extern "C" {
#endif
/**
 * Runs the given function the given amount of times, calculating mean, median and
 * std_deviation over the runs. All times in milliseconds.
 */
vul_benchmark_result vul_benchmark_millis( u32 repetitions, 
                                           void ( *function )( void *data ), void* func_data );
/**
 * Runs the given function the given amount of times, calculating mean, median and
 * std_deviation over the runs. All times in microseconds.
 */
vul_benchmark_result vul_benchmark_micros( u32 repetitions, 
                                           void ( *function )( void *data ), void *func_data );
/**
 * Runs the given function, calculating mean, median and
 * std_deviation over the runs, until the given percentage of samples (ci) is expected
 * to lie in the interval mean +- error * mean.
 * It will run at least min_iter iterations, and at most max_iter.
 */
vul_benchmark_result vul_benchmark_millis_confidence( f32 ci, f32 error, u32 min_iter, u32 max_iter,
                                                      void ( *function )( void *data ), void *func_data );
/**
 * Runs the given function, calculating mean, median and
 * std_deviation over the runs, until the given percentage of samples (ci) is expected
 * to lie in the interval mean +- error * mean.
 * It will run at least min_iter iterations, and at most max_iter.
 */
vul_benchmark_result vul_benchmark_micros_confidence( f32 ci, f32 error, u32 min_iter, u32 max_iter,
                                                      void ( *function )( void *data ), void *func_data );
/**
 * Print a histogram to stdout with a given number of buckets.
 */
void vul_benchmark_print_histogram_millis( u64 *times, u32 left, u32 right, u32 buckets );
/**
 * Print a histogram to stdout with a given number of buckets.
 */
void vul_benchmark_print_histogram_micros( u64 *times, u32 left, u32 right, u32 buckets );

#ifdef __cplusplus
}
#endif

#ifndef VUL_TYPES_H
#undef f32
#undef f64
#undef s32
#undef u32
#undef u64
#endif

#endif // VUL_BENCHMARK_H

#ifdef VUL_DEFINE

#ifndef VUL_TYPES_H
#define f32 float
#define f64 double
#define s32 uint32_t
#define u32 uint32_t
#define u64 uint64_t
#endif

#ifdef __cplusplus
extern "C" {
#endif
/**
 * Helper function used to find the median. This is the median-of-median
 * algorithm, and all the median-function does is pretend the k-argument doesn't exist.
 */
static u32 vul__benchmark_select( u64 *times, u32 left, u32 right, u32 k );
/**
 * Finds the median element of an array of times. O(n)
 */
static u64 vul__benchmark_median( u64 *times, u32 left, u32 right );
/**
 * Calculates the mean of an array of times. O(n)
 */
static f64 vul__benchmark_mean( u64 *times, u32 left, u32 right );
/**
 * Calculates the stad_deviation of an array of times given the mean of the array.  O(n)
 */
static f64 vul__benchmark_standard_deviation( u64 *times, u32 left, u32 right, f64 mean );
/**
 * Create a histogram from a range in a time array with a given number of buckets.
 */
static void vul__benchmark_create_histogram( vul_benchmark_histogram *hist, u64 *times, 
                                             u32 left, u32 right, u32 buckets );

//-------------------------
// Implementation
//

u32 vul__benchmark_select( u64 *times, u32 left, u32 right, u32 k )
{
   u64 order[ 5 ], time;
   u32 i, l, count, median;
   s32 j;

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

u64 vul__benchmark_median( u64 *times, u32 left, u32 right )
{
   return times[ vul__benchmark_select( times, left, right, ( right - left ) / 2 ) ];
}

f64 vul__benchmark_mean( u64 *times, u32 left, u32 right )
{
   u32 i;
   f64 avg, count;

   count = ( f64 )( right - left );
   avg = 0;
   for( i = left; i < right; ++i )
   {
      avg += ( f64 )times[ i ] / count;
   }

   return avg;
}

f64 vul__benchmark_standard_deviation( u64 *times, u32 left, u32 right, f64 mean )
{
   u32 i;
   f64 d, dev, inv_count;

   inv_count = 1.0 / ( f64 )( ( right - left ) - 1 );
   dev = 0;
   for( i = left; i < right; ++i )
   {
      d = ( f64 )times[ i ] - mean;
      d *= d;
      dev += d * inv_count;
   }

   return sqrt( dev );
}

vul_benchmark_result vul_benchmark_millis( u32 repetitions, 
                                           void ( *function )( void *data ), void* func_data )
{
   u32 r;
   u64 *times;
   vul_timer *clk;
   vul_benchmark_result res;

   times = ( u64* )malloc( sizeof( u64 ) * repetitions );
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

vul_benchmark_result vul_benchmark_micros( u32 repetitions, 
                                           void ( *function )( void *data ), void *func_data )
{
   u32 r;
   u64 *times;
   vul_timer *clk;
   vul_benchmark_result res;

   times = ( u64* )malloc( sizeof( u64 ) * repetitions );
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

vul_benchmark_result vul_benchmark_millis_confidence( f32 ci, f32 error, u32 min_iter, u32 max_iter,
                                                      void ( *function )( void *data ), void *func_data )
{
   u32 iter, count;
   u64 *times;
   vul_timer *clk;
   vul_benchmark_result res;

   times = 0;
   iter = 0;
   count = min_iter;
   clk = vul_timer_create( );

   while( count <= max_iter ) {
      // (re)allocate the times array, maintaining the values we already have
      if( times == NULL ) {
         times = ( u64* )malloc( sizeof( u64 ) * count );
      } else {
         times = ( u64* )realloc( times, sizeof( u64 ) * count );
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
      f64 z = error / res.std_deviation;
      // The percentage of samples expected to be in the CI is given as erf(z/sqrt(2))
      f64 eci = erf( z / sqrt( 2.f ) );
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

vul_benchmark_result vul_benchmark_micros_confidence( f32 ci, f32 error, u32 min_iter, u32 max_iter,
                                                      void ( *function )( void *data ), void *func_data )
{
   u32 iter, count;
   u64 *times;
   vul_timer *clk;
   vul_benchmark_result res;

   times = 0;
   iter = 0;
   count = min_iter;
   clk = vul_timer_create( );

   while( count <= max_iter ) {
      // (re)allocate the times array, maintaining the values we already have
      if( times == NULL ) {
         times = ( u64* )malloc( sizeof( u64 ) * count );
      } else {
         times = ( u64* )realloc( times, sizeof( u64 ) * count );
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
      f64 z = error / res.std_deviation;
      // The percentage of samples expected to be in the CI is given as erf(z/sqrt(2))
      f64 eci = erf( z / sqrt( 2.f ) );
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

void vul__benchmark_create_histogram( vul_benchmark_histogram *hist, u64 *times, 
                                      u32 left, u32 right, u32 buckets )
{
   f64 s, l, r, t;
   u32 i, j;
   hist->bucket_count = buckets;
   
   // Find range
   hist->smallest = -1;
   hist->largest = 0;
   for( i = left; i < right; ++i ) {
      hist->smallest = times[ i ] < hist->smallest ? times[ i ] : hist->smallest;
      hist->largest = times[ i ] > hist->largest ? times[ i ] : hist->largest;
   }
   s = ( f64 )( hist->largest - hist->smallest ) / ( f64 )buckets;
   if( hist->largest - hist->smallest < ( u64 )buckets ) {
      u32 hb = buckets / 2;
      f64 med = ( f64 )hist->smallest + ( s * ( f64 )hb );
      s = 1.0;
      hist->smallest = med - s * ( f64 )hb;
      hist->largest = med + s * ( f64 )hb;
   }
   
   // Fill buckets
   hist->buckets = ( u32* )malloc( sizeof( u32 ) * buckets );
   l = ( f64 )hist->smallest;
   r = ( f64 )hist->smallest + s;
   for( i = 0; i < buckets; ++i ) {
      hist->buckets[ i ] = 0;
      for( j = left; j < right; ++j ) {
         t = ( f64 )times[ j ];
         if( t >= l && ( t < r || ( i == buckets - 1  && t == ( f64 )hist->largest ) ) ) { 
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

void vul_benchmark_print_histogram_millis( u64 *times, u32 left, u32 right, u32 buckets )
{
   u32 i, j, ml;
   u64 v, s;
   f64 l, r, ds;
   vul_benchmark_histogram hist;
   
   // Calcualte it
   vul__benchmark_create_histogram( &hist, times, left, right, buckets );
   
   // Print legend
   printf( "Time (ms) | Count |0" );
   ml = ( u32 )log10( ( f64 )hist.bucket_max );
   for( i = 1; i < buckets - ml; ++i ) printf(" ");
   printf( "%d|\n", hist.bucket_max );
   printf( "----------|-------|" );
   for( i = 0; i < buckets; ++i ) printf("-");
   printf("|\n");

   s = hist.bucket_max / buckets;
   ds = ( f64 )( hist.largest - hist.smallest ) / ( f64 )buckets;
   l = ( f64 )hist.smallest;
   r = l + ds;
   for( i = 0; i < buckets; ++i ) {
      v = 0;
      printf("%02.1f-%02.1f | %d", l, r, hist.buckets[ i ]);
      ml = ( u32 )log10( ( f64 )hist.buckets[ i ] );
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

void vul_benchmark_print_histogram_micros( u64 *times, u32 left, u32 right, u32 buckets )
{
   u32 i, j, ml;
   u64 v, s;
   f64 l, r, ds;
   vul_benchmark_histogram hist;
   
   // Calcualte it
   vul__benchmark_create_histogram( &hist, times, left, right, buckets );
   
   // Print legend
   printf( "Time (ms)   | Count |0" );
   ml = ( u32 )log10( ( f64 )hist.bucket_max ) + 1;
   for( i = 1; i < buckets - ml; ++i ) printf(" ");
   printf( "%d|\n", hist.bucket_max );
   printf( "------------|-------|" );
   for( i = 0; i < buckets; ++i ) printf("-");
   printf("|\n");

   s = hist.bucket_max / buckets;
   ds = ( f64 )( hist.largest - hist.smallest ) / ( f64 )buckets;
   l = ( f64 )hist.smallest;
   r = l + ds;
   for( i = 0; i < buckets; ++i ) {
      v = 0;
      printf("%02.2f-%02.2f | %d", l / 1000.0, r / 1000.0, hist.buckets[ i ]);
      ml = ( u32 )log10( ( f64 )hist.buckets[ i ] );
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

#ifdef __cplusplus
}
#endif

#ifndef VUL_TYPES_H
#undef f32
#undef f64
#undef s32
#undef u32
#undef u64
#endif

#endif // VUL_DEFINE
