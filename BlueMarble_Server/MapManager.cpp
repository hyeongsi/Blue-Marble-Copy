#include "MapManager.h"
#include <fstream>
#include <string>

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

	ifstream readFile;
	boardData board;
	int mapSize = 0;

	readFile.open(mapFilePath);
	if (readFile.is_open())
	{
		while (!readFile.eof())
		{
			readFile >> mapSize;	// ∏  ªÁ¿Ã¡Ó

			board.mapSize = mapSize;

			for (int i = 0; i < mapSize * DIRECTION; i++)
			{
				int code;
				string name;

				readFile >> name;
				readFile >> code;
				board.name.emplace_back(name);
				board.code.emplace_back(code);
			}
		}
	}

	boardDataVector.emplace_back(board);

	readFile.close();
}

boardData* MapManager::GetBoardData(const int index)
{
	if (index >= (int)boardDataVector.size() || 0 > index)
		return nullptr;

	return &boardDataVector[index];
}
