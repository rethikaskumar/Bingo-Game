//sockserver.cpp

//using namespace std;

#include "sockserver.h"
#include <sstream>

SockServer::SockServer(int port, int connections)
{
    // bind the socket to IP address
    struct sockaddr_in sa;
    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = INADDR_ANY;
    sa.sin_port = htons(port);
    if (bind(s, (sockaddr *)&sa, sizeof(sockaddr_in)) == SOCKET_ERROR)
    {
        closesocket(s);
        std::ostringstream ss;
        ss << "Could not bind socket : " << WSAGetLastError() << std::endl;
        throw(ss.str());
    }

    listen(s, connections);
}

Socket* SockServer::acceptConnect()
{
    std::stringstream ss;
    struct sockaddr_in client;
    int c = sizeof(struct sockaddr_in);
    SOCKET sock = accept(s, (struct sockaddr *)&client, &c);
    if (sock == INVALID_SOCKET)
    {
        closesocket(s);
        ss << "accept failed with error code : " << WSAGetLastError() << std::endl;
        throw(ss.str());
    }
    ss << inet_ntoa(client.sin_addr) << ":" << "[" << client.sin_port << "]";
    ss >> clientInfo;
    std::cout << "accepted connection from " << clientInfo << std::endl;
    Socket* retSock = new Socket(sock);
    return retSock;
}
