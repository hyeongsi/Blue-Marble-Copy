#pragma once
#include "TransferResource.h"
#include "MapManager.h"
#include <vector>
#include <time.h>

constexpr const int MAX_PLAYER = 2;

enum class GameState
{
	WAIT = 0,
	SYNC_MAPDATA = 1,
	ROLL_DICE_SIGN = 2,
	ROLL_DICE = 3,
};

class GameServer;
class GameRoom
{
private:
	GameServer* gameServer = nullptr;
	vector<SOCKET> userVector;

	vector<int> userPositionVector;	// ���� ��ġ
	int takeControlPlayer = 0;	// ���� �������� ���� ����

	boardData board;

	char sendPacket[MAX_PACKET_SIZE] = {};
	unsigned int packetLastIndex = 0;

public:
	int connectPlayer = 0;
	clock_t startTime = 0;
	clock_t finishTime = 0;
	GameState state = GameState::ROLL_DICE_SIGN;

	GameRoom(SOCKET user1, SOCKET user2);

	vector<SOCKET> GetUserVector();
	void NextTurn();	// ���� �Ѱ��ֱ�
	int GetTakeControlPlayer();	// ���� �������� �� ����

	bool CheckSendDelay();
	void SendMapDataMethod(SOCKET& socekt);
	void SendRollDiceSignMethod(SOCKET& socket);

	void SendRollTheDice(int value);
	void MoveUserPosition(int diceValue);
};

