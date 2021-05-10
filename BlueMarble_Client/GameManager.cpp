#include "GameManager.h"

GameManager* GameManager::instance = nullptr;

GameManager::GameManager() {}
GameManager::~GameManager() {}

GameManager* GameManager::GetInstance()
{
	if (nullptr == instance)
	{
		instance = new GameManager();
	}

	return instance;
}

void GameManager::ReleaseInstance()
{
	delete instance;
	instance = nullptr;
}

void GameManager::Init()
{
	boardData board;

	this->board = board;
	playerCount = 0;
}

int GameManager::GetPlayerCount()
{
	return playerCount;
}

void GameManager::SetPlayerCount(int count)
{
	playerCount = count;
}

boardData* GameManager::GetBoardDataAddress()
{
	return &board;
}

boardData GameManager::GetBoardData()
{
	return board;
}
