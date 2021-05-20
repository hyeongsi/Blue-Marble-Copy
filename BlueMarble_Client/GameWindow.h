#pragma once
#include "SocketTransfer.h"

class RenderManager;
class BitmapManager;
class GameWindow
{
private:
	static GameWindow* instance;
	std::vector<HWND> hwndWindow;

	RenderManager* renderManager = nullptr;
	BitmapManager* bitmapManager = nullptr;

	GameWindow();
	~GameWindow();

	void InitClass(HWND hWnd);
	void ReInitGame(HWND hWnd);
	void ResizeWindow(const LONG width, const LONG height, const POINT printPoint, HWND hWnd);
	void CreateButton(HWND hWnd);

	static void SendDiceTriggerMsgMethod();
	void SendDiceTriggerMsg();
public:
	HWND g_hWnd = nullptr;

	static GameWindow* GetInstance();
	static void ReleaseInstance();

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static void GameUpdate();	// Game Update

	void ShowButton();
	void HideButton();
};

