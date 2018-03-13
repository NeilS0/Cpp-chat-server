#include "build.h"
#ifdef BUILD_SERVER

#include <iostream>
#include <winsock2.h>
//#include <stdlib.h>
//#include <stdio.h>
//#include <string.h>

class ChatServer{
public:
	ChatServer();
	~ChatServer();

	//data
private:
	WSADATA wsaData;

	struct sockaddr_in locals;
	struct sockaddr_in froms;
};

ChatServer::ChatServer()
{
	if (WSAStartup(0x202,&wsaData) == SOCKET_ERROR) {
		std::cout<<"WSAStartup failed with error"<<WSAGetLastError()<<"\n";
		WSACleanup();
	}
}

int main()
{
	WSADATA wsaData;
	SOCKET listen_socket;
	SOCKET msgsock;
	struct sockaddr_in local;
	struct sockaddr_in from;
	char buffer[128];
	char *interface = NULL;
	unsigned short port = DEFAULT_PORT;
	int socket_type = DEFAULT_PROTOCOL;
	int retval = 0;
	int fromlen = 0;
	int i = 0;
	/*
	struct sockaddr_in local;
	struct sockaddr_in from;
	char buffer[128];
	char *interface = NULL;
	unsigned short port = DEFAULT_PORT;
	int socket_type = DEFAULT_PROTOCOL;
	int retval = 0;
	int fromlen = 0;
	int i = 0;
	*/

	if (WSAStartup(0x202,&wsaData) == SOCKET_ERROR) {
		std::cout<<"WSAStartup failed with error"<<WSAGetLastError()<<"\n";
		//fprintf(stderr,"WSAStartup failed with error %d\n",WSAGetLastError());
		WSACleanup();
		//return -1;
	}



	local.sin_family = AF_INET;
	local.sin_addr.s_addr = (!interface)?INADDR_ANY:inet_addr(interface); 


	// Port MUST be in Network Byte Order
	local.sin_port = htons(port);

	listen_socket = socket(AF_INET, socket_type,0); // TCP socket
	
	if(listen_socket == INVALID_SOCKET)
	{
		fprintf(stderr,"socket() failed with error %d\n",WSAGetLastError());
		WSACleanup();
		return -1;
	}

	// bind() associates a local address and port combination with the
	// socket just created. This is most useful when the application is a 
	// server that has a well-known port that clients know about in advance.

	if(bind(listen_socket,(struct sockaddr*)&local,sizeof(local) ) == SOCKET_ERROR) 
	{
		fprintf(stderr,"bind() failed with error %d\n",WSAGetLastError());
		WSACleanup();
		return -1;
	}

	// So far, everything we did was applicable to TCP as well as UDP.
	// However, there are certain steps that do not work when the server is
	// using UDP.

	// We cannot listen() on a UDP socket.

	if(socket_type != SOCK_DGRAM) 
	{
		if (listen(listen_socket,5) == SOCKET_ERROR) 
		{
			fprintf(stderr,"listen() failed with error %d\n",WSAGetLastError());
			WSACleanup();
			return -1;
		}
	}

	printf("Listening on port %d, protocol %s\n\n",port,
		   (socket_type == SOCK_STREAM) ? "TCP" : "UDP");

	printf("Server is ready to accept clients...\n\n");

	while(1) 
	{
		fromlen = sizeof(from);

		// accept() doesn't make sense on UDP, since we do not listen()
		if( socket_type != SOCK_DGRAM ) 
		{
			msgsock = accept(listen_socket,(struct sockaddr*)&from, &fromlen);

			if(msgsock == INVALID_SOCKET) 
			{
				fprintf(stderr,"accept() error %d\n",WSAGetLastError());
				WSACleanup();
				return -1;
			}

			printf("accepted connection from %s, port %d\n", 
						inet_ntoa(from.sin_addr),
						htons(from.sin_port)) ;
		}
		else
		{
			msgsock = listen_socket;
		}

		// In the case of SOCK_STREAM, the server can do recv() and 
		// send() on the accepted socket and then close it.

		// However, for SOCK_DGRAM (UDP), the server will do
		// recvfrom() and sendto()  in a loop.

		if(socket_type != SOCK_DGRAM)
		{
			retval = recv(msgsock,buffer,sizeof (buffer),0 );
		}
		else 
		{
			retval = recvfrom(msgsock,buffer,sizeof (buffer),0,
				(struct sockaddr *)&from,&fromlen);
			printf("Received datagram from %s\n",inet_ntoa(from.sin_addr));
		}
			
		if(retval == SOCKET_ERROR) 
		{
			fprintf(stderr,"recv() failed: error %d\n",WSAGetLastError());
			closesocket(msgsock);
			continue;
		}

		if(retval == 0) 
		{
			printf("Client closed connection\n");
			closesocket( msgsock );
			continue;
		}

		printf("Received %d bytes, data [%s] from client\n",retval,buffer);
		printf("Echoing same data back to client\n");

		if(socket_type != SOCK_DGRAM)
		{
			retval = send(msgsock,buffer,sizeof(buffer),0);
		}
		else
		{
			retval = sendto(msgsock,buffer,sizeof (buffer),0, (struct sockaddr *)&from,fromlen);
		}

		if(retval == SOCKET_ERROR) 
		{
			fprintf(stderr,"send() failed: error %d\n",WSAGetLastError());
		}

		if(socket_type != SOCK_DGRAM)
		{
			printf("Terminating connection\n\n");
			closesocket(msgsock);
		}
		else 
		{
			printf("UDP server looping back for more requests\n");
		}

		continue;
	}

	std::cin.get();
	return 0;
}

#endif	//BUILD_SERVER