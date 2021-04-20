#pragma once
class MainSystem
{
private:
	static MainSystem* instance;

	MainSystem();
	~MainSystem();
public:
	static MainSystem* GetInstance();
	static void ReleaseInstance();

	void Update();
};

