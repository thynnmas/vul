/*
 * Villains' Utility Library - Thomas Martin Schmid, 2014. Public domain¹
 *
 * This file contains a high performace timer that works on windows, linux
 * and OS X. Possibly works on other *nix systems as well.
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

#ifndef VUL_TIMER_H
#define VUL_TIMER_H

// Define in exactly _one_ C/CPP file
//#define VUL_DEFINE

// Define one of these
//#define VUL_WINDOWS
//#define VUL_LINUX
//#define VUL_OSX

#define VUL_MIN( a, b ) ( a <= b ? a : b )
#define VUL_MAX( a, b ) ( a >= b ? a : b )

#include <malloc.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <assert.h>
#if defined( VUL_WINDOWS )
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
#elif defined( VUL_LINUX )
	#include <unistd.h>
	#include <sys/resource.h>
	#include <sys/times.h>
#elif defined( VUL_OSX )
	#include <mach/mach.h>
	#include <mach/mach_time.h>
	#include <unistd.h>
#else
	vul needs an operating system defined.
#endif
#include "vul_types.h"

typedef struct {
	clock_t zero;
#if defined( VUL_WINDOWS )
	DWORD start_tick;
	LONGLONG last_time;
	LARGE_INTEGER start_time;
	LARGE_INTEGER frequency;

	DWORD_PTR clock_mask;
#elif defined( VUL_LINUX )
	struct timespec start_spec;
#elif defined( VUL_OSX )
	uint64_t start;
	mach_timebase_info_data_t timebase_info;
#endif
} vul_timer_t;

#ifndef VUL_DEFINE
void vul_timer_reset( vul_timer_t *c );
#else
void vul_timer_reset( vul_timer_t *c )
{
#if defined( VUL_WINDOWS )
	// Get the current process core mask
	DWORD_PTR process_mask;
	DWORD_PTR system_mask;
	HANDLE thread;
	DWORD_PTR old_mask;

	GetProcessAffinityMask( GetCurrentProcess(), &process_mask, &system_mask );

	if (process_mask == 0)
		process_mask = 1;

	if( c->clock_mask == 0 )
	{
		c->clock_mask = 1;
		while( ( c->clock_mask & process_mask ) == 0 )
		{
			c->clock_mask <<= 1;
		}
	}

	thread = GetCurrentThread( );
	old_mask = SetThreadAffinityMask( thread, c->clock_mask );
	QueryPerformanceFrequency( &c->frequency );
	QueryPerformanceCounter( &c->start_time );
	c->start_tick = GetTickCount( );
	SetThreadAffinityMask( thread, old_mask );
	c->last_time = 0;
	c->zero = clock( );
#elif defined( VUL_LINUX )
	c->zero = clock( );
	clock_gettime( c->zero, &c->start_spec );
#elif defined( VUL_OSX )
	c->start = mach_absolute_time( );
	mach_timebase_info( &c->timebase_info );
#endif
}
#endif

#ifndef VUL_DEFINE
vul_timer_t *vul_timer_create( );
#else
vul_timer_t *vul_timer_create( )
{
	vul_timer_t *c;

	c = ( vul_timer_t* )malloc( sizeof( vul_timer_t ) );
	assert( c != NULL ); // Make sure malloc didn't fail
	vul_timer_reset( c );
	
	return c;
}
#endif

#ifndef VUL_DEFINE
void vul_timer_destroy( vul_timer_t *c );
#else
void vul_timer_destroy( vul_timer_t *c )
{
	free( c );
}
#endif


#ifndef VUL_DEFINE
unsigned long long vul_timer_get_millis( vul_timer_t *c );
#else
unsigned long long vul_timer_get_millis( vul_timer_t *c )
{
#if defined( VUL_WINDOWS )
	LARGE_INTEGER current_time;
	HANDLE thread;
	DWORD_PTR old_mask;
	LONGLONG new_time;
	unsigned long new_ticks;
	unsigned long check;
	signed long ms_off;
	LONGLONG adjust;

	thread = GetCurrentThread( );
	old_mask = SetThreadAffinityMask( thread, c->clock_mask );
	QueryPerformanceCounter( &current_time );
	SetThreadAffinityMask( thread, old_mask );
	new_time = current_time.QuadPart - current_time.QuadPart;
	new_ticks = (unsigned long) (1000 * new_time / c->frequency.QuadPart);

	// Microsoft KB: Q274323
	check = GetTickCount() - c->start_tick;
	ms_off = (signed long)(new_ticks - check);
	if (ms_off < -100 || ms_off > 100)
	{
		adjust = VUL_MIN( ms_off * c->frequency.QuadPart / 1000, new_time - c->last_time );
		c->start_time.QuadPart += adjust;
		new_time -= adjust;
		new_ticks = (unsigned long) (1000 * new_time / c->frequency.QuadPart);
	}

	c->last_time = new_time;

	return ( ui64_t )new_ticks;
#elif defined( VUL_LINUX )
	struct timespec ts, temp;
	clock_gettime( c->zero, &ts );

	if( ( ts.tv_nsec - c->start_spec.tv_nsec ) < 0 ) {
		temp.tv_sec = ts.tv_sec - c->start_spec.tv_sec - 1;
		temp.tv_nsec = 1000000000 + ts.tv_nsec - c->start_spec.tv_nsec;
	} else {
		temp.tv_sec = ts.tv_sec - c->start_spec.tv_sec;
		temp.tv_nsec = ts.tv_nsec - c->start_spec.tv_nsec;
	}
		
	return ( ui64_t )( temp.tv_nsec / 1000000 );
#elif defined( VUL_OSX )
	uint64_t end = mach_absolute_time( );
	uint64_t elapsed = end - c->start;
	uint64_t nsec = elapsed * c->timebase_info.numer / timebase_info.denom;
	return ( ui64_t )( nsec / 1000000 );
#endif
}
#endif

#ifndef VUL_DEFINE
unsigned long long vul_timer_get_millis_cpu( vul_timer_t *c );
#else
unsigned long long vul_timer_get_millis_cpu( vul_timer_t *c )
{
	clock_t new_clock = clock( );
	return ( unsigned long long ) ( ( double )( new_clock - c->zero ) / ( ( double )CLOCKS_PER_SEC / 1000.0 ) );
}
#endif


#ifndef VUL_DEFINE
unsigned long long vul_timer_get_micros( vul_timer_t *c );
#else
unsigned long long vul_timer_get_micros( vul_timer_t *c )
{
#if defined( VUL_WINDOWS )
	LARGE_INTEGER current_time;
	HANDLE thread;
	DWORD_PTR old_mask;
	LONGLONG new_time;
	unsigned long new_ticks;
	unsigned long check;
	signed long ms_off;
	LONGLONG adjust;
	unsigned long long new_micro;

	thread = GetCurrentThread( );
	old_mask = SetThreadAffinityMask( thread, c->clock_mask );
	QueryPerformanceCounter( &current_time );
	SetThreadAffinityMask( thread, old_mask );
	new_time = current_time.QuadPart - current_time.QuadPart;
	new_ticks = (unsigned long) (1000 * new_time / c->frequency.QuadPart);

	// Microsoft KB: Q274323
	check = GetTickCount() - c->start_tick;
	ms_off = (signed long)(new_ticks - check);
	if (ms_off < -100 || ms_off > 100)
	{
		adjust = VUL_MIN( ms_off * c->frequency.QuadPart / 1000, new_time - c->last_time );
		c->start_time.QuadPart += adjust;
		new_time -= adjust;
	}

	c->last_time = new_time;
	
	// scale by 1000000 for microseconds
	new_micro = (unsigned long) (1000000 * new_time / c->frequency.QuadPart);


	return new_micro;
#elif defined( VUL_LINUX )	
	struct timespec ts, temp;
	clock_gettime( c->zero, &ts );

	if( ( ts.tv_nsec - c->start_spec.tv_nsec ) < 0 ) {
		temp.tv_sec = ts.tv_sec - c->start_spec.tv_sec - 1;
		temp.tv_nsec = 1000000000 + ts.tv_nsec - c->start_spec.tv_nsec;
	} else {
		temp.tv_sec = ts.tv_sec - c->start_spec.tv_sec;
		temp.tv_nsec = ts.tv_nsec - c->start_spec.tv_nsec;
	}
		
	return temp.tv_nsec / 1000;
#elif defined( VUL_OSX )
	uint64_t end = mach_absolute_time( );
	uint64_t elapsed = end - c->start;
	uint64_t nsec = elapsed * c->timebase_info.numer / timebase_info.denom;
	return nsec / 1000;
#endif
}
#endif

#ifndef VUL_DEFINE
unsigned long long vul_timer_get_micros_cpu( vul_timer_t *c );
#else
unsigned long long vul_timer_get_micros_cpu( vul_timer_t *c )
{
	clock_t new_clock = clock( );
	return ( unsigned long long ) ( ( double )( new_clock - c->zero ) / ( ( double )CLOCKS_PER_SEC / 1000000.0 ) );
}
#endif

#endif
