#include <iostream> 
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sstream>
#define MAXLINE 1024

using namespace std;


vector<string> ip_addresses; //vector to hold ips 
vector<string> portVector; //vector to hold ports 
string transport_type; 


/////////////////////////////////////////////////////////////////////////
void tcpScanner(string IPstring, int start, int end)
{
   int   sd;         //socket descriptor
   int   port;         //port number
   int   rval;         //socket descriptor for connect
   char  responce[1024];      //to receive data
   char  *message= "shell";       //data to send
   struct hostent *hostaddr;   //To be used for IPaddress
   struct sockaddr_in servaddr;   //socket structure
   const char* ip = IPstring.c_str(); //saves string 
   bool isOpen = false;
   string portSwitch;

    //prep printing 
    cout << "---------------TCP---------------" << endl; 
    cout << ip << endl;
    cout << "PORT     STATE     SERVICE" << endl;

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
        {//if it is closed
            portSwitch = "CLOSED";
			
            close(sd);
        }
        else
        {//if open
			isOpen = true;
            portSwitch = "OPEN";
        }
        close(sd);         //socket descriptor
    


        struct servent *service;
        char proto[4] = "tcp";
        string name;

        service = getservbyport(htons(port), proto);

        if(!service)
        {
                name = "Unknown";
        }
        else
        {
                name = service->s_name;
                
        }
		if(isOpen == true){
			cout << port << "     " << portSwitch << "     " << name << endl;
		}
		else if(name != "Unknown"){
			cout << port << "     " << portSwitch << "     " << name << endl;
		}
    }

}

