#include<iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <string>
#include <sstream>

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define BUFLEN 512            
#define DEFAULT_PORT "27015"

using namespace std;

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

	// Send an initial buffer
	iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
	if (iResult == SOCKET_ERROR) {
		printf("send failed: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}

	printf("Bytes Sent: %ld\n", iResult);

	// shutdown the connection for sending since no more data will be sent
	// the client can still use the ConnectSocket for receiving data
	iResult = shutdown(ConnectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}

	// Receive data until the server closes the connection
	do {
		iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0)
			printf("Bytes received: %d\n", iResult);
		else if (iResult == 0)
			printf("Connection closed\n");
		else
			printf("recv failed: %d\n", WSAGetLastError());
	} while (iResult > 0);
	
	iResult = shutdown(ConnectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}

	closesocket(ConnectSocket);
	WSACleanup();

	return 0;
}