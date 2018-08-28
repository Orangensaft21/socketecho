#ifndef SOCKETHELPER_H
#define SOCKETHELPER_H
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <poll.h>
#include <mutex>


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
    std::mutex connectionMutex;

private:

};



typedef struct thread_data
{
    int socket;
    char client_address[100];
} thread_data_t;

void connectionHandler(thread_data th);

#endif // SOCKETHELPER_H
