#pragma once
#include <Windows.h>
#include "commonResource.h"
#include "GameManager.h"

class RenderManager
{
private:
	static RenderManager* instance;

	HDC hdc{ nullptr };
	HDC memDC{ nullptr };
	HDC backMemDC{ nullptr };
	HBITMAP oldCreateBitmap = nullptr;
	SIZE ClientSize = { 1280,720 };
	const POINT LEFT_TOP_PRINT_POINT = { 240,130 };
	const POINT RIGHT_BOTTOM_PRINT_POINT = { 1040,590 };

	RenderManager();
	~RenderManager();
public:
	static RenderManager* GetInstance();
	static void ReleaseInstance();

	SIZE* GetClientSize();
	void Init(HWND hWnd);
	void RenderInitSetting();

	void DrawBoardMap(boardData board);
	void DrawWindow(State state);
	void DrawHwnd(const HWND dHwnd, const POINT printPoint, const SIZE hwndSize);
	void DrawBitmap(const HBITMAP bitmap, const POINT printPoint);
	void Render();
};

