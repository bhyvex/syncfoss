/* sockwrap.h */

/* Berkley/WinSock socket API wrappers */

/* $Id: sockwrap.h,v 1.52 2014/11/16 02:58:20 deuce Exp $ */

/****************************************************************************
 * @format.tab-size 4		(Plain Text/Source Code File Header)			*
 * @format.use-tabs true	(see http://www.synchro.net/ptsc_hdr.html)		*
 *																			*
 * Copyright 2010 Rob Swindell - http://www.synchro.net/copyright.html		*
 *																			*
 * This library is free software; you can redistribute it and/or			*
 * modify it under the terms of the GNU Lesser General Public License		*
 * as published by the Free Software Foundation; either version 2			*
 * of the License, or (at your option) any later version.					*
 * See the GNU Lesser General Public License for more details: lgpl.txt or	*
 * http://www.fsf.org/copyleft/lesser.html									*
 *																			*
 * Anonymous FTP access to the most recent released source is available at	*
 * ftp://vert.synchro.net, ftp://cvs.synchro.net and ftp://ftp.synchro.net	*
 *																			*
 * Anonymous CVS access to the development source and modification history	*
 * is available at cvs.synchro.net:/cvsroot/sbbs, example:					*
 * cvs -d :pserver:anonymous@cvs.synchro.net:/cvsroot/sbbs login			*
 *     (just hit return, no password is necessary)							*
 * cvs -d :pserver:anonymous@cvs.synchro.net:/cvsroot/sbbs checkout src		*
 *																			*
 * For Synchronet coding style and modification guidelines, see				*
 * http://www.synchro.net/source.html										*
 *																			*
 * You are encouraged to submit any modifications (preferably in Unix diff	*
 * format) via e-mail to mods@synchro.net									*
 *																			*
 * Note: If this box doesn't appear square, then you need to fix your tabs.	*
 ****************************************************************************/

#ifndef _SOCKWRAP_H
#define _SOCKWRAP_H

#include "gen_defs.h"	/* BOOL */

/***************/
/* OS-specific */
/***************/
#if defined(_WIN32)	/* Use WinSock */

#ifndef _WINSOCKAPI_
	#include <winsock2.h>	/* socket/bind/etc. */
	#include <mswsock.h>	/* Microsoft WinSock2 extensions */
#if defined(__BORLANDC__)
// Borland C++ builder 6 comes with a broken ws2tcpip.h header for GCC.
#define _MSC_VER 7
#endif
    #include <ws2tcpip.h>	/* More stuff */
#if defined(__BORLANDC__)
#undef _MSC_VER
#endif
	#include <wspiapi.h>	/* getaddrinfo() for Windows 2k */
	#define SOCK_MAXADDRLEN sizeof(SOCKADDR_STORAGE)
	/* Let's agree on a standard WinSock symbol here, people */
	#define _WINSOCKAPI_
#endif

#elif defined __unix__		/* Unix-variant */
							
#include <netdb.h>			/* gethostbyname */
#include <sys/types.h>		/* For u_int32_t on FreeBSD */
#include <netinet/in.h>		/* IPPROTO_IP */
#include <sys/un.h>
/* define _BSD_SOCKLEN_T_ in order to define socklen_t on darwin */
#ifdef __DARWIN__
#define _BSD_SOCKLEN_T_	int
#endif
#include <sys/socket.h>		/* socket/bind/etc. */
#include <sys/time.h>		/* struct timeval */
#include <arpa/inet.h>		/* inet_ntoa */
#include <netinet/tcp.h>	/* TCP_NODELAY */
#include <unistd.h>			/* close */
#if defined(__solaris__)
	#include <sys/filio.h>  /* FIONBIO */
	#define INADDR_NONE -1L
#else
	#include <sys/ioctl.h>	/* FIONBIO */
#endif

#endif

#include <errno.h>		/* errno */
#include "wrapdll.h"	/* DLLEXPORT/DLLCALL */

typedef struct {
	char*	name;
	int		type;		/* Supported socket types (or 0 for unspecified) */
	int		level;
	int		value;
} socket_option_t;

/*
 * Fancy sockaddr_* union
 */
union xp_sockaddr {
	struct sockaddr			addr;
	struct sockaddr_in		in;
	struct sockaddr_in6		in6;
#ifndef _WIN32
	struct sockaddr_un		un;
#endif
	struct sockaddr_storage	store;
};

#define xp_sockaddr_len(a) ((((struct sockaddr *)a)->sa_family == AF_INET6) ? sizeof(struct sockaddr_in6) : sizeof(struct sockaddr_in))

 

/**********************************/
/* Socket Implementation-specific */
/**********************************/
#if defined(_WINSOCKAPI_)

#undef  EINTR
#define EINTR			(WSAEINTR-WSABASEERR)
#undef  ENOTSOCK
#define ENOTSOCK		(WSAENOTSOCK-WSABASEERR)
#undef  EMSGSIZE
#define EMSGSIZE		(WSAEMSGSIZE-WSABASEERR)
#undef  EWOULDBLOCK
#define EWOULDBLOCK		(WSAEWOULDBLOCK-WSABASEERR)

