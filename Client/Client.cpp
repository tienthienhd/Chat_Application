#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <iostream>
#include <cstring>
#include <string>
#include "Graphic.h"
#include "Client.h"
#include "TextFormatter.h"

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"
#define NAME_CLIENT "Client 1"

using namespace std;

int g_PosOutput = 3;
int g_PosInput;
int g_Columns, g_Rows;

void DrawInputPos(void);
void MoveInputPos(void);
void PaintLineBackGround(int fromPos, int toPos);

int main(int argc, char **argv)
{
	SetConsoleTitle("Client");
	SetSizeWindow();

	CONSOLE_SCREEN_BUFFER_INFO csbi;

    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    g_Columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    g_Rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
	g_PosInput = g_Rows - 2;

	Client myClient;

	if (argc == 2) {
		myClient.GetIPServer(argv[1]);
	}

	myClient.Connect();
	cout << "Connecting is success" << endl;
	Sleep(1000);
	system("cls");
	char hostname[256];
	if (gethostname(hostname, 256)) {
		cout << "Get hostname failed" << endl;
	}
	else {
		int LenHostname = strlen(hostname);
		gotoxy((g_Columns - LenHostname) / 2, 0);
		textcolor(GREEN);
		cout << hostname << endl;
	}


	myClient.SendAndReceive();

	myClient.CloseConnect();
	return 0;
}

void DrawInputPos(void)
{
	gotoxy(0, g_PosInput);
	textcolor(WHITE);
	textbackground(BLACK);
	cout << "->";
	textbackground(WHITE);
	textcolor(BLACK);
	cout << string(g_Columns - 2, ' ');
	cout << string(g_Columns, ' ');
	gotoxy(2, g_PosInput);
}

void MoveInputPos(void)
{
	g_PosInput = g_PosOutput + 2;
	gotoxy(0, g_PosInput - 1);
	textbackground(BLACK);
	cout << string(g_Columns, ' ');
	DrawInputPos();
}

void PaintLineBackGround(int fromPos, int toPos)
{
	gotoxy(fromPos, g_PosOutput);
	cout << string(toPos - fromPos, ' ');
	gotoxy(fromPos, g_PosOutput);
}

Client::Client(void) :ConnectSocket(INVALID_SOCKET), result(NULL), ptr(NULL), IP_ADDRESS("localhost")
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

	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
		// Create a SOCKET for connecting to server
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

		if (ConnectSocket == INVALID_SOCKET) {
			cerr << "socket failed with error: " << WSAGetLastError();
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
		cerr << "Unable to connect to server!\n";
		this->~Client();
		exit(1);
	}
}

void SendData(Client *myClient)
{
	char sendbuf[DEFAULT_BUFLEN] = NAME_CLIENT;
	int numLine = 0;

	// Send an initial buffer
	myClient->iResult = send(myClient->ConnectSocket, sendbuf, (int)strlen(sendbuf) + 1, 0);

	if (myClient->iResult == SOCKET_ERROR) {
		cerr << "send failed with error: " << WSAGetLastError() << endl;
		myClient->~Client();
		exit(1);
	}

	do {
		// Send an initial buffer
		numLine = 0;
		if (g_PosOutput < g_Rows - 3) {
			DrawInputPos();
			FormatText(sendbuf, numLine);
			gotoxy(0, 0);//Can thiet de giu nguyen trang thai man hinh
		}
		else {
			MoveInputPos();
			FormatText(sendbuf, numLine);
			gotoxy(0, g_PosInput - g_Rows);//Can thiet de giu nguyen trang thai man hinh
		}

		if (sendbuf[0] == '\0')
		{
			continue;
		}

		// print data to screen
		textbackground(BLUE);
		textcolor(WHITE);
		g_PosOutput++;
		if (g_PosOutput >= g_Rows - 3)
		{
			MoveInputPos();
			textbackground(BLUE);
			textcolor(WHITE);
		}

		if (numLine == 1)
		{
			gotoxy(g_Columns - strlen(sendbuf), g_PosOutput);
			cout << sendbuf;
		}
		else
		{
			PaintLineBackGround(4, g_Columns);
			char *p = sendbuf;
			while (*p != '\0')
			{
				putchar(*p);
				if (*p == '\n')
				{
					g_PosOutput++;
					if (g_PosOutput >= g_Rows - 3)
					{
						MoveInputPos();
						textbackground(BLUE);
						textcolor(WHITE);
					}
					PaintLineBackGround(4, g_Columns);
				}
				p++;
			}
		}

		//Tao khoang trong giua cac tin nhan
		g_PosOutput++;
		if (g_PosOutput >= g_Rows - 3)
		{
			MoveInputPos();
			textbackground(BLUE);
			textcolor(WHITE);
		}

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

		if (myClient->iResult > 0) {
			textbackground(LIGHTGRAY);
			textcolor(BLACK);
			
			++g_PosOutput;

			if (g_PosOutput >= g_Rows - 3)
			{
				MoveInputPos();
				textbackground(LIGHTGRAY);
				textcolor(BLACK);
			}

			gotoxy(0, g_PosOutput);

			if(myClient->iResult < g_Columns - 4)
			{
				PaintLineBackGround(0, myClient->iResult);
			}
			else
			{
				PaintLineBackGround(0, g_Columns - 4);
			}
			
			char *p = recvbuf;
			while (*p != '\0')
			{
				putchar(*p);
				if (*p == '\n')
				{
					g_PosOutput++;
					if (g_PosOutput >= g_Rows - 3)
					{
						MoveInputPos();
						textbackground(LIGHTGRAY);
						textcolor(BLACK);
					}
					PaintLineBackGround(0, g_Columns - 4);
				}
				p++;
			}

			g_PosOutput = wherey();

			//Tao khoang trong giua cac tin nhan
			g_PosOutput++;
			if (g_PosOutput >= g_Rows - 3)
			{
				MoveInputPos();
				textbackground(LIGHTGRAY);
				textcolor(BLACK);
			}

			// back to edit
			if (g_PosOutput < g_Rows - 3) {
				DrawInputPos();
			}
			else {
				MoveInputPos();
			}
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
