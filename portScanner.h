#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <arpa/inet.h>

#define MAXLINE 1024


using namespace std;

void tcpScanner(string IPstring, int start, int end)
{
   int   sd;         //socket descriptor
   int   port;         //port number
   int   rval;         //socket descriptor for connect
   char  responce[1024];      //to receive data
   char  *message="shell";       //data to send
   struct hostent *hostaddr;   //To be used for IPaddress
   struct sockaddr_in servaddr;   //socket structure
   const char* ip = IPstring.c_str();
   string portSwitch;

    //prep printing 
    cout << "---------------TCP---------------" << endl; 
    cout << ip << endl;
    cout << "     PORT     STATE     SERVICE" << endl;

    //Scanning Starts Here 
    for (port=start; port<=end; port++)
    {
        sd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP); //created the tcp socket
        if (sd == -1)
        {
        perror("Socket()\n");
        }

        memset( &servaddr, 0, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(port); //set the portno
        hostaddr = gethostbyname( ip ); //get the ip 1st argument
        memcpy(&servaddr.sin_addr, hostaddr->h_addr, hostaddr->h_length);


    

        //below connects to the specified ip in hostaddr
        rval = connect(sd, (struct sockaddr *) &servaddr, sizeof(servaddr));
        if (rval == -1)
        {
            portSwitch = "OPEN";
            close(sd);
        }
        else
        {
            portSwitch = "OPEN";
        }
        close(sd);         //socket descriptor
    


        struct servent *service;
        char proto[4] = "tcp";
        char *name;

        service = getservbyport(htons(port), proto);

        if(!service)
        {
                name = "Unknown";
        }
        else
        {
                name = service->s_name;
                
        }

        cout << port << "     " << portSwitch << "     " << name << endl;
    }

}

void udpScanner(string ip_as_string,int port_start, int port_end)
{
        //Socket declaration stuff
        struct hostent *ipServer;
        int sockfd = 0, valread, len;
        struct sockaddr_in serv_addr;
        char *hello = "praise the sun";
        char buffer[MAXLINE];
        struct servent *service;
        string servType;

        //Grab Ip from string and ensure it is correct
        const char* ip = ip_as_string.c_str();
        if(inet_pton(AF_INET, ip, &(serv_addr.sin_addr)) <= 0)
        {
                cout << "IP IS BAD" << endl;
                exit(1);
        }

        ipServer = gethostbyname(ip); //get ip address


        cout << endl << "UDP PORT     STATE     SERVICE" << endl;

        //Create and test all UDP ports in a loop
        for(int i = port_start; i < port_end; i++)
        {
                int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
                int n;
                if(sockfd < 0) //Test socket
                        cout << "Error opening Socket" << endl;

                bzero((char *)&serv_addr, sizeof(serv_addr));
                serv_addr.sin_family = AF_INET;
                bcopy((char *)ipServer->h_addr, (char *)&serv_addr.sin_addr.s_addr, ipServer->h_length);
                serv_addr.sin_port = htons(i); //port number
                //attempt to connect to server
                if(connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
                        cout << "Error connecting" << endl;

                //get and set service name
                service = getservbyport(htons(i), "udp");


                //Send and Recv from server
                sendto(sockfd,(const char*)hello, strlen(hello), MSG_CONFIRM,(const struct sockaddr *) &serv_addr, sizeof(serv_addr));

                struct timeval tv;
                tv.tv_sec = 0;
                tv.tv_usec = 10000; //timeout value
                if(setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0)
                {
                        perror("Error");
                }

                valread = recvfrom(sockfd, (char*)buffer, MAXLINE, MSG_WAITALL, (struct sockaddr*) &serv_addr, &len);
                
                if(errno == 11)
                {
                        if(service != NULL)
                                servType = service->s_name;
                        else
                                servType = "Unknown";

                        //print everything
                        cout << i << "     OPEN     " << servType << endl;

                }
                else
                {
                       if(service != NULL)
                        {
                                cout << i << "     CLOSED     " << servType << endl;
                        }
                }

                close(sockfd);
        }
}



void printHelp(){
    cout << "Options Available to user are as shown: " << endl; 
    cout << "--help (this is the only accepted input)" << endl;
    cout << "--port (--port 23) (--port 21,80,443) (--port 2000-3000)" << endl;
    cout << "--ip/file (--ip 127.0.0.1) (--ip 127.0.0.1, 127.0.0.2, 127.0.0.3 ) (--ip 127.0.0.1-127.0.0.5) (--ip <filename>)" << endl;
    cout << "--transport (--transport TCP) (--transport UDP)"<< endl;
}
