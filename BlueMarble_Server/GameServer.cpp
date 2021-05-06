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

	char cBuffer[MAX_PACKET_SIZE] = {};
	char header = NULL;
	unsigned int dataSize = 0;

	clientSocketMutex.lock();
	clientSocketList.emplace_back(clientSocket);
	clientSocketMutex.unlock();

	while ((recv(clientSocket, cBuffer, MAX_PACKET_SIZE, 0)) != -1)
	{
		memcpy(&header, &cBuffer[0], sizeof(char));
		cout << "recv " << (int)header << endl;

		switch (header)
		{
		case GET_MAPDATA:
			GetMapDataMethod(clientSocket);
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

void GameServer::GetMapDataMethod(SOCKET& socekt)
{
	boardData* board = MapManager::GetInstance()->GetBoardData(0);	// 나중에 enum 값으로 변경하기

	if (nullptr != board)
	{
		PacektSendMethod(socekt, GET_MAPDATA, sizeof(board->mapSize), board->mapSize);	// 맵 사이즈
		cout << "send mapSize " << endl;
		for (size_t i = 0; i < board->code.size(); i++)
		{
			PacektSendMethod(socekt, NULL, sizeof(int), board->code[i]);
		}
		cout << "send code " << endl;
		for (size_t i = 0; i < board->name.size(); i++)
		{
			PacektSendMethod(socekt, NULL, board->name[i].size(), board->name[i].c_str());
		}
		cout << "send name " << endl;
	}
	else
	{
		PrintErrorCode(NOT_FOUND_BOARDDATA_ERROR);
	}
}

template<class T>
void GameServer::PacektSendMethod(SOCKET& socekt, char header, unsigned int dataSize, T data)
{
	unsigned int packetSize = NULL;
	char* buf = nullptr;

	if (NULL == header)	// header 없이 전송
	{
		if (0 >= dataSize)
		{
			return;
		}

		packetSize = sizeof(unsigned int) + dataSize;	// datasize + data
		buf = new char[packetSize];

		memcpy(&buf[0], &dataSize, sizeof(unsigned int));	// datasize setting
		memcpy(&buf[sizeof(unsigned int)], &data, dataSize);
	}
	else   // header 포함 전송
	{
		packetSize = sizeof(char) + sizeof(unsigned int) + dataSize;	// header + datasize + data
		buf = new char[packetSize];

		buf[0] = header;	// header setting
		memcpy(&buf[1], &dataSize, sizeof(unsigned int));	// datasize setting
		memcpy(&buf[1 + sizeof(unsigned int)], &data, dataSize);
	}

	if (send(socekt, buf, packetSize, 0) == -1)
	{
		PrintErrorCode(SEND_ERROR);
	}

	delete[] buf;
}