#pragma once

typedef void(*CALLBACK_FUNC)(void);

enum class State
{
	MAIN_MENU = 0,
	RANK_MENU = 1,
	GAME = 2,
};

class MainSystem
{
private:
	static MainSystem* instance;
	CALLBACK_FUNC updateCBF = nullptr;

	MainSystem();
	~MainSystem();
public:
	static MainSystem* GetInstance();
	static void ReleaseInstance();

	void RegistUpdateCallbackFunction(CALLBACK_FUNC cbf);
	void Update();
};

