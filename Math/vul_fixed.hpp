/*
 * Villains' Utility Library - Thomas Martin Schmid, 2017. Public domain¹
 *
 * This file contains fixed point types of 32 bit size.
 * It also interfaces with the half precision type in vul_half.h
 * It uses the Q number format and takes the fractional number in as
 * an arguemnt.
 * This means Q24.8 is fixed< 8 >
 * 
 * Uses for an int to store the value,
 * and a long long as a temporary in calculations.
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

#ifndef VUL_FIXED_HPP
#define VUL_FIXED_HPP

#include <cmath>

namespace vul {
	
	//----------------
	// Declarations
	//

	struct half;

	template< int Q >
	struct fixed_32 {
		int data;

#ifdef VUL_CPLUSPLUS11
		fixed_32< Q >( );
#else
		fixed_32< Q >( );
#endif
		explicit fixed_32< Q >( half a );
		explicit fixed_32< Q >( float a );
		explicit fixed_32< Q >( double a );
		template< int Q32 >
		explicit fixed_32< Q >( fixed_32< Q32 > a );
		operator half( ) const;
		operator float( ) const;
		operator double( ) const;
		template< int Q32 >
		operator fixed_32< Q32 >( ) const; // Different Q

		fixed_32< Q > &operator=( fixed_32< Q > rhs );
		fixed_32< Q > &operator+=( fixed_32< Q > rhs );
		fixed_32< Q > &operator-=( fixed_32< Q > rhs );
		fixed_32< Q > &operator*=( fixed_32< Q > rhs );
		fixed_32< Q > &operator/=( fixed_32< Q > rhs );

		fixed_32< Q > &operator=( float rhs );
		fixed_32< Q > &operator+=( float rhs );
		fixed_32< Q > &operator-=( float rhs );
		fixed_32< Q > &operator*=( float rhs );
		fixed_32< Q > &operator/=( float rhs );
		
		fixed_32< Q > &operator=( double rhs );
		fixed_32< Q > &operator+=( double rhs );
		fixed_32< Q > &operator-=( double rhs );
		fixed_32< Q > &operator*=( double rhs );
		fixed_32< Q > &operator/=( double rhs );

		fixed_32< Q > &operator++( );
		fixed_32< Q > &operator--( );
		fixed_32< Q > &operator++( int );
		fixed_32< Q > &operator--( int );
	};

	// Non-member 32-bit functions
	template< int Q >
	bool operator==( fixed_32< Q > a, fixed_32< Q > b );
	template< int Q >
	bool operator!=( fixed_32< Q > a, fixed_32< Q > b );
	template< int Q >
	bool operator>=( fixed_32< Q > a, fixed_32< Q > b );
	template< int Q >
	bool operator<=( fixed_32< Q > a, fixed_32< Q > b );
	template< int Q >
	bool operator>( fixed_32< Q > a, fixed_32< Q > b );
	template< int Q >
	bool operator<( fixed_32< Q > a, fixed_32< Q > b );
	
	template< int Q >
	fixed_32< Q > operator+( fixed_32< Q > a, fixed_32< Q > b );
	template< int Q >
	fixed_32< Q > operator-( fixed_32< Q > a, fixed_32< Q > b );
	template< int Q >
	fixed_32< Q > operator*( fixed_32< Q > a, fixed_32< Q > b );
	template< int Q >
	fixed_32< Q > operator/( fixed_32< Q > a, fixed_32< Q > b );
	
	template< int Q >
	fixed_32< Q > const operator+( fixed_32< Q > a );
	template< int Q >
	fixed_32< Q > const operator-( fixed_32< Q > a );
	
	template< int Q >
	fixed_32< Q > abs( fixed_32< Q > a );

	// Since VC++ doesn't supply a round(), we write our own:
	float round( float v );
	double round( double v );
	

	//---------------------------
	// Definitions
	//
#ifdef VUL_DEFINE
	float round( float v )
	{
		return floor( v + 0.5f );
	}
	double round( double v )
	{
		return floor( v + 0.5 );
	}
#endif
	
#ifdef VUL_CPLUSPLUS11
	template< int Q >
	fixed_32< Q >::fixed_32( ) : data( 0 )
	{
	}
#else
	template< int Q >
	fixed_32< Q >::fixed_32( )
	{
		data = 0;
	}
#endif
	template< int Q >
	fixed_32< Q >::fixed_32( half a )
	{
		data = ( int )round( ( float )a * std::pow( 2.f, ( float )Q ) );
	}
	template< int Q >
	fixed_32< Q >::fixed_32( float a )
	{
		data = ( int )round( a * std::pow( 2.f, ( float )Q ) );
	}
	template< int Q >
	fixed_32< Q >::fixed_32( double a )
	{
		data = ( int )round( a * std::pow( 2.0, ( double )Q ) );
	}

	template< int Q >
	template< int Q32 >
	fixed_32< Q >::fixed_32( fixed_32< Q32 > a )
	{
#if defined VUL_WINDOWS && !defined( __MINGW32__ ) && !defined( __MINGW64__ )
	#pragma warning ( disable: 4293 ) // We are aware, and branch accordingly
#endif
		// Negative shifts are undefined, so branch
		if( Q > Q32 ) {
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wshift-count-negative"
#endif
			data = a.data << ( Q - Q32 );
		} else {
			data = a.data >> ( Q32 - Q );
		}
#ifdef __clang__
#pragma clang diagnostic pop
#endif
#if defined VUL_WINDOWS && !defined( __MINGW32__ ) && !defined( __MINGW64__ )
	#pragma warning ( default: 4293 )
#endif
	}
	
	template< int Q >
	fixed_32< Q >::operator half( ) const
	{
		float r = ( float )data;
		return ( half )( r * std::pow( 2.f, ( float )-Q ) );
	}
	template< int Q >
	fixed_32< Q >::operator float( ) const
	{
		float r = ( float )data;
		return ( float )( r * std::pow( 2.f, ( float )-Q ) );
	}
	template< int Q >
	fixed_32< Q >::operator double( ) const
	{
		float r = ( float )data;
		return ( double )( r * std::pow( 2.f, ( float )-Q ) );
	}
	template< int Q >
	template< int Q32 >
	fixed_32< Q >::operator fixed_32< Q32 >( ) const
	{
		fixed_32< Q32 > r;

#if defined VUL_WINDOWS && !defined( __MINGW32__ ) && !defined( __MINGW64__ )
	#pragma warning ( disable: 4293 ) // We are aware, and branch accordingly
#endif
		// Negative shifts are undefined, so branch
		if( Q > Q32 ) {
			r.data = data << ( Q - Q32 );
		} else {
			r.data = data >> ( Q32 - Q );
		}
#if defined VUL_WINDOWS && !defined( __MINGW32__ ) && !defined( __MINGW64__ )
	#pragma warning ( default: 4293 )
#endif

		return r;
	}
	
	template< int Q >
	fixed_32< Q > &fixed_32< Q >::operator=( fixed_32< Q > rhs )
	{
		data = rhs.data;
		return *this;
	}	
	template< int Q >
	fixed_32< Q > &fixed_32< Q >::operator+=( fixed_32< Q > rhs )
	{
		data += rhs.data;
		return *this;
	}	
	template< int Q >
	fixed_32< Q > &fixed_32< Q >::operator-=( fixed_32< Q > rhs )
	{
		data -= rhs.data;
		return *this;
	}	
	template< int Q >
	fixed_32< Q > &fixed_32< Q >::operator*=( fixed_32< Q > rhs )
	{
		int K;
		long long temp;

		K = ( 2 << ( Q - 1 ) );
		temp = ( long long )data * ( long long )rhs.data;
		temp += K;

		data = ( int )( temp >> Q );
		return *this;
	}	
	template< int Q >
	fixed_32< Q > &fixed_32< Q >::operator/=( fixed_32< Q > rhs )
	{
		long long temp;

		temp = ( long long )data << Q;
		temp += rhs.data / 2;
		data = ( int )( temp / rhs.data );
		return *this;
	}	

	template< int Q >
	fixed_32< Q > &fixed_32< Q >::operator=( float rhs )
	{
		data = ( int )round( rhs * std::pow( 2.f,  Q ) );
		return *this;
	}
	template< int Q >
	fixed_32< Q > &fixed_32< Q >::operator+=( float rhs )
	{
		data += ( int )round( rhs * std::pow( 2.f,  Q ) );
		return *this;
	}
	template< int Q >
	fixed_32< Q > &fixed_32< Q >::operator-=( float rhs )
	{
		data -= ( int )round( rhs * std::pow( 2.f,  Q ) );
		return *this;
	}
	template< int Q >
	fixed_32< Q > &fixed_32< Q >::operator*=( float rhs )
	{
		int K;
		long long temp;

		K = ( 2 << ( Q - 1 ) );
		temp = ( long long )data * ( long long )round( rhs * std::pow( 2.f,  Q ) );
		temp += K;

		data = ( int )( temp >> Q );
		return *this;
	}
	template< int Q >
	fixed_32< Q > &fixed_32< Q >::operator/=( float rhs )
	{
		long long temp;
		int b;

		b = ( int )round( rhs * std::pow( 2.f,  Q ) );
		temp = ( long long )data << Q;
		temp += b / 2;
		data = ( int )( temp / b );
		return *this;
	}
	
	template< int Q >
	fixed_32< Q > &fixed_32< Q >::operator=( double rhs )
	{
		data = ( int )round( rhs * std::pow( 2.f, Q ) );
		return *this;
	}
	template< int Q >
	fixed_32< Q > &fixed_32< Q >::operator+=( double rhs )
	{
		data += ( int )round( rhs * std::pow( 2.f, Q ) );
		return *this;
	}
	template< int Q >
	fixed_32< Q > &fixed_32< Q >::operator-=( double rhs )
	{
		data -= ( int )round( rhs * std::pow( 2.f, Q ) );
		return *this;
	}
	template< int Q >
	fixed_32< Q > &fixed_32< Q >::operator*=( double rhs )
	{
		int K;
		long long temp;

		K = ( 2 << ( Q - 1 ) );
		temp = ( long long )data * ( long long )round( rhs * std::pow( 2.f,  Q ) );
		temp += K;

		data = ( int )( temp >> Q );
		return *this;
	}
	template< int Q >
	fixed_32< Q > &fixed_32< Q >::operator/=( double rhs )
	{
		long long temp;
		int b;

		b = ( int )round( rhs * std::pow( 2.f,  Q ) );
		temp = ( long long )data << Q;
		temp += b / 2;
		data = ( int )( temp / b );
		return *this;
	}

	template< int Q >
	fixed_32< Q > &fixed_32< Q >::operator++( )
	{
		data += 1 << Q;
		return *this;
	}
	template< int Q >
	fixed_32< Q > &fixed_32< Q >::operator--( )
	{
		data -= 1 << Q;
		return *this;
	}
	template< int Q >
	fixed_32< Q > &fixed_32< Q >::operator++( int )
	{
		data += 1 << Q;
		return *this;
	}
	template< int Q >
	fixed_32< Q > &fixed_32< Q >::operator--( int )
	{
		data -= 1 << Q;
		return *this;
	}

	
	// Non-member 32-bit functions
	template< int Q >
	bool operator==( fixed_32< Q > a, fixed_32< Q > b )
	{
		return a.data == b.data;
	}
	template< int Q >
	bool operator!=( fixed_32< Q > a, fixed_32< Q > b )
	{
		return a.data != b.data;
	}
	template< int Q >
	bool operator>=( fixed_32< Q > a, fixed_32< Q > b )
	{
		return a.data >= b.data;
	}
	template< int Q >
	bool operator<=( fixed_32< Q > a, fixed_32< Q > b )
	{
		return a.data <= b.data;
	}
	template< int Q >
	bool operator>( fixed_32< Q > a, fixed_32< Q > b )
	{
		return a.data > b.data;
	}
	template< int Q >
	bool operator<( fixed_32< Q > a, fixed_32< Q > b )
	{
		return a.data < b.data;	
	}
	
	template< int Q >
	fixed_32< Q > operator+( fixed_32< Q > a, fixed_32< Q > b )
	{
		fixed_32< Q > r;
		
		r.data = a.data + b.data;

		return r;
	}
	template< int Q >
	fixed_32< Q > operator-( fixed_32< Q > a, fixed_32< Q > b )
	{
		fixed_32< Q > r;
		
		r.data = a.data - b.data;

		return r;
	}
	template< int Q >
	fixed_32< Q > operator*( fixed_32< Q > a, fixed_32< Q > b )
	{
		fixed_32< Q > r;
		int K;
		long long temp;

		K = ( 1 << ( Q - 1 ) );
		temp = ( long long )a.data * ( long long )b.data;
		temp += K;

		r.data = ( long )( temp >> Q );
		return r;
	}
	template< int Q >
	fixed_32< Q > operator/( fixed_32< Q > a, fixed_32< Q > b )
	{
		fixed_32< Q > r;		
		long long temp;

		temp = ( long long )a.data << Q;
		temp += b.data / 2;
		r.data = ( long )( temp / b.data );
		
		return r;
	}
	
	template< int Q >
	fixed_32< Q > const operator+( fixed_32< Q > a )
	{
		fixed_32< Q > r;

		r.data = a.data;

		return r;
	}
	template< int Q >
	fixed_32< Q > const operator-( fixed_32< Q > a )
	{
		fixed_32< Q > r;

		r.data = -a.data;

		return r;
	}
	template< int Q >
	fixed_32< Q > abs( fixed_32< Q > a )
	{
		fixed_32< Q > r;
		
		r.data = r.data >= 0 ? r.data : -r.data;

		return r;
	}
}

	//------------------------------------
	// std::numberic_limits
	//

namespace std {
	template< int Q >
	class numeric_limits< vul::fixed_32< Q > > : public numeric_limits< float >
	{
	public:
#ifdef VUL_CPLUSPLUS11
		static constexpr bool is_signed = true;
		static constexpr bool is_exact = false;
		static constexpr bool is_modulo = false;
		static constexpr bool is_iec559 = true;
		static constexpr bool has_infinity = false;
		static constexpr bool has_quiet_NaN = false;

		static vul::fixed_32< Q > min( ) noexcept			{ vul::fixed_32< Q > f; f.data = 0; return f; }
		static vul::fixed_32< Q > lowest( ) noexcept		{ vul::fixed_32< Q > f; f.data = 0xffffffff; return f; }
		static vul::fixed_32< Q > max( ) noexcept			{ vul::fixed_32< Q > f; f.data = 0x7fffffff; return f; }
		static vul::fixed_32< Q > epsilon( ) noexcept		{ vul::fixed_32< Q > f; f.data = 0x00000001; return f; }
		static vul::fixed_32< Q > round_error( ) noexcept { vul::fixed_32< Q > f; f.data = 1 << ( Q - 1); return f; }
#else
		static const bool is_signed = true;
		static const bool is_exact = false;
		static const bool is_modulo = false;
		static const bool is_iec559 = true;
		static const bool has_infinity = false;
		static const bool has_quiet_NaN = false;

		static const vul::fixed_32< Q > min( )			{ vul::fixed_32< Q > f; f.data = 0; return f; }
		static const vul::fixed_32< Q > lowest( )		{ vul::fixed_32< Q > f; f.data = 0xffffffff; return f; }
		static const vul::fixed_32< Q > max( )			{ vul::fixed_32< Q > f; f.data = 0x7fffffff; return f; }
		static const vul::fixed_32< Q > epsilon( )		{ vul::fixed_32< Q > f; f.data = 0x00000001; return f; }
		static const vul::fixed_32< Q > round_error( )	{ vul::fixed_32< Q > f; f.data = 1 << ( Q - 1); return f; };

#endif
	};
}



#endif
