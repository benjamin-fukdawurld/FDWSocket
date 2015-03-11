#include "ConnectedClient.h"



using namespace std;

ConnectedClient::ConnectedClient(SOCKET sock, string login) :
    m_sock(sock),
    m_login(login),
    m_thread(0),
    m_stop(false)
{
}




ConnectedClient::~ConnectedClient()
{
    closesocket(m_sock);
    m_stop.object() = false;

    if(m_thread != 0)
    {
        if(m_thread->joinable())
        m_thread->join();

        delete m_thread;
    }
}




SOCKET ConnectedClient::getSock() const
{
    return m_sock;
}





void ConnectedClient::setSock(SOCKET s)
{
    m_sock = s;
}





string ConnectedClient::getLogin() const
{
    return m_login;
}





void ConnectedClient::setLogin(string log)
{
    m_login = log;
}





thread *ConnectedClient::getThread()
{
    return m_thread;
}





bool ConnectedClient::getStop()
{
    return m_stop.getObject();
}





void ConnectedClient::setStop(bool b)
{
    m_stop.object() = b;
}




const SharedObject<std::queue<void *> >& ConnectedClient::getOutQueue() const
{
    return m_outQueue;
}





int ConnectedClient::outQueueSize() const
{
    return m_outQueue.getObject().size();
}





bool ConnectedClient::isOutQueueLocked() const
{
    return m_outQueue.isLocked();
}





bool ConnectedClient::lockOutQueue() const
{
    return m_outQueue.lock();
}





bool ConnectedClient::trylockOutQueue() const
{
    return m_outQueue.trylock();
}





bool ConnectedClient::unlockOutQueue() const
{
    return m_outQueue.unlock();
}




void ConnectedClient::pushOut(void* toPush)
{
    m_outQueue.lock();
    m_outQueue.object().push(toPush);
    m_outQueue.unlock();
}





bool ConnectedClient::tryPushOut(void* toPush)
{
    if(m_outQueue.isLocked() || !m_outQueue.trylock())
    return false;

    m_outQueue.object().push(toPush);
    m_outQueue.unlock();

    return true;
}





void ConnectedClient::nonBlockPushOut(void *toPush)
{
    m_outQueue.object().push(toPush);
}





void *ConnectedClient::frontOut()
{
    void *poped(0);
    m_outQueue.lock();
    poped = m_outQueue.object().front();
    m_outQueue.unlock();

    return poped;
}





void *ConnectedClient::tryFrontOut()
{
    if(m_outQueue.isLocked() || !m_outQueue.trylock())
    return 0;

    void *poped(0);
    poped = m_outQueue.object().front();
    m_outQueue.unlock();

    return poped;
}





void *ConnectedClient::nonBlockFrontOut()
{
    void *poped(0);
    poped = m_outQueue.object().front();

    return poped;
}





void *ConnectedClient::popOut()
{
    void *poped(0);
    m_outQueue.lock();
    poped = m_outQueue.object().front();
    m_outQueue.object().pop();
    m_outQueue.unlock();

    return poped;
}





void *ConnectedClient::tryPopOut()
{
    if(m_outQueue.isLocked() || !m_outQueue.trylock())
    return 0;

    void *poped(0);
    poped = m_outQueue.object().front();
    m_outQueue.object().pop();
    m_outQueue.unlock();

    return poped;
}





void *ConnectedClient::nonBlockPopOut()
{
    void *poped(0);
    poped = m_outQueue.object().front();
    m_outQueue.object().pop();

    return poped;
}



