#include "GameManager.h"
#include "RenderManager.h"

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
	state = GameState::WAIT;
	gameMessage = "매칭중";
	userPositionVector.clear();
	userMoneyVector.clear();
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

vector<int>* GameManager::GetUserPositionVector()
{
	return &userPositionVector;
}

vector<int>* GameManager::GetUserMoneyVector()
{
	return &userMoneyVector;
}

void GameManager::SetGameMessage(string msg)
{
	gameMessage = msg;
}

void GameManager::MoveUserPosition(int userIndex, int diceValue)
{
	userPositionVector[userIndex] += diceValue;

	if (userPositionVector[userIndex] >= (int)board.mapSize * DIRECTION)
	{
		userPositionVector[userIndex] -= ((int)board.mapSize * DIRECTION);
	}

	RenderManager::GetInstance()->SetPlayerBitmapLocation(userIndex, userPositionVector[userIndex]);
}

string GameManager::GetGameMessage()
{
	return gameMessage;
}

void GameManager::SetGameState(GameState state)
{
	this->state = state;
}

GameState GameManager::GetGameState()
{
	return state;
}

void GameManager::SetIsMyDiceTurn(bool turn)
{
	isMyDiceTurn = turn;
}

bool GameManager::GetIsMyDiceTurn()
{
	return isMyDiceTurn;
}

void GameManager::SetCharacterIndex(int index)
{
	myCharacterIndex = index;
}

int GameManager::GetCharacterIndex()
{
	return myCharacterIndex;
}

boardData* GameManager::GetAddressBoardData()
{
	return &board;
}

boardData GameManager::GetBoardData()
{
	return board;
}

buildData* GameManager::GetAddressBoardBuildData()
{
	return &boardBuildData;
}

buildData GameManager::GetBoarBuildData()
{
	return boardBuildData;
}