#pragma once
#include <vector>
#include "SocketTransfer.h"

class RenderManager;
class BitmapManager;
class MainWindow
{
private:
	static MainWindow* instance;
	static bool isRunRankingThread;
	static bool isRunStartThread;

	std::vector<HWND> hwndWindow;

	RenderManager* renderManager = nullptr;
	BitmapManager* bitmapManager = nullptr;

	MainWindow();
	~MainWindow();

	void InitMainMenu(HWND hWnd);
	void ReInitMainMenu(HWND hWnd);
	void ResizeWindow(const LONG width, const LONG height, const POINT printPoint, HWND hWnd);	// 해상도 변경
	void CreateButton(HWND hWnd);
	void ShowButton();	// 버튼 출력
	void HideButton();	// 버튼 숨기기
	static void StartGameMethod();	// 시작 버튼 작동
	static UINT WINAPI StartGame(void* arg);
	static void GetRankingDataMethod();	// 랭킹 정보 요청
	static UINT WINAPI GetRankingData(void* arg);
public:
	static MainWindow* GetInstance();
	static void ReleaseInstance();

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static void MainMenuUpdate();	// MainMenu Update
};

