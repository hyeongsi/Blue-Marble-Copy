#include "MatchingClient.h"
#include "GameServer.h"

MatchingClient* MatchingClient::instance = nullptr;

MatchingClient::MatchingClient() {}
MatchingClient::~MatchingClient() {}

void MatchingClient::PrintMatchingServerError(int errorCode)
{
	cout << "MatchingServer, " << errorCode << endl;
}

void MatchingClient::Init()
{
	if (0 != WSAStartup(MAKEWORD(2, 2), &instance->wsaData))
	{
		PrintMatchingServerError(WSASTARTUP_ERROR);
	}
}

bool MatchingClient::Connect()
{
	clientSocket = socket(PF_INET, SOCK_STREAM, 0);

	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(MATCH_SERVER_PORT);
	inet_pton(AF_INET, SERVER_IP, &serverAddress.sin_addr);

	int connectResult = connect(clientSocket, (SOCKADDR*)&serverAddress, sizeof(serverAddress));
	if (connectResult != 0)
	{
		PrintMatchingServerError(CONNECT_ERROR);
		return false;
	}

	return true;
}

UINT WINAPI MatchingClient::RecvDataThread(void* arg)
{
	instance->RecvDataMethod();
	return 0;
}

void MatchingClient::RecvDataMethod()
{
	while (true)
	{
		char cBuffer[MAX_PACKET_SIZE] = {};
		char header = NULL;

		if ((recv(clientSocket, cBuffer, MAX_PACKET_SIZE, 0)) == -1)
		{
			PrintMatchingServerError(MATCHING_SERVER_RECV_ERROR);
			break;
		}


		if (nullptr != recvCBF)
		{
			recvCBF(cBuffer);
		}
	}
}

void MatchingClient::SetMatchUserPacketMethod(char* packet)
{
	instance->SetMatchUser(packet);
}

void MatchingClient::SetMatchUser(char* packet)
{
	matchMakingPacket userPacket;

	memcpy(&userPacket.user1Id, &packet[1], sizeof(unsigned int));	// get socket
	memcpy(&userPacket.user2Id, &packet[1 + sizeof(unsigned int)], sizeof(unsigned int));	// get socket

	// 받은 소켓으로 게임방 만들기
}

MatchingClient* MatchingClient::GetInstance()
{
	if (nullptr == instance)
	{
		instance = new MatchingClient();
	}

	return instance;
}

void MatchingClient::ReleaseInstance()
{
	delete instance;
	instance = nullptr;
	WSACleanup();
}

void MatchingClient::ConnectMathchServer()
{
	Init();

	if (Connect())
	{
		_beginthreadex(NULL, 0, GetInstance()->RecvDataThread, nullptr, 0, NULL);
		cout << "connect MatchServer" << endl;
		cout << "start MatchServer RecvThread" << endl << endl;
	}
}

void MatchingClient::MakePacket(char header)
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

void MatchingClient::AppendPacketDataMethod(unsigned int data, unsigned int dataSize)
{
	memcpy(&instance->sendPacket[instance->packetLastIndex], &data, dataSize);
	instance->packetLastIndex += dataSize;
}

void MatchingClient::AppendPacketPointerData(char* data, unsigned int dataSize)
{
	memcpy(&sendPacket[packetLastIndex], data, dataSize);
	packetLastIndex += dataSize;
}

void MatchingClient::SendMessageToMatchServer()
{
	if (send(clientSocket, sendPacket, MAX_PACKET_SIZE, 0) == -1)
	{
		PrintMatchingServerError(MATCHING_SERVER_SEND_ERROR);
	}

	switch (sendPacket[0])	// header check
	{
	case SET_MATCHING_USER_PACKET:
		recvCBF = SetMatchUserPacketMethod;
		break;
	}
}