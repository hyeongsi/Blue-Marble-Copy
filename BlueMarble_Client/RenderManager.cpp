#include "RenderManager.h"
#include "BitmapManager.h"
#include <string>

RenderManager* RenderManager::instance = nullptr;

RenderManager::RenderManager() {}
RenderManager::~RenderManager() {}

RenderManager* RenderManager::GetInstance()
{
	if (nullptr == instance)
	{
		instance = new RenderManager();
        instance->redColorHpen = CreatePen(PS_SOLID, 3, RGB(255, 0, 0));
        instance->greenColorHpen = CreatePen(PS_SOLID, 9, RGB(0, 255, 0));
	}

	return instance;
}

void RenderManager::ReleaseInstance()
{
    DeleteObject(instance->redColorHpen);
    DeleteObject(instance->greenColorHpen);
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

void RenderManager::SetPlayerBitmapLocation(int playerIndex, int tileIndex)
{
    POINT printPoint;

    if (playerIndex % 2 == 0)
    {
        printPoint.x = 0;
        printPoint.y = (35 * (playerIndex / 2));
    }
    else
    {
        printPoint.x = 35;
        printPoint.y = (35 * (playerIndex / 2));
    }

    printPoint.x += playerBitmapPointVector[tileIndex].x;
    printPoint.y += playerBitmapPointVector[tileIndex].y;

    
    (*BitmapManager::GetInstance()->GetBitmap(State::GAME))[playerIndex].point = printPoint;
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

    PRINT_PLAYER_PIVOT_POINT[0] = POINT(970, 640);
    PRINT_PLAYER_PIVOT_POINT[1] = 
        POINT(PRINT_PLAYER_PIVOT_POINT[0].x - (tileWidth * (boardSize+1)),
            PRINT_PLAYER_PIVOT_POINT[0].y - tileHeight);
    PRINT_PLAYER_PIVOT_POINT[2] =
        POINT(PRINT_PLAYER_PIVOT_POINT[1].x + tileWidth,
            PRINT_PLAYER_PIVOT_POINT[1].y - (tileHeight * (boardSize + 1)));
    PRINT_PLAYER_PIVOT_POINT[3] =
        POINT(PRINT_PLAYER_PIVOT_POINT[2].x + (tileWidth * (boardSize + 1)),
            PRINT_PLAYER_PIVOT_POINT[2].y + tileHeight);

    // 캐릭터 출력 기준 위치 설정
    playerBitmapPointVector.clear();
    for (int i = 0; i < boardSize; i++)   // 하단가로
    {
        playerBitmapPointVector.emplace_back(
            POINT(PRINT_PLAYER_PIVOT_POINT[0].x - (i * tileWidth),
                PRINT_PLAYER_PIVOT_POINT[0].y));
    }
    for (int i = 0; i < boardSize; i++)   // 좌측세로
    {
        playerBitmapPointVector.emplace_back(
            POINT(PRINT_PLAYER_PIVOT_POINT[1].x,
                PRINT_PLAYER_PIVOT_POINT[1].y - (i * tileHeight)));
    }
    for (int i = 0; i < boardSize; i++)   // 상단가로
    {
        playerBitmapPointVector.emplace_back(
            POINT(PRINT_PLAYER_PIVOT_POINT[2].x + (i * tileWidth),
                PRINT_PLAYER_PIVOT_POINT[2].y));
    }
    for (int i = 0; i < boardSize; i++)  // 우측세로
    {
        playerBitmapPointVector.emplace_back(
            POINT(PRINT_PLAYER_PIVOT_POINT[3].x,
                PRINT_PLAYER_PIVOT_POINT[3].y + (i * tileHeight)));
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
    
    DrawSelectMode();   // 토지 선택창 출력

    // 건물 출력
    for (int i = 0; i < (int)board.name.size(); i++)
    {
        if (GameManager::GetInstance()->GetBoarBuildData().landMark[i]) // 랜드마크 그리기
        {
            Rectangle(memDC, rectVector[i].left, rectVector[i].top + (int)(tileHeight / 3),
                rectVector[i].left + tileWidth, rectVector[i].top + ((int)(tileHeight / 3) * 2));
        }
        else   // 랜드마크 아니면 건물 그리기
        {
            if (GameManager::GetInstance()->GetBoarBuildData().villa[i])    // 빌라 그리기
            {
                Rectangle(memDC, rectVector[i].left, rectVector[i].top + (int)(tileHeight / 3),
                    rectVector[i].left + (int)(tileWidth / 4), rectVector[i].top + ((int)(tileHeight / 3) * 2));
            }
            if (GameManager::GetInstance()->GetBoarBuildData().building[i]) // 빌딩 그리기
            {
                Ellipse(memDC, rectVector[i].left + (int)(tileWidth / 4) + (int)(tileWidth / 6), rectVector[i].top + (int)(tileHeight / 3),
                    rectVector[i].left + ((int)(tileWidth / 4) * 2) + (int)(tileWidth / 6), rectVector[i].top + ((int)(tileHeight / 3) * 2));
            }
            if (GameManager::GetInstance()->GetBoarBuildData().hotel[i])    // 호텔 그리기
            {
                MoveToEx(memDC, (int)(rectVector[i].left + ((tileWidth / 4) * 2) + (tileWidth / 6)) + (int)(tileWidth / 8),
                    rectVector[i].top + (int)(tileHeight / 3), NULL);
                LineTo(memDC, rectVector[i].left + ((int)(tileWidth / 4) * 2) + (int)(tileWidth / 6), rectVector[i].top + ((int)(tileHeight / 3) * 2));
                LineTo(memDC, rectVector[i].left + ((int)(tileWidth / 4) * 3) + (int)(tileWidth / 6), rectVector[i].top + ((int)(tileHeight / 3) * 2));
                LineTo(memDC, (int)(rectVector[i].left + ((tileWidth / 4) * 2) + (tileWidth / 6)) + (int)(tileWidth / 8), rectVector[i].top + (int)(tileHeight / 3));
            }
        }
    }

    // 맵 이름 출력
    for (int i = 0; i < (int)board.name.size(); i++)
    {
        switch ( GameManager::GetInstance()->GetBoardData().owner[i] )
        {
        case 1:     // 1p
            SetTextColor(memDC, RGB(237, 28, 36));      // 빨간색
            break;
        case 2:     // 2p
            SetTextColor(memDC, RGB(63, 72, 204));     // 파란색
            break;
        case 3:     // 3p
            break;
        case 4:     // 4p
            break;
        default:    // 소유주 없음
            SetTextColor(memDC, RGB(0, 0, 0));     // 검은색
            break;
        }
        DrawText(memDC, board.name[i].c_str(), -1, &rectVector[i], DT_NOCLIP | DT_CENTER);
        SetTextColor(memDC, RGB(0, 0, 0));     // 검은색
    }
}

void RenderManager::DrawWindow(State state)
{
    vector<BitmapInfo>* windowBitmap = BitmapManager::GetInstance()->GetBitmap(state);
    if (nullptr == windowBitmap)
        return;

    int count = 0;
    for (const auto&  bitmapIterator : *windowBitmap)
    {
        if (State::GAME == state)
        {
            if ((count < MAX_PLAYER) && (GameManager::GetInstance()->GetPlayerCount() <= count))
                continue;   // 플레이어 숫자에 따라 출력되는 캐릭터 수 제한

            DrawBitmap(bitmapIterator.bitmap, bitmapIterator.point, true);
        }
        else
            DrawBitmap(bitmapIterator.bitmap, bitmapIterator.point);

        count++;
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

void RenderManager::DrawGameMessage(string message)
{
    DrawText(memDC, message.c_str(), -1, &messageRect, DT_NOCLIP | DT_CENTER);

    if (GameManager::GetInstance()->GetPlayerCount() == 0)
        return;

    for (int i = 0; i < (int)GameManager::GetInstance()->GetUserMoneyVector()->size(); i++)
    {
        if (GameManager::GetInstance()->GetCharacterIndex() == i+1)
        {
            DrawText(memDC, ("나 - " + to_string((*GameManager::GetInstance()->GetUserMoneyVector())[i])).c_str(), -1, &moneyRect[i], DT_NOCLIP | DT_CENTER);
        }
        else
        {
            DrawText(memDC, to_string((*GameManager::GetInstance()->GetUserMoneyVector())[i]).c_str(), -1, &moneyRect[i], DT_NOCLIP | DT_CENTER);
        }
    }
}

void RenderManager::DrawSelectMode()
{
    if (GameManager::GetInstance()->GetBoardData().mapSize == 0)
        return;
    if (!isSelectMapMode)
        return;

    int remainder;
    // 선택된 맵타일 초록색으로 마킹
    oldHpen = (HPEN)SelectObject(memDC, (HGDIOBJ)greenColorHpen);
    for (auto iterator : GameManager::GetInstance()->selectLandIndex)
    {
        remainder = iterator % (int)GameManager::GetInstance()->GetBoardData().mapSize;   // 나눈 나머지
        switch (iterator / (int)GameManager::GetInstance()->GetBoardData().mapSize)
        {
        case SOUTH:
            Rectangle(memDC, RIGHT_BOTTOM_PRINT_POINT.x - ((remainder + 1) * tileWidth), RIGHT_BOTTOM_PRINT_POINT.y - tileHeight,
                RIGHT_BOTTOM_PRINT_POINT.x - (remainder * tileWidth), RIGHT_BOTTOM_PRINT_POINT.y);
            break;
        case WEST:
            Rectangle(memDC, LEFT_TOP_PRINT_POINT.x, RIGHT_BOTTOM_PRINT_POINT.y - ((remainder + 1) * tileHeight),
                LEFT_TOP_PRINT_POINT.x + tileWidth, RIGHT_BOTTOM_PRINT_POINT.y - (remainder * tileHeight));
            break;
        case NORTH:
            Rectangle(memDC, LEFT_TOP_PRINT_POINT.x + (remainder * tileWidth), LEFT_TOP_PRINT_POINT.y,
                LEFT_TOP_PRINT_POINT.x + ((remainder + 1) * tileWidth), LEFT_TOP_PRINT_POINT.y + tileHeight);
            break;
        case EAST:
            Rectangle(memDC, RIGHT_BOTTOM_PRINT_POINT.x - tileWidth, LEFT_TOP_PRINT_POINT.y + ((remainder + 1) * tileHeight),
                RIGHT_BOTTOM_PRINT_POINT.x, LEFT_TOP_PRINT_POINT.y + (remainder * tileHeight));
            break;
        default:
            break;
        }
    }
    greenColorHpen = (HPEN)SelectObject(memDC, (HGDIOBJ)oldHpen);

    // 선택중인 맵타일 빨간색으로 마킹
    remainder = selectPosition % (int)GameManager::GetInstance()->GetBoardData().mapSize;   // 나눈 나머지
    oldHpen = (HPEN)SelectObject(memDC, (HGDIOBJ)redColorHpen);
    switch (selectPosition/ (int)GameManager::GetInstance()->GetBoardData().mapSize)
    {
    case SOUTH:
        Rectangle(memDC, RIGHT_BOTTOM_PRINT_POINT.x - ((remainder + 1) * tileWidth), RIGHT_BOTTOM_PRINT_POINT.y - tileHeight,
            RIGHT_BOTTOM_PRINT_POINT.x - (remainder * tileWidth), RIGHT_BOTTOM_PRINT_POINT.y);
        break;
    case WEST:
        Rectangle(memDC, LEFT_TOP_PRINT_POINT.x, RIGHT_BOTTOM_PRINT_POINT.y - ((remainder + 1) * tileHeight),
            LEFT_TOP_PRINT_POINT.x + tileWidth, RIGHT_BOTTOM_PRINT_POINT.y - (remainder * tileHeight));
        break;
    case NORTH:
        Rectangle(memDC, LEFT_TOP_PRINT_POINT.x + (remainder * tileWidth), LEFT_TOP_PRINT_POINT.y,
            LEFT_TOP_PRINT_POINT.x + ((remainder + 1) * tileWidth), LEFT_TOP_PRINT_POINT.y + tileHeight);
        break;
    case EAST:
        Rectangle(memDC, RIGHT_BOTTOM_PRINT_POINT.x - tileWidth, LEFT_TOP_PRINT_POINT.y + ((remainder + 1) * tileHeight),
            RIGHT_BOTTOM_PRINT_POINT.x, LEFT_TOP_PRINT_POINT.y + (remainder * tileHeight));
        break;
    default:
        break;
    }
    redColorHpen = (HPEN)SelectObject(memDC, (HGDIOBJ)oldHpen);
}

void RenderManager::Render()
{
    BitBlt(hdc, 0, 0, ClientSize.cx, ClientSize.cy, memDC, 0, 0, SRCCOPY);
}
