#pragma once
#include <string>

using namespace std;

constexpr const int DIRECTION = 4;	// ³²,¼­,ºÏ,µ¿
constexpr const int NAME_SIZE = 50;

typedef struct BoardData
{
	int mapSize = 0;
	int* code = nullptr;
	char** name = nullptr;
} boardData;

class GameManager
{
private:
	static GameManager* instance;

	boardData board;

	GameManager();
	~GameManager();
public:
	static GameManager* GetInstance();
	static void ReleaseInstance();

	void SetBoardData(boardData board);
	boardData* GetBoardData();
};

