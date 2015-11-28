#define VUL_DEFINE
#include <CL/opencl.h>
#include "vul_cl.h"

// @TODO(thynn): Check return codes!

int main(int argc, char **argv)
{
	/* Setup */
	vul_cl_setup( NULL, 0 );

	/* Print vendor strings */
	vul_cl_print_platform_vendor_strings( );

	/* Create the test program */
	cl_platform_id pid = vul_cl_get_platform_by_context_index( 0 );
	vul_cl_program *prog = vul_cl_create_program( pid, "kernel.cl", "", 0, NULL, NULL );
	assert(prog);
	vul_cl_kernel *kernel = vul_cl_create_kernel( prog, "square" );
	assert(kernel);

	/* Create two buffers */
	float ha[ 1024 ], hb[ 1024 ];
	for( unsigned int i = 0; i < 1024; ++i ) {
		ha[ i ] = ( float )i;
		hb[ i ] = ( float )i * ( float )i;
	}
	vul_cl_buffer *a = vul_cl_create_buffer( pid, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, 
														  1024 * sizeof( float ), ha );
	assert(a);
	vul_cl_buffer *c = vul_cl_create_buffer( pid, CL_MEM_WRITE_ONLY, 1024 * sizeof( float ), NULL );
	assert(c);

	/* Run the kernel */
	vul_cl_kernel_add_argument( kernel, sizeof( cl_mem ), &a->buffer );
	vul_cl_kernel_add_argument( kernel,  sizeof( cl_mem ), &c->buffer );
	assert( vul_cl_call_kernel( kernel, 0, 1024, 0, NULL, NULL ) == CL_SUCCESS );
	assert( vul_cl_sync( kernel, 0 ) == CL_SUCCESS );
	
	/* Read back the answer and check it */
	float hc[ 1024 ];
	assert( 0 == vul_cl_read_buffer( 0, c, hc, 0, 0, 1, 0, NULL, NULL ) );
	for( size_t i = 0; i < 1024; ++i ) {
		assert( hc[ i ] == hb[ i ] );
	}

	/* Clean up */
	vul_cl_cleanup( );
	
	puts("vul_cl.h: PASS");
}

