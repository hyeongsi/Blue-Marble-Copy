#pragma once
#include "GameServer.h"
#include <stdio.h>
#include <process.h>
#include <signal.h>
#include <WS2tcpip.h>	// inet_ntop()
#include "MapManager.h"
#include "GameManager.h"

GameServer* GameServer::instance = nullptr;

GameServer::GameServer() {}
GameServer::~GameServer() {}

void GameServer::PrintErrorCode(int errorCode)
{
	printf("%s %d\n", "error code : ", errorCode);
}

bool GameServer::InitServer()
{
	if (0 != WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		PrintErrorCode(WSASTARTUP_ERROR);
		return false;
	}
	printf("%s\n", "WSAStartup");
	serverSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(PORT);
	serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);

	if (-1 == bind(serverSocket, (SOCKADDR*)&serverAddress, sizeof(serverAddress)))
	{
		PrintErrorCode(BIND_ERROR);
		return false;
	}
	printf("%s\n", "bind");

	if (-1 == listen(serverSocket, SOMAXCONN))
	{
		PrintErrorCode(LISTEN_ERROR);
		return false;
	}
	printf("%s\n", "listen");
	
	return true;
}

void GameServer::AcceptMethod()
{
	int clientAddressSize = sizeof(instance->clientAddress);
	printf("%s\n", "start Accept");

	while (true)
	{
		SOCKET clientSocket;
		clientSocket = accept(serverSocket, (SOCKADDR*)&clientAddress, &clientAddressSize);
		printf("%s %s\n", "Connect Ip : ", GetClientIp(clientAddress).c_str());
		printf("%s\n", "Working AcceptThread");

		_beginthreadex(NULL, 0, RecvDataThread, &clientSocket, 0, NULL);	// recv thread 실행
	}
}

void GameServer::StartRecvDataThread(SOCKET clientSocket)
{
	printf("%s\n", "start ListenThread");
	CALLBACK_FUNC_PACKET recvCBF = nullptr;

	char cBuffer[MAX_PACKET_SIZE] = {};
	char header = NULL;
	GameRoom* myRoom = nullptr;
	int tempRoomIndex = -1;

	clientSocketMutex.lock();
	clientSocketList.emplace_back(clientSocket);
	clientSocketMutex.unlock();

	MatchingClient::GetInstance()->MakePacket(SET_MATCHING_USER_PACKET);
	MatchingClient::GetInstance()->AppendPacketDataMethod(clientSocket, sizeof(unsigned int));
	MatchingClient::GetInstance()->SendMessageToMatchServer();

	while ((recv(clientSocket, cBuffer, MAX_PACKET_SIZE, 0)) != -1)
	{
		if (nullptr != recvCBF)
		{
			recvCBF(cBuffer);
		}
		else
		{
			memcpy(&header, &cBuffer[0], sizeof(char));
			printf("%s %d\n", "recv", (int)header);

			switch (header)
			{
			case READY:
				tempRoomIndex = GetReadySignMethod(myRoom, clientSocket);
				if (tempRoomIndex != -1)
				{
					myRoom = GameManager::GetInstance()->GetRoom(tempRoomIndex);
				}
				break;
			case ROLL_DICE_SIGN:
				GameManager::GetInstance()->RollTheDiceMethod(myRoom);
				break;
			case BUY_LAND_SIGN:
				GameManager::GetInstance()->BuyLandMethod(myRoom, cBuffer);
				break;
			case BUY_BUILDING_SIGN:
				GameManager::GetInstance()->BuyBuildingMethod(myRoom, cBuffer);
				break;
			case PAY_TOLL_SIGN:
				GameManager::GetInstance()->PayTollMethod(myRoom, cBuffer);
				break;
			case TAKE_OVER_SIGN:
				GameManager::GetInstance()->TakeOverMethod(myRoom, cBuffer);
				break;
			case BUY_LANDMARK_SIGN:
				GameManager::GetInstance()->BuyLandMarkMethod(myRoom, cBuffer);
				break;
			case REVENUE_SIGN:
				GameManager::GetInstance()->RevenueSignMethod(myRoom);
				break;
			case BUY_LAND_SYNC:
				myRoom->CheckLandKindNSendMessage();
				break;
			case BUY_BUILDING_SYNC:
				if (myRoom != nullptr)
				{
					myRoom->EndTurn();
				}
				break;
			case PAY_TOLL_SIGN_SYNC:
				myRoom->CheckPassNSellMessage();
				break;
			case TAKE_OVER_SYNC:
				myRoom->CheckCanBuild();
				break;
			case BUY_LANDMARK_SIGN_SYNC:
				myRoom->NextTurn();
				break;
			case SELL_LAND_SIGN_SYNC:
				GameManager::GetInstance()->AfterSellLandSyncMethod(myRoom);
				break;
			case REVENUE_SIGN_SYNC:
				myRoom->NextTurn();
				break;
			case FINISH_THIS_TURN_PROCESS:
				myRoom->CheckEndProcess(clientSocket);
				break;
			case SEND_SELECT_MODE_INPUT_KEY:
				GameManager::GetInstance()->SelectInputKeyProcessMethod(myRoom, cBuffer);
				break;
			case SELECT_MODE_BTN:
				GameManager::GetInstance()->SellLandProcessMethod(myRoom, cBuffer);
				break;
			default:
				break;
			}
		}
	}

	clientSocketMutex.lock();
	clientSocketList.remove(clientSocket);
	clientSocketMutex.unlock();

	printf("%s %s\n", "lost connect : ", GetClientIp(clientAddress).c_str());
}

