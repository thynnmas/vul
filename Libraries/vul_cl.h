/*
 * Villains' Utility Library - Thomas Martin Schmid, 2017. Public domain?
 *
 * This file contains utility functions to make interaction with OpenCL less
 * of a massive pain.
 * @TODO: Add a define to make eveyrthing static to avoid global state.
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
#ifndef VUL_CL_H
#define VUL_CL_H

#include <stdio.h>
#include <assert.h>
#ifdef VUL_OSX
#include <stdlib.h>
#else
#include <malloc.h>
#endif
#include <string.h>

#include "vul_stable_array.h"

/**
 * This must be included prior to this file, but may have to be included in a
 * particular order to enable OpenCL/GL interop, so we leave it to the user!
 */
//#include <CL/opencl.h>

/**
 * The two types of kernel source:
 * -VUL__CL_KERNEL_SOURCE is normal OpenCL source.
 * -VUL__CL_KERNEL_BINARY is assembly of the type that is output by
 *  clGetProgramInfo( id, CL_PROGRAM_BINARIES, ... ), which allows us to
 *  make assembly corrections/optimizations.
 */
typedef enum {
	VUL__CL_KERNEL_SOURCE,
	VUL__CL_KERNEL_BINARY
} vul__cl_kernel_type;

/**
 * The two types of buffers; those mirrored in host memory,
 * and those shared with an OpenGL context.
 */
typedef enum {
	VUL__CL_BUFFER_HOST,
	VUL__CL_BUFFER_OPENGL
} vul__cl_buffer_type;

/**
 * Our OpenCL context and a list of devices. This will be a single global
 * vul__cl_context.
 */
typedef struct vul__cl_context {
	cl_platform_id platform;
	cl_context context;

	cl_uint device_count;
	cl_device_id *device_list;
	cl_command_queue *queue_list;	
	
	vul_svector *programs; // List of all programs built in this context
	vul_svector *buffers;  // List of all buffers allocated in this context
} vul__cl_context;

static size_t vul__cl_context_count = 0;
static vul__cl_context **vul__cl_contexts = NULL;

/**
 * The struct of a cl program to keep track of loaded ones.
 */
typedef struct vul_cl_program {
	cl_program program;
	vul__cl_context *context;
	vul__cl_kernel_type type;	// Type of source ( binary or cl source )
	union {
		char *source;			// The full source if of source type
		unsigned char *binary;			// The full binary if of binary type
	};
	vul_svector *kernels;		// List of all kernels belonging to this program
} vul_cl_program;

/**
 * The struct of a cl kernel. Contains a reference to the program and the entry point.
 */
typedef struct vul_cl_kernel {
	cl_kernel kernel;
	vul_cl_program *program;
	char *entry_point;
	vul_svector *arguments;
} vul_cl_kernel;

/**
 * The struct of a cl kernel argument. Contains size of argument, 
 * and a pointer to the arguemnt.
 */
typedef struct vul_cl_kernel_argument {
	size_t size;
	const void *content;
} vul_cl_kernel_argument;

/**
 * The struct of a cl buffer. Contains a type ( pure buffer or GL/CL shared buffer )
 * the cl_mem of the buffer in the CL context and either a pointer to the host memeory,
 * or the GLuint of the buffer in the GL context.
 */
typedef struct vul_cl_buffer {
	vul__cl_context *context;
	cl_mem buffer;
	vul__cl_buffer_type type;
	union {
		void *host_ptr;
		unsigned int gl_buffer;
	};
	size_t size; // Only used for Host buffers
	cl_mem_flags flags;
} vul_cl_buffer;

