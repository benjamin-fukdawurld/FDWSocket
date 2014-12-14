#ifndef SERVEURSOCKET_H
#define SERVEURSOCKET_H

#include <vector>
#include <map>

#include "Socket.h"
#include "ConnectedClient.h"

class ServeurSocket : public Socket
{
    protected:
        std::map<std::string, ConnectedClient*> clients;

        //Overide
        virtual bool quit();

        virtual int getNfds();

    public:
        ServeurSocket(bool start = false, int port = DEFAULT_PORT, int maxcli = DEFAULT_MAX_CLIENTS, size_t buf_size = DEFAULT_BUF_SIZE, SOCKET in = STDIN_FILENO);

        virtual ~ServeurSocket();

        virtual std::map<std::string, ConnectedClient*> &getClients();
        virtual ConnectedClient *getClient(std::string login);
        virtual void setClients(const std::map<std::string, ConnectedClient *> &cli);

        virtual void clearClients();
        virtual void removeClient(std::string login);
        virtual void addClient(SOCKET csock, std::string login);
        virtual int acceptClient();
        virtual void sendMessageToAll(const ConnectedClient *sender, const Message &message);
        virtual bool sendMessage(ConnectedClient *cli, const Message &message);
        virtual bool receiveMessage(ConnectedClient *cli, Message &buffer);

        virtual int refreshReadFileSet();
        virtual int readFDSet(std::vector<std::string> &cli, std::vector<std::string> &cli_in, std::string &in, struct timeval *t = NULL);

};

#endif // SERVEURSOCKET_H
