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

void GameManager::SetPlayerCount(int count)
{
	playerCount = count;
}

int GameManager::GetPlayerCount()
{
	return playerCount;
}

void GameManager::SetGameState(GameState state)
{
	this->state = state;
}

GameState GameManager::GetGameState()
{
	return state;
}

void GameManager::SetIsMyTurn(bool turn)
{
	isMyTurn = turn;
}

bool GameManager::GetIsMyTurn()
{
	return isMyTurn;
}

boardData* GameManager::GetAddressBoardData()
{
	return &board;
}

boardData GameManager::GetBoardData()
{
	return board;
}
