#pragma once
#include <string>
#include <vector>
#include <map>

using namespace std;

constexpr const int DIRECTION = 4;	// 남,서,북,동
constexpr const int NAME_SIZE = 50;
constexpr const int MAX_PLAYER = 4;

typedef struct BoardData
{
	int mapSize = 0;
	vector<int> code;
	vector<string> name;
	vector<int> owner;
} boardData;

typedef struct BuildData
{
	map<int, bool> villa;
	map<int, bool> building;
	map<int, bool> hotel;
	map<int, bool> landMark;
} buildData;

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
	buildData boardBuildData;
	GameState state = GameState::WAIT;

	vector<int> userPositionVector;	// 유저 위치
	vector<int> userMoneyVector;	// 유저 돈 텍스트

	bool isMyDiceTurn = false;
	int playerCount = 0;	// 총 플레이 유저 수
	int myCharacterIndex = 0;	// 본인 캐릭터 인덱스 (1부터 시작)

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
	vector<int>* GetUserMoneyVector();

	void SetIsMyDiceTurn(bool turn);
	bool GetIsMyDiceTurn();

	void SetPlayerCount(int count);
	int GetPlayerCount();

	void SetCharacterIndex(int index);
	int GetCharacterIndex();

	void SetGameMessage(string msg);
	string GetGameMessage();

	boardData* GetAddressBoardData();
	boardData GetBoardData();

	buildData* GetAddressBoardBuildData();
	buildData GetBoarBuildData();

	void MoveUserPosition(int userIndex, int diceValue);
};

