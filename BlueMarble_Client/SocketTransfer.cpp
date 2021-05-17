#include "SocketTransfer.h"
#include <string>
#include <WS2tcpip.h>	// inet_pton()
#include <process.h>
#include "MainSystem.h"
#include "GameManager.h"
#include "GameWindow.h"

SocketTransfer* SocketTransfer::instance = nullptr;

SocketTransfer::SocketTransfer() {}
SocketTransfer::~SocketTransfer() {}

void SocketTransfer::RecvDataMethod(SOCKET clientSocket)
{
	char header = NULL;

	while (nullptr != recvThreadHandle)
	{
		char cBuffer[MAX_PACKET_SIZE] = {};
		header = NULL;

		if ((recv(clientSocket, cBuffer, MAX_PACKET_SIZE, 0)) == -1)
		{
			PrintErrorCode(State::GAME, RECV_ERROR);
			break;
		}

		if (nullptr != recvCBF)
		{
			recvCBF(cBuffer);
		}
		else
		{
			memcpy(&header, &cBuffer[0], sizeof(char));

			switch (header)	// 나중에 enum 값으로 변경하기
			{
			case GET_MAPDATA:
				GetMapDataMethod1(cBuffer);
				break;
			case READY:
				GetReadyMethod(cBuffer);
				break;
			case ROLL_DICE_SIGN:
				GetRollDiceSignMethod(cBuffer);
				break;
			case ROLL_DICE:
				GetRollDiceMethod(cBuffer);
				break;
			case FINISH_THIS_TURN_PROCESS:
				SendNextTurnSignMethod();
				break;
			default:
				break;
			}
		}
	}

	TerminateRecvDataThread();
}

UINT WINAPI SocketTransfer::RecvDataThread(void* arg)
{
	instance->RecvDataMethod(GetInstance()->clientSocket);
	return 0;
}

void SocketTransfer::GetMapDataMethod1(char* packet)
{
	instance->GetMapData1(packet);
}

void SocketTransfer::GetMapData1(char* packet)
{
	mapPacket1 _mapPacket1;
	int code = 0;

	memcpy(&_mapPacket1.mapSize, &packet[1], sizeof(unsigned int));	// get mapSize
	for (int i = 0; i < (int)_mapPacket1.mapSize * DIRECTION; i++)
	{
		memcpy(&code, &packet[1 + sizeof(unsigned int) + (i*sizeof(int))], sizeof(int));	// get mapCode
		_mapPacket1.code.emplace_back(code);
	}

	GameManager::GetInstance()->GetAddressBoardData()->mapSize = _mapPacket1.mapSize;
	GameManager::GetInstance()->GetAddressBoardData()->code = _mapPacket1.code;

	GameManager::GetInstance()->SetGameMessage("");	// 메시지 초기화
	recvCBF = GetMapDataMethod2;
}

void SocketTransfer::GetMapDataMethod2(char* packet)
{
	instance->GetMapData2(packet);
}

void SocketTransfer::GetMapData2(char* packet)
{
	mapPacket2 _mapPacket2;
	char str[100];
	unsigned int stackMemorySize = 0;

	for (int i = 0; i < 8 * DIRECTION; i++)
	{
		memcpy(&_mapPacket2.charSize, &packet[stackMemorySize], sizeof(unsigned int));	// get charSize
		stackMemorySize += sizeof(unsigned int);

		strcpy_s(str, _mapPacket2.charSize, &packet[stackMemorySize]);
		stackMemorySize += _mapPacket2.charSize;

		_mapPacket2.name.emplace_back(str);
	}

	GameManager::GetInstance()->GetAddressBoardData()->name = _mapPacket2.name;
	RenderManager::GetInstance()->InitDrawBoardMap();
	recvCBF = nullptr;
}

