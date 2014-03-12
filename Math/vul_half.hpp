/*
 * Villains' Utility Library - Thomas Martin Schmid, 2014. Public domain¹
 *
 * This file contains an implementation of IEEE 754 half precision flaoting point.
 * It also contains interfaces to the fixed point types of vul_fixed.h
 * if VUL_HALF_TABLE is defined, a lookup table is used for single/half
 * conversions, at the expense of 8576+1536 bytes of memory. ( see: ftp://ftp.fox-toolkit.org/pub/fasthalffloatconversion.pdf )
 * If VUL_HALF_SSE is defined, the SSE instructions _mm_cvtph_ps & _mm_vctps_ph
 * are used for single/half conversions.
 * Otherwise, a slower runtime calculation is done.
 * 
 * @TODO: Implement table and SSE versions!
 * @TODO: Convert directly to/from double and fixed formats.
 * @TODO: Test if working in half format the whole way is faster (for ops)
 *
 * ¹ If public domain is not legally valid in your country and or legal area,
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

// Choose one or neither of these
//#define VUL_HALF_TABLE // @TODO: Actually calculate the table and include it!
//#define VUL_HALF_SSE // @TODO: Fix this; doesn't work atm.

namespace vul {
	
	template< int Q >
	struct fixed_32;

	//----------------
	// Declarations
	//
	
	struct half {
		unsigned short data;

#ifdef VUL_CPLUSPLUS11
		constexpr half( );
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

#ifdef VUL_CPLUSPLUS11
	half constexpr operator+( half a );
	half constexpr operator-( half a );
#else
	half operator+( half a );
	half operator-( half a );
#endif

	half abs( half a );

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
	constexpr half::half( )
	{
		data = 0;
	}
#else
	half::half( )
	{
		data = 0;
	}
#endif
	half::half( float a )
	{
#if defined( VUL_HALF_TABLE )
		error
#elif defined( VUL_HALF_SSE )
		data = _mm_cvtph_ps( a );
#else
		int i, te;
		unsigned int f, e;
		f = *( ( unsigned int* )&a );
		i = ( f >> 16 ) & 0x8000u;									// Always copy sign bit
		if( ( f & 0x7f800000u ) == 0 ) {							// Zero is a special case
			data = i; // Signed
			return;
		}
		e = ( ( ( f & 0x7f800000u ) - 0x38000000u ) >> 13 ) & 0x7c00u;	// Subtract exponent bias ( ( 127 - 15 ) << 23), shift and mask
		
		te = ( e >> 10 ) - 15;
		if ( te < -24 ) {			// Exponent is smaller than -24, can't be represented in a half, so we set it to zero
			data = i;				// Just copy sign bit.
		} else if ( te < -14 ) {	// Exponent is smaller than -14, so small they need to be represented as denormals
			data = i | ( 0x0400u >> ( -te - 14 ) )			// Copy sign, shift exponent 
				     | ( ( f >> ( -te - 1 ) ) & 0x03ffu );	// and shift, mask and copy mantissa.
		} else if ( te <= 15 ) {	// Exponent is smaller than 16, normal number
			data = i | e | ( ( f >> 13 ) & 0x03ffu );		// Copy sign bit and exponent, shift, mask and copy mantissa.
		} else if ( te < 128 ) {	// Exponent is smaller than 128, large number is mapped to infinity
			data = i | 0x7c00u;		// Sign bit is copied, and the exponent is set to max ( 31 ). Mantissa is empty
		} else {					// Exponent is 128, Infinity or NaN stays Infinity or NaN
			data = i | 0x7c00u | ( ( f >> 13 ) & 0x03ffu );	// Sign bit is copied, exponent set to max ( 31 ) and mantissa shifted, masked and copied
		}
#endif
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
#if defined( VUL_HALF_TABLE )

#elif defined( VUL_HALF_SSE )
		return _cvtsh_ss( data );
#else
		int i;
		unsigned int e, m;
		
		i = ( ( data & 0x8000 ) << 16 );				// Always copy sign bit
		
		e = ( data & 0x7c00 );	// Exponent
		m = ( data & 0x3ff );	// Mantissa
		if ( e == 0 ) {			// Exponent is zero
			if ( m != 0 ) {						// This is a subnormal number, and we should map it to a normal float.
				m = m << 13;					// Zero pad mantissa
				e = 0;							// Zero exponent
				while( !( m & 0x00800000 ) ) {	// While not normalized
					e -= 0x00800000;			// Decrement exponent ( 1 << 23 )
					m <<= 1;					// Shift mantissa
				}
				m &= ~0x00800000;				// Clear leading 1 bit
				e += 0x38800000;				// Adjust bias ( ( 127-15 ) << 23 )

				i |= m | e;
			} // Otherwise e == 0 && m == 0, and we have negative or positive zero, so just copy sign, which we have already done.
		} else if ( e == 0x7c00 ) {			// Exponent is 31 and the half is infinity or NaN
			i |= ( ( e + 0x23c00 ) << 13 );	// Add bias ( ( 255 - ( 127-15) ) << 10 ) to exponent and shift
			i |= ( m << 13 );				// Append 13 0-bits to mantissa and shift
		} else {
			// Normal number, just convert it
			i |= ( ( e + 0x1c000 ) << 13 ); // Add bias ( ( 127-15) << 10 ) to exponent and shift
			i |= ( m << 13 );				// Append 13 0-bits to mantissa and shift
		}

		return *( ( float* )&i );
#endif
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

#ifdef VUL_CPLUSPLUS11
	half constexpr operator+( half a )
	{
		return half( a );
	}
	half constexpr operator-( half a )
	{
		half r;
		unsigned int sign;
		
		sign = a.data & 0x80000000;
		r.data = ( a.data & 0x7fffffff )		// Copy exponent and mantissa
				 | ( ( ~sign ) & 0x80000000 );	// Negate sign

		return r;
	}
#else
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
#endif
	
	half abs( half a )
	{
		half r;
		
		r.data = a.data & 0x7fff;

		return r;
	}

	// Mass conversion functions
	void vul_single_to_half_array( half *out, float *in, unsigned int count )
	{
		unsigned int i;
#ifdef VUL_HALF_SSE
		M128 veci;
		M128i veco;
		unsigned int of[ 4 ], j;

		for ( i = 0; i < ( count & 0xfffffffc ); i += 4 ) { // i < count - ( count % 4 )
			veci = _mm_loadu_ps( &in[ i ] );
			veco = _mm_cvtps_ph( veci );
			_mm_store_si128( ( M128i* )of, veco );
			for( j = 0; j < 4; ++j ) {
				out[ j ].data = ( unsigned short )of[ j ];
			}
		}
		j = count & 3;
		veci = _mm_loadu_ps( &in[ i ] );
		veco = _mm_cvtps_ph( veci );
		_mm_store_si128( ( M128i* )of, veco );
		for(; j > 0; --j ) {
			out[ i + j ].data = ( unsigned short )of[ j ];
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
#ifdef VUL_HALF_SSE
		M128i veci;
		M128 veco;
		unsigned int inf[ 4 ], j;
		float of[ 4 ];

		for ( i = 0; i < ( count & 0xfffffffc ); i += 4 ) { // i < count - ( count % 4 )
			veci = _mm_setr_epi16( in[ i ].data, in[ i ].data,
								   in[ i + 1 ].data, in[ i + 1 ].data,
								   in[ i + 2 ].data, in[ i + 2 ].data,
								   in[ i + 3 ].data, in[ i + 3 ].data );
			veco = _mm_cvtph_ps( veci );
			_mm_store_ps( &out[ i ], veco );
		}
		veci = _mm_setr_epi16( in[ i ].data, in[ i ].data,
								   in[ i + 1 ].data, in[ i + 1 ].data,
								   in[ i + 2 ].data, in[ i + 2 ].data,
								   in[ i + 3 ].data, in[ i + 3 ].data );
			veco = _mm_cvtph_ps( veci );
			_mm_store_ps( of, veco );
		for( j = count & 3; j > 0; --j ) {
			out[ i + j ] = of[ j ];
		}
#else
		for ( i = 0; i < count; ++i ) {
			out[ i ] = half( in[ i ] );
		}
#endif
	}
	
	void vul_double_to_half_array( half *out, double *in, unsigned int count )
	{
		unsigned int i;
#ifdef VUL_HALF_SSE
		M128 veci;
		M128i veco;
		unsigned int of[ 4 ], j;

		for ( i = 0; i < ( count & 0xfffffffc ); i += 4 ) { // i < count - ( count % 4 )
			veci = _mm_set_ps( ( float )in[ i ], ( float )in[ i + 1 ], ( float )in[ i + 2 ], ( float )in[ i + 3 ] );
			veco = _mm_cvtps_ph( veci );
			_mm_store_si128( ( M128i* )of, veco );
			for( j = 0; j < 4; ++j ) {
				out[ j ].data = ( unsigned short )of[ j ];
			}
		}
		j = count & 3;
		veci = _mm_set_ps( ( float )in[ i ], ( float )in[ i + 1 ], ( float )in[ i + 2 ], ( float )in[ i + 3 ] );
		veco = _mm_cvtps_ph( veci );
		_mm_store_si128( ( M128i* )of, veco );
		for(; j > 0; --j ) {
			out[ i + j ].data = ( unsigned short )of[ j ];
		}		
#else
		for ( i = 0; i < count; ++i ) {
			out[ i ] = half( in[ i ] );
		}
#endif
	}
	void vul_half_to_double_array( double *out, half *in, unsigned int count )
	{
		unsigned int i;
#ifdef VUL_HALF_SSE
		M128i veci;
		M128 veco;
		unsigned int inf[ 4 ], j;
		float of[ 4 ];

		for ( i = 0; i < ( count & 0xfffffffc ); i += 4 ) { // i < count - ( count % 4 )
			veci = _mm_setr_epi16( in[ i ].data, in[ i ].data,
								   in[ i + 1 ].data, in[ i + 1 ].data,
								   in[ i + 2 ].data, in[ i + 2 ].data,
								   in[ i + 3 ].data, in[ i + 3 ].data );
			veco = _mm_cvtph_ps( veci );
			_mm_store_ps( &of[ i ], veco );
			for( j = 0; j < 4; ++j ) {
				out[ i + j ] = ( double )of[ j ];
			}
		}
		veci = _mm_setr_epi16( in[ i ].data, in[ i ].data,
								   in[ i + 1 ].data, in[ i + 1 ].data,
								   in[ i + 2 ].data, in[ i + 2 ].data,
								   in[ i + 3 ].data, in[ i + 3 ].data );
			veco = _mm_cvtph_ps( veci );
			_mm_store_ps( of, veco );
		for( j = count & 3; j > 0; --j ) {
			out[ i + j ] = ( double )of[ j ];
		}
#else
		for ( i = 0; i < count; ++i ) {
			out[ i ] = half( in[ i ] );
		}
#endif
	}
#endif

}

	//------------------------------------
	// std::numberic_limits
	//

namespace std {
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
		static constexpr vul::half min( ) noexcept				{ vul::half h; h.data = 0x0400; return h; }
		static constexpr vul::half lowest( ) noexcept			{ vul::half h; h.data = 0xfbff; return h; }
		static constexpr vul::half max( ) noexcept				{ vul::half h; h.data = 0x7bff; return h; }
		static constexpr vul::half epsilon( ) noexcept			{ vul::half h; h.data = 0x1400; return h; }
		static constexpr vul::half round_error( ) noexcept		{ vul::half h; h.data = 0x3800; return h; };
		static constexpr vul::half infinity( ) noexcept			{ vul::half h; h.data = 0x7c00; return h; };
		static constexpr vul::half quiet_NaN( ) noexcept		{ vul::half h; h.data = 0x7fff; return h; };
		static constexpr vul::half signaling_NaN( ) noexcept	{ vul::half h; h.data = 0x7dff; return h; };
		static constexpr vul::half denorm_min( ) noexcept		{ vul::half h; h.data = 0x0001; return h; };
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
	
#ifdef VUL_CPLUSPLUS11
	template< > struct hash< vul::half >
	{
		typedef vul::half argument_type;
		typedef size_t result_type;
		
		size_t operator( )( vul::half arg ) const
		{
			return hash< vul::half >( ) ( static_cast< unsigned int >( arg.data ) & -(arg.data != 0x8000 ) );
		}
	};
#endif
}

#endif
