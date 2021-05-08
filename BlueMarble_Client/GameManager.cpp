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

boardData* GameManager::GetBoardDataAddress()
{
	return &board;
}

boardData GameManager::GetBoardData()
{
	return board;
}
