/*
 * Ease setup of CL kernels.
 */

// Possibly some state to keep track of which kernels already
// exist (hashmap cl_int -> kernel source?)

cl_int vul_cl_setup( SOMETHING );

cl_int vul_cl_make_buffer( SOMETHING );

cl_int vul_cl_call_kernel( SOMETHING );

cl_int vul_cl_cleanup( SOMETHING );