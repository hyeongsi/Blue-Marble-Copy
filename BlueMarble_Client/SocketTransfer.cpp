#include "SocketTransfer.h"
#include <string>
#include <WS2tcpip.h>	// inet_pton()
#include <process.h>
#include "MainSystem.h"

SocketTransfer* SocketTransfer::instance = nullptr;

SocketTransfer::SocketTransfer() {}
SocketTransfer::~SocketTransfer() {}

void SocketTransfer::RecvDataMethod(SOCKET clientSocket)
{
	char cBuffer[PACKET_SIZE] = {};
	customPacket packet;

	while (nullptr != recvThreadHandle)
	{
		if ((recv(clientSocket, cBuffer, PACKET_SIZE, 0)) == -1)
		{
			PrintErrorCode(RecvError);
			break;
		}
		
		packet = *(customPacket*)cBuffer;
		switch (packet.header)	// ���߿� enum ������ �����ϱ�
		{
		case 0:
			break;
		case 1:
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

void SocketTransfer::PrintErrorCode(const int errorCode)
{
	MessageBox(MainSystem::GetInstance()->GetWindowHwnd(State::MAIN_MENU), 
		("error code : " + to_string(errorCode)).c_str(), "error �߻�", MB_OK);
}

SocketTransfer* SocketTransfer::GetInstance()
{
	if (nullptr == instance)
	{
		instance = new SocketTransfer();

		if (0 != WSAStartup(MAKEWORD(2, 2), &instance->wsaData))
		{
			instance->PrintErrorCode(WSAStartupError);
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
		PrintErrorCode(ConnectError);
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
		PrintErrorCode(SendError);
	}
}
