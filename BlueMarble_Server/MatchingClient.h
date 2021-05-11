#pragma once
#include "TransferResource.h"

typedef struct MatchMakingPacket
{
	unsigned int user1Id;
	unsigned int user2Id;
}matchMakingPacket;

class MatchingClient
{
private:
	static MatchingClient* instance;

	char sendPacket[MAX_PACKET_SIZE] = {};
	unsigned int packetLastIndex = 0;

	WSADATA wsaData;
	SOCKET clientSocket;
	SOCKADDR_IN serverAddress = {};
	HANDLE recvThreadHandle = nullptr;

	CALLBACK_FUNC_PACKET recvCBF = nullptr;

	MatchingClient();
	~MatchingClient();

	void PrintMatchingServerError(int errorCode);
	void Init();
	bool Connect();

	static UINT WINAPI RecvDataThread(void* arg);
	void RecvDataMethod();

	static void SetMatchUserPacketMethod(char* packet);
	void SetMatchUser(char* packet);
public:
	static MatchingClient* GetInstance();
	static void ReleaseInstance();

	void ConnectMathchServer();

	void MakePacket(char header);
	void AppendPacketDataMethod(unsigned int data, unsigned int dataSize);
	void AppendPacketPointerData(char* data, unsigned int dataSize);
	void SendMessageToMatchServer();
};

