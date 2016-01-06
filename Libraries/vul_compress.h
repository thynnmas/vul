/*
 * Villains' Utility Library - Thomas Martin Schmid, 2016. Public domain¹
 *
 * This file describes several simple lossless compression algorithms,
 * with matchin decompression versions.
 *
 * Implemented are:
 *  - Plain run-length encoding
 * On the todo list:
 *  - @TODO: LZ77 - http://nothings.org/stb/stb_compress.txt
 *	- @TODO: LZ77 for floats:
 *           Copy sign bit regardless, do LZ77 on exponent and mantissa separately instead
 *           of treating everything as chars. Should get good compression on exponent at least.
 *           Might also want to look at huffman trees for this.
 * Maybe, someday, if I need it and find the time:
 *  - @TODO: LZX - http://xavprods.free.fr/lzx/
 *	- @TODO: LZMA - https://en.wikipedia.org/wiki/Lempel-Ziv-Markov_chain_algorithm
 *
 * All algorithms have two versions, one which allocates memory itself (usign malloc/realloc)
 * and one which takes a buffer as input and errs if it is too small.
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

#ifndef VUL_COMPRESS_H
#define VUL_COMPRESS_H

#include <stdlib.h>
#ifndef VUL_OSX
#include <malloc.h>
#endif
#include <assert.h>
#include <string.h>

#include "vul_types.h"
/**
 * If defined, the functions are defined and not just declared. Only do this in _one_ c/cpp file!
 */
//#define VUL_DEFINE

// Returns the number of bytes needed to encode the number in our RLE style
#define vul__compress_rle_char_count_needed( n ) ( n < 0x80		? 1 : \
												   n < 0x4000	? 2 : \
												   n < 0x200000 ? 3 : \
												   n < 0x8000000? 4 : 5 )

/**
 * Helper function of vul_compress_rle. Encodes the given number in a series
 * of 7-bit numbers where the 8th bit (most significant) indicates if another number
 * follows.
 */
#ifndef VUL_DEFINE
void vul__compress_rle_encode_number( ui8_t *buf, ui32_t num );
#else
void vul__compress_rle_encode_number( ui8_t *buf, ui32_t num )
{
	if( num < 0x80 ) {
		buf[ 0 ] = ( ui8_t )( num & 0x7f );
	} else if( num < 0x4000 ) {
		buf[ 1 ] = ( ui8_t )( ( num >> 7 ) & 0x7f ) | ( ui8_t )0x80;
		buf[ 0 ] = ( ui8_t )( num & 0x7f );
	} else if( num < 0x200000 ) {
		buf[ 2 ] = ( ui8_t )( ( num >> 14 ) & 0x7f ) | ( ui8_t )0x80;
		buf[ 1 ] = ( ui8_t )( ( num >> 7 ) & 0x7f ) | ( ui8_t )0x80;
		buf[ 0 ] = ( ui8_t )( num & 0x7f );
	} else if( num < 0x8000000 ) {
		buf[ 3 ] = ( ui8_t )( ( num >> 21 ) & 0x7f ) | ( ui8_t )0x80;
		buf[ 2 ] = ( ui8_t )( ( num >> 14 ) & 0x7f ) | ( ui8_t )0x80;
		buf[ 1 ] = ( ui8_t )( ( num >> 7 ) & 0x7f ) | ( ui8_t )0x80;
		buf[ 0 ] = ( ui8_t )( num & 0x7f );
	} else {
		buf[ 4 ] = ( ui8_t )( ( num >> 28 ) & 0x7f ) | ( ui8_t )0x80;
		buf[ 3 ] = ( ui8_t )( ( num >> 21 ) & 0x7f ) | ( ui8_t )0x80;
		buf[ 2 ] = ( ui8_t )( ( num >> 14 ) & 0x7f ) | ( ui8_t )0x80;
		buf[ 1 ] = ( ui8_t )( ( num >> 7 ) & 0x7f ) | ( ui8_t )0x80;
		buf[ 0 ] = ( ui8_t )( num & 0x7f );
	}
}
#endif

