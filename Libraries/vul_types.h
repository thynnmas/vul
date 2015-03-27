/*
 * Villains' Utility Library - Thomas Martin Schmid, 2015. Public domain¹
 *
 * This file defines shorter type names for the common math types in VUL.
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

#if !defined( VUL_TYPES_H ) && !defined( VUL_TYPES_HPP )
#define VUL_TYPES_H

#include <stdint.h>

// Floats
typedef float f32_t;
typedef double f64_t;

// Signed Integers
typedef int8_t i8_t;
typedef int16_t i16_t;
typedef int32_t i32_t;
typedef int64_t i64_t;

// Unsigned integers
typedef uint8_t ui8_t;
typedef uint16_t ui16_t;
typedef uint32_t ui32_t;
typedef uint64_t ui64_t;

typedef ui32_t bool32_t;
#define VUL_TRUE 1
#define VUL_FALSE 0

#endif