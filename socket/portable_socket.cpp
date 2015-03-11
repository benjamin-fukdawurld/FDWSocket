#include "portable_socket.h"

int sock_init()
{
    #ifdef WIN32
        WSADATA wsa;
        int err = WSAStartup(MAKEWORD(2, 2), &wsa);
        if(err < 0)
        {
            fputs(stderr, "WSAStartup failed : %s", sock_err_message(sock_error()));
            return EXIT_FAILURE;
        }
    #endif

    return 0;
}

int sock_quit()
{
    #ifdef WIN32
        return WSACleanup();
    #endif

    return 0;
}

int sock_error()
{
    #ifdef WIN32 /* si vous êtes sous Windows */
        return WSAGetLastError();
    #elif defined (linux) /* si vous êtes sous Linux */
        return errno;
    #endif
        return 0;
}

char* sock_err_message(int err)
{
    #ifdef WIN32 /* si vous êtes sous Windows */
        return FormatMessage(err);
    #elif defined (linux) /* si vous êtes sous Linux */
        return strerror(err);
    #endif
        return 0;
}

bool sock_getAddr(SOCKADDR_IN &sin, const char *address)
{
    if(address != NULL)
    {
        struct hostent *hostinfo = gethostbyname(address);
        if (hostinfo == NULL)
        {
            fprintf (stderr, "Unknown host %s.\n", address);
            return false;
        }

        sin.sin_addr = *(IN_ADDR *) hostinfo->h_addr;
    }
    else
    sin.sin_addr.s_addr = htonl(INADDR_ANY);

    return true;
}

SOCKET sock_init_connection(const char *address, int port, int maxcli)
{
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    SOCKADDR_IN sin;
    memset(&sin, 0, sizeof(SOCKADDR_IN));

    if(sock == INVALID_SOCKET)
    {
        int err = sock_error();
        fprintf (stderr, "socket() : %s\n", sock_err_message(err));
        return err;
    }

    if(!sock_getAddr(sin, address))
    {
        fprintf (stderr, "connection failed\n");
        return EXIT_FAILURE;
    }

    sin.sin_port = htons(port);
    sin.sin_family = AF_INET;

    if(address != NULL && strlen(address) > 0)
    {
        if(connect(sock,(SOCKADDR *) &sin, sizeof(SOCKADDR)) == SOCKET_ERROR)
        {
            int err = sock_error();
            fprintf (stderr, "connect() : %s\n", sock_err_message(err));
            return err;
        }
    }
    else
    {
        if(bind(sock,(SOCKADDR *) &sin, sizeof sin) == SOCKET_ERROR)
        {
            int err = sock_error();
            fprintf (stderr, "socket() : %s\n", sock_err_message(err));
            return err;
        }

        if(listen(sock, maxcli) == SOCKET_ERROR)
        {
            int err = sock_error();
            fprintf (stderr, "listen() : %s\n", sock_err_message(err));
            return err;
        }
    }

   return sock;
}

int sock_end_connection(int sock)
{
    if(closesocket(sock) == -1)
    {
        int err = sock_error();
        fprintf(stderr, "closesocket() : %s\n", sock_err_message(err));
        return err;
    }

    return  0;
}

