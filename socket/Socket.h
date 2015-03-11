#ifndef ABSTRACTSOCKET_H
#define ABSTRACTSOCKET_H

#include "socket/SocketConfiguration.h"

#include <thread>
#include <mutex>
#include <string>
#include <queue>

#include "portable_socket.h"
#include "concurency/SharedObject.h"


class Socket
{
    protected:

        SOCKET m_sock;
        mutable std::mutex m_inMutex;
        mutable std::mutex m_outMutex;

        bool m_started;

        SocketConfiguration m_config;

        std::thread *m_inThread;
        std::thread *m_outThread;
        SharedObject<bool> m_inStop;
        SharedObject<bool> m_outStop;


        SharedObject<std::queue<void*> > m_inQueue;
        SharedObject<std::queue<void*> > m_outQueue;


        int m_err;



    public:

        virtual ~Socket();

        static bool isReadable(SOCKET sock);
        static bool isWritable(SOCKET sock);


        virtual bool start();
        virtual bool restart();
        virtual bool stop();
        virtual bool isStarted() const;

        virtual SOCKET getSock() const;
        virtual int getPort() const;
        virtual int getErr() const;

        virtual std::thread *getInThread();
        virtual std::thread *getOutThread();

        virtual bool getInStop() const;
        virtual bool getOutStop() const;

        virtual void setInStop(bool b);
        virtual void setOutStop(bool b);

        virtual const SharedObject<std::queue<void *> > &getInQueue() const;
        virtual const SharedObject<std::queue<void *> > &getOutQueue() const;

        virtual size_t getInQueueSize() const;
        virtual size_t getOutQueueSize() const;

        virtual void pushIn(void *toPush);
        virtual bool tryPushIn(void *toPush);
        virtual void nonBlockPushIn(void *toPush);
        virtual void pushOut(void *toPush);
        virtual bool tryPushOut(void *toPush);
        virtual void nonBlockPushOut(void *toPush);

        virtual void *frontIn();
        virtual void *tryFrontIn();
        virtual void *nonBlockFrontIn();
        virtual void *frontOut();
        virtual void *tryFrontOut();
        virtual void *nonBlockFrontOut();

        virtual void *popIn();
        virtual void *tryPopIn();
        virtual void *nonBlockPopIn();
        virtual void *popOut();
        virtual void *tryPopOut();
        virtual void *nonBlockPopOut();

        virtual size_t getBufferSize() const;



        //Serveur
        virtual int getMaxClients() const;

        //Client
        virtual std::string getAddress() const;

        virtual bool isReadyToRead();
        virtual bool isReadyToWrite();


        virtual bool sendMessage(const std::string &mess);
        virtual bool sendMessage(const std::string &mess, SOCKET& sock);

        virtual bool receiveMessage(std::string &mess);
        virtual bool receiveMessage(std::string &mess, SOCKET& sock);

        virtual void in_routine() = 0;
        virtual void out_routine() = 0;


    protected:

        Socket(bool start = false,
               std::string address = std::string(),
               int port = 0,
               int maxcli = 0,
               size_t buf_size = 0);


        virtual int64_t readAny(SOCKET sock, void *buffer, const int64_t &s);
        virtual int64_t writeAny(SOCKET sock, const void *buffer, const int64_t &s);


        virtual int64_t readCharArray(SOCKET sock, unsigned char *buffer, const int64_t &s);
        virtual int64_t writeCharArray(SOCKET sock, const unsigned char *buffer, const int64_t &s);

        virtual bool readSized(SOCKET sock, unsigned char **buffer, int64_t &taille);
        virtual bool writeSized(SOCKET sock, const unsigned char *buffer, const int64_t &taille);


        virtual void setErr(const int &e);

        virtual void setBufferSize(size_t s);

        virtual bool setPort(int p);


        //Serveur
        virtual bool setMaxClients(int m);

        //Client
        virtual bool setAddress(std::string addr);

        virtual bool init();
        virtual bool quit();
};

#endif // ABSTRACTSOCKET_H
