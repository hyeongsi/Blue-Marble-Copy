#pragma once
#include <Windows.h>
#include "commonResource.h"
#include <mutex>

constexpr const ULONGLONG FPS = 60;

enum class WindowState
{
	MAIN_WINDOW = 0,
	GAME_WINDOW = 1,
};

class MainSystem
{
private:
	static MainSystem* instance;
	CALLBACK_FUNC updateCBF = nullptr;

	HWND mainWindowHwnd = nullptr;
	HWND gameWindowHwnd = nullptr;

	HINSTANCE g_hInst = nullptr;

	float dwElapsedTicks = 0; //루프 순회간 경과된 시간
	ULONGLONG dwLastTicks = 0;	//이전 루프 순회가 끝난 시간
	ULONGLONG dwInterval = 1000 / FPS; //루프 순회당 소요 되어야 하는 시간

	MainSystem();
	~MainSystem();
public:
	WindowState windowState = WindowState::MAIN_WINDOW;
	std::mutex windowStateMutex;

	static MainSystem* GetInstance();
	static void ReleaseInstance();

	void SetWindowHwnd(State state, HWND hwnd);
	HWND GetWindowHwnd(State state);

	void SetHinstance(HINSTANCE hInst);
	HINSTANCE GetHinstance();

	void RegistUpdateCallbackFunction(CALLBACK_FUNC cbf);
	void Update();
};

