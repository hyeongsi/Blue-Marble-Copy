#pragma once
#include <WinSock2.h>
#include <mutex>

#pragma comment(lib, "ws2_32")
using namespace std;

constexpr const int PORT = 4567;
constexpr const char* SERVER_IP = "192.168.123.101";

typedef struct customPackets
{
	int header = -1;
	int dataSize = 0;
	char* data = nullptr;
	customPackets() {};
	customPackets(int _header, int _dataSize, char* _data) :
		header(_header), dataSize(_dataSize), data(_data) {};
} customPacket;

enum ErrorCode
{
	WSAStartupError = 200,
	ConnectError = 201,
	RecvError = 202,
	SendError = 203,
};

class SocketTransfer
{
private:
	static SocketTransfer* instance;

	WSADATA wsaData;
	SOCKET clientSocket;
	SOCKADDR_IN serverAddress = {};
	HANDLE recvThreadHandle = nullptr;
	
	mutex recvThreadMutex;

	SocketTransfer();
	~SocketTransfer();

	void RecvDataMethod(SOCKET clientSocket);
	static UINT WINAPI RecvDataThread(void* arg);
public:
	void PrintErrorCode(const int errorCode);

	static SocketTransfer* GetInstance();
	static void ReleaseInstance();

	bool ConnectServer();
	void StartRecvDataThread();
	void TerminateRecvDataThread();

	void SendMessageToGameServer(int header, int dataSize, char* data);
};