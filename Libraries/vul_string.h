/*
* Villains' Utility Library - Thomas Martin Schmid, 2015. Public domain¹
*
* This file contains useful string-handling functions. The UTF-8 handling
* is from stb.h, while the search/pattern matching is our own.
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
#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <wchar.h>

typedef wchar_t vul_wchar;

typedef struct vul__string_search_table
{
	size_t size;
	int *entries;
} vul__string_search_table;


// Define this in ONE of your C/C++ files
// #define VUL_DEFINE

//--------------------------
// UTF-8 handline
//

#ifndef VUL_DEFINE
vul_wchar *vul__wchar_from_utf8( vul_wchar *buffer, char *ostr, int n );
#else
vul_wchar *vul__wchar_from_utf8( vul_wchar *buffer, char *ostr, int n)
{
   unsigned char *str = ( unsigned char* ) ostr;
   unsigned int c;
   int i=0;
   --n;
   while (*str) {
      if (i >= n)
         return NULL;
      if (!(*str & 0x80))
         buffer[i++] = *str++;
      else if ((*str & 0xe0) == 0xc0) {
         if (*str < 0xc2) return NULL;
         c = (*str++ & 0x1f) << 6;
         if ((*str & 0xc0) != 0x80) return NULL;
         buffer[i++] = c + (*str++ & 0x3f);
      } else if ((*str & 0xf0) == 0xe0) {
         if (*str == 0xe0 && (str[1] < 0xa0 || str[1] > 0xbf)) return NULL;
         if (*str == 0xed && str[1] > 0x9f) return NULL; // str[1] < 0x80 is checked below
         c = (*str++ & 0x0f) << 12;
         if ((*str & 0xc0) != 0x80) return NULL;
         c += (*str++ & 0x3f) << 6;
         if ((*str & 0xc0) != 0x80) return NULL;
         buffer[i++] = c + (*str++ & 0x3f);
      } else if ((*str & 0xf8) == 0xf0) {
         if (*str > 0xf4) return NULL;
         if (*str == 0xf0 && (str[1] < 0x90 || str[1] > 0xbf)) return NULL;
         if (*str == 0xf4 && str[1] > 0x8f) return NULL; // str[1] < 0x80 is checked below
         c = (*str++ & 0x07) << 18;
         if ((*str & 0xc0) != 0x80) return NULL;
         c += (*str++ & 0x3f) << 12;
         if ((*str & 0xc0) != 0x80) return NULL;
         c += (*str++ & 0x3f) << 6;
         if ((*str & 0xc0) != 0x80) return NULL;
         c += (*str++ & 0x3f);
         // utf-8 encodings of values used in surrogate pairs are invalid
         if ((c & 0xFFFFF800) == 0xD800) return NULL;
         if (c >= 0x10000) {
            c -= 0x10000;
            if (i + 2 > n) return NULL;
            buffer[i++] = 0xD800 | (0x3ff & (c >> 10));
            buffer[i++] = 0xDC00 | (0x3ff & (c      ));
         }
      } else
         return NULL;
   }
   buffer[i] = 0;
   return buffer;
}
#endif

#ifndef VUL_DEFINE
char *vul__wchar_to_utf8( char *buffer, vul_wchar *str, int n );
#else
char *vul__wchar_to_utf8( char *buffer, vul_wchar *str, int n )
{
   int i=0;
   --n;
   while (*str) {
      if (*str < 0x80) {
         if (i+1 > n) return NULL;
         buffer[i++] = (char) *str++;
      } else if (*str < 0x800) {
         if (i+2 > n) return NULL;
         buffer[i++] = 0xc0 + (*str >> 6);
         buffer[i++] = 0x80 + (*str & 0x3f);
         str += 1;
      } else if (*str >= 0xd800 && *str < 0xdc00) {
         unsigned int c;
         if (i+4 > n) return NULL;
         c = ((str[0] - 0xd800) << 10) + ((str[1]) - 0xdc00) + 0x10000;
         buffer[i++] = 0xf0 + (c >> 18);
         buffer[i++] = 0x80 + ((c >> 12) & 0x3f);
         buffer[i++] = 0x80 + ((c >>  6) & 0x3f);
         buffer[i++] = 0x80 + ((c      ) & 0x3f);
         str += 2;
      } else if (*str >= 0xdc00 && *str < 0xe000) {
         return NULL;
      } else {
         if (i+3 > n) return NULL;
         buffer[i++] = 0xe0 + (*str >> 12);
         buffer[i++] = 0x80 + ((*str >> 6) & 0x3f);
         buffer[i++] = 0x80 + ((*str     ) & 0x3f);
         str += 1;
      }
   }
   buffer[i] = 0;
   return buffer;
}
#endif

#ifndef VUL_DEFINE
vul_wchar *vul_wchar_from_utf8_large( char *str );
#else
vul_wchar *vul_wchar_from_utf8_large( char *str )
{
	static vul_wchar buffer[ 4096 ];
	return vul__wchar_from_utf8( buffer, str, 4096 );
}
#endif

#ifndef VUL_DEFINE
vul_wchar *vul_wchar_from_utf8_small( char *str );
#else
vul_wchar *vul_wchar_from_utf8_small( char *str )
{
	static vul_wchar buffer[ 64 ];
	return vul__wchar_from_utf8( buffer, str, 64 );
}
#endif

#ifndef VUL_DEFINE
char *vul_wchar_to_utf8_large( vul_wchar *str );
#else
char *vul_wchar_to_utf8_large( vul_wchar *str )
{
	static char buffer [ 4096 ];
	return vul__wchar_to_utf8( buffer, str, 4096 );
}
#endif

#ifndef VUL_DEFINE
char *vul_wchar_to_utf8_small( vul_wchar *str );
#else
char *vul_wchar_to_utf8_small( vul_wchar *str )
{
	static char buffer[ 64 ];
	return vul__wchar_to_utf8( buffer, str, 64 );
}
#endif

//--------------------------------------------
// String searching / pattern matching.
//

#ifndef VUL_DEFINE
void vul__string_calculate_search_table( const char *pattern, vul__string_search_table *table );
#else
void vul__string_calculate_search_table( const char *pattern, vul__string_search_table *table )
{
	unsigned int pos;
	int cnd;

	pos = 2;
	cnd = 0;
	table->entries[ 0 ] = -1;
	table->entries[ 1 ] = 0;

	while ( pos < table->size )
	{
		if ( pattern[ pos - 1 ] == pattern[ cnd ] ) {
			table->entries[ pos++ ] = ++cnd;
		} else if ( cnd > 0 ) {
			cnd = table->entries[ cnd ];
		} else {
			table->entries[ pos++ ] = 0;
		}
	}
}
#endif

#ifndef VUL_DEFINE
void vul__wstring_calculate_search_table( const vul_wchar *pattern, vul__string_search_table *table );
#else
void vul__wstring_calculate_search_table( const vul_wchar *pattern, vul__string_search_table *table )
{
	unsigned int pos;
	int cnd;

	pos = 2;
	cnd = 0;
	table->entries[ 0 ] = -1;
	table->entries[ 1 ] = 0;

	while ( pos < table->size )
	{
		if ( pattern[ pos - 1 ] == pattern[ cnd ] ) {
			table->entries[ pos++ ] = ++cnd;
		} else if ( cnd > 0 ) {
			cnd = table->entries[ cnd ];
		} else {
			table->entries[ pos++ ] = 0;
		}
	}
}
#endif

#ifndef VUL_DEFINE
size_t vul__string_search( const char *str, const char *pattern, size_t str_len, vul__string_search_table *table );
#else
size_t vul__string_search( const char *str, const char *pattern, size_t str_len, vul__string_search_table *table )
{
	size_t m, i;

	m = 0;
	i = 0;

	while ( m + i < str_len )
	{
		if ( pattern[ i ] == str[ m + i ] ) {
			if ( i++ == table->size - 1 )
				return m;
		} else {
			m = m + i - table->entries[ i ];
			if ( table->entries[ i ] > -1 )
				i = table->entries[ i ];
			else
				i = 0;
		}
	}
	return str_len;
}
#endif

#ifndef VUL_DEFINE
size_t vul__wstring_search( const vul_wchar *str, const vul_wchar *pattern, size_t str_len, vul__string_search_table *table );
#else
size_t vul__wstring_search( const vul_wchar *str, const vul_wchar *pattern, size_t str_len, vul__string_search_table *table )
{
	size_t m, i;

	m = 0;
	i = 0;

	while ( m + i < str_len )
	{
		if ( pattern[ i ] == str[ m + i ] ) {
			if ( i++ == table->size - 1 )
				return m;
		} else {
			m = m + i - table->entries[ i ];
			if ( table->entries[ i ] > -1 )
				i = table->entries[ i ];
			else
				i = 0;
		}
	}
	return str_len;
}
#endif

/**
 * Search for a pattern in a string. Uses Knuth-Morris-Pratt.
 * Returns the zero-based index of the first character of the match,
 * or the length of the string if not found.
 */
