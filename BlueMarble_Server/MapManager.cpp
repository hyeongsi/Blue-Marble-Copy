#pragma once
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

	int code;
	string name;
	double landPrice = 0.0, villaPrice = 0.0, buildingPrice = 0.0, hotelPrice = 0.0, landMarkPrice = 0.0;

	readFile.open(mapFilePath);
	if (readFile.is_open())
	{
		while (!readFile.eof())
		{
			readFile >> mapSize;	// 맵 사이즈

			board.mapSize = mapSize;

			for (int i = 0; i < mapSize * DIRECTION; i++)
			{
				readFile >> name;
				readFile >> code;
				readFile >> landPrice;
				readFile >> villaPrice;
				readFile >> buildingPrice;
				readFile >> hotelPrice;
				readFile >> landMarkPrice;

				board.name.emplace_back(name);
				board.code.emplace_back(code);
				board.land.emplace_back(landPrice);
				board.villa.emplace_back(villaPrice);
				board.building.emplace_back(buildingPrice);
				board.hotel.emplace_back(hotelPrice);
				board.landMark.emplace_back(landMarkPrice);
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
