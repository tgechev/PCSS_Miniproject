#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>
#include <thread>
#include <vector>


#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define DEFAULT_PORT "27015"
#define BUFLEN 512

//Declaring a struct representing a client
struct clientStruct
{
	int id;
	SOCKET socket;
	std::string nickname;
};

int main() {

	WSADATA wsaData;
	struct addrinfo hints;
	struct addrinfo *server = NULL;
	SOCKET serverSocket = INVALID_SOCKET;
	std::string initMessage = "";

	//Initialize Winsock
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	//Setup hints
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;


	//Setup Server
	std::cout << "Setting up server..." << std::endl;
	getaddrinfo(NULL, DEFAULT_PORT, &hints, &server);

	//Create a listening socket for connecting to server
	serverSocket = socket(server->ai_family, server->ai_socktype, server->ai_protocol);

	//Assign an address to the server socket
	bind(serverSocket, server->ai_addr, (int)server->ai_addrlen);

	//Listen for incoming connections
	std::cout << "Awaiting for the client to connect" << std::endl;
	listen(serverSocket, SOMAXCONN);


 
	while (1)
	{
		//Accept client socket
		SOCKET incomingClient = INVALID_SOCKET;
		incomingClient = accept(serverSocket, NULL, NULL);
		//If the client has invalid socket continue accepting
		if (incomingClient == INVALID_SOCKET)
			continue;
	}

	//Close listening socket
	closesocket(serverSocket);

	//Clean up Winsock
	WSACleanup();
	std::cout << "Program closed successfully!" << std::endl;

	return 0;
}