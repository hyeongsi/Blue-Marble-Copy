#include "MainSystem.h"

MainSystem* MainSystem::instance = nullptr;

MainSystem::MainSystem()
{
	dwLastTicks = GetTickCount64();
}
MainSystem::~MainSystem(){}

MainSystem* MainSystem::GetInstance()
{
	if (nullptr == instance)
	{
		instance = new MainSystem();
	}

	return instance;
}
void MainSystem::ReleaseInstance()
{
	delete instance;
	instance = nullptr;
}

void MainSystem::RegistUpdateCallbackFunction(CALLBACK_FUNC cbf)
{
	updateCBF = cbf;
}

void MainSystem::Update()
{
	if (nullptr == updateCBF)
		return;

	dwElapsedTicks += (GetTickCount64() - dwLastTicks);

	if (dwElapsedTicks < dwInterval)//경과된 시간이 루프 1순회당 지연 되어야 할 시간 보다 작은 경우, 다시 말해 FPS가 빠르게 나오게 될 경우 이를 정해진 FPS 크기로 낮춘다.
	{
		Sleep((DWORD)(dwInterval - dwElapsedTicks));
		dwElapsedTicks = 0;//경과된 시간 만큼 대기 했으므로 경과된 시간을 무효화 한다.
	}
	else//FPS가 저하 되어 지연 시간이 커졌을때, 지연으로 인해 처리가 늦어진 데이터들을 Sleep() 함수 호출을 스킵하는 방식으로 대기 시간 없이 빠르게 처리한다.
	{
		dwElapsedTicks -= dwInterval;//루프를 한 번 순회 할 때마다 기존의 경과 시간에서 루프 1순회당 소요되어야 하는 시간을 차감한다.
	}

	dwLastTicks = GetTickCount64();//루프문 1순회 종료 시점을 기록

	updateCBF();
}