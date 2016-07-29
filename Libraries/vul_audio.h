/*
 * Villains' Utility Library - Thomas Martin Schmid, 2016. Public domain?
 *
 * This file contains a low-level wrapper for different audio APIs.
 * The goal is to support (in order of attempts/fallback):
 *  - Linux: pulseaudio -> alsa -> oss
 *  - OSX: TODO
 *  - Windows: TODO
 *
 * Error reporting: A lot can go wrong, especially in setup. We have multiple
 * ways to report errors back to the user (in addition to return values being
 * VUL_ERROR). Define any of these to use it (no default):
 *  - VUL_AUDIO_ERROR_STDERR: Print an erro message to stderr.
 *  - VUL_AUDIO_ERROR_STR: Last error message is written to vul_last_error.
 *  - VUL_AUDIO_ERROR_ASSERT: Trigger an assert on error.
 *  - VUL_AUDIO_ERROR_QUIET: Do no reporting at all, simple fail.
 *  - VUL_AUDIO_ERROR_CUSTOM: Define your own macro VUL_AUDIO_ERROR_CUSTOM( ... )
 *    and do whatever you want with the error message (printf style formatting/arguments)
 *    before failing.
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
#ifndef VUL_AUDIO_H
#define VUL_AUDIO_H

/**
 * Define in exactly _one_ C/CPP file.
 */
//#define VUL_DEFINE

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#if defined( VUL_WINDOWS )
	#pragma comment(lib, "dsound.lib")
	#pragma comment(lib, "dxguid.lib")
	#pragma comment(lib, "winmm.lib")
	#include <windows.h>
	#include <mmsystem.h>
	#include <dsound.h>
#elif defined( VUL_LINUX )
	#include <dlfcn.h>
	//#include <poll.h>
	#include <sys/ioctl.h>
	#include <sys/soundcard.h>
	#include <alsa/asoundlib.h>
	#include <pulse/simple.h>
	#include <pulse/error.h>
#elif defined( VUL_OSX )
	BAH
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

typedef enum vul__audio_lib {
	VUL__AUDIO_LINUX_ALSA,
	VUL__AUDIO_LINUX_PULSE,
	VUL__AUDIO_LINUX_OSS,

	VUL__AUDIO_Count
} vul__audio_lib;

typedef enum vul_audio_mode {
	VUL_AUDIO_MODE_PLAYBACK,
	VUL_AUDIO_MODE_RECORDING,
	VUL_AUDIO_MODE_DUPLEX
} vul_audio_mode;

typedef enum vul_audio_sample_format {
	VUL_AUDIO_SAMPLE_FORMAT_16BIT_NATIVE,
	VUL_AUDIO_SAMPLE_FORMAT_32BIT_NATIVE,

	VUL_AUDIO_SAMPLE_FORMAT_Count // @TODO(thynn): More formats
} vul_audio_sample_format;

typedef enum vul_audio_return {
	VUL_OK = 0,
	VUL_ERROR = 1
} vul_audio_return;

