//using namespace std;

#include "socket.h"
#include <iostream>
#include <sstream>


int Socket::numSockets = 0;

//default constructor
Socket::Socket() : s(0), refCount(0)
{
    // initialize socket interface
    beginInterface();

    // TCP: SOCK_STREAM
    if((s = socket(AF_INET , SOCK_STREAM , 0 )) == INVALID_SOCKET)
    {
        std::ostringstream ss;
        ss << "Could not create socket : " << WSAGetLastError() << std::endl;
        throw(ss.str());
    }
    refCount++;
}

//copy constructor
Socket::Socket(SOCKET sock) : s(sock), refCount(0)
{
    // initialize socket interface
    beginInterface();
    refCount++;
};

//assignment operator
Socket& Socket::operator=(Socket& sock)
{
    refCount = sock.refCount;
    refCount++;
    s = sock.s;
    numSockets++;
    return *this;
}

Socket::~Socket()
{
    if (0 == --refCount)
    {
        close();
    }

    --numSockets;
    if (0 == numSockets)
    {
        endInterface();
    }
}

//close()
void Socket::close()
{
    closesocket(s);
}

//begin
void Socket::beginInterface()
{
    if (!numSockets)
    {
        WSADATA wsa;
        if (WSAStartup(MAKEWORD(2, 2), &wsa))
        {
            std::cout << "WSAStartup Failed. Error Code : " << WSAGetLastError() << std::endl;
        }
        else
        {
            ++numSockets;
        }
    }
}

//end
void Socket::endInterface()
{
    WSACleanup();
}

//SendString
//transmits length of the string first, followed by data bytes
void Socket::sendString(const std::string& str)
{
    int len = str.length();
    send(s, (const char*)&len, sizeof(int),0);
    send(s, str.c_str(), str.length(),0);
}

//RecvString
//receives length of the string first, followed by data bytes
void Socket::recvString(std::string& str)
{
    int len;
    if( sizeof(int) == recv(s, (char *)&len, sizeof(int), 0))
    {
        char r;
        int ret = 1;
        for(int i = 0; (1 == ret) && (i<len); i++)
        {
            ret = recv(s, &r, 1, 0);
            if(1 == ret) str += r;
        }
    }
}
