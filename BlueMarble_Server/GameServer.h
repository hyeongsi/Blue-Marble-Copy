#pragma once

#include <string>
#include <List>
#include <mutex>
#include "MatchingClient.h"
#include "GameRoom.h"

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

	bool InitServer();
	void AcceptMethod();
	void StartRecvDataThread(SOCKET clientSocket);
	static UINT WINAPI RecvDataThread(void* arg);

	string GetClientIp(SOCKADDR_IN clientAddress);
	void GetReadySignMethod(SOCKET& socekt, GameRoom* myRoom);
public:
	static GameServer* GetInstance();
	static void ReleaseInstance();

	void PrintErrorCode(int errorCode);
	void StartServer();

	void MakePacket(char* sendPacket, unsigned int* packetLastIndex, char header);
	template<class T>
	void AppendPacketData(char* sendPacket, unsigned int* packetLastIndex, T data, unsigned int dataSize);
	void AppendPacketPointerData(char* sendPacket, unsigned int* packetLastIndex, const char* data, unsigned int dataSize);
	void PacektSendMethod(char* sendPacket, SOCKET& socket);

	list<SOCKET> GetClientSocketList();
};

template<class T>
void GameServer::AppendPacketData(char* sendPacket, unsigned int* packetLastIndex, T data, unsigned int dataSize)
{
	memcpy(&sendPacket[*packetLastIndex], &data, dataSize);
	*packetLastIndex += dataSize;
}
