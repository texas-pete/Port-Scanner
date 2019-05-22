

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

using namespace std;

/* Main programs starts*/
int main(int argc, char **argv)
{
   int   sd;         //socket descriptor
   int    port;         //port number
   int   start;         //start port
   int    end;         //end port
   int    rval;         //socket descriptor for connect   
   char    responce[1024];      //to receive data
   char   *message="shell";       //data to send
   struct hostent *hostaddr;   //To be used for IPaddress
   struct sockaddr_in servaddr;   //socket structure
   int    sendfd;
   int    recvfd;
   int   tx_packet;
  int    rx_packet;


   if(argv[4] = "tdp")
   {
   if (argc < 4 )
   {
      printf("------Created By www.h4ck-y0u.org-----------\n");
      printf("--------------------------------------------------\n");
      return (EINVAL);
   }
   start = atoi(argv[2]);
   end   = atoi(argv[3]);
   for (port=start; port<=end; port++)
   {

         //portno is ascii to int second argument      

   sd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP); //created the tcp socket
   if (sd == -1)
   {
     perror("Socket()\n");
     return (errno);
   }   

   memset( &servaddr, 0, sizeof(servaddr));

   servaddr.sin_family = AF_INET;
   servaddr.sin_port = htons(port); //set the portno
   
   hostaddr = gethostbyname( argv[1] ); //get the ip 1st argument
   
   memcpy(&servaddr.sin_addr, hostaddr->h_addr, hostaddr->h_length);
      
   //below connects to the specified ip in hostaddr
   
 

   rval = connect(sd, (struct sockaddr *) &servaddr, sizeof(servaddr));
   if (rval == -1)
   {
   printf("Port %d is closed\n", port);
   close(sd);
   }
   else
   printf("Port %d is open\n",port);
   
   close(sd);         //socket descriptor
   }




  struct servent *service;
  int port2;
  char proto[4] = "tcp";
  char *name;

  port2 = 21;

  service = getservbyport(htons(port2), proto);

  if(!service)
  {
        cout << "Unknown app loser" << endl;
  }
  else
  {
        name = service->s_name;
        cout << "getservbyport worked name is: " << name <<  endl;
  }

   
  }
 else
 {
	if((sendfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
	{
		perror("*** socket(,,IPPROTO_UDP) failed ***n");
		exit(-1);
	}
	if((recvfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0)
	{
		perror("*** socket(,,IPPROTO_UDP) failed ***n");
                exit(-1);

	}


	struct servent *srvport;

	for(port = start; port <= end; port++)
  	 {
    
		  srvport = getservbyport(htons(port), "udp");
		  
                 if(!srvport)
			cout << "Unknown app loser" << endl;
		 else
		       printf("tport %d: %sn", port, srvport->s_name);
	
		  fflush(stdout); 
    		
  	 } 
 }

 
    
}
