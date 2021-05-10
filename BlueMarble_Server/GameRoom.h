#pragma once
#include <WinSock2.h>
#include <list>

#pragma comment(lib, "ws2_32")

using namespace std;

class GameRoom
{
private:
	list<SOCKET> userList;

public:
	GameRoom(SOCKET user1, SOCKET user2);

	list<SOCKET> GetUserList();
};

