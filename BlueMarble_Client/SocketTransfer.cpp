#include "SocketTransfer.h"
#include <string>
#include <WS2tcpip.h>	// inet_pton()
#include "MainSystem.h"

using namespace std;

SocketTransfer* SocketTransfer::instance = nullptr;

SocketTransfer::SocketTransfer() {}
SocketTransfer::~SocketTransfer() {}

void SocketTransfer::PrintErrorCode(const int errorCode)
{
	MessageBox(MainSystem::GetInstance()->GetWindowHwnd(State::MAIN_MENU), 
		("error code : " + to_string(errorCode)).c_str(), "error ¹ß»ý", MB_OK);
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