#ifdef __cplusplus
extern "C" {
#endif
/**
 * Returns a string describing the given error code
 */
const char *vul_cl_get_error_string( cl_int err );
/**
 * Creates the OpenCL context, sets up all available devices and
 * creates command queues for them all.
 * If errors are encountered, everything that needs cleaning up
 * is cleaned up before returning!
 * MUST be called before any other functions in this file are used,
 * and vul_cl_cleanup() MUST be called before the program exits.
 * 
 * Defaults parameters are ( NULL, 0 )
 */
void vul_cl_setup( cl_context_properties *context_properties, cl_command_queue_properties command_queue_properites );
/**
 * Cleans up the contexts set up in vul_cl_setup and
 * all programs and kernels loaded in vul_cl_create_program/_kernel.
 */
void vul_cl_cleanup( );
/**
 * Write the output from a compile to the given output stream.
 */
void vul_cl_write_compile_output( vul_cl_program *prog, FILE *out );
/**
 * Loads a program. Requires vul_cl_setup to have been called prior.
 * platform_id indicates which platform to create & build the program on.
 * If no build options are wanted, set to an empty string.
 * If is_binary is non-zero, the source is assumed to be in binary format.
 * If async_build_callback is not NULL, the build does not wait until completions,
 * and the callback is called once it is done. See http://www.khronos.org/registry/cl/sdk/1.0/docs/man/xhtml/clBuildProgram.html
 * user_data is passed to async_build_callback. Set to NULL to ignore.
 * Do NOT manually clean this up, it is cleaned up with vul_cl_cleanup!
 */
vul_cl_program *vul_cl_create_program( cl_platform_id platform_id, 
													char *file_path, 
													const char *build_options, 
													int is_binary,
													void ( *async_build_callback )( cl_program, void* ), 
													void *user_data );
/**
 * Creates a new kernel in the given program with the given entry point name.
 * Copies the entry point name!
 * Do NOT manually clean this up, it is cleaned up with vul_cl_cleanup!
 */
vul_cl_kernel *vul_cl_create_kernel( vul_cl_program *program, const char *entry_point );
/**
 * Adds an argument to a kernel.
 */
void vul_cl_kernel_add_argument( vul_cl_kernel *kernel, size_t data_size, const void *data_ptr );
/**
 * Allocates a new buffer on the given OpenCL platform from data in the host's memory.
 * host_ptr should point to a buffer of equal or greater size than the given size parameter
 * indicates, but may or may not already be allocated. It must however be allocated before
 * a read or write is attempted!
 * For flags, see http://www.khronos.org/registry/cl/sdk/1.0/docs/man/xhtml/clCreateBuffer.html
 */
vul_cl_buffer *vul_cl_create_buffer( cl_platform_id platform_id, cl_mem_flags flags, size_t size, void *host_ptr );
/**
 * Allocates a new buffer on the given OpenCL platform from data in the host's memory.
 * gl_buffer is the OpenGL identifier of the shared buffer. If must already have been created
 * with a call to glBufferData, but needs not be initialized prior to this call.
 * For flags, see http://www.khronos.org/registry/cl/sdk/1.0/docs/man/xhtml/clCreateBuffer.html
 */
vul_cl_buffer *vul_cl_create_gl_buffer( cl_platform_id platform_id, cl_mem_flags flags, unsigned int gl_buffer );
/**
 * Writes data to the given buffer.
 * queue_index is the index of the desired command_queue in the buffer's context.
 * If no host_ptr is set, the one in buffer is used, with
 * buffer->size as size, and 0 offset.
 * wait_event is the reference object to this event; it's the one to wait 
 * for when waiting for this write is desired.
 * Returns 0 if successful, !0 otherwise.
 */
int vul_cl_write_buffer( size_t device_index, 
								 vul_cl_buffer *buffer, 
								 void *host_ptr, 
								 size_t offset, 
								 size_t size_of_write, 
								 int blocking_write, // Acts as bool
								 cl_uint event_wait_list_size,
								 cl_event *event_wait_list, 
								 cl_event *wait_event );
/**
 * Copies data between the given buffers.
 * queue_index is the index of the desired command_queue in the buffer's context.
 * If no size_of_copy is set, the smaller of the two buffer's size is used.
 * wait_event is the reference object to this event; it's the one to wait 
 * for when waiting for this copy is desired.
 * Returns 0 if successful, !0 otherwise.
 */
int vul_cl_copy_buffer( size_t queue_index, 
								vul_cl_buffer *src_buffer, 
								vul_cl_buffer *dst_buffer, 
								size_t src_offset,
								size_t dst_offset, 
								size_t size_of_copy, 
								cl_uint event_wait_list_size, 
								cl_event *event_wait_list, 
								cl_event *wait_event );
/**
 * Reads data from the given buffer.
 * queue_index is the index of the desired command_queue in the buffer's context.
 * If no size_of_read is set, entire buffer's size is used, which means host_ptr
 * must point to a buffer of minimum size of either size_to_read or buffer->size.
 * wait_event is the reference object to this event; it's the one to wait 
 * for when waiting for this read is desired.
 * Returns 0 if successful, !0 otherwise.
 */
int vul_cl_read_buffer( size_t device_index, 
								vul_cl_buffer *buffer, 
								void *host_ptr, 
								size_t offset,
								size_t size_of_read, 
								int blocking_read, // Acts as bool
								cl_uint event_wait_list_size,
								cl_event *event_wait_list, 
								cl_event *wait_event );
/** 
 * Resizes the given buffer.
 */
int vul_cl_resize_buffer( vul_cl_buffer *buffer, size_t new_size, void *host_ptr );
/**
 * Uploads arguments, determines work group size and executes
 * the given kernel. The device to execute it on is determined
 * by the given device index, and index into the device_lsit of the 
 * context in which the program this kernel belongs to was created.
 * global_size is the maximum work group size, i.e. the number of
 * instances of this kernel to spawn. 
 * wait_event is the reference object to this event; it's the one to wait 
 * for when waiting for this execution is desired.
 * @NOTE: This function only handles 1-dimensional work groups
 */
cl_int vul_cl_call_kernel( vul_cl_kernel *kernel, 
									size_t device_index, 
									size_t global_size, 
									cl_uint event_wait_list_size, 
									cl_event *event_wait_list, 
									cl_event *wait_event );
/**
 * Syncs the command queue for the device of given index in the kernel's context.
 */
cl_int vul_cl_sync( vul_cl_kernel *kernel, size_t device_index );
/**
 * Retrieves the first platform found that matches the vendor string given.
 * \return cl_platform_id of the platform, NULL if none found.
 */
cl_platform_id vul_cl_get_platform_by_vendor_string( const char *vendor_string );
/**
 * Retrieves the cl_platform_id of the context at the given index.
 */
cl_platform_id vul_cl_get_platform_by_context_index( size_t index );
/**
 * Prints the vendor string for all platforms found
 */
void vul_cl_print_platform_vendor_strings( );
/**
 * Prints all platforms found
 */
void vul_cl_print_platform_info( );

#ifdef __cplusplus
}
#endif
#endif

