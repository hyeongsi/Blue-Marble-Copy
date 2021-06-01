#pragma once
#include <string>
#include <vector>
#include <map>

using namespace std;

constexpr const int NAME_SIZE = 50;
constexpr const int DIRECTION = 4;	// 남,서,북,동

typedef struct BoardData
{
	unsigned int mapSize = 0;
	vector<int> code;
	vector<string> name;
	vector<int> land;
	vector<int> villa;
	vector<int> building;
	vector<int> hotel;
	vector<int> landMark;
	vector<int> tollLand;
	vector<int> tollVilla;
	vector<int> tollBuilding;
	vector<int> tollHotel;
	vector<int> tollLandMark;
} boardData;

typedef struct LandData
{
	map<int, int> land;
	map<int, int> villa;
	map<int, int> building;
	map<int, int> hotel;
	map<int, int> landMark;
	map<int, int> olympic;
} landData;

enum MapData
{
	ORIGINAL = 0,
};

enum TileCode
{
	START_TILE = 0,
	LAND_TILE = 1,			// 지역
	TOUR_TILE = 2,			// 휴양지
	CARD_TILE = 3,			// 카드
	DESERT_ISLAND_TILE = 4,	// 무인도
	OLYMPIC_TILE = 5,		// 올림픽
	WORLD_TRABLE_TILE = 6,	// 세계여행
	REVENUE_TILE = 7,		// 국세청
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