typedef struct vul_audio_device {
	u32 channels;
	u32 sample_rate;
	vul_audio_sample_format format;
	vul_audio_mode mode;

#ifdef VUL_WINDOWS
	LPDIRECTSOUND dsound;
	LPDIRECTSOUNDBUFFER dsound_buffer;
	HPSTR buffer_ptr;
#elif defined( VUL_ODX )
	BAH
#elif defined( VUL_LINUX )
	vul__audio_lib lib;
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

#ifdef _cplusplus
extern "C" {
#endif

/*
 * @TODO(thynn): Document
 */
vul_audio_return vul_audio_write( vul_audio_device *dev, void *samples, u32 sample_count );

/*
 * @TODO(thynn): Document
 */
vul_audio_return vul_audio_read( vul_audio_device *dev, void *samples, u32 sample_count );

/*
 * @TODO(thynn): Document
 */
vul_audio_return vul_audio_destroy( vul_audio_device *dev, int drain_before_close );


/*
 * @TODO(thynn): Document
 */
#ifdef VUL_WINDOWS
vul_audio_return vul_audio_init( vul_audio_device *out, 
											HWND hwnd,
											vul_audio_mode mode,
											u32 channels, 
											u32 sample_rate,
											vul_audio_sample_format fmt );
#elif VUL_OSX
	TODO
#elif VUL_LINUX
vul_audio_return vul_audio_init( vul_audio_device *out, 
											const char *device_name,  
											vul_audio_mode mode,
											u32 channels, 
											u32 sample_rate,
											vul_audio_sample_format fmt );
#else
	You must define an operating system (VUL_WINDOWS, VUL_LINUX, VUL_OSX)
#endif

#ifdef _cplusplus
}
#endif
#endif // VUL_AUDIO_H

#ifdef VUL_DEFINE

#ifdef _cplusplus
extern "C" {
#endif

#ifdef VUL_AUDIO_ERROR_STDERR
#define ERR( ... )\
	fprintf( stderr, __VA_ARGS__ );\
	return VUL_ERROR;
#elif defined( VUL_AUDIO_ERROR_STR )
char vul_last_error[ 1024 ];
#define ERR( ... )\
	snprintf( vul_last_error, 1023, __VA_ARGS__ );\
	vul_last_error[ 1023 ] = 0;\
	return VUL_ERROR;
#elif defined( VUL_AUDIO_ERROR_ASSERT )
#define ERR( ... )\
	assert( 0 );\
	return VUL_ERROR;
#elif defined( VUL_AUDIO_ERROR_QUIET )
#define ERR( ... )\
	return VUL_ERROR;
#elif defined( VUL_AUDIO_ERROR_CUSTOM )
#define ERR VUL_AUDIO_ERROR_CUSTOM
#endif

// -----------
// Helpers
//
static u32 vul__audio_bytes_per_sample( vul_audio_device *dev )
{
	u32 sample_size = 2;
	switch( dev->format ) {
	case VUL_AUDIO_SAMPLE_FORMAT_16BIT_NATIVE:
		sample_size = 2;
		break;
	case VUL_AUDIO_SAMPLE_FORMAT_32BIT_NATIVE:
		sample_size = 4;
		break;
	default:
		ERR( "Unknown sample format.\n" );
	}
	return sample_size;
}

#ifdef VUL_WINDOWS

#define DLLOAD( sym, lib, name )\
	sym = ( void* )GetProcAddress( lib, name );\
	if( sym == NULL ) {\
		ERR( "Failed to load symbol %s from DirectSound.\n", name );\
	}


HRESULT ( WINAPI *pDirectSoundCreate )( GUID FAR *, LPDIRECTSOUND FAR *, IUnknown FAR * );

vul_audio_return vul_audio_write( vul_audio_device *dev, void *samples, u32 sample_count )
{
}

vul_audio_return vul_audio_read( vul_audio_device *dev, void *samples, u32 sample_count )
{
	if( !( dev->mode == VUL_AUDIO_MODE_RECORDING || dev->mode == VUL_AUDIO_MODE_DUPLEX ) ) {
		ERR( "Device read requested while not in recording or duplex mode.\n" );
	}

}

vul_audio_return vul_audio_destroy( vul_audio_device *dev, int drain_before_close )
{
}

vul_audio_return vul_audio_init( vul_audio_device *out, 
											HWND hwnd,
											vul_audio_mode mode,
											u32 channels, 
											u32 sample_rate,
											vul_audio_sample_format fmt )
{
	HMODULE dsound;
	if( ( dsound = LoadLibrary( "dsound.dll" ) ) == NULL ) {
		ERR( "Failed to load dsound.dll.\n" );
	}
	DLLOAD( pDirectSoundCreate, dsound, "DirectSoundCreate" );
	
	assert( out );
	memset( out, 0, sizeof( vul_audio_device ) );

	out->channels = channels;
	out->sample_rate = sample_rate;
	out->format = fmt;
	out->mode = mode;

	DSBUFFERDESC bufferdesc;
	WAVEFORMATEX waveformat;

	HRESULT res = pDirectSoundCreate( NULL, // Default device, @TODO(thynn): Make this a parameter?
												 &out->dsound,
												 NULL );
	if( res != DS_OK ) {
		ERR( "Failed to start DirectSound.\n" );
	}

	DSCAPS dscaps;
	dscaps.dwSize = sizeof( dscaps );
	if( out->dsound->lbVtbl->GetCaps( out->dsound, &dscaps ) != DS_OK ) {
		ERR( "Failed to get DirectSound capabilities.\n" );
	}

	if( dscaps.dwFlags & DSCAPS_EMULDRIVER ) {
		out->dsound->lbVtbl->Release( out->dsound );
		ERR( "No DirectSound driver installed.\n" );
	}

	if( out->dsound->lbVtbl->SetCooperativeLevel( out->dsound, hwnd, DSSCL_PRIORITY ) != DS_OK ) {
		out->dsound->lbVtbl->Release( out->dsound );
		ERR( "Failed to set coop level to exclusive.\n" );
	}

	WAVEFORMATEX format;
	memset( &format, 0, sizeof( format ) );
	format.wFormatTag = WAVE_FORMAT_PCM;
	format.nChannels = out->channels;
	format.wBitsPerSample = vul__audio_bytes_per_sample( out ) * 8;
	format.nSamplesPerSec = out->sample_rate;
	format.nBlockAlign = format.nChannels * format.wBitsPerSample / 8;
	format.cbSize = 0;
	format.nAvgBytesPerSec = format.nSamplesPerSec * format.nBlockAlign;

	DSBUFFERDESC buffer;
	memset( &buffer, 0, sizeof( buffer ) );
	buffer.dwSize = sizeof( DSBUFFERDESC );
	buffer.dwFlags = DSBCAPS_PRIMARYBUFFER;
	buffer.dwBufferBytes = 0;
	buffer.lpwfxFormat = NULL;

	if( out->dsound->lbVtbl->CreateSoundBuffer( out->dsound, &buffer, &out->dsound_buffer, NULL ) != DS_OK ) {
		ERR( "Failed to create primary DirectSound buffer.\n" );
	}
	if( out->dsound_buffer->lpVtbl->SetFormat( out->dsound_buffer, &format ) != DS_OK ) {
		ERR( "Failed to set primary sound buffer.\n" );
	}
	// @TODO(thynn): Fallback for the above two (see https://github.com/id-Software/Quake/blob/master/WinQuake/snd_win.c#L340). Do we _really_ need to set these?
	DSBCAPS buffer_caps;
	memset( &buffer_caps, 0, sizeof( buffer_caps ) );
	buffer_caps.dwSize = sizeof( DSBCAPS );
	// @TODO(thynn): Change coop level if writing to primary, get capabilities of the new buffer, create fallback if nonprimary etc.
	

	// Make sure the mixer is running
	out->dsound->lpVtbl->Play( out->dsound_buffer, 0, 0, DSBPLAY_LOOPING );

	// Initialize the buffer
		// @TODO(thynn): Lock, unlock, stop, get current pos, start, idk...

	// @TODO(thynn): Fallback to waveOut!
}


#elif defined( VUL_OSX )

#elif defined( VUL_LINUX )

#define DLLOAD( sym, lib, name )\
	sym = dlsym( lib, name );\
	if( sym == NULL ) {\
		ERR( "Failed to load symbol %s, error: %s.\n", name, dlerror() );\
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
		ERR( "Unable to open device /dev/dsp.\n" );
	}

	// Set format
	switch( dev->format ) {
	case VUL_AUDIO_SAMPLE_FORMAT_16BIT_NATIVE:
		tmp = AFMT_S16_NE;
		break;
	case VUL_AUDIO_SAMPLE_FORMAT_32BIT_NATIVE:
		tmp = AFMT_S32_NE;
		break;
	default:
		ERR( "Unknown sample format encountered.\n" );
	}
	if( ioctl( fd, SNDCTL_DSP_SETFMT, &tmp ) == -1 ) {
		close( fd );
		ERR( "Failed to set sample format.\n" );
	}
	switch( dev->format ) {
	case VUL_AUDIO_SAMPLE_FORMAT_16BIT_NATIVE: 
		if( tmp != AFMT_S16_NE ) {
			ERR( "Sample format returned from device does not match wanted format.\n" );return VUL_ERROR;
		}
		break;
	case VUL_AUDIO_SAMPLE_FORMAT_32BIT_NATIVE: 
		if( tmp != AFMT_S32_NE ) {
			ERR( "Sample format returned from device does not match wanted format.\n" );return VUL_ERROR;
		}
		break;
	default:
		ERR( "Unknown sample format encountered.\n" );
	}

	// Set channel count
	tmp = dev->channels;
	if( ioctl( fd, SNDCTL_DSP_CHANNELS, &tmp ) == -1 ) {
		ERR( "Failed to set channel count.\n" );
	}
	if( tmp != dev->channels ) {
		ERR( "Channel count returned does not match wanted count.\n" );
	}

	// Set sample rate
	tmp = dev->sample_rate;
	if( ioctl( fd, SNDCTL_DSP_SPEED, &tmp ) == -1 ) {
		ERR( "Failed to set sample rate.\n" );
	}
	if( tmp != dev->sample_rate ) {
		ERR( "Sample rate returned does not match wanted rate.\n" );
	}

	// Store file descriptor
	dev->device.oss_device_fd = fd;
	dev->lib = VUL__AUDIO_LINUX_OSS;

	return VUL_OK;
}

static vul_audio_return vul__audio_write_oss( vul_audio_device *dev, void *samples, u32 sample_count )
{
	u32 size = sample_count * vul__audio_bytes_per_sample( dev );
	if( write( dev->device.oss_device_fd, samples, size ) != size ) {
		ERR( "Failed to write samples to device.\n" );
	}
	return VUL_OK;
}

// ------
// ALSA
//

snd_pcm_sframes_t ( *alsa_read)( snd_pcm_t *, void *, snd_pcm_uframes_t ) = 0;
snd_pcm_sframes_t ( *alsa_write )( snd_pcm_t *, const void *, snd_pcm_uframes_t ) = 0;
int ( *alsa_prepare )( snd_pcm_t * ) = 0;
const char * ( *alsa_strerror )( int ) = 0;
int ( *alsa_open )( snd_pcm_t **, const char *, snd_pcm_stream_t, int ) = 0;
int ( *alsa_hw_malloc )( snd_pcm_hw_params_t ** ) = 0;
int ( *alsa_hw_any )( snd_pcm_t *, snd_pcm_hw_params_t * ) = 0;
int ( *alsa_hw_set_access )( snd_pcm_t *, snd_pcm_hw_params_t *, snd_pcm_access_t ) = 0;
int ( *alsa_hw_set_format )( snd_pcm_t *, snd_pcm_hw_params_t *, snd_pcm_format_t ) = 0;
int ( *alsa_hw_set_rate_near )( snd_pcm_t *, snd_pcm_hw_params_t *, unsigned int *, int * ) = 0;
int ( *alsa_hw_set_channels )( snd_pcm_t *, snd_pcm_hw_params_t *, unsigned int ) = 0;
int ( *alsa_hw_params )( snd_pcm_t *, snd_pcm_hw_params_t * ) = 0;
void ( *alsa_hw_free )( snd_pcm_hw_params_t * ) = 0;
int( *alsa_sw_malloc )( snd_pcm_sw_params_t ** ) = 0;
int( *alsa_sw_current )( snd_pcm_t *, snd_pcm_sw_params_t * ) = 0;
int( *alsa_sw_set_avail_min )( snd_pcm_t *, snd_pcm_sw_params_t *, snd_pcm_uframes_t ) = 0;
int( *alsa_sw_set_start_threshold )( snd_pcm_t *, snd_pcm_sw_params_t *, snd_pcm_uframes_t ) = 0;
int( *alsa_sw_params )( snd_pcm_t *, snd_pcm_sw_params_t * ) = 0;
int( *alsa_sw_free )( snd_pcm_sw_params_t * ) = 0;
int( *alsa_drain )( snd_pcm_t * ) = 0;
int( *alsa_close )( snd_pcm_t * ) = 0;

static vul_audio_return vul__audio_read_alsa( vul_audio_device *dev, void *samples, u32 sample_count )
{	
	u32 size = sample_count * vul__audio_bytes_per_sample( dev );
	u32 frames = sample_count / dev->channels;

	s32 r = alsa_read( dev->device.alsa.handle, samples, frames ); // @TODO(thynn): Do we want frame-count as an argument to all write functions??
	if( r == -EPIPE ) {
		// Reprepare before failing
		alsa_prepare( dev->device.alsa.handle );
		ERR( "ALSA read returned in a buffer overrun.\n" )
	}
	if( r < 0 ) {
		ERR( "ALSA read failed: %s.\n", alsa_strerror( r ) );
	}
	if( r != frames ) {
		ERR( "Frame count read (%d) does not match wanted count (%d).\n", r, frames );
	}
	return VUL_OK;
}

static vul_audio_return vul__audio_write_alsa( vul_audio_device *dev, void *samples, u32 sample_count )
{
	u32 size = sample_count * vul__audio_bytes_per_sample( dev );
	u32 frames = sample_count / dev->channels;

	s32 r = alsa_write( dev->device.alsa.handle, samples, frames ); // @TODO(thynn): Do we want frame-count as an argument to all write functions??
	if( r == -EPIPE ) {
		// Reprepare before failing
		alsa_prepare( dev->device.alsa.handle );
		ERR( "ALSA write returned in a buffer underrun.\n" )
	}
	if( r < 0 ) {
		ERR( "ALSA write failed: %s.\n", alsa_strerror( r ) );
	}
	if( r != frames ) {
		ERR( "Frame count written (%d) does not match wanted count (%d).\n", r, frames );
	}
	return VUL_OK;
}

static vul_audio_return vul__audio_init_alsa( vul_audio_device *dev, const char *device_name )
{
	snd_pcm_hw_params_t *hwp;
	snd_pcm_sw_params_t *swp;
	snd_pcm_sframes_t frames_to_deliver;
	int devices, err;
	struct pollfd *device_fds;
	
	// Library loads
	if( ( dev->device.alsa.dlib = dlopen( "libasound.so", RTLD_NOW ) ) == NULL ) {
		ERR( "Failed to load ALSA library.\n" );
	}
	DLLOAD( alsa_prepare, dev->device.alsa.dlib, "snd_pcm_prepare" );
	DLLOAD( alsa_write, dev->device.alsa.dlib, "snd_pcm_writei" );
	DLLOAD( alsa_read, dev->device.alsa.dlib, "snd_pcm_readi" );
	DLLOAD( alsa_strerror, dev->device.alsa.dlib, "snd_strerror" );
	DLLOAD( alsa_open, dev->device.alsa.dlib, "snd_pcm_open" );
	DLLOAD( alsa_hw_malloc, dev->device.alsa.dlib, "snd_pcm_hw_params_malloc" );
	DLLOAD( alsa_hw_any, dev->device.alsa.dlib, "snd_pcm_hw_params_any" );
	DLLOAD( alsa_hw_set_access, dev->device.alsa.dlib, "snd_pcm_hw_params_set_access" );
	DLLOAD( alsa_hw_set_format, dev->device.alsa.dlib, "snd_pcm_hw_params_set_format" );
	DLLOAD( alsa_hw_set_rate_near, dev->device.alsa.dlib, "snd_pcm_hw_params_set_rate_near" );
	DLLOAD( alsa_hw_set_channels, dev->device.alsa.dlib, "snd_pcm_hw_params_set_channels" );
	DLLOAD( alsa_hw_params, dev->device.alsa.dlib, "snd_pcm_hw_params" );
	DLLOAD( alsa_hw_free, dev->device.alsa.dlib, "snd_pcm_hw_params_free" );
	DLLOAD( alsa_sw_malloc, dev->device.alsa.dlib, "snd_pcm_sw_params_malloc" );
	DLLOAD( alsa_sw_current, dev->device.alsa.dlib, "snd_pcm_sw_params_current" );
	DLLOAD( alsa_sw_set_avail_min, dev->device.alsa.dlib, "snd_pcm_sw_params_set_avail_min" );
	DLLOAD( alsa_sw_set_start_threshold, dev->device.alsa.dlib, "snd_pcm_sw_params_set_start_threshold" );
	DLLOAD( alsa_sw_params, dev->device.alsa.dlib, "snd_pcm_sw_params" );
	DLLOAD( alsa_sw_free, dev->device.alsa.dlib, "snd_pcm_sw_params_free" );
	DLLOAD( alsa_drain, dev->device.alsa.dlib, "snd_pcm_drain" );
	DLLOAD( alsa_close, dev->device.alsa.dlib, "snd_pcm_close" );
	// Hardware parameters

	// @TODO(thynn): Other modes than playback!
	if( ( err = alsa_open( &dev->device.alsa.handle, device_name, SND_PCM_STREAM_PLAYBACK, 0 ) ) < 0 ) {
		ERR( "Failed to open ALSA device %s.\n", device_name );
	}

	if( ( err = alsa_hw_malloc( &hwp ) ) < 0 ) {
		ERR( "Failed to allocate ALSA hardware parameters struct.\n" );
	}
	if( ( err = alsa_hw_any( dev->device.alsa.handle, hwp ) ) < 0 ) {
		ERR( "Failed to get initial ALSA hardware parameters.\n" );
	}

	// @TODO(thynn): Interleaved vs. other buffer/stream types!!
	if( ( err = alsa_hw_set_access( dev->device.alsa.handle, hwp, SND_PCM_ACCESS_RW_INTERLEAVED ) ) < 0 ) {
		ERR( "Failed to set ALSA access pattern.\n" );
	}

	int fmt;
	switch( dev->format ) {
	case VUL_AUDIO_SAMPLE_FORMAT_16BIT_NATIVE:
		fmt = SND_PCM_FORMAT_S16_LE;
		break;
	case VUL_AUDIO_SAMPLE_FORMAT_32BIT_NATIVE:
		fmt = SND_PCM_FORMAT_S32_LE;
		break;
	default:
		ERR( "Unknown sample format encountered.\n" );
	}
	if( ( err = alsa_hw_set_format( dev->device.alsa.handle, hwp, fmt ) ) < 0 ) {
		ERR( "Failed to set ALSA sample format.\n" );
	}
	if( ( err = alsa_hw_set_rate_near( dev->device.alsa.handle, hwp, &dev->sample_rate, 0 ) ) < 0 ) {
		ERR( "Failed to set ALSA sample rate.\n" );
	}
	if( ( err = alsa_hw_set_channels( dev->device.alsa.handle, hwp, dev->channels ) ) < 0 ) {
		ERR( "Failed to set ALSA channel count.\n" );
	}
	if( ( err = alsa_hw_params( dev->device.alsa.handle, hwp ) ) < 0 ) {
		ERR( "Failed to set final ALSA hardware parameters.\n" );
	}

	alsa_hw_free( hwp );

	// Software parameters
	// @TODO(thynn): To supply a unified architecture, can we avoid callbacks here (or make OSS use callbacks?)
	if( ( err = alsa_sw_malloc( &swp ) ) < 0 ) {
		ERR( "Failed to allocate ALSA software parameters struct.\n" );
	}
	if( ( err = alsa_sw_current( dev->device.alsa.handle, swp ) ) < 0 ) {
		ERR( "Failed to get current ALSA software parameters.\n" );
	}
	// @TODO(thynn): Different frame sizes!!
	if( ( err = alsa_sw_set_avail_min( dev->device.alsa.handle, swp, 4096 ) ) < 0 ) {
		ERR( "Failed to set ALSA frame size.\n" );
	}
	if( ( err = alsa_sw_set_start_threshold( dev->device.alsa.handle, swp, 0U ) ) < 0 ) {
		ERR( "Failed to set ALSA start threshold.\n" );
	}
	if( ( err = alsa_sw_params( dev->device.alsa.handle, swp ) ) < 0 ) {
		ERR( "Failed to set final ALSA software parameters.\n" );
	}
	// @TODO(thynn): Can we free this (swp)??

	// Prepare device
	if( ( err = alsa_prepare( dev->device.alsa.handle ) ) < 0 ) {
		ERR( "Failed to start ALSA device.\n" );
	}

	dev->lib = VUL__AUDIO_LINUX_ALSA;
	return VUL_OK;
}

// ---------------
// Pulse audio
//

pa_simple * ( *pulse_new )( const char *, const char *, pa_stream_direction_t, 
									 const char *, const char *, const pa_sample_spec*, 
									 const pa_channel_map*, const pa_buffer_attr*, int * ) = 0;
void ( *pulse_free )( pa_simple * ) = 0;
int ( *pulse_write )( pa_simple *, const void *, size_t, int * ) = 0;
int ( *pulse_read )( pa_simple *, void *, size_t, int * ) = 0;
int ( *pulse_drain )( pa_simple *, int * ) = 0;
const char* ( *pulse_error )( int ) = 0;

// @TODO(thynn): Move from simple API to asynch API? https://freedesktop.org/software/pulseaudio/doxygen/async.html
vul_audio_return vul__audio_init_pulse( vul_audio_device *dev, const char *name, const char *description )
{
	// Library loads
	if( ( dev->device.pulse.dlib = dlopen( "libpulse.so", RTLD_NOW ) ) == NULL ) {
		ERR( "Failed to load PulseAudio library.\n" );
	}
	if( ( dev->device.pulse.dlib_simple = dlopen( "libpulse-simple.so", RTLD_NOW ) ) == NULL ) {
		ERR( "Failed to load PulseAudio Simple API library.\n" );
	}
	DLLOAD( pulse_new, dev->device.pulse.dlib_simple, "pa_simple_new" );
	DLLOAD( pulse_free, dev->device.pulse.dlib_simple, "pa_simple_free" );
	DLLOAD( pulse_read, dev->device.pulse.dlib_simple, "pa_simple_read" );
	DLLOAD( pulse_write, dev->device.pulse.dlib_simple, "pa_simple_write" );
	DLLOAD( pulse_drain, dev->device.pulse.dlib_simple, "pa_simple_drain" );
	DLLOAD( pulse_error, dev->device.pulse.dlib, "pa_strerror" );

	pa_sample_spec ss;
	switch( dev->format ) {
	case VUL_AUDIO_SAMPLE_FORMAT_16BIT_NATIVE:
		ss.format = PA_SAMPLE_S16NE;
		break;
	case VUL_AUDIO_SAMPLE_FORMAT_32BIT_NATIVE:
		ss.format = PA_SAMPLE_S32NE;
		break;
	default:
		ERR( "Unkown sample format encountered.\n" );
	}
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
		assert( 0 && "Not supported yet" );
		break;
	default:
		ERR( "Unkown device mode encountered.\n" );
	}
	
	dev->device.pulse.client = pulse_new( NULL, // Default server, (@TODO(thynn): Make an argument?)
													  name,
													  dir,
													  NULL, // Default device (@TODO(thynn): Make an argument?)
													  description,
													  &ss,
													  NULL, // Default channel map, 
													  NULL, // Default buffering attributes
													  NULL ); // Ignore error code
	dev->lib = VUL__AUDIO_LINUX_PULSE;
	return VUL_OK;
}

