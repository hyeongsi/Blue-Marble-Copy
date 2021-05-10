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

	SendMapDataMethod(socekt);
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

void GameServer::SendMapDataMethod(SOCKET& socekt)
{
	boardData* board = MapManager::GetInstance()->GetBoardData(ORIGINAL);	// 나중에 enum 값으로 변경하기
	char sendPacket[MAX_PACKET_SIZE] = {};
	unsigned int packetLastIndex = 0;

	if (nullptr != board)
	{
		MakePacket(sendPacket, &packetLastIndex, GET_MAPDATA);
		AppendPacketData(sendPacket, &packetLastIndex,board->mapSize, sizeof(board->mapSize));
		for (int i = 0; i < (int)board->code.size(); i++)
		{
			AppendPacketData(sendPacket, &packetLastIndex,  board->code[i], sizeof(board->code[i]));
		}
		PacektSendMethod(sendPacket, socekt);
		cout << "send MapData1" << endl;

		MakePacket(sendPacket, &packetLastIndex, NULL);
		for (int i = 0; i < (int)board->code.size(); i++)
		{
			AppendPacketData(sendPacket, &packetLastIndex, board->name[i].size()+1, sizeof(unsigned int));
			AppendPacketPointerData(sendPacket, &packetLastIndex, board->name[i].c_str(), board->name[i].size());
			AppendPacketData(sendPacket, &packetLastIndex, '\0', sizeof(char));
		}
		PacektSendMethod(sendPacket, socekt);
		cout << "send MapData2" << endl;
	}
	else
	{
		PrintErrorCode(NOT_FOUND_BOARDDATA_ERROR);
	}
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

template<class T>
void GameServer::AppendPacketData(char* sendPacket, unsigned int* packetLastIndex, T data, unsigned int dataSize)
{
	memcpy(&sendPacket[*packetLastIndex], &data, dataSize);
	*packetLastIndex += dataSize;
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