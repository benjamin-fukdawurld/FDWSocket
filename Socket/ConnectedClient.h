#ifndef CONNECTEDCLIENT_H
#define CONNECTEDCLIENT_H

#include <string>

#include "portable_socket.h"

class ConnectedClient
{
    public:
        ConnectedClient(SOCKET sock, std::string login);
        virtual ~ConnectedClient();
        virtual SOCKET getSock() const;
        virtual void setSock(SOCKET s);
        virtual std::string getLogin() const;
        virtual void setLogin(std::string log);

    protected:
        SOCKET sock;
        std::string login;
};

#endif // CONNECTEDCLIENT_H
