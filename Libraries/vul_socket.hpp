/*
 * Villains' Utility Library - Thomas Martin Schmid, 2014. Public domain¹
 *
 * This file contains a wrapper for sockets, abstracting OS differences away.
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
#ifndef VUL_SOCKET_HPP
#define VUL_SOCKET_HPP

/**
 * Define the correct OS
 */
#define VUL_WINDOWS

#include <cassert>
#if defined( VUL_WINDOWS )
	#define WIN32_LEAN_AND_MEAN
	#include <Windows.h>
	#include <WinSock2.h>
	#include <WS2tcpip.h>
#elif defined( VUL_LINUX )
	#include <sys/socket.h>
#elif defined( VUL_OSX )
	BAH
#else
	vul needs a system defined
#endif
#include "vul_types.h"
#include "vul_timer.h"

namespace vul {

	typedef unsigned long ui32_t;
	typedef unsigned short ui16_t;
	typedef unsigned char ui8_t;
	typedef char i8_t;

	/**
	 * Address struct
	 */
	struct address_t {
		sockaddr_in addr;
	};

	/**
	 * Packet abstraction struct.
	 */
	class packet_t {

	private:
		ui32_t size_bytes;
		i8_t *data;

	public:
		/**
		 * Create a new packet.
		 */
		packet_t( );
		/**
		 * Destroy the packet and free the data array.
		 */
		~packet_t( );

		/**
		 * Returns a const pointer to the data array.
		 * Using this supplies a promise not to alter the array.
		 */
		const i8_t *get_const_data_ptr( ) const;
		/** 
		 * Returns a pointer to the data array.
		 */
		i8_t *get_data_ptr( );
		/** 
		 * Returns the size of the data array.
		 */
		const ui32_t get_size( ) const;
		/**
		 * Resizes the data array.
		 */
		void resize( ui32_t bytes );
	};

	
	packet_t::packet_t( )
	{
		this->data = NULL;
		this->size_bytes = 0;
	}
	packet_t::~packet_t( )
	{
		if( this->data != NULL ) {
			delete [] this->data;
		}
	}

	const i8_t *packet_t::get_const_data_ptr( ) const
	{
		return this->data;
	}
	i8_t *packet_t::get_data_ptr( )
	{
		return this->data;
	}
	const ui32_t packet_t::get_size( ) const
	{
		return this->size_bytes;
	}
	void packet_t::resize( ui32_t bytes )
	{
		if( this->size_bytes ) {
			delete [] this->data;
			this->data = NULL;
		}

		this->data = new i8_t[ bytes ];
		this->size_bytes = bytes;
		assert( this->data );
	}


	/**
	 * Error struct contains the last error number
	 * encountered and the function in which it was encountered.
	 */
	struct socket_error_t {
		int code;
		const char *func;
		socket_error_t( int i, const char *f )
		{
			code = i;
			func = f;
		}
	};

	/**
	 * Socket abstraction class.
	 */
	class socket_t {

	private:
		SOCKET sock;
		address_t bind_addr;
		socket_error_t last_err;

		address_t create_address( const char *ip, ui16_t port );
		void destroy_address( const address_t &a );
		
		void open( );
		void close( );
				
	public:
		/**
		 * Creates a socket on the given interface and port, then opens the socket.
		 */
		socket_t( const char *ip, ui16_t port );
		/**
		 * Frees any resources of the socket. Closes it if needed.
		 */
		~socket_t( );

		void send_packet( const packet_t &p, ui32_t timeout_millis = 0, bool no_delay = true );
		void receive_packet( packet_t *p, ui32_t timeout_millis = 0 );

		void listen_on_port( ui16_t port, const char *ip = NULL );
		void accept_connection( socket_t *ret );
		void connect_to_remote( const char *ip, ui16_t port );

		socket_error_t get_last_error( );
	};
	
	address_t create_address( const char *ip, ui16_t port )
	{
		address_t a;
		
		a.addr.sin_family = AF_INET;
		a.addr.sin_addr.s_addr = inet_addr( ip );
		a.addr.sin_port = htons( port );

		return a;
	}
	void destroy_address( const address_t &a )
	{
		freeaddrinfo( ( PADDRINFOA )&a.addr );
	}

