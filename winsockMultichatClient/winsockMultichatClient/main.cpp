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

	if (argc != 3) {
		printf("usage: %s server-name nickname (to be displayed in the chat)\n", argv[0]);
		return 1;
	}

	cout << "Entering chat with nickname: " << argv[2] << endl;

	// Initialize Winsock
	//Winsock (Windows Sockets DLL) is intialized in order to be able to call Winsock functions
	//Doing this also makes sure that Winsock is supported on the system

	//Calls WSAStartup and return its value as an integer
	//The if-statement is made to check for errors 
	iResult = WSAStartup(MAKEWORD(2, 2), &wsa_data);
	if (iResult != 0) {
	cout << "WSAStartup() failed with error: " << iResult << endl;
	return 1;
	}

	//The internet address family is unspecified, which means that either an IPv5 or IPv4 address can be returned
	//The application requests the socket type to be a stream socket for the TCP protocol
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;			//unspecified add. family
	hints.ai_socktype = SOCK_STREAM;		//socket type = stream
	hints.ai_protocol = IPPROTO_TCP;		//TCP protocol

	//Output the text "connecting to server..." to the console window
	cout << "Connecting to server...\n";	

	//The following line of code calls the getaddrinfo function and requests the IP address for the server name passed on the command´line
	//The TCP port on the server, which the client will connect to, is defined by DEFAULT_PORT as 27015
	//The getaddrinfo function returns a integer value, which is checked for errors (if-statement)
	// Resolve the server address and port
	iResult = getaddrinfo(argv[1], DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
	cout << "getaddrinfo() failed with error: " << iResult << endl;
	WSACleanup();					//WSACleanup is used to terminate the use of WS2_32 DLL
	return 1;
	}

	//Creates a socket object called ConnectSocket
	SOCKET ConnectSocket = INVALID_SOCKET;

	//Attempt to connect to the first address returned by the call to getaddrinfo
	ptr = result;

	// Attempt to connect to an address until one succeeds
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

	// Create a SOCKET for connecting to server
	currentClient.socket = socket(ptr->ai_family, ptr->ai_socktype,
	ptr->ai_protocol);
	
	//Check for errors to ensure that the socket is a valid socket
	if (currentClient.socket == INVALID_SOCKET) {
	cout << "socket() failed with error: " << WSAGetLastError() << endl;
	WSACleanup();				
	return 1;
	}

	//Calls the connect function, passing the created socket and the sockaddrstructure as parameters
	// Connect to server.
	iResult = connect(currentClient.socket, ptr->ai_addr, (int)ptr->ai_addrlen);
	//Checks for errors
	if (iResult == SOCKET_ERROR) {
	closesocket(currentClient.socket);
	currentClient.socket = INVALID_SOCKET;
	continue;
	}
	break;
	}

	//Free the resources returned by getaddrinfo 
	freeaddrinfo(result);
	//Checks for errors 
	if (currentClient.socket == INVALID_SOCKET) {
	cout << "Unable to connect to server!" << endl;
	WSACleanup();
	return 1;
	}

	//If successfully connected the following message will be printed in the console window "Successfully connected!"
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
				cout << "send() failed: " << WSAGetLastError() << endl;
				break;
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