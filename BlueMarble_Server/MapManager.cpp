#include "MapManager.h"
#include <fstream>

MapManager* MapManager::instance = nullptr;

MapManager::MapManager() {}
MapManager::~MapManager() {}

MapManager* MapManager::GetInstance()
{
	if (nullptr == instance)
	{
		instance = new MapManager();
	}

	return instance;
}

void MapManager::ReleaseInstance()
{
	delete instance;
	instance = nullptr;
}

void MapManager::LoadMapData()
{
	const char* mapFilePath = "mapData/original.txt";
	const int DIRECTION = 4;	// ��,��,��,��

	ifstream readFile;
	boardData board;
	int mapSize = 0;
	int code = 0;
	string name = "";

	readFile.open(mapFilePath);
	if (readFile.is_open())
	{
		while (!readFile.eof())
		{
			readFile >> mapSize;	// �� ������

			board.mapSize = mapSize;
			board.code = new int[mapSize];
			board.name = new string[mapSize];	// �����ŭ ���� Ȯ��

			for (int i = 0; i < mapSize * DIRECTION; i++)
			{
				readFile >> code;
				readFile >> name;
			}
		}
	}

	boardDataVector.emplace_back(board);

	readFile.close();
}
