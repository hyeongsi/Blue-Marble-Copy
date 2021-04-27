#include "BitmapManager.h"
#include <Windows.h>
#include <fstream>

BitmapManager* BitmapManager::instance = nullptr;

BitmapManager::BitmapManager() {}
BitmapManager::~BitmapManager() {}

BitmapManager* BitmapManager::GetInstance()
{
	if (nullptr == instance)
	{
		instance = new BitmapManager();
	}

	return instance;
}

void BitmapManager::ReleaseInstance()
{
	delete(instance);
	instance = nullptr;
}

void BitmapManager::LoadHwndData(State state)
{
	const char* mainMenuFilePath = "sprites/MainMenu/mainMenuHwnd.txt";
	const char* gameFilePath = "sprites/Game/gameHwnd.txt";

	ifstream readFile;
	HwndInfo hwndInfo;

	switch (state)
	{
	case State::MAIN_MENU:
		readFile.open(mainMenuFilePath);
		break;
	case State::RANK_MENU:
		break;
	case State::GAME:
		readFile.open(gameFilePath);
		break;
	default:
		return;
	}

	if (readFile.is_open())
	{
		while (!readFile.eof())
		{
			readFile >> hwndInfo.type;
			readFile >> hwndInfo.text;
			readFile >> hwndInfo.id;
			readFile >> hwndInfo.point.x;
			readFile >> hwndInfo.point.y;
			readFile >> hwndInfo.size.cx;
			readFile >> hwndInfo.size.cy;

			mainMenuHwnd.emplace_back(hwndInfo);
		}
	}

	readFile.close();
}

void BitmapManager::LoadBitmapData(State state)
{
	const char* mainMenuFilePath = "sprites/MainMenu/mainMenuSprites.txt";
	const char* gameFilePath = "sprites/Game/gameSprites.txt";

	ifstream readFile;
	string bitmapPath;
	BitmapInfo bitmapInfo;

	switch (state)
	{
	case State::MAIN_MENU:
		readFile.open(mainMenuFilePath);
		break;
	case State::RANK_MENU:
		break;
	case State::GAME:
		readFile.open(gameFilePath);
		break;
	default:
		return;
	}

	if (readFile.is_open())
	{
		while (!readFile.eof())
		{
			readFile >> bitmapPath;
			readFile >> bitmapInfo.point.x;
			readFile >> bitmapInfo.point.y;

			bitmapInfo.bitmap = (HBITMAP)LoadImageA(NULL, bitmapPath.c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
			mainMenuBitmap.emplace_back(bitmapInfo);
		}
	}

	readFile.close();
}

vector<HwndInfo>* BitmapManager::GetHwnd(State state)
{
	switch (state)
	{
	case State::MAIN_MENU:
		return &mainMenuHwnd;
	case State::RANK_MENU:
		return nullptr;
	case State::GAME:
		return &gameHwnd;
	default:
		return nullptr;
	}
}

vector<BitmapInfo>* BitmapManager::GetBitmap(State state)
{
	switch (state)
	{
	case State::MAIN_MENU:
		return &mainMenuBitmap;
	case State::RANK_MENU:
		return nullptr;
	case State::GAME:
		return &gameBitmap;
	default:
		return nullptr;
	}
}
