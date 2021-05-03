#pragma once
#include <string>
#include <vector>

using namespace std;

constexpr const int NAME_SIZE = 50;
constexpr const int DIRECTION = 4;	// ³²,¼­,ºÏ,µ¿

typedef struct BoardData
{
	int mapSize = 0;
	int* code = nullptr;
	char** name = nullptr;
} boardData;

class MapManager
{
private:
	static MapManager* instance;
	vector<boardData> boardDataVector;

	MapManager();
	~MapManager();
public:
	static MapManager* GetInstance();
	static void ReleaseInstance();

	void LoadMapData();
	boardData* GetBoardData(const int index);
};

