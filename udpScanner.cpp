#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define MAXLINE 1024

using namespace std;

void udpScanner(string ip_as_string, port_start, port_end)
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
		if(setsockopt(sockfd, SQL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0)
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
			cout << i << "     Open     " << servType << endl; 
		
		}
		else
		{
		       if(service != NULL)
			{
				cout << i << "     Closed     " << servType << endl;
			}
		}

        	close(sockfd);

	}

}
