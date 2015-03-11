#include "ConnectedClient.h"

#include <iostream>

using namespace std;

ConnectedClient::ConnectedClient(SOCKET sock, string login)
{
    this->sock = sock;
    this->login = login;
}

ConnectedClient::~ConnectedClient()
{
    closesocket(sock);
}

SOCKET ConnectedClient::getSock() const
{
    return sock;
}

void ConnectedClient::setSock(SOCKET s)
{
    sock = s;
}

string ConnectedClient::getLogin() const
{
    return login;
}

void ConnectedClient::setLogin(string log)
{
    login = log;
}