vul_audio_return vul__audio_read_pulse( vul_audio_device *dev, void *samples, u32 sample_count )
{
	u32 size = sample_count * vul__audio_bytes_per_sample( dev );
	s32 err;
	if( pulse_read( dev->device.pulse.client,
						 samples,
						 size,
						 &err ) < 0 ) {
		ERR( "Failed to read samples to PulseAudio: %s.\n", pulse_error( err ) );
	}
	return VUL_OK;
}

vul_audio_return vul__audio_write_pulse( vul_audio_device *dev, void *samples, u32 sample_count )
{
	// @TODO(thynn): Make "flush first" a parameter? "Flush if latence over N ms" a parameter?
	u32 size = sample_count * vul__audio_bytes_per_sample( dev );
	s32 err;
	if( pulse_write( dev->device.pulse.client,
						  samples,
						  size,
						  &err ) < 0 ) {
		ERR( "Failed to write samples to PulseAudio: %s.\n", pulse_error( err ) );
	}
	return VUL_OK;
}

// --------------
// Public API
//

vul_audio_return vul_audio_write( vul_audio_device *dev, void *samples, u32 sample_count )
{
	if( !( dev->mode == VUL_AUDIO_MODE_PLAYBACK || dev->mode == VUL_AUDIO_MODE_DUPLEX ) ) {
		ERR( "Device write requested while not in playback or duplex mode.\n" );
	}

	switch( dev->lib ) {
	case VUL__AUDIO_LINUX_OSS:
		return vul__audio_write_oss( dev, samples, sample_count );
	case VUL__AUDIO_LINUX_ALSA:
		return vul__audio_write_alsa( dev, samples, sample_count );
	case VUL__AUDIO_LINUX_PULSE:
		return vul__audio_write_pulse( dev, samples, sample_count );
	default:
		ERR( "Unknown device library in use.\n" );
	}
	return VUL_OK;
}

