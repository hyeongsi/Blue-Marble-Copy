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
	LAND_TILE = 2,
	CARD_TILE = 3,
	DESERT_ISLAND_TILE = 4,
	OLYMPIC_TILE = 5,
	WORLD_TRABLE_TILE = 6,
	REVENUE_TILE = 7,
	NEXT_TURN = 9,
};

class GameServer;
class GameRoom
{
private:
	GameServer* gameServer = nullptr;
	vector<SOCKET> userVector;

	int roomId = 0;

	vector<int> userPositionVector;	// 유저 위치
	vector<int> userMoneyVector;	// 유저 소지 자금
	int takeControlPlayer = 0;	// 누구 차례인지 구분 변수
	int diceDoubleCount = 0;	// 주사위 더블 카운트 변수,

	vector<bool> isFinishTurnProcessVector;	// 해당 차례 처리 유무 
	landData landBoardData;	// 건축물 데이터
	boardData board;		// 지역 정보 및 건물 가격 데이터

	char sendPacket[MAX_PACKET_SIZE] = {};
	unsigned int packetLastIndex = 0;

public:
	int connectPlayer = 0;
	clock_t startTime = 0;
	clock_t finishTime = 0;

	bool isDouble = false;	// 더블 유무

	GameState state = GameState::ROLL_DICE_SIGN;

	GameRoom(SOCKET user1, SOCKET user2);

	vector<SOCKET> GetUserVector();
	vector<int>* GetPUserMoneyVector();
	vector<int> GetUserPositionVector();

	void NextTurn();	// 차례 넘겨주기
	int GetTakeControlPlayer();	// 누구 차례인지 값 전송

	landData GetLandBoardData();	// 건축물 정보 전송
	landData* GetPLandBoardData();	// 건축물 정보 전송(포인터)
	boardData GetMapData();			// 맵의 정보 전송

	int GetDiceDoubleCount();		// 더블 카운트 전송
	void SetDiceDoubleCount(int count);	// 더블 카운트 변경

	bool CheckSendDelay();	// 딜레이 체크 함수
	void SendMapDataMethod(SOCKET& socekt);	// 맵 정보 전송 함수
	void SendRollDiceSignMethod(SOCKET& socket);	// 주사위 신호 전송 함수

	void SendRollTheDice(int value1, int value2);	// 주사위 눈 전송
	void MoveUserPosition(int diceValue);	// 캐릭터 보드판 위치 이동

	void SendPayTollSign();	// 통행료 지불
	void SendBuyLandSign();	// 구입 시 처리

	void SendLandSyncSign(int turn, bool isBuy);
	void SendBuildingSyncSign(int turn, bool isBuy, bool isBuyVilla, bool isBuyBuilding, bool isBuyHotel, int accumPrice);

	void CheckLandKindNSendMessage();

	void SendFinishTurnSign();	// 모든 처리 끝나고, 다음턴으로 넘어가도 되는지 확인 메시지 전송
	void CheckEndProcess(SOCKET clientSocket);	// 다음턴으로 이동
};