#ifdef VUL_DEFINE

#define vul_cl_check( err ) {\
	const char *estr;\
	if( err != CL_SUCCESS ) {\
		estr = vul_cl_get_error_string( err );\
		fprintf( stderr, "[%s:%d] OpenCL error: '%s' (%d).\n", __FILE__, __LINE__, estr, err );\
		assert(0);\
	}\
}
// @TODO(thynn): Make sure we don't assert in release builds...

#ifdef __cplusplus
extern "C" {
#endif
	
const char *vul_cl_get_error_string( cl_int err )
{
	const char *str = "Unknown error";
	switch( err ) {
		case CL_SUCCESS:												str = "Success";									break;
		case CL_DEVICE_NOT_FOUND:									str = "Device not found";						break;
		case CL_DEVICE_NOT_AVAILABLE:								str = "Device not available";					break;
		case CL_COMPILER_NOT_AVAILABLE:							str = "Compiler not available";				break;
		case CL_MEM_OBJECT_ALLOCATION_FAILURE:					str = "Mem object allocation failure";		break;
		case CL_OUT_OF_RESOURCES:									str = "Out of resources";						break;
		case CL_OUT_OF_HOST_MEMORY:								str = "Out of host memory";					break;
		case CL_PROFILING_INFO_NOT_AVAILABLE:					str = "Profiling info not available";		break;
		case CL_MEM_COPY_OVERLAP:									str = "Memcopy overlaps";						break;
		case CL_IMAGE_FORMAT_MISMATCH:							str = "Image format mismatch";				break;
		case CL_IMAGE_FORMAT_NOT_SUPPORTED:						str = "Image format not supported";			break;
		case CL_BUILD_PROGRAM_FAILURE:							str = "Build program failure";				break;
		case CL_MAP_FAILURE:											str = "Map failure";								break;
		case CL_MISALIGNED_SUB_BUFFER_OFFSET:					str = "Misaligned sub buffer offset";		break;
		case CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST: str = "Exec status error for wait list event";	break;
	
		case CL_INVALID_VALUE:										str = "Invalid value";							break;
		case CL_INVALID_DEVICE_TYPE:								str = "Invalid device type";					break;
		case CL_INVALID_PLATFORM:									str = "Invalid platform";						break;
		case CL_INVALID_DEVICE:										str = "Invalid device";							break;
		case CL_INVALID_CONTEXT:									str = "Invalid context";						break;
		case CL_INVALID_QUEUE_PROPERTIES:						str = "Invalid queue properties";			break;
		case CL_INVALID_COMMAND_QUEUE:							str = "Invalid command queue";				break;
		case CL_INVALID_HOST_PTR:									str = "Invalid host pointer";					break;
		case CL_INVALID_MEM_OBJECT:								str = "Invalid mem object";					break;
		case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:				str = "Invalid image format descriptor";	break;
		case CL_INVALID_IMAGE_SIZE:								str = "Invalid image size";					break;
		case CL_INVALID_SAMPLER:									str = "Invalid sampler";						break;
		case CL_INVALID_BINARY:										str = "Invalid binary";							break;
		case CL_INVALID_BUILD_OPTIONS:							str = "Invalid build options";				break;
		case CL_INVALID_PROGRAM:									str = "Invalid program";						break;
		case CL_INVALID_PROGRAM_EXECUTABLE:						str = "Invalid program executable";			break;
		case CL_INVALID_KERNEL_NAME:								str = "Invalid kernel name";					break;
		case CL_INVALID_KERNEL_DEFINITION:						str = "Invalid kernel definition";			break;
		case CL_INVALID_KERNEL:										str = "Invalid kernel";							break;
		case CL_INVALID_ARG_INDEX:									str = "Invalid argument index";				break;
		case CL_INVALID_ARG_VALUE:									str = "Invalid argument value";				break;
		case CL_INVALID_ARG_SIZE:									str = "Invalid argument size";				break;
		case CL_INVALID_KERNEL_ARGS:								str = "Invalid kernel arguments";			break;
		case CL_INVALID_WORK_DIMENSION:							str = "Invalid work dimenstion";				break;
		case CL_INVALID_WORK_GROUP_SIZE:							str = "Invalid work group size";				break;
		case CL_INVALID_WORK_ITEM_SIZE:							str = "Invalid work item size";				break;
		case CL_INVALID_GLOBAL_OFFSET:							str = "Invalid global offset";				break;
		case CL_INVALID_EVENT_WAIT_LIST:							str = "Invalid event wait list";				break;
		case CL_INVALID_EVENT:										str = "Invalid event";							break;
		case CL_INVALID_OPERATION:									str = "Invalid operation";						break;
		case CL_INVALID_GL_OBJECT:									str = "Invalid OpenGL object";				break;
		case CL_INVALID_BUFFER_SIZE:								str = "Invalid buffer size";					break;
		case CL_INVALID_MIP_LEVEL:									str = "Invalid MIP level";						break;
		case CL_INVALID_GLOBAL_WORK_SIZE:						str = "Invalid global work size";			break;
		case CL_INVALID_PROPERTY:									str = "Invalid property";						break;
	}
	return str;
}

void vul_cl_setup( cl_context_properties *context_properties, cl_command_queue_properties command_queue_properites )
{
	cl_int err;
	cl_platform_id *platforms;
	cl_uint platform_count, i, j;
	vul__cl_context *context; // A pointer to the current working context of vul__cl_contexts
	
	// Check that the context array is not already initialized
	assert( vul__cl_contexts == NULL );

	// Get platforms
	platforms = NULL;
	err = clGetPlatformIDs( 0, NULL, &platform_count );
	if( err != CL_SUCCESS || platform_count == 0 ) {
		return;
	}
	platforms = ( cl_platform_id* )malloc( sizeof( cl_platform_id ) * platform_count );
	assert( platforms );
	err = clGetPlatformIDs( platform_count, platforms, NULL );
	if ( err != CL_SUCCESS ) {
		free( platforms );
		return;
	}

	// Allocate context array
	vul__cl_context_count = platform_count;
	vul__cl_contexts = ( vul__cl_context** )malloc( sizeof( vul__cl_context* ) * platform_count );
	assert( vul__cl_contexts );

	// Now, for every platform
	for( i = 0; i < platform_count; ++i ) {
		// Create the context
		vul__cl_contexts[ i ] = ( vul__cl_context* )malloc( sizeof( vul__cl_context ) );
		context = vul__cl_contexts[ i ];
		assert( context );

		// Store platform id
		context->platform = platforms[ i ];

		// Get devices for context
		context->device_list = NULL;
		err = clGetDeviceIDs( context->platform, CL_DEVICE_TYPE_ALL, 0, NULL, &context->device_count );
		if ( err != CL_SUCCESS ) {
			// We failed loading devices for this platform, set context to NULL
			free( context );
			vul__cl_contexts[ i ] = NULL;
			// Continue to next platform
			continue;
		}
		context->device_list = ( cl_device_id* )malloc( sizeof( cl_device_id ) * context->device_count );
		assert( context->device_list );
		err = clGetDeviceIDs( context->platform, CL_DEVICE_TYPE_ALL, context->device_count, context->device_list, NULL );
		if ( err != CL_SUCCESS ) {
			// We failed loading devices for this platform, set context to NULL
			free( context->device_list );
			free( context );
			vul__cl_contexts[ i ] = NULL;
			// Continue to next platform
			continue;
		}

		// Create the context
		context->context = clCreateContext( context_properties,
											context->device_count,
											context->device_list,
											NULL, /* @TODO: Callback function that logs errors for us, see http://www.khronos.org/registry/cl/sdk/1.0/docs/man/xhtml/clCreateContext.html */
											NULL, /* User data to go with the above, probably just a pointer to the current context */
											&err );
		if( !context->context || err != CL_SUCCESS ) {
			// We failed creating the context for this platform, set context to NULL
			free( context->device_list );
			free( context );
			vul__cl_contexts[ i ] = NULL;
			// Continue to next platform
			continue;
		}

		// Create a command queue for each device
		context->queue_list = ( cl_command_queue* )malloc( sizeof( cl_command_queue ) * context->device_count );
		assert( context->queue_list );
		for( j = 0; j < context->device_count; ++j ) {
			context->queue_list[ j ] = clCreateCommandQueue( context->context, context->device_list[ j ], command_queue_properites, &err );
			if ( !context->queue_list[ j ] || err != CL_SUCCESS ) {
				context->queue_list[ j ] = NULL; // Set it to NULL and continue
			}
		}
		
		// Create program list
		context->programs = vul_svector_create( sizeof( vul_cl_program ), 8, malloc, free );
		
		// Create buffer list
		context->buffers = vul_svector_create( sizeof( vul_cl_buffer ), 8, malloc, free );
	}

	free( platforms );
}

void vul_cl_cleanup( )
{
	size_t i, k, l, size, size2;
	cl_uint j;
	vul__cl_context *context;

	assert( vul__cl_contexts );
	assert( vul__cl_context_count );

	for( i = 0; i < vul__cl_context_count ; ++i ) {
		context = vul__cl_contexts[ i ];
		if( context == NULL ) {
			continue;
		}

		for( j = 0; j < context->device_count; ++j ) {
			if( context->queue_list[ j ] != 0 ) {
			clReleaseCommandQueue( context->queue_list[ j ] );
			context->queue_list[ j ] = 0;
			}
		}
		if( context->queue_list != NULL ) {
			free( context->queue_list );
			context->queue_list = NULL;
		}
		if( context->device_list != NULL ) {
			free( context->device_list );
			context->device_list = NULL;
			context->device_count = 0;
		}

		if( context->context != 0 ) {
			clReleaseContext( context->context );
		}

		// Destroy all programs, and all kernels in those programs
		size = vul_svector_size( context->programs );
		for( k = 0; k < size; ++k ) {
			vul_cl_program *it = ( vul_cl_program* )vul_svector_get( context->programs, k );
			size2 = vul_svector_size( it->kernels );
			for( l = 0; l < size2; ++l ) {
				vul_cl_kernel *it2  = ( vul_cl_kernel* )vul_svector_get( it->kernels, l );
				// Release the kernels and free the entry point string
				if( it2->kernel ) {
					clReleaseKernel( it2->kernel );
				}
				if( it2->entry_point ) {
					free( it2->entry_point );
				}
				// Empty the argument list
				vul_svector_destroy( it2->arguments );
			}
			// Destroy the kernel list
			vul_svector_destroy( it->kernels );
			it->kernels = NULL;
			// Release the program
			clReleaseProgram( it->program );
			// Free the source
			if( it->type == VUL__CL_KERNEL_BINARY ) {
				if( it->binary ) {
					free( it->binary );
					it->binary = NULL;
				}
			} else {
				if( it->source ) {
					free( it->source );
					it->source = NULL;
				}
			}
		}
		// Destroy the program list
		vul_svector_destroy( context->programs );
		context->programs = NULL;

		// Destroy all the buffers
		size = vul_svector_size( context->buffers );
		for( k = 0; k < size; ++k ) {
			vul_cl_buffer *it = ( vul_cl_buffer* )vul_svector_get( context->buffers, k );
			if( it->buffer != 0 ) {
				clReleaseMemObject( it->buffer );
				it->buffer = 0;
			}
		}
		// Destroy the buffer list
		vul_svector_destroy( context->buffers );
		
		// Free the context
		free( context );

		vul__cl_contexts[ i ] = NULL;
	}

	free( vul__cl_contexts );
}

void vul_cl_write_compile_output( vul_cl_program *prog, FILE *out )
{
	size_t len;
	char buffer[ 65536 ];

	for( size_t i = 0; i < prog->context->device_count; ++i ) {
		fprintf( out, "Writing compile output for device %d:\n", i );
		clGetProgramBuildInfo( prog->program,
							   prog->context->device_list[ i ],
							   CL_PROGRAM_BUILD_LOG, 
							   sizeof( buffer ),
							   &buffer,
							   &len );
		fprintf( out, "%s\n", buffer );
	}
}

vul_cl_program *vul_cl_create_program( cl_platform_id platform_id, 
													char *file_path, 
													const char *build_options, 
													int is_binary,
													void ( *async_build_callback )( cl_program, void* ), 
													void *user_data )
{
	FILE *f;
	size_t lsize, j, platform_index, *lengths;
	vul_cl_program *ret;
	cl_int err, *statuses;
	cl_uint i;
	unsigned char** texts;

	// Find the platform
	assert( vul__cl_contexts );
	platform_index = vul__cl_context_count;
	for( j = 0; j < vul__cl_context_count; ++j ) {
	if( vul__cl_contexts[ j ]->platform == platform_id ) {
			platform_index = j;
			break;
		}
	}
	assert( platform_index < vul__cl_context_count );

	// Create the return object
	ret = ( vul_cl_program* )vul_svector_append_empty( vul__cl_contexts[ platform_index ]->programs );
	assert( ret );
	ret->context = vul__cl_contexts[ platform_index ];
	ret->type = is_binary ? VUL__CL_KERNEL_BINARY : VUL__CL_KERNEL_SOURCE;
	// Create kernel list
	ret->kernels = vul_svector_create( sizeof( vul_cl_kernel ), 8, malloc, free );

	// Read the suorce / binary
	f = fopen( file_path, "r" );
	assert( f );
	fseek( f, 0, SEEK_END );
	lsize = ftell( f );
	rewind( f );

	if( is_binary ) {
		ret->binary = ( unsigned char* )malloc( sizeof( char ) * ( lsize + 1 ) );
		assert( ret->binary );
		size_t val = fread( ret->binary, 1, lsize, f );
		if ( val != lsize ) {
			assert( ferror( f ) == 0 );
			ret->binary[ val ] = 0;
		}
	} else {
		ret->source = ( char* )malloc( sizeof( char ) * ( lsize + 1 ) );
		assert( ret->source );
		size_t val = fread( ret->source, 1, lsize, f );
		if ( val != lsize ) {
			assert( ferror( f ) == 0 );
			ret->source[ val ] = 0;
		}
	}
	fclose( f );

	// Load the program
	if( is_binary ) {
		lengths = ( size_t* )malloc( sizeof( size_t ) * vul__cl_contexts[ platform_index ]->device_count );
		assert( lengths );
		texts = ( unsigned char** )malloc( sizeof( unsigned char** ) * vul__cl_contexts[ platform_index ]->device_count );
		assert( texts );
		for( i = 0; i < vul__cl_contexts[ platform_index ]->device_count; ++i ) {
			lengths[ i ] = lsize;
			texts[ i ] = ret->binary;
		}
		statuses = ( cl_int* )malloc( sizeof( cl_int ) * vul__cl_contexts[ platform_index ]->device_count );
		assert( statuses );
		ret->program = clCreateProgramWithBinary( vul__cl_contexts[ platform_index ]->context,
												 vul__cl_contexts[ platform_index ]->device_count,
												 vul__cl_contexts[ platform_index ]->device_list,
												 lengths,
												 ( const unsigned char ** )texts, 
												 statuses,
												 &err );
		for( cl_uint i = 0; i < vul__cl_contexts[ platform_index ]->device_count; ++i ) {
			assert( statuses[ i ] == CL_SUCCESS );
		}
		assert( ret->program && err == CL_SUCCESS );
		free( lengths );
		free( texts );
		free( statuses );
	} else {
		ret->program = clCreateProgramWithSource( vul__cl_contexts[ platform_index ]->context,
												  1,
												  ( const char ** )&ret->source, 
												  NULL,
												  &err );
		assert( ret->program && err == CL_SUCCESS );
	}

	// Build it
	err = clBuildProgram( ret->program,
						  vul__cl_contexts[ platform_index ]->device_count,
						  vul__cl_contexts[ platform_index ]->device_list,
						  build_options,
						  async_build_callback, 
						  user_data );
	if( err != CL_SUCCESS ) {
		vul_cl_write_compile_output( ret, stdout );
		assert( 0 );
	}
	
	return ret;
}

vul_cl_kernel *vul_cl_create_kernel( vul_cl_program *program, const char *entry_point )
{
	vul_cl_kernel *ret;
	size_t len;
	cl_int err;
	
	ret = ( vul_cl_kernel* )vul_svector_append_empty( program->kernels );
	assert( ret );

	len = strlen( entry_point );
	ret->entry_point = ( char* )malloc( len + 1 );
	assert( ret->entry_point );
	strncpy( ret->entry_point, entry_point, len );
	ret->entry_point[ len ] = 0;
	ret->program = program;
	
	ret->kernel = clCreateKernel( program->program, ret->entry_point, &err );
	assert( ret->kernel && err == CL_SUCCESS );

	ret->arguments = vul_svector_create( sizeof( vul_cl_kernel_argument ), 8, malloc, free );

	return ret;
}

void vul_cl_kernel_add_argument( vul_cl_kernel *kernel, size_t data_size, const void *data_ptr )
{
	vul_cl_kernel_argument *arg;
	
	arg = ( vul_cl_kernel_argument* )vul_svector_append_empty( kernel->arguments );
	arg->content = data_ptr;
	arg->size = data_size;
}

vul_cl_buffer *vul_cl_create_buffer( cl_platform_id platform_id, cl_mem_flags flags, size_t size, void *host_ptr )
{
	size_t j, platform_index;
	vul_cl_buffer *ret;
	cl_int err;

	// Find the platform
	assert( vul__cl_contexts );
	platform_index = vul__cl_context_count;
	for( j = 0; j < vul__cl_context_count; ++j ) {
		if( vul__cl_contexts[ j ]->platform == platform_id ) {
			platform_index = j;
			break;
		}
	}
	assert( j < vul__cl_context_count );

	// Create the return object
	ret = ( vul_cl_buffer* )vul_svector_append_empty( vul__cl_contexts[ platform_index ]->buffers );
	ret->context = vul__cl_contexts[ platform_index ];
	ret->type = VUL__CL_BUFFER_HOST;
	ret->flags = flags;
	ret->gl_buffer = 0u;
	ret->host_ptr = host_ptr;
	ret->size = size;
	ret->buffer = clCreateBuffer( vul__cl_contexts[ platform_index ]->context,
								  flags,
								  size,
								  host_ptr,
								  &err );
	if( !ret->buffer || err != CL_SUCCESS ) {
		printf("CL error in buffer creation %d\n", err);
		ret->buffer = 0;
		// We failed, remove it from the reference list
		vul_svector_remove_swap( vul__cl_contexts[ platform_index ]->buffers, 
										 vul_svector_size( vul__cl_contexts[ platform_index ]->buffers ) - 1 );
		return NULL;
	}

	return ret;
}

vul_cl_buffer *vul_cl_create_gl_buffer( cl_platform_id platform_id, cl_mem_flags flags, unsigned int gl_buffer )
{
	size_t j, platform_index;
	vul_cl_buffer *ret;
	cl_int err;

	// Find the platform
	assert( vul__cl_contexts );
	platform_index = vul__cl_context_count;
	for( j = 0; j < vul__cl_context_count; ++j ) {
		if( vul__cl_contexts[ j ]->platform == platform_id ) {
			platform_index = j;
			break;
		}
	}
	assert( j < vul__cl_context_count );

	// Create the return object
	ret = ( vul_cl_buffer* )vul_svector_append_empty( vul__cl_contexts[ platform_index ]->buffers );
	ret->context = vul__cl_contexts[ platform_index ];
	ret->type = VUL__CL_BUFFER_OPENGL;
	ret->flags = flags;
	ret->gl_buffer = gl_buffer;
	ret->host_ptr = NULL; // @TODO(thynn): Find and set ret->size
	ret->buffer = clCreateFromGLBuffer( vul__cl_contexts[ platform_index ]->context,
								  flags,
								  gl_buffer,
								  &err );
	if( !ret->buffer || err != CL_SUCCESS ) {
		ret->buffer = 0;
		// We failed, remove it from the reference list
		vul_svector_remove_swap( vul__cl_contexts[ platform_index ]->buffers, 
										 vul_svector_size( vul__cl_contexts[ platform_index ]->buffers ) - 1 );
		return NULL;
	}

	return ret;
}

int vul_cl_write_buffer( size_t device_index, 
								 vul_cl_buffer *buffer, 
								 void *host_ptr, 
								 size_t offset, 
								 size_t size_of_write, 
								 int blocking_write, // Acts as bool
								 cl_uint event_wait_list_size,
								 cl_event *event_wait_list, 
								 cl_event *wait_event )
{
	cl_int err;

	assert( buffer );
	assert( buffer->type == VUL__CL_BUFFER_HOST );
	assert( device_index < buffer->context->device_count );
	assert( buffer->context->queue_list[ device_index ] );

	err = clEnqueueWriteBuffer( buffer->context->queue_list[ device_index ],
								buffer->buffer, 
								blocking_write ? CL_TRUE : CL_FALSE, 
								offset,
								size_of_write == 0 ? buffer->size : size_of_write, 
								host_ptr == 0 ? buffer->host_ptr : host_ptr,
								event_wait_list_size, 
								event_wait_list,
								wait_event );
	return err;
}

int vul_cl_copy_buffer( size_t queue_index, 
								vul_cl_buffer *src_buffer, 
								vul_cl_buffer *dst_buffer, 
								size_t src_offset,
								size_t dst_offset, 
								size_t size_of_copy, 
								cl_uint event_wait_list_size, 
								cl_event *event_wait_list, 
								cl_event *wait_event )
{
	cl_int err;

	assert( src_buffer );
	assert( dst_buffer );
	assert( src_buffer->context == dst_buffer->context );
	assert( queue_index < src_buffer->context->device_count );

	err = clEnqueueCopyBuffer( src_buffer->context->queue_list[ queue_index ],
								src_buffer->buffer, 
								dst_buffer->buffer, 
								src_offset,
								dst_offset,
								size_of_copy == 0 ? 
									src_buffer->size < dst_buffer->size ?
										src_buffer->size : 
										dst_buffer->size :
									size_of_copy, 
								event_wait_list_size, 
								event_wait_list,
								wait_event );
	return err;
}

int vul_cl_read_buffer( size_t device_index, 
								vul_cl_buffer *buffer, 
								void *host_ptr, 
								size_t offset,
								size_t size_of_read, 
								int blocking_read, // Acts as bool
								cl_uint event_wait_list_size,
								cl_event *event_wait_list, 
								cl_event *wait_event )
{
	cl_int err;

	assert( buffer );
	assert( host_ptr );
	assert( device_index < buffer->context->device_count );

	err = clEnqueueReadBuffer( buffer->context->queue_list[ device_index ],
								buffer->buffer, 
								blocking_read,
								offset,
								size_of_read == 0 ? buffer->size : size_of_read,
								host_ptr,
								event_wait_list_size, 
								event_wait_list,
								wait_event );
	return err;
}

int vul_cl_resize_buffer( vul_cl_buffer *buffer, size_t new_size, void *host_ptr ) 
{
	cl_int err;

	assert( buffer );
	
	if( buffer->buffer ) {
		clReleaseMemObject( buffer->buffer );
		buffer->buffer = 0;
	}

	buffer->size = new_size;
	if( host_ptr ) {
		buffer->host_ptr = host_ptr;
	}
	buffer->buffer = clCreateBuffer( buffer->context->context, 
									 buffer->flags, 
									 new_size,
									 host_ptr,
									 &err );
	if( !buffer->buffer || err != CL_SUCCESS ) {
		return err == CL_SUCCESS ? -1000 : err;
	}
	return 0;
}

cl_int vul_cl_call_kernel( vul_cl_kernel *kernel, 
									size_t device_index, 
									size_t global_size, 
									cl_uint event_wait_list_size, 
									cl_event *event_wait_list, 
									cl_event *wait_event )
{
	cl_int err;
	cl_uint i, size;

	assert( kernel );
	assert( kernel->program );
	assert( kernel->program->context );
	assert( device_index < kernel->program->context->device_count );

	err = 0;
	// Upload all arguments
	size = vul_svector_size( kernel->arguments );
	for( i = 0; i < size; ++i ) {
		vul_cl_kernel_argument *it = ( vul_cl_kernel_argument* )vul_svector_get( kernel->arguments, i );
		err |= clSetKernelArg( kernel->kernel, i++, it->size, it->content );
	}
	if( err != CL_SUCCESS ) {
		return err; // We failed in uploading arguments
	}
	// Get work group sizes
	size_t local_size;
	err = clGetKernelWorkGroupInfo( kernel->kernel, 
									kernel->program->context->device_list[ device_index ],
									CL_KERNEL_WORK_GROUP_SIZE, 
									sizeof( local_size ), 
									&local_size, 
									NULL );
	if( err != CL_SUCCESS ) {
		return err; // We couldn't get the work group size, we failed
	}
	if ( local_size > global_size ) {
		local_size = global_size; // Make sure we don't crash
	}
	err = clEnqueueNDRangeKernel( kernel->program->context->queue_list[ device_index ],
								  kernel->kernel, 
								  1,
								  0, 
								  &global_size, 
								  &local_size,
								  event_wait_list_size, 
								  event_wait_list, 
								  wait_event );
	if( err != CL_SUCCESS ) {
		return err; // Execution failed!
	}

	return err;
}

cl_int vul_cl_sync( vul_cl_kernel *kernel, size_t device_index )
{
	cl_int err = clFlush( kernel->program->context->queue_list[ device_index ] );
	if( err != CL_SUCCESS ) {
		return err;
	}

	return err;
}

cl_platform_id vul_cl_get_platform_by_vendor_string( const char *vendor_string )
{
	cl_int err;
	size_t i;
	size_t size;
	char *name;

	for( i = 0; i < vul__cl_context_count; ++i ) {
      if( !vul__cl_contexts[ i ] ) {
         continue;
      }
		err = clGetPlatformInfo( vul__cl_contexts[ i ]->platform,
								 CL_PLATFORM_VENDOR,
								 0,
								 NULL,
								 &size );
		name = ( char* )malloc( sizeof( char ) * size );
		err = clGetPlatformInfo( vul__cl_contexts[ i ]->platform,
								 CL_PLATFORM_VENDOR,
								 size,
								 name,
								 NULL );

		if( strcmp( vendor_string, name ) == 0 ) {
			return vul__cl_contexts[ i ]->platform;
		}
	}
	return NULL;
}

cl_platform_id vul_cl_get_platform_by_context_index( size_t index )
{
	assert( index <= vul__cl_context_count );
   
   if( vul__cl_contexts[ index ] ) {
      return vul__cl_contexts[ index ]->platform;
   }
   return ( cl_platform_id )-1;
}

void vul_cl_print_platform_vendor_strings( )
{
   cl_int err;
   size_t i;
   char *name;
	size_t size;

   for( i = 0; i < vul__cl_context_count; ++i ) {
      if( !vul__cl_contexts[ i ] ) {
         continue;
      }
		err = clGetPlatformInfo( vul__cl_contexts[ i ]->platform,
								 CL_PLATFORM_VENDOR,
								 0,
								 NULL,
								 &size );
		name = ( char* )malloc( sizeof( char ) * size );
		err = clGetPlatformInfo( vul__cl_contexts[ i ]->platform,
								 CL_PLATFORM_VENDOR,
								 size,
								 name,
								 NULL );
      printf( "Platform %d: %s\n", i, name );
   }
}

void vul_cl_print_platform_info( )
{
	cl_int err;
	size_t i, j;
	size_t size;
	char *name;
	size_t type;

	for( i = 0; i < vul__cl_context_count; ++i ) {
		if( !vul__cl_contexts[ i ]  ) {
			continue;
		}
		err = clGetPlatformInfo( vul__cl_contexts[ i ]->platform,
								 CL_PLATFORM_VENDOR,
								 0,
								 NULL,
								 &size );
		name = ( char* )malloc( sizeof( char ) * size );
		err = clGetPlatformInfo( vul__cl_contexts[ i ]->platform,
								 CL_PLATFORM_VENDOR,
								 size,
								 name,
								 NULL );
		printf( "Platform %d: %s\n", i, name );
		free( name );
		for( j = 0; j < vul__cl_contexts[ i ]->device_count; ++j ) {
			if( !vul__cl_contexts[ i ]->device_list[ j ] ) {
				continue;
			}

			err = clGetDeviceInfo( vul__cl_contexts[ i ]->device_list[ i ],
										  CL_DEVICE_NAME,
										  0,
										  NULL,
										  &size );
			name = ( char* )malloc( sizeof( char ) * size );
			err = clGetDeviceInfo( vul__cl_contexts[ i ]->device_list[ i ],
										  CL_DEVICE_NAME,
										  size,
										  name,
										  NULL );
			err = clGetDeviceInfo( vul__cl_contexts[ i ]->device_list[ i ],
										  CL_DEVICE_TYPE,
										  sizeof( type ),
										  &type,
										  NULL );
			const char *typename;
			switch(type) {
				case CL_DEVICE_TYPE_CPU: typename = "CPU"; break;
				case CL_DEVICE_TYPE_GPU: typename = "GPU"; break;
				case CL_DEVICE_TYPE_ACCELERATOR: typename = "Accelerator"; break;
				case CL_DEVICE_TYPE_DEFAULT: typename = "Default"; break;
				default: typename = "Unknown device type"; break;
			}
			printf( "\tDevice %d: %s - %s\n", j, name, typename );
			free( name );
		}
	}
}

// @TODO: Useful auxilliary functions, like "do_we_support_this_extesion_on_given_device"
// or "type_of_given_device" to find the best devices for a certain thing.
// Also better support GL interop by having aquire/release GL resources abstracted!

#ifdef __cplusplus
}
#endif

#endif
