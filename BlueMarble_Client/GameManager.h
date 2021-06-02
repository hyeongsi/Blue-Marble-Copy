#pragma once
#include <string>
#include <vector>
#include <map>
#include <time.h>

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
	map<int, int> olympic;
} buildData;

enum class GameState
{
	WAIT = 0,
	ROLL_DICE = 1,
	BUY_LAND = 2,
	SELL_LAND = 3,
	SELECT_MODE = 4,
};

enum InputKey
{
	NONE = 0,
	INPUT_ENTER = 1,
	INPUT_LEFT = 2,
	INPUT_RIGHT = 3,
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
	vector<bool> backruptcyVector;	// 유저 파산 유무

	vector<string> cardMessageVector;	// 카드 메시지

	bool isMyDiceTurn = false;
	int playerCount = 0;	// 총 플레이 유저 수
	int myCharacterIndex = 0;	// 본인 캐릭터 인덱스 (1부터 시작)

	string gameMessage = "매칭중";

	GameManager();
	~GameManager();
public:
	int goalPrice = 0;
	bool isSelectTurn = false;
	long keyInputDelayTime = 0;
	vector<int> selectLandIndex;
	int totalSelectLandSellPrice = 0;

	static GameManager* GetInstance();
	static void ReleaseInstance();

	void LoadCardMessage();
	void Init();

	void SetGameState(GameState state);
	GameState GetGameState();

	vector<int>* GetUserPositionVector();
	vector<int>* GetUserMoneyVector();
	vector<bool>* GetBackruptcyVector();
	vector<string> GetCardMsgVector();

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

	string replaceAll(const string& str, const string& pattern, const string& replace);

	void SetSelectMapMode(bool isMyTurn, int goalPrice);

	void MoveUserPosition(int userIndex, int diceValue);

	int SelectModeInputKey();
};

