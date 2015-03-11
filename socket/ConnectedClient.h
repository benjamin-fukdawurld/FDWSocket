#ifndef CONNECTEDCLIENT_H
#define CONNECTEDCLIENT_H

#include <string>
#include <thread>
#include <queue>

#include "portable_socket.h"
#include "concurency/SharedObject.h"

class ConnectedClient
{
    public:
        ConnectedClient(SOCKET m_sock, std::string m_login);
        virtual ~ConnectedClient();

        virtual SOCKET getSock() const;
        virtual void setSock(SOCKET s);

        virtual std::string getLogin() const;
        virtual void setLogin(std::string log);

        virtual std::thread *getThread();

        virtual bool getStop();
        virtual void setStop(bool b);

        virtual const SharedObject<std::queue<void *> > &getOutQueue() const;
        virtual int outQueueSize() const;
        virtual bool isOutQueueLocked() const;
        virtual bool lockOutQueue() const;
        virtual bool trylockOutQueue() const;
        virtual bool unlockOutQueue() const;



        virtual void pushOut(void *toPush);
        virtual bool tryPushOut(void *toPush);
        virtual void nonBlockPushOut(void *toPush);

        virtual void *frontOut();
        virtual void *tryFrontOut();
        virtual void *nonBlockFrontOut();

        virtual void *popOut();
        virtual void *tryPopOut();
        virtual void *nonBlockPopOut();


    protected:
        SOCKET m_sock;
        std::string m_login;
        std::thread *m_thread;
        SharedObject<bool> m_stop;
        SharedObject<std::queue<void*> > m_outQueue;
};

#endif // CONNECTEDCLIENT_H
