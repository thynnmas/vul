/* Villains' Utility Library - Thomas Martin Schmid, 2015. Public domain¹
 *
 * This library renders SVG files to buffers. Allows for multipsampling.
 * Parses the file with vul_xml, then renders it.
 * See the SVG specification for more info: http://www.w3.org/TR/SVG11/Overview.html
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

#ifndef VUL_SVG
#define VUL_SVG

// Define this in exactly one SVG file.
//#define VUL_DEFINE_SVG

#include "vul_types.h"

struct vul_svg_buffer {
	ui8_t *buffer;
	ui32_t size;
};

#ifndef VUL_DEFINE_SVG
int vul_svg_render( vul_svg_buffer *dst, const char *file );
#else
int vul_svg_render( vul_svg_buffer *dst, const char *file )
{
	// While not done
		// Parse the xml for next element

		// Switch on element type
			// Call vul__svg_draw_[element_type]( dst, element );
}
#endif

#endif