#pragma once
#include "TransferResource.h"
#include <vector>
#include <time.h>

constexpr const int MAX_PLAYER = 2;

enum class GameState
{
	WAIT = 0,
	ROLL_DICE_SIGN = 1,
	ROLL_DICE = 2,
};

class GameServer;
class GameRoom
{
private:
	GameServer* gameServer = nullptr;
	vector<SOCKET> userVector;
	int takeControlPlayer = 0;	// 누구 차례인지 구분 변수

	char sendPacket[MAX_PACKET_SIZE] = {};
	unsigned int packetLastIndex = 0;

public:
	int connectPlayer = 0;
	clock_t startTime = 0;
	clock_t finishTime = 0;
	GameState state = GameState::ROLL_DICE_SIGN;

	GameRoom(SOCKET user1, SOCKET user2);

	vector<SOCKET> GetUserVector();
	void NextTurn();	// 차례 넘겨주기
	int GetTakeControlPlayer();	// 누구 차례인지 값 전송

	bool CheckSendDelay();
	void SendMapDataMethod(SOCKET& socekt);
	void SendRollDiceSignMethod(SOCKET& socket);

	void SendRollTheDice(int value);
	void UpdateMapData(int diceValue);
};

