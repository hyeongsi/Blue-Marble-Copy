#include "SocketTransfer.h"
#include <string>
#include <WS2tcpip.h>	// inet_pton()
#include <process.h>
#include "MainSystem.h"
#include "GameManager.h"

SocketTransfer* SocketTransfer::instance = nullptr;

SocketTransfer::SocketTransfer() {}
SocketTransfer::~SocketTransfer() {}

void SocketTransfer::RecvDataMethod(SOCKET clientSocket)
{
	while (nullptr != recvThreadHandle)
	{
		char cBuffer[MAX_PACKET_SIZE] = {};
		char header = NULL;
		unsigned int dataSize = 0;

		if ((recv(clientSocket, cBuffer, MAX_PACKET_SIZE, 0)) == -1)
		{
			PrintErrorCode(State::GAME, RECV_ERROR);
			break;
		}

		memcpy(&header, &cBuffer[0], sizeof(char));

		switch (header)	// ���߿� enum ������ �����ϱ�
		{
		case GET_MAPDATA:
			GetMapDataMethod(cBuffer);
			break;
		default:
			break;
		}
		// recv�� callback���� �����Ϸ� ������ ������, �׷��� �����Ϸ��� send �Ŀ��� recv�� �����ϵ��� �����ؾ� callback ��� ����
		// send���� �ʰ� recv �����ϵ��� ����� ����, callback ��� X
		// ���� send�ϰ� recv callback ������ε� �������� ������ �شٰ� �����ϸ�, send�� recv ó���� �ƴ�
		// �ٸ� recv ó���� �� ��Ȳ�� ����� ������ �� �� ����.
	}

	TerminateRecvDataThread();
}

UINT WINAPI SocketTransfer::RecvDataThread(void* arg)
{
	instance->RecvDataMethod(GetInstance()->clientSocket);
	return 0;
}

void SocketTransfer::GetMapDataMethod(char* packet)
{
	unsigned int dataSize = 0;
	unsigned int mapSize = 0;
	boardData board;
	int code = NULL;
	string name = "";

	memcpy(&dataSize, &packet[1], sizeof(unsigned int));
	memcpy(&mapSize, &packet[1 + sizeof(unsigned int)], dataSize);

	char cBuffer[MAX_PACKET_SIZE] = {};

	for (size_t i = 0; i < mapSize*DIRECTION; i++)	// get map code (mapsize * direction)
	{
		if ((recv(clientSocket, cBuffer, MAX_PACKET_SIZE, 0)) == -1)
		{
			PrintErrorCode(State::GAME, RECV_ERROR);
		}

		memcpy(&dataSize, &cBuffer, sizeof(unsigned int));
		memcpy(&code, &cBuffer[sizeof(unsigned int)], dataSize);

		board.code.emplace_back(code);
	}

	for (size_t i = 0; i < mapSize * DIRECTION; i++)	// get map name (mapsize * direction)
	{
		if ((recv(clientSocket, cBuffer, MAX_PACKET_SIZE, 0)) == -1)
		{
			PrintErrorCode(State::GAME, RECV_ERROR);
		}

		memcpy(&dataSize, &cBuffer, sizeof(unsigned int));
		memcpy(&name, &cBuffer[sizeof(unsigned int)], dataSize);

		board.name.emplace_back(name);
	}

	GameManager::GetInstance()->SetBoardData(board);
}

void SocketTransfer::PrintErrorCode(State state, const int errorCode)
{
	switch (state)
	{
	case State::MAIN_MENU:
	case State::RANK_MENU:
		MessageBox(MainSystem::GetInstance()->GetWindowHwnd(State::MAIN_MENU),
			("error code : " + to_string(errorCode)).c_str(), "error �߻�", MB_OK);
		break;
	case State::GAME:
		MessageBox(MainSystem::GetInstance()->GetWindowHwnd(State::GAME),
			("error code : " + to_string(errorCode)).c_str(), "error �߻�", MB_OK);
		break;
	default:
		break;
	}
	
}

SocketTransfer* SocketTransfer::GetInstance()
{
	if (nullptr == instance)
	{
		instance = new SocketTransfer();

		if (0 != WSAStartup(MAKEWORD(2, 2), &instance->wsaData))
		{
			instance->PrintErrorCode(State::GAME, WSASTARTUP_ERROR);
		}
	}

	return instance;
}

void SocketTransfer::ReleaseInstance()
{
	delete instance;
	instance = nullptr;
	WSACleanup();
}

bool SocketTransfer::ConnectServer()
{
	clientSocket = socket(PF_INET, SOCK_STREAM, 0);

	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(PORT);
	inet_pton(AF_INET, SERVER_IP, &serverAddress.sin_addr);

	int connectResult = connect(clientSocket, (SOCKADDR*)&serverAddress, sizeof(serverAddress));
	if (connectResult != 0)
	{
		PrintErrorCode(State::GAME, CONNECT_ERROR);
		return false;
	}

	return true;
}

void SocketTransfer::StartRecvDataThread()
{
	if (nullptr != recvThreadHandle)
	{
		TerminateRecvDataThread();
	}

	recvThreadHandle = (HANDLE)_beginthreadex(NULL, 0, GetInstance()->RecvDataThread, nullptr, 0, NULL);   // recv thread start
}

void SocketTransfer::TerminateRecvDataThread()
{
	recvThreadMutex.lock();
	recvThreadHandle = nullptr;
	recvThreadMutex.unlock();
}

void SocketTransfer::SendMessageToGameServer(char header, unsigned int dataSize, char* data)
{
	unsigned int packetSize = NULL;
	char* buf = nullptr;

	if (NULL == header)	// header ���� ����
	{
		if (0 >= dataSize)
		{
			return;
		}

		packetSize = sizeof(unsigned int) + dataSize;	// datasize + data
		buf = new char[packetSize];

		memcpy(&buf, &dataSize, sizeof(unsigned int));	// datasize setting
		memcpy(&buf[sizeof(unsigned int)], &data, dataSize);
	}
	else   // header ���� ����
	{
		packetSize = sizeof(char) + sizeof(unsigned int) + dataSize;	// header + datasize + data
		buf = new char[packetSize];

		buf[0] = header;	// header setting
		memcpy(&buf[1], &dataSize, sizeof(unsigned int));	// datasize setting
		memcpy(&buf[1 + sizeof(unsigned int)], &data, dataSize);
	}

	if (send(clientSocket, buf, packetSize, 0) == -1)
	{
		PrintErrorCode(State::GAME, SEND_ERROR);
	}

	delete[] buf;
}
