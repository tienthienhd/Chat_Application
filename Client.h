

class Client
{
private:
	SOCKET ConnectSocket;
	struct addrinfo *result, *ptr, hints;
	WSADATA wsaData;
	int iResult;
public:
	Client(const char* IP_ADDRESS);
	~Client(void);
	void Connect(void);
	friend void SendData(Client *myClient);
	HANDLE Send(void);
	friend void ReceiveData(Client *myClient);
	HANDLE Receive(void);
	void CloseConnect(void);
};