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

void RenderManager::InitDrawBoardMap()
{
    rectVector.clear();
    int boardSize = GameManager::GetInstance()->GetBoardData().mapSize;
    RECT rect;

    tileWidth = (RIGHT_BOTTOM_PRINT_POINT.x - LEFT_TOP_PRINT_POINT.x) / (boardSize + 1);
    tileHeight = (RIGHT_BOTTOM_PRINT_POINT.y - LEFT_TOP_PRINT_POINT.y) / (boardSize + 1);

    for (int i = 0; i < boardSize; i++)   // 하단가로
    {
        rect.left = RIGHT_BOTTOM_PRINT_POINT.x - (tileWidth * (i + 1));
        rect.top = RIGHT_BOTTOM_PRINT_POINT.y - tileHeight;
        rect.right = RIGHT_BOTTOM_PRINT_POINT.x - (tileWidth * i);
        rect.bottom = RIGHT_BOTTOM_PRINT_POINT.y;
        rectVector.emplace_back(rect);
    }
    for (int i = 0; i < boardSize; i++)   // 좌측세로
    {
        rect.left = LEFT_TOP_PRINT_POINT.x;
        rect.top = RIGHT_BOTTOM_PRINT_POINT.y - (tileHeight * (i + 1));
        rect.right = LEFT_TOP_PRINT_POINT.x + tileWidth;
        rect.bottom = RIGHT_BOTTOM_PRINT_POINT.y - (tileHeight * i);
        rectVector.emplace_back(rect);
    }
    for (int i = 0; i < boardSize; i++)   // 상단가로
    {
        rect.left = LEFT_TOP_PRINT_POINT.x + (tileWidth * i);
        rect.top = LEFT_TOP_PRINT_POINT.y;
        rect.right = LEFT_TOP_PRINT_POINT.x + (tileWidth * (i + 1));
        rect.bottom = LEFT_TOP_PRINT_POINT.y + tileHeight;
        rectVector.emplace_back(rect);
    }
    for (int i = 0; i < boardSize; i++)  // 우측세로
    {
        rect.left = RIGHT_BOTTOM_PRINT_POINT.x - tileWidth;
        rect.top = LEFT_TOP_PRINT_POINT.y + (tileHeight * i);
        rect.right = RIGHT_BOTTOM_PRINT_POINT.x;
        rect.bottom = LEFT_TOP_PRINT_POINT.y + (tileHeight * (i + 1));
        rectVector.emplace_back(rect);
    }
}

void RenderManager::DrawBoardMap()
{
    boardData board = GameManager::GetInstance()->GetBoardData();
    if (0 >= board.mapSize)
        return;

    Rectangle(memDC, LEFT_TOP_PRINT_POINT.x, LEFT_TOP_PRINT_POINT.y, RIGHT_BOTTOM_PRINT_POINT.x, RIGHT_BOTTOM_PRINT_POINT.y);
    Rectangle(memDC, LEFT_TOP_PRINT_POINT.x + tileWidth, LEFT_TOP_PRINT_POINT.y + tileHeight, RIGHT_BOTTOM_PRINT_POINT.x - tileWidth, RIGHT_BOTTOM_PRINT_POINT.y - tileHeight);

    for (int i = 0; i < (int)board.mapSize; i++)   // 하단가로
    {
        MoveToEx(memDC, RIGHT_BOTTOM_PRINT_POINT.x - (tileWidth * (i + 1)), RIGHT_BOTTOM_PRINT_POINT.y - tileHeight, NULL);
        LineTo(memDC, RIGHT_BOTTOM_PRINT_POINT.x - (tileWidth * (i + 1)), RIGHT_BOTTOM_PRINT_POINT.y);
    }
    for (int i = 0; i < (int)board.mapSize; i++)   // 좌측세로
    {
        MoveToEx(memDC, LEFT_TOP_PRINT_POINT.x, RIGHT_BOTTOM_PRINT_POINT.y - (tileHeight * (i + 1)), NULL);
        LineTo(memDC, LEFT_TOP_PRINT_POINT.x + tileWidth, RIGHT_BOTTOM_PRINT_POINT.y - (tileHeight * (i + 1)));
    }
    for (int i = 0; i < (int)board.mapSize; i++)   // 상단가로
    {
        MoveToEx(memDC, LEFT_TOP_PRINT_POINT.x + (tileWidth * (i+1)), LEFT_TOP_PRINT_POINT.y, NULL);
        LineTo(memDC, LEFT_TOP_PRINT_POINT.x + (tileWidth * (i+1)), LEFT_TOP_PRINT_POINT.y + tileHeight);
    }
    for (int i = 0; i < (int)board.mapSize; i++)  // 우측세로
    {
        MoveToEx(memDC, RIGHT_BOTTOM_PRINT_POINT.x - tileWidth, LEFT_TOP_PRINT_POINT.y + (tileHeight * (i+1)), NULL);
        LineTo(memDC, RIGHT_BOTTOM_PRINT_POINT.x, LEFT_TOP_PRINT_POINT.y + (tileHeight * (i+1)));
    }
    
    for (int i = 0; i < board.name.size(); i++)
    {
        DrawText(memDC, board.name[i].c_str(), -1, &rectVector[i], DT_NOCLIP | DT_CENTER);
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
