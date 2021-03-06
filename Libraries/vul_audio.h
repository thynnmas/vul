/*
 * Villains' Utility Library - Thomas Martin Schmid, 2017. Public domain¹
 *
 * This file contains a low-level wrapper for a few different audio APIs.
 * Supported APIs by system (in order of attempts/fallback):
 *  - Linux: pulseaudio -> alsa -> oss @TODO(thynn): Test OSS support
 *  - OSX: CoreAudio
 *  - Windows: waveOut @TODO(thynn): Windows Core Audio
 * @TODO(thynn): These two would probably be useful
 *  - Emscripten
 *  - Mobile: iOS & Android
 *
 * @TODO(thynn): Hide a statically linked version behind a define? Would this be useful?
 * @TODO(thynn): Make the mixer wide (SSE, AVX and NEON at least).
 *
 * Linux and OSX variation require linking with pthreads (because dlopen-ing
 * libpthread.so simply does not work!).
 *
 * Error reporting: A lot can go wrong, especially in setup. We have multiple
 * ways to report errors back to the user (in addition to return value being
 * VUL_ERROR). Define any of these to use it (no default):
 *  - VUL_AUDIO_ERROR_STDERR: Print an error message to stderr.
 *  - VUL_AUDIO_ERROR_STR: Last error message is written to the global vul_last_error.
 *  - VUL_AUDIO_ERROR_ASSERT: Trigger an assert on error.
 *  - VUL_AUDIO_ERROR_QUIET: Do no reporting at all, simple fail.
 *  - VUL_AUDIO_ERROR_CUSTOM: Define your own macro VUL_AUDIO_ERROR_CUSTOM( ... )
 *    and do whatever you want with the error message (printf style formatting/arguments)
 *    before failing.
 * 
 * ¹ If public domain is not legally valid in your legal jurisdiction
 *   the MIT licence applies (see the LICENCE file)
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
#ifndef VUL_AUDIO_H
#define VUL_AUDIO_H

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#if defined( VUL_WINDOWS )
	#if !defined( __MINGW32__ ) && !defined( __MINGW64__ )
		#pragma comment(lib, "winmm.lib")
	#endif
	#include <windows.h>
	#include <mmsystem.h>
#elif defined( VUL_LINUX )
	#include <dlfcn.h>
	//#include <poll.h>
	#include <sys/ioctl.h>
	#include <sys/soundcard.h>
	#include <alsa/asoundlib.h>
	#include <pulse/simple.h>
	#include <pulse/error.h>
   #include <pthread.h>
#elif defined( VUL_OSX )
   #include <pthread.h>
   #include <AudioToolbox/AudioToolbox.h>
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
#endif

#ifdef VUL_AUDIO_SAMPLE_16BIT
#define smp s16
#define smx s32
#elif VUL_AUDIO_SAMPLE_32BIT
#define smp s32
#define smx s64
#else
#error "vul_audio.h: Must define sample size at compile time!"
#endif

typedef struct vul__audio_mixer_clip {
	u64 id;
	smp *samples;
	u64 sample_count;
	u64 current_offset;
	u32 channels;

	b32 playing, looping, keep_after_finish;
	f32 volume;
} vul__audio_mixer_clip;

typedef struct vul__audio_mixer {
	vul__audio_mixer_clip *clips;
	u64 count, size, next_id;
	
	f32 volume;
	u32 channels;

	smx *mixbuf;
	smp *samples;
	u32 mixbuf_sample_count;
} vul__audio_mixer;

typedef enum vul__audio_lib {
#ifdef VUL_WINDOWS
	VUL__AUDIO_WINDOWS_DSOUND,
	VUL__AUDIO_WINDOWS_WAVEOUT,
#elif defined( VUL_OSX )
   VUL__AUDIO_OSX_CORE_AUDIO,
#elif defined( VUL_LINUX )
	VUL__AUDIO_LINUX_ALSA,
	VUL__AUDIO_LINUX_PULSE,
	VUL__AUDIO_LINUX_OSS,
#else
	Must define an OS
#endif

	VUL__AUDIO_Count
} vul__audio_lib;

typedef enum vul_audio_mode {
	VUL_AUDIO_MODE_PLAYBACK,
	VUL_AUDIO_MODE_RECORDING,
	VUL_AUDIO_MODE_DUPLEX
} vul_audio_mode;

typedef enum vul_audio_return {
	VUL_OK = 0,
	VUL_ERROR = 1
} vul_audio_return;

typedef struct vul_audio_device {
	u32 channels;
	u32 sample_rate;
	vul_audio_mode mode;
   
   void (*mix_function)( void*, size_t, void* );
   void *mix_function_data;

   vul__audio_mixer mixer;
	vul__audio_lib lib;
   b32 thread_dead;
#ifdef VUL_WINDOWS
   HANDLE thread, mixer_mutex, close_event;
	union {
      struct {
         HWAVEOUT handle;
         WAVEHDR headers[ 2 ];
         smp *buffers[ 2 ];
         HANDLE event;
      } waveout;
	} device;
#elif defined( VUL_OSX )
   pthread_mutex_t mixer_mutex;
   AudioQueueRef queue;
#elif defined( VUL_LINUX )
   pthread_t thread;
   pthread_mutex_t mixer_mutex, thread_mutex; 
	union {
		s32 oss_device_fd;
		struct alsa {
			snd_pcm_t *handle;
			void *dlib;
		} alsa;
		struct pulse {
			pa_simple *client;
			void *dlib;
			void *dlib_simple;
		} pulse;
	} device;
#else
	You must define an operating system (VUL_WINDOWS, VUL_LINUX, VUL_OSX)
#endif
} vul_audio_device;

#ifdef __cplusplus
extern "C" {
#endif

//-----------------
// Public API
//

/*
 * Stops the audio system (disconnects from the audio server), stops
 * the worker thread and frees all allocated memory.
 * ALSA and Pulse make use of the drain_before_close argument, which
 * drain its internal audio buffers before shutting down (finish
 * playing any uploaded audio) if the argument is non-zero.
 */
vul_audio_return vul_audio_destroy( vul_audio_device *dev, int drain_before_close );

/*
 * Initialize the audio subsystem and start the thread that writes to it.
 *
 * If the mix_function argument is supplied, the function is called when
 * the audio system requires new data (it is actually called immediately 
 * after a write, and waits until needed after the call to write to the buffer).
 * It is supplied a buffer to fill with data (channels are interleaved),
 * the size of said buffer and mix_function_user_data.
 *
 * If no mix_function is supplied, the internal mixer is used.
 *
 * The frame-size argument is the size of the audio buffer upload frame
 * in bytes. If unsure what to use, 0x4000 is recommended (0x1000 is the
 * typical default on linux, but may cause stutter on windows).
 *
 * The linux version takes three additional arguments: 
 * -server_name  is the name to the pulse audio server to attempt to connect 
 *  to (or NULL for default), and is ignored by ALSA and OSS. 
 * -device_name  is the name of the device to output to. For pulse, the default
 *  device is used if device_name is NULL; for ALSA the default device is
 *  used when the argument is set to "default".
 * -identifier   is used only by pulse, and is the description of the application
 *  visible to the user in the pulse audio server. The application will
 *  show as "vul_audio" "[description]".
 *
 * Remember to call vul_audio_destroy to stop the audio system, free
 * its memory and stop the started thread!
 */
#if defined( VUL_WINDOWS ) || defined( VUL_OSX )
vul_audio_return vul_audio_init( vul_audio_device *out, 
											vul_audio_mode mode,
											u32 channels, 
											u32 sample_rate,
                                 u32 frame_size,
											void (*mix_function)( void *, size_t, void* ), 
                                 void *mix_function_user_data );
#elif VUL_LINUX
vul_audio_return vul_audio_init( vul_audio_device *out, 
											const char *server_name,  
											const char *device_name,  
                                 const char *description,
											vul_audio_mode mode,
											u32 channels, 
											u32 sample_rate,
                                 u32 frame_size,
											void (*mix_function)( void *, size_t, void* ),
                                 void *mix_function_user_data );
#else
	You must define an operating system (VUL_WINDOWS, VUL_LINUX, VUL_OSX)
#endif

//----------------------
// Public Mixer API
//

/*
 * Adds a clip to the mixer. The clip may have a different channel count than the
 * audio system, but if the amount is larger than the system supports (N_mixer), only the first
 * N_mixer channels are used. The sample rate must match that of the audio system.
 * The volume given is in range [0,1].
 * After calling this function, the clip is NOT playing, NOT set to loop, and SET to
 * be deleted after completing once.
 *
 * A identifier to the clip is returned. If the adding fails, 0 is returned.
 */
