#pragma once
#include <Windows.h>
#include <vector>
#include "MainSystem.h"

class BitmapManager
{
private:
	static BitmapManager* instance;
	std::vector<HBITMAP> mainMenuBitmap;

	BitmapManager();
	~BitmapManager();
public:
	static BitmapManager* GetInstance();
	static void ReleaseInstance();

	void LoadMainMenuBitmap(HINSTANCE hInst);
	HBITMAP* GetBitmap(State state, int index);
};

enum MainMenuBitmap
{
	BACKGROUND = 0,
};
