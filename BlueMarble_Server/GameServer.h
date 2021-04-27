#pragma once
#include <WinSock2.h>
#include <string>
#include <List>
#pragma comment(lib, "ws2_32")

constexpr const int PORT = 4567;
constexpr const int PACKET_SIZE = 1024;

enum ErrorCode
{
	WSAStartupError = 0,
	BindError = 1,
	ListenError = 2,
};

class GameServer
{
private:
	static GameServer* instance;

	WSADATA wsaData;
	SOCKET serverSocket;
	SOCKADDR_IN clientAddress = {};
	SOCKADDR_IN serverAddress = {};

	std::list<SOCKET> clientSocketList;
	mutex clientSocketMutex;

	GameServer();
	~GameServer();

	void PrintErrorCode(int errorCode);
	bool InitServer();
	void AcceptMethod();
	void StartListenThread(SOCKET clientSocket);
	static UINT WINAPI ListenThread(void* arg);

	std::string GetClientIp(SOCKADDR_IN clientAddress);
public:
	static GameServer* GetInstance();
	static void ReleaseInstance();

	void StartServer();
};

