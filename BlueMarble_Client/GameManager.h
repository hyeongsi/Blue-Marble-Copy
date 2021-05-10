#pragma once
#include <string>
#include <vector>

using namespace std;

constexpr const int DIRECTION = 4;	// 남,서,북,동
constexpr const int NAME_SIZE = 50;
constexpr const int MAX_PLAYER = 4;

typedef struct BoardData
{
	int mapSize = 0;
	vector<int> code;
	vector<string> name;
} boardData;

class GameManager
{
private:
	static GameManager* instance;

	boardData board;
	int playerCount = 0;

	GameManager();
	~GameManager();
public:
	static GameManager* GetInstance();
	static void ReleaseInstance();

	void Init();
	void SetPlayerCount(int count);
	int GetPlayerCount();

	boardData* GetBoardDataAddress();
	boardData GetBoardData();
};