/**
 * Helper function of vul_decompress_rle. Decodes the number in our RLE
 * format inside the given buffer.
 */
#ifndef VUL_DEFINE
ui32_t vul__compress_rle_decode_number( const ui8_t *buf );
#else
ui32_t vul__compress_rle_decode_number( const ui8_t *buf )
{
	ui32_t i, n;
	
	n = 0;
	for( i = 0; i < 5; ++i )
	{
		n += ( ui32_t )( buf[ i ] & 0x7f );
		if( !( buf[ i ] & 0x80 ) ){
			break;
		}
	}

	return n;
}
#endif

/**
 * Run Length encoding. The length of a sequence is encoded in a series
 * of 8-bit numbers, where the most-significant bit is set only if another
 * number follows. Length of the source buffer is currently limited by 32-bit numbers.
 * Dynamically allocates (malloc) the destination buffer and tightens it before returning.
 * The size of the returned buffer is written to the parameter dst_size.
 */
#ifndef VUL_DEFINE
const ui8_t *vul_compress_rle( const ui8_t *src, ui32_t src_size, ui32_t *dst_size );
#else
const ui8_t *vul_compress_rle( const ui8_t *src, ui32_t src_size, ui32_t *dst_size )
{
	ui8_t *dst;
	ui8_t last;
	ui32_t local_count, total_count, dst_index, bytes;
	
	assert( src != NULL );
	assert( src_size != 0 );

	*dst_size = src_size / 16; // Somewhat optimistic first size estimate
	dst = ( ui8_t* )malloc( *dst_size );
	assert( dst != NULL ); // Make sure malloc doesn't fail

	total_count = 0;
	local_count = 1;
	last = src[ 0 ];
	dst_index = 0;
	while( ++total_count < src_size )
	{
		if( src[ total_count ] != last ) {
			// Run ended, write to dest
			bytes = vul__compress_rle_char_count_needed( local_count );
			if ( dst_index + bytes + 1 > *dst_size ) {
				*dst_size *= 2;	// Less optimistic, but we'll shrink it after, so rather go high here.
				dst = ( ui8_t* )realloc( dst, *dst_size );
				assert( dst != NULL ); // Make sure realloc doesn't fail
			}
			vul__compress_rle_encode_number( &dst[ dst_index ], local_count );
			dst_index += bytes;
			dst[ dst_index++ ] = last;
			local_count = 1;
			last = src[ total_count ];
		} else {
			++local_count;
		}
	}
	// Write last run
	// Run ended, write to dest
	bytes = vul__compress_rle_char_count_needed( local_count );
	if ( dst_index + bytes + 1 > *dst_size ) {
		*dst_size *= 2;	// Less optimistic, but we'll shrink it after, so rather go high here.
		dst = ( ui8_t* )realloc( dst, *dst_size );
		assert( dst != NULL ); // Make sure realloc doesn't fail
	}
	vul__compress_rle_encode_number( &dst[ dst_index ], local_count );
	dst_index += bytes;
	dst[ dst_index++ ] = last;
	dst[ dst_index++ ] = 0;

	// Tighten the dst-buffer
	dst = ( ui8_t* )realloc( dst, dst_index );
	*dst_size = dst_index;

	return dst;
}
#endif


/**
 * Run Length encoding. The length of a sequence is encoded in a series
 * of 8-bit numbers, where the most-significant bit is set only if another
 * number follows. Length of the source buffer is currently limited by 32-bit numbers.
 * Takes the destination buffer as an input. If it is found too small, an assert will fail.
 */
