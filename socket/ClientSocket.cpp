#include "ClientSocket.h"

#include <chrono>

#include "conversion/byteConversion.h"



using namespace std;



ClientSocket::ClientSocket(string login,
                           string address,
                           bool start,
                           int port,
                           size_t buf_size) :
    Socket(start, address, port, 0, buf_size)
{
    this->m_login = login;
}





ClientSocket::~ClientSocket(){}





bool ClientSocket::init()
{
    if(m_login.empty() || !Socket::init())
    {
        m_started = false;
        return false;
    }

    string reponse;
    if(!sendMessage(m_login) || !receiveMessage(reponse) || reponse != "CONNECTED")
    {
        cerr << "ERROR : " << reponse;
        Socket::quit();
        return false;
    }

    return true;
}





string ClientSocket::getLogin() const
{
    return m_login;
}





bool ClientSocket::setLogin(string log)
{
    if(m_started)
    return false;

    m_login = log;
    return true;
}





void ClientSocket::in_routine()
{
    do
    {
        bool sleep(false);
        if(!m_inQueue.isLocked() && m_inQueue.lock())
        {
            if(isReadyToRead())
            {
                unsigned char* buffer = 0;
                int64_t taille(0);
                if(readSized(m_sock, &buffer, taille) && taille > 0)
                {
                    nonBlockPushIn(buffer);
                }
                else
                sleep = true;
            }
            else
            sleep = true;

            if(!m_inQueue.unlock())
            sleep = true;

        }
        else
        sleep = true;

        if(sleep)
        {
            chrono::milliseconds tmp(1000);
            this_thread::sleep_for(tmp);
        }

    }while(!m_inStop.getObject());
}





void ClientSocket::out_routine()
{
    do
    {
        bool sleep(false);
        if(m_outQueue.getObject().size() > 0 && !m_outQueue.isLocked() && m_outQueue.lock())
        {
            if(isReadyToWrite())
            {
                unsigned char* buffer = static_cast<unsigned char*>(nonBlockPopOut());
                if(buffer != 0)
                {
                    int64_t taille(0);
                    fromByteArray(&taille, buffer, sizeof(int64_t));
                    if(taille > 0 && writeSized(m_sock, buffer, taille))
                    {
                        delete[] buffer;
                    }
                    else
                    sleep = true;
                }
                else
                sleep = true;
            }
            else
            sleep = true;

            if(!m_outQueue.unlock())
            sleep = true;

        }
        else
        sleep = true;

        if(sleep)
        {
            chrono::milliseconds tmp(1000);
            this_thread::sleep_for(tmp);
        }

    }while(!m_outStop.getObject());
}

