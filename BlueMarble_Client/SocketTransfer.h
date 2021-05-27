#pragma once
#include <WinSock2.h>
#include <mutex>
#include "commonResource.h"
#include "PacketResource.h"
#include "RenderManager.h"

#pragma comment(lib, "ws2_32")

class SocketTransfer
{
private:
	static SocketTransfer* instance;

	char sendPacket[MAX_PACKET_SIZE] = {};
	unsigned int packetLastIndex = 0;

	WSADATA wsaData;
	SOCKET clientSocket;
	SOCKADDR_IN serverAddress = {};
	HANDLE recvThreadHandle = nullptr;
	
	mutex recvThreadMutex;
	CALLBACK_FUNC_PACKET recvCBF = nullptr;

	SocketTransfer();
	~SocketTransfer();

	void RecvDataMethod(SOCKET clientSocket);
	static UINT WINAPI RecvDataThread(void* arg);

	static void GetMapDataMethod1(char* packet);
	void GetMapData1(char* packet);
	static void GetMapDataMethod2(char* packet);
	void GetMapData2(char* packet);
	void GetReadyMethod(char* packet);

	static void GetRollDiceSignMethod(char* packet);
	void GetRollDiceSign(char* packet);

	static void GetRollDiceMethod(char* packet);
	void GetRollDice(char* packet);

	static void BuyLandSignMethod(char* packet);
	void BuyLandSign(char* packet);

	static void BuyBuildingMethod(char* packet);
	void BuyBuilding(char* packet);

	static void PayTollSignMethod(char* packet);
	void PayTollSign(char* packet);

	static void TakeOverSignMethod(char* packet);
	void TakeOverSign(char* packet);

	static void BuyLandMarkSignMethod(char* packet);
	void BuyLandMarkSign(char* packet);

	static void GetBuyLandSyncMethod(char* packet);
	void GetBuyLandSync(char* packet);

	static void GetBuyBuildSyncMethod(char* packet);
	void GetBuyBuildSync(char* packet);

	static void GetPayTollSignSyncMethod(char* packet);
	void GetPayTollSignSync(char* packet);

	static void GetTakeOverSyncMethod(char* packet);
	void GetTakeOverSync(char* packet);

	static void GetBuyLandMarkSyncMethod(char* packet);
	void GetBuyLandMarkSync(char* packet);

	static void SendNextTurnSignMethod();
	void SendNextTurnSign();

public:
	void PrintErrorCode(State state, const int errorCode);

	static SocketTransfer* GetInstance();
	static void ReleaseInstance();

	bool ConnectServer();
	void StartRecvDataThread();
	void TerminateRecvDataThread();

	void MakePacket(char header);
	template<class T>
	void AppendPacketData(T data, unsigned int dataSize);
	void AppendPacketPointerData(char* data, unsigned int dataSize);
	void SendMessageToGameServer();
	void RegistRecvCallbackFunction(CALLBACK_FUNC_PACKET cbf);

	void SendRollDiceSign();
};

template<class T>
void SocketTransfer::AppendPacketData(T data, unsigned int dataSize)
{
	memcpy(&sendPacket[packetLastIndex], &data, dataSize);
	packetLastIndex += dataSize;
}