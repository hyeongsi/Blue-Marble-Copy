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
	vector<double> land;
	vector<double> villa;
	vector<double> building;
	vector<double> hotel;
	vector<double> landMark;
} boardData;

enum MapData
{
	ORIGINAL = 0,
};

enum TileCode
{
	START_TILE = 0,
	LAND_TILE = 1,
	TOUR_TILE = 2,
	CARD_TILE = 3,
	DESERT_ISLAND_TILE = 4,
	OLYMPIC_TILE = 5,
	WORLD_TRABLE_TILE = 6,
	REVENUE_TILE = 7,
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

