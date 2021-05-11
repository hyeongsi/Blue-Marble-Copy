#pragma once
#include "TransferResource.h"
#include <list>

constexpr const int MAX_PLAYER = 2;

class GameRoom
{
private:
	list<SOCKET> userList;
	
	unsigned int takeControlPlayer = 0;	// 누구 차례인지 구분 변수

public:
	int connectPlayer = 0;

	GameRoom(SOCKET user1, SOCKET user2);

	list<SOCKET> GetUserList();
	void NextTurn();	// 차례 넘겨주기
	int GetTakeControlPlayer();	// 누구 차례인지 값 전송

	void SendMapDataMethod(SOCKET& socekt);
};

