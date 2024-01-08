//using namespace std;

#include "sockclient.h"
#include <sstream>

SockClient::SockClient(const std::string& host, int port) : Socket() {
    hostent *he = gethostbyname(host.c_str());
    if (NULL == he) {
        std::ostringstream ss;
        ss << "Error <" << WSAGetLastError() << "> Resolving Hostname <" << host.c_str() << ">" << std::endl;
        //std::cout<<typeid(ss.str());
        throw(ss.str());
    }

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr = *((in_addr *)he->h_addr);
    memset(&(addr.sin_zero), 0, 8);

    int retval;
    retval = ::connect(s, (sockaddr *) &addr, sizeof(sockaddr));
    if (retval)
    {
        std::ostringstream ss;
        ss << "Error <" << WSAGetLastError() << "> connecting" << std::endl;
        throw(ss.str());
    }
}
