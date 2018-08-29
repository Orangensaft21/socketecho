//Der Serverside Code, der mit ecph//isohost kommuniziert und ipv4/ipv6 akzeptiert
//nomm test
#include <iostream>
#include <arpa/inet.h>
#include <string.h> //strlen
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h> //FD_SET, FD_ZERO, FD_ISSET etc
#include <poll.h>
#include <vector>
#include "sockethelper.h"

using namespace std;
#define PORT 3333

int main()
{

    SocketHelper* uiuiui = new SocketHelper();
    return 1;


    int master_socket4,master_socket6, addrlen, new_socket, client_socket[30],
        max_clients=30, activity, i, valread;
    int max_sd;
    struct sockaddr_in ipv4Addr;

    //neues geiles teil
    struct sockaddr_storage ipAddr;
    socklen_t addrLen = sizeof(ipAddr);

    char buffer[1025];  // data buffer of 1k

    //set of socket descriptors
    struct pollfd fds[max_clients];

    //welcome message
    string message = "lalalulu";


    // initialize all client_socket[] to 0 so not checked
    for (auto &fd: fds){
        fd.fd=-1;
        fd.events=POLLIN;
    }
cout << message << endl;
    //create master socket
    if((master_socket4 = socket(AF_INET,SOCK_STREAM,0)) == 0)
    {
        cerr << "socket creation failed" << endl;
        exit(EXIT_FAILURE);
    }
    fds[0].fd=master_socket4;

    //set master socket to allow multiple connections,
    //this is just a good habit, it will work without
    int opt = 1;
    if(setsockopt(master_socket4,SOL_SOCKET,SO_REUSEADDR,(char *) &opt,sizeof(opt)) < 0){
        cerr << "setsockopt failed" << endl;
        exit(EXIT_FAILURE);
    }

    //type of socket created
    ipv4Addr.sin_family = AF_INET;
    ipv4Addr.sin_addr.s_addr = INADDR_ANY;
    ipv4Addr.sin_port = htons(3333);

    //bind the socket to localhost port 3333
    if(bind(master_socket4,(struct sockaddr*) &ipv4Addr,sizeof(ipv4Addr)) < 0 ){
        cerr << "bind failed" << endl;
        exit(EXIT_FAILURE);
    }
    cout << "Listening on port 3333 (ipv4), filedesc:" << fds[0].fd << endl;

    //try to specify maxiumum of 3 pending connections for master socket
    if (listen(master_socket4, 3)<0){
        cerr << "only 3 max pending connections allowed!" << endl;
        exit(EXIT_FAILURE);
    }

    // IPV6
    struct sockaddr_in6 ipv6Addr;

    master_socket6 = socket(AF_INET6, SOCK_STREAM , IPPROTO_TCP);
    if (master_socket6 == -1) {return EXIT_FAILURE;} else {fds[1].fd=master_socket6;}

    ipv6Addr.sin6_family = AF_INET6;
    inet_pton(AF_INET6, "::1", &ipv6Addr.sin6_addr);
    ipv6Addr.sin6_port = htons(3333);

    fds[1].fd=master_socket6;
    //bind ipv6
    if(bind(master_socket6,(struct sockaddr*) &ipv6Addr,sizeof(ipv6Addr)) < 0 ){
            cerr << "bind failed" << endl;
            exit(EXIT_FAILURE);
        }
    cout << "Listening on port 3333 (ipv6), filedesc:" << fds[1].fd << endl;



    //try to specify maxiumum of 3 pending connections for master socket
    if (listen(master_socket6, 3)<0){
        cerr << "only 3 max pending connections allowed!" << endl;
        exit(EXIT_FAILURE);
    }

    //accept the incoming connections
    addrlen = sizeof(ipv6Addr);
    cout << "Waiting for connections..." << endl;

    while (1) {

        cout << "es wird neu gepolled!" <<  endl;
        cout << fds[1].fd << endl;
        activity = poll(&fds[0],30,1000);
        if(activity==0){
            continue;
        }else{
            cout << "erfolgreich gepolled!" << endl << "activity: " << activity << "master_sock" << master_socket6  << fds[1].revents <<  endl;
        }




        if ((activity < 0) && (errno!= EINTR)){
            cerr << "SELECT/POLL ERROR!" << endl;
        }
        //If something happens on the Master Socket(s), then its an incomming connection
        if((fds[1].revents & POLLIN) || (fds[0].revents & POLLIN)){
            cout << "bin hier" << endl;
            if ((new_socket = accept(master_socket6, (struct sockaddr*) &ipAddr,(socklen_t*) &addrLen)) <0){
                cerr << "Error beim accepten des Sockets"  << new_socket<< endl;
                exit(EXIT_FAILURE);
            }
            cout << "New Connection!" << endl << "socket_fd is:" << new_socket << endl
                 << "ip is: " << inet_ntoa(ipv4Addr.sin_addr) << endl
                 << "port is: " << ntohs(ipv4Addr.sin_port) << endl;

            //add new_socket to sock array
            for(auto& fdd : fds){
                //if position empty
                if(fdd.fd==-1){
                    cout << "vorher: " << fdd.fd << endl;
                    fdd.fd=new_socket;
                    //fds[1].fd = new_socket;

                    break;
                }
            }

        };
        /*if(fds[1].revents & POLLIN){
            if ((new_socket = accept(master_socket6, (struct sockaddr*) &ipv6Addr,(socklen_t*) &ipv6Addr)) <0){
                cerr << "Error beim accepten des Sockets" << endl;
                exit(EXIT_FAILURE);
            }
            char straddr[INET6_ADDRSTRLEN];
            cout << "New Connection!" << endl << "socket_fd is:" << new_socket << endl
                 << "ip is: " << inet_ntop(AF_INET6, &ipv6Addr.sin6_addr, straddr,
                                                                          sizeof(straddr)) << endl
                 << "port is: " << ntohs(ipv6Addr.sin6_port) << endl;

            //add new_socket to sock array
            for(auto& fdd : fds){
                //if position empty
                if(fdd.fd==-1){
                    cout << "vorher: " << fdd.fd << endl;
                    fdd.fd=new_socket;
                    //fds[1].fd = new_socket;

                    break;
                }
            }

        };*/
        //else its some IO on another socket
        for(i=2;i<max_clients;i++){

            if (fds[i].revents & POLLIN){

                //Check if it was for closing, and also read the incoming message
                cout << "will was lesen" << endl;
                if((valread = read(fds[i].fd,buffer,1024)) == 0){
                    cout << "client message: " << buffer;
                    //Somebody disconnectedm get his details and print
                    getpeername(fds[i].fd, (struct sockaddr*) &ipv4Addr,(socklen_t*)&addrlen);
                    cout << "Somebody disconnected!" << endl << "socket_fd is:" << fds[i].fd << endl
                         << "ip is: " << inet_ntoa(ipv4Addr.sin_addr) << endl
                         << "port is: " << ntohs(ipv4Addr.sin_port) << endl;

                    //close connection and mark as 0 for reuse
                    close(fds[i].fd);
                    fds[i].fd = -1;
                }

                //Echo back the message that came in
                else{
                    //set the string termination NULL Byte on the end of the data read
                    buffer[valread] = '\0';
                    send(fds[i].fd,buffer,strlen(buffer),0);
                }
            }
        }
    }

}