#ifndef EPROTOTYPE
#define EPROTOTYPE		(WSAEPROTOTYPE-WSABASEERR)
#endif
#ifndef ENOPROTOOPT
#define ENOPROTOOPT		(WSAENOPROTOOPT-WSABASEERR)
#endif
#ifndef EPROTONOSUPPORT
#define EPROTONOSUPPORT	(WSAEPROTONOSUPPORT-WSABASEERR)
#endif
#ifndef ESOCKTNOSUPPORT
#define ESOCKTNOSUPPORT	(WSAESOCKTNOSUPPORT-WSABASEERR)
#endif
#ifndef EOPNOTSUPP
#define EOPNOTSUPP		(WSAEOPNOTSUPP-WSABASEERR)
#endif
#ifndef EPFNOSUPPORT
#define EPFNOSUPPORT	(WSAEPFNOSUPPORT-WSABASEERR)
#endif
#ifndef EAFNOSUPPORT
#define EAFNOSUPPORT	(WSAEAFNOSUPPORT-WSABASEERR)
#endif

#undef  EADDRINUSE
#define EADDRINUSE		(WSAEADDRINUSE-WSABASEERR)
#undef  EADDRNOTAVAIL
#define EADDRNOTAVAIL	(WSAEADDRNOTAVAIL-WSABASEERR)
#undef  ECONNABORTED
#define ECONNABORTED	(WSAECONNABORTED-WSABASEERR)
#undef  ECONNRESET
#define ECONNRESET		(WSAECONNRESET-WSABASEERR)
#undef  ENOBUFS
#define ENOBUFS			(WSAENOBUFS-WSABASEERR)
#undef  EISCONN
#define EISCONN			(WSAEISCONN-WSABASEERR)
#undef  ENOTCONN
#define ENOTCONN		(WSAENOTCONN-WSABASEERR)
#undef  ESHUTDOWN
#define ESHUTDOWN		(WSAESHUTDOWN-WSABASEERR)
#undef  ETIMEDOUT
#define ETIMEDOUT		(WSAETIMEDOUT-WSABASEERR)
#undef  ECONNREFUSED
#define ECONNREFUSED	(WSAECONNREFUSED-WSABASEERR)
#undef	EINPROGRESS
#define EINPROGRESS		(WSAEINPROGRESS-WSABASEERR)

/* for shutdown() */
#define SHUT_RD			SD_RECEIVE
#define SHUT_WR			SD_SEND
#define SHUT_RDWR		SD_BOTH

#define s_addr			S_un.S_addr
#define sa_family_t		ushort

static  int wsa_error;
#define ERROR_VALUE		((wsa_error=WSAGetLastError())>0 ? wsa_error-WSABASEERR : wsa_error)
#define sendsocket(s,b,l)	send(s,b,l,0)

/* For getaddrinfo() */
#ifndef AI_ADDRCONFIG
# define AI_ADDRCONFIG 0x400 // Vista or later
#endif
#ifndef AI_NUMERICSERV
# define AI_NUMERICSERV 0		// Not supported by Win32
#endif

#else	/* BSD sockets */

/* WinSock-isms */
#define HOSTENT			struct hostent
#define IN_ADDR			struct in_addr
#define SOCKADDR		struct sockaddr
#define SOCKADDR_IN		struct sockaddr_in
#define LINGER			struct linger
#define SOCKET			int
#define SOCKET_ERROR	-1
#define INVALID_SOCKET  (SOCKET)(~0)
#define closesocket		close
#define ioctlsocket		ioctl
#define ERROR_VALUE		errno
#define sendsocket		write		/* FreeBSD send() is broken */

#ifdef __WATCOMC__
	#define socklen_t		int
#endif

#endif	/* __unix__ */

#ifdef __cplusplus
extern "C" {
#endif

DLLEXPORT socket_option_t* DLLCALL
		getSocketOptionList(void);
DLLEXPORT int DLLCALL		getSocketOptionByName(const char* name, int* level);

DLLEXPORT int DLLCALL		sendfilesocket(int sock, int file, off_t* offset, off_t count);
DLLEXPORT int DLLCALL		recvfilesocket(int sock, int file, off_t* offset, off_t count);
DLLEXPORT BOOL DLLCALL	socket_check(SOCKET sock, BOOL* rd_p, BOOL* wr_p, DWORD timeout);
DLLEXPORT int DLLCALL 	retry_bind(SOCKET s, const struct sockaddr *addr, socklen_t addrlen
				   ,uint retries, uint wait_secs, const char* prot
				   ,int (*lprintf)(int level, const char *fmt, ...));
DLLEXPORT int DLLCALL		nonblocking_connect(SOCKET, struct sockaddr*, size_t, unsigned timeout /* seconds */);
DLLEXPORT union xp_sockaddr* DLLCALL inet_ptoaddr(char *addr_str, union xp_sockaddr *addr, size_t size);
DLLEXPORT const char* DLLCALL inet_addrtop(union xp_sockaddr *addr, char *dest, size_t size);
DLLEXPORT uint16_t DLLCALL inet_addrport(union xp_sockaddr *addr);
DLLEXPORT void DLLCALL inet_setaddrport(union xp_sockaddr *addr, uint16_t port);

#ifdef __cplusplus
}
#endif

#ifndef IPPORT_HTTP
#define IPPORT_HTTP			80
#endif
#ifndef IPPORT_HTTPS
#define IPPORT_HTTPS			443
#endif
#ifndef IPPORT_FTP
#define IPPORT_FTP			21
#endif
#ifndef IPPORT_TELNET
#define IPPORT_TELNET		23
#endif
#ifndef IPPORT_SMTP
#define IPPORT_SMTP			25
#endif
#ifndef IPPORT_POP3
#define IPPORT_POP3			110
#endif
#ifndef IPPORT_IDENT
#define IPPORT_IDENT		113
#endif
#ifndef IPPORT_SUBMISSION
#define IPPORT_SUBMISSION	587
#endif


#endif	/* Don't add anything after this line */
