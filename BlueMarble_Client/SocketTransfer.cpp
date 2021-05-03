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
		customPacket* packet;
		char cBuffer[PACKET_SIZE] = {};

		if ((recv(clientSocket, cBuffer, PACKET_SIZE, 0)) == -1)
		{
			PrintErrorCode(State::GAME, RECV_ERROR);
			break;
		}
		packet = (customPacket*)cBuffer;
		switch (packet->header)	// ���߿� enum ������ �����ϱ�
		{
		case GET_MAPDATA:
			GetMapDataMethod(packet);
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

void SocketTransfer::GetMapDataMethod(customPacket* packet)
{
	const int RECV_COUNT = 2;

	boardData board;
	board.mapSize = (int)packet->data;

	board.code = new int[board.mapSize * DIRECTION];
	board.name = new char* [board.mapSize * DIRECTION];

	for (int i = 0; i < board.mapSize * DIRECTION; i++)
	{
		board.name[i] = new char[NAME_SIZE];
	}

	char cBuffer[PACKET_SIZE] = {};

	for (int i = 0; i < RECV_COUNT; i++)
	{
		if ((recv(clientSocket, cBuffer, PACKET_SIZE, 0)) == -1)
		{
			PrintErrorCode(State::GAME, RECV_ERROR);
			return;
		}
		packet = (customPacket*)cBuffer;

		if (0 == i)
		{
			memcpy(&board.code, &packet->data, packet->dataSize);
		}
		else if (1 == i)
		{
			memcpy(&board.name, &packet->data, packet->dataSize);
		}
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

void SocketTransfer::SendMessageToGameServer(int header, int dataSize, char* data)
{
	customPacket packet(header, dataSize, data);

	if (send(clientSocket, (char*)&packet, PACKET_SIZE, 0) == -1)
	{
		PrintErrorCode(State::GAME, SEND_ERROR);
	}
}