u64 vul_audio_clip_add( vul_audio_device *dev, smp *data, u64 sample_count, u32 channels, f32 volume );

/*
 * Pauses the clip with the given identifier. If reset is set, the current sample
 * of the clip is set to the first sample (in effect restarting the clip).
 */
vul_audio_return vul_audio_clip_pause( vul_audio_device *dev, u64 id, b32 reset );

/* 
 * Starts playback of the clip with the given identifier. If looping is set,
 * the clip is restarted when its end is reached. If keep is set, the clip
 * remains in the mixer's clip-list upon completion, otherwise it is deleted
 * (this is _default behavior_).
 */
vul_audio_return vul_audio_clip_play( vul_audio_device *dev, u64 id, b32 looping, b32 keep );

/*
 * Resumes playback of the clip with the given identifier. This only
 * alters the playing state, it does not alter the current position
 * in the clip. vul_audio_clip_play should be used to start the clip
 * instead of this, although is default behavior is wanted (not looping
 * and not keeping the clip upon completion), this does the same.
 */
vul_audio_return vul_audio_clip_resume( vul_audio_device *dev, u64 id );

/*
 * Removes the clip with the given identifier from the mixer.
 */
vul_audio_return vul_audio_clip_remove( vul_audio_device *dev, u64 id );

/*
 * Sets the volume of the clip with the given identifier. Volume range is
 * [0,1], values outside are clamped to this range.
 */
vul_audio_return vul_audio_clip_volume( vul_audio_device *dev, u64 id, f32 vol );

/*
 * Sets the global mixer volume. All clips' volume are multiplied with this
 * volume. Range is [0,1], values outside are clamped to this range.
 */
vul_audio_return vul_audio_set_global_volume( vul_audio_device *dev, f32 volume );


#ifdef __cplusplus
}
#endif

#undef smp
#undef smx

#ifndef VUL_TYPES_H
#undef s8
#undef s16
#undef s32
#undef s64
#undef u8t
#undef u16
#undef u32
#undef u64
#undef f32
#undef f64
#endif // VUL_TYPES_H

#endif // VUL_AUDIO_H

#ifdef VUL_DEFINE

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
#endif

#ifdef VUL_AUDIO_SAMPLE_16BIT
#define smp s16
#define smx s32
#define clamp_max SHRT_MAX
#define clamp_min SHRT_MIN
#elif VUL_AUDIO_SAMPLE_32BIT
#define smp s32
#define smx s64
#define clamp_max INT_MAX
#define clamp_min INT_MIN
#else
error in vul_audio.h: Must define sample size at compile time!
#endif

