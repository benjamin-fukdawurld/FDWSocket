#include "socket/SocketConfiguration.h"


const int SocketConfiguration::DEFAULT_PORT(1977);
const int SocketConfiguration::DEFAULT_MAX_CLIENTS(100);
const int SocketConfiguration::DEFAULT_BUF_SIZE(1024);
const std::string SocketConfiguration::DEFAULT_ADDRESS("127.0.0.1");


SocketConfiguration::SocketConfiguration(std::string address,
                                         int port,
                                         int maxcli,
                                         size_t bufSize) :
    bufferSize(bufSize > 0 ? bufSize : DEFAULT_BUF_SIZE),
    port(port > 0 ? port : DEFAULT_PORT),
    maxClients(maxcli > 0 ? maxcli : DEFAULT_MAX_CLIENTS),
    address(address.empty() ? DEFAULT_ADDRESS : address)
{
}

