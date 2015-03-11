#include "Socket.h"

#include <iostream>
#include <sstream>

#include "conversion/byteConversion.h"

using namespace std;

Socket::Socket(bool start, string address, int port, int maxcli, size_t bufSize) :
    m_started(false),
    m_config(address, port, maxcli, bufSize),
    m_inThread(0),
    m_outThread(0),
    m_inStop(false),
    m_outStop(false),
    m_inQueue(),
    m_outQueue(),
    m_err(0)
{
    if(start)
    init();
}



Socket::~Socket()
{
    void *tmp;
    while((tmp = popIn()) != 0)
    {
        delete static_cast<unsigned char*>(tmp);
    }

    while((tmp = popOut()) != 0)
    {
        delete static_cast<unsigned char*>(tmp);
    }

    quit();
}




bool Socket::isReadable(SOCKET sock)
{
    fd_set fd;
    FD_ZERO(&fd);
    FD_SET(sock, &fd);
    timeval t;
    t.tv_sec = 1;
    t.tv_usec = 0;
    if(select(sock + 1, &fd, 0, 0, &t) != -1 && FD_ISSET(sock, &fd))
    return true;

    return false;
}





bool Socket::isWritable(SOCKET sock)
{
    fd_set fd;
    FD_ZERO(&fd);
    FD_SET(sock, &fd);
    timeval t;
    t.tv_sec = 1;
    t.tv_usec = 0;
    if(select(sock + 1, 0, &fd, 0, &t) != -1 && FD_ISSET(sock, &fd))
    return true;

    return false;
}



bool Socket::init()
{
    if(m_started)
    return false;

    m_sock = sock_init_connection((m_config.address.empty() ? NULL : m_config.address.c_str()), m_config.port, m_config.maxClients);
    if(m_sock > 0)
    m_started = true;
    else
    {
        m_started = false;
        m_err = m_sock;
        m_sock = 0;
    }

    return m_started;
}



bool Socket::quit()
{
    if(!m_started)
    return false;

    int tmp = sock_end_connection(m_sock);
    if(tmp != 0)
    {
        m_err = tmp;
        return false;
    }

    m_started = false;
    return true;
}



bool Socket::start()
{
    return init();
}



bool Socket::restart()
{
    return stop() && start();
}



bool Socket::stop()
{
    return quit();
}



bool Socket::isStarted() const
{
    return m_started;
}



SOCKET Socket::getSock() const
{
    return m_sock;
}




int Socket::getPort() const
{
    return m_config.port;
}



int Socket::getErr() const
{
    return m_err;
}





thread *Socket::getInThread()
{
    return m_inThread;
}




thread *Socket::getOutThread()
{
    return m_outThread;
}





bool Socket::getInStop() const
{
    return m_inStop.getObject();
}




bool Socket::getOutStop() const
{
    return m_outStop.getObject();
}





void Socket::setInStop(bool b)
{
    m_inStop.object() = b;
}





void Socket::setOutStop(bool b)
{
    m_outStop.object() = b;
}





const SharedObject<std::queue<void *> > &Socket::getInQueue() const
{
    return m_inQueue;
}



const SharedObject<std::queue<void *> > &Socket::getOutQueue() const
{
    return m_outQueue;
}



size_t Socket::getInQueueSize() const
{
    return m_inQueue.getObject().size();
}



size_t Socket::getOutQueueSize() const
{
    return m_outQueue.getObject().size();
}



void Socket::pushIn(void* toPush)
{
    m_inQueue.lock();
    m_inQueue.object().push(toPush);
    m_inQueue.unlock();
}





bool Socket::tryPushIn(void* toPush)
{
    if(m_inQueue.isLocked() || !m_inQueue.trylock())
    return false;

    m_inQueue.object().push(toPush);
    m_inQueue.unlock();

    return true;
}





void Socket::nonBlockPushIn(void *toPush)
{
    m_inQueue.object().push(toPush);
}



void Socket::pushOut(void* toPush)
{
    m_outQueue.lock();
    m_outQueue.object().push(toPush);
    m_outQueue.unlock();
}





