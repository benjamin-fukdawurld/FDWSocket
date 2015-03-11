#ifndef SERVEURSOCKET_H
#define SERVEURSOCKET_H

#include <vector>
#include <map>

#include "Socket.h"
#include "ConnectedClient.h"

class ServeurSocket : public Socket
{
    protected:
        mutable std::mutex m_clientMutex;
        std::map<std::string, ConnectedClient*> m_clients;

        //Overide
        virtual bool quit();

    public:
        ServeurSocket(bool start = false,
                      int port = SocketConfiguration::DEFAULT_PORT,
                      int maxcli = SocketConfiguration::DEFAULT_MAX_CLIENTS,
                      size_t buf_size = SocketConfiguration::DEFAULT_BUF_SIZE);

        virtual ~ServeurSocket();

        virtual std::map<std::string, ConnectedClient*> &getClients();
        virtual ConnectedClient *getClient(std::string login);
        virtual void setClients(const std::map<std::string, ConnectedClient *> &cli);

        virtual void clearClients();
        virtual void removeClient(std::string login);
        virtual void addClient(SOCKET csock, std::string login);
        virtual int acceptClient();


        virtual void in_routine();
        virtual void out_routine();
        virtual void client_routine(ConnectedClient* current_client);

};

#endif // SERVEURSOCKET_H
