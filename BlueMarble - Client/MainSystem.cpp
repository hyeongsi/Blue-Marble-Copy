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

void MainSystem::Update()
{

}