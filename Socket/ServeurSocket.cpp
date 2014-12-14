#include "ServeurSocket.h"

#include <iostream>
#include <sstream>

using namespace std;

ServeurSocket::ServeurSocket(bool start, int port, int maxcli, size_t buf_size, SOCKET in) : Socket(start, string(), port, maxcli, buf_size, in)
{
}

ServeurSocket::~ServeurSocket(){}

bool ServeurSocket::quit()
{
    clearClients();
    return Socket::quit();
}

void ServeurSocket::clearClients()
{
   for(map<string, ConnectedClient*>::iterator it = clients.begin(); it != clients.end(); it++)
   {
       string login = it->second->getLogin();
       delete it->second;
       clients.erase(login);
   }
}

void ServeurSocket::removeClient(std::string login)
{
    if(clients.find(login) != clients.end())
    {
        delete clients[login];
        clients.erase(login);
    }
}

void ServeurSocket::addClient(SOCKET csock, string login)
{
    ConnectedClient *c = new ConnectedClient(csock, login);
    clients[login] = c;
}

int ServeurSocket::acceptClient()
{
    /* new client */
    SOCKADDR_IN csin = { 0 };
    size_t sinsize = sizeof csin;
    SOCKET csock = accept(sock, (SOCKADDR *)&csin, (socklen_t *) &sinsize);
    if(csock == SOCKET_ERROR)
    {
        err = sock_error();
        fprintf(stderr, "accept() : %s\n", sock_err_message(err));
        return err;
    }

    /* after connecting the client sends its name */
    char buffer[buffer_size];
    for(int i = 0; i < (int) buffer_size; i++)
    buffer[i] = 0;

    if(readMessage(csock, buffer) == -1)
    {
        fprintf(stderr, "No name sent for client : %d", (int) csock);
        closesocket(csock);
        return -1;
    }

    if(max_clients <= 0 || max_clients > (int) clients.size())
    {
        addClient(csock, string(buffer));
    }
    else
    {
        fprintf(stderr, "Max client reached (max = %d)", (int) max_clients);
        closesocket(csock);
        return -1;
    }

    return 0;
}

void ServeurSocket::sendMessageToAll(const ConnectedClient *sender, const Message& message)
{
    int64_t taille = 0;

    unsigned char *buffer = NULL;
    mess.toSendable(&buffer);

    if(buffer == NULL)
    return;

    if(sender == NULL)
    {
        for(map<string, ConnectedClient*>::iterator it = clients.begin(); it != clients.end(); it++)
        {
            writeCharArray(it->second, buffer, mess.getTaille());
        }
    }
    else
    {
        for(map<string, ConnectedClient*>::iterator it = clients.begin(); it != clients.end(); it++)
        {
            if(sender->getSock() != it->second->getSock())
            {
                writeCharArray(it->second, buffer, mess.getTaille());
            }
        }
    }

    delete buffer;
}


bool ServeurSocket::sendMessage(ConnectedClient *client, const Message &message)
{
    return message.getTaille() == writeMessage(client->getSock(), mess);
}

bool ServeurSocket::receiveMessage(ConnectedClient *client, string &message)
{
    char *buf = new char[buffer_size];
    memset(buf, (int) '\0', buffer_size);

    if(readMessage(client->getSock(), buf) < 1)
    return false;

    message += buf;
    return true;
}

map<string, ConnectedClient*> &ServeurSocket::getClients()
{
    return clients;
}

ConnectedClient *ServeurSocket::getClient(string login)
{
    return clients[login];
}

void ServeurSocket::setClients(const map<string, ConnectedClient*> &cli)
{
    clients = cli ;
}

int ServeurSocket::getNfds()
{
    int n_max = max(sock, sock_in);
    for(map<string, ConnectedClient*>::iterator it = clients.begin(); it != clients.end(); it++)
    {
        int tmp = it->second->getSock();
        if(tmp > n_max)
        n_max = tmp;
    }

    return n_max + 1;
}

int ServeurSocket::refreshReadFileSet()
{
    int n = Socket::refreshReadFileSet();
    for(map<string, ConnectedClient*>::iterator it = clients.begin(); it != clients.end(); it++)
    {
        int tmp = it->second->getSock();
        FD_SET(tmp, &read_fileset);
        n++;
    }

    return n;
}

int ServeurSocket::readFDSet(vector<string> &cli, vector<string> &cli_in, string &in, struct timeval *t)
{
    getUpdatedReadFileSet(t);
    if(FD_ISSET(sock_in, &read_fileset))
    {
        // Keyboard input
        in = readSockIn();
    }

    if(FD_ISSET(sock, &read_fileset))
    {
        // Serveur socket input
        if(acceptClient() != 0)
        return -1;
    }


    for(map<string, ConnectedClient*>::iterator it = clients.begin(); it != clients.end(); it++)
    {
        ConnectedClient *c = it->second;
        if(FD_ISSET(c->getSock(), &read_fileset))
        {
            // Client socket input
            string buf;
            if(receiveMessage(c, buf))
            {
                cli.push_back(c->getLogin());
                cli_in.push_back(buf);
            }
            else
            {
                buf = "disconnected";
                cli.push_back(c->getLogin());
                cli_in.push_back(buf);
                removeClient(c->getLogin());
            }
        }
    }


    return 0;
}
