#pragma once
#include <Windows.h>
#include <vector>

class RenderManager;
class BitmapManager;
class MainWindow
{
private:
	static MainWindow* instance;

	HWND hWnd = nullptr;
	HINSTANCE hInst = nullptr;
	std::vector<HWND> hwndWindow;

	RenderManager* renderManager = nullptr;
	BitmapManager* bitmapManager = nullptr;

	MainWindow();
	~MainWindow();

	void InitMainMenu(HWND hWnd);
	void ResizeWindow(const LONG width, const LONG height, const POINT printPoint);	// 해상도 변경
	void CreateButton();
	void ShowButton();	// 버튼 출력
	void HideButton();	// 버튼 숨기기
	static void StartGameMethod();	// 시작 버튼 작동
	void StartGame();
	static void GetRankingDataMethod();	// 랭킹 정보 요청
	void GetRankingData();
public:
	static MainWindow* GetInstance();
	static void ReleaseInstance();

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static void MainMenuUpdate();	// MainMenu Update
};

