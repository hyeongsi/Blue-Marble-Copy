#include "RenderManager.h"
#include "BitmapManager.h"

RenderManager* RenderManager::instance = nullptr;

RenderManager::RenderManager() {}
RenderManager::~RenderManager() {}

RenderManager* RenderManager::GetInstance()
{
	if (nullptr == instance)
	{
		instance = new RenderManager();
	}

	return instance;
}

void RenderManager::ReleaseInstance()
{
	delete instance;
	instance = nullptr;
}

SIZE* RenderManager::GetClientSize()
{
    return &ClientSize;
}

void RenderManager::Init(HWND hWnd)
{
    if (hdc != nullptr)
    {
        ReleaseDC(hWnd, hdc);
        hdc = nullptr;
    }
    if (memDC != nullptr)
    {
        DeleteObject(memDC);
        memDC = nullptr;
    }

    if (backMemDC != nullptr)
    {
        DeleteObject(backMemDC);
        backMemDC = nullptr;
    }

    hdc = GetDC(hWnd);
    memDC = CreateCompatibleDC(hdc);
    backMemDC = CreateCompatibleDC(hdc);
}

void RenderManager::RenderInitSetting()
{
    if (oldCreateBitmap != nullptr)
    {
        DeleteObject(oldCreateBitmap);
        oldCreateBitmap = nullptr;
    }

    oldCreateBitmap = (HBITMAP)SelectObject(memDC, CreateCompatibleBitmap(hdc, ClientSize.cx, ClientSize.cy));
    RECT windowRect{ 0,0,ClientSize.cx, ClientSize.cy };
    FillRect(memDC, &windowRect, (HBRUSH)GetStockObject(WHITE_BRUSH));      // 바탕 흰색으로 초기화
}

void RenderManager::DrawBoardMap(boardData board)
{
    int width = (RIGHT_BOTTOM_PRINT_POINT.x - LEFT_TOP_PRINT_POINT.x) / board.mapSize;
    int height = (RIGHT_BOTTOM_PRINT_POINT.y - LEFT_TOP_PRINT_POINT.y) / board.mapSize;

    int dir = 0;
    for (size_t i = 0; i < board.mapSize; i++)
    {
        Rectangle(backMemDC, RIGHT_BOTTOM_PRINT_POINT.x - (width * i), RIGHT_BOTTOM_PRINT_POINT.y - height, RIGHT_BOTTOM_PRINT_POINT.x, RIGHT_BOTTOM_PRINT_POINT.y);
    }
    dir++;
    for (size_t i = board.mapSize* dir; i < board.mapSize * dir + board.mapSize; i++)
    {
        Rectangle(backMemDC, LEFT_TOP_PRINT_POINT.x, RIGHT_BOTTOM_PRINT_POINT.y - (height * i), LEFT_TOP_PRINT_POINT.x, RIGHT_BOTTOM_PRINT_POINT.y);
    }
    dir++;
    for (size_t i = board.mapSize * dir; i < board.mapSize * dir + board.mapSize; i++)
    {
        Rectangle(backMemDC, LEFT_TOP_PRINT_POINT.x + (width * i), LEFT_TOP_PRINT_POINT.y, LEFT_TOP_PRINT_POINT.x, LEFT_TOP_PRINT_POINT.y);
    }
    dir++;
    for (size_t i = board.mapSize * dir; i < board.mapSize * dir + board.mapSize; i++)
    {
        Rectangle(backMemDC, RIGHT_BOTTOM_PRINT_POINT.x - width, RIGHT_BOTTOM_PRINT_POINT.y + (width * i), RIGHT_BOTTOM_PRINT_POINT.x, RIGHT_BOTTOM_PRINT_POINT.y);
    }

    BitBlt(memDC, 0, 0, ClientSize.cx, ClientSize.cy, backMemDC, 0, 0, SRCCOPY);
}

void RenderManager::DrawWindow(State state)
{
    vector<BitmapInfo>* windowBitmap = BitmapManager::GetInstance()->GetBitmap(state);
    if (nullptr == windowBitmap)
        return;

    for (const auto&  bitmapIterator : *windowBitmap)
    {
        DrawBitmap(bitmapIterator.bitmap, bitmapIterator.point);
    }
}

void RenderManager::DrawHwnd(const HWND dHwnd, const POINT printPoint, const SIZE hwndSize)
{
    HDC hwndDc = GetDC(dHwnd);

    BitBlt(memDC, printPoint.x, printPoint.y, hwndSize.cx, hwndSize.cy, hwndDc, 0, 0, SRCCOPY);
    ReleaseDC(dHwnd, hwndDc);
}

void RenderManager::DrawBitmap(const HBITMAP bitmap, const POINT printPoint)
{
    BITMAP bitmapSize{};
    SelectObject(backMemDC, bitmap);
    GetObject(bitmap, sizeof(bitmapSize), &bitmapSize);
    BitBlt(memDC, printPoint.x, printPoint.y, bitmapSize.bmWidth, bitmapSize.bmHeight, backMemDC,0, 0, SRCCOPY);

    /*TransparentBlt(memDC, printPoint.x, printPoint.y,
        bitmapSize.bmWidth, bitmapSize.bmHeight, backMemDC, 0, 0, bitmapSize.bmWidth, bitmapSize.bmHeight, RGB(132, 126, 135));*/
}

void RenderManager::Render()
{
    BitBlt(hdc, 0, 0, ClientSize.cx, ClientSize.cy, memDC, 0, 0, SRCCOPY);
}
