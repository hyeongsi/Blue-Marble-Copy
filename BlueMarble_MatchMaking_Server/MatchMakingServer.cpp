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
		memcpy(&header, &cBuffer[0], sizeof(char));
		cout << "recv " << (int)header << endl;

		recvCBF = recvCallbackFuncMap[header];

		if (nullptr != recvCBF)
		{
			recvCBF(cBuffer);
			recvCBF = nullptr;
		}
	}

	cout << "lost connect : " << GetClientIp(clientAddress) << endl;
	lostConnectCBF = AcceptSocket;
}

void MatchMakingServer::InitServer()
{
	recvCallbackFuncMap[SET_MATCHING_USER_PACKET] = PushUserId;
	recvCallbackFuncMap[POP_MATCHING_USER_PACKET] = PopUserIndex;

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
	instance->recvCBF = nullptr;
	instance->lostConnectCBF = nullptr;
	
	instance->matchQueue.clear();
	int clientAddressSize = sizeof(instance->clientAddress);

	instance->clientSocket = accept(instance->serverSocket, (SOCKADDR*)&instance->clientAddress, &clientAddressSize);
	cout << "Connect Ip : " << instance->GetClientIp(instance->clientAddress) << endl;
	cout << "Working AcceptThread" << endl << endl;;

	_beginthreadex(NULL, 0, RecvDataThread, &instance->clientSocket, 0, NULL);	// recv thread 실행
}

void MatchMakingServer::PushUserId(char* packet)
{
	unsigned int userId;

	memcpy(&userId, &packet[sizeof(char)], sizeof(userId));
	instance->matchQueue.push_front(userId);
}

void MatchMakingServer::PopUserIndex(char* packet)
{
	unsigned int userId;

	memcpy(&userId, &packet[sizeof(char)], sizeof(userId));

	for (auto it = instance->matchQueue.begin(); it != instance->matchQueue.end(); it++)
	{
		if (userId != (*it))
			continue;

		instance->matchQueue.erase(it);
		break;
	}
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
		if (lostConnectCBF != nullptr)
		{
			lostConnectCBF();	// AcceptSocket();    ->    서버와 연결이 끊겼을 경우, 다시 acceptSocket을 호출함,
		}

		if (MAX_MATCH_QUEUE_SIZE <= matchQueue.size())
		{
			MakePacket(SET_MATCHING_USER_PACKET);
			AppendPacketData(MAX_MATCH_QUEUE_SIZE, sizeof(int));	// 플레이 인원

			for (int i = 0; i < MAX_MATCH_QUEUE_SIZE; i++)
			{
				AppendPacketData(matchQueue.back(), sizeof(unsigned int));
				matchQueue.pop_back();
			}

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
