#include <windows.h>
#include <winsock2.h>
//#define NDEBUG
 
#ifndef NDEBUG
	#define DEBUG_DESTRUCTOR cout << "Destructor." << endl;
#else
	#define DEBUG_DESTRUCTOR
#endif

class Server
{
private:
	SOCKET ListenSocket, ClientSocket;
	struct addrinfo *result, hints;
	WSADATA wsaData;
	int iResult;
public:
	Server(void);
	~Server(void);
  	void Bind(void);
  	void Listen(void);
	void Accept(void);
	friend void ReceiveData(Server *myServer);
  	HANDLE Receive(void);
  	friend void SendData(Server *myServer);
	HANDLE Send(void);
	void Disconnect(void);
};