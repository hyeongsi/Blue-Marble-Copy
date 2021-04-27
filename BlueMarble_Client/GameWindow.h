#pragma once
#include <Windows.h>

class RenderManager;
class BitmapManager;
class GameWindow
{
private:
	static GameWindow* instance;
	HWND hWnd = nullptr;
	HINSTANCE hInst = nullptr;

	RenderManager* renderManager = nullptr;
	BitmapManager* bitmapManager = nullptr;

	GameWindow();
	~GameWindow();

	void InitMainMenu(HWND hWnd);
	void ResizeWindow(const LONG width, const LONG height, const POINT printPoint);
public:
	static GameWindow* GetInstance();
	static void ReleaseInstance();

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static void GameUpdate();	// Game Update
};

