#include "GameRoom.h"
#include "GameServer.h"

GameRoom::GameRoom(SOCKET user1, SOCKET user2)
{
	userList.emplace_back(user1);
	userList.emplace_back(user2);
}

list<SOCKET> GameRoom::GetUserList()
{
	return userList;
}
