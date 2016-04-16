/*
 * Villains' Utility Library - Thomas Martin Schmid, 2016. Public domain?
 *
 * This file contains a low-level wrapper for different audio APIs.
 * The goal is to support (in order of attempts/fallback):
 *  - Linux: pulseaudio -> jack -> alsa -> oss
 *  - OSX: TODO
 *  - Windows: TODO
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
#if defined( VUL_WINDOWS )
#pragma comment(lib, "Soemthing.lib")
#elif defined( VUL_LINUX )
#include <stdio.h>
#include <poll.h>
#include <alsa/asoundlib.h>
#include <jack.h>
#elif defined( VUL_OSX )
	BAH
#endif

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef float f32;
typedef double f64;

typedef enum vul__audio_lib {
	VUL__AUDIO_LINUX_ALSA,
	VUL__AUDIO_LINUX_PULSE,
	VUL__AUDIO_LINUX_JACK,
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

	vul__audio_lib lib;
	union {
		FILE *oss_device_fd;
		snd_pcm_t alsa_handle;
		struct jack {
			jack_client_t *client;
			jack_port_t *input_port;
			jack_port_t *output_port;
		} jack;
	} device;
} vul_audio_device;

#ifdef _cplusplus
extern "C" {
#endif

// TODO(thynn): Declare API

#ifdef _cplusplus
}
#endif
#endif

#ifdef VUL_DEFINE

#ifdef _cplusplus
extern "C" {
#endif

// ------
// OSS
// 
static vul_audio_return vul__audio_init_oss( vul_audio_device *dev, const char *device_name, int mode )
{
	s32 tmp, fd;
	if( ( fd = open( device_name, mode, 0 ) ) == -1 ) {
		return VUL_ERROR;
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
		return VUL_ERROR;
	}
	if( ioctl( fd, SNDCTL_DSP_SETFMT, &tmp ) == -1 ) {
		close( fd );
		return VUL_ERROR;
	}
	switch( dev->format ) {
	case VUL_AUDIO_SAMPLE_FORMAT_16BIT_NATIVE: if( tmp != AFMT_S16_NE ) return VUL_ERROR;
	case VUL_AUDIO_SAMPLE_FORMAT_32BIT_NATIVE: if( tmp != AFMT_S32_NE ) return VUL_ERROR;
	default:
		return VUL_ERROR;
	}

	// Set channel count
	tmp = dev->channels;
	if( ioctl( fd, SNDCTL_DSP_CHANNELS, &tmp ) == -1 ) {
		return VUL_ERROR;
	}
	if( tmp != dev->channels ) {
		return VUL_ERROR;
	}

	// Set sample rate
	tmp = dev->sample_rate;
	if( ioctl( fd, SNDCTL_DSP_SPEED, &tmp ) == -1 ) {
		return VUL_ERROR;
	}
	if( tmp != dev->sample_rate ) {
		return VUL_ERROR;
	}

	// Store file descriptor
	dev->lib.oss_device_fd = fd;
	dev->lib = VUL__AUDIO_LINUX_OSS;

	return VUL_OK;
}

static vul_audio_return vul__audio_write_oss( vul_audio_device *dev, void *samples, u32 sample_count )
{
	u32 sample_size = 2;
	switch( dev->format ) {
	case VUL_AUDIO_SAMPLE_FORMAT_16BIT_NATIVE:
		sample_size = 2;
		break;
	case VUL_AUDIO_SAMPLE_FORMAT_32BIT_NATIVE:
		sample_size = 4;
		break;
	}
	u32 size = sample_count * sample_size;
	if( write( dev->lib.oss_device_fd, samples, size ) != size ) {
		return VUL_ERROR;
	}
	return VUL_OK;
}

// ------
// ALSA
//

static vul_audio_return vul__audio_init_alsa( vul_audio_device *dev, const char *device_name )
{
	snd_pcm_hw_params_t *hwp;
	snd_pcm_sw_params_t *swp;
	snd_pdm_sframes_t frames_to_deliver;
	int devices, err;
	struct pollfd *device_fds;

	// Hardware parameters

	// @TODO(thynn): Other modes than playback!
	if( ( err = snd_pcm_open( &dev->lib.alsa_handle, device_name, SND_PCM_STREAM_PLAYBACK, 0 ) ) < 0 ) {
		return VUL_ERROR;
	}

	if( ( err = snd_pcm_hw_params_malloc( &hwp ) ) < 0 ) {
		return VUL_ERROR;
	}
	if( ( err = snd_pcm_hw_params_any( dev->lib.alsa_handle, hwp ) ) < 0 ) {
		return VUL_ERROR;
	}

	// @TODO(thynn): Interleaved vs. other buffer/stream types!!
	if( ( err = snd_pcm_hw_params_set_access( dev->lib.alsa_handle, hwp, SND_PCM_ACCESS_RW_INTERLEAVED ) ) < 0 ) {
		return VUL_ERROR;
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
		return VUL_ERROR;
	}
	if( ( err = snd_pcm_hw_params_set_format( dev->lib.alsa_handle, hwp, fmt ) ) < 0 ) {
		return VUL_ERROR;
	}
	if( ( err = snd_pcm_hw_params_set_rate_near( dev->lib.alsa_handle, hwp, dev->sample_rate, 0 ) ) < 0 ) {
		return VUL_ERROR;
	}
	if( ( err = snd_pcm_hw_params_set_channels( dev->lib.alsa_handle, hwp, dev->channels ) ) < 0 ) {
		return VUL_ERROR;
	}
	if( ( err = snd_pcm_hw_params( dev->lib.alsa_handle, hwp ) ) < 0 ) {
		return VUL_ERROR;
	}

	snd_pcm_he_params_free( hwp );

	// Software parameters
	// @TODO(thynn): To supply a unified architecture, can we avoid callbacks here (or make OSS use callbacks?)
	if( ( err = snd_pcm_sw_params_malloc( &swp ) ) < 0 ) {
		return VUL_ERROR;
	}
	if( ( err = snd_pcm_sw_params_current( dev->lib.alsa_handle, swp ) ) < 0 ) {
		return VUL_ERROR;
	}
	// @TODO(thynn): Different frame sizes!!
	if( ( err = snd_pcm_sw_params_set_avail_min( dev->lib.alsa_handle, swp, 4096 ) ) < 0 ) {
		return VUL_ERROR;
	}
	if( ( err = snd_pcm_sw_params_set_start_threshold( dev->lib.alsa_handle, swp, 0U ) ) < 0 ) {
		return VUL_ERROR;
	}
	if( ( err = snd_pcm_sw_params( dev->lib.alsa_handle, swp ) ) < 0 ) {
		return VUL_ERROR;
	}
	// @TODO(thynn): Can we free this (swp)??
	
	// Prepare device
	if( ( err = snd_pcm_prepare( dev->lib.alsa_handle ) ) < 0 ) {
		return VUL_ERROR;
	}

	dev->lib = VUL__AUDIO_LINUX_ALSA;
}

// -------
// JACK
//

static int vul__audio_callback_jack( jack_nframes_t nframes, void *arg )
{
	// @TODO(thynn): If we're doing callback-based APIs I think we store dev + buffer as static vars
	// and access them from here; possibly add a mutex so we wait until they're ready (double buffer?)
	// API then becomes vul_audio_write [writes to static buffer] - callback reads from buffer.
	// Do we then block on the audio write? (probably bad!) until the read is done?

	jack_default_audio_sample_t *out = ( jack_default_audio_sample_t* )jack_port_get_buffer( output_port, nframes );
	jack_default_audio_sample_t *in = ( jack_default_audio_sample_t* )jack_port_get_buffer( input_port, nframes );

	memcpy( out, in, sizeof( jack_default_audio_sample_t ) * nframes );
}

static void vul__audio_shutdown_jack( void *arg )
{
	// @TODO(thynn): Signal a panic!
	assert( 0 && "The JACK server died!" );
}

static vul_audio_return vul__audio_init_jack( vul_audio_device *dev, const char *name )
{
	jack_status_t status;
	const char **ports;
	jack_option_t opts = 0;
	
	if( strlen( name ) > jack_client_name_size( ) ) {
		return VUL_ERROR;
	}
	if( ( dev->lib.jack.client = jack_client_open( name, opts, &status ) ) == NULL ) {
		return VUL_ERROR;
	}

	jack_set_process_callback( dev->lib.jack.client, vul__audio_callback_jack );

	jack_on_shutdown( dev->lib.jack.client, vul__audio_shutdown_jack );

	// @TODO(thynn): Setting sample rate (how?)
	if( dev->sample_rate != jack_get_sample_rate( dev->lib.jack.client ) ) {
		return VUL_ERROR;
	}

	if( dev->mode == VUL_AUDIO_MODE_PLAYBACK || dev->mode == VUL_AUDIO_MODE_DUPLEX ) {
		dev->lib.jack.output_port = jack_port_register( dev->lib.jack.client, "output", JACK_DEFAULT_AUDIO_TYPE, JackPortisOutput, 0 );
	}
	if( dev->mode == VUL_AUDIO_MODE_RECORDING || dev->mode == VUL_AUDIO_MODE_DUPLEX ) {
		dev->lib.jack.input_port = jack_port_register( dev->lib.jack.client, "input", JACK_DEFAULT_AUDIO_TYPE, JackPortisInput, 0 );
	}

	if( jack_activate( dev->lib.jack.client ) ) {
		return VUL_ERROR;
	}

	if( ( ports = jack_get_ports( dev->lib.jack.client, NULL, NULL, 
											JackPortIsPhysical |JackPortIsInput ) ) == NULL ) {
		return VUL_ERROR;
	}
	if( jack_connect( dev->lib.jack.client, ports[ 0 ], jack_port_name( input_port ) ) ) {
		return VUL_ERROR;
	}
	free( ports );
	if( ( ports = jack_get_ports( dev->lib.jack.client, NULL, NULL, 
											JackPortIsPhysical |JackPortIsOutput ) ) == NULL ) {
		return VUL_ERROR;
	}
	if( jack_connect( dev->lib.jack.client, jack_port_name( output_port ), ports[ 0 ] ) ) {
		return VUL_ERROR;
	}
	free( ports );

	dev->lib = VUL__AUDIO_LINUX_JACK;
	return VUL_OK;
}

// ---------------
// Pulse audio
//
// @TODO(thynn): See freedesktop.org/software/pulseaudio/doxygen/simple.html (probably don't want simple...)

// --------------
// Public API
//

vul_audio_return vul_audio_write( vul_audio_device *dev, void *samples, u32 sample_count )
{
	switch( dev->lib ) {
	case VUL__AUDIO_LINUX_OSS:
		return vul__audio_write_oss( dev, samples, sample_count );
	case VUL__AUDIO_LINUX_ALSA:
	case VUL__AUDIO_LINUX_JACK:
	case VUL__AUDIO_LINUX_PULSE:
		break;
	default:
		return VUL_ERROR;
	}
	return VUL_OK;
}

vul_audio_return vul_audio_destroy( vul_audio_device *dev )
{
	switch( dev->lib ) {
	case VUL__AUDIO_LINUX_ALSA:
		snd_pcm_close( dev->lib.alsa_handle );
		break;
	case VUL__AUDIO_LINUX_OSS:
		close( dev->lib.oss_device_fd );
		break;
	case VUL__AUDIO_LINUX_JACK:
		jack_client_close( dev->lib.jack.client );
		break;
	case VUL__AUDIO_LINUX_PULSE:
		// @TODO(thynn): This
		break;
	default:
		return VUL_ERROR;
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

	// Try ALSA
	if( VUL_OK == vul__audio_init_alsa( out, device_name ) ) {
		return VUL_OK;
	}

	// Try OSS
	// @TODO(thynn): Other modes based on out->mode!
	int fdmode = O_WRONLY;
	if( VUL_OK == vul__audio_init_oss( out, device_name, fdmode ) ) {
		return VUL_OK;
	}

	return VUL_ERROR;
}

#ifdef _cplusplus
}
#endif

#endif