void SocketTransfer::GetReadyMethod(char* packet)
{
	readyPacket rPacket;
	memcpy(&rPacket.header, &packet[0], sizeof(char));					// get ready sign
	memcpy(&rPacket.roomIndex, &packet[1], sizeof(int));				// get roomIndex
	memcpy(&rPacket.number, &packet[1 + sizeof(int)], sizeof(int));		// get number
	memcpy(&rPacket.playerCount, &packet[1 + sizeof(int) + sizeof(int)], sizeof(int));		// get playerCount

	for (int i = 0; i < rPacket.playerCount; i++)
	{
		GameManager::GetInstance()->GetUserPositionVector()->emplace_back(0);
	}
	GameManager::GetInstance()->SetPlayerCount(rPacket.playerCount);

	MakePacket(READY);
	AppendPacketData(rPacket.roomIndex, sizeof(int));
	SendMessageToGameServer();
}

void SocketTransfer::GetRollDiceSignMethod(char* packet)
{
	instance->GetRollDiceSign(packet);
}

void SocketTransfer::GetRollDiceSign(char* packet)
{
	GameManager::GetInstance()->SetIsMyTurn(true);
	GameManager::GetInstance()->SetGameState(GameState::ROLL_DICE);
	GameManager::GetInstance()->SetGameMessage("주사위를 돌려주세요");
	
	GameWindow::GetInstance()->ShowButton();
}

void SocketTransfer::GetRollDiceMethod(char* packet)
{
	instance->GetRollDice(packet);
	instance->recvCBF = nullptr;
}

void SocketTransfer::GetRollDice(char* packet)
{
	diceRollPacket dPacket;
	memcpy(&dPacket.header, &packet[0], sizeof(char));						// get header
	memcpy(&dPacket.whosTurn, &packet[1], sizeof(int));						// get turn
	memcpy(&dPacket.diceValue1, &packet[1 + sizeof(int)], sizeof(int));		// get diceValue1
	memcpy(&dPacket.diceValue2, &packet[1 + sizeof(int) + sizeof(int)], sizeof(int));		// get diceValue2

	GameManager::GetInstance()->SetGameMessage(to_string(dPacket.diceValue1) + " , " + to_string(dPacket.diceValue2));	// 메시지 갱신
	GameWindow::GetInstance()->HideButton();

	GameManager::GetInstance()->MoveUserPosition(dPacket.whosTurn, dPacket.diceValue1 + dPacket.diceValue2);
}

void SocketTransfer::SendNextTurnSignMethod()
{
	instance->SendNextTurnSign();
}

void SocketTransfer::SendNextTurnSign()
{
	MakePacket(FINISH_THIS_TURN_PROCESS);
	SendMessageToGameServer();
}

void SocketTransfer::PrintErrorCode(State state, const int errorCode)
{
	switch (state)
	{
	case State::MAIN_MENU:
	case State::RANK_MENU:
		MessageBox(MainSystem::GetInstance()->GetWindowHwnd(State::MAIN_MENU),
			("error code : " + to_string(errorCode)).c_str(), "error 발생", MB_OK);
		break;
	case State::GAME:
		MessageBox(MainSystem::GetInstance()->GetWindowHwnd(State::GAME),
			("error code : " + to_string(errorCode)).c_str(), "error 발생", MB_OK);
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

void SocketTransfer::MakePacket(char header)
{
	if(NULL != header)
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
void SocketTransfer::AppendPacketData(T data, unsigned int dataSize)
{
	memcpy(&sendPacket[packetLastIndex], &data, dataSize);
	packetLastIndex += dataSize;
}

void SocketTransfer::AppendPacketPointerData(char* data, unsigned int dataSize)
{
	memcpy(&sendPacket[packetLastIndex], data, dataSize);
	packetLastIndex += dataSize;
}

void SocketTransfer::SendMessageToGameServer()
{
	if (send(clientSocket, sendPacket, MAX_PACKET_SIZE, 0) == -1)
	{
		PrintErrorCode(State::GAME, SEND_ERROR);
	}

	switch (sendPacket[0])	// header check
	{
	case GET_MAPDATA:
		recvCBF = GetMapDataMethod1;
		break;
	}
}

void SocketTransfer::RegistRecvCallbackFunction(CALLBACK_FUNC_PACKET cbf)
{
	recvCBF = cbf;
}

void SocketTransfer::SendRollDiceSign()
{
	recvCBF = GetRollDiceMethod;

	MakePacket(ROLL_DICE_SIGN);
	SendMessageToGameServer();
}
