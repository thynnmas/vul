/*
 * Villains' Utility Library - Thomas Martin Schmid, 2015. Public domain¹
 *
 * This file includes an abstraction of mmap.
 *
 * @TODO: Properly test all of these!
 * 
 * ¹ If public domain is not legally valid in your legal jurisdiction
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
#endif

#ifdef VUL_WINDOWS
#define PROT_NONE 0
#define PROT_READ FILE_MAP_READ
#define PROT_WRITE FILE_MAP_WRITE
#define PROT_EXEC FILE_MAP_EXECUTE
#else
#define PROT_NONE 0
#define PROT_READ 1
#define PROT_WRITE 2
#define PROT_EXEC 3
#endif

#define MAP_FAILED 0
#define MAP_SHARED 1
#define MAP_PRIVATE 2
#define MAP_FIXED 4

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
#include <sys/mman.h>
vul_mmap_file_t vul_mmap_file( const char *path, void *base_addr, int prot, int flags, size_t file_offset, size_t map_length )
{
	vul_mmap_file_t ret;

	ret.fd = fopen( path, // @TODO(thynn): this flag! );
	if( map_length == -1 ) {
		fseek( ret.fd, 0L, SEEK_END );
		map_length = ftell( ret.fd );
	}
	ret.map = mmap( addr, map_length, prot, flags, fd, file_offset );
	ret.length = length;

	return ret;
}
int vul_munmap( vul_mmap_file_t file )
{
	int ret;
	
	ret = munmap( file.map, file.length );
	ret |= flose( ret.fd );

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