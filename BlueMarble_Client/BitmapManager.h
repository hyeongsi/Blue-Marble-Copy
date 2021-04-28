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

class BitmapManager
{
private:
	static BitmapManager* instance;
	std::vector<BitmapInfo> mainMenuBitmap;
	std::vector<HwndInfo> mainMenuHwnd;

	std::vector<BitmapInfo> gameBitmap;
	std::vector<HwndInfo> gameHwnd;

	BitmapManager();
	~BitmapManager();
public:
	static BitmapManager* GetInstance();
	static void ReleaseInstance();

	void LoadHwndData(State state);
	void LoadBitmapData(State state);

	vector<HwndInfo>* GetHwnd(State state);
	vector<BitmapInfo>* GetBitmap(State state);
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