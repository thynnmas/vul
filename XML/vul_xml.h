/* Villains' Utility Library - Thomas Martin Schmid, 2015. Public domain*
 *
 * This library is a tiny XML parser. It was initially made for vul_svg
 * @TODO: -One markup cases is not yet handled:
 *			-&	;
			-Additionally, CDATA is untested, and I'm somewhat insecure about where CDATA should be handled.
 *		  -Unicode
 *		  -Comments
 *		  Or, just have a proepr look at http://www.w3.org/TR/xml/
 *		  and stop winging it...DEFINITELY do this BEFORE releasing this POC!
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

#ifndef VUL_XML
#define VUL_XML

// Define this in exactly one SVG file.
//#define VUL_DEFINE

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "vul_types.h"
#include "vul_graph.h"

#define vul__xml_is_char( c )		1
#define vul__xml_is_sp( c )			( c == 0x20 || c == 0x09 || c == 0x0a || c == 0x0d )
#define vul__xml_is_alpha( c )		( ( c | 0x20 ) - 'a' < 0x1a )
#define vul__xml_is_num( c )		( c - '0' < 0x0a )
#define vul__xml_is_hex( c )		( vul__xml_is_num( c ) || ( c | 0x20 ) - 'a' < 0x06 )
#define vul__xml_is_enc_name( c )	( vul__xml_is_alpha( c ) || vul__xml_is_num( c ) || c == '.' || c == '_' || c == '-' )
#define vul__xml_is_name_start( c )	( vul__xml_is_alpha( c ) || c == ':' || c == '_' || c >= 0x80 )
#define vul__xml_is_name( c )		( vul__xml_is_name_start( c ) || vul__xml_is_num( c ) || c == '-' || c == '.' )
#define vul__xml_is_attr_value( c )	( vul__xml_is_char( c ) && c != xml->quote && c != '<' && c != '&' )
/* @TODO: Add support for entities with '.', ':', etc, anwyay. */
#define vul__xml_is_ref(c)			( vul__xml_is_num( c ) || vul__xml_is_alpha( c ) || c == '#' )

#define vul__int_from_five_chars(a, b, c, d, e) ( ( ( ( ui64_t )( a ) ) << 32 )\
												| ( ( ( ui64_t )( b ) ) << 24 )\
												| ( ( ( ui64_t )( c ) ) << 16 )\
												| ( ( ( ui64_t )( d ) ) << 8)\
												|	  ( ui64_t )( e ) )


enum vul__xml_return_value {
	VUL_XML_UNEXPECTED_EOF		= -5,
	VUL_XML_INVALID_REFERENCE	= -4,
	VUL_XML_INVALID_CLOSE_TAG	= -3,
	VUL_XML_STACK_OVERFLOW		= -2,
	VUL_XML_SYNTAX_ERROR		= -1,
	VUL_XML_OK					=  0,
	VUL_XML_ELEMENT_START		=  1,
	VUL_XML_CONTENT				=  2,
	VUL_XML_ELEMENT_END			=  3,
	VUL_XML_ATTRIBUTE_START		=  4,
	VUL_XML_ATTRIBUTE_VALUE		=  5,
	VUL_XML_ATTRIBUTE_END		=  6,
	VUL_XML_PI_START			=  7,
	VUL_XML_PI_CONTENT			=  8,
	VUL_XML_PI_END				=  9
};

enum vul__xml_state {
	VUL__XML_STATE_INIT,
	VUL__XML_STATE_START_TAG,
	VUL__XML_STATE_ATTRIBUTE_NAME,
	VUL__XML_STATE_ATTRIBUTE_VALUE,
	VUL__XML_STATE_CONTENT,
	VUL__XML_STATE_CDATA,
	VUL__XML_STATE_END_TAG
	// @TODO: CDATA, more?
	// @TODO: apersands...
};

/**
 * Sets the destination character to the value of ch, where ch is in range [0, 255].
 * Have to do this like this because char = unsigned char is implementation defined
 * in C. Should be optimized down to a single move.
 */
