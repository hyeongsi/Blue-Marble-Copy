#pragma once
#include <vector>
#include <WinSock2.h>

class GameRoom;
class GameManager
{
private:
	static GameManager* instance;
	std::vector<GameRoom*> roomVector;

	GameManager();
	~GameManager();
public:
	static GameManager* GetInstance();
	static void ReleaseInstance();

	void CreateRoom(SOCKET& user1, SOCKET& user2);
	GameRoom* GetRoom(int index);
	int FindBelongRoom(SOCKET& socket);	// 속한 게임방 찾기

	static UINT WINAPI RoomLogicThread(void* arg);
	void RoomLogicThreadMethod(GameRoom* room);

	static void RollTheDiceMethod(GameRoom* room);
	void RollTheDice(GameRoom* room);
};

