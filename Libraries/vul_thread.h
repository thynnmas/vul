/*
 * Villains' Utility Library - Thomas Martin Schmid, 2017. Public domain¹
 *
 * This file contains an OS agnostic thread/mutex wrapper for when you want
 * to avoid the C++ stdlib
 *
 * To use, define VUL_DEFINE in exacly _one_ C/CPP compilation unit before including 
 * this file.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#ifndef VUL_THREAD_H
#define VUL_THREAD_H

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#if defined( VUL_WINDOWS )
	#include <windows.h>
#elif defined( VUL_LINUX ) || defined( VUL_OSX )
   #include <pthread.h>
#else
   #error "vul_thread.h: Unknown OS"
#endif

#ifndef VUL_THREAD_ERROR
#include <assert.h>
#define VUL_THREAD_ERROR( str, ... ) assert( 0 && str );
#endif

#ifndef VUL_TYPES_H
#define s8 int8_t
#define s16 int16_t
#define s32 int32_t
#define s64 int64_t
#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t
#define u64 uint64_t
#define f32 float
#define f64 double
#define b32 u32
#endif

#ifdef VUL_WINDOWS
typedef HANDLE vul_thread;
typedef HANDLE vul_mutex;
typedef LPTHREAD_START_ROUTINE vul_thread_func;
#elif defined( VUL_OSX ) || defined( VUL_LINUX )
typedef pthread_t vul_thread;
typedef pthread_mutex_t vul_mutex;
typedef void *( *vul_thread_func )( void * );
#else
   #error "vul_thread.h: Unknown OS"
#endif
// @TODO(thynn): Condition variables, events

typedef struct vul_thread_attributes {
   size_t stack_size;
   b32 create_suspended,  // @TODO(thynn): Can we do this with pthreads?
       stack_size_reserve_not_commit;
   // @TODO(thynn): Security settings on windows?
} vul_thread_attributes;

#ifdef __cplusplus
extern "C" {
#endif

//-----------------
// Public API
//

vul_thread vul_thread_create( vul_thread_attributes attr,
                              vul_thread_func func,
                              void *arg );

void vul_thread_join( vul_thread t, void **ret );

inline vul_mutex vul_mutex_create( b32 owned_initially, const char *name );
inline void vul_mutex_destroy( vul_mutex *m );
inline void vul_mutex_wait_and_lock( vul_mutex *m );
inline void vul_mutex_release( vul_mutex *m );

#ifdef __cplusplus
}
#endif

#ifndef VUL_TYPES_H
#undef s8
#undef s16
#undef s32
#undef s64
#undef u8t
#undef u16
#undef u32
#undef b32
#undef u64
#undef f32
#undef f64
#endif // VUL_TYPES_H

#endif // VUL_THREAD_H

#ifdef VUL_DEFINE

#ifndef VUL_TYPES_H
#define s8 int8_t
#define s16 int16_t
#define s32 int32_t
#define s64 int64_t
#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t
#define b32 u32
#define u64 uint64_t
#define f32 float
#define f64 double
#endif

#ifdef __cplusplus
extern "C" {
#endif

vul_thread vul_thread_create( vul_thread_attributes attr, 
                              vul_thread_func func,
                              void *arg )
{
   vul_thread t;
#ifdef VUL_WINDOWS
   DWORD ss, flags;
   
   ss = ( DWORD )attr.stack_size;
   flags = 0;
   flags |= attr.stack_size_reserve_not_commit ? STACK_SIZE_PARAM_IS_A_RESERVATION : 0;
   flags |= attr.create_suspended ? CREATE_SUSPENDED : 0;
   t = CreateThread( NULL, ss,
                     func, arg,
                     flags, NULL ); // @TODO(thynn): Should thread ID be an optional return value?
   if( !t ) {
      VUL_THREAD_ERROR( "Failed to create thread: Code %d", GetLastError( ) );
   }
#elif defined( VUL_OSX ) || defined( VUL_LINUX )
   int r;
   pthread_attr_t pattr;

   pthread_attr_init( &pattr );
   if( attr.stack_size ) {
      pthread_attr_setstacksize( &pattr, attr.stack_size );
   }
   r = pthread_create( &t, &pattr, func, arg );
   pthread_attr_destroy( &pattr );
   if( r ) {
      VUL_THREAD_ERROR( "Failed to create thread: Code %d", r );
   }
#else
   #error "vul_thread.h: Unknown OS"
#endif
   return t;
}

void vul_thread_join( vul_thread t, void **ret )
{
#ifdef VUL_WINDOWS
   DWORD r;
   BOOL s;
   
   r = WaitForSingleObject( t, INFINITE );
   if( r == WAIT_FAILED ) {
      VUL_THREAD_ERROR( "Could not join thread. Code %d", GetLastError( ) );
   }
   if( ret ) {
      s = GetExitCodeThread( t, &r );
      if( !s ) {
         VUL_THREAD_ERROR( "Could not retrieve thread exit code. Error code %d", 
                           GetLastError( ) );
      }
      **( ( DWORD** )ret ) = r;
   }
#elif defined( VUL_OSX ) || defined( VUL_LINUX )
   int r;

   r = pthread_join( t, ret );
   if( r ) {
      VUL_THREAD_ERROR( "Could not join thread. Code %d", r );
   }
#else
   #error "vul_thread.h: Unknown OS"
#endif
}

inline vul_mutex vul_mutex_create( b32 owned_initially, const char *name )
{
   vul_mutex m;
#ifdef VUL_WINDOWS
   // @TODO(thynn): Security attributes?
   m = CreateMutex( NULL, owned_initially ? TRUE : FALSE, name );
   if( !m ) {
      if( name ) {
         VUL_THREAD_ERROR( "Failed to create mutex %s, code %d\n", name, GetLastError( ) );
      } else {
         VUL_THREAD_ERROR( "Failed to create mutex, code %d\n", GetLastError( ) );
      }
   }
#elif defined( VUL_OSX ) || defined( VUL_LINUX )
   int r;
   pthread_mutexattr_t attr;

   pthread_mutexattr_init( &attr );
#ifdef VUL_DEBUG
   pthread_mutexattr_settype( &attr, PTHREAD_MUTEX_ERRORCHECK );
#else
   pthread_mutexattr_settype( &attr, PTHREAD_MUTEX_NORMAL );
#endif
   r = pthread_mutex_init( &m, &attr );
   if( r ) {
      if( name ) {
         VUL_THREAD_ERROR( "Failed to create mutex %s, code %d\n", name, GetLastError( ) );
      } else {
         VUL_THREAD_ERROR( "Failed to create mutex, code %d\n", GetLastError( ) );
      }
   }
#else
   #error "vul_thread.h: Unknown OS"
#endif
   return m;
}

inline void vul_mutex_destroy( vul_mutex *m )
{
#ifdef VUL_WINDOWS
   CloseHandle( *m );
#elif defined( VUL_OSX ) || defined( VUL_LINUX )
   pthread_mutex_destroy( m );
#else
   #error "vul_thread.h: Unknown OS"
#endif
}
inline void vul_mutex_wait_and_lock( vul_mutex *m )
{
#ifdef VUL_WINDOWS
   DWORD r;
   
   r = WaitForSingleObject( *m, INFINITE );
   if( r != WAIT_OBJECT_0 ) {
      VUL_THREAD_ERROR( "Failed to lock mutex. Code %d", GetLastError( ) );
   }
#elif defined( VUL_OSX ) || defined( VUL_LINUX )
   int r;

   r = pthread_mutex_lock( m );
   if( r ) {
      VUL_THREAD_ERROR( "Failed to lock mutex. Code %d", r );
   }
#else
   #error "vul_thread.h: Unknown OS"
#endif
}

inline void vul_mutex_release( vul_mutex *m )
{
#ifdef VUL_WINDOWS
   if( !ReleaseMutex( *m ) ) {
      VUL_THREAD_ERROR( "Failed to release mutex. Code %d", GetLastError( ) );
   }
#elif defined( VUL_OSX ) || defined( VUL_LINUX )
   int r;

   r = pthread_mutex_unlock( m );
   if( r ) {
      VUL_THREAD_ERROR( "Failed to release mutex. Code %d", r );
   }
#else
   #error "vul_thread.h: Unknown OS"
#endif
}

#ifdef __cplusplus
}
#endif

#endif // VUL_DEFINE

#ifndef VUL_TYPES_H
#undef s8
#undef s16
#undef s32
#undef s64
#undef u8t
#undef u16
#undef u32
#undef b32
#undef u64
#undef f32
#undef f64
#endif // VUL_TYPES_H

