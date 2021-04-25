#pragma once
#include <Windows.h>
#include "RenderManager.h"

class MainMenu
{
private:
	static MainMenu* instance;
	HWND hWnd = nullptr;
	HINSTANCE hInst = nullptr;
	HWND hStartButton = nullptr;

	RenderManager* renderManager = nullptr;

	const POINT StartButtonPoint = { 100,100 };
	const SIZE StartButtonSize = { 100,50 };

	const POINT RankingButtonPoint = { 300,100 };
	const SIZE RankingButtonSize = { 100,50 };

	MainMenu();
	~MainMenu();

	void InitMainMenu(HWND hWnd);
	void ResizeWindow(const LONG width, const LONG height, const POINT printPoint);	// �ػ� ����
	void CreateButton();
	void ShowButton();	// ��ư ���
	void HideButton();	// ��ư �����
	static void StartGameMethod();	// ���� ��ư �۵�
	void StartGame();
	static void GetRankingDataMethod();	// ��ŷ ���� ��û
	void GetRankingData();
public:
	static MainMenu* GetInstance();
	static void ReleaseInstance();

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static void MainMenuUpdate();	// MainMenu Update
};

