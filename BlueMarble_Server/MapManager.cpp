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
			readFile >> mapSize;	// 맵 사이즈

			board.mapSize = mapSize;
			board.name = new char*[mapSize* DIRECTION];	// 사이즈만큼 공간 확보

			for (int i = 0; i < mapSize * DIRECTION; i++)
			{
				board.name[i] = new char[NAME_SIZE];
			}
			board.code = new int[mapSize* DIRECTION];

			for (int i = 0; i < mapSize * DIRECTION; i++)
			{
				int code;
				string name;

				readFile >> name;
				readFile >> code;
				strcpy_s(board.name[i], NAME_SIZE, name.c_str());
				board.code[i] = code;
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
