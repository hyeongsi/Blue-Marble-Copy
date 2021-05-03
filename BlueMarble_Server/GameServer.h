#pragma once

#include <string>
#include <List>
#include <mutex>
#include "TransferResource.h"

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
	void GetMapDataMethod(SOCKET& socekt, customPacket& packet);
	void PacektSendMethod(SOCKET& socekt, customPacket& packet, void* data, int size);
};

