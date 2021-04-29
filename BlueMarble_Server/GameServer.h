#pragma once
#include <WinSock2.h>
#include <string>
#include <List>
#include <mutex>
#pragma comment(lib, "ws2_32")

using namespace std;

constexpr const int PORT = 4567;
constexpr const int PACKET_SIZE = 1024;

enum ErrorCode
{
	WSAStartupError = 100,
	BindError = 101,
	ListenError = 102,
};

class GameServer
{
private:
	static GameServer* instance;

	WSADATA wsaData;
	SOCKET serverSocket;
	SOCKADDR_IN clientAddress = {};
	SOCKADDR_IN serverAddress = {};

	list<SOCKET> clientSocketList;
	mutex clientSocketMutex;

	GameServer();
	~GameServer();

	void PrintErrorCode(int errorCode);
	bool InitServer();
	void AcceptMethod();
	void StartRecvDataThread(SOCKET clientSocket);
	static UINT WINAPI RecvDataThread(void* arg);

	string GetClientIp(SOCKADDR_IN clientAddress);
public:
	static GameServer* GetInstance();
	static void ReleaseInstance();

	void StartServer();
};

