#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "Server.h"
#include "Graphic.h"
#include "TextFormatter.h"

#define DEFAULT_PORT "27015"

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")

using namespace std;

extern int g_PosOutput;
extern int g_PosInput;
extern int g_Columns, g_Rows;

int main()
{
	SetConsoleTitle("Server");

	SetSizeWindow();
	GetSizeWindow();

	Server myServer;

	myServer.Bind();
	myServer.Listen();
	WaittingScreen(myServer);
	myServer.SendAndReceive();
	myServer.CloseConnect();

	return 0;
}

void WaittingScreen(Server &myServer)
{
	// display hostname
	TextColor(GREEN);
	cout << "Start server success" << endl;
	hostent *MyPC;
	char hostname[256];
	gethostname(hostname, 256);
	MyPC = gethostbyname(hostname);
	cout << "Name of my PC is: " << MyPC->h_name << endl;
	cout << "IP address of my PC is: \n" << inet_ntoa(*(in_addr*)MyPC->h_addr_list[0]) << endl;
	cout << "Waiting for connecting..." << endl;

	myServer.Accept();

	// display
	system("cls");
	int LenHostname = strlen(hostname);
	Gotoxy((g_Columns - LenHostname)/2, 0);
	cout << hostname << endl;
	TextColor(WHITE);
}

Server::Server(void) :ListenSocket(INVALID_SOCKET), ClientSocket(NULL), result(NULL)
{
	DEBUG_CONSTRUCTOR
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
		this->~Server();
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
	int line = 5;
	char sendbuf[DEFAULT_BUFLEN];
	int numLine = 0;//Dem so dong cua doan van ban gui di
	do {
		// Send an initial buffer
		InputSendMessage(numLine, sendbuf);

		if (sendbuf[0] == '\0') {
			continue;
		}
		// print data to screen
		PrintSendMessage(numLine, sendbuf);
		
		myServer->iResult = send(myServer->ClientSocket, sendbuf, (int)strlen(sendbuf) + 1, 0);
		if (myServer->iResult == SOCKET_ERROR) {
			cerr << "send failed with error: " << WSAGetLastError() << endl;
			exit(1);
		}

		if (strcmp(sendbuf, "###") == 0) {
			break;
		}
	} while (1);
}

void ReceiveData(Server *myServer)
{
	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;

	myServer->iResult = recv(myServer->ClientSocket, recvbuf, recvbuflen, 0);

	if (myServer->iResult > 0) {
	}
	else {
		cerr << "recv failed: " << WSAGetLastError() << endl;
		closesocket(myServer->ClientSocket);
		myServer->~Server();
		exit(1);
	}
	// Receive until the peer shuts down the connection
	do {
		myServer->iResult = recv(myServer->ClientSocket, recvbuf, recvbuflen, 0);

		if (strcmp(recvbuf, "###") == 0) {
			break;
		}

		if (myServer->iResult > 0) {
			PrintReceiveMessage(myServer->iResult, recvbuf);

			// back to edit
			BackToInputBox();
		}
		else if (myServer->iResult < 0) {
			cerr<< "recv failed: "<< WSAGetLastError()<< endl;
			closesocket(myServer->ClientSocket);
			myServer->~Server();
			exit(1) ;
		}
	} while (myServer->iResult > 0);
}

void Server::SendAndReceive(void)
{
	HANDLE h[2];
	DWORD id;

	h[0] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)SendData, this, FALSE, &id);
	h[1] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ReceiveData, this, FALSE, &id);

	WaitForMultipleObjects(2, h, false, INFINITE);

	CloseHandle(h[0]);
	CloseHandle(h[1]);
}

void Server::CloseConnect(void)
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
