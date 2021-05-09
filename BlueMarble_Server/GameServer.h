#pragma once

#include <string>
#include <List>
#include <mutex>
#include "TransferResource.h"
#include "MatchingClient.h"

class GameServer
{
private:
	static GameServer* instance;

	char sendPacket[MAX_PACKET_SIZE] = {};
	unsigned int packetLastIndex = 0;

	WSADATA wsaData;
	SOCKET serverSocket;
	SOCKADDR_IN clientAddress = {};
	SOCKADDR_IN serverAddress = {};

	list<SOCKET> clientSocketList;
	mutex clientSocketMutex;
	CALLBACK_FUNC_PACKET recvCBF = nullptr;

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
	void GetMapDataMethod(SOCKET& socekt);

	void MakePacket(char header);
	template<class T>
	void AppendPacketData(T data, unsigned int dataSize);
	void AppendPacketPointerData(const char* data, unsigned int dataSize);
	void PacektSendMethod(SOCKET& socket);
	void RegistRecvCallbackFunction(CALLBACK_FUNC_PACKET cbf);

	list<SOCKET> GetClientSocketList();
};

