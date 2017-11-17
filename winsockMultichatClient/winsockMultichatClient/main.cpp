#include<iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <string>
#include <sstream>
#include <thread>

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")
//Setting the buff-length and port to use.
#define BUFLEN 512            
#define DEFAULT_PORT "27015"
//Added namespace so "std::" in front of variables isn't needed.
using namespace std;
//Creating variables for the client, giving each client a socket, ID, nickname and the option to recieve messages.
struct clientStruct
{
	SOCKET socket;
	int id;
	string nickname;
	char receivedMessage[BUFLEN];
};

int handleClient(clientStruct &newClient)
{
	while (1)
	{
		memset(newClient.receivedMessage, 0, BUFLEN);

		if (newClient.socket != 0)
		{
			int iResult = recv(newClient.socket, newClient.receivedMessage, BUFLEN, 0);

			if (iResult != SOCKET_ERROR)
				cout << newClient.receivedMessage << endl;
			else if (WSAGetLastError() == WSAECONNRESET)
			{
				cout << "The server has shut down!" << endl;
				break;
			}
			else {
				cout << "recv() failed: " << WSAGetLastError() << endl;
				break;
			}
		}
	}

	return 0;
}

int __cdecl main(int argc, char **argv) {

	WSAData wsa_data;
	struct addrinfo *result = NULL, *ptr = NULL, hints;
	clientStruct currentClient = { INVALID_SOCKET, -1, "" };
	int iResult = 0;
	string sentMessage = "";
	string checkServerMessage;

	if (argc != 3) {
		printf("usage: %s server-name nickname (to be displayed in the chat)\n", argv[0]);
		return 1;
	}

	cout << "Entering chat with nickname: " << argv[2] << endl;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsa_data);
	if (iResult != 0) {
	cout << "WSAStartup() failed with error: " << iResult << endl;
	return 1;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	cout << "Connecting to server...\n";

	// Resolve the server address and port
	iResult = getaddrinfo(argv[1], DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
	cout << "getaddrinfo() failed with error: " << iResult << endl;
	WSACleanup();

	return 1;
	}

	SOCKET ConnectSocket = INVALID_SOCKET;
	ptr = result;

	// Attempt to connect to an address until one succeeds
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

	// Create a SOCKET for connecting to server
	currentClient.socket = socket(ptr->ai_family, ptr->ai_socktype,
	ptr->ai_protocol);
	if (currentClient.socket == INVALID_SOCKET) {
	cout << "socket() failed with error: " << WSAGetLastError() << endl;
	WSACleanup();

	return 1;
	}

	// Connect to server.
	iResult = connect(currentClient.socket, ptr->ai_addr, (int)ptr->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
	closesocket(currentClient.socket);
	currentClient.socket = INVALID_SOCKET;
	continue;
	}
	break;
	}

	freeaddrinfo(result);

	if (currentClient.socket == INVALID_SOCKET) {
	cout << "Unable to connect to server!" << endl;
	WSACleanup();

	return 1;
	}

	cout << "Successfully connected!" << endl;

	//Send client nickname to server
	send(currentClient.socket, argv[2], strlen(argv[2]), 0);

	//Obtain id from server for this client;
	recv(currentClient.socket, currentClient.receivedMessage, BUFLEN, 0);
	checkServerMessage = currentClient.receivedMessage;

	if (checkServerMessage != "Server is full")
	{
		currentClient.id = atoi(currentClient.receivedMessage);

		thread my_thread(handleClient, currentClient);

		cout << "Input message: " << endl;

		while (1)
		{
			getline(cin, sentMessage);

			iResult = send(currentClient.socket, sentMessage.c_str(), strlen(sentMessage.c_str()), 0);

			if (iResult <= 0)
			{
				continue;
			}
			else if (!strcmp(sentMessage.c_str(), "exit")) {
				cout << "Exiting chat..." << endl;
				break;
			}
			
		}


		//Shutdown the connection since no more data will be sent
		my_thread.detach();
	}
	else
		cout << currentClient.receivedMessage << endl;


	cout << "Shutting down socket..." << endl;
	iResult = shutdown(currentClient.socket, SD_SEND);

	if (iResult == SOCKET_ERROR) {
		cout << "shutdown() failed with error: " << WSAGetLastError() << endl;
		closesocket(currentClient.socket);
		WSACleanup();

		return 1;
	}

	closesocket(currentClient.socket);
	WSACleanup();

	return 0;
}