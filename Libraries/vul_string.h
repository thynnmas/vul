/*
* Villains' Utility Library - Thomas Martin Schmid, 2016. Public domain?
*
* This file contains useful string-handling functions. The UTF-8 handling
* is from stb.h, while the search/pattern matching is our own.
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
#ifndef VUL_STRING_H
#define VUL_STRING_H

#include <string.h>
#include <stdio.h>
#ifndef VUL_OSX
#include <malloc.h>
#endif
#include <stdlib.h>
#include <wchar.h>

#ifndef VUL_TYPES_H
#include <stdint.h>
#define s32 int32_t
#define u32 uint32_t
#define u8 uint8_t
#endif

typedef wchar_t vul_wchar;

struct vul__string_search_table
{
   size_t size;
   int *entries;
};

#ifdef __cplusplus
extern "C" {
#endif
//--------------------------
// UTF-8 handling
//
/*
 * Convert a utf8 string to utf16. The destination buffer must be allocated,
 * n is the maximal length of the output string.
 */
vul_wchar *vul_wchar_from_utf8( vul_wchar *buffer, const char *ostr, s32 n );
/*
 * Convert a utf16 string to utf8. The destination buffer must be allocated,
 * n is the maximal length of the output string.
 */
char *vul_wchar_to_utf8( char *buffer, const vul_wchar *str, s32 n );

/*
 * Convert a utf8 string to utf16. Strings up to 4096 characters, 
 * lifetime until next call to this function. Not threadsafe!
 */
vul_wchar *vul_wchar_from_utf8_large( const char *str );
/*
 * Convert a utf8 string to utf16. Strings up to 64 characters, 
 * lifetime until next call to this function. Not threadsafe!
 */
vul_wchar *vul_wchar_from_utf8_small( const char *str );
/*
 * Convert a utf16 string to utf8. Strings up to 4096 characters, 
 * lifetime until next call to this function. Not threadsafe!
 */
char *vul_wchar_to_utf8_large( const vul_wchar *str );
/*
 * Convert a utf16 string to utf8. Strings up to 64 characters, 
 * lifetime until next call to this function. Not threadsafe!
 */
char *vul_wchar_to_utf8_small( const vul_wchar *str );

//--------------------------------------------
// String searching / pattern matching.
//
/**
 * Search for a pattern in a string. Uses Knuth-Morris-Pratt.
 * Returns the zero-based index of the first character of the match,
 * or the length of the string if not found.
 */
size_t vul_string_search( const char *str, const char *pattern );
/**
* Search for a pattern in a string. Uses Knuth-Morris-Pratt.
* Returns the zero-based index of the first character of the match,
* or the length of the string if not found.
*/
size_t vul_wstring_search( const vul_wchar *str, const vul_wchar *pattern );
//-----------
// Helpers

static void vul__string_calculate_search_table( const char *pattern, 
                                                struct vul__string_search_table *table );
static void vul__wstring_calculate_search_table( const vul_wchar *pattern, 
                                                 struct vul__string_search_table *table );
static size_t vul__string_search( const char *str, const char *pattern, 
                                  size_t str_len, struct vul__string_search_table *table );
static size_t vul__wstring_search( const vul_wchar *str, const vul_wchar *pattern, 
                                   size_t str_len, struct vul__string_search_table *table );

//-----------------------------
// Useful substring functions
//

/**
 * Returns a pointer to the first character after the last occurance of divisor.
 */
char *vul_string_divide_get_last( const char *str, const char divisor );
/**
 * Returns a pointer to the first character after the last occurance of divisor.
 */
vul_wchar *vul_wstring_divide_get_last( const vul_wchar *str, const vul_wchar divisor );
/**
 * Returns a pointer to the last character before the frist occurance of divisor.
 */
char *vul_string_divide_get_first( const char *str, const char divisor );
/**
 * Returns a pointer to the first character after the last occurance of divisor.
 */
vul_wchar *vul_wstring_divide_get_first( const vul_wchar *str, const vul_wchar divisor );

#ifdef __cplusplus
}
#endif

#ifndef VUL_TYPES_H
#undef s32
#undef u32
#undef u8
#endif

#endif // VUL_STRING_H

#ifdef VUL_DEFINE

#ifndef VUL_TYPES_H
#define s32 int32_t
#define u32 uint32_t
#define u8 uint8_t
#endif

#ifdef __cplusplus
extern "C" {
#endif

vul_wchar *vul_wchar_from_utf8( vul_wchar *buffer, const char *ostr, s32 n )
{
   u8 *str = ( u8* ) ostr;
   u32 c;
   s32 i = 0;

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

char *vul_wchar_to_utf8( char *buffer, const vul_wchar *str, s32 n )
{
   s32 i = 0;

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

vul_wchar *vul_wchar_from_utf8_large( const char *str )
{
   static vul_wchar buffer[ 4096 ];
   return vul_wchar_from_utf8( buffer, str, 4096 );
}

vul_wchar *vul_wchar_from_utf8_small( const char *str )
{
   static vul_wchar buffer[ 64 ];
   return vul_wchar_from_utf8( buffer, str, 64 );
}

char *vul_wchar_to_utf8_large( const vul_wchar *str )
{
   static char buffer [ 4096 ];
   return vul_wchar_to_utf8( buffer, str, 4096 );
}

char *vul_wchar_to_utf8_small( const vul_wchar *str )
{
   static char buffer[ 64 ];
   return vul_wchar_to_utf8( buffer, str, 64 );
}

static void vul__string_calculate_search_table( const char *pattern, 
                                                struct vul__string_search_table *table )
{
   u32 pos;
   s32 cnd;

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

static void vul__wstring_calculate_search_table( const vul_wchar *pattern, 
                                                 struct vul__string_search_table *table )
{
   u32 pos;
   s32 cnd;

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

static size_t vul__string_search( const char *str, const char *pattern, 
                                  size_t str_len, struct vul__string_search_table *table )
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

static size_t vul__wstring_search( const vul_wchar *str, const vul_wchar *pattern, 
                                   size_t str_len, struct vul__string_search_table *table )
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

size_t vul_string_search( const char *str, const char *pattern )
{
   size_t ret, lenS;
   struct vul__string_search_table table;

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

size_t vul_wstring_search( const vul_wchar *str, const vul_wchar *pattern )
{
   size_t ret, lenS;
   struct vul__string_search_table table;

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

char *vul_string_divide_get_last( const char *str, const char divisor )
{
   size_t index;
   
   index = strlen( str ) - 1;
   
   while ( index > 0 && str[ index ] != divisor )
      --index;

   return ( char* )( str + index );
}

vul_wchar *vul_wstring_divide_get_last( const vul_wchar *str, const vul_wchar divisor )
{
   size_t index;
   
   index = wcslen( str ) - 1;
   
   while ( index > 0 && str[ index ] != divisor )
      --index;

   return ( vul_wchar* )( str + index );
}

char *vul_string_divide_get_first( const char *str, const char divisor )
{
   size_t index, len;
   
   len = strlen( str ) - 1;
   index = 0;
   
   while ( index < len && str[ index ] != divisor )
      ++index;

   return ( char* )( str + index );
} 
vul_wchar *vul_wstring_divide_get_first( const vul_wchar *str, const vul_wchar divisor )
{
   size_t index, len;
   
   len = wcslen( str ) - 1;
   index = 0;
   
   while ( index < len && str[ index ] != divisor )
      ++index;

   return ( vul_wchar* )( str + index );
}

#ifdef __cplusplus
}
#endif

#ifndef VUL_TYPES_H
#undef s32
#undef u32
#undef u8
#endif

#endif // VUL_DEFINE
