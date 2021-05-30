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
		instance->keyInputDelayTime = clock();
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

void GameManager::SetSelectMapMode(bool isMyTurn, int goalPrice)
{
	bool isHaveLand = false;

	this->goalPrice = goalPrice;
	isSelectTurn = isMyTurn;

	RenderManager::GetInstance()->isSelectMapMode = true;	// 선택모드 활성화

	for (int i = 0; i < (int)board.code.size(); i++)
	{
		if (board.owner[i] == myCharacterIndex)
		{
			isHaveLand = true;
			RenderManager::GetInstance()->selectPosition = i;	// 초기 선택 땅 초기화
			break;
		}
	}

	if(!isHaveLand)
		RenderManager::GetInstance()->selectPosition = -1; // 초기 선택 땅 초기화
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

int GameManager::SelectModeInputKey()
{
	if (!RenderManager::GetInstance()->isSelectMapMode)
		return NONE;

	if ((clock() - keyInputDelayTime) >= 200) // 딜레이 0.2초
	{
		if (GetAsyncKeyState(VK_SPACE) & 0x8000)	// 스페이스바
		{
			keyInputDelayTime = clock();
			return INPUT_SPACE;
		}
		else if (GetAsyncKeyState(VK_LEFT) & 0x8000)	// 왼쪽 방향키
		{
			keyInputDelayTime = clock();
			return INPUT_LEFT;
		}
		else if (GetAsyncKeyState(VK_RIGHT) & 0x8000)	// 오른쪽 방향키
		{
			keyInputDelayTime = clock();
			return INPUT_RIGHT;
		}
	}

	return NONE;
}