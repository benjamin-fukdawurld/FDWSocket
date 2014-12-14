#include "Socket.h"

#include <iostream>
#include <sstream>

using namespace std;

Socket::Socket(bool start, string address, int port, int maxcli, size_t buf_size, SOCKET in)
{
    this->address = address;
    this->port = (port > 0 ? port : DEFAULT_PORT);
    this->max_clients = (maxcli > 0 ? maxcli : DEFAULT_MAX_CLIENTS);
    this->buffer_size = (buf_size > 0 ? buf_size : DEFAULT_BUF_SIZE);
    sock_in = in;
    started = false;
    err = 0;
    if(start)
    init();
}

Socket::~Socket()
{
    quit();
}

bool Socket::init()
{
    if(started)
    return false;

    sock = sock_init_connection((address.empty() ? NULL : address.c_str()), port, max_clients);
    if(sock > 0)
    started = true;
    else
    {
        started = false;
        err = sock;
        sock = 0;
    }

    return started;
}

bool Socket::quit()
{
    if(!started)
    return false;

    int tmp = sock_end_connection(sock);
    if(tmp != 0)
    {
        err = tmp;
        return false;
    }

    started = false;
    return true;
}

bool Socket::start()
{
    return init();
}

bool Socket::restart()
{
    return stop() && start();
}

bool Socket::stop()
{
    return quit();
}

bool Socket::isStarted() const
{
    return started;
}

fd_set Socket::getReadFileSet()
{
    return read_fileset;
}

SOCKET Socket::getSock() const
{
    return sock;
}

SOCKET Socket::getSockIn() const
{
    return sock_in;
}

void Socket::setSockIn(SOCKET in)
{
    if(in < 0)
    sock_in = -1;
    else
    sock_in = in;
}

int Socket::getPort() const
{
    return port;
}

int Socket::getErr() const
{
    return err;
}

size_t Socket::getBufferSize() const
{
    return buffer_size;
}

int Socket::getMaxClients() const
{
    return max_clients;
}

string Socket::getAddress() const
{
    return address;
}

void Socket::setReadFileSet(const fd_set &fdst)
{
    read_fileset = fdst;
}

void Socket::setErr(const int &e)
{
    err = e;
}

bool Socket::setPort(int p)
{
    if(started || p <= 0)
    return false;

    port = p;

    return true;
}

bool Socket::setMaxClients(int m)
{
    if(started || m <= 0)
    return false;

    max_clients = m;

    return true;
}

bool Socket::setAddress(string addr)
{
    if(started)
    return false;

    address = addr;

    return true;
}

void Socket::setBufferSize(size_t s)
{
    buffer_size = s;
}

int Socket::getNfds()
{
    return max(sock, sock_in) + 1;
}

int Socket::refreshReadFileSet()
{
    FD_ZERO(&read_fileset);
    FD_SET(sock_in, &read_fileset);
    FD_SET(sock, &read_fileset);

    return 2;
}

fd_set Socket::getUpdatedReadFileSet(timeval *t)
{
    refreshReadFileSet();
    if(select(getNfds(), &read_fileset, NULL, NULL, t) == -1)
    {
        perror("select() :");
        FD_ZERO(&read_fileset);
        exit(EXIT_FAILURE);
    }

    return read_fileset;
}

int64_t Socket::readCharArray(SOCKET sock, unsigned char *buffer, const size_t &s)
{
    if(!started)
    {
        fprintf(stderr, "Socket must be started before send or receive anything\n");
        return -1;
    }

    int n = 0;

    if((n = recv(sock, buffer, s, 0)) < 0)
    {
       int err = sock_error();
       fprintf(stderr, "recv() : %s\n", sock_err_message(err));
       return -1;
    }

    return n;
}

int64_t Socket::writeCharArray(SOCKET sock, const unsigned char *buffer, const size_t &s)
{
    if(!started)
    {
        fprintf(stderr, "Socket must be started before send or receive anything\n");
        return -1;
    }

    int n = 0;

    if((n = send(sock, buffer, s, 0)) < 0)
    {
        setErr(sock_error());
        fprintf(stderr, "send() : %s\n", sock_err_message(err));
        return -1;
    }

    return n;
}

bool Socket::readMessage(SOCKET sock, Message &mess)
{
    int64_t taille = 0, lu = 0;

    unsigned char *buffer = NULL;

    //Lecture de la taille
    {
        size_t s = sizeof(int64_t);
        buffer = new unsigned char[s];

        lu += readCharArray(sock, buffer, s);
        if(lu < (int64_t) s)
        {
            delete buffer;
            return false;
        }

        taille = Message::Bin_Array_2_int64(buffer);

        delete buffer;
        buffer = NULL;
    }

    //Lecture du message
    {
        int64_t tmp = 0;
        buffer = new unsigned char[taille];
        Message::int64_2_Bin_Array((int64_t *) &taille, buffer);

        while(lu < taille && (tmp = readCharArray(sock, buffer + lu, taille - lu)) > 0)
        {
            lu += tmp;
        }

        if(lu != taille)
        {
            delete buffer;
            return false;
        }

        mess = Message::fromSendable(buffer);

        delete buffer;
        buffer = NULL;
    }


    return true;
}

bool Socket::writeMessage(SOCKET sock, const Message &mess)
{
    int64_t taille = 0;

    unsigned char *buffer = NULL;
    mess.toSendable(&buffer);

    if(buffer == NULL)
    return false;

    taille = writeCharArray(sock, buffer, mess.getTaille());

    delete buffer;
    buffer = NULL;

    return taille == mess.getTaille();
}


string Socket::readSockIn()
{
    string str;
    if(sock_in >= 0)
    {
        if(sock_in == STDIN_FILENO)
        {
            char *buffer = NULL, *c = NULL;
            buffer = new char[buffer_size];

            do
            {
                fgets(buffer, buffer_size - 1, stdin);
                c = strstr(buffer, "\n");
                if(c != NULL)
                *c = '\0';

                str += buffer;
            }while(c == NULL);
        }
        else
        {
            bool stop = false;
            char *buffer = new char[buffer_size];

            while(!stop)
            {
                memset(buffer, (int) '\0', buffer_size);
                int n = read(sock_in, buffer, buffer_size - 1);

                //Fin du message
                if(n == 0 || (n == -1 && (errno == EWOULDBLOCK || errno == EAGAIN)))
                stop = true;
                //Erreur
                else if(n == -1)
                {
                    fprintf(stderr, "read() : %s\n", strerror(err));
                    if(errno == EBADF || errno == EISDIR)
                    {
                        sock_in = -1;
                        cerr << "input file descriptor has been unseted because it was not an apropriate descriptor" << endl;
                    }

                    delete buffer;
                    return str;
                }
                else
                str += buffer;
            }

            delete buffer;
        }
    }


    return str;
}