#ifndef VUL_DEFINE
void vul_compress_rle_inplace( ui8_t *dst, ui32_t dst_size, const ui8_t *src, ui32_t src_size );
#else
void vul_compress_rle_inplace( ui8_t *dst, ui32_t dst_size, const ui8_t *src, ui32_t src_size )
{
	ui8_t last;
	ui32_t local_count, total_count, dst_index, bytes;
	
	assert( src != NULL );
	assert( src_size != 0 );
	assert( dst != NULL );
	assert( dst_size != 0 );

	total_count = 0;
	local_count = 0;
	last = src[ 0 ];
	dst_index = 0;
	while( ++total_count < src_size )
	{
		if( src[ total_count ] != last ) {
			// Run ended, write to dest
			bytes = vul__compress_rle_char_count_needed( local_count );
			assert( dst_index + bytes + 1 < dst_size ); // Err if destination buffer is too small
			vul__compress_rle_encode_number( &dst[ dst_index ], local_count );
			dst_index += bytes;
			dst[ dst_index++ ] = last;
			local_count = 1;
			last = src[ total_count ];
		} else {
			++local_count;
		}
	}
	// Write last run
	// Run ended, write to dest
	bytes = vul__compress_rle_char_count_needed( local_count );
	assert( dst_index + bytes + 1 < dst_size ); // Err if destination buffer is too small
	vul__compress_rle_encode_number( &dst[ dst_index ], local_count );
	dst_index += bytes;
	dst[ dst_index++ ] = last;
	dst[ dst_index ] = 0;
}
#endif


/**
 * Run Length decoding.
 * Dynamically allocates (malloc) the destination buffer and tightens it before returning.
 * The size of the returned buffer is written to the parameter dst_size.
 */
#ifndef VUL_DEFINE
const ui8_t *vul_decompress_rle( const ui8_t *src, ui32_t src_size, ui32_t *dst_size );
#else
const ui8_t *vul_decompress_rle( const ui8_t *src, ui32_t src_size, ui32_t *dst_size )
{
	ui8_t *dst;
	ui32_t local_count, total_count, dst_index, i;
	
	assert( src != NULL );
	assert( src_size != 0 );

	*dst_size = src_size * 16; // Somewhat pessimistic first size estimate
	dst = ( ui8_t* )malloc( *dst_size );
	assert( dst != NULL ); // Make sure malloc doesn't fail

	total_count = 0;
	local_count = 0;
	dst_index = 0;
	while( total_count < src_size )
	{
		local_count = vul__compress_rle_decode_number( &src[ total_count ] );
		total_count += vul__compress_rle_char_count_needed( local_count );
		if( dst_index + local_count > *dst_size ) {
			*dst_size *= 2; // Pessimistic, but we tighten after, so _should_ be fine.
			dst = ( ui8_t* )realloc( dst, *dst_size );
			assert( dst != NULL ); // Make sure realloc doesn't fail
		}
		for( i = 0; i < local_count; ++i )
		{
			dst[ dst_index++ ] = src[ total_count ];
		}
		++total_count;
	}
	dst[ dst_index++ ] = 0;
	*dst_size = dst_index;

	// Tighten the dst-buffer
	dst = ( ui8_t* )realloc( dst, dst_index );


	return dst;
}
#endif

/**
 * Run Length decoding.
 * Takes the destination buffer as an argument. If it is found too small an assert will fail.
 */
#ifndef VUL_DEFINE
void vul_decompress_rle_inplace( ui8_t *dst, ui32_t dst_size, const ui8_t *src, ui32_t src_size );
#else
void vul_decompress_rle_inplace( ui8_t *dst, ui32_t dst_size, const ui8_t *src, ui32_t src_size )
{
	ui32_t local_count, total_count, dst_index, i;
	
	assert( src != NULL );
	assert( src_size != 0 );
	assert( dst != NULL );
	assert( dst_size != 0 );

	total_count = 0;
	local_count = 0;
	dst_index = 0;
	while( total_count < src_size )
	{
		local_count = vul__compress_rle_decode_number( &src[ total_count ] );
		total_count += vul__compress_rle_char_count_needed( local_count );
		assert( dst_index + local_count < dst_size ); // This errs if the destination buffer is too small
		for( i = 0; i < local_count; ++i )
		{
			dst[ dst_index++ ] = src[ total_count ];
		}
		++total_count;
	}
	dst[ dst_index ] = 0;
}
#endif

#endif
