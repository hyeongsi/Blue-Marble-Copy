#include "MatchingClient.h"

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
	serverAddress.sin_port = htons(PORT);
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
	while (nullptr != recvThreadHandle)
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
		else
		{
			//memcpy(&header, &cBuffer[0], sizeof(char));

			//switch (header)	// 나중에 enum 값으로 변경하기
			//{
			//case GET_MATCHING_USER_PACKET:
			//	//GetMapDataMethod1(cBuffer);
			//	break;
			//default:
			//	break;
			//}
		}
	}
}

void MatchingClient::SetMatchUserPacketMethod(char* packet)
{
	instance->SetMatchUser(packet);
}

void MatchingClient::SetMatchUser(char* packet)
{
	// 게임 시작에 받은 유저들을 모아서 실제 게임 플레이 하도록 구현하기
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

template<class T>
void MatchingClient::AppendPacketData(T data, unsigned int dataSize, bool isAddress)
{
	memcpy(&sendPacket[packetLastIndex], &data, dataSize);
	packetLastIndex += dataSize;
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