#ifdef __cplusplus
extern "C" {
#endif

// Mutex helpers
vul_audio_return vul__audio_mixer_wait_and_lock( vul_audio_device *dev );
vul_audio_return vul__audio_mixer_release( vul_audio_device *dev );

#ifdef VUL_AUDIO_ERROR_STDERR
#define VUL__AUDIO_ERR( ... )\
	fprintf( stderr, __VA_ARGS__ );\
	return VUL_ERROR;
#define VUL__AUDIO_ERR_NORETURN( ... )\
	fprintf( stderr, __VA_ARGS__ );
#elif defined( VUL_AUDIO_ERROR_STR )
char vul_last_error[ 1024 ];
#define VUL__AUDIO_ERR( ... )\
	snprintf( vul_last_error, 1023, __VA_ARGS__ );\
	vul_last_error[ 1023 ] = 0;\
	return VUL_ERROR;
#define VUL__AUDIO_ERR_NORETURN( ... )\
	snprintf( vul_last_error, 1023, __VA_ARGS__ );\
	vul_last_error[ 1023 ] = 0;
#elif defined( VUL_AUDIO_ERROR_ASSERT )
#define VUL__AUDIO_ERR( ... )\
	assert( 0 );\
	return VUL_ERROR;
#define VUL__AUDIO_ERR_NORETURN( ... )\
	assert( 0 );
#elif defined( VUL_AUDIO_ERROR_QUIET )
#define VUL__AUDIO_ERR( ... )\
	return VUL_ERROR;
#define VUL__AUDIO_ERR_NORETURN( ... )\
   {(void)0;}
#elif defined( VUL_AUDIO_ERROR_CUSTOM )
#define VUL__AUDIO_ERR( ... )\
   VUL_AUDIO_ERROR_CUSTOM( __VA_ARGS__ )\
   return VUL_ERROR;
#define VUL__AUDIO_ERR_NORETURN( ... )\
   VUL_AUDIO_ERROR_CUSTOM( __VA_ARGS__ )
#endif

vul_audio_return vul__audio_write( vul_audio_device *dev, void *samples, u32 sample_count );

void vul__audio_mixer_init( vul__audio_mixer *mixer, u32 channels, u32 buffer_sample_count, u32 clip_count_initial )
{
	memset( mixer, 0, sizeof( vul__audio_mixer ) );
	mixer->clips = ( vul__audio_mixer_clip* )malloc( sizeof( vul__audio_mixer_clip ) * clip_count_initial );
	mixer->size = clip_count_initial;
	mixer->count = 0;
   mixer->next_id = 1;
	mixer->channels = channels;
	mixer->mixbuf_sample_count = buffer_sample_count;
	mixer->mixbuf  = ( smx* )malloc( sizeof( smx ) * mixer->mixbuf_sample_count * mixer->channels * 2 );
	mixer->samples = ( smp* )malloc( sizeof( smp ) * mixer->mixbuf_sample_count * mixer->channels );
}

void vul__audio_mixer_destroy( vul__audio_mixer *mixer )
{
	if( mixer ) {
		free( mixer->mixbuf );
		if( mixer->samples ) {
			free( mixer->samples );
			mixer->samples = 0;
		}
		if( mixer->mixbuf ) {
			free( mixer->mixbuf );
			mixer->mixbuf = 0;
		}
		if( mixer->clips ) {
			free( mixer->clips );
			mixer->clips = 0;
		}
		free( mixer );
		mixer = 0;
	}
}

void vul__audio_clip_remove_internal( vul__audio_mixer *mixer, u64 idx )
{
   if( idx >= 1 ) {
      for( u64 i = idx + 1; i < mixer->count; ++i ) {
         memcpy( &mixer->clips[ i - 1 ], &mixer->clips[ i ], sizeof( vul__audio_mixer_clip ) );
      }
   }
      
   mixer->count -= 1;
}

u64 vul_audio_clip_add( vul_audio_device *dev, smp *data, u64 sample_count, u32 channels, f32 volume )
{
	if( !dev ) {
      VUL__AUDIO_ERR_NORETURN( "No audio device supplied.\n" );
		return 0;
	}

   if( VUL_ERROR == vul__audio_mixer_wait_and_lock( dev ) ) {
      VUL__AUDIO_ERR_NORETURN( "Failed to lock audio mixer.\n" );
		return 0;
   }

	if( dev->mixer.count == dev->mixer.size - 1 ) {
		vul__audio_mixer_clip* ptr;
		ptr = ( vul__audio_mixer_clip* )realloc( dev->mixer.clips, 
                                               sizeof( vul__audio_mixer_clip ) * dev->mixer.size * 2 );
		if( ptr == 0 ) {
			VUL__AUDIO_ERR_NORETURN( "Failed to reallocate mixer clip collection." );
			return 0;
		}
		dev->mixer.clips = ptr;
		dev->mixer.size *= 2;
	}

	vul__audio_mixer_clip *clip = &dev->mixer.clips[ dev->mixer.count ];
	clip->id = dev->mixer.next_id++;
	clip->samples = data;
	clip->sample_count = sample_count;
	clip->channels = channels;
	clip->current_offset = 0;
	clip->playing = 0;
	clip->looping = 0;
   clip->keep_after_finish = 0;
	if( volume < 0.f || volume > 1.f ) {
		VUL__AUDIO_ERR_NORETURN( "Volume should be in range [0,1]. Value was clamped." );
	}
	clip->volume = volume > 1.f ? 1.f : volume < 0.f ? 0.f : volume;
   dev->mixer.count++;

   vul__audio_mixer_release( dev );

	return clip->id;
}

vul_audio_return vul_audio_clip_pause( vul_audio_device *dev, u64 id, b32 reset )
{
   u64 idx;

   if( !dev ) {
      VUL__AUDIO_ERR( "No audio device supplied.\n" );
   }
   if( VUL_ERROR == vul__audio_mixer_wait_and_lock( dev ) ) {
      VUL__AUDIO_ERR( "Failed to lock audio mixer.\n" );
   }

   idx = dev->mixer.count;
   for( u64 i = 0; i < dev->mixer.count; ++i ) {
      if( dev->mixer.clips[ i ].id == id ) {
         idx = i;
         break;
      }
   }

	if( idx >= dev->mixer.count ) {
      VUL__AUDIO_ERR( "Clip not found, can't pause it." );
	}

	dev->mixer.clips[ idx ].playing = 0;
   if( reset ) {
      dev->mixer.clips[ idx ].current_offset = 0;
   }

   vul__audio_mixer_release( dev );

   return VUL_OK;
}

vul_audio_return vul_audio_clip_play( vul_audio_device *dev, u64 id, b32 looping, b32 keep )
{
   u64 idx;

   if( !dev ) {
      VUL__AUDIO_ERR( "No audio device supplied.\n" );
   }
   if( VUL_ERROR == vul__audio_mixer_wait_and_lock( dev ) ) {
      VUL__AUDIO_ERR( "Failed to lock audio mixer.\n" );
   }

   idx = dev->mixer.count;
   for( u64 i = 0; i < dev->mixer.count; ++i ) {
      if( dev->mixer.clips[ i ].id == id ) {
         idx = i;
         break;
      }
   }

	if( idx >= dev->mixer.count ) {
      VUL__AUDIO_ERR( "Clip not found, can't play it." );
	}

	dev->mixer.clips[ idx ].playing = 1;
	dev->mixer.clips[ idx ].looping = looping;
   dev->mixer.clips[ idx ].keep_after_finish = keep;

   vul__audio_mixer_release( dev );

   return VUL_OK;
}

vul_audio_return vul_audio_clip_resume( vul_audio_device *dev, u64 id )
{
   u64 idx;

   if( !dev ) {
      VUL__AUDIO_ERR( "No audio device supplied.\n" );
   }
   if( VUL_ERROR == vul__audio_mixer_wait_and_lock( dev ) ) {
      VUL__AUDIO_ERR( "Failed to lock audio mixer.\n" );
   }

   idx = dev->mixer.count;
   for( u64 i = 0; i < dev->mixer.count; ++i ) {
      if( dev->mixer.clips[ i ].id == id ) {
         idx = i;
         break;
      }
   }

	if( idx >= dev->mixer.count ) {
      VUL__AUDIO_ERR( "Clip not found, can't resume it." );
	}

	dev->mixer.clips[ idx ].playing = 1;

   vul__audio_mixer_release( dev );

   return VUL_OK;
}

vul_audio_return vul_audio_clip_remove( vul_audio_device *dev, u64 id )
{
   u64 idx;
   if( !dev ) {
      VUL__AUDIO_ERR( "No audio device supplied.\n" );
   }
   if( VUL_ERROR == vul__audio_mixer_wait_and_lock( dev ) ) {
      VUL__AUDIO_ERR( "Failed to lock audio mixer.\n" );
   }

   idx = dev->mixer.count;
   for( u64 i = 0; i < dev->mixer.count; ++i ) {
      if( dev->mixer.clips[ i ].id == id ) {
         idx = i;
         break;
      }
   }

	if( idx >= dev->mixer.count ) {
      VUL__AUDIO_ERR( "Clip not found, can't remove it." );
	}

   vul__audio_clip_remove_internal( &dev->mixer, idx );

   vul__audio_mixer_release( dev );

   return VUL_OK;
}

vul_audio_return vul_audio_clip_volume( vul_audio_device *dev, u64 id, f32 vol )
{
   u64 idx;

   if( !dev ) {
      VUL__AUDIO_ERR( "No audio device supplied.\n" );
   }
   if( VUL_ERROR == vul__audio_mixer_wait_and_lock( dev ) ) {
      VUL__AUDIO_ERR( "Failed to lock audio mixer.\n" );
   }

   idx = dev->mixer.count;
   for( u64 i = 0; i < dev->mixer.count; ++i ) {
      if( dev->mixer.clips[ i ].id == id ) {
         idx = i;
         break;
      }
   }

	if( idx >= dev->mixer.count ) {
      VUL__AUDIO_ERR( "Clip not found, can't remove it." );
	}

	if( vol > 1.f || vol < 0.f ) {
		vol = vol < 0.f ? 0.f : 1.f;
	}

	dev->mixer.clips[ idx ].volume = vol;

   vul__audio_mixer_release( dev );

   return VUL_OK;
}

vul_audio_return vul_audio_set_global_volume( vul_audio_device *dev, f32 volume )
{
   if( !dev ) {
      VUL__AUDIO_ERR( "No audio device supplied.\n" );
   }
   if( VUL_ERROR == vul__audio_mixer_wait_and_lock( dev ) ) {
      VUL__AUDIO_ERR( "Failed to lock audio mixer.\n" );
   }

	if( volume < 0.f || volume > 1.f ) {
		volume = volume < 0.f ? 0.f : 1.f;
	}
	dev->mixer.volume = volume;

   vul__audio_mixer_release( dev );

   return VUL_OK;
}

// @TODO(thynn): SIMD mixing. We should define types and functions ala 
// sse_add = _mm_add_epi32 / _mm_add_epi16 / vadd_s32 / vadd_s16
// For this we need to enforce clips->channels == mixer->channels
// anyway so we can process bytes in order without repacking, and then we need to malloc mixbuf aligned
// and handle non-aligned first and last bytes separately. For now, we do it scalar
void vul__audio_mix( vul__audio_mixer *mixer )
{
	u32 sample_count, min_channels, ofs;

	// Mix
	memset( mixer->mixbuf, 0, mixer->mixbuf_sample_count * mixer->channels * sizeof( smx ) );
	for( u64 i = 0; i < mixer->count; ++i ) {
		if( !mixer->clips[ i ].playing ) {
			continue;
		}
		min_channels = mixer->clips[ i ].channels < mixer->channels 
						 ? mixer->clips[ i ].channels : mixer->channels;
		sample_count = mixer->clips[ i ].sample_count - ( mixer->clips[ i ].current_offset / mixer->clips[ i ].channels );
		sample_count = sample_count > mixer->mixbuf_sample_count ? mixer->mixbuf_sample_count : sample_count;
		for( u64 j = 0; j < sample_count; ++j ) {
			for( u32 k = 0; k < min_channels; ++k ) {
				ofs = mixer->clips[ i ].current_offset + j * mixer->clips[ i ].channels + k;
				mixer->mixbuf[ j * mixer->channels + k ] += ( smx )( ( f64 )mixer->clips[ i ].samples[ ofs ] * 
																					  ( f64 )mixer->clips[ i ].volume );
			}
		}
		mixer->clips[ i ].current_offset += sample_count * mixer->clips[ i ].channels;
		// Handle looping
		if( ( mixer->clips[ i ].current_offset / mixer->clips[ i ].channels ) == mixer->clips[ i ].sample_count && mixer->clips[ i ].looping ) {
			s64 remaining = ( s64 )mixer->mixbuf_sample_count - ( s64 )sample_count;
			for( u64 j = sample_count * mixer->channels; ( s64 )j < remaining * mixer->channels; ++j ) {
				for( u32 k = 0; k < min_channels; ++k ) {
					ofs = j * mixer->clips[ i ].channels + k;
					mixer->mixbuf[ j * mixer->channels + k ] += ( smx )( ( f64 )mixer->clips[ i ].samples[ ofs ] *
																						  ( f64 )mixer->clips[ i ].volume );
				}
			}
			mixer->clips[ i ].current_offset = remaining * mixer->channels;
		}
	}

	// Clamp into the upload-buffer
	memset( mixer->samples, 0, mixer->mixbuf_sample_count * mixer->channels * sizeof( smp ) );
	for( u64 i = 0; i < mixer->mixbuf_sample_count * mixer->channels; ++i ) {
		smx expanded = mixer->mixbuf[ i ];
		if( expanded > clamp_max ) {
			expanded = clamp_max;
		} else if( expanded < clamp_min ) {
			expanded = clamp_min;
		}
		mixer->samples[ i ] = ( smp )expanded;
	}

	// Remove non-looping clips that are done. Back to front to minimize copying, but still inefficient.
	for( s64 i = ( s64 )mixer->count - 1; i >= 0; --i ) {
		if( ( mixer->clips[ i ].current_offset / mixer->clips[ i ].channels ) == mixer->clips[ i ].sample_count ) {
			if( !mixer->clips[ i ].looping ) {
            if( mixer->clips[ i ].keep_after_finish ) {
               mixer->clips[ i ].playing = 0;
               mixer->clips[ i ].current_offset = 0;
            } else {
               vul__audio_clip_remove_internal( mixer, i );
            }
			}
		}
	}
}

#if defined( VUL_WINDOWS ) || defined( VUL_LINUX )

vul_audio_return vul__audio_callback_internal( vul_audio_device *dev )
{
   if( dev->mix_function ) {
      dev->mix_function( ( void* )dev->mixer.samples, 
                         ( size_t )( dev->mixer.mixbuf_sample_count * dev->mixer.channels ),
                         dev->mix_function_data );
   } else {
      if( VUL_ERROR == vul__audio_mixer_wait_and_lock( dev ) ) {
         VUL__AUDIO_ERR( "Failed to lock audio mixer.\n" );
      }

      vul__audio_mix( &dev->mixer );

      vul__audio_mixer_release( dev );
   }

   // Upload the data
   vul__audio_write( dev, dev->mixer.samples, dev->mixer.mixbuf_sample_count );

   return VUL_OK;
}

#endif

#ifdef VUL_WINDOWS

#define DLLOAD( fsym, sym, lib, name )\
	sym = ( fsym )GetProcAddress( lib, name );\
	if( sym == NULL ) {\
		VUL__AUDIO_ERR( "Failed to load symbol %s from DLL.\n", name );\
	}


vul_audio_return vul__audio_mixer_wait_and_lock( vul_audio_device *dev )
{
   DWORD res = WaitForSingleObject( dev->mixer_mutex, INFINITE );
   switch( res ) {
   case WAIT_OBJECT_0: {
      return VUL_OK;
   } break;
   case WAIT_ABANDONED: {
      return VUL_ERROR;
   } break;
   }
   return VUL_ERROR;
}

vul_audio_return vul__audio_mixer_release( vul_audio_device *dev )
{
   return ReleaseMutex( dev->mixer_mutex ) ? VUL_OK : VUL_ERROR;
}

DWORD vul__audio_callback( LPVOID data )
{
   vul_audio_return ret;
   vul_audio_device *dev = ( vul_audio_device* )data;

   while( WAIT_OBJECT_0 != WaitForSingleObject( dev->close_event, 0 ) ) {
      
      ret = vul__audio_callback_internal( dev );

      if( ret != VUL_OK ) {
         return 1;
      }
   }
   return 0;
}

//----------------
// waveOut
//

typedef MMRESULT ( *fnWaveOutOpen )( LPHWAVEOUT, UINT_PTR, LPWAVEFORMATEX, DWORD_PTR, DWORD_PTR, DWORD );
typedef MMRESULT ( *fnWaveOutPrepareHeader )( HWAVEOUT, LPWAVEHDR, UINT );
typedef MMRESULT ( *fnWaveOutWrite )( HWAVEOUT, LPWAVEHDR, UINT );
typedef MMRESULT ( *fnWaveOutUnprepareHeader )( HWAVEOUT, LPWAVEHDR, UINT );
typedef MMRESULT ( *fnWaveOutClose )( HWAVEOUT );

fnWaveOutOpen pWaveOutOpen = 0;
fnWaveOutPrepareHeader pWaveOutPrepareHeader = 0;
fnWaveOutWrite pWaveOutWrite = 0;
fnWaveOutUnprepareHeader pWaveOutUnprepareHeader = 0;
fnWaveOutClose pWaveOutClose = 0;

static vul_audio_return vul__audio_write_waveout( vul_audio_device *dev, void *samples, u32 sample_count )
{
   b32 uploaded = 0;
   static b32 skip = 0; // @NOTE(thynn): This is a dirty hack to handle the case
                        // where we want to upload to both buffers before waiting

   for( s32 i = 0; i < 2; ++i ) {
      if( skip ) {
         skip = 0;
         continue;
      }

      if( 0 != ( dev->device.waveout.headers[ i ].dwFlags & WHDR_INQUEUE ) ) {
         continue;
      }

      if( i == 1 && uploaded ) {
         skip = 1;
         return VUL_OK;
      }

      memcpy( dev->device.waveout.buffers[ i ], 
              samples, 
              sample_count * dev->channels * sizeof( smp ) );

      if( pWaveOutWrite( dev->device.waveout.handle,
                         &dev->device.waveout.headers[ i ], 
                         sizeof( WAVEHDR ) ) != MMSYSERR_NOERROR ) {
         VUL__AUDIO_ERR( "Failed to write audio data.\n" );
      }
      uploaded = 1;
   }
   WaitForSingleObject( dev->device.waveout.event, INFINITE );
   
   return VUL_OK;
}

static vul_audio_return vul__audio_destroy_waveout( vul_audio_device *dev, int drain_before_close )
{
   SetEvent( dev->device.waveout.event );
   CloseHandle( dev->device.waveout.event );

   for( int i = 0; i < 2; ++i ) {
      pWaveOutUnprepareHeader( dev->device.waveout.handle,
                               &dev->device.waveout.headers[ i ],
                               sizeof( WAVEHDR ) );
      if( dev->device.waveout.buffers[ i ] ) {
         free( dev->device.waveout.buffers[ i ] );
         dev->device.waveout.buffers[ i ] = 0;
      }
   }
	if( pWaveOutClose( dev->device.waveout.handle ) != MMSYSERR_NOERROR ) {
		VUL__AUDIO_ERR( "Failed to close waveOut device.\n" );
	}
	return VUL_OK;
}

static vul_audio_return vul__audio_init_waveout( vul_audio_device *out, u32 frame_size )
{
	HMODULE waveout;
	if( ( waveout = LoadLibrary( "winmm.dll" ) ) == NULL ) {
		VUL__AUDIO_ERR( "Failed to load winmm.dll.\n" );
	}
	DLLOAD( fnWaveOutOpen, pWaveOutOpen, waveout, "waveOutOpen" );
	DLLOAD( fnWaveOutPrepareHeader, pWaveOutPrepareHeader, waveout, "waveOutPrepareHeader" );
	DLLOAD( fnWaveOutWrite, pWaveOutWrite, waveout, "waveOutWrite" );
	DLLOAD( fnWaveOutUnprepareHeader, pWaveOutUnprepareHeader, waveout, "waveOutUnprepareHeader" );
	DLLOAD( fnWaveOutClose, pWaveOutClose, waveout, "waveOutClose" );
	
   out->device.waveout.event = CreateEvent( 0, FALSE, FALSE, 0 );
   if( !out->device.waveout.event ) {
      VUL__AUDIO_ERR( "Failed to create event for waveout.\n" );
   }

	WAVEFORMATEX format;
	memset( &format, 0, sizeof( format ) );
	format.wFormatTag = WAVE_FORMAT_PCM;
	format.nChannels = out->channels;
	format.wBitsPerSample = sizeof( smp ) * 8;
	format.nSamplesPerSec = out->sample_rate;
	format.nBlockAlign = ( format.nChannels * format.wBitsPerSample ) / 8;
	format.nAvgBytesPerSec = format.nSamplesPerSec * format.nBlockAlign;

	if( pWaveOutOpen( &out->device.waveout.handle, WAVE_MAPPER, &format,
                     ( DWORD_PTR )out->device.waveout.event, 0, CALLBACK_EVENT ) != MMSYSERR_NOERROR ) {
		VUL__AUDIO_ERR( "Failed to open waveOut library.\n" );
	}

   for( int i = 0; i < 2; ++i ) {
      out->device.waveout.buffers[ i ] = ( smp* )malloc( frame_size );
      memset( out->device.waveout.buffers[ i ], 0, frame_size );
      memset( &out->device.waveout.headers[ i ], 0, sizeof( WAVEHDR ) );
      out->device.waveout.headers[ i ].dwBufferLength = frame_size;
      out->device.waveout.headers[ i ].lpData = ( LPSTR )out->device.waveout.buffers[ i ];
      if( MMSYSERR_NOERROR != pWaveOutPrepareHeader( out->device.waveout.handle,
                                                     &out->device.waveout.headers[ i ],
                                                     sizeof( WAVEHDR ) ) ) {
         VUL__AUDIO_ERR( "Failed to prepare waveout upload header %d.\n", i );
      }
   }

	out->lib = VUL__AUDIO_WINDOWS_WAVEOUT;
	return VUL_OK;
}

vul_audio_return vul__audio_write( vul_audio_device *dev, void *samples, u32 sample_count )
{
	switch( dev->lib ) {
	case VUL__AUDIO_WINDOWS_WAVEOUT: {
		return vul__audio_write_waveout( dev, samples, sample_count );
	} break;
	default: {
		VUL__AUDIO_ERR( "Unknown device library in use.\n" );
	}
	}
}

// --------------
// Public API
//

vul_audio_return vul_audio_destroy( vul_audio_device *dev, int drain_before_close )
{
   DWORD res;

   SetEvent( dev->close_event );
   res = WaitForSingleObject( dev->thread, INFINITE );
   if( res != WAIT_OBJECT_0 ) {
      VUL__AUDIO_ERR_NORETURN( "Failed to aquire access to the upload thread.\n" );
   }
   CloseHandle( dev->thread );
   CloseHandle( dev->close_event );
   CloseHandle( dev->mixer_mutex );

   vul__audio_mixer_destroy( &dev->mixer );

	switch( dev->lib ) {
	case VUL__AUDIO_WINDOWS_WAVEOUT: {
		return vul__audio_destroy_waveout( dev, drain_before_close );
	} break;
	default: {
		VUL__AUDIO_ERR( "Unknown device library in use.\n" );
	}
	}
}

vul_audio_return vul_audio_init( vul_audio_device *out, 
											vul_audio_mode mode,
											u32 channels, 
											u32 sample_rate,
                                 u32 frame_size,
											void (*mix_function)( void *, size_t, void* ),
                                 void *mix_function_user_data )
{
   vul_audio_return ret;
	if( !out ) {
      VUL__AUDIO_ERR( "No vul_audio_device supplied as out argument.\n" );
      return VUL_ERROR;
   }
   memset( out, 0, sizeof( vul_audio_device ) );

	out->channels = channels;
	out->sample_rate = sample_rate;
	out->mode = mode;
   if( mix_function ) {
      out->mix_function = mix_function;
      out->mix_function_data = mix_function_user_data;
   }
   vul__audio_mixer_init( &out->mixer, channels, frame_size / ( sizeof( smp ) * channels ), 32 );

	// Try waveOut
   ret = vul__audio_init_waveout( out, frame_size );
   if( ret != VUL_OK ) {
      VUL__AUDIO_ERR( "Failed to open audio device with any of the attempted libraries.\n" );
      return VUL_ERROR;
   }

   out->close_event = CreateEvent( 0, FALSE, FALSE, 0 );
   if( !out->close_event ) {
      VUL__AUDIO_ERR( "Failed to create event for waveout.\n" );
   }
   out->mixer_mutex = CreateMutex( NULL, FALSE, NULL );
   if( !out->mixer_mutex ) {
      VUL__AUDIO_ERR( "Failed to create mutex to lock mixer.\n" );
   }
   out->thread = CreateThread( NULL, 0, 
                               ( LPTHREAD_START_ROUTINE )vul__audio_callback,
                               ( LPVOID )out, 0, NULL );
   if( !out->thread ) {
      VUL__AUDIO_ERR( "Failed to create audio callback thread (%lu).\n", GetLastError( ) );
   }
   return VUL_OK;
}

#elif defined( VUL_OSX )

vul_audio_return vul__audio_mixer_wait_and_lock( vul_audio_device *dev )
{
   int res = pthread_mutex_lock( &dev->mixer_mutex );
   return res == 0 ? VUL_OK : VUL_ERROR;
}

vul_audio_return vul__audio_mixer_release( vul_audio_device *dev )
{
   int res = pthread_mutex_unlock( &dev->mixer_mutex );
   return res == 0 ? VUL_OK : VUL_ERROR;
}


static void vul__audio_callback( void *data, AudioQueueRef queue, AudioQueueBufferRef buffer )
{
   vul_audio_return ret;
   vul_audio_device *dev = ( vul_audio_device* )data;

   if( dev->mix_function ) {
      dev->mix_function( ( void* )buffer->mAudioData, 
                         ( size_t )buffer->mAudioDataByteSize,
                         dev->mix_function_data );
   } else {
      if( VUL_ERROR == vul__audio_mixer_wait_and_lock( dev ) ) {
         printf( "Failed to lock audio mixer.\n" );
			return;
      }

      vul__audio_mix( &dev->mixer );

      vul__audio_mixer_release( dev );

      memcpy( buffer->mAudioData, dev->mixer.samples, buffer->mAudioDataByteSize );
   }

   // Upload the data
   AudioQueueEnqueueBuffer( queue, buffer, 0, NULL );
}

// --------------
// Public API
//

vul_audio_return vul_audio_destroy( vul_audio_device *dev, int drain_before_close )
{
   pthread_mutex_destroy( &dev->mixer_mutex );

   AudioQueueStop( dev->queue, 1 );
   AudioQueueDispose( dev->queue, 0 );
}

vul_audio_return vul_audio_init( vul_audio_device *out, 
											vul_audio_mode mode,
											u32 channels, 
											u32 sample_rate,
                                 u32 frame_size,
											void (*mix_function)( void*, size_t, void* ),
                                 void *mix_function_user_data )
{
   vul_audio_return ret;
   OSStatus res;
   s32 i;

	if( !out ) {
      VUL__AUDIO_ERR( "No vul_audio_device supplied as out argument.\n" );
      return VUL_ERROR;
   }
	memset( out, 0, sizeof( vul_audio_device ) );

	out->channels = channels;
	out->sample_rate = sample_rate;
	out->mode = mode;
   if( mix_function ) {
      out->mix_function = mix_function;
      out->mix_function_data = mix_function_user_data;
   } else {
      vul__audio_mixer_init( &out->mixer, channels, frame_size / ( sizeof( smp ) * channels ), 32 );
   }

   // Initialize CoreAudio (do this in here instead of a subfunction because it's the only choice)
   AudioStreamBasicDescription format;
   memset( &format, 0, sizeof( format ) );
   format.mSampleRate = sample_rate;
   format.mFormatID = kAudioFormatLinearPCM;
   format.mFormatFlags = kLinearPCMFormatFlagIsSignedInteger
                       | kAudioFormatFlagIsPacked;
   format.mBytesPerPacket = channels * sizeof( smp );
   format.mFramesPerPacket = 1;
   format.mBytesPerFrame = format.mBytesPerPacket;
   format.mChannelsPerFrame = channels;
   format.mBitsPerChannel = sizeof( smp ) * 8;
   
   res = AudioQueueNewOutput( &format, 
                              vul__audio_callback, out, 
                              NULL, NULL, 0, 
                              &out->queue );
   if( res ) {
      VUL__AUDIO_ERR( "Failed to create core audio queue.\n" );
   }

   for( i = 0; i < 2; ++i ) {
      AudioQueueBufferRef buffer;
      res = AudioQueueAllocateBuffer( out->queue, frame_size, &buffer );
      if( res ) {
         VUL__AUDIO_ERR( "Failed to create core audio buffer.\n" );
      }
      buffer->mAudioDataByteSize = frame_size;
      memset( buffer->mAudioData, 0, buffer->mAudioDataByteSize );
      AudioQueueEnqueueBuffer( out->queue, buffer, 0, NULL );
   }

   res = AudioQueueStart( out->queue, NULL );
   if( res ) {
      VUL__AUDIO_ERR( "Failed to start core audio queue playback.\n" );
   }

   pthread_mutex_init( &out->mixer_mutex, NULL );

   out->lib = VUL__AUDIO_OSX_CORE_AUDIO;

   return VUL_OK;
}


#elif defined( VUL_LINUX )

#define DLLOAD( fn_sym, sym, lib, name )\
	sym = ( fn_sym )dlsym( lib, name );\
	if( sym == NULL ) {\
		VUL__AUDIO_ERR( "Failed to load symbol %s, error: %s.\n", name, dlerror() );\
	}

vul_audio_return vul__audio_mixer_wait_and_lock( vul_audio_device *dev )
{
   int res = pthread_mutex_lock( &dev->mixer_mutex );
   return res == 0 ? VUL_OK : VUL_ERROR;
}

vul_audio_return vul__audio_mixer_release( vul_audio_device *dev )
{
   int res = pthread_mutex_unlock( &dev->mixer_mutex );
   return res == 0 ? VUL_OK : VUL_ERROR;
}

void *vul__audio_callback( void *data )
{
   int res;
   vul_audio_return ret;
   vul_audio_device *dev = ( vul_audio_device* )data;

   while( 1 ) {
      res = pthread_mutex_lock( &dev->thread_mutex );
      if( res != 0 ) {
         printf( "Failed to lock thread mutex.\n" );
         break;
      }
      if( dev->thread_dead ) {
         break;
      }
      res = pthread_mutex_unlock( &dev->thread_mutex );
      if( res != 0 ) {
         printf( "Failed to unlock thread mutex.\n" );
         break;
      }

      ret = vul__audio_callback_internal( dev );
   }

   pthread_exit( NULL );
}

// ------
// OSS
// 
#if !defined( AFMT_S32_NE )
	#if defined( WORDS_BIGENDIAN )
		#if defined( AFMT_S32_LE )
			#define AFMT_S32_NE AFMT_S32_BE
		#else
			#define AFMT_S32_NE 0x00004000
		#endif
	#else
		#if defined( AFMT_S32_LE )
			#define AFMT_S32_NE AFMT_S32_LE
		#else
			#define AFMT_S32_NE 0x00002000
		#endif
	#endif
#endif
static vul_audio_return vul__audio_init_oss( vul_audio_device *dev, int mode )
{
	s32 tmp, fd;
	if( ( fd = open( "/dev/dsp", mode, 0 ) ) == -1 ) {
		VUL__AUDIO_ERR( "Unable to open device /dev/dsp.\n" );
	}

	// Set format
   #ifdef VUL_AUDIO_SAMPLE_16BIT
   tmp = AFMT_S16_NE;
   #elif VUL_AUDIO_SAMPLE_32BIT
   tmp = AFMT_S32_NE;
   #else
      Error in vul_audio: Must define sample size
   #endif
	if( ioctl( fd, SNDCTL_DSP_SETFMT, &tmp ) == -1 ) {
		close( fd );
		VUL__AUDIO_ERR( "Failed to set sample format.\n" );
	}
   #ifdef VUL_AUDIO_SAMPLE_16BIT
   if( tmp != AFMT_S16_NE ) {
      VUL__AUDIO_ERR( "Sample format returned from device does not match wanted format.\n" );return VUL_ERROR;
   }
   #elif VUL_AUDIO_SAMPLE_32BIT
   if( tmp != AFMT_S32_NE ) {
      VUL__AUDIO_ERR( "Sample format returned from device does not match wanted format.\n" );return VUL_ERROR;
   }
   #else
      Error in vul_audio: Must define sample size
   #endif

	// Set channel count
	tmp = dev->channels;
	if( ioctl( fd, SNDCTL_DSP_CHANNELS, &tmp ) == -1 ) {
		VUL__AUDIO_ERR( "Failed to set channel count.\n" );
	}
	if( tmp != dev->channels ) {
		VUL__AUDIO_ERR( "Channel count returned does not match wanted count.\n" );
	}

	// Set sample rate
	tmp = dev->sample_rate;
	if( ioctl( fd, SNDCTL_DSP_SPEED, &tmp ) == -1 ) {
		VUL__AUDIO_ERR( "Failed to set sample rate.\n" );
	}
	if( tmp != dev->sample_rate ) {
		VUL__AUDIO_ERR( "Sample rate returned does not match wanted rate.\n" );
	}

	// Store file descriptor
	dev->device.oss_device_fd = fd;
	dev->lib = VUL__AUDIO_LINUX_OSS;

	return VUL_OK;
}

static vul_audio_return vul__audio_write_oss( vul_audio_device *dev, void *samples, u32 sample_count )
{
	u32 size = sample_count * sizeof( smp ) * dev->channels;
	if( write( dev->device.oss_device_fd, samples, size ) != size ) {
		VUL__AUDIO_ERR( "Failed to write samples to device.\n" );
	}
	return VUL_OK;
}

// ------
// ALSA
//
typedef snd_pcm_sframes_t ( *fn_alsa_write )( snd_pcm_t *, const void *, snd_pcm_uframes_t );
typedef int ( *fn_alsa_prepare )( snd_pcm_t * );
typedef const char * ( *fn_alsa_strerror )( int );
typedef int ( *fn_alsa_open )( snd_pcm_t **, const char *, snd_pcm_stream_t, int );
typedef int ( *fn_alsa_hw_malloc )( snd_pcm_hw_params_t ** );
typedef int ( *fn_alsa_hw_any )( snd_pcm_t *, snd_pcm_hw_params_t * );
typedef int ( *fn_alsa_hw_set_access )( snd_pcm_t *, snd_pcm_hw_params_t *, snd_pcm_access_t );
typedef int ( *fn_alsa_hw_set_format )( snd_pcm_t *, snd_pcm_hw_params_t *, snd_pcm_format_t );
typedef int ( *fn_alsa_hw_set_rate_near )( snd_pcm_t *, snd_pcm_hw_params_t *, unsigned int *, int * );
typedef int ( *fn_alsa_hw_set_buffer_size )( snd_pcm_t *, snd_pcm_hw_params_t *, snd_pcm_uframes_t );
typedef int ( *fn_alsa_hw_set_period_size )( snd_pcm_t *, snd_pcm_hw_params_t *, snd_pcm_uframes_t, int );
typedef int ( *fn_alsa_hw_set_channels )( snd_pcm_t *, snd_pcm_hw_params_t *, unsigned int );
typedef int ( *fn_alsa_hw_params )( snd_pcm_t *, snd_pcm_hw_params_t * );
typedef void ( *fn_alsa_hw_free )( snd_pcm_hw_params_t * );
typedef int( *fn_alsa_sw_malloc )( snd_pcm_sw_params_t ** );
typedef int( *fn_alsa_sw_current )( snd_pcm_t *, snd_pcm_sw_params_t * );
typedef int( *fn_alsa_sw_set_avail_min )( snd_pcm_t *, snd_pcm_sw_params_t *, snd_pcm_uframes_t );
typedef int( *fn_alsa_sw_set_start_threshold )( snd_pcm_t *, snd_pcm_sw_params_t *, snd_pcm_uframes_t );
typedef int( *fn_alsa_sw_params )( snd_pcm_t *, snd_pcm_sw_params_t * );
typedef int( *fn_alsa_sw_free )( snd_pcm_sw_params_t * );
typedef int( *fn_alsa_drain )( snd_pcm_t * );
typedef int( *fn_alsa_close )( snd_pcm_t * );

fn_alsa_write alsa_write = 0;
fn_alsa_prepare alsa_prepare = 0;
fn_alsa_strerror alsa_strerror = 0;
fn_alsa_open alsa_open = 0;
fn_alsa_hw_malloc alsa_hw_malloc = 0;
fn_alsa_hw_any alsa_hw_any = 0;
fn_alsa_hw_set_access alsa_hw_set_access = 0;
fn_alsa_hw_set_format alsa_hw_set_format = 0;
fn_alsa_hw_set_rate_near alsa_hw_set_rate_near = 0;
fn_alsa_hw_set_buffer_size alsa_hw_set_buffer_size = 0;
fn_alsa_hw_set_period_size alsa_hw_set_period_size = 0;
fn_alsa_hw_set_channels alsa_hw_set_channels = 0;
fn_alsa_hw_params alsa_hw_params = 0;
fn_alsa_hw_free alsa_hw_free = 0;
fn_alsa_sw_malloc alsa_sw_malloc = 0;
fn_alsa_sw_current alsa_sw_current = 0;
fn_alsa_sw_set_avail_min alsa_sw_set_avail_min = 0;
fn_alsa_sw_set_start_threshold alsa_sw_set_start_threshold = 0;
fn_alsa_sw_params alsa_sw_params = 0;
fn_alsa_sw_free alsa_sw_free = 0;
fn_alsa_drain alsa_drain = 0;
fn_alsa_close alsa_close = 0;

// @TODO(thynn): Make this work properly (as in, poll for when to continue writing,
// and not hardcode the wait!), see write_and_poll_loop in
// http://www.alsa-project.org/alsa-doc/alsa-lib/_2test_2pcm_8c-example.html 
static vul_audio_return vul__audio_write_alsa( vul_audio_device *dev, void *samples, u32 sample_count )
{
   s32 r;
   u32 size = sample_count;// * dev->mixer.channels * sizeof( smp );

   while( -EAGAIN == ( r = alsa_write( dev->device.alsa.handle, 
                                       samples, 
                                       size ) ) )
      ; // Keep looping
	if( r == -EPIPE ) {
		// Reprepare before failing
		alsa_prepare( dev->device.alsa.handle );
		VUL__AUDIO_ERR( "ALSA write returned in a buffer overrun.\n" )
	}
	if( r < 0 ) { // @TODO(thynn): Underrun, attempt a recovery
		VUL__AUDIO_ERR( "ALSA write failed: %s.\n", alsa_strerror( r ) );
	}
	if( r != size ) {
		VUL__AUDIO_ERR( "Frame count write (%d) does not match wanted count (%d).\n", r, size );
	}
   vul_sleep(20);
	return VUL_OK;
}

static vul_audio_return vul__audio_init_alsa( vul_audio_device *dev, const char *device_name, u32 frame_size )
{
	snd_pcm_hw_params_t *hwp;
	snd_pcm_sw_params_t *swp;
	int err;
	
	// Library loads
	if( ( dev->device.alsa.dlib = dlopen( "libasound.so", RTLD_NOW ) ) == NULL ) {
		VUL__AUDIO_ERR( "Failed to load ALSA library.\n" );
	}
	DLLOAD( fn_alsa_prepare, alsa_prepare, dev->device.alsa.dlib, "snd_pcm_prepare" );
	DLLOAD( fn_alsa_write, alsa_write, dev->device.alsa.dlib, "snd_pcm_writei" );
	DLLOAD( fn_alsa_strerror, alsa_strerror, dev->device.alsa.dlib, "snd_strerror" );
	DLLOAD( fn_alsa_open, alsa_open, dev->device.alsa.dlib, "snd_pcm_open" );
	DLLOAD( fn_alsa_hw_malloc, alsa_hw_malloc, dev->device.alsa.dlib, "snd_pcm_hw_params_malloc" );
	DLLOAD( fn_alsa_hw_any, alsa_hw_any, dev->device.alsa.dlib, "snd_pcm_hw_params_any" );
	DLLOAD( fn_alsa_hw_set_access, alsa_hw_set_access, dev->device.alsa.dlib, "snd_pcm_hw_params_set_access" );
	DLLOAD( fn_alsa_hw_set_format, alsa_hw_set_format, dev->device.alsa.dlib, "snd_pcm_hw_params_set_format" );
	DLLOAD( fn_alsa_hw_set_rate_near, alsa_hw_set_rate_near, dev->device.alsa.dlib, "snd_pcm_hw_params_set_rate_near" );
	DLLOAD( fn_alsa_hw_set_buffer_size, alsa_hw_set_buffer_size, dev->device.alsa.dlib, "snd_pcm_hw_params_set_buffer_size" );
	DLLOAD( fn_alsa_hw_set_period_size, alsa_hw_set_period_size, dev->device.alsa.dlib, "snd_pcm_hw_params_set_period_size" );
	DLLOAD( fn_alsa_hw_set_channels, alsa_hw_set_channels, dev->device.alsa.dlib, "snd_pcm_hw_params_set_channels" );
	DLLOAD( fn_alsa_hw_params, alsa_hw_params, dev->device.alsa.dlib, "snd_pcm_hw_params" );
	DLLOAD( fn_alsa_hw_free, alsa_hw_free, dev->device.alsa.dlib, "snd_pcm_hw_params_free" );
	DLLOAD( fn_alsa_sw_malloc, alsa_sw_malloc, dev->device.alsa.dlib, "snd_pcm_sw_params_malloc" );
	DLLOAD( fn_alsa_sw_current, alsa_sw_current, dev->device.alsa.dlib, "snd_pcm_sw_params_current" );
	DLLOAD( fn_alsa_sw_set_avail_min, alsa_sw_set_avail_min, dev->device.alsa.dlib, "snd_pcm_sw_params_set_avail_min" );
	DLLOAD( fn_alsa_sw_set_start_threshold, alsa_sw_set_start_threshold, dev->device.alsa.dlib, "snd_pcm_sw_params_set_start_threshold" );
	DLLOAD( fn_alsa_sw_params, alsa_sw_params, dev->device.alsa.dlib, "snd_pcm_sw_params" );
	DLLOAD( fn_alsa_sw_free, alsa_sw_free, dev->device.alsa.dlib, "snd_pcm_sw_params_free" );
	DLLOAD( fn_alsa_drain, alsa_drain, dev->device.alsa.dlib, "snd_pcm_drain" );
	DLLOAD( fn_alsa_close, alsa_close, dev->device.alsa.dlib, "snd_pcm_close" );
	// Hardware parameters

	if( ( err = alsa_open( &dev->device.alsa.handle, device_name, SND_PCM_STREAM_PLAYBACK, 0 ) ) < 0 ) {
		VUL__AUDIO_ERR( "Failed to open ALSA device %s.\n", device_name );
	}

	if( ( err = alsa_hw_malloc( &hwp ) ) < 0 ) {
		VUL__AUDIO_ERR( "Failed to allocate ALSA hardware parameters struct.\n" );
	}
	if( ( err = alsa_hw_any( dev->device.alsa.handle, hwp ) ) < 0 ) {
		VUL__AUDIO_ERR( "Failed to get initial ALSA hardware parameters.\n" );
	}

	if( ( err = alsa_hw_set_access( dev->device.alsa.handle, hwp, SND_PCM_ACCESS_RW_INTERLEAVED ) ) < 0 ) {
		VUL__AUDIO_ERR( "Failed to set ALSA access pattern.\n" );
	}

	snd_pcm_format_t fmt;
   #ifdef VUL_AUDIO_SAMPLE_16BIT
   fmt = SND_PCM_FORMAT_S16_LE;
   #elif VUL_AUDIO_SAMPLE_32BIT
   fmt = SND_PCM_FORMAT_S32_LE;
   #else
		Error in vul_audio: Must define sample size
   #endif
	if( ( err = alsa_hw_set_format( dev->device.alsa.handle, hwp, fmt ) ) < 0 ) {
		VUL__AUDIO_ERR( "Failed to set ALSA sample format.\n" );
	}
   int rate = dev->sample_rate;
	if( ( err = alsa_hw_set_rate_near( dev->device.alsa.handle, hwp, &dev->sample_rate, 0 ) ) < 0 ) {
		VUL__AUDIO_ERR( "Failed to set ALSA sample rate.\n" );
	}
   if( rate != dev->sample_rate ) {
      VUL__AUDIO_ERR( "Failed to set ALSA sample rate to desired rate (%d vs %d desired).\n", dev->sample_rate, rate );
   }
	if( ( err = alsa_hw_set_channels( dev->device.alsa.handle, hwp, dev->channels ) ) < 0 ) {
		VUL__AUDIO_ERR( "Failed to set ALSA channel count.\n" );
	}
   if( ( err = alsa_hw_set_buffer_size( dev->device.alsa.handle, hwp, frame_size ) ) < 0 ) {
      VUL__AUDIO_ERR( "Failed to set ALSA buffer size.\n" );
   }
   if( ( err = alsa_hw_set_period_size( dev->device.alsa.handle, hwp, frame_size / 4, 0 ) ) < 0 ) {
      VUL__AUDIO_ERR( "Failed to set ALSA period size.\n" );
   }
	if( ( err = alsa_hw_params( dev->device.alsa.handle, hwp ) ) < 0 ) {
		VUL__AUDIO_ERR( "Failed to set final ALSA hardware parameters.\n" );
	}

	alsa_hw_free( hwp );

	// Software parameters
	if( ( err = alsa_sw_malloc( &swp ) ) < 0 ) {
		VUL__AUDIO_ERR( "Failed to allocate ALSA software parameters struct.\n" );
	}
	if( ( err = alsa_sw_current( dev->device.alsa.handle, swp ) ) < 0 ) {
		VUL__AUDIO_ERR( "Failed to get current ALSA software parameters.\n" );
	}
	if( ( err = alsa_sw_set_avail_min( dev->device.alsa.handle, swp, frame_size ) ) < 0 ) {
		VUL__AUDIO_ERR( "Failed to set ALSA frame size.\n" );
	}
	if( ( err = alsa_sw_set_start_threshold( dev->device.alsa.handle, swp, frame_size ) ) < 0 ) {
		VUL__AUDIO_ERR( "Failed to set ALSA start threshold.\n" );
	}
	if( ( err = alsa_sw_params( dev->device.alsa.handle, swp ) ) < 0 ) {
		VUL__AUDIO_ERR( "Failed to set final ALSA software parameters.\n" );
	}

	// Prepare device
	if( ( err = alsa_prepare( dev->device.alsa.handle ) ) < 0 ) {
		VUL__AUDIO_ERR( "Failed to start ALSA device.\n" );
	}

	dev->lib = VUL__AUDIO_LINUX_ALSA;
	return VUL_OK;
}

// ---------------
// Pulse audio
//

typedef pa_simple * ( *fn_pulse_new )( const char *, const char *, pa_stream_direction_t, 
                                       const char *, const char *, const pa_sample_spec*, 
                                       const pa_channel_map*, const pa_buffer_attr*, int * );
typedef void ( *fn_pulse_free )( pa_simple * );
typedef int ( *fn_pulse_write )( pa_simple *, const void *, size_t, int * );
typedef int ( *fn_pulse_drain )( pa_simple *, int * );
typedef const char* ( *fn_pulse_error )( int );

fn_pulse_new pulse_new = 0;
fn_pulse_free pulse_free = 0;
fn_pulse_write pulse_write = 0;
fn_pulse_drain pulse_drain = 0;
fn_pulse_error pulse_error = 0;

vul_audio_return vul__audio_init_pulse( vul_audio_device *dev, const char *name, const char *description, const char *server_name, const char *device_name )
{
	// Library loads
	if( ( dev->device.pulse.dlib = dlopen( "libpulse.so", RTLD_NOW ) ) == NULL ) {
		VUL__AUDIO_ERR( "Failed to load PulseAudio library.\n" );
	}
	if( ( dev->device.pulse.dlib_simple = dlopen( "libpulse-simple.so", RTLD_NOW ) ) == NULL ) {
		VUL__AUDIO_ERR( "Failed to load PulseAudio Simple API library.\n" );
	}
	DLLOAD( fn_pulse_new, pulse_new, dev->device.pulse.dlib_simple, "pa_simple_new" );
	DLLOAD( fn_pulse_free, pulse_free, dev->device.pulse.dlib_simple, "pa_simple_free" );
	DLLOAD( fn_pulse_write, pulse_write, dev->device.pulse.dlib_simple, "pa_simple_write" );
	DLLOAD( fn_pulse_drain, pulse_drain, dev->device.pulse.dlib_simple, "pa_simple_drain" );
	DLLOAD( fn_pulse_error, pulse_error, dev->device.pulse.dlib, "pa_strerror" );

	pa_sample_spec ss;
   #ifdef VUL_AUDIO_SAMPLE_16BIT
   ss.format = PA_SAMPLE_S16NE;
   #elif VUL_AUDIO_SAMPLE_32BIT
   ss.format = PA_SAMPLE_S32NE;
   #else
		Error in vul_audio: Must define sample size
   #endif
	ss.channels = dev->channels;
	ss.rate = dev->sample_rate;

	enum pa_stream_direction dir = PA_STREAM_NODIRECTION;
	switch( dev->mode ) {
	case VUL_AUDIO_MODE_PLAYBACK:
		dir = PA_STREAM_PLAYBACK;
		break;
	case VUL_AUDIO_MODE_RECORDING:
		dir = PA_STREAM_RECORD;
		break;
	case VUL_AUDIO_MODE_DUPLEX:
		VUL__AUDIO_ERR( "Duplex not yet supported in vul_audio.\n" );
      return VUL_ERROR;
	default:
		VUL__AUDIO_ERR( "Unkown device mode encountered.\n" );
      return VUL_ERROR;
	}
	
	dev->device.pulse.client = pulse_new( server_name,
													  name,
													  dir,
                                         device_name,
													  description,
													  &ss,
													  NULL, // Default channel map, 
													  NULL, // Default buffering attributes
													  NULL ); // Ignore error code
   if( !dev->device.pulse.client ) {
      VUL__AUDIO_ERR( "Failed to open pulse device.\n" );
   }

	dev->lib = VUL__AUDIO_LINUX_PULSE;
	return VUL_OK;
}

vul_audio_return vul__audio_write_pulse( vul_audio_device *dev, void *samples, u32 sample_count )
{
	u32 size = sample_count * sizeof( smp ) * dev->channels;
	s32 err;
	if( pulse_write( dev->device.pulse.client,
						  samples,
						  size,
						  &err ) < 0 ) {
		VUL__AUDIO_ERR( "Failed to write samples to PulseAudio: %s.\n", pulse_error( err ) );
	}
	return VUL_OK;
}

vul_audio_return vul__audio_write( vul_audio_device *dev, void *samples, u32 sample_count )
{
	if( !( dev->mode == VUL_AUDIO_MODE_PLAYBACK || dev->mode == VUL_AUDIO_MODE_DUPLEX ) ) {
		VUL__AUDIO_ERR( "Device write requested while not in playback or duplex mode.\n" );
	}

	switch( dev->lib ) {
	case VUL__AUDIO_LINUX_OSS:
		return vul__audio_write_oss( dev, samples, sample_count );
	case VUL__AUDIO_LINUX_ALSA:
		return vul__audio_write_alsa( dev, samples, sample_count );
	case VUL__AUDIO_LINUX_PULSE:
		return vul__audio_write_pulse( dev, samples, sample_count );
	default:
		VUL__AUDIO_ERR( "Unknown device library in use.\n" );
	}
	return VUL_OK;
}

// --------------
// Public API
//

vul_audio_return vul_audio_destroy( vul_audio_device *dev, int drain_before_close )
{
   int res;
   
   res = pthread_mutex_lock( &dev->thread_mutex );
   if( res != 0 ) {
      VUL__AUDIO_ERR( "Failed to obtain thread mutex.\n" );
   }
   dev->thread_dead = 1;
   pthread_mutex_unlock( &dev->thread_mutex );

   pthread_join( dev->thread, NULL );

   pthread_mutex_destroy( &dev->thread_mutex );
   pthread_mutex_destroy( &dev->mixer_mutex );

	switch( dev->lib ) {
	case VUL__AUDIO_LINUX_ALSA:
		if( drain_before_close ) alsa_drain( dev->device.alsa.handle ); 
		alsa_close( dev->device.alsa.handle );
		dlclose( dev->device.alsa.dlib );
		break;
	case VUL__AUDIO_LINUX_OSS:
		close( dev->device.oss_device_fd );
		break;
	case VUL__AUDIO_LINUX_PULSE: {
		int err;
		if( drain_before_close ) pulse_drain( dev->device.pulse.client, &err ); 
		pulse_free( dev->device.pulse.client );
		dlclose( dev->device.pulse.dlib );
		dlclose( dev->device.pulse.dlib_simple );
	} break;
	default:
		VUL__AUDIO_ERR( "Unknown device library in use.\n" );
	}
   return VUL_OK;
}

vul_audio_return vul_audio_init( vul_audio_device *out, 
                                 const char *server_name, // Only pulse
											const char *device_name, // Both pulse and ALSA
                                 const char *description, // Only pulse
											vul_audio_mode mode,
											u32 channels, 
											u32 sample_rate,
                                 u32 frame_size,
											void (*mix_function)( void*, size_t, void* ),
                                 void *mix_function_user_data )
{
   vul_audio_return ret;
   int res;

	if( !out ) {
      VUL__AUDIO_ERR( "No vul_audio_device supplied as out argument.\n" );
      return VUL_ERROR;
   }
	memset( out, 0, sizeof( vul_audio_device ) );

	out->channels = channels;
	out->sample_rate = sample_rate;
	out->mode = mode;
   if( mix_function ) {
      out->mix_function = mix_function;
      out->mix_function_data = mix_function_user_data;
   } else {
      vul__audio_mixer_init( &out->mixer, channels, frame_size / ( sizeof( smp ) * channels ), 32 );
   }

	// Try pulse
	ret = vul__audio_init_pulse( out, "vul_audio", description, server_name, device_name );

	// Try ALSA
	if( VUL_OK != ret ) {
      if( !device_name ) {
         device_name = "default";
      }
	   ret = vul__audio_init_alsa( out, device_name, frame_size );
      if( VUL_OK != ret ) {
         // Try OSS
         int fdmode = O_WRONLY;
         ret = vul__audio_init_oss( out, fdmode );
         if( VUL_OK != ret ) {
            VUL__AUDIO_ERR( "Failed to open audio device with any of the attempted libraries.\n" );
         }
      }
   }
   
   out->thread_dead = 0;
   pthread_mutex_init( &out->mixer_mutex, NULL );
   pthread_mutex_init( &out->thread_mutex, NULL );

   res = pthread_create( &out->thread, NULL, vul__audio_callback, out );
   if( res != 0 ) {
      VUL__AUDIO_ERR( "Failed to create audio callback thread (%d).\n", res );
   }
   return VUL_OK;
}

#else
	You must define an operating system (VUL_WINDOWS, VUL_LINUX, VUL_OSX)
#endif

#undef ERR

#ifdef __cplusplus
}
#endif

#undef smx
#undef smp
#undef clamp_max
#undef clamp_min

#endif // VUL_DEFINE

#ifndef VUL_TYPES_H
#undef s8
#undef s16
#undef s32
#undef s64
#undef u8t
#undef u16
#undef u32
#undef u64
#undef f32
#undef f64
#endif // VUL_TYPES_H

