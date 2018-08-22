//Der Serverside Code, der mit ecph//isohost kommuniziert und ipv4/ipv6 akzeptiert

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

using namespace std;
#define PORT 3333

int main()
{
    int master_socket, addrlen, new_socket, client_socket[30],
        max_clients=30, activity, i, valread;
    int max_sd;
    struct sockaddr_in address;

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
    if((master_socket = socket(AF_INET,SOCK_STREAM,0)) == 0)
    {
        cerr << "socket creation failed" << endl;
        exit(EXIT_FAILURE);
    }
    fds[0].fd=master_socket;

    //set master socket to allow multiple connections,
    //this is just a good habit, it will work without
    int opt = 1;
    if(setsockopt(master_socket,SOL_SOCKET,SO_REUSEADDR,(char *) &opt,sizeof(opt)) < 0){
        cerr << "setsockopt failed" << endl;
        exit(EXIT_FAILURE);
    }

    //type of socket created
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(3333);

    // IPV6
    master_socket = socket(AF_INET6, SOCK_STREAM , IPPROTO_TCP);
    if (master_socket == -1) {return EXIT_FAILURE};


    //bind the socket to localhost port 3333
    if(bind(master_socket,(struct sockaddr*) &address,sizeof(address)) < 0 ){
        cerr << "bind failed" << endl;
        exit(EXIT_FAILURE);
    }
    cout << "Listening on port 3333" << endl;

    //try to specify maxiumum of 3 pending connections for master socket
    if (listen(master_socket, 3)<0){
        cerr << "only 3 max pending connections allowed!" << endl;
        exit(EXIT_FAILURE);
    }

    //accept the incoming connections
    addrlen = sizeof(address);
    cout << "Waiting for connections..." << endl;

    while (1) {

        cout << "es wird neu gepolled!" <<  endl;
        cout << fds[1].fd << endl;
        activity = poll(&fds[0],30,1000);
        cout << "erfolgreich gepolled!" << endl << "activity: " << activity << "master_sock" << master_socket <<  endl;


        if ((activity < 0) && (errno!= EINTR)){
            cerr << "SELECT/POLL ERROR!" << endl;
        }

        //If something happens on the Master Socket, then its an incomming connection
        if(fds[0].revents & POLLIN){
            if ((new_socket = accept(master_socket, (struct sockaddr*) &address,(socklen_t*) &addrlen)) <0){
                cerr << "Error beim accepten des Sockets" << endl;
                exit(EXIT_FAILURE);
            }
            cout << "New Connection!" << endl << "socket_fd is:" << new_socket << endl
                 << "ip is: " << inet_ntoa(address.sin_addr) << endl
                 << "port is: " << ntohs(address.sin_port) << endl;

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
        //else its some IO on another socket
        for(i=1;i<max_clients;i++){

            if (fds[i].revents & POLLIN){
                //Check if it was for closing, and also read the incoming message
                cout << "will was lesen" << endl;
                if((valread = read(fds[i].fd,buffer,1024)) == 0){
                    cout << "client message: " << buffer;
                    //Somebody disconnectedm get his details and print
                    getpeername(fds[i].fd, (struct sockaddr*) &address,(socklen_t*)&addrlen);
                    cout << "Somebody disconnected!" << endl << "socket_fd is:" << fds[i].fd << endl
                         << "ip is: " << inet_ntoa(address.sin_addr) << endl
                         << "port is: " << ntohs(address.sin_port) << endl;

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


    cout << message << endl;
    return 0;

}