void udpScanner(string ip_as_string,int port_start, int port_end)
{
        //Socket declaration stuff
        struct hostent *ipServer;
        int sockfd = 0, valread;//, len;
		
        struct sockaddr_in serv_addr;
		socklen_t len = sizeof(&serv_addr);
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

		cout << "---------------UDP---------------" << endl;
		cout << ip << endl;
        cout << endl << "PORT     STATE     SERVICE" << endl;

        //Create and test all UDP ports in a loop
        for(int i = port_start; i <= port_end; i++)
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////



void help_parse(int argc, char* argv[])
{
    //if help is one of the command line arguments, print help and exit program
    
    for(int i = 0; i < argc; i++)
    {
      if(string(argv[i]) == "--help")
      { 
         printHelp();
         exit(1);
      }
    }
}
    
    
/*vector<string>*/void ip_parse(int argc, char* argv[])
{

 
   for(int i = 0; i < argc; i++)
   {
     //turn argv into one long string regardless of type
     string comma_ip_string = string(argv[i]);
     string comma = ",";
     string dash = "-";
	if(comma_ip_string == "--ip")
	{
		 comma_ip_string = string(argv[i + 1]);
		 if(comma_ip_string.find(comma) != string::npos) //if the ip is a comma separated list then:
		 {
	
		   //populate vector with ips at indexes
			
			char str[1024];
			strcpy(str, argv[i + 1]);
			char *pch;
			
			pch = strtok(str, ",");
			while(pch != NULL){
					ip_addresses.push_back(pch);
					cout << pch << endl;
					pch = strtok(NULL, ",");
			}
			
			/*stringstream ss(comma_ip_string);
			string i; 
			while(ss >> i)
			{
			  ip_addresses.push_back(i);
			  if(ss.peek() == ',')
				ss.ignore();
			}*/
			

			//return ip_addresses;  
		 }
		 else if(comma_ip_string.find(dash) != string::npos) //if the ip is a range then:
		 {
	
		   //Substring for ip before dash
		   int pos1 = comma_ip_string.find("-");
		   string beforeDash = comma_ip_string.substr(0,pos1);

		   //Substring for ip after dash
		   string afterDash = comma_ip_string.substr(pos1+1);

		   //Substring for first 3 octets
		   reverse(comma_ip_string.begin(), comma_ip_string.end());
		   int pos2 = comma_ip_string.find(".");
		   
		   reverse(comma_ip_string.begin(),comma_ip_string.end());

		   //Substring for last octet of first value before dash
		   reverse(beforeDash.begin(), beforeDash.end());
		   int pos3 = beforeDash.find(".");
		   string lastOctetBeforeDash = beforeDash.substr(0,pos3);
		   reverse(lastOctetBeforeDash.begin(), lastOctetBeforeDash.end());
		   reverse(beforeDash.begin(), beforeDash.end());
		
			string firstThreeOctets = comma_ip_string.substr(0, comma_ip_string.length() - pos2);
		   
		   reverse(comma_ip_string.begin(), comma_ip_string.end());
		   //cout << "comma: " << comma_ip_string << endl;
		   
		   for(int i = stoi(lastOctetBeforeDash); i <= stoi(afterDash); i++)
		   {
			   cout << i << endl;
			 ip_addresses.push_back(firstThreeOctets+to_string(i));
		   }
		
		 }
		 else //if the ip is an individual ip then:
		 {
			ip_addresses.push_back(string(comma_ip_string));   
		 }
	}
   }
   
}    

/*vector<string>*/void file_parse(int argc, char* argv[])
{
  for(int i = 0; i < argc; i++)
  {
    if(string(argv[i]) == "--file")
    {
      //if we see --file, open file 
      ifstream infile(argv[i+1]);

      if(!infile) //check if file opens proper
      {
        cout << "Error opening file" << endl;
      }

      string str;

      while(getline(infile,str)) //Read next line until EOF
      {
        if(str.size() > 0) //If line contains string of size > 0 then push it to vector 
            ip_addresses.push_back(str);
      }

      infile.close();
    }
  }
	
  //return ip_addresses;
}

/*vector<string>*/void port_parse(int argc, char* argv[])  
{
   for(int i = 0; i < argc; i++)
   {


	//Turn argv into one long string 
    
	string comma = ",";
	string dash = "-";



     if(string(argv[i]) == "--port")
     {
		 string port_string = string(argv[i+1]);
       if(port_string.find(comma)) //if the ports are comma separated list then 
       {
        //populate vector with ips at indexes
        stringstream pp(port_string);
        int i; 
        while(pp >> i)
        {
          portVector.push_back(to_string(i));
          if(pp.peek() == ',')
            pp.ignore();
        }
		
		
	
        //return portVector;
       }
       else if(port_string.find(dash)) //if the ports are a range then 
       {
		   string port_string = string(argv[i+1]);
      //Substring for port before dash
      int pos1 = port_string.find("-");
      string beforeDashPort = port_string.substr(0,pos1);

      //Substring for port after dash
      string afterDashPort = port_string.substr(pos1+1);

      for(int i = stoi(beforeDashPort); i < stoi(afterDashPort); i++)
      {
        portVector.push_back(beforeDashPort + to_string(i));
      }
      //return portVector;

      }
       else //if its an individual port 
       {
		 string port_string = string(argv[i+1]);
         portVector.push_back(port_string);
         //return portVector;
       }
     }
   }
   
}
  
string transport_parse(int argc, char* argv[])
{
   for(int i = 0; i < argc; i++)
   {
     if(string(argv[i]) == "--transport")
     {
       if(string(argv[i+1]) == "tcp")
       {
         transport_type = "tcp";
       }
       else if(string(argv[i+1]) == "udp")
       {
         transport_type == "udp";
       }
     }
   }
   return transport_type;
} 


int main(int argc, char*argv[])
{
  //pass all of the command line arguments into my various parsing functions 
   /*for(int i = 0; i < argc; i++)
   {*/

     help_parse(argc, argv);
     ip_parse(argc, argv);
     file_parse(argc, argv);
     port_parse(argc, argv);
     transport_parse(argc, argv);
   //}

   //run scanners according to transport type/given ports

   if(portVector.empty()) //if there are no given ports, scan 1-1024 for both port types
   {
      for(int i = 0; i < ip_addresses.size(); i++ )
      {
        tcpScanner(ip_addresses[i], 1, 1024);
        udpScanner(ip_addresses[i], 1, 1024);
      }
   }
   else if (portVector.empty() && transport_type == "tcp") //if user does not give ports but gives type tcp then scan tcp for ports 1-1024
   {
      for(int i = 0; i < ip_addresses.size(); i++ )
      {
        tcpScanner(ip_addresses[i], 1,1024);
      }
   }
   else if(portVector.empty() && transport_type == "udp")  //if user does not give ports but gives type udp then scan udp for ports 1-1024
   {
      for(int i = 0; i < ip_addresses.size(); i++ )
      {
        udpScanner(ip_addresses[i], 1,1024);
      }
   }
   else if(!portVector.empty()) //if user gives ports but no type scan given ports for both udp and tcp
   {
      for(int i = 0; i < ip_addresses.size(); i++ )
      {
        for(int j = 0; j <portVector.size(); j++)
        {
          tcpScanner(ip_addresses[i], stoi(portVector[j]), stoi(portVector[j]));
		  udpScanner(ip_addresses[i], stoi(portVector[j]), stoi(portVector[j]));
		 
        }
      }
   }
   else if(!portVector.empty() && transport_type == "tcp") //Scan given ports on tcp only 
   {
      for(int i = 0; i < ip_addresses.size(); i++ )
      {
        for(int j = 0; j <portVector.size(); j++)
        {
          tcpScanner(ip_addresses[i], stoi(portVector[j]), stoi(portVector[j]));
        }
      }
   }
   else if(!portVector.empty() && transport_type == "udp") //Scan given ports for udp only
   {
      for(int i = 0; i < ip_addresses.size(); i++ )
      {
        for(int j = 0; j <portVector.size(); j++)
        {
          udpScanner(ip_addresses[i], stoi(portVector[j]), stoi(portVector[j]));
        }
      }
   }

}
