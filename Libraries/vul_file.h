/*
 * Villains' Utility Library - Thomas Martin Schmid, 2016. Public domain?
 *
 * This file includes an abstraction of mmap and a bunch of file handling functions
 * from stb.h (since including only those from stb.h without the rest of the file 
 * was a bit of a hassle). They have been renamed to avoid collission if stb.h is
 * included.
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
#define VUL_FILE_H

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#ifndef VUL_OSX
#include <malloc.h>
#endif

#ifdef VUL_WINDOWS
#include <Windows.h>
#include <io.h>
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

#include "vul_string.h"
#include "vul_types.h"

#define VUL_TRUE 1
#define VUL_FALSE 0

#ifdef VUL_WINDOWS
#define VUL_MMAP_PROT_NONE 0
#define VUL_MMAP_PROT_READ 1
#define VUL_MMAP_PROT_WRITE 2
#define VUL_MMAP_PROT_EXEC 4
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

typedef struct vul_mmap_file {
	void *map;
	size_t length;
#ifdef VUL_WINDOWS
	HANDLE hMapping;
	HANDLE hFile;
#else
	s32 fd;
#endif
} vul_mmap_file;

typedef struct vul_file {
	FILE *file;
	char *path;
	char *tmp_path;
} vul_file;

typedef enum vul_file_keep {
	vul_file_keep_no = 0,
	vul_file_keep_yes = 1,
	vul_file_keep_if_different = 2,
} vul_file_keep;

#ifdef _cplusplus
extern "C" {
#endif

vul_mmap_file vul_mmap( const char *path, void *base_addr, s32 prot, s32 flags, size_t file_offset, size_t map_length );
b32 vul_munmap( vul_mmap_file file );
char *vul_file_find_postfix( char *filename );
char *vul_file_name_without_path( char *filename );
b32 vul_file_fullpath( char *abs_path, size_t abs_path_max_len, const char *rel_path );
size_t vul_file_length( FILE *f );
s32 vul_file_compare( FILE *f, FILE *g );
s32 vul_file_equal( char *s1, char *s2 );
s32 vul_file_exists( char *filename );
vul_file *vul_file_open( char *filename, char *mode, void* ( *allocator )( size_t ) );
s32 vul_file_close( vul_file *f, vul_file_keep keep, void( *deallcator )( void* ) );
b32 vul_file_copy( char *src, char *dest );

#ifdef _cplusplus
}
#endif
#endif

#ifdef VUL_DEFINE

#ifdef _cplusplus
extern "C" {
#endif

vul_mmap_file vul_mmap( const char *path, void *base_addr, s32 prot, s32 flags, size_t file_offset, size_t map_length )
{
	vul_mmap_file ret;

#ifdef VUL_WINDOWS
	DWORD mode = GENERIC_READ 
			   | ( ( prot & VUL_MMAP_PROT_WRITE ) ? GENERIC_WRITE : 0 )
			   | ( ( prot & VUL_MMAP_PROT_EXEC )  ? GENERIC_EXECUTE : 0 );
	DWORD pmode = PAGE_READONLY;
	if( prot & VUL_MMAP_PROT_WRITE ) pmode = PAGE_READWRITE;
	if( prot & VUL_MMAP_PROT_EXEC ) {
		if( pmode == PAGE_READWRITE ) {
			pmode = PAGE_EXECUTE_READWRITE;
		} else {
			pmode = PAGE_EXECUTE_READ;
		}
	}
	ret.hFile = CreateFile( path, mode, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	ret.hMapping = CreateFileMapping( ret.hFile, NULL, pmode, 0, 0, NULL );
	
	if( map_length == -1 ) {
		map_length = GetFileSize( ret.hFile, NULL );
	}

	u64 ofs_high = ( u64 )file_offset;
	if( base_addr ) {
		ret.map = MapViewOfFileEx( ret.hMapping,
									prot,
									ofs_high >> 32,
									file_offset & 0xffffffff,
									map_length,
									base_addr );
	} else {
		ret.map = MapViewOfFileEx( ret.hMapping,
								   prot,
								   ofs_high >> 32,
								   file_offset & 0xffffffff,
								   map_length,
								   NULL );
	}
#else
	ret.fd = open( path, O_RDONLY );
	if( map_length == -1 ) {
		struct stat sb;
		fstat(ret.fd, &sb);
		map_length = ( size_t )sb.st_size;
	}
	ret.map = mmap( base_addr, map_length, prot, flags, ret.fd, file_offset );
#endif
	ret.length = map_length;

	return ret;
}

b32 vul_munmap( vul_mmap_file file )
{
	b32 ret;
#ifdef VUL_WINDOWS
	ret = UnmapViewOfFile( file.map );
	if( !ret ) {
		return -1;
	}

	ret = CloseHandle( file.hFile );

	return ( ret == 0 ) ? -1 : 0;
#else
	ret = munmap( file.map, file.length );
	ret |= close( file.fd );
#endif
	return ret;
}

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

char *vul_file_name_without_path( char *filename )
{
	char *p = filename;
	while( *( p++ ) )
		; // Move to the end of the string
	--p;
	while( *p != '\\' && *p != '/' )
		--p; // Search backward for the next \ or /
	return ( *p == '\\' || *p == '/' )
		? ( char* )( ( size_t )p + 1 )
		: p;		
}

b32 vul_file_fullpath( char *abs_path, size_t abs_path_max_len, const char *rel_path )
{
#ifdef VUL_WINDOWS
	return _fullpath( abs_path, rel_path, abs_path_max_len ) != NULL;
#else
	if( rel_path[ 0 ] == '/' || rel_path[ 0 ] == '~') {
		if( ( s32 )strlen( rel_path ) >= abs_path_max_len ) {
			return VUL_FALSE;
		}
		strcpy( abs_path, rel_path );
		return VUL_TRUE;
	} else {
		s32 n;
		getcwd( abs_path, abs_path_max_len );
		n = strlen( abs_path );
		if( n + ( s32 )strlen( rel_path ) + 2 <= abs_path_max_len ) {
			abs_path[ n ] = '/';
			strcpy( abs_path + n + 1, rel_path );
			return VUL_TRUE;
		} else {
			return VUL_FALSE;
		}
	}
#endif
}

size_t vul_file_length( FILE *f )
{
	size_t len;
	long pos;

	pos = ftell( f );
	fseek( f, 0, SEEK_END );
	len = ftell( f );
	fseek( f, pos, SEEK_SET );
	return len;
}

s32 vul_file_compare( FILE *f, FILE *g )
{
	char buf1[ 1024 ], buf2[ 1024 ];
	size_t n1, n2;
	s32 res = 0;

	while( 1 ) {
		n1 = fread( buf1, 1, sizeof( buf1 ), f );
		n2 = fread( buf2, 1, sizeof( buf2 ), g );
		res = memcmp( buf1, buf2, ( ( n1 < n2 ) ? n1 : n2 ) );
		if( res )
			break;
		if( n1 != n2 ) {
			res = n1 < n2 ? -1 : 1;
			break;
		}
		if( n1 == 0 )
			break;
	}

	fclose( f );
	fclose( g );
	return res;
}

s32 vul_file_equal( char *s1, char *s2 )
{
	FILE *f, *g;

#ifdef VUL_WINDOWS
	f = _wfopen( vul_wchar_from_utf8_large( s1 ), 
					   vul_wchar_from_utf8_small( "rb" ) );
	g = _wfopen( vul_wchar_from_utf8_large( s2 ), 
					   vul_wchar_from_utf8_small( "rb" ) );
#else
	f = fopen( s1, "rb" );
	g = fopen( s2, "rb" );
#endif

	if( f == NULL || g == NULL ) {
		if( f ) fclose( f );
		if( g ) fclose( g );
		return f == g;
	}

	if( vul_file_length( f ) != vul_file_length( g ) ) {
		fclose( f );
		fclose( g );
		return 0;
	}

	return !vul_file_compare( f, g );
}

s32 vul_file_exists( char *filename )
{
#ifdef VUL_WINDOWS
	struct _stat buf;
	return _wstat( ( const wchar_t * )vul_wchar_from_utf8_large( filename ), &buf ) == 0;
#else
	struct stat buf;
	return stat( filename, &buf ) == 0;
#endif
}

vul_file *vul_file_open( char *filename, char *mode, void* ( *allocator )( size_t ) )
{
	vul_file *f = ( vul_file* )allocator( sizeof( vul_file ) );
	size_t p;
	char name_full[ 4096 ];
	char temp_full[ sizeof( name_full ) + 12 ];
#ifdef VUL_WINDOWS
	s32 j;
#endif

	/* Handle the read-only case */
	if( mode[ 0 ] != 'w' && !strchr( mode, '+' ) ) {
		
		/* Mark that we have no tempfile */
		f->tmp_path = NULL;
		f->path = NULL;

		/* And open it */
#ifdef VUL_WINDOWS
		f->file = _wfopen( ( const wchar_t * )vul_wchar_from_utf8_large( filename ),
						   ( const wchar_t * )vul_wchar_from_utf8_small( mode ) );
#else
		f->file = fopen( filename, mode );
#endif
		return f;
	}
	
	// save away the full path to the file so if the program
	// changes the cwd everything still works right! unix has
	// better ways to do this, but we have to work in windows
	name_full[0] = '\0'; // stb_fullpath reads name_full[0]
	if (vul_file_fullpath(name_full, sizeof(name_full), filename)==0)
		return NULL;
	
	// try to generate a temporary file in the same directory
	p = strlen(name_full) - 1;
	while( p > 0 && name_full[ p ] != '/' && name_full[ p ] != '\\'
				 && name_full[ p ] != ':' && name_full[ p ] != '~' )
			--p;
	++p;

	memcpy( temp_full, name_full, p );

