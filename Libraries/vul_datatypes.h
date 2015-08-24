/*
 * Villains' Utility Library - Thomas Martin Schmid, 2015. Public domain¹
 *
 * This file includes the various datatypes of vul. At the moment, this includes:
 *	-General graph
 *	-Linked list (doubly linked)
 *	-Skip list
 *	-Queue
 *	-Resizable array (vector)
 *	-Hash map (albeit without hash functions)
 * While not including:
 *	-Fibonacci heap (@TODO)
 *	-Simpler heap (@TODO)
 *	-Stack; since the vector can be used for this with no extra work.
 * As of now proper iteration is not supported. This is however on my todo list, see:
 * @TODO: Proper iterators for all data types, see http://nothings.org/computer/iterate.html
 *		  Might want to write fast, simple iterators as well, expose them as const_iterate
 *		  and require that the underlying data structure is unaltered by the iteration.
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

#include "vul_linked_list.h"
#include "vul_skip_list.h"
#include "vul_queue.h"
#include "vul_resizable_array.h"
#include "vul_stable_array.h"
#include "vul_hash_map.h"
#include "vul_stack.h"
#include "vul_priority_heap.h"