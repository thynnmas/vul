#define VUL_TEST_CPP // @TODO: Remove this and do it properly
#ifdef VUL_TEST_CPP

#define VUL_DEFINE
#include "Tests/test_gl.hpp"
#include <stdio.h>

using namespace vul;
using namespace vul_test;

#include <malloc.h>
int main( int argc, char **argv )
{
	TestGL::test( );

	// Wait for input
	printf( "Done, no errors.\n" );

	return 0;
}

#endif