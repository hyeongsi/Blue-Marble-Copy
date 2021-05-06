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

    oldCreateBitmap = (HBITMAP)SelectObject(memDC, CreateCompatibleBitmap(backMemDC, ClientSize.cx, ClientSize.cy));
    RECT windowRect{ 0,0,ClientSize.cx, ClientSize.cy };
    FillRect(memDC, &windowRect, (HBRUSH)GetStockObject(WHITE_BRUSH));      // 바탕 흰색으로 초기화
}

void RenderManager::DrawBoardMap()
{
    boardData board = GameManager::GetInstance()->GetBoardData();

    board.mapSize = 8;  // 임시로 출력하고있는거
    //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

    int width = (RIGHT_BOTTOM_PRINT_POINT.x - LEFT_TOP_PRINT_POINT.x) / (board.mapSize+1);
    int height = (RIGHT_BOTTOM_PRINT_POINT.y - LEFT_TOP_PRINT_POINT.y) / (board.mapSize+1);

    Rectangle(memDC, LEFT_TOP_PRINT_POINT.x, LEFT_TOP_PRINT_POINT.y, RIGHT_BOTTOM_PRINT_POINT.x, RIGHT_BOTTOM_PRINT_POINT.y);
    Rectangle(memDC, LEFT_TOP_PRINT_POINT.x + width, LEFT_TOP_PRINT_POINT.y + height, RIGHT_BOTTOM_PRINT_POINT.x - width, RIGHT_BOTTOM_PRINT_POINT.y - height);

    for (int i = 0; i < (int)board.mapSize; i++)   // 좌상단
    {
        MoveToEx(memDC, LEFT_TOP_PRINT_POINT.x + (width * (i+1)), LEFT_TOP_PRINT_POINT.y, NULL);
        LineTo(memDC, LEFT_TOP_PRINT_POINT.x + (width * (i+1)), LEFT_TOP_PRINT_POINT.y + height);
    }
    for (int i = 0; i < (int)board.mapSize; i++)  // 우상단
    {
        MoveToEx(memDC, RIGHT_BOTTOM_PRINT_POINT.x - width, LEFT_TOP_PRINT_POINT.y + (height * (i+1)), NULL);
        LineTo(memDC, RIGHT_BOTTOM_PRINT_POINT.x, LEFT_TOP_PRINT_POINT.y + (height * (i+1)));
    }
    for (int i = 0; i < (int)board.mapSize; i++)   // 우하단
    {
        MoveToEx(memDC, RIGHT_BOTTOM_PRINT_POINT.x - (width * (i+1)), RIGHT_BOTTOM_PRINT_POINT.y - height, NULL);
        LineTo(memDC, RIGHT_BOTTOM_PRINT_POINT.x - (width * (i+1)), RIGHT_BOTTOM_PRINT_POINT.y);
    }
    for (int i = 0; i < (int)board.mapSize; i++)   // 좌상단
    {
        MoveToEx(memDC, LEFT_TOP_PRINT_POINT.x, RIGHT_BOTTOM_PRINT_POINT.y - (height * (i+1)), NULL);
        LineTo(memDC, LEFT_TOP_PRINT_POINT.x + width, RIGHT_BOTTOM_PRINT_POINT.y - (height * (i+1)));
    }
}

void RenderManager::DrawWindow(State state)
{
    vector<BitmapInfo>* windowBitmap = BitmapManager::GetInstance()->GetBitmap(state);
    if (nullptr == windowBitmap)
        return;

    for (const auto&  bitmapIterator : *windowBitmap)
    {
        if(State::GAME == state)
            DrawBitmap(bitmapIterator.bitmap, bitmapIterator.point, true);
        else
            DrawBitmap(bitmapIterator.bitmap, bitmapIterator.point);
    }
}

void RenderManager::DrawHwnd(const HWND dHwnd, const POINT printPoint, const SIZE hwndSize)
{
    HDC hwndDc = GetDC(dHwnd);

    BitBlt(memDC, printPoint.x, printPoint.y, hwndSize.cx, hwndSize.cy, hwndDc, 0, 0, SRCCOPY);
    ReleaseDC(dHwnd, hwndDc);
}

void RenderManager::DrawBitmap(const HBITMAP bitmap, const POINT printPoint, bool isTransparentBlt)
{
    BITMAP bitmapSize{};
    SelectObject(backMemDC, bitmap);
    GetObject(bitmap, sizeof(bitmapSize), &bitmapSize);

    if (isTransparentBlt)
    {
        TransparentBlt(memDC, printPoint.x, printPoint.y,
        bitmapSize.bmWidth, bitmapSize.bmHeight, backMemDC, 0, 0, bitmapSize.bmWidth, bitmapSize.bmHeight, RGB(215, 123, 186));
    }
    else
    {
        BitBlt(memDC, printPoint.x, printPoint.y, bitmapSize.bmWidth, bitmapSize.bmHeight, backMemDC, 0, 0, SRCCOPY);
    } 
}

void RenderManager::Render()
{
    BitBlt(hdc, 0, 0, ClientSize.cx, ClientSize.cy, memDC, 0, 0, SRCCOPY);
}
