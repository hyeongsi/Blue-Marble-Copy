#pragma once
#include <Windows.h>

class MainMenu
{
private:
	static MainMenu* instance;

	MainMenu();
	~MainMenu();
public:
	static MainMenu* GetInstance();
	static void ReleaseInstance();

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

