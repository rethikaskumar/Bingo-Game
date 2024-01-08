//socket.h

#ifndef __SOCKET_H
#define __SOCKET_H

#include <iostream>
#include <winsock2.h>

class Socket
{
public:
    Socket(const Socket&); // copy constructor
    Socket& operator=(Socket&); // assignment operator
    virtual ~Socket();

    void close();
    void sendString(const std::string&);
    void recvString(std::string& str);

protected:
    Socket(SOCKET s);
    Socket();

    SOCKET s;
    int refCount;

    friend class SockServer;

private:
    static void beginInterface();
    static void endInterface();
    static int numSockets;
};

#endif //__SOCKET_H
