#pragma once
#include <Windows.h>

class GameWindow
{
private:
	static GameWindow* instance;

	GameWindow();
	~GameWindow();
public:
	static GameWindow* GetInstance();
	static void ReleaseInstance();

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

