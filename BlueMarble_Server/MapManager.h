#pragma once
#include <string>
#include <vector>

using namespace std;

constexpr const int NAME_SIZE = 50;
constexpr const int DIRECTION = 4;	// 남,서,북,동

typedef struct BoardData
{
	unsigned int mapSize = 0;
	vector<int> code;
	vector<string> name;
} boardData;

enum MapData
{
	ORIGINAL = 0,
};

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

