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
	SET_MATCHING_USER_PACKET = 100,
};