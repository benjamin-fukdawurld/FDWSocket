#ifndef CLIENTSOCKET_H
#define CLIENTSOCKET_H

#include "Socket.h"

class ClientSocket : public Socket
{
    protected:
        std::string m_login;

    public:
        ClientSocket(std::string login,
                     std::string address = "127.0.0.1",
                     bool start = false,
                     int port = SocketConfiguration::DEFAULT_PORT,
                     size_t buf_size= SocketConfiguration::DEFAULT_BUF_SIZE);

        virtual ~ClientSocket();

        virtual std::string getLogin() const;
        virtual bool setLogin(std::string log);


        virtual void in_routine();
        virtual void out_routine();

    protected:
        virtual bool init();

};

#endif // CLIENTSOCKET_H
