#ifndef CLIENT_H
#define CLIENT_H

//#define NDEBUG

#ifndef NDEBUG
#define DEBUG_DESTRUCTOR cout << "Destructor." << endl;
#define DEBUG_CONSTRUCTOR cout << "Constructor." << endl;
#else
#define DEBUG_DESTRUCTOR
#define DEBUG_CONSTRUCTOR
#endif

class Client
{
private:
	SOCKET ConnectSocket;
	struct addrinfo *result, *ptr, hints;
	WSADATA wsaData;
	int iResult;
	char* IP_ADDRESS;
	FILE *log;
public:
	Client(void);
	~Client(void);
	void GetIPServer(char* IP_ADDRESS);
	void Connect(void);

	friend void SendData(Client *myClient);
	friend void ReceiveData(Client *myClient);
	void SendAndReceive(void);

	void CloseConnect(void);
};

#endif // !CLIENT_H