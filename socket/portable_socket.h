#ifndef PORTABLE_SOCKET_H
#define PORTABLE_SOCKET_H

#ifdef WIN32 /* si vous êtes sous Windows */

#include <winsock2.h>

#elif defined (unix) || defined (__unix__) || defined(__unix) || \
    (defined(__APPLE__) && defined(__MACH__)) /* si vous êtes sous Linux */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h> /* close */
#include <netdb.h> /* gethostbyname */
#include <string.h>
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define closesocket(s) close(s)
typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
typedef struct in_addr IN_ADDR;

#else /* sinon vous êtes sur une plateforme non supportée */

#error not defined for this platform

#endif // System defined

#include <errno.h>

#include <cstdlib>
#include <cstdio>



int sock_init(void);
int sock_quit(void);
int sock_error(void);
char* sock_err_message(int err);
bool sock_getAddr(SOCKADDR_IN &sin, const char *address = NULL);
SOCKET sock_init_connection(const char *address, int port, int maxcli);
int sock_end_connection(int sock);

#endif // PORTABLE_SOCKET_H
