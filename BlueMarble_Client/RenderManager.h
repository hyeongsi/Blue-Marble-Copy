#pragma once
#include <Windows.h>
#include "commonResource.h"
#include "GameManager.h"

#pragma comment(lib, "msimg32.lib")

enum DrawDir
{
	SOUTH = 0,
	WEST = 1,
	NORTH = 2,
	EAST = 3,
};

enum SelectBtnNumber
{
	ROLL_DICE_BTN_NUMBER,
	SELECT_MODE_OK_BTN_NUMBER,
	SELECT_MODE_CANCEL_BTN_NUMBER,
};

class RenderManager
{
private:
	static RenderManager* instance;

	HDC hdc{ nullptr };
	HDC memDC{ nullptr };
	HDC backMemDC{ nullptr };
	HBITMAP oldCreateBitmap = nullptr;

	HPEN redColorHpen = nullptr;
	HPEN greenColorHpen = nullptr;
	HPEN oldHpen = nullptr;

	SIZE ClientSize = { 1280,720 };
	const POINT LEFT_TOP_PRINT_POINT = { 240,60 };
	const POINT RIGHT_BOTTOM_PRINT_POINT = { 1040,640 };

	POINT PRINT_PLAYER_PIVOT_POINT[4];	// 캐릭터 출력 중심 좌표
	vector<POINT> playerBitmapPointVector;	// 캐릭터 타일 별 좌표

	vector<RECT> rectVector;
	RECT messageRect = { 550,350,750,450 };
	RECT moneyRect[4] = { {30,10,90,70} , {1190,10,1250,70},
		{30,640,90,710}, {1190,640,1250,710} };

	int tileWidth = 0, tileHeight = 0;

	RenderManager();
	~RenderManager();
public:
	bool isSelectMapMode = false;
	int selectPosition = -1;

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
	void DrawSelectMode(); // selectMode라면 맵 선택 테두리 그리기
	void Render();
};