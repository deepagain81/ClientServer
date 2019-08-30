// This is a server

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <arpa/inet.h>
#include <sstream>
#include <fstream>
#include <iostream>

using namespace std;

//
int putpayload(char *payload)
{
	char bytetowrite[4];
	strcpy(bytetowrite, payload);
	ofstream myfile;
	myfile.open("output.txt");

	//std::fstream f("output.txt",std::ios::trunc );
	if(bytetowrite[3] == '*')
	{
		bytetowrite[3]='\0';
	}
	//f<<bytetowrite;
	myfile<<bytetowrite;
	cout<<bytetowrite<<endl;
	myfile.close();

	/////////////
/*	FILE *file;
	char bytetowrite [4];
	strcpy(bytetowrite, payload);
	file = fopen("output.txt", "w");
	int chara = fwrite(bytetowrite, sizeof(char), sizeof(bytetowrite), file);
	cout<<bytetowrite<<endl;
	fclose(file);
*/

	return 0;
}

// Main function
int main(int argc, char const *argv[])
{
	// Declare socket
	int mysocket = 0;
	if ((mysocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)	// (IP type IPv4, semantics, protocol flag 0 default
	{
		perror("Could not create socket");
		return -1;
	}

	// Setting server address to bind and listen client.
	struct sockaddr_in server_address;
	memset((char*)&server_address, 0, sizeof(server_address));		// Allocate memory to store address
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(atoi(argv[1]));		// assign port from terminal
	server_address.sin_addr.s_addr = htons(INADDR_ANY);		// Listen to any not only localhost

	// Bind the socket to listen client
	if ((bind(mysocket, (struct sockaddr*)&server_address, sizeof(server_address))) == -1)
	{
		perror("Could not bind");
		return -1;
	}

	//// Listen to client
	if((listen(mysocket, 5))== -1)
	{
		perror("Listen fail1");
		return -1;
	}

	// Receiving data: put into new address,address of client
	struct sockaddr_in client_address;
	socklen_t clen = sizeof(client_address);

	/// Accept msg from client and new socket
	int new_socket = ( accept(mysocket, (struct sockaddr*)&client_address, &clen));
	char Hello[256];	// set mas payload ready
	memset(Hello,0,256);
	int numbytesRead = read(new_socket,Hello,5);	// socket, msg, bits
	if (numbytesRead<0)
	{
		perror("Read error");
		return -1;
	}

	// Generate Random port
	srand((unsigned)time(0)); 
    int num=(rand()%65536)+1024;
	char r_port[256];
	memset(r_port,0,256);
	sprintf(r_port,"%d",num);

	// Send back r_port
    cout<< "Negotiation detected. Selected the following random port "<<r_port<<endl;
	if ((send(new_socket, r_port, 256, 0))== -1)
	{
		perror("Send fail");
		return -1;
	}
	
	
	close(mysocket);
	close(new_socket);
	

//######################### Stage: 2 DGRAM #########################
	
	int dgram_socket = 0;
	if ((dgram_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0)	// (IP type IPv4, semantics, protocol flag 0 default
	{
		perror("Could not create socket");
		return -1;
	}
	server_address.sin_port = htons(atoi(r_port));


	
	
	// Bind the socket to listen client
	if ((bind(dgram_socket, (struct sockaddr*)&server_address, sizeof(server_address))) == -1)
	{
		perror("Could not bind");
		return -1;
	}
	int flag = 0;
	char *msg;
	msg = (char*)malloc(4);
	memset(msg,0,4);

		char ack[4];
		memset(ack,0,4);
// Remove if file already exist
	//remove("output.txt");
// loop through to get all messages
	while (msg[3] !='*')
	{
		if((recvfrom(dgram_socket,msg, 4, 0, (struct sockaddr*)&client_address, &clen))== -1)
		{
			perror("Receive fail");
			return -1;
		}
		putpayload(msg);

// Capotalize all msg make ack
		
		for (int i=0; i< 4; i++)
		{
			msg[i] = toupper(msg[i]);


		}
		
		if((sendto(dgram_socket, msg, 4,0, (struct sockaddr*)&client_address, clen))== -1)
		{
			perror("Send fail");
			return -1;
		}
		
		close(new_socket);
	}
	
	

} // end main