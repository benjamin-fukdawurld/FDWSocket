#include "ServeurSocket.h"

#include <iostream>
#include <sstream>


#include "conversion/byteConversion.h"


using namespace std;

ServeurSocket::ServeurSocket(bool start,
                             int port,
                             int maxcli,
                             size_t buf_size) :
    Socket(start, string(), port, maxcli, buf_size)
{
}


ServeurSocket::~ServeurSocket(){}





map<string, ConnectedClient*> &ServeurSocket::getClients()
{
    return m_clients;
}






ConnectedClient *ServeurSocket::getClient(string login)
{
    return m_clients[login];
}





void ServeurSocket::setClients(const map<string, ConnectedClient*> &cli)
{
    m_clients = cli ;
}




bool ServeurSocket::quit()
{
    clearClients();
    return Socket::quit();
}





void ServeurSocket::clearClients()
{
   for(map<string, ConnectedClient*>::iterator it = m_clients.begin(); it != m_clients.end(); it++)
   {
       string login = it->second->getLogin();
       delete it->second;
       m_clients.erase(login);
   }
}





void ServeurSocket::removeClient(std::string login)
{
    if(m_clients.find(login) != m_clients.end())
    {
        delete m_clients[login];
        m_clients.erase(login);
    }
}





void ServeurSocket::addClient(SOCKET csock, string login)
{
    ConnectedClient *c = new ConnectedClient(csock, login);
    m_clients[login] = c;
}





int ServeurSocket::acceptClient()
{
    /* new client */
    SOCKADDR_IN csin;
    memset(&csin, 0, sizeof(SOCKADDR_IN));
    size_t sinsize = sizeof csin;
    SOCKET csock = accept(m_sock, (SOCKADDR *)&csin, (socklen_t *) &sinsize);
    if(csock == SOCKET_ERROR)
    {
        m_err = sock_error();
        cerr << "accept() : " << sock_err_message(m_err) << endl;
        return m_err;
    }



    /* after connecting the client sends its name */
    string login;
    if(!receiveMessage(login))
    {
        cerr << "No name sent for client : " << (int) csock << endl;
        closesocket(csock);
        return -1;
    }

    if(m_config.maxClients <= 0 || m_config.maxClients > (int) m_clients.size())
    {
        if(m_clients.find(login) == m_clients.end())
        addClient(csock, login);
        else
        {
            string errMessage(login);
            errMessage += " is already token by another user.";
            writeSized(csock, reinterpret_cast<const unsigned char *>(errMessage.c_str()), errMessage.size() + 1);
            closesocket(csock);
            return -1;
        }
    }
    else
    {
        cerr << "Max client reached (max = " << (int) m_config.maxClients << ")" << endl;
        closesocket(csock);
        return -1;
    }

    return 0;
}





void ServeurSocket::in_routine()
{
    do
    {
        bool sleep(false);
        if(isReadyToRead())
        {
            acceptClient();
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





void ServeurSocket::out_routine()
{
    do
    {
        bool sleep(false);
        if(m_outQueue.getObject().size() > 0 && !m_outQueue.isLocked() && m_outQueue.lock())
        {
            pair<ConnectedClient*, unsigned char*>* mess(static_cast<pair<ConnectedClient*, unsigned char*>*>(nonBlockPopOut()));
            if(mess != 0)
            {
                int64_t taille(0);
                fromByteArray(&taille, mess->second, sizeof(int64_t));
                if(taille > 0)
                {
                    if(mess->first == 0)
                    {
                        //BROADCAST
                        map<string, ConnectedClient*>::iterator it(m_clients.begin());
                        while(it != m_clients.end())
                        {
                            unsigned char* tmp = new unsigned char[taille];
                            memcpy(tmp, mess->second, taille);
                            it->second->pushOut(tmp);
                            it++;
                        }

                        delete[] mess->second;
                    }
                    else
                    {
                        mess->first->pushOut(mess->second);
                        sleep = true;
                    }
                }
                else
                sleep = true;
            }
            else
            sleep = true;

            delete mess;

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





void ServeurSocket::client_routine(ConnectedClient* current_client)
{
    do
    {
        bool sleep(false);

        if(!m_inQueue.isLocked() && m_inQueue.lock())
        {
            if(Socket::isReadable(current_client->getSock()))
            {
                unsigned char* buffer = 0;
                int64_t taille(0);
                if(readSized(current_client->getSock(), &buffer, taille) && taille > 0)
                {
                    pair<ConnectedClient*, unsigned char*>* mess(new pair<ConnectedClient*, unsigned char*>());
                    mess->first = current_client;
                    mess->second = buffer;
                    nonBlockPushIn(mess);
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


        if(current_client->outQueueSize() > 0 && !current_client->isOutQueueLocked() && current_client->lockOutQueue())
        {
            if(Socket::isWritable(current_client->getSock()))
            {
                unsigned char* mess(static_cast<unsigned char*>(current_client->nonBlockPopOut()));
                if(mess != 0)
                {
                    int64_t taille(0);
                    fromByteArray(&taille, mess, sizeof(int64_t));
                    if(taille > 0)
                    {
                        if(!writeSized(current_client->getSock(), mess, taille))
                        {
                            sleep = true;
                        }
                    }
                    else
                    sleep = true;
                }
                else
                sleep = true;

                delete[] mess;
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

    }while(!current_client->getStop());
}


