#pragma once
#include <Windows.h>
#include "commonResource.h"
#include "GameManager.h"

#pragma comment(lib, "msimg32.lib")

class RenderManager
{
private:
	static RenderManager* instance;

	HDC hdc{ nullptr };
	HDC memDC{ nullptr };
	HDC backMemDC{ nullptr };
	HBITMAP oldCreateBitmap = nullptr;
	SIZE ClientSize = { 1280,720 };
	const POINT LEFT_TOP_PRINT_POINT = { 240,60 };
	const POINT RIGHT_BOTTOM_PRINT_POINT = { 1040,640 };

	POINT PRINT_PLAYER_PIVOT_POINT[4];	// 캐릭터 출력 중심 좌표
	vector<POINT> playerBitmapPointVector;	// 캐릭터 타일 별 좌표

	vector<RECT> rectVector;
	RECT messageRect = { 550,350,750,450 };
	int tileWidth = 0, tileHeight = 0;

	RenderManager();
	~RenderManager();
public:
	static RenderManager* GetInstance();
	static void ReleaseInstance();

	SIZE* GetClientSize();
	void Init(HWND hWnd);
	void RenderInitSetting();

	void SetPlayerBitmapLocation(int playerIndex, int tileIndex);

	void InitDrawBoardMap();
	void DrawBoardMap();
	void DrawWindow(State state);
	void DrawHwnd(const HWND dHwnd, const POINT printPoint, const SIZE hwndSize);
	void DrawBitmap(const HBITMAP bitmap, const POINT printPoint, bool isTransparentBlt = false);
	void DrawGameMessage(string message);
	void Render();
};

