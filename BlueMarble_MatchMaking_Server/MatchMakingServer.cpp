#include "MatchMakingServer.h"

MatchMakingServer* MatchMakingServer::instance = nullptr;

MatchMakingServer::MatchMakingServer() {}
MatchMakingServer::~MatchMakingServer() {}

string MatchMakingServer::GetClientIp(SOCKADDR_IN clientAddress)
{
	char buf[32] = { 0, };
	return inet_ntop(AF_INET, &clientAddress.sin_addr, buf, sizeof(buf));
}

UINT WINAPI MatchMakingServer::RecvDataThread(void* arg)
{
	instance->StartRecvDataThread(*(SOCKET*)arg);
	return 0;
}

void MatchMakingServer::StartRecvDataThread(SOCKET clientSocket)
{
	cout << "start ListenThread" << endl;

	char cBuffer[MAX_PACKET_SIZE] = {};
	char header = NULL;

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
			case 0:
				break;
			default:
				break;
			}
		}
	}

	cout << "lost connect : " << GetClientIp(clientAddress) << endl;
}

void MatchMakingServer::InitServer()
{
	if (0 != WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		cout << "WSAStartup Error" << endl;
		return;
	}
	cout << "WSAStartup" << endl;
	serverSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(PORT);
	serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);

	if (-1 == bind(serverSocket, (SOCKADDR*)&serverAddress, sizeof(serverAddress)))
	{
		cout << "bind Error" << endl;
		return;
	}
	cout << "bind" << endl;

	if (-1 == listen(serverSocket, SOMAXCONN))
	{
		cout << "listen Error" << endl;
		return;
	}
	cout << "listen" << endl;
}

void MatchMakingServer::AcceptSocket()
{
	int clientAddressSize = sizeof(instance->clientAddress);

	clientSocket = accept(serverSocket, (SOCKADDR*)&clientAddress, &clientAddressSize);
	cout << "Connect Ip : " << GetClientIp(clientAddress) << endl;
	cout << "Working AcceptThread" << endl << endl;;

	_beginthreadex(NULL, 0, RecvDataThread, &clientSocket, 0, NULL);	// recv thread 실행
}

MatchMakingServer* MatchMakingServer::GetInstance()
{
	if (nullptr == instance)
	{
		instance = new MatchMakingServer();
	}

	return instance;
}

void MatchMakingServer::ReleaseInstance()
{
	delete instance;
	instance = nullptr;
}

void MatchMakingServer::StartServer()
{
	InitServer();
	AcceptSocket();
	
	while (true)
	{
		if (2 <= matchQueue.size())
		{
			MakePacket(SET_MATCHING_USER_PACKET);
			AppendPacketData(matchQueue.front(), sizeof(int));
			matchQueue.pop();
			AppendPacketData(matchQueue.front(), sizeof(int));
			matchQueue.pop();

			PacektSendMethod(clientSocket);
		}
	}
}

void MatchMakingServer::MakePacket(char header)
{
	if (NULL != header)
	{
		memset(sendPacket, 0, MAX_PACKET_SIZE);		// 패킷 초기화
		sendPacket[0] = header;	// header setting
		packetLastIndex = 1;
	}
	else
	{
		memset(sendPacket, 0, MAX_PACKET_SIZE);		// 패킷 초기화
		packetLastIndex = 0;
	}
}

template<class T>
void MatchMakingServer::AppendPacketData(T data, unsigned int dataSize)
{
	memcpy(&sendPacket[packetLastIndex], &data, dataSize);
	packetLastIndex += dataSize;
}

void MatchMakingServer::AppendPacketPointerData(const char* data, unsigned int dataSize)
{
	memcpy(&sendPacket[packetLastIndex], data, dataSize);
	packetLastIndex += dataSize;
}

void MatchMakingServer::PacektSendMethod(SOCKET& socket)
{
	if (send(socket, sendPacket, MAX_PACKET_SIZE, 0) == -1)
	{
		cout << "send Error" << endl;
	}
	cout << "send Data" << endl;
}
