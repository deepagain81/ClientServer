// this is a client

#include <stdio.h>
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

char * getpayload(const char *file, int position)
{
	FILE *myfile;
	char *buffer;
	size_t result;

	myfile = fopen(file,"rb");
	fseek(myfile, position, SEEK_SET);
	buffer = (char*)malloc(5);
	memset(buffer,0,5);
	result = fread(buffer, 1, 4, myfile);
	
	if(result < 4)
	{
		
		buffer[3] = '*';
		//cout<<"result: "<<(result)<<endl;
	}
	fclose(myfile);
	return buffer;
}

int getlenght(const char*file)
{
	FILE *myfile;
	char *buffr;
	size_t length;
	long lsize;
	myfile = fopen(file,"rb");
	fseek(myfile, 0, SEEK_END);
	lsize = ftell(myfile);
	rewind(myfile);
	buffr = (char*)malloc(sizeof(char)*lsize);
	length = fread(buffr, 1, lsize, myfile);
	fclose(myfile);
	return length;
}

// main function
int main(int argc, char const *argv[])
{
//####################### Stage:1 Negotiation ######################
	// --------------------------Declare socket--------------------------
	int mysocket = 0;
	if ((mysocket = socket(AF_INET, SOCK_STREAM,0)) < 0) // SOCK_STREAM for TCP
	{
		perror("Could not create socket");
		return -1;
	}// end if

	// --------------------------Get host address--------------------------
	struct hostent *s;
	s = gethostbyname(argv[1]);

	// --------------------------Set destination/server address to initiate negotiation.--------------------------
	struct sockaddr_in server_address;
	memset((char*)&server_address,0, sizeof(server_address)); // set memory space
	server_address.sin_family = AF_INET;	
	server_address.sin_port = htons(atoi(argv[2]));	// port from terminal
	bcopy((char*)s->h_addr, (char*)&server_address.sin_addr.s_addr, s->h_length);
	socklen_t slen = sizeof(server_address);

	// --------------------Connect to server----------------------------
	if ((connect(mysocket, (struct sockaddr*)&server_address, slen))== -1)
	{
		perror("Connection fail");
		return -1;
	}

	//----------------> Sending & Receiving <---------------------
	// --------------------------Message--------------------------
	char payload[256];
	memset(payload,0,256);
	sprintf(payload,"%d",259);
	// --------------------------Send msg to server--------------------------//use write/read
	if ((send(mysocket, payload, 256, 0))== -1)
	{
		perror("Send fail");
		return -1;
	}

	// --------------------------Receive ack--------------------------
	char r_port[256];
	memset(r_port,0,256);
	int m =0;
	m = socket(AF_INET, SOCK_STREAM,0);
	//int m = accept(mysocket, (struct sockaddr*)&client_address, &clen);
	int numbytesRead = read(mysocket,r_port,5);
	
	cout<<"Random port:"<<r_port<<endl;

	// --------------------------Close the socket--------------------------
	close(mysocket);

// ##################### Stage: 2 DGRAM Transfer ####################

// Declear DGRAM socket

	const char *file = argv[3];
	int data_socket = 0;
	if((data_socket = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
	{
		perror("socket fail");
		return -1;
	}

	// Set new port
	server_address.sin_port = htons(atoi(r_port));

// Read file
	cout<<"here"<<endl;
	int len = getlenght(file);
	cout<<"len"<<endl;
	char *mess;


	for (int i=0; i<len; i=i+4)
	{
		mess = getpayload(file,i);
		
		//result = fread(buffer, 1, 4, myfile);
		//cout<<"result: "<<result<<endl;
		if((sendto(data_socket, mess, 5,0, (struct sockaddr*)&server_address, slen))== -1)
		{
			perror("Send fail");
			return -1;
		}
		
		//cout<<"result:"<<mess<<endl;
		///////////////////////////////////
		char msg[4];
		memset(msg,0,8);
		if((recvfrom(data_socket,msg, 4, 0, (struct sockaddr*)&server_address, &slen))== -1)
		{
			perror("Receive fail");
			return -1;
		}
		cout<<(msg)<<endl;

	}

	close(data_socket);

}// end main

