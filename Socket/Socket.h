#ifndef ABSTRACTSOCKET_H
#define ABSTRACTSOCKET_H

#include <string>

#include "portable_socket.h"

class Socket
{
    public:
        virtual ~Socket();

        virtual bool start();
        virtual bool restart();
        virtual bool stop();
        virtual bool isStarted() const;

        virtual SOCKET getSock() const;
        virtual SOCKET getSockIn() const;
        virtual void setSockIn(SOCKET in);
        virtual int getPort() const;
        virtual int getErr() const;

        virtual size_t getBufferSize() const;
        virtual fd_set getReadFileSet();
        virtual void setReadFileSet(const fd_set &fdst);
        virtual int refreshReadFileSet();
        virtual fd_set getUpdatedReadFileSet(timeval *t = NULL);
        virtual std::string readSockIn();

        //Serveur
        virtual int getMaxClients() const;

        //Client
        virtual std::string getAddress() const;


    protected:
        SOCKET sock;
        SOCKET sock_in; //Soit STDIN_FILENO soit une socket non bloquante
        bool started;
        fd_set read_fileset;

        Socket(bool start = false, std::string address = NULL, int port = DEFAULT_PORT, int maxcli = DEFAULT_MAX_CLIENTS, size_t buf_size = DEFAULT_BUF_SIZE, SOCKET in = STDIN_FILENO);

        virtual bool readMessage(SOCKET sock, Message &mess);
        virtual bool writeMessage(SOCKET sock, const Message &mess);


        virtual int64_t readCharArray(SOCKET sock, unsigned char *buffer, const size_t &s);
        virtual int64_t writeCharArray(SOCKET sock, const unsigned char *buffer, const size_t &s);

        int err;
        virtual void setErr(const int &e);

        size_t buffer_size;
        virtual void setBufferSize(size_t s);

        int port;
        virtual bool setPort(int p);


        virtual int getNfds();

        //Serveur
        int max_clients;
        virtual bool setMaxClients(int m);

        //Client
        std::string address;
        virtual bool setAddress(std::string addr);

        virtual bool init();
        virtual bool quit();
};

#endif // ABSTRACTSOCKET_H
