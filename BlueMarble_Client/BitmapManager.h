#pragma once
#include <Windows.h>
#include <vector>
#include <string>
#include "commonResource.h"
#include "MainSystem.h"

using namespace std;

struct BitmapInfo
{
	HBITMAP bitmap{ NULL };
	POINT point{ 0,0 };
	BitmapInfo() {}
	BitmapInfo(HBITMAP _bitmap, POINT _point) :
		bitmap(_bitmap), point(_point) {}
};

struct HwndInfo
{
	string type{""};
	string text{""};
	int id{0};
	POINT point{ 0,0 };
	SIZE size{ 0,0 };
	HwndInfo(){}
	HwndInfo(string _type, string _text, int _id, POINT _point, SIZE _size) :
		type(_type), text(_text), id(_id), point(_point), size(_size) {}
};

struct AnimationBitmapInfo
{
	HBITMAP bitmap{ NULL };
	POINT point{ 0,0 };
	int row = 0;
	int col = 0;
	SIZE size{ 0,0 };
	AnimationBitmapInfo() {}
	AnimationBitmapInfo(HBITMAP _bitmap, POINT _point, int _row, int _col, SIZE _size) :
		bitmap(_bitmap), point(_point), row(_row), col(_col), size(_size) {}
};

class BitmapManager
{
private:
	static BitmapManager* instance;
	std::vector<BitmapInfo> mainMenuBitmap;
	std::vector<BitmapInfo> mainMenuButtonBitmap;
	std::vector<HwndInfo> mainMenuHwnd;

	std::vector<BitmapInfo> gameBitmap;
	std::vector<BitmapInfo> gameButtonBitmap;
	std::vector<HwndInfo> gameHwnd;
	std::vector<AnimationBitmapInfo> gameAnimationBitmap;

	BitmapManager();
	~BitmapManager();
public:
	static BitmapManager* GetInstance();
	static void ReleaseInstance();

	void LoadHwndData(State state);
	void LoadBitmapData(State state);
	void LoadButtonBitmapData(State state);
	void LoadAnimationBitmapData(State state);

	vector<HwndInfo>* GetHwnd(State state);
	vector<BitmapInfo>* GetBitmap(State state);
	vector<BitmapInfo>* GetButtonBitmap(State state);
	vector<AnimationBitmapInfo>* GetAnimationBitmap(State state);
};

enum MainMenuBitmap
{
	MAIN_MENU_BACKGROUND = 0,
};

enum MainMenuHwnd
{
	START_BUTTON = 0,
	RANKING_BUTTON = 1,
};

enum GameBitmap
{
	GAME_WINDOW_BACKGROUND = 0,
	START_TILE_BACKGROUND = 1,
	DESERT_ISLAND_TILE_BACKGROUND = 2,
	OLYMPIC_TILE_BACKGROUND = 3,
	AIRPLAIN_TILE_BACKGROUND = 4,
	REVENUE_TILE_BACKGROUND = 5,
	CARD1_TILE_BACKGROUND = 6,
	CARD2_TILE_BACKGROUND = 7,
	CARD3_TILE_BACKGROUND = 8,
	CARD4_TILE_BACKGROUND = 9,
	PLAYER1_PIECE = 10,
	PLAYER2_PIECE = 11,
	PLAYER3_PIECE = 12,
	PLAYER4_PIECE = 13,
	PLAYER1_MONEY_UI = 14,
	PLAYER2_MONEY_UI = 15,
	PLAYER3_MONEY_UI = 16,
	PLAYER4_MONEY_UI = 17,
	PLAYER1_CHECK_UI = 18,
	PLAYER2_CHECK_UI = 19,
	PLAYER3_CHECK_UI = 20,
	PLAYER4_CHECK_UI = 21,
	PLAYER1_TURN_UI = 22,
	PLAYER2_TURN_UI = 23,
	PLAYER3_TURN_UI = 24,
	PLAYER4_TURN_UI = 25,
	PLAYER1_ME_UI = 26,
	PLAYER2_ME_UI = 27,
	PLAYER3_ME_UI = 28,
	PLAYER4_ME_UI = 29,
	BLACKBOARD_UI = 30,
};