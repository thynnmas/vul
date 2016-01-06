/*
 * Villains' Utility Library - Thomas Martin Schmid, 2016. Public domain¹
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
 * Define in exactly _one_ C/CPP file.
 */
//#define VUL_DEFINE

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#if defined( VUL_WINDOWS )
	#include <malloc.h>
	#define WIN32_LEAN_AND_MEAN
	#include <stdio.h>
	#include <Windows.h>
	#include <WinSock2.h>
	#include <WS2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
#elif defined( VUL_LINUX )
	#include <malloc.h>
	#include <sys/types.h>
	#include <sys/socket.h>
    #include <arpa/inet.h>
    #include <netinet/in.h>
	#include <netinet/tcp.h>
#elif defined( VUL_OSX )
	BAH
#endif

#define VUL_SOCKET_DEFAULT_BUFFER_LENGTH 512
		
/**
 * Adderss abstraction.
 */
typedef struct vul_address_t {
#ifdef VUL_WINDOWS
	struct addrinfo hints;
	struct addrinfo *addr;
#else
	struct sockaddr_in addr;
#endif
} vul_address_t;


/**
 * Socket abstraction.
 */
typedef struct vul_socket_t {
#ifdef VUL_WINDOWS
	SOCKET socket;
#else
	int socket;
#endif
} vul_socket_t;

/**
 * Packet abstraction
 */
typedef struct vul_packet_t {
	uint32_t size_bytes;
	uint8_t *data;
} vul_packet_t;

#endif

/**
 * Initialize winsock if on windows, does nothing on posix systems
 */
#ifndef VUL_DEFINE
int vul_socket_init( );
#else
int vul_socket_init( )
{
#ifdef VUL_WINDOWS
	WSADATA wsa;
	int res;

	res = WSAStartup( MAKEWORD(2, 2), &wsa );
	return res;
#else
	return 0;
#endif
}
#endif

/**
 * Cleans up winsock, does nothing on posix systems
 */
#ifndef VUL_DEFINE
void vul_socket_destroy( );
#else
void vul_socket_destroy( )
{
#ifdef VUL_WINDOWS
	WSACleanup( );
#endif
}
#endif


/**
 * Sends the given packet to the given socket.
 * Includes an optional timeout value.
 */
#ifndef VUL_DEFINE
int vul_socket_send( vul_socket_t *s, vul_packet_t *p, uint32_t timeout_millis, uint32_t no_delay );
#else
int vul_socket_send( vul_socket_t *s, vul_packet_t *p, uint32_t timeout_millis, uint32_t no_delay )
{
	if( no_delay ) {
		int flag = 1;
		setsockopt( s->socket, IPPROTO_TCP, TCP_NODELAY, ( char* )&flag, sizeof( int ) );
	}
	if( timeout_millis ) {
		setsockopt( s->socket, SOL_SOCKET, SO_SNDTIMEO, ( char* )&timeout_millis, sizeof( uint32_t ) );
	}
	return send( s->socket, p->data, p->size_bytes, 0 );
}
#endif

/**
 * Recieves a packet from a socket. Allocates the packet data pointer inside.
 * Includes an optional timeout value.
 */
#ifndef VUL_DEFINE
int vul_socket_receive( vul_socket_t *s, vul_packet_t *p, uint32_t timeout_millis );
#else
int vul_socket_receive( vul_socket_t *s, vul_packet_t *p, uint32_t timeout_millis )
{
	char buffer[ VUL_SOCKET_DEFAULT_BUFFER_LENGTH ];
	int res;
	memset( p, 0, sizeof( vul_packet_t ) );

	if( timeout_millis ) {
		setsockopt( s->socket, SOL_SOCKET, SO_RCVTIMEO, ( char* )&timeout_millis, sizeof( uint32_t ) );
	}
	do {
		res = recv( s->socket, buffer, VUL_SOCKET_DEFAULT_BUFFER_LENGTH, 0 );
		if( res > 0 ) {
			if( !p->size_bytes ) {
				p->size_bytes = res;
				p->data = ( char* )malloc( p->size_bytes );
				if( p->data == NULL ) return -1;
				memcpy( p->data, buffer, res );
			} else {
				size_t oldsize = p->size_bytes;
				p->size_bytes += res;
				p->data = ( char* )realloc( p->data, p->size_bytes );
				if( p->data == NULL ) return -1;
				memcpy( p->data + oldsize, buffer, res );
			}
		} else if( res == 0 ) {
			// Socket closed!
			return res;
		} else {
			// Error!
#ifdef VUL_WINDOWS
			return WSAGetLastError( );
#else
			return res;
#endif
		}
	} while ( res > 0 && res == VUL_SOCKET_DEFAULT_BUFFER_LENGTH );
	return 0;
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
	int ret;
#ifdef VUL_WINDOWS
	/* Create the socket */
	s->socket = socket( a->addr->ai_family, a->addr->ai_socktype, a->addr->ai_protocol );
	if( s->socket == INVALID_SOCKET ) {
		ret = WSAGetLastError( );
		return ret;
	}
	/* Bind first */
	ret = bind( s->socket, a->addr->ai_addr, ( int )a->addr->ai_addrlen );
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
#else
	/* Create the socket */
	s->socket = socket( PF_INET, SOCK_STREAM, IPPROTO_TCP );
	if( s->socket < 0 ) {
		return s->socket;
	}
	/* Bind first */
	ret = bind( s->socket, ( struct sockaddr *)&a->addr, sizeof( a->addr ) );
	if( ret < 0 ) {
		close( s->socket );
		return ret;
	}
	/* Then listen */
	ret = listen( s->socket, SOMAXCONN );
	if( ret < 0 ) {
		close( s->socket );
	}
	return ret;
#endif
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
#ifdef VUL_WINDOWS
	ret->socket = accept( listen->socket, NULL, NULL );
	if( ret->socket == INVALID_SOCKET ) {
		int ret = WSAGetLastError( );
		WSACleanup( );
		return ret;
	}
	return 0;
#else
	ret->socket = accept( listen->socket, ( struct sockaddr * )NULL, NULL );
	return 0;
#endif
}
#endif

