/*
 * Villains' Utility Library - Thomas Martin Schmid, 2014. Public domain¹
 *
 * This file contains a high performace timer that works on windows, linux
 * and OS X. Possibly works on other *nix systems as well.
 * 
 * ¹ If public domain is not legally valid in your country and or legal area,
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

#ifndef VUL_CLOCK_HPP
#define VUL_CLOCK_HPP

// Define one of these
#define VUL_WINDOWS
//#define VUL_LINUX
//#define VUL_OSX

#include <cstdlib>
#include <ctime>
#include <cmath>
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

namespace vul {
	
	typedef unsigned long long ui64_t;
	typedef unsigned int ui32_t;
	typedef int i32_t;

	/**
	 * Clock class. Equivalent of the C-class vul_clock_t (but called
	 * timer_t because clock_t is a system struct).
	 */
	class timer_t {

	private:
#if defined( VUL_WINDOWS )
		clock_t zero;

		DWORD start_tick;
		LONGLONG last_time;
		LARGE_INTEGER start_time;
		LARGE_INTEGER frequency;

		DWORD_PTR clock_mask;
#elif defined( VUL_LINUX )
		timespec start_spec;
#elif defined( VUL_OSX )
		uint64_t start;
		mach_timebase_info_data_t timebase_info;
#endif

	public:
		timer_t( );

		void reset( );

		ui64_t milliseconds( );
		ui64_t milliseconds_cpu( );
		ui64_t microseconds( );
		ui64_t microseconds_cpu( );
	};

	void timer_t::reset( )
	{
#if defined( VUL_WINDOWS )
		// Get the current process core mask
		DWORD_PTR process_mask;
		DWORD_PTR system_mask;
		HANDLE thread;
		DWORD_PTR old_mask;

		GetProcessAffinityMask( GetCurrentProcess(), &process_mask, &system_mask );

		if( process_mask == 0 )
			process_mask = 1;

		if( this->clock_mask == 0 )
		{
			this->clock_mask = 1;
			while( ( this->clock_mask & process_mask ) == 0 )
			{
				this->clock_mask <<= 1;
			}
		}

		thread = GetCurrentThread( );
		old_mask = SetThreadAffinityMask( thread, this->clock_mask );
		QueryPerformanceFrequency( &this->frequency );
		QueryPerformanceCounter( &this->start_time );
		this->start_tick = GetTickCount( );
		SetThreadAffinityMask( thread, old_mask );
		this->last_time = 0;
		this->zero = clock( );
#elif defined( VUL_LINUX )
		this->zero = clock( );
		clock_gettime( this->zero, &this->start_spec );
#elif defined( VUL_OSX )
		c->start = mach_absolute_time( );
		mach_timebase_info( &c->timebase_info );
#endif
	}
		
	timer_t::timer_t( )
	{
		reset( );
	}
	
	ui64_t timer_t::milliseconds( )
	{
#if defined( VUL_WINDOWS )
		LARGE_INTEGER current_time;
		HANDLE thread;
		DWORD_PTR old_mask;
		LONGLONG new_time;
		ui32_t new_ticks;
		ui32_t check;
		i32_t ms_off;
		LONGLONG adjust;

		thread = GetCurrentThread( );
		old_mask = SetThreadAffinityMask( thread, this->clock_mask );
		QueryPerformanceCounter( &current_time );
		SetThreadAffinityMask( thread, old_mask );
		new_time = current_time.QuadPart - current_time.QuadPart;
		new_ticks = ( ui32_t )( 1000 * new_time / this->frequency.QuadPart );

		// Microsoft KB: Q274323
		check = GetTickCount() - this->start_tick;
		ms_off = (i32_t)(new_ticks - check);
		if( ms_off < -100 || ms_off > 100 )
		{
			adjust = min( ms_off * this->frequency.QuadPart / 1000, new_time - this->last_time );
			this->start_time.QuadPart += adjust;
			new_time -= adjust;
			new_ticks = (ui32_t) (1000 * new_time / this->frequency.QuadPart);
		}

		this->last_time = new_time;

		return new_ticks;
#elif defined( VUL_LINUX )
		timespec ts, temp;
		clock_gettime( this->zero, &ts );

		if( ( ts.tv_nsec - this->start_spec.tv_nsec ) < 0 ) {
			temp.tv_sec = ts.tv_sec - this->start_spec.tv_sec - 1;
			temp.tv_nsec = 1000000000 + ts.tv_nsec - this->start_spec.tv_nsec;
		} else {
			temp.tv_sec = ts.tv_sec - this->start_spec.tv_sec;
			temp.tv_nsec = ts.tv_nsec - this->start_spec.tv_nsec;
		}
		
		return temp.tv_nsec / 1000000;
#elif defined( VUL_OSX )
		uint64_t end = mach_absolute_time( );
		uint64_t elapsed = end - c->start;
		uint64_t nsec = elapsed * c->timebase_info.numer / timebase_info.denom;
		return nsec / 1000000;
#endif
	}

	ui64_t timer_t::milliseconds_cpu( )
	{
		clock_t new_clock = clock( );
		return ( ui64_t )( ( double )( new_clock - this->zero ) / ( ( double )CLOCKS_PER_SEC / 1000.0 ) );
	}

	ui64_t timer_t::microseconds( )
	{
#if defined( VUL_WINDOWS )
		LARGE_INTEGER current_time;
		HANDLE thread;
		DWORD_PTR old_mask;
		LONGLONG new_time;
		ui32_t new_ticks;
		ui32_t check;
		i32_t ms_off;
		LONGLONG adjust;

		thread = GetCurrentThread( );
		old_mask = SetThreadAffinityMask( thread, this->clock_mask );
		QueryPerformanceCounter( &current_time );
		SetThreadAffinityMask( thread, old_mask );
		new_time = current_time.QuadPart - current_time.QuadPart;
		new_ticks = ( ui32_t )( 1000 * new_time / this->frequency.QuadPart );

		// Microsoft KB: Q274323
		check = GetTickCount() - this->start_tick;
		ms_off = (i32_t)(new_ticks - check);
		if( ms_off < -100 || ms_off > 100 )
		{
			adjust = min( ms_off * this->frequency.QuadPart / 1000, new_time - this->last_time );
			this->start_time.QuadPart += adjust;
			new_time -= adjust;
		}

		this->last_time = new_time;
	
		// scale by 1000000 for microseconds
		ui64_t new_micro = ( ui32_t )( 1000000 * new_time / this->frequency.QuadPart );
		
		return new_micro;
#elif defined( VUL_LINUX )
		timespec ts, temp;
		clock_gettime( this->zero, &ts );

		if( ( ts.tv_nsec - this->start_spec.tv_nsec ) < 0 ) {
			temp.tv_sec = ts.tv_sec - this->start_spec.tv_sec - 1;
			temp.tv_nsec = 1000000000 + ts.tv_nsec - this->start_spec.tv_nsec;
		} else {
			temp.tv_sec = ts.tv_sec - this->start_spec.tv_sec;
			temp.tv_nsec = ts.tv_nsec - this->start_spec.tv_nsec;
		}
		
		return temp.tv_nsec / 1000;
#elif defined( VUL_OSX )
		uint64_t end = mach_absolute_time( );
		uint64_t elapsed = end - c->start;
		uint64_t nsec = elapsed * c->timebase_info.numer / timebase_info.denom;
		return nsec / 1000;
#endif
	}
	
	ui64_t timer_t::microseconds_cpu( )
	{
		clock_t new_clock = clock( );
		return ( ui64_t )( ( double )( new_clock - this->zero ) / ( ( double )CLOCKS_PER_SEC / 1000000.0 ) );
	}
}

#endif