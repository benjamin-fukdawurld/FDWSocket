#ifndef SOCKETCONFIGURATION_H
#define SOCKETCONFIGURATION_H


#include <cstdlib>
#include <string>



struct SocketConfiguration
{
    static const int DEFAULT_PORT;
    static const int DEFAULT_MAX_CLIENTS;
    static const int DEFAULT_BUF_SIZE;
    static const std::string DEFAULT_ADDRESS;

    size_t bufferSize;
    int port;
    int maxClients;
    std::string address;

    SocketConfiguration(std::string address = DEFAULT_ADDRESS,
                        int port = DEFAULT_PORT,
                        int maxcli = DEFAULT_MAX_CLIENTS,
                        size_t bufSize = DEFAULT_BUF_SIZE);
};




#endif // SOCKETCONFIGURATION_H
