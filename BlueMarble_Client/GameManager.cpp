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

void GameManager::SetBoardData(boardData board)
{
	this->board.code = new int[board.mapSize * DIRECTION];
	this->board.name = new char* [board.mapSize * DIRECTION];

	for (int i = 0; i < board.mapSize * DIRECTION; i++)
	{
		this->board.name[i] = new char[NAME_SIZE];
	}

	this->board = board;

	int test = this->board.code[0];
}

boardData* GameManager::GetBoardData()
{
	return &board;
}
