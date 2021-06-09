#pragma once
#include <WinSock2.h>
#pragma comment(lib, "ws2_32")

using namespace std;

constexpr const int PORT = 4567;
constexpr const int MATCH_SERVER_PORT = 4568;
constexpr const int MAX_PACKET_SIZE = 1024;
constexpr const char* SERVER_IP = "192.168.123.101";

typedef void(*CALLBACK_FUNC_PACKET)(char*);

enum ErrorCode
{
	WSASTARTUP_ERROR = 100,
	BIND_ERROR = 101,
	LISTEN_ERROR = 102,
	NOT_FOUND_BOARDDATA_ERROR = 103,
	SEND_ERROR = 104,

	CONNECT_ERROR = 201,
	MATCHING_SERVER_RECV_ERROR = 202,
	MATCHING_SERVER_SEND_ERROR = 203,
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
	CARD_SIGN_SYNC = 12,
	TRAP_CARD_SYNC = 13,
	DESERT_ISLAND_SIGN = 14,
	OLYMPIC_SIGN = 15,
	OLYMPIC = 16,
	WORLD_TRABLE_SIGN = 17,
	WORLD_TRABLE_SIGN_SYNC = 18,
	REVENUE_SIGN = 19,
	BUY_LAND_SYNC = 20,
	BUY_BUILDING_SYNC = 21,
	PAY_TOLL_SIGN_SYNC = 22,
	TAKE_OVER_SYNC = 23,
	BUY_LANDMARK_SIGN_SYNC = 24,
	SELL_LAND_SIGN_SYNC = 25,
	REVENUE_SIGN_SYNC = 26,
	FINISH_THIS_TURN_PROCESS = 27,
	SEND_SELECT_MODE_INPUT_KEY = 30,
	SELECT_MODE_BTN = 31,
	BANKRUPTCY_SIGN = 32,
	END_GAME = 33,
	GAMEOVER_SIGN = 90,
	SET_MATCHING_USER_PACKET = 100,
	POP_MATCHING_USER_PACKET = 101,
};

typedef struct BuyLandPacket
{
	char header;
	int whosTurn;	// 누구의 요구인지
	bool isBuy;		// 구매여부
}buyLandPacket;

typedef struct BuyBuildingPacket
{
	char header;
	int whosTurn;	// 누구의 요구인지
	bool isBuy;		// 구매여부
	bool isBuyVilla;	// 빌라 구매유무
	bool isBuyBuilding;	// 빌딩 구매유무
	bool isBuyHotel;	// 호텔 구매유무
}buyBuildingPacket;

typedef struct PayTollSignPacket
{
	char header;
	int whosTurn;	// 누구의 요구인지
}payTollSignPacket;

typedef struct TakeOverSignPacket
{
	char header;
	int whosTurn;	// 누구의 요구인지
	bool isTakeOver;	// 인수유무
}takeOverSignPacket;

typedef struct BuyLandMarkSignPacket
{
	char header;
	int whosTurn;	// 누구의 요구인지
	bool isBuy;	// 구매유무
}buyLandMarkSignPacket;

typedef struct GetSelectIndexPacket	// OLYMPIC_SIGN, WORLD_TRABLE_SIGN
{
	char header;
	int selectIndex;	// 선택 값
}getSelectIndexPacket;

typedef struct SelectInputKeyPacket
{
	char header;
	int inputKey;	// 입력 키값
	int currentSelectValue;	// 현재 선택 값
}selectInputKeyPacket;

typedef struct SellLandProcessPacket
{
	char header;
	bool isOK;
}sellLandProcessPacket;

typedef struct CardSignProcessPacket
{
	char header;
	bool isTrapCard;	// 발동 카드 유무
	bool isOk;			// 발동 카드 사용 유무
}cardSignProcessPacket;