#ifdef VUL_WINDOWS
	// try multiple times to make a temp file... just in
	// case some other process makes the name first
	f->file = NULL;
	for( j = 0; j < 32; ++j ) {
		// windows _mktemp sucks, so add a small random prefix ourselves
		char name[ 15 ], c1, c2, c3;
		s32 r = rand() % RAND_MAX;
		c1 = 'a' + ( r & 0xf );
		c2 = 'a' + ( ( r & 0xf0 ) >> 4 );
		c3 = 'a' + ( ( r & 0xf00 ) >> 8 );
		sprintf( name, "vtmp%c%c%cXXXXXX", c1, c2, c3 );
		name[ 14 ] = 0;
		strcpy( temp_full + p, name );
		if( _mktemp( temp_full ) == NULL ) {
			continue;
		}

		f->file = fopen( temp_full, mode );
		if( f->file != NULL ) {
			break;
		}
	}
#else
	{
		strcpy( temp_full + p, "vtmpXXXXXX" );
		s32 fd = mkstemp( temp_full );
		if( fd == -1 ) return NULL;
		f->file = fdopen( fd, mode );
		if( f->file == NULL ) {
			unlink( temp_full );
			close( fd );
			return NULL;
		}
	}
#endif
   if( f->file != NULL ) {
		/* Store the information */
#ifdef VUL_WINDOWS
		f->tmp_path = _strdup( temp_full );
		f->path = _strdup( name_full );
#else
		f->tmp_path = strdup( temp_full );
		f->path = strdup( name_full );
#endif

		/* And return the file */
		return f;
   }

   return NULL;
}

