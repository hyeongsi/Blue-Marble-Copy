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
	CARD_SIGN = 9,
	DESERT_ISLAND_SIGN = 10,
	OLYMPIC_SIGN = 11,
	OLYMPIC = 12,
	WORLD_TRABLE_SIGN = 13,
	WORLD_TRABLE = 14,
	REVENUE_SIGN = 15,
	PAY_TOLL_SIGN = 16,
	BUY_LAND_SYNC = 17,
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
	int initMoney;	// 초기 자금
}readyPacket;

typedef struct DiceRollPacket
{
	char header;
	int whosTurn;	// 누구차례인지
	int diceValue1;	// 주사위1 값
	int diceValue2;	// 주사위2 값
}diceRollPacket;

typedef struct BuyLandPacket	// BUY_LAND_SIGN, BUY_TOUR_SIGN
{
	char header;
	int whosTurn;	// 누구 턴인지
	int passPrice;	// 통행료
}buyLandPacket;

typedef struct BuyLandSyncPacket	//BUY_Land_Sync
{
	char header;
	bool isBuy;		// 구입 여부
	int whosTurn;	// 누구 턴인지
	int landPrice;	// 땅 가격
	int userMoney;	// 해당 유저 돈
}buyLandSyncPacket;

typedef struct BuyLandSyncPacke2t	//BUY_LAND
{
	char header;
	bool isBuy;		// 구입 여부
	int whosTurn;	// 누구 턴인지
	float userMoney;	// 해당 유저 돈

	float villaPrice;	// 빌라 가격
	float buildingPrice;// 빌딩 가격
	float hotelPrice;	// 호텔 가격
	float landMarkPrice;// 랜드마크 가격

	bool isBuildVilla;		// 빌라 건축 유무
	bool isBuildBuilding;	// 빌딩 건축 유무
	bool isBuildHotel;		// 호텔 건축 유무
}buyLandSyncPacket2;