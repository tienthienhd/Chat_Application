#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <cstring>
#include "Server.h"

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")

using namespace std;

int main()
{
	Server myServer;

	myServer.Bind();
	myServer.Listen();
	cout << "Start server success" << endl;
	cout << "waiting for connecting..." << endl;
	myServer.Accept();

	HANDLE h[2];
	h[0] = myServer.Send();
	h[1] = myServer.Receive();

	WaitForMultipleObjects(2, h, false, INFINITE);

	CloseHandle(h[0]);
	CloseHandle(h[1]);

	myServer.Disconnect();

	return 0;
}

Server::Server(void) :ListenSocket(INVALID_SOCKET), ClientSocket(NULL), result(NULL)
{
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (iResult != 0) {
		cerr<<"WSAStartup failed with error: "<< iResult<< endl;
		this->~Server();
		exit(1) ;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the local address and port to be used by the server
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		cerr<<"getaddrinfo failed: "<<iResult<<endl;
		this->~Server();
		exit(1) ;
	}

	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

	if (ListenSocket == INVALID_SOCKET) {
		cerr << "Error at socket(): " << WSAGetLastError() << endl;
		freeaddrinfo(result);
		this->~Server();
		exit(1) ;
	}
}

Server::~Server(void)
{
	DEBUG_DESTRUCTOR
	WSACleanup();
}

void Server::Bind(void)
{
	// Setup the TCP listening socket
	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);

	if (iResult == SOCKET_ERROR) {
		cerr << "bind failed with error: " << WSAGetLastError() << endl;
		freeaddrinfo(result);
		closesocket(ListenSocket);
		exit(1) ;
	}

	freeaddrinfo(result);
}

void Server::Listen(void)
{
	if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) {
		cerr<<"Listen failed with error: "<< WSAGetLastError()<<endl;
		closesocket(ListenSocket);
		this->~Server();
		exit(1) ;
	}
}

void Server::Accept(void)
{
	// Accept a client socket
	ClientSocket = accept(ListenSocket, NULL, NULL);
	if (ClientSocket == INVALID_SOCKET) {
		cerr<<"accept failed: "<< WSAGetLastError();
		closesocket(ListenSocket);
		this->~Server();
		exit(1) ;
	}

	closesocket(ListenSocket);
}


void SendData(Server *myServer)
{
	char sendbuf[DEFAULT_BUFLEN];
	do {
		// Send an initial buffer
		cin.getline(sendbuf, DEFAULT_BUFLEN);
		
		myServer->iResult = send(myServer->ClientSocket, sendbuf, (int)strlen(sendbuf) + 1, 0);
		if (myServer->iResult == SOCKET_ERROR) {
			cerr << "send failed with error: " << WSAGetLastError() << endl;
			exit(1);
		}

		if (strcmp(sendbuf, "###") == 0)
		{
			break;
		}
	} while (1);
}

HANDLE Server::Send(void)
{
	HANDLE h;
	DWORD id;

	h = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)SendData, this, FALSE, &id);

	return h;
}

void ReceiveData(Server *myServer)
{
	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;

	myServer->iResult = recv(myServer->ClientSocket, recvbuf, recvbuflen, 0);
	
	if (myServer->iResult > 0) {
		cout << recvbuf << " is connected."<< endl;
	}
	else
	{
		cerr << "recv failed: " << WSAGetLastError() << endl;
		closesocket(myServer->ClientSocket);
		myServer->~Server();
		exit(1);
	}
	// Receive until the peer shuts down the connection
	do {
		myServer->iResult = recv(myServer->ClientSocket, recvbuf, recvbuflen, 0);

		if (strcmp(recvbuf, "###") == 0)
		{
			break;
		}

		if (myServer->iResult > 0) {
			cout<<"->\t\t"<<recvbuf<<endl;
		}
		else if (myServer->iResult == 0)
			cout<< "Connection closing..."<< endl;
		else {
			cerr<< "recv failed: "<< WSAGetLastError()<< endl;
			closesocket(myServer->ClientSocket);
			myServer->~Server();
			exit(1) ;
		}
	} while (myServer->iResult > 0);
}

HANDLE Server::Receive(void)
{
	HANDLE h;
	DWORD id;

	h = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ReceiveData, this, FALSE, &id);

	return h;
}

void Server::Disconnect(void)
{
	// shutdown the send half of the connection since no more data will be sent
	iResult = shutdown(ClientSocket, SD_SEND);

	if (iResult == SOCKET_ERROR) {
		cerr<< "shutdown failed: "<< WSAGetLastError()<<endl;
		closesocket(ClientSocket);
		this->~Server();
		exit(1) ;
	}

	closesocket(ClientSocket);
}
