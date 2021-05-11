#pragma once
#include "GameServer.h"
#include <iostream>
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
	cout << "error code : " << errorCode << endl;
}

bool GameServer::InitServer()
{
	if (0 != WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		PrintErrorCode(WSASTARTUP_ERROR);
		return false;
	}
	cout << "WSAStartup" << endl;
	serverSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(PORT);
	serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);

	if (-1 == bind(serverSocket, (SOCKADDR*)&serverAddress, sizeof(serverAddress)))
	{
		PrintErrorCode(BIND_ERROR);
		return false;
	}
	cout << "bind" << endl;

	if (-1 == listen(serverSocket, SOMAXCONN))
	{
		PrintErrorCode(LISTEN_ERROR);
		return false;
	}
	cout << "listen" << endl;
	
	return true;
}

void GameServer::AcceptMethod()
{
	int clientAddressSize = sizeof(instance->clientAddress);
	cout << "start Accept" << endl;

	while (true)
	{
		SOCKET clientSocket;
		clientSocket = accept(serverSocket, (SOCKADDR*)&clientAddress, &clientAddressSize);
		cout << "Connect Ip : " << GetClientIp(clientAddress) << endl;
		cout << "Working AcceptThread" << endl << endl;;

		_beginthreadex(NULL, 0, RecvDataThread, &clientSocket, 0, NULL);	// recv thread 실행
	}
}

void GameServer::StartRecvDataThread(SOCKET clientSocket)
{
	cout << "start ListenThread" << endl;
	CALLBACK_FUNC_PACKET recvCBF = nullptr;

	char cBuffer[MAX_PACKET_SIZE] = {};
	char header = NULL;
	GameRoom* myRoom = nullptr;

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
			cout << "recv " << (int)header << endl;

			switch (header)
			{
			case READY:
				GetReadySignMethod(clientSocket, myRoom);
				break;
			default:
				break;
			}
		}
	}

	clientSocketMutex.lock();
	clientSocketList.remove(clientSocket);
	clientSocketMutex.unlock();

	cout << "lost connect : " << GetClientIp(clientAddress) << endl;
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

void GameServer::GetReadySignMethod(SOCKET& socekt, GameRoom* myRoom)
{
	readyPacket rPacket;
	char cBuffer[MAX_PACKET_SIZE] = {};

	memcpy(&rPacket.roomIndex, &cBuffer[1], sizeof(int));
	myRoom = GameManager::GetInstance()->GetRoom(rPacket.roomIndex);	// 방 수신 후 
	myRoom->connectPlayer++;

	if (myRoom->connectPlayer >= MAX_PLAYER)
	{
		_beginthreadex(NULL, 0, GameManager::GetInstance()->RoomLogicThread, myRoom, 0, NULL);	// recv thread 실행
	}
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
	cout << "send Data" << endl;
}