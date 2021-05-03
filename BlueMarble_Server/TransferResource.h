#pragma once
#include <WinSock2.h>
#pragma comment(lib, "ws2_32")

using namespace std;

constexpr const int PORT = 4567;
constexpr const int PACKET_SIZE = 2048;

typedef struct customPackets
{
	int header = -1;
	int dataSize = 0;
	char* data = nullptr;
	customPackets() {};
	customPackets(int _header, int _dataSize, char* _data) :
		header(_header), dataSize(_dataSize), data(_data) {};
} customPacket;

enum ErrorCode
{
	WSASTARTUP_ERROR = 100,
	BIND_ERROR = 101,
	LISTEN_ERROR = 102,
	NOT_FOUND_BOARDDATA_ERROR = 103,
	SEND_ERROR = 104,
};

enum MessageCode
{
	GET_MAPDATA = 1,

};