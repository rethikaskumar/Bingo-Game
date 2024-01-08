#ifndef SOCKSERVER_H_INCLUDED
#define SOCKSERVER_H_INCLUDED

#include "socket.h"
#include <iostream>

//using namespace std;

class SockServer : public Socket
{
public:
    SockServer(int port, int connections);
    Socket* acceptConnect();
    std::string clientInfo;
};


#endif // SOCKSERVER_H_INCLUDED