UINT WINAPI GameServer::RecvDataThread(void* arg)
{
	instance->StartRecvDataThread(*(SOCKET*)arg);
	return 0;
}

string GameServer::GetClientIp(SOCKADDR_IN clientAddress)
{
	char buf[32] = { 0, };
	return inet_ntop(AF_INET, &clientAddress.sin_addr, buf, sizeof(buf));
}

int GameServer::GetReadySignMethod(GameRoom* myRoom, SOCKET& socket)
{
	int roomIndex = 0;

	roomIndex = GameManager::GetInstance()->FindBelongRoom(socket);
	if (roomIndex == -1)
	{
		return -1;
	}
	myRoom = GameManager::GetInstance()->GetRoom(roomIndex);	// 방 수신 후 
	myRoom->connectPlayer++;

	if (myRoom->connectPlayer >= MAX_PLAYER)
	{
		_beginthreadex(NULL, 0, GameManager::GetInstance()->RoomLogicThread, myRoom, 0, NULL);	// recv thread 실행
	}

	return roomIndex;
}

GameServer* GameServer::GetInstance()
{
	if (nullptr == instance)
	{
		instance = new GameServer();
	}

	return instance;
}

void GameServer::ReleaseInstance()
{
	delete instance;
	instance = nullptr;
}

void GameServer::StartServer()
{
	GetInstance();
	MapManager::GetInstance()->LoadMapData();
	MatchingClient::GetInstance()->ConnectMathchServer();	// 매칭서버 연결

	if (!InitServer())
		return;

	AcceptMethod();

	MapManager::ReleaseInstance();
	ReleaseInstance();
}

list<SOCKET> GameServer::GetClientSocketList()
{
	return clientSocketList;
}

void GameServer::MakePacket(char* sendPacket, unsigned int* packetLastIndex, char header)
{
	if (NULL != header)
	{
		memset(sendPacket, 0, MAX_PACKET_SIZE);		// 패킷 초기화
		sendPacket[0] = header;	// header setting
		*packetLastIndex = 1;
	}
	else
	{
		memset(sendPacket, 0, MAX_PACKET_SIZE);		// 패킷 초기화
		*packetLastIndex = 0;
	}
}

void GameServer::AppendPacketPointerData(char* sendPacket, unsigned int* packetLastIndex, const char* data, unsigned int dataSize)
{
	memcpy(&sendPacket[*packetLastIndex], data, dataSize);
	*packetLastIndex += dataSize;
}

void GameServer::PacektSendMethod(char* sendPacket, SOCKET& socket)
{
	if (send(socket, sendPacket, MAX_PACKET_SIZE, 0) == -1)
	{
		PrintErrorCode(SEND_ERROR);
	}
	printf("%s\n", "send Data");
}