// FILE IO
// A lot of this is inspired by / taken from stb.h

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <malloc.h>

#ifdef _MSC_VER
// Windows doesn't hav mmap, so we create our own

#define PROT_NONE 0
#define PROT_READ 1
#define PROT_WRITE 2

#define MAP_FAILED 0
#define MAP_SHARED 1
#define MAP_PRIVATE 2
#define MAP_FIXED 4

extern void *vul_mmap( void *addr, size_t length, int prot, int flags, int fd, size_t offset );
extern int vul_munmap( void *addr, size_t length );

#ifdef VUL_DEFINE
void *vul_mmap( void *addr, size_t length, int prot, int flags, int fd, size_t offset )
{

}
int vul_munmap( void *addr, size_t length )
{

}
#endif
#else
	#include <sys/mman.h>
	#define vul_mmap mmap
	#define vul_munmap munmap
#endif