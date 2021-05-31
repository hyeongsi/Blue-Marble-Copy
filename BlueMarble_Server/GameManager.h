#pragma once
#include <vector>
#include <WinSock2.h>
#include "commonGameResource.h"

enum InputKey
{
	NONE = 0,
	INPUT_SPACE = 1,
	INPUT_LEFT = 2,
	INPUT_RIGHT = 3,
};

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

	void ArriveLandTileMethod(GameRoom* room);

	static UINT WINAPI RoomLogicThread(void* arg);
	void RoomLogicThreadMethod(GameRoom* room);

	static void RollTheDiceMethod(GameRoom* room);
	void RollTheDice(GameRoom* room);

	static void BuyLandMethod(GameRoom* room, char* data);
	void BuyLand(GameRoom* room, char* data);

	static void BuyBuildingMethod(GameRoom* room, char* data);
	void BuyBuilding(GameRoom* room, char* data);

	static void PayTollMethod(GameRoom* room, char* data);
	void PayToll(GameRoom* room, char* data);

	static void TakeOverMethod(GameRoom* room, char* data);
	void TakeOver(GameRoom* room, char* data);

	static void BuyLandMarkMethod(GameRoom* room, char* data);
	void BuyLandMark(GameRoom* room, char* data);

	static void SelectInputKeyProcessMethod(GameRoom* room, char* data);	// 선택모드 입력키 처리 후 값 전송
	void SelectInputKeyProcess(GameRoom* room, char* data);

	static void SellLandProcessMethod(GameRoom* room, char* data);
	void SellLandProcess(GameRoom* room, char* data);

	static void AfterSellLandSyncMethod(GameRoom* room);
	void AfterSellLandSync(GameRoom* room);
};

