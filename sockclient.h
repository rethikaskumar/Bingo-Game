#ifndef SOCKCLIENT_H_INCLUDED
#define SOCKCLIENT_H_INCLUDED

#include "socket.h"
#include <iostream>

class SockClient : public Socket
{
public:
    SockClient(const std::string& host, int port);
};


#endif // SOCKCLIENT_H_INCLUDED
