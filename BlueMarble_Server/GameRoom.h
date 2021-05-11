#pragma once
#include "TransferResource.h"
#include <list>

constexpr const int MAX_PLAYER = 2;

class GameRoom
{
private:
	list<SOCKET> userList;
	
	unsigned int takeControlPlayer = 0;	// ���� �������� ���� ����

public:
	int connectPlayer = 0;

	GameRoom(SOCKET user1, SOCKET user2);

	list<SOCKET> GetUserList();
	void NextTurn();	// ���� �Ѱ��ֱ�
	int GetTakeControlPlayer();	// ���� �������� �� ����

	void SendMapDataMethod(SOCKET& socekt);
};

