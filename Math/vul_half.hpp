/*
 * Villains' Utility Library - Thomas Martin Schmid, 2016. Public domain¹
 *
 * This file contains an implementation of IEEE 754 half precision flaoting point.
 * It also contains interfaces to the fixed point types of vul_fixed.h
 *
 * For bulk conversion VUL_HALF_F16C can be defined to do conversions in 4-wide 
 * AVX-compatible F16C, or VUL_HALF_SSE for 4-wide SSE2.
 *
 * @NOTE(thynn): VUL_HALF_F16C is untested, as I have a "Sandy Bridge" processor
 * and F16C is "Ivy Bridge" and later.
 *
 * By default we round towards +inf. If you want to round towards nearest even
 * value, define VUL_HALF_ROUND_NEAREST_EVEN.
 * 
 * @Thanks(thynn): The conversion code in this file is (mostly) from here:
 * https://gist.github.com/rygorous/2156668 courtesy of Fabian Giessen (@rygorous)
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

#ifndef VUL_HALF_HPP
#define VUL_HALF_HPP

#include <algorithm>
#include <limits>
#include <climits>
#include <cmath>

#ifdef VUL_HALF_F16C
	#include <immintrin.h>
#elif defined( VUL_HALF_SSE )
	#include <emmintrin.h>
#endif

namespace vul {
	
	template< int Q >
	struct fixed_32;

	//----------------
	// Declarations
	//
	
	struct half {
		unsigned short data;

#ifdef VUL_CPLUSPLUS11
		half( );
#else
		half( );	
#endif
		explicit half( float a );
		explicit half( double a );
		template< int Q >
		explicit half( fixed_32< Q > a );
		operator float( ) const;
		operator double( ) const;
		template< int Q >
		operator fixed_32< Q >( ) const;

		half& operator=( half rhs );
		half& operator+=( half rhs );
		half& operator-=( half rhs );
		half& operator*=( half rhs );
		half& operator/=( half rhs );

		half& operator=( float rhs );
		half& operator+=( float rhs );
		half& operator-=( float rhs );
		half& operator*=( float rhs );
		half& operator/=( float rhs );
		
		template< int Q >
		half& operator=( fixed_32< Q > rhs );
		template< int Q >
		half& operator+=( fixed_32< Q > rhs );
		template< int Q >
		half& operator-=( fixed_32< Q > rhs );
		template< int Q >
		half& operator*=( fixed_32< Q > rhs );
		template< int Q >
		half& operator/=( fixed_32< Q > rhs );

		half& operator++( );
		half& operator--( );
		half operator++( int );
		half operator--( int );
	};

	// Non-member functions
	bool operator==( half a, half b );
	bool operator!=( half a, half b );
	bool operator>=( half a, half b );
	bool operator<=( half a, half b );
	bool operator>( half a, half b );
	bool operator<( half a, half b );

	half operator+( half a, half b );
	half operator-( half a, half b );
	half operator*( half a, half b );
	half operator/( half a, half b );

	half operator+( half a );
	half operator-( half a );
	
	// Mass conversion functions. These use 4-wide SSE if VUL_HALF_SSE are defined.
	void vul_single_to_half_array( half *out, float *in, unsigned int count );
	void vul_half_to_single_array( float *out, half *in, unsigned int count );
	
	void vul_double_to_half_array( half *out, double *in, unsigned int count );
	void vul_half_to_double_array( double *out, half *in, unsigned int count );

	//---------------------------
	// Definitions
	//
#ifdef VUL_DEFINE
#ifdef VUL_CPLUSPLUS11
	half::half( ) : data( 0 )
	{
	}
#else
	half::half( )
	{
		data = 0;
	}
#endif
	half::half( float a )
	{
		union F32 {
			unsigned int u;
			float f;
		};
		F32 f; f.f = a;
		F32 f32infty = { 255 << 23 };
#ifdef VUL_HALF_ROUND_NEAREST_EVEN
		F32 f16max = { (127 + 16) << 23 };
		F32 denorm_magic = { ( ( 127 - 15 ) + ( 23 - 10 ) + 1 ) << 23 };
#else // Round towards +inf
		F32 f16infty = { 31 << 23 };
		F32 magic = { 15 << 23 };
#endif

		unsigned int sign_mask = 0x80000000u;
		unsigned int round_mask = ~0xfffu;
		data = 0;
		unsigned int sign = f.u & sign_mask;
		f.u ^= sign;

#ifdef VUL_HALF_ROUND_NEAREST_EVEN
		if( f.u >= f16max.u ) {
			data = ( f.u > f32infty.u ) ? 0x7e00 : 0x7c00;
		} else {
			if( f.u < ( 113 << 23 ) ) {
				f.f += denorm_magic.f;
				data = f.u - denorm_magic.u;
			} else {
				unsigned int mant_odd = ( f.u >> 13 ) & 1;
				f.u += ( ( 15 - 127 ) << 23 ) + 0xfff;
				f.u += mant_odd;
				data = f.u >> 13;
			}
		}
#else // Round towards +inf
		if( f.u >= f32infty.u ) {
			data = ( f.u > f32infty.u ) ? 0x7e00 : 0x7c00;
		} else {
			f.u &= round_mask;
			f.f *= magic.f;
			f.u -= round_mask;
			if( f.u > f16infty.u ) f.u = f16infty.u;

			data = f.u >> 13;
		}
#endif
		data |= sign >> 16;
	}
	half::half( double a )
	{
		// We lose the precision anyway, so just convert from float
		*this = half( ( float )a );
	}
	template< int Q >
	half::half( fixed_32< Q > a )
	{
		*this = half( ( float )a );
	}
	half::operator float( ) const
	{
		union F32 {
			unsigned int u;
			float f;
		};
		const F32 magic = { 113 << 23 };
		const unsigned int shifted_exp = 0x7c00 << 13;
		F32 o;

		o.u = ( data & 0x7fff ) << 13;
		unsigned int ex = shifted_exp & o.u;
		o.u += ( 127 - 15 ) << 23;

		if( ex == shifted_exp ) {
			o.u += ( 128 - 16 ) << 23;
		} else if( ex == 0 ) {
			o.u += 1 << 23;
			o.f -= magic.f;
		}
		o.u |= ( data & 0x8000 ) << 16;
		return o.f;
	}
	half::operator double( ) const
	{
		return ( double )( ( float )*this );
	}
	template< int Q >
	half::operator fixed_32< Q >( ) const
	{
		return fixed_32< Q >( ( float )*this );
	}

	half& half::operator=( half rhs )
	{
		data = rhs.data;
		return *this;
	}
	half& half::operator+=( half rhs )
	{
		float a, b, r;

		a = ( float )*this;
		b = ( float )rhs;
		r = a + b;

		*this = half( r );
		return *this;
	}
	half& half::operator-=( half rhs )
	{
		float a, b, r;

		a = ( float )*this;
		b = ( float )rhs;
		r = a - b;

		*this = half( r );
		return *this;
	}
	half& half::operator*=( half rhs )
	{
		float a, b, r;

		a = ( float )*this;
		b = ( float )rhs;
		r = a * b;

		*this = half( r );
		return *this;
	}
	half& half::operator/=( half rhs )
	{
		float a, b, r;

		a = ( float )*this;
		b = ( float )rhs;
		r = a / b;

		*this = half( r );
		return *this;
	}

	half& half::operator=( float rhs )
	{
		*this = half( rhs );
		return *this;
	}
	half& half::operator+=( float rhs )
	{
		float a, r;

		a = ( float )*this;
		r = a + rhs;

		*this = half( r );
		return *this;
	}
	half& half::operator-=( float rhs )
	{
		float a, r;

		a = ( float )*this;
		r = a - rhs;

		*this = half( r );
		return *this;
	}
	half& half::operator*=( float rhs )
	{
		float a, r;

		a = ( float )*this;
		r = a * rhs;

		*this = half( r );
		return *this;
	}
	half& half::operator/=( float rhs )
	{
		float a, r;

		a = ( float )*this;
		r = a / rhs;

		*this = half( r );
		return *this;
	}
		
	template< int Q >
	half& half::operator=( fixed_32< Q > rhs )
	{
		*this = half( rhs );
		return *this;
	}
	template< int Q >
	half& half::operator+=( fixed_32< Q > rhs )
	{		
		float a, b, r;

		a = ( float )*this;
		b = ( float )rhs;
		r = a + b;

		*this = half( r );
		return *this;
	}
	template< int Q >
	half& half::operator-=( fixed_32< Q > rhs )
	{
		float a, b, r;

		a = ( float )*this;
		b = ( float )rhs;
		r = a - b;

		*this = half( r );
		return *this;
	}
	template< int Q >
	half& half::operator*=( fixed_32< Q > rhs )
	{
		float a, b, r;

		a = ( float )*this;
		b = ( float )rhs;
		r = a * b;

		*this = half( r );
		return *this;
	}
	template< int Q >
	half& half::operator/=( fixed_32< Q > rhs )
	{
		float a, b, r;

		a = ( float )*this;
		b = ( float )rhs;
		r = a / b;

		*this = half( r );
		return *this;
	}

	half& half::operator++( )
	{
		float f;

		f = ( ( float )*this ) + 1.f;

		*this = half( f );
		return *this;
	}
	half& half::operator--( )
	{
		float f;

		f = ( ( float )*this ) - 1.f;

		*this = half( f );
		return *this;
	}
	half half::operator++( int )
	{
		float f;

		f = ( ( float )*this ) + 1.f;

		*this = half( f );
		return *this;
	}
	half half::operator--( int )
	{
		float f;

		f = ( ( float )*this ) - 1.f;

		*this = half( f );
		return *this;
	}

	// Non-member functions
	bool operator==( half a, half b )
	{
		return a.data == b.data;
	}
	bool operator!=( half a, half b )
	{
		return a.data != b.data;
	}
	bool operator>=( half a, half b )
	{
		return a.data >= b.data;
	}
	bool operator<=( half a, half b )
	{
		return a.data <= b.data;
	}
	bool operator>( half a, half b )
	{
		return a.data > b.data;
	}
	bool operator<( half a, half b )
	{
		return a.data < b.data;
	}

	half operator+( half a, half b )
	{
		float r;

		r = ( float )a + ( float )b;

		return half( r );
	}
	half operator-( half a, half b )
	{
		float r;

		r = ( float )a - ( float )b;

		return half( r );
	}
	half operator*( half a, half b )
	{
		float r;

		r = ( float )a * ( float )b;

		return half( r );
	}
	half operator/( half a, half b )
	{
		float r;

		r = ( float )a / ( float )b;

		return half( r );
	}

	half operator+( half a )
	{
		return half( a );
	}
	half operator-( half a )
	{
		half r;
		unsigned int sign;
		
		sign = a.data & 0x8000;
		r.data = ( a.data & 0x7fff )		// Copy exponent and mantissa
				 | ( ( ~sign ) & 0x8000 );	// Negate sign

		return r;
	}

	// Mass conversion functions
	void vul_single_to_half_array( half *out, float *in, unsigned int count )
	{
		unsigned int i;
#ifdef VUL_HALF_F16C
		__m128 veci;
		__m128i veco;
		unsigned int j;

		for ( i = 0; i < ( count & 0xfffffffc ); i += 4 ) { // i < count - ( count % 4 )
			veci = _mm_loadu_ps( &in[ i ] );
#ifdef VUL_HALF_ROUND_NEAREST_EVEN
			veco = _mm_cvtps_ph( veci, _MM_FROUND_TO_NEAREST_INT );
#else
			veco = _mm_cvtps_ph( veci, _MM_FROUND_TO_POS_INF );
#endif
			for( j = 0; j < 4; ++j ) {
				out[ i + j ].data = veco.m128i_u16[ j * 2 ];
			}
		}
		j = count & 3;
		veci = _mm_loadu_ps( &in[ i ] );
#ifdef VUL_HALF_ROUND_NEAREST_EVEN
		veco = _mm_cvtps_ph( veci, _MM_FROUND_TO_NEAREST_INT );
#else
		veco = _mm_cvtps_ph( veci, _MM_FROUND_TO_POS_INF );
#endif
		for(; j > 0; --j ) {
			out[ i + j ].data = veco.m128i_u16[ j * 2 ];
		}	
#elif defined( VUL_HALF_SSE )
		unsigned int j;

		#define CONSTF(name) _mm_castsi128_ps(name)
		for ( i = 0; i < ( count & 0xfffffffc ); i += 4 ) { // i < count - ( count % 4 )
			__m128 f = _mm_set_ps( in[ i + 3 ], in[ i + 2 ], in[ i + 1 ], in[ i ] );
#ifdef VUL_HALF_ROUND_NEAREST_EVEN
			__m128i mask_sign       = _mm_set1_epi32( 0x80000000u );
			__m128i c_f16max        = _mm_set1_epi32( ( 127 + 16 ) << 23 );
			__m128i c_nanbit        = _mm_set1_epi32( 0x200 );
			__m128i c_infty_as_fp16 = _mm_set1_epi32( 0x7c00 );
			__m128i c_min_normal    = _mm_set1_epi32( ( 127 - 14 ) << 23 );
			__m128i c_subnorm_magic = _mm_set1_epi32( ( ( 127 - 15 ) + ( 23 - 10 ) + 1 ) << 23 );
			__m128i c_normal_bias   = _mm_set1_epi32( 0xfff - ( ( 127 - 15 ) << 23 ) );

			__m128  msign       = CONSTF( mask_sign );
			__m128  justsign    = _mm_and_ps( msign, f );
			__m128  absf        = _mm_xor_ps( f, justsign );
			__m128i absf_int    = _mm_castps_si128( absf );
			__m128i f16max      = c_f16max;
			__m128  b_isnan     = _mm_cmpunord_ps( absf, absf );
			__m128i b_isregular = _mm_cmpgt_epi32( f16max, absf_int );
			__m128i nanbit      = _mm_and_si128( _mm_castps_si128( b_isnan ), c_nanbit );
			__m128i inf_or_nan  = _mm_or_si128( nanbit, c_infty_as_fp16 );

			__m128i min_normal  = c_min_normal;
			__m128i b_issub     = _mm_cmpgt_epi32( min_normal, absf_int );

			__m128  subnorm1    = _mm_add_ps( absf, CONSTF( c_subnorm_magic ) );
			__m128i subnorm2    = _mm_sub_epi32( _mm_castps_si128( subnorm1 ), c_subnorm_magic );

			__m128i mantoddbit  = _mm_slli_epi32( absf_int, 31 - 13 );
			__m128i mantodd     = _mm_srai_epi32( mantoddbit, 31 );

			__m128i round1      = _mm_add_epi32( absf_int, c_normal_bias );
			__m128i round2      = _mm_sub_epi32( round1, mantodd );
			__m128i normal      = _mm_srli_epi32( round2, 13 );

			__m128i nonspecial  = _mm_or_si128( _mm_and_si128( subnorm2, b_issub ), _mm_andnot_si128( b_issub, normal ) );

			__m128i joined      = _mm_or_si128( _mm_and_si128( nonspecial, b_isregular ), _mm_andnot_si128( b_isregular, inf_or_nan ) );

			__m128i sign_shift  = _mm_srli_epi32( _mm_castps_si128( justsign ), 16 );
			__m128i o           = _mm_or_si128( joined, sign_shift );
#else
			__m128i mask_sign       = _mm_set1_epi32( 0x80000000u );
			__m128i mask_round      = _mm_set1_epi32( ~0xfffu );
			__m128i c_f32infty      = _mm_set1_epi32( 255 << 23 );
			__m128i c_magic         = _mm_set1_epi32( 15 << 23 );
			__m128i c_nanbit        = _mm_set1_epi32( 0x200 );
			__m128i c_infty_as_fp16 = _mm_set1_epi32( 0x7c00 );
			__m128i c_clamp         = _mm_set1_epi32( ( 31 << 23 ) - 0x1000 );

			__m128  msign       = CONSTF( mask_sign );
			__m128  justsign    = _mm_and_ps( msign, f );
			__m128i f32infty    = c_f32infty;
			__m128  absf        = _mm_xor_ps( f, justsign );
			__m128  mround      = CONSTF( mask_round );
			__m128i absf_int    = _mm_castps_si128( absf );
			__m128i b_isnan     = _mm_cmpgt_epi32( absf_int, f32infty );
			__m128i b_isnormal  = _mm_cmpgt_epi32( f32infty, _mm_castps_si128( absf ) );
			__m128i nanbit      = _mm_and_si128( b_isnan, c_nanbit );
			__m128i inf_or_nan  = _mm_or_si128( nanbit, c_infty_as_fp16 );

			__m128  fnosticky   = _mm_and_ps( absf, mround );
			__m128  scaled      = _mm_mul_ps( fnosticky, CONSTF( c_magic ) );
			__m128  clamped     = _mm_min_ps( scaled, CONSTF( c_clamp ) );
			__m128i biased      = _mm_sub_epi32( _mm_castps_si128( clamped ), _mm_castps_si128( mround ) );
			__m128i shifted     = _mm_srli_epi32( biased, 13 );
			__m128i normal      = _mm_and_si128( shifted, b_isnormal );
			__m128i not_normal  = _mm_andnot_si128( b_isnormal, inf_or_nan );
			__m128i joined      = _mm_or_si128( normal, not_normal );

			__m128i sign_shift  = _mm_srli_epi32( _mm_castps_si128( justsign ), 16 );
			__m128i o	        = _mm_or_si128( joined, sign_shift );
#endif
			for( j = 0; j < 4; ++j ) {
				out[ i + j ].data = o.m128i_u16[ j * 2 ];
			}
		}
		#undef CONSTF
		// Do the last 3 non-sse
		for( j = count & 3; j > 0; --j ) {
			out[ i + j ] = half( in[ i + j ] );
		}
#else
		for ( i = 0; i < count; ++i ) {
			out[ i ] = half( in[ i ] );
		}
#endif
	}
	void vul_half_to_single_array( float *out, half *in, unsigned int count )
	{
		unsigned int i;
#ifdef VUL_HALF_F16C
		__m128i veci;
		__m128 veco;
		unsigned int j;

		for ( i = 0; i < ( count & 0xfffffffc ); i += 4 ) { // i < count - ( count % 4 )
			veci = _mm_setr_epi16( in[ i + 3 ].data, in[ i + 3 ].data,
								   in[ i + 2 ].data, in[ i + 2 ].data,
								   in[ i + 1 ].data, in[ i + 1 ].data,
								   in[ i     ].data, in[ i     ].data );
			veco = _mm_cvtph_ps( veci );
			_mm_store_ps( &out[ i ], veco );
		}
		veci = _mm_setr_epi16( in[ i + 3 ].data, in[ i + 3 ].data,
							   in[ i + 2 ].data, in[ i + 2 ].data,
							   in[ i + 1 ].data, in[ i + 1 ].data,
							   in[ i     ].data, in[ i     ].data );
		veco = _mm_cvtph_ps( veci );
		for( j = count & 3; j > 0; --j ) {
			out[ i + j ] = f.m128_f32[ j ];
		}
#elif defined( VUL_HALF_SSE )
		unsigned int j;

		for ( i = 0; i < ( count & 0xfffffffc ); i += 4 ) { // i < count - ( count % 4 )
			__m128i h = _mm_setr_epi32( in[ i ].data, in[ i + 1 ].data, in[ i + 2 ].data, in[ i + 3 ].data );
			
			__m128i mask_exman      = _mm_set1_epi32( 0x7fff );
			__m128i shifted_exp	    = _mm_set1_epi32( 0x7c00 << 13 );
			__m128i exp_adjust      = _mm_set1_epi32( ( 127 - 15 ) << 23 );
			__m128i zero            = _mm_set1_epi32( 0 );
			__m128i infnan_adjust   = _mm_set1_epi32( ( 128 - 16 ) << 23 );
			__m128i zeroden_adjust  = _mm_set1_epi32( 1 << 23 );
			__m128i magic           = _mm_set1_epi32( 113 << 23 );
			__m128i sign_mask       = _mm_set1_epi32( 0x8000 );

			__m128i u             = _mm_slli_epi32( _mm_and_si128( h, mask_exman ), 13 );
			__m128i ex            = _mm_and_si128( shifted_exp, u );
			__m128i adjusted      = _mm_add_epi32( exp_adjust, u );

			__m128i b_isinfnan    = _mm_cmpeq_epi32( ex, shifted_exp );
			__m128i infnan_bias   = _mm_and_si128( b_isinfnan, infnan_adjust );
			__m128i biased        = _mm_add_epi32( adjusted, infnan_bias );

			__m128i b_iszeroden   = _mm_cmpeq_epi32( ex, zero );
			__m128i zeroden_bias  = _mm_and_si128( b_iszeroden, zeroden_adjust );
			__m128i biasadjusted  = _mm_add_epi32( biased, zeroden_bias );

			__m128 magicf         = _mm_castsi128_ps( _mm_and_si128( b_iszeroden, magic ) );
			__m128 fabs           = _mm_sub_ps( _mm_castsi128_ps( biasadjusted ), magicf );

			__m128i signbit       = _mm_slli_epi32( _mm_and_si128( sign_mask, h ), 16 );
			__m128 f              = _mm_castsi128_ps( _mm_or_si128( _mm_castps_si128( fabs ), signbit ) );
			
			for( j = 0; j < 4; ++j ) {
				out[ i + j ] = f.m128_f32[ j ];
			}
		}
		// Do the last 3 non-sse
		for( j = count & 3; j > 0; --j ) {
			out[ i + j ] = ( float )in[ i + j ];
		}
#else
		for ( i = 0; i < count; ++i ) {
			out[ i ] = ( float )in[ i ];
		}
#endif
	}
	
	void vul_double_to_half_array( half *out, double *in, unsigned int count )
	{
		unsigned int i;
#ifdef VUL_HALF_F16C
		__m128 veci;
		__m128i veco;
		unsigned int j;

		for ( i = 0; i < ( count & 0xfffffffc ); i += 4 ) { // i < count - ( count % 4 )
			veci = _mm_set_ps( ( float )in[ i + 3 ], ( float )in[ i + 2 ], ( float )in[ i + 1 ], ( float )in[ i ] );
			
#ifdef VUL_HALF_ROUND_NEAREST_EVEN
			veco = _mm_cvtps_ph( veci, _MM_FROUND_TO_NEAREST_INT  );
#else
			veco = _mm_cvtps_ph( veci, _MM_FROUND_TO_POS_INF );
#endif
			for( j = 0; j < 4; ++j ) {
				out[ i + j ].data = veco.m128i_u16[ j * 2 ];
			}
		}
		j = count & 3;
		veci = _mm_set_ps( ( float )in[ i + 3 ], ( float )in[ i + 2 ], ( float )in[ i + 1 ], ( float )in[ i ] );
#ifdef VUL_HALF_ROUND_NEAREST_EVEN
		veco = _mm_cvtps_ph( veci, _MM_FROUND_TO_NEAREST_INT );
#else
		veco = _mm_cvtps_ph( veci, _MM_FROUND_TO_POS_INF );
#endif
		for(; j > 0; --j ) {
			out[ i + j ].data = veco.m128i_u16[ j * 2 ];
		}
#elif defined( VUL_HALF_SSE )
		unsigned int j;

		#define CONSTF(name) _mm_castsi128_ps(name)
		for ( i = 0; i < ( count & 0xfffffffc ); i += 4 ) { // i < count - ( count % 4 )
			__m128 f = _mm_set_ps( ( float )in[ i + 3 ], ( float )in[ i + 2 ], ( float )in[ i + 1 ], ( float )in[ i ] );
#ifdef VUL_HALF_ROUND_NEAREST_EVEN
			__m128i mask_sign       = _mm_set1_epi32( 0x80000000u );
			__m128i c_f16max        = _mm_set1_epi32( ( 127 + 16 ) << 23 );
			__m128i c_nanbit        = _mm_set1_epi32( 0x200 );
			__m128i c_infty_as_fp16 = _mm_set1_epi32( 0x7c00 );
			__m128i c_min_normal    = _mm_set1_epi32( ( 127 - 14 ) << 23 );
			__m128i c_subnorm_magic = _mm_set1_epi32( ( ( 127 - 15 ) + ( 23 - 10 ) + 1 ) << 23 );
			__m128i c_normal_bias   = _mm_set1_epi32( 0xfff - ( ( 127 - 15 ) << 23 ) );

			__m128  msign       = CONSTF( mask_sign );
			__m128  justsign    = _mm_and_ps( msign, f );
			__m128  absf        = _mm_xor_ps( f, justsign );
			__m128i absf_int    = _mm_castps_si128( absf );
			__m128i f16max      = c_f16max;
			__m128  b_isnan     = _mm_cmpunord_ps( absf, absf );
			__m128i b_isregular = _mm_cmpgt_epi32( f16max, absf_int );
			__m128i nanbit      = _mm_and_si128( _mm_castps_si128( b_isnan ), c_nanbit );
			__m128i inf_or_nan  = _mm_or_si128( nanbit, c_infty_as_fp16 );

			__m128i min_normal  = c_min_normal;
			__m128i b_issub     = _mm_cmpgt_epi32( min_normal, absf_int );

			__m128  subnorm1    = _mm_add_ps( absf, CONSTF( c_subnorm_magic ) );
			__m128i subnorm2    = _mm_sub_epi32( _mm_castps_si128( subnorm1 ), c_subnorm_magic );

			__m128i mantoddbit  = _mm_slli_epi32( absf_int, 31 - 13 );
			__m128i mantodd     = _mm_srai_epi32( mantoddbit, 31 );

			__m128i round1      = _mm_add_epi32( absf_int, c_normal_bias );
			__m128i round2      = _mm_sub_epi32( round1, mantodd );
			__m128i normal      = _mm_srli_epi32( round2, 13 );

			__m128i nonspecial  = _mm_or_si128( _mm_and_si128( subnorm2, b_issub ), _mm_andnot_si128( b_issub, normal ) );

			__m128i joined      = _mm_or_si128( _mm_and_si128( nonspecial, b_isregular ), _mm_andnot_si128( b_isregular, inf_or_nan ) );

			__m128i sign_shift  = _mm_srli_epi32( _mm_castps_si128( justsign ), 16 );
			__m128i o           = _mm_or_si128( joined, sign_shift );
#else
			__m128i mask_sign       = _mm_set1_epi32( 0x80000000u );
			__m128i mask_round      = _mm_set1_epi32( ~0xfffu );
			__m128i c_f32infty      = _mm_set1_epi32( 255 << 23 );
			__m128i c_magic         = _mm_set1_epi32( 15 << 23 );
			__m128i c_nanbit        = _mm_set1_epi32( 0x200 );
			__m128i c_infty_as_fp16 = _mm_set1_epi32( 0x7c00 );
			__m128i c_clamp         = _mm_set1_epi32( ( 31 << 23 ) - 0x1000 );

			__m128  msign       = CONSTF( mask_sign );
			__m128  justsign    = _mm_and_ps( msign, f );
			__m128i f32infty    = c_f32infty;
			__m128  absf        = _mm_xor_ps( f, justsign );
			__m128  mround      = CONSTF( mask_round );
			__m128i absf_int    = _mm_castps_si128( absf );
			__m128i b_isnan     = _mm_cmpgt_epi32( absf_int, f32infty );
			__m128i b_isnormal  = _mm_cmpgt_epi32( f32infty, _mm_castps_si128( absf ) );
			__m128i nanbit      = _mm_and_si128( b_isnan, c_nanbit );
			__m128i inf_or_nan  = _mm_or_si128( nanbit, c_infty_as_fp16 );

			__m128  fnosticky   = _mm_and_ps( absf, mround );
			__m128  scaled      = _mm_mul_ps( fnosticky, CONSTF( c_magic ) );
			__m128  clamped     = _mm_min_ps( scaled, CONSTF( c_clamp ) );
			__m128i biased      = _mm_sub_epi32( _mm_castps_si128( clamped ), _mm_castps_si128( mround ) );
			__m128i shifted     = _mm_srli_epi32( biased, 13 );
			__m128i normal      = _mm_and_si128( shifted, b_isnormal );
			__m128i not_normal  = _mm_andnot_si128( b_isnormal, inf_or_nan );
			__m128i joined      = _mm_or_si128( normal, not_normal );

			__m128i sign_shift  = _mm_srli_epi32( _mm_castps_si128( justsign ), 16 );
			__m128i o	        = _mm_or_si128( joined, sign_shift );
#endif
			for( j = 0; j < 4; ++j ) {
				out[ i + j ].data = o.m128i_u16[ j * 2 ];
			}
		}
		#undef CONSTF
		// Do the last 3 non-sse
		for( j = count & 3; j > 0; --j ) {
			out[ i + j ] = half( in[ i + j ] );
		}
#else
		for( i = 0; i < count; ++i ) {
			out[ i ] = half( in[ i ] );
		}
#endif
	}
	void vul_half_to_double_array( double *out, half *in, unsigned int count )
	{
		unsigned int i;
#ifdef VUL_HALF_F16C
		__m128i veci;
		__m128 veco;
		unsigned int j;

		for ( i = 0; i < ( count & 0xfffffffc ); i += 4 ) { // i < count - ( count % 4 )
			veci = _mm_setr_epi16( in[ i + 3 ].data, in[ i + 3 ].data,
								   in[ i + 2 ].data, in[ i + 2 ].data,
								   in[ i + 1 ].data, in[ i + 1 ].data,
								   in[ i     ].data, in[ i     ].data );
			veco = _mm_cvtph_ps( veci );
			for( j = 0; j < 4; ++j ) {
				out[ i + j ] = ( double )veco.m128_f32[ j ];
			}
		}
		veci = _mm_setr_epi16( in[ i + 3 ].data, in[ i + 3 ].data,
							   in[ i + 2 ].data, in[ i + 2 ].data,
							   in[ i + 1 ].data, in[ i + 1 ].data,
							   in[ i     ].data, in[ i     ].data );
		veco = _mm_cvtph_ps( veci );
		for( j = count & 3; j > 0; --j ) {
			out[ i + j ] = ( double )veco.m128_f32[ j ];
		}
#elif defined( VUL_HALF_SSE )
		unsigned int j;

		#define CONSTF(name) _mm_castsi128_ps(name)
		for ( i = 0; i < ( count & 0xfffffffc ); i += 4 ) { // i < count - ( count % 4 )
			__m128i h = _mm_setr_epi32( in[ i ].data, in[ i + 1 ].data, in[ i + 2 ].data, in[ i + 3 ].data );
			
			__m128i mask_exman      = _mm_set1_epi32( 0x7fff );
			__m128i shifted_exp	    = _mm_set1_epi32( 0x7c00 << 13 );
			__m128i exp_adjust      = _mm_set1_epi32( ( 127 - 15 ) << 23 );
			__m128i zero            = _mm_set1_epi32( 0 );
			__m128i infnan_adjust   = _mm_set1_epi32( ( 128 - 16 ) << 23 );
			__m128i zeroden_adjust  = _mm_set1_epi32( 1 << 23 );
			__m128i magic           = _mm_set1_epi32( 113 << 23 );
			__m128i sign_mask       = _mm_set1_epi32( 0x8000 );

			__m128i u             = _mm_slli_epi32( _mm_and_si128( h, mask_exman ), 13 );
			__m128i ex            = _mm_and_si128( shifted_exp, u );
			__m128i adjusted      = _mm_add_epi32( exp_adjust, u );

			__m128i b_isinfnan    = _mm_cmpeq_epi32( ex, shifted_exp );
			__m128i infnan_bias   = _mm_and_si128( b_isinfnan, infnan_adjust );
			__m128i biased        = _mm_add_epi32( adjusted, infnan_bias );

			__m128i b_iszeroden   = _mm_cmpeq_epi32( ex, zero );
			__m128i zeroden_bias  = _mm_and_si128( b_iszeroden, zeroden_adjust );
			__m128i biasadjusted  = _mm_add_epi32( biased, zeroden_bias );

			__m128 magicf         = _mm_castsi128_ps( _mm_and_si128( b_iszeroden, magic ) );
			__m128 fabs           = _mm_sub_ps( _mm_castsi128_ps( biasadjusted ), magicf );

			__m128i signbit       = _mm_slli_epi32( _mm_and_si128( sign_mask, h ), 16 );
			__m128 f              = _mm_castsi128_ps( _mm_or_si128( _mm_castps_si128( fabs ), signbit ) );
			
			for( j = 0; j < 4; ++j ) {
				out[ i + j ] = ( double )f.m128_f32[ j ];
			}
		}
		#undef CONSTF
		// Do the last 3 non-sse
		for( j = count & 3; j > 0; --j ) {
			out[ i + j ] = ( double )in[ i + j ];
		}
#else
		for ( i = 0; i < count; ++i ) {
			out[ i ] = ( double )in[ i ];
		}
#endif
	}
#endif
}

	//------------------------------------
	// std::numberic_limits
	//

namespace std {
		
#ifdef VUL_DEFINE
	vul::half abs( vul::half a )
	{
		vul::half r;
		
		r.data = a.data & 0x7fff;

		return r;
	}

	template< >
	class numeric_limits< vul::half > : public numeric_limits< float >
	{
	public:
#ifdef VUL_CPLUSPLUS11
		static constexpr bool is_signed = true;
		static constexpr bool is_exact = false;
		static constexpr bool is_modulo = false;
		static constexpr bool is_iec559 = true;
		static constexpr bool has_infinity = true;
		static constexpr bool has_quiet_NaN = true;

		static constexpr std::float_denorm_style has_denorm = std::denorm_present;
		static constexpr std::float_round_style round_style = std::round_to_nearest;
		static constexpr int digits = 10;
		static constexpr int digits10 = 3;
		static constexpr int max_digits10 = 5;
		static constexpr int radix = 2;
		static constexpr int min_exponent = -14;
		static constexpr int min_exponent10 = -4;
		static constexpr int max_exponent = 15;
		static constexpr int max_exponent10 = 4;
		static vul::half min( ) noexcept			{ vul::half h; h.data = 0x0400; return h; }
		static vul::half lowest( ) noexcept			{ vul::half h; h.data = 0xfbff; return h; }
		static vul::half max( ) noexcept			{ vul::half h; h.data = 0x7bff; return h; }
		static vul::half epsilon( ) noexcept		{ vul::half h; h.data = 0x1400; return h; }
		static vul::half round_error( ) noexcept	{ vul::half h; h.data = 0x3800; return h; };
		static vul::half infinity( ) noexcept		{ vul::half h; h.data = 0x7c00; return h; };
		static vul::half quiet_NaN( ) noexcept		{ vul::half h; h.data = 0x7fff; return h; };
		static vul::half signaling_NaN( ) noexcept	{ vul::half h; h.data = 0x7dff; return h; };
		static vul::half denorm_min( ) noexcept		{ vul::half h; h.data = 0x0001; return h; };
#else
		static const bool is_signed = true;
		static const bool is_exact = false;
		static const bool is_modulo = false;
		static const bool is_iec559 = true;
		static const bool has_infinity = true;
		static const bool has_quiet_NaN = true;

		static const std::float_denorm_style has_denorm = std::denorm_present;
		static const std::float_round_style round_style = std::round_to_nearest;
		static const int digits = 10;
		static const int digits10 = 3;
		static const int max_digits10 = 5;
		static const int radix = 2;
		static const int min_exponent = -14;
		static const int min_exponent10 = -4;
		static const int max_exponent = 15;
		static const int max_exponent10 = 4;
		static const vul::half min( )				{ vul::half h; h.data = 0x0400; return h; }
		static const vul::half lowest( )			{ vul::half h; h.data = 0xfbff; return h; }
		static const vul::half max( )				{ vul::half h; h.data = 0x7bff; return h; }
		static const vul::half epsilon( )			{ vul::half h; h.data = 0x1400; return h; }
		static const vul::half round_error( )		{ vul::half h; h.data = 0x3800; return h; };
		static const vul::half infinity( )			{ vul::half h; h.data = 0x7c00; return h; };
		static const vul::half quiet_NaN( )			{ vul::half h; h.data = 0x7fff; return h; };
		static const vul::half signaling_NaN( )		{ vul::half h; h.data = 0x7dff; return h; };
		static const vul::half denorm_min( )		{ vul::half h; h.data = 0x0001; return h; };
#endif
	};
	
#endif
}

#endif
