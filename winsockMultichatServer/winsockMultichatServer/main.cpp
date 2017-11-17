//Includes
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

//Preload libraries
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

//Define default port and buffer length
#define DEFAULT_PORT "27015"
#define BUFLEN 512

//Declaring a struct representing a client
struct clientStruct
{
	int id;
	SOCKET socket;
	std::string nickname;
};

//Set max clients
const int MAX_CLIENTS = 5;


int handleClient(clientStruct &newClient, std::vector<clientStruct> &clientsArray, std::thread &thread)
{
	std::string messageToSend = "";
	char receivedMessage[BUFLEN] = "";

	//Session
	while (1)
	{
		memset(receivedMessage, 0, BUFLEN);

		if (newClient.socket != 0)
		{
			int iResult = recv(newClient.socket, receivedMessage, BUFLEN, 0);

			if (iResult == SOCKET_ERROR || !strcmp(receivedMessage, "exit"))
			{
				messageToSend = newClient.nickname + " disconnected!";

				std::cout << messageToSend << std::endl;

				closesocket(newClient.socket);
				closesocket(clientsArray[newClient.id].socket);
				clientsArray[newClient.id].socket = INVALID_SOCKET;

				//Broadcast the disconnection message to the other clients
				for (int i = 0; i < MAX_CLIENTS; i++)
				{
					if (clientsArray[i].socket != INVALID_SOCKET)
						iResult = send(clientsArray[i].socket, messageToSend.c_str(), strlen(messageToSend.c_str()), 0);
				}

				break;
			}
			else if (strcmp("", receivedMessage))
				messageToSend = newClient.nickname + ": " + receivedMessage;
			std::cout << messageToSend.c_str() << std::endl;

			//Broadcast the sent message to the other clients
			for (int i = 0; i < MAX_CLIENTS; i++)
			{
				if (clientsArray[i].socket != INVALID_SOCKET)
					if (newClient.id != i)
						iResult = send(clientsArray[i].socket, messageToSend.c_str(), strlen(messageToSend.c_str()), 0);
			}
		}
	} //end while

	thread.detach();

	return 0;
}

int main() {

	WSADATA wsaData;
	struct addrinfo hints;
	struct addrinfo *server = NULL;
	SOCKET serverSocket = INVALID_SOCKET;
	std::string initMessage = "";
	std::vector<clientStruct> clients(MAX_CLIENTS);

	int tempId = -1;

	//Buffer for the client's nickname
	char clientNickname[BUFLEN];

	//Instantiate thread
	std::thread clientThread[MAX_CLIENTS];

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

	//Listen for incoming connections.
	std::cout << "Awaiting for clients to connect (up to 5)..." << std::endl;
	listen(serverSocket, SOMAXCONN);

	//Initialize list of clients
	for (int i = 0; i < MAX_CLIENTS; i++)
	{
		clients[i] = { -1, INVALID_SOCKET };
	}


	while (1)
	{

		SOCKET incomingClient = INVALID_SOCKET;
		incomingClient = accept(serverSocket, NULL, NULL);
		//If the client has invalid socket continue accepting
		if (incomingClient == INVALID_SOCKET)
			continue;
		else {

			//Receive the nickname of the client
			memset(clientNickname, 0, BUFLEN);
			recv(incomingClient, clientNickname, BUFLEN, 0);
		}



		//Create a temporary id for the next client
		tempId = -1;
		for (int i = 0; i < MAX_CLIENTS; i++)
		{
			if (clients[i].socket == INVALID_SOCKET && tempId == -1)
			{
				clients[i].socket = incomingClient;
				clients[i].id = i;
				clients[i].nickname = clientNickname;
				tempId = i;
			}

		}

		if (tempId != -1)
		{
			//Send the id to that client
			std::cout << clients[tempId].nickname << " connected!" << std::endl;
			initMessage = std::to_string(clients[tempId].id);
			send(clients[tempId].socket, initMessage.c_str(), strlen(initMessage.c_str()), 0);

			//Create a thread process for that client
			clientThread[tempId] = std::thread(handleClient, std::ref(clients[tempId]), std::ref(clients), std::ref(clientThread[tempId]));
		}
		else
		{
			initMessage = "Server is full!";
			send(incomingClient, initMessage.c_str(), strlen(initMessage.c_str()), 0);
			std::cout << initMessage << std::endl;
		}
	} //end while



	//Close listening socket
	closesocket(serverSocket);

	//Close client socket
	for (int i = 0; i < MAX_CLIENTS; i++)
	{
		clientThread[i].detach();
		closesocket(clients[i].socket);
	}

	//Clean up Winsock
	WSACleanup();
	std::cout << "Program closed successfully!" << std::endl;

	return 0;
}
