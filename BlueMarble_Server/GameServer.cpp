#include "GameServer.h"
#include <iostream>
#include <process.h>
#include <signal.h>
#include <WS2tcpip.h>	// inet_ntop()
#include "GameManager.h"
#include "MapManager.h"

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

	char cBuffer[PACKET_SIZE] = {};
	customPacket packet;

	clientSocketMutex.lock();
	clientSocketList.emplace_back(clientSocket);
	clientSocketMutex.unlock();

	while ((recv(clientSocket, cBuffer, PACKET_SIZE, 0)) != -1)
	{
		packet = *(customPacket*)cBuffer;

		switch (packet.header)	
		{
		case GET_MAPDATA:
			GetMapDataMethod(clientSocket, packet);
			break;
		default:
			break;
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

	if (!InitServer())
		return;

	AcceptMethod();

	MapManager::ReleaseInstance();
	ReleaseInstance();
}

void GameServer::GetMapDataMethod(SOCKET& socekt, customPacket& packet)
{
	cout << "recv " << packet.header << endl;

	customPacket sendPacket = packet;
	boardData* board = MapManager::GetInstance()->GetBoardData(0);	// 나중에 enum 값으로 변경하기

	if (nullptr != board)
	{
		PacektSendMethod(socekt, sendPacket, (char*)board->mapSize, sizeof(board->mapSize));
		PacektSendMethod(socekt, sendPacket, (char*)board->code, sizeof(int)* board->mapSize* DIRECTION);
		PacektSendMethod(socekt, sendPacket, (char*)board->name, sizeof(char)* NAME_SIZE* board->mapSize* DIRECTION);
	}
	else
	{
		PrintErrorCode(NOT_FOUND_BOARDDATA_ERROR);
	}
}

void GameServer::PacektSendMethod(SOCKET& socekt, customPacket& packet, char* data, int size)
{
	memcpy(&packet.data, &data, size);
	packet.dataSize = size;

	if (send(socekt, (char*)&packet, PACKET_SIZE, 0) == -1)
	{
		PrintErrorCode(SEND_ERROR);
	}
	else
	{
		cout << "send " << packet.header << endl;
	}
}
