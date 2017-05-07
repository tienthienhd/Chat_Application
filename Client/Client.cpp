#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <iostream>
#include <cstring>
#include "Client.h"

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"
#define NAME_CLIENT "Client 1"

using namespace std;

int main(int argc, char **argv)
{
	Client myClient;
	
	if (argc == 2) {
		myClient.GetIPServer(argv[1]);
	}

	myClient.Connect();

	system("cls");
	cout<< "Connecting is success"<< endl;

	myClient.SendAndReceive();
	cout << "Connection closing...\n";

	myClient.CloseConnect();
	return 0;
}

Client::Client(void):ConnectSocket(INVALID_SOCKET), result(NULL), ptr(NULL), IP_ADDRESS("localhost")
{
	DEBUG_CONSTRUCTOR
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (iResult != 0) {
		cerr << "WSAStartup failed with error: " << iResult << endl;
		this->~Client();
		exit(1);
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
}

Client::~Client(void)
{
	DEBUG_DESTRUCTOR
	closesocket(ConnectSocket);
	WSACleanup();
}

void Client::GetIPServer(char* IP_ADDRESS)
{
	this->IP_ADDRESS = IP_ADDRESS;
}

void Client::Connect() {
	// Resolve the server address and port
	iResult = getaddrinfo(IP_ADDRESS, DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		cerr << "getaddrinfo failed with error: " << iResult << endl;
		this->~Client();
		exit(1);
	}

	for (ptr = result; ptr != NULL;ptr = ptr->ai_next) {
		// Create a SOCKET for connecting to server
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

		if (ConnectSocket == INVALID_SOCKET) {
			cerr<<"socket failed with error: "<< WSAGetLastError();
			this->~Client();
			exit(1);
		}

		// Connect to server.
		iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}

	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET) {
		cerr<< "Unable to connect to server!\n";
		this->~Client();
		exit(1);
	}
}

void SendData(Client *myClient)
{
	char sendbuf[DEFAULT_BUFLEN] = NAME_CLIENT;

	myClient->iResult = send(myClient->ConnectSocket, sendbuf, (int)strlen(sendbuf) + 1, 0);

	if (myClient->iResult == SOCKET_ERROR) {
		cerr << "send failed with error: " << WSAGetLastError() << endl;
		myClient->~Client();
		exit(1);
	}

	do {
		// Send an initial buffer
		cin.getline(sendbuf, DEFAULT_BUFLEN);
		
		myClient->iResult = send(myClient->ConnectSocket, sendbuf, (int)strlen(sendbuf) + 1, 0);
		if (myClient->iResult == SOCKET_ERROR) {
			cerr << "send failed with error: " << WSAGetLastError() << endl;
			myClient->~Client();
			exit(1);
		}

		if (strcmp(sendbuf, "###") == 0)
		{
			break;
		}
	} while (1);
}

void Client::SendAndReceive(void)
{
	HANDLE h[2];
	DWORD id;

	h[0] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)SendData, this, FALSE, &id);
	h[1] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ReceiveData, this, FALSE, &id);

	WaitForMultipleObjects(2, h, false, INFINITE);

	CloseHandle(h[0]);
	CloseHandle(h[1]);
}

void ReceiveData(Client *myClient)
{
	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;

	// Receive until the peer closes the connection
	do {
		myClient->iResult = recv(myClient->ConnectSocket, recvbuf, recvbuflen, 0);

		if (strcmp(recvbuf, "###") == 0)
		{
			break;
		}

		if (myClient->iResult > 0)
		{
			cout << "->\t\t" << recvbuf << endl;
		}	
		else if (myClient->iResult < 0)
		{
			cerr << "recv failed with error: " << WSAGetLastError() << endl;
			myClient->~Client();
			exit(1);
		}
	} while (myClient->iResult > 0);
}

void Client::CloseConnect(void)
{
	// shutdown the connection since no more data will be sent
	iResult = shutdown(ConnectSocket, SD_SEND);

	if (iResult == SOCKET_ERROR) {
		cerr << "shutdown failed with error: " << WSAGetLastError() << endl;
		this->~Client();
		exit(1);
	}
}