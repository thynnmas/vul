// FILE IO
// A lot of this is inspired by / taken from stb.h

#include <stdlib.h>
#include <stdio.h>
#include <io.h>
#include <assert.h>
#include <malloc.h>
#include "vul_string.h"

#define VUL_TRUE 1
#define VUL_FALSE 0

#ifdef _WIN32
	#define vulf__fopen( fn, rm )	_wfopen( vul__wchar_from_utf8_large( fn ), vul__wchar_from_utf8_small( rm ) )
	#define vulf__windows( x, y )	x
#else
	#define vulf__fopen( fn, rm )	fopen( fn, rm )
	#define vulf__windows( x, y )	y
#endif
#ifdef _MSC_VER
  #define vul_mktemp			_mktemp
  #define vul_rename( fn, rm )  _wrename(vul__wchar_from_utf8_large( fn ), vul__wchar_from_utf8_small( rm ) )
#else
  #define vul_mktemp   mktemp
  #define vul_rename   rename
#endif

typedef struct
{
	char *temp_name;
	char *name;
	int errors;
} vulf__file_data;

int vul_fullpath(char *abs, int abs_size, char *rel)
{
#ifdef _MSC_VER
	return _fullpath(abs, rel, abs_size) != NULL;
#else
	if (abs[0] == '/' || abs[0] == '~') {
		if ((int) strlen(rel) >= abs_size)
			return 0;
		strcpy(abs,rel);
		return VUL_TRUE;
	} else {
		int n;
		getcwd(abs, abs_size);
		n = strlen(abs);
		if (n+(int) strlen(rel)+2 <= abs_size) {
			abs[n] = '/';
			strcpy(abs+n+1, rel);
			return VUL_TRUE;
		} else {
			return VUL_FALSE;
		}
	}
#endif
}

FILE *vul_fopen( char *filename, char *mode )
{
	FILE *f;
	char name_full[ 4096 ];
	char temp_full[ sizeof( name_full ) + 12 ];
	int j,p;
	if ( mode[ 0 ] != 'w' && !strchr(mode, '+'))
		return vulf__fopen( filename, mode );

	// save away the full path to the file so if the program
	// changes the cwd everything still works right! unix has
	// better ways to do this, but we have to work in windows
	if ( vul_fullpath( name_full, sizeof( name_full ), filename ) == 0 )
		return 0;

	// try to generate a temporary file in the same directory
	p = strlen( name_full ) - 1;
	while ( p > 0 && name_full[ p ] != '/' && name_full[ p ] != '\\'
				&& name_full[ p ] != ':' && name_full[ p ] != '~')
		--p;
	++p;

	memcpy( temp_full, name_full, p );

#ifdef _MSC_VER
	// try multiple times to make a temp file... just in
	// case some other process makes the name first
	for ( j=0; j < 32; ++j ) {
		strcpy( temp_full + p, "vtmpXXXXXX" );
		if ( vul_mktemp( temp_full ) == NULL )
			return 0;

		f = fopen(temp_full, mode);
		if (f != NULL)
			break;
	}
#else
	{
		strcpy(temp_full+p, "stmpXXXXXX");
		int fd = mkstemp(temp_full);
		if (fd == -1) return NULL;
		f = fdopen(fd, mode);
		if (f == NULL) {
			unlink(temp_full);
			close(fd);
			return NULL;
		}
	}
#endif
	if (f != NULL) {
		vulf__file_data *d = ( vulf__file_data * ) malloc( sizeof( *d ) );
		if ( !d ) { assert( 0 );  /* NOTREACHED */ fclose( f ); return NULL; }
		if ( stb__files == NULL ) stb__files = stb_ptrmap_create();
		d->temp_name = strdup(temp_full);
		d->name      = strdup(name_full);
		d->errors    = 0;
		stb_ptrmap_add(stb__files, f, d);
		return f;
	}

	return NULL;
}