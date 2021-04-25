#include "BitmapManager.h"

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

void BitmapManager::LoadMainMenuBitmap(HINSTANCE hInst)
{
	mainMenuBitmap.emplace_back(
		(HBITMAP)LoadImageA(NULL, "sprites/blueMarbleLogo.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION));
}

HBITMAP* BitmapManager::GetBitmap(State state, int index)
{
	switch (state)
	{
	case State::MAIN_MENU:
		if(0 <= index && index < (int)mainMenuBitmap.size())
			return &mainMenuBitmap[index];

		return nullptr;
	case State::RANK_MENU:
		return nullptr;
	case State::GAME:
		return nullptr;
	default:
		return nullptr;
	}
}
