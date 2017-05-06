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
	if (argc != 2) {
		argv[1] = "localhost";
	}
	
	Client myClient(argv[1]);

	myClient.Connect();

	system("cls");
	cout<< "Connecting is success"<< endl;

	HANDLE h[2];
	h[0] = myClient.Send();
	h[1] = myClient.Receive();

	WaitForMultipleObjects(2, h, false, INFINITE);

	CloseHandle(h[0]);
	CloseHandle(h[1]);

	myClient.CloseConnect();
	return 0;
}

Client::Client(const char* IP_ADDRESS):ConnectSocket(INVALID_SOCKET), result(NULL), ptr(NULL)
{
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


	// Attempt to connect to the first address returned by
	// the call to getaddrinfo
	ptr = result;

	// Resolve the server address and port
	iResult = getaddrinfo(IP_ADDRESS, DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		cerr<< "getaddrinfo failed with error: "<< iResult<< endl;
		this->~Client();
		exit(1);
	}
}

Client::~Client(void)
{
	closesocket(ConnectSocket);
	WSACleanup();
}

void Client::Connect() {
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
			ConnectSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}

	freeaddrinfo(result);
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

HANDLE Client::Send(void)
{
	HANDLE h;
	DWORD id;

	h = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)SendData, this, FALSE, &id);

	return h;
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
		else if (myClient->iResult == 0)
		{
			cout << "Connection closed\n";
		}	
		else
		{
			cerr << "recv failed with error: " << WSAGetLastError() << endl;
			myClient->~Client();
			exit(1);
		}
	} while (myClient->iResult > 0);
}

HANDLE Client::Receive(void)
{
	HANDLE h;
	DWORD id;

	h = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ReceiveData, this, FALSE, &id);

	return h;
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