#ifndef VUL_DEFINE
size_t vul_string_search( const char *str, const char *pattern );
#else
size_t vul_string_search( const char *str, const char *pattern )
{
	size_t ret, lenS;
	vul__string_search_table table;

	lenS = strlen( str );

	if ( lenS == 0 ) return 0;

	table.size = strlen( pattern );
	if ( table.size == 0 ) return lenS;

	table.entries = ( int* )malloc( table.size * sizeof( int ) );
	if ( !table.entries ) return lenS;

	vul__string_calculate_search_table( pattern, &table );
	
	ret = vul__string_search( str, pattern, lenS, &table );
	
	free( table.entries );
	
	return ret;
}
#endif

/**
* Search for a pattern in a string. Uses Knuth-Morris-Pratt.
* Returns the zero-based index of the first character of the match,
* or the length of the string if not found.
*/
#ifndef VUL_DEFINE
size_t vul_wstring_search( const vul_wchar *str, const vul_wchar *pattern );
#else
size_t vul_wstring_search( const vul_wchar *str, const vul_wchar *pattern )
{
	size_t ret, lenS;
	vul__string_search_table table;

	lenS = wcslen( str );

	if ( lenS == 0 ) return 0;

	table.size = wcslen( pattern );
	if ( table.size == 0 ) return lenS;

	table.entries = ( int* )malloc( table.size * sizeof( int ) );
	if ( !table.entries ) return lenS;

	vul__wstring_calculate_search_table( pattern, &table );
	
	ret = vul__wstring_search( str, pattern, lenS, &table );
	
	free( table.entries );
	
	return ret;
}
#endif

