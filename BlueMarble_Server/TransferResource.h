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
	DESERT_ISLAND_SIGN = 12,
	OLYMPIC_SIGN = 13,
	OLYMPIC = 14,
	WORLD_TRABLE_SIGN = 15,
	WORLD_TRABLE = 16,
	REVENUE_SIGN = 17,
	BUY_LAND_SYNC = 18,
	BUY_BUILDING_SYNC = 19,
	PAY_TOLL_SIGN_SYNC = 20,
	TAKE_OVER_SYNC = 21,
	BUY_LANDMARK_SIGN_SYNC = 22,
	FINISH_THIS_TURN_PROCESS = 25,
	SET_MATCHING_USER_PACKET = 100,
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