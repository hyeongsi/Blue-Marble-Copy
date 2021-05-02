#pragma once
#include <WinSock2.h>
#pragma comment(lib, "ws2_32")

using namespace std;

constexpr const int PORT = 4567;
constexpr const int PACKET_SIZE = 1024;

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
	WSAStartupError = 100,
	BindError = 101,
	ListenError = 102,
};

enum MessageCode
{
	GET_MAPDATA = 1,

};