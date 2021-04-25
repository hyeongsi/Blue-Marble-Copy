#pragma once
#include <Windows.h>

class RenderManager
{
private:
	static RenderManager* instance;

	HDC hdc{ nullptr };
	HDC memDC{ nullptr };
	HDC backMemDC{ nullptr };
	HBITMAP oldCreateBitmap = nullptr;
	SIZE ClientSize = { 1280,720 };

	RenderManager();
	~RenderManager();
public:
	static RenderManager* GetInstance();
	static void ReleaseInstance();

	SIZE* GetClientSize();
	void Init(HWND hWnd);
	void RenderInitSetting();

	void DrawMainMenu();
	void DrawBitmap(HBITMAP bitmap, POINT printPoint);
	void Render();
};

