#pragma once
#include <string>
#include <vector>

using namespace std;

typedef struct BoardData
{
	int mapSize = 0;
	int* code = nullptr;
	string* name = nullptr;
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
};

