/*
 * Villains' Utility Library - Thomas Martin Schmid, 2016. Public domain?
 *
 * This file contains a high performace timer that works on windows, linux
 * and OS X. Possibly works on other *nix systems as well.
 * It also contains an OS agnostic sleep function.
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
#ifndef VUL_TIMER_H
#define VUL_TIMER_H

// Define in exactly _one_ C/CPP file
//#define VUL_DEFINE

// Define one of these
//#define VUL_WINDOWS
//#define VUL_LINUX
//#define VUL_OSX

// If you are on pre-vista windows, GetTickCount64 is not available. If so, define this:
//#define VUL_TIMER_OLD_WINDOWS

#define VUL_MIN( a, b ) ( a <= b ? a : b )
#define VUL_MAX( a, b ) ( a >= b ? a : b )

#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <assert.h>
#if defined( VUL_WINDOWS )
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#include <malloc.h>
#elif defined( VUL_LINUX )
	#include <malloc.h>
	#include <time.h>
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
		
typedef struct vul_timer {
#if defined( VUL_WINDOWS )
#ifdef VUL_TIMER_OLD_WINDOWS
	DWORD start_tick;
#else
	ULONGLONG start_tick;
#endif
	LONGLONG last_time;
	LARGE_INTEGER start_time;
	LARGE_INTEGER frequency;

	DWORD_PTR clock_mask;
#elif defined( VUL_LINUX )
	struct timespec start;
#elif defined( VUL_OSX )
	uint64_t start;
	mach_timebase_info_data_t timebase_info;
#endif
} vul_timer;
	
#ifdef _cplusplus
extern "C" {
#endif
/**
 * Reset the timer to zero
 */
void vul_timer_reset( vul_timer *c );

/**
 * Create a new timer on the heap (using malloc).
 * To create one on the stack (or using your own
 * allocation scheme, allocate enough memory for it,
 * then pass that to vul_timer_reset to initialize it.
 */
vul_timer *vul_timer_create( );

/**
 * Destroy a timer created with vul_timer_create (using free).
 */
void vul_timer_destroy( vul_timer *c );

/**
 * Get the number of mulliseconds elapsed since the last reset/creation.
 */
u64 vul_timer_get_millis( vul_timer *c );

/**
 * Get the number of microseconds since the last reset/creation.
 */
u64 vul_timer_get_micros( vul_timer *c );

/*
 * OS agnostic sleep.
 * Takes milliseconds to sleep.
 * Returns milliseconds the amount of time left if interrupted before finishing
 * the alotted sleeping time; only on unix.
 */
u32 vul_sleep( u32 milliseconds );

#ifdef _cplusplus
}
#endif
#endif

#ifdef VUL_DEFINE

