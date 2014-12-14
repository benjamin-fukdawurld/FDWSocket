#include "ClientSocket.h"

using namespace std;

ClientSocket::ClientSocket(string login, string address, bool start, int port, size_t buf_size, SOCKET in) : Socket(start, address, port, 0, buf_size, in)
{
    this->login = login;
}

ClientSocket::~ClientSocket(){}

bool ClientSocket::init()
{
    if(login.empty() || !Socket::init() || !sendMessage(login))
    {
        started = false;
        return false;
    }

    return true;
}

string ClientSocket::getLogin() const
{
    return login;
}

bool ClientSocket::setLogin(string log)
{
    if(started)
    return false;

    login = log;
    return true;
}

int ClientSocket::readMessage(char *buffer) const
{
   int n = 0;

   if((n = recv(sock, buffer, buffer_size - 1, 0)) < 0)
   {
       int err = sock_error();
       fprintf(stderr, "recv() : %s\n", sock_err_message(err));
       return -1;
   }

   buffer[n] = '\0';

   return n;
}

int ClientSocket::writeMessage(const char *buffer) const
{
   int n = 0;

    if((n = send(sock, buffer, strlen(buffer), 0)) < 0)
   {
       int err = sock_error();
       fprintf(stderr, "send() : %s\n", sock_err_message(err));
       return -1;
   }

    return n;
}

bool ClientSocket::sendMessage(const std::string &mess) const
{
    return (int) mess.size() == writeMessage(mess.c_str());
}

bool ClientSocket::receiveMessage(std::string &mess) const
{
    char *buf = new char[buffer_size];
    for(int i = 0, c = (int) buffer_size; i < c; i++)
    buf[i] = '\0';

    if(readMessage(buf) <= 0)
    return false;

    mess += buf;
    return true;
}


int ClientSocket::readFDSet(std::string &serv_in, std::string &in, timeval *t)
{
    getUpdatedReadFileSet(t);
    if(FD_ISSET(sock_in, &read_fileset))
    {
        // Keyboard input
        in = readSockIn();
    }

    if(FD_ISSET(sock, &read_fileset))
    {
        // Serveur input
        if(!receiveMessage(serv_in))
        {
            // Serveur disconnected
            serv_in = "Serveur disconnected";
            return -1;
        }
    }

    return 0;
}
