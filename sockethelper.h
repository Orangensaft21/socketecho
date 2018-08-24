#ifndef SOCKETHELPER_H
#define SOCKETHELPER_H
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <poll.h>


class SocketHelper
{
public:
    SocketHelper();
    ~SocketHelper();
    void init(); //TODO Init IPV4 only?
    void pollSock();

    struct addrinfo hints, *infoptr; // so brauchen wir keine memset global variablen
    //einmal für ipv4 einmal ipv6
    struct pollfd fds[2];
    struct sockaddr_in  peeraddr;
};



#endif // SOCKETHELPER_H