/**
 * Closes a simple socket 
 */
#ifndef VUL_DEFINE
void vul_socket_close( vul_socket_t *s );
#else
void vul_socket_close( vul_socket_t *s )
{
#ifdef VUL_WINDOWS
	closesocket( s->socket );
#else
	close( s->socket );
#endif
}
#endif


/**
 * Closes a socket sending a shutdown signal first.
 */
#ifndef VUL_DEFINE
int vul_socket_close_polite( vul_socket_t *s );
#else
int vul_socket_close_polite( vul_socket_t *s )
{
#ifdef VUL_WINDOWS
	int res;
	res = shutdown( s->socket, SD_SEND );
	if( res == SOCKET_ERROR ) {
		closesocket( s->socket );
		res = WSAGetLastError( );
		return res;
	}
	closesocket( s->socket );
	return 0;
#else
	close( s->socket );
	return 0;
#endif
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
	int ret;
#ifdef VUL_WINDOWS
	s->socket = socket( a->addr->ai_family, a->addr->ai_socktype, a->addr->ai_protocol );
	if( s->socket == INVALID_SOCKET ) {
		ret = WSAGetLastError( );
		return ret;
	}
	ret = connect( s->socket, a->addr->ai_addr, ( int )a->addr->ai_addrlen );
	if( ret == SOCKET_ERROR ) {
		closesocket( s->socket );
		ret = WSAGetLastError( );
		WSACleanup( );
		return ret;
	}
	return 0;
#else
	s->socket = socket( PF_INET, SOCK_STREAM, IPPROTO_TCP );
	if( s->socket < 0 ) {
		return s->socket;
	}
	return connect( s->socket, ( struct sockaddr * )&a->addr, sizeof( a->addr ) );
#endif
}
#endif

/**
 * Populate the given address struct with the given IP string and port.
 */
#ifndef VUL_DEFINE
int vul_socket_address_create( vul_address_t *a, const char *ip, uint16_t port );
#else
int vul_socket_address_create( vul_address_t *a, const char *ip, uint16_t port )
{
#ifdef VUL_WINDOWS
	char str[ 16 ];
	
	ZeroMemory( &a->hints, sizeof( a->hints ) );
	a->hints.ai_family = AF_INET;
	a->hints.ai_socktype = SOCK_STREAM;
	a->hints.ai_protocol = IPPROTO_TCP;
	a->hints.ai_flags = AI_PASSIVE;
	sprintf_s( str, 15, "%d", port );
	return getaddrinfo( ip, str, &a->hints, &a->addr );	
#else
	memset( &a->addr, 0, sizeof( a->addr ) );
	a->addr.sin_family = AF_INET;
	if( ip ) {
		a->addr.sin_addr.s_addr = inet_addr( ip );
	} else {
		a->addr.sin_addr.s_addr = htonl( INADDR_ANY );
	}
	a->addr.sin_port = htons( port );
#endif
}
#endif

/**
 * Destroy and address struct
 */
#ifndef VUL_DEFINE
void vul_socket_address_destroy( vul_address_t *a );
#else
void vul_socket_address_destroy( vul_address_t *a )
{
	freeaddrinfo( a->addr );
}
#endif