vul_audio_return vul_audio_read( vul_audio_device *dev, void *samples, u32 sample_count )
{
	if( !( dev->mode == VUL_AUDIO_MODE_RECORDING || dev->mode == VUL_AUDIO_MODE_DUPLEX ) ) {
		ERR( "Device read requested while not in recording or duplex mode.\n" );
	}

	switch( dev->lib ) {
	case VUL__AUDIO_LINUX_OSS:
		assert( 0 && "Not implemented yet." );
	case VUL__AUDIO_LINUX_ALSA:
		return vul__audio_read_alsa( dev, samples, sample_count );
	case VUL__AUDIO_LINUX_PULSE:
		return vul__audio_read_pulse( dev, samples, sample_count );
	default:
		ERR( "Unknown device library in use.\n" );
	}
	return VUL_OK;
}

vul_audio_return vul_audio_destroy( vul_audio_device *dev, int drain_before_close )
{
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
		ERR( "Unknown device library in use.\n" );
	}
}

vul_audio_return vul_audio_init( vul_audio_device *out, 
											const char *device_name,  
											vul_audio_mode mode,
											u32 channels, 
											u32 sample_rate,
											vul_audio_sample_format fmt )
{
	assert( out );
	memset( out, 0, sizeof( vul_audio_device ) );

	out->channels = channels;
	out->sample_rate = sample_rate;
	out->format = fmt;
	out->mode = mode;

	// Try pulse
	if( VUL_OK == vul__audio_init_pulse( out, "vul_audio", "@TODO(thynn): This and name should be parameters!" ) ) {
		return VUL_OK;
	}

	// Try ALSA
	if( VUL_OK == vul__audio_init_alsa( out, device_name ) ) {
		return VUL_OK;
	}

	// Try OSS
	// @TODO(thynn): Other modes based on out->mode!
	int fdmode = O_WRONLY;
	if( VUL_OK == vul__audio_init_oss( out, fdmode ) ) {
		return VUL_OK;
	}

	ERR( "Failed to open audio device with any of the attempted libraries.\n" );
}

#else
	You must define an operating system (VUL_WINDOWS, VUL_LINUX, VUL_OSX)
#endif

#undef ERR

#ifdef _cplusplus
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
#undef u64
#undef f32
#undef f64
#endif // VUL_TYPES_H
