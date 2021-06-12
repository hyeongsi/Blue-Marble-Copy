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

enum GameBitmap
{
	Player_1 = 0,
	Player_2 = 1,
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