#ifndef VUL_DEFINE
static inline void vul__xml_setchar( i8_t *dst, ui32_t ch );
#else
static inline void vul__xml_setchar( i8_t *dst, ui32_t ch ) {
	ui8_t _ch = ch;
	memcpy( dst, &_ch, sizeof( ui8_t ) );
}
#endif

/**
 * Converts any unicode point ( in ch ) to utf8 and writes it into
 * dst, which must have room for at least 5 characters. Writes a
 * null byte at the end of dst.
 */
#ifndef VUL_DEFINE
static void vul__xml_setutf8( i8_t *dst, ui32_t ch);
#else
static void vul__xml_setutf8( i8_t *dst, ui32_t ch)
{
	if( ch <= 0x007f ) {
		vul__xml_setchar( dst++, ch);
	} else if( ch <= 0x07ff ) {
		vul__xml_setchar( dst++, 0xC0 | ( ch >> 6 ) );
		vul__xml_setchar( dst++, 0x80 | ( ch & 0x3f ) );
	} else if( ch <= 0xffff ) {
		vul__xml_setchar( dst++, 0xE0 | ( ch >> 12 ) );
		vul__xml_setchar( dst++, 0x80 | ( ( ch >> 6 ) & 0x3f ) );
		vul__xml_setchar( dst++, 0x80 | ( ch & 0x3f ) );
	} else {
		vul__xml_setchar( dst++, 0xF0 | ( ch >> 18 ) );
		vul__xml_setchar( dst++, 0x80 | ( ( ch >> 12 ) & 0x3f ) );
		vul__xml_setchar( dst++, 0x80 | ( ( ch >> 6 ) & 0x3f ) );
		vul__xml_setchar( dst++, 0x80 | ( ch & 0x3f ) );
	}
	*dst = 0;
}
#endif

enum vul_xml_node_type {
	VUL_XML_NODE_TYPE_DOCUMENT,
	VUL_XML_NODE_TYPE_ELEMENT_NAME,
	VUL_XML_NODE_TYPE_ELEMENT_CONTENT,
	VUL_XML_NODE_TYPE_ELEMENT_CDATA,
	VUL_XML_NODE_TYPE_ATTRIBUTE_NAME,
	VUL_XML_NODE_TYPE_ATTRIBUTE_VALUE
};

struct vul_xml_node_content {
	vul_xml_node_type type;
	char *string;
	ui32_t strlen;
};

/**
 * Parses the document given as a null-terminated string
 * and fills the tree of the resulting objects into the root node
 * given, which is expected to be empty.
 */
