#pragma once
#include <Windows.h>

typedef void(*CALLBACK_FUNC)(void);

enum class State
{
	MAIN_MENU = 0,
	RANK_MENU = 1,
	GAME = 2,
};

constexpr const ULONGLONG FPS = 60;

class MainSystem
{
private:
	static MainSystem* instance;
	CALLBACK_FUNC updateCBF = nullptr;

	float dwElapsedTicks = 0; //루프 순회간 경과된 시간
	ULONGLONG dwLastTicks = 0;	//이전 루프 순회가 끝난 시간
	ULONGLONG dwInterval = 1000 / FPS; //루프 순회당 소요 되어야 하는 시간

	MainSystem();
	~MainSystem();
public:
	static MainSystem* GetInstance();
	static void ReleaseInstance();

	void RegistUpdateCallbackFunction(CALLBACK_FUNC cbf);
	void Update();
};