//-----------------------------
// Useful substring functions
//

/**
 * Returns a pointer to the first character after the last occurance of divisor.
 */
#ifndef VUL_DEFINE
char *vul_string_divide_get_last( const char *str, const char divisor );
#else
char *vul_string_divide_get_last( const char *str, const char divisor )
{
	size_t index;
	
	index = strlen( str ) - 1;
	
	while ( index > 0 && str[ index ] != divisor )
		--index;

	return ( char* )( str + index );
} 
#endif

#ifndef VUL_DEFINE
vul_wchar *vul_wstring_divide_get_last( const vul_wchar *str, const vul_wchar divisor );
#else
vul_wchar *vul_wstring_divide_get_last( const vul_wchar *str, const vul_wchar divisor )
{
	size_t index;
	
	index = wcslen( str ) - 1;
	
	while ( index > 0 && str[ index ] != divisor )
		--index;

	return ( vul_wchar* )( str + index );
}
#endif

/**
 * Returns a pointer to the last character before the frist occurance of divisor.
 */
#ifndef VUL_DEFINE
char *vul_string_divide_get_first( const char *str, const char divisor );
#else
char *vul_string_divide_get_first( const char *str, const char divisor )
{
	size_t index, len;
	
	len = strlen( str ) - 1;
	index = 0;
	
	while ( index < len && str[ index ] != divisor )
		++index;

	return ( char* )( str + index );
} 
#endif

#ifndef VUL_DEFINE
vul_wchar *vul_wstring_divide_get_first( const vul_wchar *str, const vul_wchar divisor );
#else
vul_wchar *vul_wstring_divide_get_first( const vul_wchar *str, const vul_wchar divisor )
{
	size_t index, len;
	
	len = wcslen( str ) - 1;
	index = 0;
	
	while ( index < len && str[ index ] != divisor )
		++index;

	return ( vul_wchar* )( str + index );
}
#endif