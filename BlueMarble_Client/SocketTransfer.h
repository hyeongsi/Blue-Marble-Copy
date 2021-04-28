#pragma once
#include <WinSock2.h>
#include "MainWindow.h"

#pragma comment(lib, "ws2_32")

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

	SocketTransfer();
	~SocketTransfer();

public:
	void PrintErrorCode(const int errorCode);


	static SocketTransfer* GetInstance();
	static void ReleaseInstance();

	bool ConnectServer();
};

