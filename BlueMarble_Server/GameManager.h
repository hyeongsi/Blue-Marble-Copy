#pragma once
#include <vector>
#include <WinSock2.h>
#include "commonGameResource.h"
#include <mutex>

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
	std::mutex gameRoomVectorMutex;

	static GameManager* GetInstance();
	static void ReleaseInstance();

	void CreateRoom(SOCKET& user1, SOCKET& user2);
	GameRoom* GetRoom(int index);
	void DeleteGameRoom(GameRoom* room);

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

	static void GetCardSignSyncMethod(GameRoom* room);
	void GetCardSignSync(GameRoom* room);

	static void GetTrapCardSyncMethod(GameRoom* room);
	void GetTrapCardSync(GameRoom* room);

	static void GetSelectIndexMethod(GameRoom* room, char* data, char header);
	void GetSelectIndex(GameRoom* room, char* data, char header);

	static void RevenueSignMethod(GameRoom* room);
	void RevenueSign(GameRoom* room);

	static void SelectInputKeyProcessMethod(GameRoom* room, char* data);	// 선택모드 입력키 처리 후 값 전송
	void SelectInputKeyProcess(GameRoom* room, char* data);

	static void SellLandProcessMethod(GameRoom* room, char* data);
	void SellLandProcess(GameRoom* room, char* data);

	static void GetBankruptcySignMethod(GameRoom* room);
	void GetBankruptcySign(GameRoom* room);

	static void AfterSellLandSyncMethod(GameRoom* room);
	void AfterSellLandSync(GameRoom* room);
};

