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
	int landPrice = 0, villaPrice = 0, buildingPrice = 0, hotelPrice = 0, landMarkPrice = 0;
	int tolllandPrice = 0, tollvillaPrice = 0, tollbuildingPrice = 0, tollhotelPrice = 0, tolllandMarkPrice = 0;

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

				readFile >> tolllandPrice;
				readFile >> tollvillaPrice;
				readFile >> tollbuildingPrice;
				readFile >> tollhotelPrice;
				readFile >> tolllandMarkPrice;

				board.name.emplace_back(name);
				board.code.emplace_back(code);
				board.land.emplace_back(landPrice);
				board.villa.emplace_back(villaPrice);
				board.building.emplace_back(buildingPrice);
				board.hotel.emplace_back(hotelPrice);
				board.landMark.emplace_back(landMarkPrice);

				board.tollLand.emplace_back(tolllandPrice);
				board.tollVilla.emplace_back(tollvillaPrice);
				board.tollBuilding.emplace_back(tollbuildingPrice);
				board.tollHotel.emplace_back(tollhotelPrice);
				board.tollLandMark.emplace_back(tolllandMarkPrice);
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