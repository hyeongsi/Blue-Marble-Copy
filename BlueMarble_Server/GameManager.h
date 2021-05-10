#pragma once
#include "TransferResource.h"
#include "GameRoom.h"
#include <vector>

class GameManager
{
private:
	static GameManager* instance;
	vector<GameRoom*> roomVector;

	GameManager();
	~GameManager();
public:
	static GameManager* GetInstance();
	static void ReleaseInstance();

	void CreateRoom(SOCKET& user1, SOCKET& user2);
	GameRoom* GetRoom(int index);
};

