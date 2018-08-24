#include "sockethelper.h"
#include <iostream>
#include <poll.h>
#include <unistd.h>

using namespace std;

SocketHelper::SocketHelper()
{
    init();
}

SocketHelper::~SocketHelper()
{
    freeaddrinfo(infoptr);
}

void SocketHelper::init()
{
    cout << "starte was" << endl;
    char host[256];

    //die hints muss man setzen, damit nicht alle möglichen Addressen ausgegeben werden
    hints.ai_flags = AI_PASSIVE |AI_ADDRCONFIG;
    // fuer bind muss node NULL und AI_PASSIVE als flag gesetzt sein
    // passive nicht gesetzt: socket zum senden benutzen
    hints.ai_socktype = SOCK_STREAM;

    //speichert ipv4 und ipv6 addresse in infoptr
    int result = getaddrinfo("localhost","3333", &hints, &infoptr);
    if (result){
        cerr <<"getaddrinfo: " << gai_strerror(result);
    }


    //Anzahl Filde descriptors
    unsigned int numfds = 0;
    struct addrinfo* p;
    for (p=infoptr; p!=nullptr; p = p->ai_next){
        fds[numfds].fd = socket(p->ai_family,p->ai_socktype,p->ai_protocol);

        if (fds[numfds].fd == -1){
            cerr << "Socket init failed" << endl;
            exit(EXIT_FAILURE);
        }else{
            cout << "SOCKET CREATED: " << endl;
            getnameinfo(p->ai_addr, p->ai_addrlen, host, sizeof (host), nullptr, 0, NI_NUMERICHOST);
            cout << host << endl;
        }

        //da wir an dem socket Verbindungen akzeptieren, sind wir an POLLIN events interessiert
        fds[numfds].events = POLLIN;
        //damit der Port immer benutzbar ist REUSEADDR
        int enable = 1;
        setsockopt(fds[numfds].fd,SOL_SOCKET,SO_REUSEADDR,&enable,sizeof (enable));

        //bind socket
        int errCode;
        if (( errCode = bind(fds[numfds].fd, p->ai_addr, p->ai_addrlen)) != 0){
            if (errno != EADDRINUSE){
                cerr << "ERROR socket bind failed! " << errCode << endl;
                exit(EXIT_FAILURE);
            }
            close(fds[numfds].fd);
        }

        //listen on Socket
        if((errCode = listen(fds[numfds].fd,SOMAXCONN)) != 0){
            cerr << "ERROR socket listen failed! "  << errCode << endl;
            exit(EXIT_FAILURE);
        }

        ++numfds;
    }


    //nur Testausgabe


    for (p=infoptr; p!= nullptr; p = p->ai_next){
        getnameinfo(p->ai_addr, p->ai_addrlen, host, sizeof (host), nullptr, 0, NI_NUMERICHOST);
        cout << sizeof (infoptr)/sizeof (infoptr[0]) << endl;
        cout << host << endl;
    }

    pollSock();
}

/*
 * TODO IFDEF IPV4 ACCEPT ON SOCK 1
 *      IFDEF IPV6 ACCEPT ON SOCK 0
 */
void SocketHelper::pollSock(){
    int socket_0=fds[0].fd,socket_1=fds[1].fd;
    int peeraddr_len = sizeof(peeraddr);

    while ((socket_0 = accept(socket_1, (struct sockaddr*) &peeraddr, (socklen_t*) &peeraddr_len))) {
        cout << "juju" << socket_0 << endl;
    }


}


