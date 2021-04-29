#pragma once
#include <WinSock2.h>
#include <mutex>

#pragma comment(lib, "ws2_32")
using namespace std;

constexpr const int PORT = 4567;
constexpr const char* SERVER_IP = "192.168.123.101";

enum ErrorCode
{
	WSAStartupError = 200,
	ConnectError = 201,
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
};