s32 vul_file_close( vul_file *f, vul_file_keep keep, void (*deallcator)( void* ) )
{
	s32 ok = VUL_FALSE;
	if( f->file == NULL ) {
		return VUL_FALSE;
	}

	if( ferror( f->file ) )
		keep = vul_file_keep_no;

	fclose( f->file );

	if( f->tmp_path == NULL ) {
		return VUL_TRUE; // No temporary file to copy
	}

	if( keep == vul_file_keep_if_different ) {
		// check if the files are identical
		if( vul_file_equal( f->path, f->tmp_path ) ) {
			keep = vul_file_keep_no;
			ok = VUL_TRUE;  // report success if no change
		}
	}

	if( keep != vul_file_keep_no ) {
		if( vul_file_exists( f->path ) && remove( f->path ) ) {
			// failed to delete old, so don't keep new
			keep = vul_file_keep_no;
		} else {
#ifdef VUL_WINDOWS
			vul_wchar btmp[ 4096 ], bp[ 4096 ];
			vul_wchar_from_utf8( btmp, f->tmp_path, 4096 );
			vul_wchar_from_utf8( bp, f->path, 4096 );
			if( !_wrename( btmp, bp ) ) {
#else
			if( !rename( f->tmp_path, f->path ) ) {
#endif
				ok = VUL_TRUE;
			} else {
				keep = vul_file_keep_no;
			}
		}
	}

	if( keep == vul_file_keep_no )
		remove( f->tmp_path );

	deallcator( f->tmp_path );
	deallcator( f->path );
	deallcator( f );

	return ok;
}

b32 vul_file_copy( char *src, char *dest, 
						void* ( *allocator )( size_t ),
						void( *deallcator )( void* ) )
{
	char raw_buffer[ 1024 ];
	char *buffer;
	s32 buf_size = 65536;

	FILE *f, *g;

	// if file already exists at destination, do nothing
	if( vul_file_equal( src, dest ) ) return VUL_TRUE;

	// open file
#ifdef VUL_WINDOWS
	f = _wfopen( vul_wchar_from_utf8_large( src ), vul_wchar_from_utf8_small( "rb" ) );
#else
	f = fopen( src, "rb" );
#endif
	if( f == NULL ) return VUL_FALSE;

	// open file for writing
#ifdef VUL_WINDOWS
	g = _wfopen( vul_wchar_from_utf8_large( src ), vul_wchar_from_utf8_small( "wb" ) );
#else
	g = fopen( src, "wb" );
#endif
	if( g == NULL ) {
		fclose( f );
		return VUL_FALSE;
	}

	buffer = ( char * )allocator( buf_size );
	if( buffer == NULL ) {
		buffer = raw_buffer;
		buf_size = sizeof( raw_buffer );
	}

	while( !feof( f ) ) {
		size_t n = fread( buffer, 1, buf_size, f );
		if( n != 0 )
			fwrite( buffer, 1, n, g );
	}

	fclose( f );
	if( buffer != raw_buffer )
		deallcator( buffer );

	fclose( g );
	return VUL_TRUE;
}

#ifdef _cplusplus
}
#endif

#endif
