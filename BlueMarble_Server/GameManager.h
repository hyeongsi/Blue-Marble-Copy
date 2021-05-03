#pragma once
#include "TransferResource.h"

class GameManager
{
private:
	static GameManager* instance;

	GameManager();
	~GameManager();
public:
	static GameManager* GetInstance();
	static void ReleaseInstance();


};

