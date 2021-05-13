﻿#pragma once
#include <string>
#include <vector>

using namespace std;

constexpr const int DIRECTION = 4;	// 남,서,북,동
constexpr const int NAME_SIZE = 50;
constexpr const int MAX_PLAYER = 4;

typedef struct BoardData
{
	int mapSize = 0;
	vector<int> code;
	vector<string> name;
} boardData;

enum class GameState
{
	WAIT = 0,
	ROLL_DICE = 1,
	BUY_LAND = 2,
	SELL_LAND = 3,
};

class GameManager
{
private:
	static GameManager* instance;

	boardData board;
	GameState state = GameState::WAIT;

	vector<int> userPositionVector;	// 유저 위치

	bool isMyTurn = false;
	int playerCount = 0;	// 총 플레이 유저 수

	string gameMessage = "매칭중";

	GameManager();
	~GameManager();
public:
	static GameManager* GetInstance();
	static void ReleaseInstance();

	void Init();

	void SetGameState(GameState state);
	GameState GetGameState();

	vector<int>* GetUserPositionVector();

	void SetIsMyTurn(bool turn);
	bool GetIsMyTurn();

	void SetPlayerCount(int count);
	int GetPlayerCount();

	void SetGameMessage(string msg);
	string GetGameMessage();

	boardData* GetAddressBoardData();
	boardData GetBoardData();

	void MoveUserPosition(int userIndex, int diceValue);
};