#ifdef _cplusplus
extern "C" {
#endif

void vul_timer_reset( vul_timer *c )
{
#if defined( VUL_WINDOWS )
	// Get the current process core mask
	DWORD_PTR process_mask;
	DWORD_PTR system_mask;
	HANDLE thread;
	DWORD_PTR old_mask;

	GetProcessAffinityMask( GetCurrentProcess( ), &process_mask, &system_mask );

	if( process_mask == 0 )
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
#ifdef VUL_TIMER_OLD_WINDOWS
	c->start_tick = GetTickCount( );
#else
	c->start_tick = GetTickCount64( );
#endif
	SetThreadAffinityMask( thread, old_mask );
	c->last_time = 0;
#elif defined( VUL_LINUX )
	clock_gettime( CLOCK_REALTIME, &c->start );
#elif defined( VUL_OSX )
	c->start = mach_absolute_time( );
	mach_timebase_info( &c->timebase_info );
#endif
}

vul_timer *vul_timer_create( )
{
	vul_timer *c;

	c = ( vul_timer* )malloc( sizeof( vul_timer ) );
	assert( c != NULL ); // Make sure malloc didn't fail
	vul_timer_reset( c );

	return c;
}

void vul_timer_destroy( vul_timer *c )
{
	free( c );
}

u64 vul_timer_get_millis( vul_timer *c )
{
#if defined( VUL_WINDOWS )
	LARGE_INTEGER current_time;
	HANDLE thread;
	DWORD_PTR old_mask;
	LONGLONG new_time;
	u32 new_ticks;
#ifdef VUL_TIMER_OLD_WINDOWS
	DWORD check;
#else
	ULONGLONG check;
#endif
	s32 ms_off;
	LONGLONG adjust;

	thread = GetCurrentThread( );
	old_mask = SetThreadAffinityMask( thread, c->clock_mask );
	QueryPerformanceCounter( &current_time );
	SetThreadAffinityMask( thread, old_mask );
	new_time = current_time.QuadPart - c->start_time.QuadPart;
	new_ticks = ( u32 )( 1000 * new_time / c->frequency.QuadPart );

	// Microsoft KB: Q274323
#ifdef VUL_TIMER_OLD_WINDOWS
	check = GetTickCount( ) - c->start_tick;
#else
	check = GetTickCount64( ) - c->start_tick;
#endif
	ms_off = ( s32 )( new_ticks - check );
	if( ms_off < -100 || ms_off > 100 )
	{
		adjust = VUL_MIN( ms_off * c->frequency.QuadPart / 1000, new_time - c->last_time );
		c->start_time.QuadPart += adjust;
		new_time -= adjust;
		new_ticks = ( u32 )( 1000 * new_time / c->frequency.QuadPart );
	}

	c->last_time = new_time;

	return ( u64 )new_ticks;
#elif defined( VUL_LINUX )
	struct timespec now;
	clock_gettime( CLOCK_REALTIME, &now );
	return ( ( now.tv_sec - c->start.tv_sec ) * 1000 )
		+ ( ( now.tv_nsec - c->start.tv_nsec ) / 1000000 );
#elif defined( VUL_OSX )
	u64 end = mach_absolute_time( );
	u64 elapsed = end - c->start;
	u64 nsec = elapsed * c->timebase_info.numer / c->timebase_info.denom;
	return ( u64 )( nsec / 1000000 );
#endif
}

u64 vul_timer_get_micros( vul_timer *c )
{
#if defined( VUL_WINDOWS )
	LARGE_INTEGER current_time;
	HANDLE thread;
	DWORD_PTR old_mask;
	LONGLONG new_time;
	u32 new_ticks;
#ifdef VUL_TIMER_OLD_WINDOWS
	DWORD check;
#else
	ULONGLONG check;
#endif
	s32 ms_off;
	LONGLONG adjust;
	u64 new_micro;

	thread = GetCurrentThread( );
	old_mask = SetThreadAffinityMask( thread, c->clock_mask );
	QueryPerformanceCounter( &current_time );
	SetThreadAffinityMask( thread, old_mask );
	new_time = current_time.QuadPart - c->start_time.QuadPart;
	new_ticks = ( u32 )( 1000 * new_time / c->frequency.QuadPart );

	// Microsoft KB: Q274323
#ifdef VUL_TIMER_OLD_WINDOWS
	check = GetTickCount( ) - c->start_tick;
#else
	check = GetTickCount64( ) - c->start_tick;
#endif
	ms_off = ( s32 )( new_ticks - check );
	if( ms_off < -100 || ms_off > 100 )
	{
		adjust = VUL_MIN( ms_off * c->frequency.QuadPart / 1000, new_time - c->last_time );
		c->start_time.QuadPart += adjust;
		new_time -= adjust;
	}

	c->last_time = new_time;

	// scale by 1000000 for microseconds
	new_micro = ( u32 )( 1000000 * new_time / c->frequency.QuadPart );
	
	return new_micro;
#elif defined( VUL_LINUX )	
	struct timespec now;
	clock_gettime( CLOCK_REALTIME, &now );
	return ( ( now.tv_sec - c->start.tv_sec ) * 1000000 )
		   + ( now.tv_nsec - c->start.tv_nsec ) / 1000;
#elif defined( VUL_OSX )
	u64 end = mach_absolute_time( );
	u64 elapsed = end - c->start;
	u64 nsec = elapsed * c->timebase_info.numer / c->timebase_info.denom;
	return nsec / 1000;
#endif
}

unsigned int vul_sleep( unsigned int milliseconds )
{
#ifdef VUL_WINDOWS
	DWORD ms;

	ms = milliseconds;
	Sleep( ms );
	return 0;
#elif defined( VUL_LINUX ) || defined( VUL_OSX )
	struct timespec rem, req;
	s32 err;

	req.tv_sec = ( time_t )milliseconds / 1000;
	req.tv_nsec = ( u64 )( milliseconds % 1000l ) * 1000000l;
#ifdef VUL_OSX
	err = nanosleep( &req, &rem );
#else
	err = clock_nanosleep( CLOCK_REALTIME, 0, &req, &rem );
#endif
	if( err ) {
		return ( s32 )( rem.tv_sec * 1000 ) + ( s32 )( rem.tv_nsec / 1000000l );
	}
	return 0;
#else
	assert( 0 && "vul_timer.h: OS not supported. Did you forget to specify an OS define?" );
#endif
}

#ifdef _cplusplus
}
#endif

#endif
