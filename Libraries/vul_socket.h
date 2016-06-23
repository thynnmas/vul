/*
 * Villains' Utility Library - Thomas Martin Schmid, 2015. Public domain?
 *
 * This file contains a wrapper for sockets, abstracting OS differences away.
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
	#include <unistd.h>
	#include <sys/types.h>
	#include <sys/socket.h>
   #include <arpa/inet.h>
   #include <netinet/in.h>
	#include <netinet/tcp.h>
	#include <netdb.h>
#elif defined( VUL_OSX )
	#include <CoreFoundation/CoreFoundation.h>
	#include <sys/types.h>
	#include <sys/socket.h>
   #include <arpa/inet.h>
   #include <netinet/in.h>
	#include <netinet/tcp.h>
#endif

#define VUL_SOCKET_DEFAULT_BUFFER_LENGTH 512

/**
 * Adderss abstraction.
 */
typedef struct vul_address {
#ifdef VUL_WINDOWS
	struct addrinfo hints;
	struct addrinfo *addr;
#else
	struct sockaddr_in addr;
#endif
} vul_address;


/**
 * Socket abstraction.
 */
typedef struct vul_socket {
#ifdef VUL_WINDOWS
	SOCKET socket;
#else
	int socket;
#endif
} vul_socket;

/**
 * Packet abstraction
 */
typedef struct vul_packet {
	uint32_t size_bytes;
	uint8_t *data;
} vul_packet;

#ifdef _cplusplus
extern "C" {
#endif
/**
 * Initialize winsock if on windows, does nothing on posix systems
 */
int vul_socket_init( );
/**
 * Cleans up winsock, does nothing on posix systems
 */
void vul_socket_destroy( );
/**
 * Sends the given packet to the given socket.
 * Includes an optional timeout value.
 */
int vul_socket_send( vul_socket *s, vul_packet *p, unsigned int timeout_millis, unsigned int no_delay );
/**
 * Recieves a packet from a socket. Allocates the packet data pointer inside.
 * Includes an optional timeout value.
 */
int vul_socket_receive( vul_socket *s, vul_packet *p, unsigned int timeout_millis );
/**
 * Binds to the given port, opens a listening socket.
 */
int vul_socket_listen( vul_socket *s, vul_address *a );
/**
 * Accepts connections on the given port. Must be listening already,
 * so call vul_socket_liste( 'listen' ) first.
 */
int vul_socket_accept( vul_socket *listen, vul_socket *ret );
/**
 * Closes a simple socket 
 */
void vul_socket_close( vul_socket *s );
/**
 * Closes a socket sending a shutdown signal first.
 */
int vul_socket_close_polite( vul_socket *s );
/**
 * Connects the given socket to the given address
 */
int vul_socket_connect( vul_socket *s, vul_address *a );
/**
 * Populate the given address struct with the given IP string and port.
 */
int vul_socket_address_create( vul_address *a, const char *ip, unsigned short port );
/**
 * Destroy and address struct
 */
void vul_socket_address_destroy( vul_address *a );
#ifdef _cplusplus
}
#endif
#endif

#ifdef VUL_DEFINE

#ifdef _cplusplus
extern "C" {
#endif

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

void vul_socket_destroy( )
{
#ifdef VUL_WINDOWS
	WSACleanup( );
#endif
}

int vul_socket_send( vul_socket *s, vul_packet *p, unsigned int timeout_millis, unsigned int no_delay )
{
	if( no_delay ) {
		int flag = 1;
		setsockopt( s->socket, IPPROTO_TCP, TCP_NODELAY, ( char* )&flag, sizeof( int ) );
	}
	if( timeout_millis ) {
		setsockopt( s->socket, SOL_SOCKET, SO_SNDTIMEO, ( char* )&timeout_millis, sizeof( unsigned int ) );
	}
	return send( s->socket, p->data, p->size_bytes, 0 );
}

int vul_socket_receive( vul_socket *s, vul_packet *p, unsigned int timeout_millis )
{
	char buffer[ VUL_SOCKET_DEFAULT_BUFFER_LENGTH ];
	int res;
	memset( p, 0, sizeof( vul_packet ) );

	if( timeout_millis ) {
		setsockopt( s->socket, SOL_SOCKET, SO_RCVTIMEO, ( char* )&timeout_millis, sizeof( unsigned int ) );
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

int vul_socket_listen( vul_socket *s, vul_address *a )
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

int vul_socket_accept( vul_socket *listen, vul_socket *ret )
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

void vul_socket_close( vul_socket *s )
{
#ifdef VUL_WINDOWS
	closesocket( s->socket );
#else
	close( s->socket );
#endif
}

int vul_socket_close_polite( vul_socket *s )
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

int vul_socket_connect( vul_socket *s, vul_address *a )
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

int vul_socket_address_create( vul_address *a, const char *ip, unsigned short port )
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
		if( a->addr.sin_addr.s_addr == INADDR_NONE ) {
			struct hostent *he;
			if( ( he = gethostbyname( ip ) ) == NULL ) {
				return -1;
			}
			memcpy( &a->addr.sin_addr, he->h_addr_list[ 0 ], he->h_length );
		}
	} else {
		a->addr.sin_addr.s_addr = htonl( INADDR_ANY );
	}
	a->addr.sin_port = htons( port );
	return a->addr.sin_addr.s_addr == INADDR_NONE;
#endif
}

void vul_socket_address_destroy( vul_address *a )
{
#ifdef VUL_WINDOWS
	freeaddrinfo( a->addr );
#endif
}

#ifdef _cplusplus
}
#endif

#endif
