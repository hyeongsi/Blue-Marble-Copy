#pragma once
#include "TransferResource.h"
#include "MapManager.h"
#include <vector>
#include <time.h>

constexpr const int MAX_PLAYER = 2;

enum class GameState
{
	WAIT = 0,
	ROLL_DICE_SIGN = 1,
	NEXT_TURN = 2,
};

class GameServer;
class GameRoom
{
private:
	GameServer* gameServer = nullptr;
	vector<SOCKET> userVector;

	vector<int> userPositionVector;	// 유저 위치
	int takeControlPlayer = 0;	// 누구 차례인지 구분 변수
	int diceDoubleCount = 0;	// 주사위 더블 카운트 변수,

	vector<bool> isFinishTurnProcessVector;	// 해당 차례 처리 유무 
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
	void NextTurn();	// 차례 넘겨주기
	int GetTakeControlPlayer();	// 누구 차례인지 값 전송

	int GetDiceDoubleCount();		// 더블 카운트 전송
	void SetDiceDoubleCount(int count);	// 더블 카운트 변경

	bool CheckSendDelay();	// 딜레이 체크 함수
	void SendMapDataMethod(SOCKET& socekt);	// 맵 정보 전송 함수
	void SendRollDiceSignMethod(SOCKET& socket);	// 주사위 신호 전송 함수

	void SendRollTheDice(int value1, int value2);	// 주사위 눈 전송
	void MoveUserPosition(int diceValue);	// 캐릭터 보드판 위치 이동

	void SendFinishTurnSign();	// 모든 처리 끝나고, 다음턴으로 넘어가도 되는지 확인 메시지 전송
	void CheckEndProcess(SOCKET clientSocket);	// 다음턴으로 이동
};

