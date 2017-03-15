/*
 * Villains' Utility Library - Thomas Martin Schmid, 2017. Public domain¹
 *
 * This file contains an "exploding comment". It is intended to replace
 * comments of this style:
 *  T0DO: Replace me before ship
 *  H4CK for [event]: F1x afterwards!
 * The exploding comment takes a date as an argument, and when that date is reached
 * it asserts; that way you either need to bump the date or tend to the actual
 * problem. It only asserts if VUL_DEBUG is specified, to avoid forgetting these
 * and having something blow up in production or during demos.
 *
 * This REQUIRES C++11 (constexpr) to work, and the month comparison is quite
 * slow/costly. If anyone has a nice hash function f(3-character-english-month)->integer
 * that orderes months, I'd LOVE to hear about it!
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
#ifndef VUL_EXPLODING_COMMENT_H
#define VUL_EXPLODING_COMMENT_H

#ifdef VUL_DEBUG
   constexpr int vul__exploding_comment_strcmp( const char* l, const char *r )
   {
      return ( ( '\0' == l[0] ) && ( '\0' == r[0] ) ) ? 0
           : ( l[0] != r[0] ) ? ( l[0] - r[0] )
           : vul__exploding_comment_strcmp( l + 1, r + 1 );
   }
   
   constexpr int vul__exploding_comment_strcmp_fixed_length( const char* l, const char *r, int len )
   {
      return ( len == 0 || ( ( '\0' == l[0] ) && ( '\0' == r[0] ) ) ) ? 0
           : ( l[0] != r[0] ) ? ( l[0] - r[0] )
           : vul__exploding_comment_strcmp_fixed_length( l + 1, r + 1, len - 1 );
   }
   
   constexpr int vul__exploding_comment_month_compare( const char* l, const char *r )
   {
      return ( vul__exploding_comment_strcmp_fixed_length( l, "Jan", 3 ) == 0 ? 0
             : vul__exploding_comment_strcmp_fixed_length( l, "Feb", 3 ) == 0 ? 1
             : vul__exploding_comment_strcmp_fixed_length( l, "Mar", 3 ) == 0 ? 2
             : vul__exploding_comment_strcmp_fixed_length( l, "Apr", 3 ) == 0 ? 3
             : vul__exploding_comment_strcmp_fixed_length( l, "May", 3 ) == 0 ? 4
             : vul__exploding_comment_strcmp_fixed_length( l, "Jun", 3 ) == 0 ? 5
             : vul__exploding_comment_strcmp_fixed_length( l, "Jul", 3 ) == 0 ? 6
             : vul__exploding_comment_strcmp_fixed_length( l, "Aug", 3 ) == 0 ? 7
             : vul__exploding_comment_strcmp_fixed_length( l, "Sep", 3 ) == 0 ? 8
             : vul__exploding_comment_strcmp_fixed_length( l, "Oct", 3 ) == 0 ? 9
             : vul__exploding_comment_strcmp_fixed_length( l, "Nov", 3 ) == 0 ? 10 : 11 )
           - ( vul__exploding_comment_strcmp_fixed_length( r, "Jan", 3 ) == 0 ? 0
             : vul__exploding_comment_strcmp_fixed_length( r, "Feb", 3 ) == 0 ? 1
             : vul__exploding_comment_strcmp_fixed_length( r, "Mar", 3 ) == 0 ? 2
             : vul__exploding_comment_strcmp_fixed_length( r, "Apr", 3 ) == 0 ? 3
             : vul__exploding_comment_strcmp_fixed_length( r, "May", 3 ) == 0 ? 4
             : vul__exploding_comment_strcmp_fixed_length( r, "Jun", 3 ) == 0 ? 5
             : vul__exploding_comment_strcmp_fixed_length( r, "Jul", 3 ) == 0 ? 6
             : vul__exploding_comment_strcmp_fixed_length( r, "Aug", 3 ) == 0 ? 7
             : vul__exploding_comment_strcmp_fixed_length( r, "Sep", 3 ) == 0 ? 8
             : vul__exploding_comment_strcmp_fixed_length( r, "Oct", 3 ) == 0 ? 9
             : vul__exploding_comment_strcmp_fixed_length( r, "Nov", 3 ) == 0 ? 10 : 11 );
   }

   constexpr int vul__exploding_comment_date_compare( const char* then, const char* now)
   {
      // First compare year, then month, then day
      return vul__exploding_comment_strcmp_fixed_length(&then[7], &now[7], 4) != 0
           ? vul__exploding_comment_strcmp_fixed_length(&then[7], &now[7], 4)
           : vul__exploding_comment_month_compare(&then[0], &now[0]) != 0
           ? vul__exploding_comment_month_compare(&then[0], &now[0])
           : vul__exploding_comment_strcmp_fixed_length(&then[4], &now[4], 2);

   }

   constexpr int testing(const char *a, const char *b) {
      return *a - *b;
   }
   #define VUL_EXPLODING_COMMENT(date, text) static_assert(vul__exploding_comment_date_compare(date, __DATE__) >= 0,\
                                                           "Exploding comment timed out: '" text "'.")
   #undef VUL__MACRO_CONCAT
#else
   #define VUL_EXPLODING_COMMENT(date, text) { void; }
#endif

#endif
