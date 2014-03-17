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
#ifndef VUL_SOCKET_H
#define VUL_SOCKET_H

/**
 * Define the correct OS
 */
#define VUL_WINDOWS
/**
 * Define in exactly _one_ C/CPP file.
 */
//#define VUL_DEFINE

#include <stdlib.h>
#include <malloc.h>
#if defined( VUL_WINDOWS )
	#define WIN32_LEAN_AND_MEAN
	#include <Windows.h>
	#include <WinSock2.h>
	#include <WS2tcpip.h>
#elif defined( VUL_LINUX )
	#include <sys/socket.h>
#elif defined( VUL_OSX )
	BAH
#endif
#include "vul_types.h"
#include "vul_timer.h"

/**
 * Socket abstraction.
 */
struct vul_socket_t {
	SOCKET socket;
};

/**
 * Adderss abstraction.
 */
struct vul_address_t {
	sockaddr_in addr;
};

/**
 * Packet abstraction
 */
struct vul_packet_t {
	ui32_t size_bytes;
	ui8_t *data;
};

/**
 * Sends the given packet to the given socket.
 * Includes an optional timeout value.
 */
#ifndef VUL_DEFINE
int vul_socket_send( vul_socket_t *s, vul_packet_t *p, ui32_t timeout_millis = 0, bool no_delay = true );
#else
int vul_socket_send( vul_socket_t *s, vul_packet_t *p, ui32_t timeout_millis = 0, bool no_delay = true )
{
	if( no_delay ) {
		int flag = 1;
		setsockopt( s->socket, IPPROTO_TCP, TCP_NODELAY, ( char* )&flag, sizeof( int ) );
	}
	if( timout_millis ) {
		setsockopt( s->socket, SOL_SOCKET, SO_SNDTIMEO, ( char* )&timeout_millis, sizeof( ui32_t ) );
	}
	return send( s->socket, p->data, p->size_bytes, 0 );
}
#endif

/**
 * Recieves a packet from a socket. Allocates the packet data pointer inside.
 * Includes an optional timeout value.
 */
#ifndef VUL_DEFINE
int vul_socket_receive( vul_socket_t *s, vul_packet_t *p, ui32_t timeout_millis = 0 );
#else
int vul_socket_receive( vul_socket_t *s, vul_packet_t *p, ui32_t timeout_millis = 0 )
{
	if( timout_millis ) {
		setsockopt( s->socket, SOL_SOCKET, SO_RCVTIMEO, ( char* )&timeout_millis, sizeof( ui32_t ) );
	}
	int res;
	do {
		res = recv( s->socket, p->data, p->size_bytes, 0 );
		if( res > 0 ) {
			if( !p->size_bytes ) {
				p->size_bytes = res;
				p->data = ( char* )malloc( p->size_bytes );
				assert( p->data != NULL ); // Make sure malloc didn't fail
			} else {
				p->size_bytes += res;
				p->data = ( char* )realloc( p->data, p->size_bytes );
				assert( p->data != NULL ); // Make sure realloc didn't fail
			}
		} else if( res == 0 ) {
			// Socket closed!
			return res;
		} else {
			// Error!
			return WSAGetLastError( );
		}
	} while ( res > 0 );
}
#endif


/**
 * Binds to the given port, opens a listening socket.
 */
#ifndef VUL_DEFINE
int vul_socket_listen( vul_socket_t *s, vul_address_t *a );
#else
int vul_socket_listen( vul_socket_t *s, vul_address_t *a )
{
	/* Bind first */
	int ret = bind( s->socket, ( SOCKADDR* )&a->addr, sizeof( a->addr ) );
	if( ret == SOCKET_ERROR ) {
		ret = WSAGetLastError( );
		WSACleanup( );
		return ret;
	}
	/* Then listen */
	ret = listen( s->socket, SOMAXCONN );
	if( ret == SOCKET_ERROR ) {
		ret = WSAGetLastError( );
		WSACleanup( );
		return ret;
	}
	return 0;
}
#endif

/**
 * Accepts connections on the given port. Must be listening already,
 * so call vul_socket_liste( 'listen' ) first.
 */
#ifndef VUL_DEFINE
int vul_socket_accept( vul_socket_t *listen, vul_socket_t *ret );
#else
int vul_socket_accept( vul_socket_t *listen, vul_socket_t *ret )
{
	ret->socket = accept( listen->socket, NULL, NULL );
	if( ret->socket == INVALID_SOCKET ) {
		int ret = WSAGetLastError( );
		WSACleanup( );
		return ret;
	}
	return 0;
}
#endif

/**
 * Connects the given socket to the given address
 */
#ifndef VUL_DEFINE
int vul_socket_connect( vul_socket_t *s, vul_address_t *a );
#else
int vul_socket_connect( vul_socket_t *s, vul_address_t *a )
{
	int ret = connect( s->socket, ( SOCKADDR* )&a->addr, sizeof( a->addr ) );
	if( ret == SOCKET_ERROR ) {
		closesocket( s->socket );
		ret = WSAGetLastError( );
		WSACleanup( );
		return ret;
	}
	return 0;
}
#endif

/**
 * Populate the given address struct with the given IP string and port.
 */
#ifndef VUL_DEFINE
int vul_socket_address_create( vul_address_t *a, const char *ip, ui16_t port );
#else
int vul_socket_address_create( vul_address_t *a, const char *ip, ui16_t port )
{
	a->addr.sin_family = AF_INET;
	a->addr.sin_addr.s_addr = inet_addr( ip );
	a->addr.sin_port = htons( port );
}
#endif

/**
 * Destroy and address struct
 */
#ifndef VUL_DEFINE
int vul_socket_address_destroy( vul_address_t *a );
#else
int vul_socket_address_destroy( vul_address_t *a )
{
	freeaddrinfo( ( PADDRINFOA )&a->addr );
}
#endif

#endif