bool Socket::tryPushOut(void* toPush)
{
    if(m_outQueue.isLocked() || !m_outQueue.trylock())
    return false;

    m_outQueue.object().push(toPush);
    m_outQueue.unlock();

    return true;
}





void Socket::nonBlockPushOut(void *toPush)
{
    m_outQueue.object().push(toPush);
}



void *Socket::frontIn()
{
    void *poped(0);
    m_inQueue.lock();
    poped = m_inQueue.object().front();
    m_inQueue.unlock();

    return poped;
}




void *Socket::tryFrontIn()
{
    if(m_inQueue.isLocked() || !m_inQueue.trylock())
    return 0;

    void *poped(0);
    poped = m_inQueue.object().front();
    m_inQueue.unlock();

    return poped;
}





void *Socket::nonBlockFrontIn()
{
    void *poped(0);
    poped = m_inQueue.object().front();

    return poped;
}



void *Socket::frontOut()
{
    void *poped(0);
    m_outQueue.lock();
    poped = m_outQueue.object().front();
    m_outQueue.unlock();

    return poped;
}





void *Socket::tryFrontOut()
{
    if(m_outQueue.isLocked() || !m_outQueue.trylock())
    return 0;

    void *poped(0);
    poped = m_outQueue.object().front();
    m_outQueue.unlock();

    return poped;
}





void *Socket::nonBlockFrontOut()
{
    void *poped(0);
    poped = m_outQueue.object().front();

    return poped;
}



void *Socket::popIn()
{
    void *poped(0);
    m_inQueue.lock();
    poped = m_inQueue.object().front();
    m_inQueue.object().pop();
    m_inQueue.unlock();

    return poped;
}




void *Socket::tryPopIn()
{
    if(m_inQueue.isLocked() || !m_inQueue.trylock())
    return 0;

    void *poped(0);
    poped = m_inQueue.object().front();
    m_inQueue.object().pop();
    m_inQueue.unlock();

    return poped;
}





void *Socket::nonBlockPopIn()
{
    void *poped(0);
    poped = m_inQueue.object().front();
    m_inQueue.object().pop();

    return poped;
}



void *Socket::popOut()
{
    void *poped(0);
    m_outQueue.lock();
    poped = m_outQueue.object().front();
    m_outQueue.object().pop();
    m_outQueue.unlock();

    return poped;
}





void *Socket::tryPopOut()
{
    if(m_outQueue.isLocked() || !m_outQueue.trylock())
    return 0;

    void *poped(0);
    poped = m_outQueue.object().front();
    m_outQueue.object().pop();
    m_outQueue.unlock();

    return poped;
}





void *Socket::nonBlockPopOut()
{
    void *poped(0);
    poped = m_outQueue.object().front();
    m_outQueue.object().pop();

    return poped;
}




size_t Socket::getBufferSize() const
{
    return m_config.bufferSize;
}



int Socket::getMaxClients() const
{
    return m_config.maxClients;
}



string Socket::getAddress() const
{
    return m_config.address;
}



bool Socket::isReadyToRead()
{
    return isReadable(m_sock);
}





bool Socket::isReadyToWrite()
{
    return isWritable(m_sock);
}





bool Socket::sendMessage(const string &mess)
{
    return sendMessage(mess, m_sock);
}





bool Socket::sendMessage(const string &mess, SOCKET& sock)
{
    return writeSized(sock, reinterpret_cast<const unsigned char*>(mess.c_str()), mess.size() + 1);
}





bool Socket::receiveMessage(string &mess)
{
    return receiveMessage(mess, m_sock);
}





bool Socket::receiveMessage(string &mess, SOCKET& sock)
{
    unsigned char *buf;
    int64_t taille;

    if(!readSized(sock, &buf, taille))
    return false;

    mess = reinterpret_cast<char*>(buf + sizeof(int64_t));
    delete[] buf;

    return true;
}



void Socket::setErr(const int &e)
{
    m_err = e;
}



bool Socket::setPort(int p)
{
    if(m_started || p <= 0)
    return false;

    m_config.port = p;

    return true;
}



bool Socket::setMaxClients(int m)
{
    if(m_started || m <= 0)
    return false;

    m_config.maxClients = m;

    return true;
}



