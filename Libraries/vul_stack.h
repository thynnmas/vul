/*
* Villains' Utility Library - Thomas Martin Schmid, 2017. Public domain?
*
* This file describes a thin wrapper around the stable array
* providing stack behavior.
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
#ifndef VUL_STACK_H
#define VUL_STACK_H

#include <stdlib.h>
#include <string.h>

#include "vul_types.h"
#ifndef VUL_STABLE_RESIZEABLE_ARRAY_H
#include "vul_stable_array.h"
#endif

/**
* If defined, the functions are defined and not just declared. Only do this in _one_ c/cpp file!
*/
//#define VUL_DEFINE

typedef vul_svector vul_stack;

#define vul_stack_create vul_svector_create
#define vul_stack_destroy vul_svector_destroy
#define vul_stack_push vul_svector_append
#define vul_stack_size vul_svector_size
#define vul_stack_is_empty vul_svector_is_empty
#define vul_stack_clear vul_svector_freemem

#ifdef __cplusplus
extern "C" {
#endif
/**
 * Removes the last element from the stack and returns a copy of it in out
 */
void vul_stack_pop( vul_stack *stack, void *out );
/**
* Returns the top element of the stack without altering the stack itself.
*/
void *vul_stack_peek( vul_stack *stack );
#ifdef __cplusplus
}
#endif
#endif

#ifdef VUL_DEFINE

#ifdef __cplusplus
extern "C" {
#endif

void vul_stack_pop( vul_stack *stack, void *out )
{
	u32 idx = vul_stack_size( stack ) - 1;
	void *ret = vul_svector_get( stack, idx );
	memcpy( out, ret, stack->element_size );
	vul_svector_remove_swap( stack, idx );
}

void *vul_stack_peek( vul_stack *stack )
{
	return vul_svector_get( stack, vul_stack_size( stack ) - 1 );
}

#ifdef __cplusplus
}
#endif

#endif
