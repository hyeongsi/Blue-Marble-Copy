#pragma once

#include <WinSock2.h>
#include <iostream>
#include <process.h>
#include <WS2tcpip.h>	// inet_ntop()
#include <queue>

#pragma comment(lib, "ws2_32")

using namespace std;

constexpr const int PORT = 4568;
constexpr const int MAX_PACKET_SIZE = 1024;

typedef void(*CALLBACK_FUNC_PACKET)(char*);

enum MessageCode
{
	SET_MATCHING_USER_PACKET = 100,
};

class MatchMakingServer
{
private:
	static MatchMakingServer* instance;

	queue<unsigned int> matchQueue;

	char sendPacket[MAX_PACKET_SIZE] = {};
	unsigned int packetLastIndex = 0;

	WSADATA wsaData;
	SOCKET serverSocket;
	SOCKET clientSocket;
	SOCKADDR_IN clientAddress = {};
	SOCKADDR_IN serverAddress = {};

	CALLBACK_FUNC_PACKET recvCBF = nullptr;

	MatchMakingServer();
	~MatchMakingServer();

	string GetClientIp(SOCKADDR_IN clientAddress);
	static UINT WINAPI RecvDataThread(void* arg);
	void StartRecvDataThread(SOCKET clientSocket);

	void InitServer();
	void AcceptSocket();
	void PushUserId(char * packet);
public:
	static MatchMakingServer* GetInstance();
	static void ReleaseInstance();

	void StartServer();

	void MakePacket(char header);
	template<class T>
	void AppendPacketData(T data, unsigned int dataSize);
	void AppendPacketPointerData(const char* data, unsigned int dataSize);
	void PacektSendMethod(SOCKET& socket);
};