#ifndef VUL_DEFINE
int vul_xml_parse( vul_node_t *root, const char *document );
#else
int vul_xml_parse( vul_node_t *root, const char *document )
{
	const char *c, *start;
	vul_node_t *n;
	vul__xml_state state;
	ui32_t i;
	
	// Copy content to root node
	vul_xml_node_content content;
	content.string = NULL;
	content.strlen = 0;
	content.type = VUL_XML_NODE_TYPE_DOCUMENT;
	root->content = malloc( sizeof( vul_xml_node_content ) );
	memcpy_s( root->content, sizeof( vul_xml_node_content ), &content, sizeof( vul_xml_node_content ) );

	// Our working node starts at root.
	n = root;
	state = VUL__XML_STATE_INIT;

	// Parse each character and fill the vector
	char* end = (char*)(strlen( document ) + document);
	for( c = document; *c; ++c )
	{
		switch( state ) {
		case VUL__XML_STATE_INIT:
			if( *c == '<' ) {
				if( *( c + sizeof( char ) ) == '?' ) {
					++c;
				}
				state = VUL__XML_STATE_START_TAG;
				start = ( c + sizeof( char ) );
			}
			break;
		case VUL__XML_STATE_START_TAG:
			if( *c == '/' || *c == '?' ) {
				if( *(++c) == '>' ) {
					// Tag ended, but is empty
					content.string = ( char* )start;
					content.strlen = ( i32_t )c - ( i32_t )start;
					content.type = VUL_XML_NODE_TYPE_ELEMENT_NAME;
					// Add the child Stay at same level though, since it is empty.
					vul_graph_insert( n, &content, sizeof( vul_xml_node_content ) );
					// Store the new start character, but don't advance yet.
					start = ( c + sizeof( char ) );
					// Update state
					state = VUL__XML_STATE_CONTENT;
				} else {
					--c;
				}
			} else if( *c == '>' ) {
				// Tag ended
				content.string = ( char* )start;
				content.strlen = ( i32_t )c - ( i32_t )start;
				content.type = VUL_XML_NODE_TYPE_ELEMENT_NAME;
				// Add the child, move one step deeper into the tree
				n = vul_graph_insert( n, &content, sizeof( vul_xml_node_content ) );
				// Store the new start character, but don't advance yet.
				start = ( c + sizeof( char ) );
				// Update state
				state = VUL__XML_STATE_CONTENT;
			} else if ( vul__xml_is_sp( *c ) ) {
				// If a space, we have spotted an attribute
				// First store the tag name
				content.string = ( char* )start;
				content.strlen = ( i32_t )c - ( i32_t )start;
				content.type = VUL_XML_NODE_TYPE_ELEMENT_NAME;
				// Add the child, move one step deeper into the tree
				n = vul_graph_insert( n, &content, sizeof( vul_xml_node_content ) );
				// Advance past the whitespace
				while( *c && vul__xml_is_sp( *c ) ) {
					++c;	
				};
				start = c;
				// Update state
				state = VUL__XML_STATE_ATTRIBUTE_NAME;
			}
			break;
		case VUL__XML_STATE_END_TAG:
			i = 0;
			while( *c != '>' ) {
				// At this point n is an ELEMENT_NAME node
				if( *(c++) != ( ( vul_xml_node_content* )n->content )->string[ i++ ] ) {
					// This is an error; the opening and closing tags don't match!
					// @TODO: Handle this error
					return -1;
				}
			}
			n = n->parent;
			state = VUL__XML_STATE_CONTENT;
			start = ( char* )( c + sizeof( char ) );
			break;
		case VUL__XML_STATE_ATTRIBUTE_NAME:
			if( *c == '=' ) {
				// Attribute name ended
				content.string = ( char* )start;
				content.strlen = ( i32_t )c - ( i32_t )start;
				content.type = VUL_XML_NODE_TYPE_ATTRIBUTE_NAME;
				// Add the child, move one step deeper into the tree
				n = vul_graph_insert( n, &content, sizeof( vul_xml_node_content ) );
				// Advance past the = and quote
				while( *c && *(c++) == '=' ) 
					;
				while( *c && ( *c == '\'' || *c == '"' ) ) {
					++c;
				}
				start = c;
				// Update state
				state = VUL__XML_STATE_ATTRIBUTE_VALUE;
			} 
			break;
		case VUL__XML_STATE_ATTRIBUTE_VALUE:
			if( *c == '\'' || *c == '"' ) {
				// Attribute value ended
				content.string = ( char* )start;
				content.strlen = ( i32_t )c - ( i32_t )start;
				content.type = VUL_XML_NODE_TYPE_ATTRIBUTE_VALUE;
				// Add the child and move back up to the nearest element
				vul_graph_insert( n, &content, sizeof( vul_xml_node_content ) );
				n = n->parent;	// We know we have an ELEMENT_NAME->ATTRIBUTE_NAME chain
				// Update state
				state = VUL__XML_STATE_START_TAG;
				// Advance start past the quote
				while( *c && ( *c == '\'' || *c == '"' ) ) {
					++c;
				}
				start = c;
				// Now, if c is >, this is the end of the tag and we 
				// should move to content, otherwise we are back at start tag.
				if( *c == '>' ) {
					state = VUL__XML_STATE_CONTENT;
					start = ( char* )(c + sizeof( char ) );
				} else if( *c == '/' || *c == '?' ) {
					if( *( c + sizeof( char ) ) == '>' ) {
						// Empty tags and meta tags are also ended here, if encountered.
						state = VUL__XML_STATE_CONTENT;
						start = ( char* )(c + 2 * sizeof( char ) );
					}
				}
			} 
			break;
		case VUL__XML_STATE_CONTENT:
			if( *c == '<' ) {
				// Content ended
				content.string = ( char* )start;
				content.strlen = ( i32_t )c - ( i32_t )start;
				content.type = VUL_XML_NODE_TYPE_ELEMENT_CONTENT;
				if( content.strlen > 0 ) {
					// Add the child
					vul_graph_insert( n, &content, sizeof( vul_xml_node_content ) );
				}				
				if(	   *( c +	  sizeof( char ) )	== '!'
					&& *( c + 2 * sizeof( char ) )	== '['
					&& *( c + 3 * sizeof( char ) )	== 'C'
					&& *( c + 4 * sizeof( char ) )	== 'D'
					&& *( c + 5 * sizeof( char ) )	== 'A'
					&& *( c + 6 * sizeof( char ) )	== 'T'
					&& *( c + 7 * sizeof( char ) )	== 'A'
					&& *( c + 8 * sizeof( char ) )	== '[' ) {
					// CDATA section
					state = VUL__XML_STATE_CDATA;
					start = ( char* )( c + 9 * sizeof( char ) );
					c = ( char* )( start - sizeof( char ) );
				} else {
					// End of content section
					if( *(++c) == '/' ) {
						state = VUL__XML_STATE_END_TAG;
						start = ++c;
					} else {
						state = VUL__XML_STATE_START_TAG;
						start = c;
					}
					--c;
				}
			}
			break;
		case VUL__XML_STATE_CDATA:
			if( *c								== ']'
				&& *( c +	  sizeof( char ) )	== ']'
				&& *( c + 2 * sizeof( char ) )	== '>' ) {
				// End of CDATA section
				content.string = ( char* )start;
				content.strlen = ( i32_t )c - ( i32_t )start;
				content.type = VUL_XML_NODE_TYPE_ELEMENT_CDATA;
				if( content.strlen > 0 ) {
					// Add the child
					vul_graph_insert( n, &content, sizeof( vul_xml_node_content ) );
				}
				// Return to content section
				state = VUL__XML_STATE_CONTENT;
				c += sizeof( char ) * 3;
			}
			break;
		}
	}
	return 0;
}
#endif


