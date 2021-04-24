#include "MainSystem.h"

MainSystem* MainSystem::instance = nullptr;

MainSystem::MainSystem(){}
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

	updateCBF();
}