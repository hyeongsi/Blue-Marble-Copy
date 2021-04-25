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

	if (dwElapsedTicks < dwInterval)//����� �ð��� ���� 1��ȸ�� ���� �Ǿ�� �� �ð� ���� ���� ���, �ٽ� ���� FPS�� ������ ������ �� ��� �̸� ������ FPS ũ��� �����.
	{
		Sleep((DWORD)(dwInterval - dwElapsedTicks));
		dwElapsedTicks = 0;//����� �ð� ��ŭ ��� �����Ƿ� ����� �ð��� ��ȿȭ �Ѵ�.
	}
	else//FPS�� ���� �Ǿ� ���� �ð��� Ŀ������, �������� ���� ó���� �ʾ��� �����͵��� Sleep() �Լ� ȣ���� ��ŵ�ϴ� ������� ��� �ð� ���� ������ ó���Ѵ�.
	{
		dwElapsedTicks -= dwInterval;//������ �� �� ��ȸ �� ������ ������ ��� �ð����� ���� 1��ȸ�� �ҿ�Ǿ�� �ϴ� �ð��� �����Ѵ�.
	}

	dwLastTicks = GetTickCount64();//������ 1��ȸ ���� ������ ���

	updateCBF();
}