/**
 * Helper function that can be passed to vul_graph_bfs to print a parse tree.
 */
#ifndef VUL_DEFINE
void vul_xml_print_node( vul_node_t *node );
#else
void vul_xml_print_node( vul_node_t *node )
{
	vul_xml_node_content *c = ( vul_xml_node_content* )node->content;
	char *str = ( char* )malloc( sizeof( char ) * c->strlen );
	assert( str );
	for( ui32_t i = 0; i < c->strlen; ++i ) {
		str[ i ] = c->string[ i ];
	}
	str[ c->strlen ] = 0;
	switch( c->type )
	{
	case VUL_XML_NODE_TYPE_DOCUMENT:
		printf( "<DOCUMENT: '%s' >\n", str );
		break;
	case VUL_XML_NODE_TYPE_ELEMENT_NAME:
		printf( "<ELEMENT NAME: '%s' >\n", str );
		break;
	case VUL_XML_NODE_TYPE_ELEMENT_CONTENT:
		printf( "<ELEMENT CONTENT: '%s' >\n", str );
		break;
	case VUL_XML_NODE_TYPE_ATTRIBUTE_NAME:
		printf( "<ATTRIBUTE NAME: '%s' >\n", str );
		break;
	case VUL_XML_NODE_TYPE_ATTRIBUTE_VALUE:
		printf( "<ATTRIBUTE CONTENT: '%s' >\n", str );
		break;
	}
}
#endif

#endif