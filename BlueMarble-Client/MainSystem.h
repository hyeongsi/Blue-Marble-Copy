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

	float dwElapsedTicks = 0; //���� ��ȸ�� ����� �ð�
	ULONGLONG dwLastTicks = 0;	//���� ���� ��ȸ�� ���� �ð�
	ULONGLONG dwInterval = 1000 / FPS; //���� ��ȸ�� �ҿ� �Ǿ�� �ϴ� �ð�

	MainSystem();
	~MainSystem();
public:
	static MainSystem* GetInstance();
	static void ReleaseInstance();

	void RegistUpdateCallbackFunction(CALLBACK_FUNC cbf);
	void Update();
};

