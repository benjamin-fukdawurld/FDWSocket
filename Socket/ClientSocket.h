#ifndef CLIENTSOCKET_H
#define CLIENTSOCKET_H

#include "Socket.h"

class ClientSocket : public Socket
{
    protected:
        std::string login;

        virtual int readMessage(char *buffer) const;
        virtual int writeMessage(const char *buffer) const;

        virtual bool init();

    public:
        ClientSocket(std::string login, std::string address = "127.0.0.1", bool start = false, int port = DEFAULT_PORT, size_t buf_size= DEFAULT_BUF_SIZE, SOCKET in = STDIN_FILENO);

        virtual ~ClientSocket();

        virtual std::string getLogin() const;
        virtual bool setLogin(std::string log);
        virtual bool sendMessage(const std::string &mess) const;
        virtual bool receiveMessage(std::string &mess) const;
        virtual int readFDSet(std::string &serv_in, std::string &in, struct timeval *t = NULL);

};

#endif // CLIENTSOCKET_H
