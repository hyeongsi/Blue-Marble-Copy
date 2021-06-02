#include "GameManager.h"
#include "RenderManager.h"
#include <fstream>

GameManager* GameManager::instance = nullptr;

GameManager::GameManager() {}
GameManager::~GameManager() {}

GameManager* GameManager::GetInstance()
{
	if (nullptr == instance)
	{
		instance = new GameManager();
		instance->keyInputDelayTime = clock();
		instance->LoadCardMessage();
	}

	return instance;
}

void GameManager::ReleaseInstance()
{
	delete instance;
	instance = nullptr;
}

void GameManager::LoadCardMessage()
{
	const char* cardDataPath = "card/original.txt";

	ifstream readFile;
	int cardId;

	readFile.open(cardDataPath);
	if (readFile.is_open())
	{
		while (!readFile.eof())
		{
			char cardMessage[300];
			string cardMsgString;

			readFile >> cardId;		// 카드 ID
			readFile >> cardMessage;	// 지급,지불 금액

			cardMsgString = cardMessage;
			cardMsgString = replaceAll(cardMsgString, "-", "\n");
			cardMsgString = replaceAll(cardMsgString, "_", " ");

			cardMessageVector.emplace_back(cardMsgString);
		}
	}

	readFile.close();
}

void GameManager::Init()
{
	boardData board;
	state = GameState::WAIT;
	gameMessage = "매칭중";
	userPositionVector.clear();
	userMoneyVector.clear();
	backruptcyVector.clear();
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

vector<bool>* GameManager::GetBackruptcyVector()
{
	return &backruptcyVector;
}

vector<string> GameManager::GetCardMsgVector()
{
	return cardMessageVector;
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
	state = GameState::SELECT_MODE;

	RenderManager::GetInstance()->isSelectMapMode = SELL_LAND_MODE;	// 선택모드 활성화

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

string GameManager::replaceAll(const string& str, const string& pattern, const string& replace)
{
	string result = str;
	size_t pos = 0;
	size_t offset = 0;
	while ((pos = result.find(pattern, offset)) != string::npos)
	{
		result.replace(result.begin() + pos, result.begin() + pos + pattern.size(), replace);
		offset = pos + replace.size();
	}
	return result;
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
	if (RenderManager::GetInstance()->isSelectMapMode == IDLE_MODE)
		return NONE;

	if ((clock() - keyInputDelayTime) >= 200) // 딜레이 0.2초
	{
		if (GetAsyncKeyState(VK_RETURN) & 0x8000)	// 엔터
		{
			keyInputDelayTime = clock();
			return INPUT_ENTER;
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