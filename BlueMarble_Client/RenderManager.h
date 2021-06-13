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
	EXIT_MATCH_BTN_NUMBER,
};

enum SelectMode
{
	IDLE_MODE = 0,
	SELL_LAND_MODE = 1,
	OLYMPIC_MODE = 2,
	WORLD_TRABLE_MODE = 3,
};

enum TurnCheckImageIndex
{
	FIRST_TURN_IMAGE_INDEX = 12,
	LAST_TURN_IMAGE_INDEX = 15,
};

enum MyturnImageIndex
{
	FIRST_MY_TURN_IMAGE_INDEX = 16,
	LAST_MY_TURN_IMAGE_INDEX = 19,
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
	RECT moneyRect[4] = { {60,15,90,70} , {1090,15,1250,70},
		{60,680,90,710}, {1090,680,1250,710} };

	int tileWidth = 0, tileHeight = 0;

	RenderManager();
	~RenderManager();
public:
	int isSelectMapMode = IDLE_MODE;
	int selectPosition = -1;

	static RenderManager* GetInstance();
	static void ReleaseInstance();

	SIZE* GetClientSize();
	void Init(HWND hWnd);
	void RenderInitSetting();

	void MoveSelectPosition(bool isLeft);
	void SetPlayerBitmapLocation(int playerIndex, int tileIndex, const bool isInit = false);

	void InitDrawBoardMap();
	void DrawAnimation(State state, const int index, const int rate);
	void DrawBoardMap();
	void DrawWindow(State state);
	void DrawHwnd(const HWND dHwnd, const POINT printPoint, const SIZE hwndSize);
	void DrawBitmap(const HBITMAP bitmap, const POINT printPoint, bool isTransparentBlt = false);
	void DrawAnimationBitmap(const HBITMAP bitmap, const POINT printPoint, const SIZE printSize, int row, int col, int& count, bool isTransparentBlt = false);
	void DrawGameMessage(string message);
	void DrawSelectMode(); // selectMode라면 맵 선택 테두리 그리기
	void Render();
};