bool Socket::setAddress(string addr)
{
    if(m_started)
    return false;

    m_config.address = addr;

    return true;
}



void Socket::setBufferSize(size_t s)
{
    m_config.bufferSize = s;
}



int64_t Socket::readAny(SOCKET sock, void *buffer, const int64_t &s)
{
    if(!m_started)
    {
        fprintf(stderr, "Socket must be started before send or receive anything\n");
        return -1;
    }

    if(s < 1)
    {
        fprintf(stderr, "Size is invalid\n");
        return -1;
    }

    if(buffer == 0)
    {
        fprintf(stderr, "Buffer is invalid\n");
        return -1;
    }

    int64_t n = 0;

    m_inMutex.lock();
    if((n = recv(sock, buffer, s, 0)) < 0)
    {
       int err = sock_error();
       fprintf(stderr, "recv() : %s\n", sock_err_message(err));
       return -1;
    }
    m_inMutex.unlock();

    return n;
}



int64_t Socket::writeAny(SOCKET sock, const void *buffer, const int64_t &s)
{
    if(!m_started)
    {
        fprintf(stderr, "Socket must be started before send or receive anything\n");
        return -1;
    }

    if(s < 1)
    {
        fprintf(stderr, "Size is invalid\n");
        return -1;
    }

    if(buffer == 0)
    {
        fprintf(stderr, "Buffer is invalid\n");
        return -1;
    }

    int64_t n = 0;

    m_outMutex.lock();
    if((n = send(sock, buffer, s, 0)) < 0)
    {
        setErr(sock_error());
        fprintf(stderr, "send() : %s\n", sock_err_message(m_err));
        return -1;
    }
    m_outMutex.unlock();

    return n;
}



int64_t Socket::readCharArray(SOCKET sock, unsigned char *buffer, const int64_t &s)
{
    return readAny(sock, static_cast<void*>(buffer), s);
}



int64_t Socket::writeCharArray(SOCKET sock, const unsigned char *buffer, const int64_t &s)
{
    return writeAny(sock, static_cast<const void *>(buffer), s);
}



bool Socket::readSized(SOCKET sock, unsigned char **buffer, int64_t &taille)
{
    *buffer = 0;
    int64_t lu = 0;
    unsigned char *tmpBuffer = 0;

    //Lecture de la taille
    {
        int64_t tmp = sizeof(int64_t);
        tmpBuffer = new unsigned char[tmp];

        lu = readCharArray(sock, tmpBuffer, tmp);
        if(lu < tmp)
        {
            delete[] tmpBuffer;
            return false;
        }

        fromByteArray(&taille, tmpBuffer, sizeof(int64_t));
        if(taille < 1)
        {
            delete[] tmpBuffer;
            return false;
        }

        delete[] tmpBuffer;
        tmpBuffer = 0;
    }

    //Lecture du message
    {
        int64_t tmp = 0;
        int64_t rcvTaille = taille + sizeof(int64_t);
        tmpBuffer = new unsigned char[rcvTaille];
        toByteArray(tmpBuffer, &taille, sizeof(int64_t));

        while(lu < rcvTaille && (tmp = readCharArray(sock, tmpBuffer + lu, rcvTaille - lu)) > 0)
        {
            lu += tmp;
        }


        if(tmp < 0)
        {
            delete[] tmpBuffer;
            return false;
        }


        if(lu != rcvTaille)
        {
            delete[] tmpBuffer;
            return false;
        }

        *buffer = tmpBuffer;
    }

    return true;
}



bool Socket::writeSized(SOCKET sock, const unsigned char *buffer, const int64_t &taille)
{
    int64_t retTaille = 0;

    int64_t sendTaille = taille + sizeof(int64_t);


    unsigned char* toSend = new unsigned char[sendTaille];
    toByteArray(toSend, &taille, sizeof(int64_t));
    toByteArray(toSend + sizeof(int64_t), buffer, taille);

    retTaille = writeCharArray(sock, toSend, sendTaille);

    delete[] toSend;


    if(retTaille < 1)
    return false;

    return retTaille == sendTaille;
}


