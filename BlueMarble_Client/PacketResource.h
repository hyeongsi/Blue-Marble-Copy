#pragma once
#include <vector>
using namespace std;

constexpr const int PORT = 4567;
constexpr const char* SERVER_IP = "192.168.123.101";

enum ErrorCode
{
	WSASTARTUP_ERROR = 200,
	CONNECT_ERROR = 201,
	RECV_ERROR = 202,
	SEND_ERROR = 203,
};

enum MessageCode
{
	GET_MAPDATA = 1,
	READY = 2,
	ROLL_DICE_SIGN = 3,
	ROLL_DICE = 4,
	BUY_LAND_SIGN = 5,
	BUY_LAND = 6,
	BUY_TOUR_SIGN = 7,
	BUY_TOUR = 8,
	CARD_SIGN = 9,
	DESERT_ISLAND_SIGN = 10,
	OLYMPIC_SIGN = 11,
	OLYMPIC = 12,
	WORLD_TRABLE_SIGN = 13,
	WORLD_TRABLE = 14,
	REVENUE_SIGN = 15,
	FINISH_THIS_TURN_PROCESS = 20,
};

typedef struct MapPacket1
{
	char header = -1;
	unsigned int mapSize = NULL;
	vector<int> code;
}mapPacket1;

typedef struct MapPacket2
{
	unsigned int charSize = NULL;
	vector<string> name;
}mapPacket2;

typedef struct ReadyPacket
{
	char header;
	int number;	// 몇번째 캐릭터인지 구분하기 위해 사용
	int playerCount;	// 총 플레이 유저 수
	float initMoney;	// 초기 자금
}readyPacket;

typedef struct DiceRollPacket
{
	char header;
	int whosTurn;	// 누구차례인지
	int diceValue1;	// 주사위1 값
	int diceValue2;	// 주사위2 값
}diceRollPacket;