/*
 * Villains' Utility Library - Thomas Martin Schmid, 2014. Public domain¹
 *
 * This file contains a high performace timer that works on windows, linux
 * and OS X. Possibly works on other *nix systems as well.
 * 
 * ¹ If public domain is not legally valid in your country and or legal area,
 *   the MIT licence applies (see the LICENCE file)
 */

#ifndef VUL_CLOCK_H
#define VUL_CLOCK_H

// Define in exactly _one_ C/CPP file
//#define VUL_DEFINE

// Define one of these
#define VUL_WINDOWS
//#define VUL_LINUX
//#define VUL_OSX

#include <time.h>
#include <math.h>
#if defined( VUL_WINDOWS )
	#include <windows.h>
#elif defined( VUL_LINUX )

#elif defined( VUL_OSX )

#else
	vul needs an operating system defined.
#endif

struct vul_clock_t {
#if defined( VUL_WINDOWS )
	clock_t zero;

	DWORD start_tick;
	LONGLONG last_time;
	LARGE_INTEGER start_time;
	LARGE_INTEGER frequency;

	DWORD_PTR clock_mask;
#elif defined( VUL_LINUX )
	STUFF
#elif defined( VUL_OSX )
	STUFF
#endif
};

#ifndef VUL_DEFINE
static void vul_clock_reset( vul_clock_t *c );
#else
static void vul_clock_reset( vul_clock_t *c )
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
		STUFF
#elif defined( VUL_OSX )
		STUFF
#endif
}
#endif

#ifndef VUL_DEFINE
static vul_clock_t *vul_makevul_clock_create_clock( );
#else
static vul_clock_t *vul_clock_create( )
{
	vul_clock_t *c;

	c = ( vul_clock_t* )malloc( sizeof( vul_clock_t ) );
	assert( c != NULL ); // Make sure malloc didn't fail
	vul_clock_reset( c );
	
	return c;
}
#endif

#ifndef VUL_DEFINE
static void vul_clock_destroy( vul_clock_t *c );
#else
static void vul_clock_destroy( vul_clock_t *c )
{
	free( c );
}
#endif


#ifndef VUL_DEFINE
static unsigned long long vul_clock_get_millis( vul_clock_t *c );
#else
static unsigned long long vul_clock_get_millis( vul_clock_t *c )
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
		adjust = min( ms_off * c->frequency.QuadPart / 1000, new_time - c->last_time );
		c->start_time.QuadPart += adjust;
		new_time -= adjust;
		new_ticks = (unsigned long) (1000 * new_time / c->frequency.QuadPart);
	}

	c->last_time = new_time;

	return new_ticks;
#elif defined( VUL_LINUX )
		STUFF
#elif defined( VUL_OSX )
		STUFF
#endif
}
#endif

#ifndef VUL_DEFINE
static unsigned long long vul_clock_get_millis_cpu( vul_clock_t *c );
#else
static unsigned long long vul_clock_get_millis_cpu( vul_clock_t *c )
{
	clock_t new_clock = clock( );
	return ( unsigned long long ) ( ( double )( new_clock - c->zero ) / ( ( double )CLOCKS_PER_SEC / 1000.0 ) );
}
#endif


#ifndef VUL_DEFINE
static unsigned long long vul_clock_get_micros( vul_clock_t *c );
#else
static unsigned long long vul_clock_get_micros( vul_clock_t *c )
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
		adjust = min( ms_off * c->frequency.QuadPart / 1000, new_time - c->last_time );
		c->start_time.QuadPart += adjust;
		new_time -= adjust;
	}

	c->last_time = new_time;
	
	// scale by 1000000 for microseconds
	unsigned long long new_micro = (unsigned long) (1000000 * new_time / c->frequency.QuadPart);


	return new_micro;
#elif defined( VUL_LINUX )
		STUFF
#elif defined( VUL_OSX )
		STUFF
#endif
}
#endif

#ifndef VUL_DEFINE
static unsigned long long vul_clock_get_micros_cpu( vul_clock_t *c );
#else
static unsigned long long vul_clock_get_micros_cpu( vul_clock_t *c )
{
	clock_t new_clock = clock( );
	return ( unsigned long long ) ( ( double )( new_clock - c->zero ) / ( ( double )CLOCKS_PER_SEC / 1000000.0 ) );
}
#endif

#endif