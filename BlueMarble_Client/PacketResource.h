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
	BUY_BUILDING_SIGN = 6,
	PAY_TOLL_SIGN = 7,
	TAKE_OVER_SIGN = 8,
	BUY_LANDMARK_SIGN = 9,
	SELL_LAND_SIGN = 10,
	CARD_SIGN = 11,
	DESERT_ISLAND_SIGN = 12,
	OLYMPIC_SIGN = 13,
	OLYMPIC = 14,
	WORLD_TRABLE_SIGN = 15,
	WORLD_TRABLE_SIGN_SYNC = 16,
	REVENUE_SIGN = 17,
	BUY_LAND_SYNC = 18,
	BUY_BUILDING_SYNC = 19,
	PAY_TOLL_SIGN_SYNC = 20,
	TAKE_OVER_SYNC = 21,
	BUY_LANDMARK_SIGN_SYNC = 22,
	SELL_LAND_SIGN_SYNC = 23,
	REVENUE_SIGN_SYNC = 24,
	FINISH_THIS_TURN_PROCESS = 25,
	SEND_SELECT_MODE_INPUT_KEY = 30,
	SELECT_MODE_BTN = 31,
};

enum SellState
{
	BUILD_LANDMARK = 0,
	PAY_TOLL = 1,
	TAKE_OVER_LAND = 2,
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
	int plusMoney;  // 추가 자금
	bool isDesertIsland;	//감옥 유무
}diceRollPacket;

typedef struct BuyLandPacket	// BUY_LAND_SIGN, BUY_TOUR_SIGN
{
	char header;
	int whosTurn;	// 누구 턴인지
	int passPrice;	// 땅 가격
}buyLandPacket;

typedef struct BuyLandSyncPacket	//BUY_Land_Sync
{
	char header;
	bool isBuy;		// 구입 여부
	int whosTurn;	// 누구 턴인지
	int landPrice;	// 땅 가격
	int userMoney;	// 해당 유저 돈
}buyLandSyncPacket;

typedef struct BuyBuildingPacket	//BUY_BUILDING_SIGN
{
	char header;
	int whosTurn;			// 누구 턴인지

	int villaPrice;			// 빌라 가격
	int buildingPrice;		// 빌딩 가격
	int hotelPrice;			// 호텔 가격

	bool isBuildVilla;		// 빌라 건축 유무
	bool isBuildBuilding;	// 빌딩 건축 유무
	bool isBuildHotel;		// 호텔 건축 유무

	int userMoney;	// 해당 유저 돈
}buyBuildingPacket;

typedef struct BuyBuildSyncPacket	//SELL_LAND_SIGN
{
	char header;
	bool isBuy;		// 구입 여부
	int whosTurn;	// 누구 턴인지

	bool isBuyVilla;	// 빌라 구매유무
	bool isBuyBuilding;	// 빌딩 구매유무
	bool isBuyHotel;	// 호텔 구매유무

	int accumPrice;	// 총 가격
	int userMoney;	// 해당 유저 돈
}buyBuildingSyncPacket;

typedef struct SellLandSignPacket	// SELL_LAND_SIGN
{
	char header = 0;
	int whosTurn = 0;	// 누구 턴인지
	int goalPrice = 0;	// 요구 금액
	int userMoney = 0;	// 해당 유저 돈
	int landSize = 0; // 땅 개수
	vector<int> landPosition;	// 땅 위치
}sellLandSignPacket;

typedef struct OlympicSignPacket	// OLYMPIC_SIGN
{
	char header;
	int userPosition;	// 현재 위치
}olympicSignPacket;

typedef struct OlympicSignSyncPacket	// OLYMPIC
{
	char header;
	int whosTurn;
	int olympicPosition;	// 올림픽 위치
	int olympicStack;	// 올림픽 열린 횟수
}olympicSignSyncPacket;

typedef struct WorldTrableSignPacket	// WORLD_TRABLE_SIGN
{
	char header;
	int userPosition;	// 현재 위치
}worldTrableSignPacket;

typedef struct WorldTrableSignSyncPacket	// WORLD_TRABLE_SIGN_SYNC
{
	char header;
	int whosTurn;	// 누구 턴인지
	int userPosition; // 유저 위치
	int userMoney;	// 유저 돈
}worldTrableSignSyncPacket;

typedef struct RevenueSignPacket	// REVENUE_SIGN
{
	char header;
	int whosTurn;	// 누구 턴인지
	int tax;		// 세금
}revenueSignPacket;

typedef struct PayTollPacket	// PAY_TOLL_SIGN
{
	char header;
	int whosTurn;	// 누구 턴인지
	int passPrice;	// 통행료
}payTollPacket;

typedef struct PayTollSyncPacket	//PAY_TOLL_SIGN_SYNC
{
	char header;
	bool isPass;	// 통행료 지불 유무
	int whosTurn;	// 누구 턴인지
	int landOwner;	// 땅 주인 번호
	int toll;		// 통행료
	int userMoney;	// 해당 유저 돈
	int landOwnerMoney; // 땅 주인 돈
}payTollSyncPacket;

typedef struct BuyLandMarkSignPacket	// BUY_LANDMARK_SIGN
{
	char header;
	int whosTurn;	// 누구 턴인지
	int landMarkPrice;	// 랜드마크 가격
}buyLandMarkSignPacket;

typedef struct TakeOverPacket	// TAKE_OVER_SIGN
{
	char header;
	int whosTurn;	// 누구 턴인지
	int takeOverPrice;	// 인수비용
}takeOverPacket;

typedef struct TakeOverSyncPacket	// TAKE_OVER_SYNC
{
	char header;
	int whosTurn;	// 누구 턴인지
	int takeOverPrice;	// 인수비용
	int owner;	// 땅 주인
	int userMoney; // 해당 유저 돈
	int ownerMoney; // 원래 땅 주인 돈
}takeOverSyncPacket;

typedef struct BuyLandMarkSyncPacket	// BUY_LANDMARK_SIGN_SYNC
{
	char header;
	int whosTurn;	// 누구 턴인지
	int landMarkPrice;	// 랜드마크 가격
	int userMoney; // 해당 유저 돈
}buyLandMarkSyncPacket;

typedef struct SellLandSyncPacket	// SELL_LAND_SIGN_SYNC
{
	char header;
	int whosTurn;	// 누구 턴인지
	int userMoney;	// 유저 돈
	int goalPrice;	// 목표 금액
	int sellLandCount;	// 판매하는 땅 개수
	vector<int> landIndex; // 땅 번호
}sellLandSyncPacket;

typedef struct RevenueSignSyncPacket	// REVENUE_SIGN_SYNC
{
	char header;
	int whosTurn;	// 누구 턴인지
	int tax;	// 세금
	int userMoney;	// 유저 돈
}revenueSignSyncPacket;

typedef struct SelectInputKeyPacket	// SEND_SELECT_MODE_INPUT_KEY
{
	char header;
	int selectLandIndex;	// 선택 지역 번호
	bool isSpaceBar;	// 스페이스바 동작 유무
	bool isErase;		// 데이터 삭제 유무
	int sellLandPrice;  // 매각 땅 가격
}selectInputKeyPacket;