	void socket_t::open( )
	{
		// Open the socket
#if defined( VUL_WINDOWS )
		WSAData data;
		WSAStartup( MAKEWORD( 2, 2 ), &data );
		this->sock = INVALID_SOCKET;
		this->sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if( this->sock == INVALID_SOCKET ) {
			int ret = WSAGetLastError( );
			WSACleanup( );
			last_err.code = ret;
			last_err.func = "open";
		}
#elif defined( VUL_LINUX )
		na
#elif defined( VUL_OSX )
		na
#endif
	}

	void socket_t::close( )
	{
#if defined( VUL_WINDOWS )
		int ret = shutdown( this->sock, SD_SEND );
		if( ret == SOCKET_ERROR ) {
			closesocket( this->sock );
			ret = WSAGetLastError( );
			WSACleanup( );
			last_err.code = ret;
			last_err.func = "close";
		}
#elif defined( VUL_LINUX )
			na
#elif defined( VUL_OSX )
			na
#endif
		this->sock = NULL;
	}

	socket_t::socket_t( const char *ip, ui16_t port )
	{
		// Create the address struct
		bind_addr = create_address( ip, port );
		// Open the socket
		this->sock = NULL;
		this->open( );
		// Initialize no-error
		last_err.code = 0;
		last_err.func = NULL;
	}
	socket_t::~socket_t( )
	{
		// Close the socket if needed
		if( this->sock != NULL ) {
			this->close( );
		}

		// Free the address struct
		destroy_address( bind_addr );
	}

	void socket_t::send_packet( const packet_t &p, ui32_t timeout_millis = 0, bool no_delay = true )
	{
		if( no_delay ) {
			int flag = 1;
			setsockopt( this->sock, IPPROTO_TCP, TCP_NODELAY, ( char* )&flag, sizeof( int ) );
		}
		if( timeout_millis ) {
			setsockopt( this->sock, SOL_SOCKET, SO_SNDTIMEO, ( char* )&timeout_millis, sizeof( ui32_t ) );
		}
		int ret = send( this->sock, p.get_const_data_ptr( ), p.get_size( ), 0 );
		if( ret == SOCKET_ERROR ) {
			last_err.code = ret;
			last_err.func = "send_packet";
		}
	}
	void socket_t::receive_packet( packet_t *p, ui32_t timeout_millis = 0 )
	{
		if( timeout_millis ) {
			setsockopt( this->sock, SOL_SOCKET, SO_RCVTIMEO, ( char* )&timeout_millis, sizeof( ui32_t ) );
		}
		int res;
		do {
			res = recv( this->sock, ( char* )p->get_data_ptr( ), p->get_size( ), 0 );
			if( res > 0 ) {
				p->resize( p->get_size( ) + res );
			} else if( res == 0 ) {
				// Socket closed!
				last_err.code = res;
				last_err.func = "receive_packet";
			} else {
				// Error!
				last_err.code = WSAGetLastError( );
				last_err.func = "receive_packet";
			}
		} while ( res > 0 );
	}

	void socket_t::listen_on_port( ui16_t port, const char *ip = NULL )
	{
		address_t a = create_address( ip, port );
		/* Bind first */
		int ret = bind( this->sock, ( SOCKADDR* )&a.addr, sizeof( a.addr ) );
		if( ret == SOCKET_ERROR ) {
			ret = WSAGetLastError( );
			WSACleanup( );
			last_err.code = ret;
			last_err.func = "listen_on_port";
		}
		/* Then listen */
		ret = listen( this->sock, SOMAXCONN );
		if( ret == SOCKET_ERROR ) {
			ret = WSAGetLastError( );
			WSACleanup( );
			last_err.code = ret;
			last_err.func = "listen_on_port";
		}
	}
	void socket_t::accept_connection( socket_t *ret )
	{
		ret->sock = accept( this->sock, NULL, NULL );
		if( ret->sock == INVALID_SOCKET ) {
			int ret = WSAGetLastError( );
			WSACleanup( );
			last_err.code = ret;
			last_err.func = "accept_connection";
		}
	}
	void socket_t::connect_to_remote( const char *ip, ui16_t port )
	{
		address_t a = create_address( ip, port );
		int ret = connect( this->sock, ( SOCKADDR* )&a.addr, sizeof( a.addr ) );
		if( ret == SOCKET_ERROR ) {
			closesocket( this->sock );
			ret = WSAGetLastError( );
			WSACleanup( );
			last_err.code = ret;
			last_err.func = "connect_to_remote";
		}
	}
	socket_error_t socket_t::get_last_error( )
	{
		return last_err;
	}
}

#endif