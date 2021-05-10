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
};

typedef struct MapPacket1
{
	char header;
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
	int roomIndex;
	int number;	// 몇번째 캐릭터인지 구분하기 위해 사용
	int playerCount;	// 총 플레이 유저 수
}readyPacket;