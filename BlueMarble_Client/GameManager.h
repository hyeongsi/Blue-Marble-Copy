#pragma once
#include <string>
#include <vector>

using namespace std;

constexpr const int DIRECTION = 4;	// ³²,¼­,ºÏ,µ¿
constexpr const int NAME_SIZE = 50;

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

	GameManager();
	~GameManager();
public:
	static GameManager* GetInstance();
	static void ReleaseInstance();

	void SetBoardData(boardData board);
	boardData GetBoardData();
};

