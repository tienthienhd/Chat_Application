#include <windows.h>
#include <winsock2.h>
#define NDEBUG
 
#ifndef NDEBUG
	#define DEBUG_DESTRUCTOR cout << "Destructor." << endl;
	#define DEBUG_CONSTRUCTOR cout << "Constructor." << endl;
#else
	#define DEBUG_DESTRUCTOR
	#define DEBUG_CONSTRUCTOR
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
	friend void SendData(Server *myServer);
  	void SendAndReceive(void);

	void CloseConnect(void);
};