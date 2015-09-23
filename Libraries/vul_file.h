/*
 * Villains' Utility Library - Thomas Martin Schmid, 2015. Public domain?
 *
 * This file includes an abstraction of mmap.
 *
 * @TODO: Properly test all of these!
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
#ifndef VUL_FILE_H

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <malloc.h>

#ifdef VUL_WINDOWS
#include <Windows.h>
#else
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#endif

#ifdef VUL_WINDOWS
#define VUL_MMAP_PROT_NONE 0
#define VUL_MMAP_PROT_READ FILE_MAP_READ
#define VUL_MMAP_PROT_WRITE FILE_MAP_WRITE
#define VUL_MMAP_PROT_EXEC FILE_MAP_EXECUTE
#else
#define VUL_MMAP_PROT_NONE 0
#define VUL_MMAP_PROT_READ 1
#define VUL_MMAP_PROT_WRITE 2
#define VUL_MMAP_PROT_EXEC 3
#endif

#define VUL_MMAP_MAP_FAILED 0
#define VUL_MMAP_MAP_SHARED 1
#define VUL_MMAP_MAP_PRIVATE 2
#define VUL_MMAP_MAP_FIXED 4

typedef struct vul_mmap_file_t {
	void *map;
	size_t length;
#ifdef VUL_WINDOWS
	HANDLE hFile;
#else
	int fd;
#endif
} vul_mmap_file_t;

vul_mmap_file_t vul_mmap_file( const char *path, void *base_addr, int prot, int flags, size_t file_offset, size_t map_length );
int vul_munmap( vul_mmap_file_t );

#ifdef VUL_DEFINE
#ifdef VUL_WINDOWS
vul_mmap_file_t vul_mmap_file( const char *path, void *base_addr, int prot, int flags, size_t file_offset, size_t map_length )
{
	vul_mmap_file_t ret;

	ret.hFile = OpenFileMapping( ( DWORD )base_addr, TRUE, path );

	if( map_length == -1 ) {
		map_length = GetFileSize( ret.hFile, NULL );
	}

	ui64_t ofs_high = ( ui64_t )file_offset;
	if( base_addr ) {
		ret.map = MapViewOfFileEx( ret.hFile,
									prot,
									ofs_high >> 32,
									file_offset & 0xffffffff,
									map_length,
									base_addr );
	} else {
		ret.map = MapViewOfFileEx( ret.hFile,
								   prot,
								   ofs_high >> 32,
								   file_offset & 0xffffffff,
								   map_length,
								   NULL );
	}

	return ret;
}
int vul_munmap( vul_mmap_file_t file )
{
	BOOL ret;
	
	ret = UnmapViewOfFile( file.map );
	if( !ret ) {
		return -1;
	}

	ret = CloseHandle( file.hFile );

	return ( ret == 0 ) ? -1 : 0;
}
#else
vul_mmap_file_t vul_mmap_file( const char *path, void *base_addr, int prot, int flags, size_t file_offset, size_t map_length )
{
   vul_mmap_file_t ret;

   ret.fd = open( path, O_RDONLY );
   if( map_length == -1 ) {
	  struct stat sb;
	  fstat(ret.fd, &sb);
	  map_length = ( size_t )sb.st_size;
   }
   ret.map = mmap( base_addr, map_length, prot, flags, ret.fd, file_offset );
   ret.length = map_length;

   return ret;
}
int vul_munmap( vul_mmap_file_t file )
{
	int ret;
	
	ret = munmap( file.map, file.length );
	ret |= close( file.fd );

	return ret;
}
#endif

#endif

#ifndef VUL_DEFINE
char *vul_file_find_postfix( char *filename );
#else
char *vul_file_find_postfix( char *filename )
{
	char *p = filename;
	while( *( p++ ) )
		; // Find the end
	--p;
	while( *( p ) != '.' && p != filename )
		--p;
	return ( *p == '.' ) 
		 ? ( char* )( ( size_t )p + 1 )
		 : p;
}
#endif

#define VUL_FILE_H
#endif
