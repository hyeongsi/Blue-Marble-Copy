#include "RenderManager.h"
#include "BitmapManager.h"
#include <string>
#include <time.h>

RenderManager* RenderManager::instance = nullptr;

RenderManager::RenderManager() {}
RenderManager::~RenderManager() {}

RenderManager* RenderManager::GetInstance()
{
	if (nullptr == instance)
	{
		instance = new RenderManager();
        instance->redColorHbrush = CreateSolidBrush(COLORREF(0xBD9173));
        instance->yellowColorHbrush = CreateSolidBrush(COLORREF(0xFDFFD7));
        instance->blueColorHbrush = CreateSolidBrush(COLORREF(0xD2EFFF));
        instance->greenColorHbrush = CreateSolidBrush(COLORREF(0xD7FFDB));
        instance->purpleColorHbrush = CreateSolidBrush(COLORREF(0xF9D7FF));
        instance->redColorHpen = CreatePen(PS_SOLID, 3, RGB(255, 0, 0));
        instance->greenColorHpen = CreatePen(PS_SOLID, 9, RGB(0, 255, 0));
	}

	return instance;
}

void RenderManager::ReleaseInstance()
{
    DeleteObject(instance->redColorHbrush);
    DeleteObject(instance->yellowColorHbrush);
    DeleteObject(instance->blueColorHbrush);
    DeleteObject(instance->greenColorHbrush);
    DeleteObject(instance->purpleColorHbrush);
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
    SetBkMode(memDC, TRANSPARENT);
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

vector<AnimationInfo>* RenderManager::GetGameAnimationInfoVector()
{
    return &gameAnimationInfoVector;
}

void RenderManager::MoveSelectPosition(bool isLeft)
{
    if (isLeft)
    {
        selectPosition += 1;

        if ((int)GameManager::GetInstance()->GetBoardData().code.size() <= selectPosition)
        {
            selectPosition -= GameManager::GetInstance()->GetBoardData().code.size();
        }
    }
    else
    {
        selectPosition -= 1;

        if (0 > selectPosition)
        {
            selectPosition += (GameManager::GetInstance()->GetBoardData().code.size());
        }
    }
}

void RenderManager::SetPlayerBitmapLocation(int playerIndex, int tileIndex, const bool isInit)
{
    POINT printPoint;
    int count = 0;
    const int printSpace = 35;
    int addValue = 0;

    for (int i = 0; i < (int)(*GameManager::GetInstance()->GetUserPositionVector()).size(); i++)
    {
        if ((*GameManager::GetInstance()->GetBackruptcyVector())[i] == true)
            continue;

        if (tileIndex == (*GameManager::GetInstance()->GetUserPositionVector())[i])
            count++;
    }

    if (isInit)
        count = (playerIndex + 1);

    switch (tileIndex / GameManager::GetInstance()->GetBoardData().mapSize)
    {
    case 0: // 
        if ((count - 1) % 2 == 0)
        {}
        else
        {
            addValue = printSpace;
        }

        printPoint.x = addValue;
        printPoint.y = (printSpace * ((count - 1) / 2));
        break;
    case 1:
        if ((count - 1) % 2 == 0)
        {
            if ((count) == 1)
                count = 3;
            else
                count = 1;     
        }
        else
        {
            if ((count) == 2)
                count = 4;
            else
                count = 2;

            addValue = printSpace;
        }

        printPoint.x = (printSpace * ((count - 1) / 2)); 
        printPoint.y = addValue;
        break;
    case 2:
        if ((count - 1) % 2 == 0)
        {
            if ((count) == 1)
                count = 3;
            else
                count = 1;
        }
        else
        {
            if ((count) == 2)
                count = 4;
            else
                count = 2;

            addValue = printSpace;
        }
        printPoint.x = addValue;
        printPoint.y = (printSpace * ((count - 1) / 2));
        break;
    case 3:
        if ((count - 1) % 2 == 0) 
        {
            addValue = printSpace;
        }
        else
        {}
        printPoint.x = (printSpace * ((count - 1) / 2));
        printPoint.y = addValue;
        break;
    }

    printPoint.x += playerBitmapPointVector[tileIndex].x;
    printPoint.y += playerBitmapPointVector[tileIndex].y;

    int playerBitmapindex = PLAYER1_PIECE + playerIndex;

    (*BitmapManager::GetInstance()->GetBitmap(State::GAME))[playerBitmapindex].point = printPoint;
}

void RenderManager::InitDrawBoardMap()
{
    rectVector.clear();
    int boardSize = GameManager::GetInstance()->GetBoardData().mapSize;
    RECT rect;

    tileWidth = (RIGHT_BOTTOM_PRINT_POINT.x - LEFT_TOP_PRINT_POINT.x) / (boardSize+1);
    tileHeight = (RIGHT_BOTTOM_PRINT_POINT.y - LEFT_TOP_PRINT_POINT.y) / (boardSize+1);

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
        POINT(PRINT_PLAYER_PIVOT_POINT[0].x - (tileWidth * (boardSize+1)) - 10,
            PRINT_PLAYER_PIVOT_POINT[0].y - tileHeight);
    PRINT_PLAYER_PIVOT_POINT[2] =
        POINT(PRINT_PLAYER_PIVOT_POINT[1].x + tileWidth,
            PRINT_PLAYER_PIVOT_POINT[1].y - (tileHeight * (boardSize + 1)) - 5);
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

    for (int i = 0; i < (int)(*BitmapManager::GetInstance()->GetBitmap(State::GAME)).size(); i++)
    {
        if (GameManager::GetInstance()->GetPlayerCount() <= i)
            break;

        SetPlayerBitmapLocation(i, 0, true);
    }
}

void RenderManager::DrawGameAnimation()
{
    if ((int)gameAnimationInfoVector.size() == 0)
        return;

    bool stop = false;

    if(GameManager::GetInstance()->GetGameState() == GameState::MATCHING)
        DrawAnimation(State::GAME, LOADING_UI, 200, stop);
    else if (GameManager::GetInstance()->GetGameState() != GameState::MATCHING &&
        GameManager::GetInstance()->diceInfoValue.isrun)
    {
        GameManager::GetInstance()->diceMutex.lock();
        if (gameAnimationInfoVector[ROLL_DICE_UI1].count >= GameManager::GetInstance()->diceInfoValue.saveDice1Count + 3)
            stop = true;

        DrawAnimation(State::GAME, ROLL_DICE_UI1, 200, stop);
        DrawAnimation(State::GAME, ROLL_DICE_UI2, 200, stop);
        GameManager::GetInstance()->diceMutex.unlock();
    }
}

void RenderManager::DrawAnimation(State state, const int index, const int rate, bool stop)
{
    vector<AnimationBitmapInfo> animationBitmapInfo = *BitmapManager::GetInstance()->GetAnimationBitmap(state);
    if (0 > index && (int)animationBitmapInfo.size() <= index)
        return;

    DrawAnimationBitmap(animationBitmapInfo[index].bitmap, animationBitmapInfo[index].point,
        animationBitmapInfo[index].size, animationBitmapInfo[index].row, animationBitmapInfo[index].col,
        gameAnimationInfoVector[index].count, true);
    if ((clock() - gameAnimationInfoVector[index].endClock) >= rate)
    {
        gameAnimationInfoVector[index].endClock = clock();
        if(!stop)
            gameAnimationInfoVector[index].count++;
    }
}

void RenderManager::DrawBoardMap()
{
    boardData board = GameManager::GetInstance()->GetBoardData();
    if (0 >= board.mapSize)
        return;

    Rectangle(memDC, LEFT_TOP_PRINT_POINT.x, LEFT_TOP_PRINT_POINT.y, RIGHT_BOTTOM_PRINT_POINT.x, RIGHT_BOTTOM_PRINT_POINT.y);
    Rectangle(memDC, LEFT_TOP_PRINT_POINT.x + tileWidth, LEFT_TOP_PRINT_POINT.y + tileHeight, RIGHT_BOTTOM_PRINT_POINT.x - tileWidth, RIGHT_BOTTOM_PRINT_POINT.y - tileHeight);

    int count = -1;
    for (int i = 0; i < (int)board.mapSize; i++)   // 하단가로
    {
        count++;

        if (board.code[count] == LAND_TILE)
        {
            oldHBrush = (HBRUSH)SelectObject(memDC, redColorHbrush);
            Rectangle(memDC, RIGHT_BOTTOM_PRINT_POINT.x - (tileWidth * i), RIGHT_BOTTOM_PRINT_POINT.y, RIGHT_BOTTOM_PRINT_POINT.x - (tileWidth * (i+1)), RIGHT_BOTTOM_PRINT_POINT.y - tileHeight);
            SelectObject(memDC, oldHBrush);
        }
        else if (board.code[count] == TOUR_TILE)
        {
            oldHBrush = (HBRUSH)SelectObject(memDC, purpleColorHbrush);
            Rectangle(memDC, RIGHT_BOTTOM_PRINT_POINT.x - (tileWidth * i), RIGHT_BOTTOM_PRINT_POINT.y, RIGHT_BOTTOM_PRINT_POINT.x - (tileWidth * (i + 1)), RIGHT_BOTTOM_PRINT_POINT.y - tileHeight);
            SelectObject(memDC, oldHBrush);
        }
    }
    for (int i = 0; i < (int)board.mapSize; i++)   // 좌측세로
    {
        count++;
        if (board.code[count] == LAND_TILE)
        {
            oldHBrush = (HBRUSH)SelectObject(memDC, yellowColorHbrush);
            Rectangle(memDC, LEFT_TOP_PRINT_POINT.x + tileWidth, RIGHT_BOTTOM_PRINT_POINT.y - (tileHeight * i), LEFT_TOP_PRINT_POINT.x, RIGHT_BOTTOM_PRINT_POINT.y - (tileHeight * (i + 1)));
            SelectObject(memDC, oldHBrush);
        }
        else if (board.code[count] == TOUR_TILE)
        {
            oldHBrush = (HBRUSH)SelectObject(memDC, purpleColorHbrush);
            Rectangle(memDC, LEFT_TOP_PRINT_POINT.x + tileWidth, RIGHT_BOTTOM_PRINT_POINT.y - (tileHeight * i), LEFT_TOP_PRINT_POINT.x, RIGHT_BOTTOM_PRINT_POINT.y - (tileHeight * (i + 1)));
            SelectObject(memDC, oldHBrush);
        }
    }
    for (int i = 0; i < (int)board.mapSize; i++)   // 상단가로
    {
        count++;
        if (board.code[count] == LAND_TILE)
        {
            oldHBrush = (HBRUSH)SelectObject(memDC, blueColorHbrush);
            Rectangle(memDC, LEFT_TOP_PRINT_POINT.x + (tileWidth * i), LEFT_TOP_PRINT_POINT.y, LEFT_TOP_PRINT_POINT.x + (tileWidth * (i + 1)), LEFT_TOP_PRINT_POINT.y + tileHeight);
            SelectObject(memDC, oldHBrush);
        }
        else if (board.code[count] == TOUR_TILE)
        {
            oldHBrush = (HBRUSH)SelectObject(memDC, purpleColorHbrush);
            Rectangle(memDC, LEFT_TOP_PRINT_POINT.x + (tileWidth * i), LEFT_TOP_PRINT_POINT.y, LEFT_TOP_PRINT_POINT.x + (tileWidth * (i + 1)), LEFT_TOP_PRINT_POINT.y + tileHeight);
            SelectObject(memDC, oldHBrush);
        } 
    }
    for (int i = 0; i < (int)board.mapSize; i++)  // 우측세로
    {
        count++;
        if (board.code[count] == LAND_TILE)
        {
            oldHBrush = (HBRUSH)SelectObject(memDC, greenColorHbrush);
            Rectangle(memDC, RIGHT_BOTTOM_PRINT_POINT.x - tileWidth, LEFT_TOP_PRINT_POINT.y + (tileHeight * i), RIGHT_BOTTOM_PRINT_POINT.x, LEFT_TOP_PRINT_POINT.y + (tileHeight * (i + 1)));
            SelectObject(memDC, oldHBrush);
        }
        else if (board.code[count] == TOUR_TILE)
        {
            oldHBrush = (HBRUSH)SelectObject(memDC, purpleColorHbrush);
            Rectangle(memDC, RIGHT_BOTTOM_PRINT_POINT.x - tileWidth, LEFT_TOP_PRINT_POINT.y + (tileHeight * i), RIGHT_BOTTOM_PRINT_POINT.x, LEFT_TOP_PRINT_POINT.y + (tileHeight * (i + 1)));
            SelectObject(memDC, oldHBrush);
        }
    }
    
    vector<BitmapInfo>* windowBitmap = BitmapManager::GetInstance()->GetBitmap(State::GAME);    // 타일 이미지 출력
    
    for (int i = START_TILE_BACKGROUND; i < PLAYER1_PIECE; i++)
    {
        DrawBitmap((*windowBitmap)[i].bitmap, (*windowBitmap)[i].point);    // 타일 이미지
    }

    DrawBitmap((*windowBitmap)[BLACKBOARD_UI].bitmap, (*windowBitmap)[BLACKBOARD_UI].point);    // 중앙 칠판 이미지

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
            SetTextColor(memDC, RGB(255, 201, 14));     // 노란색
            break;
        case 4:     // 4p
            SetTextColor(memDC, RGB(34, 177, 16));     // 초록색
            break;
        default:    // 소유주 없음
            SetTextColor(memDC, RGB(0, 0, 0));     // 검은색
            break;
        }
        
        if (GameManager::GetInstance()->GetBoarBuildData().olympic[i] != 0)  // 올림픽 개최 장소라면
        {
            DrawText(memDC, ((board.name[i]) + " x"+ 
                to_string((int)pow(2,GameManager::GetInstance()->GetBoarBuildData().olympic[i]))).c_str(),
                -1, &rectVector[i], DT_NOCLIP | DT_CENTER);
        }
        else  // 개최 장소 아니면
        {
            DrawText(memDC, board.name[i].c_str(), -1, &rectVector[i], DT_NOCLIP | DT_CENTER);
        }
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
        count++;

        if (State::GAME == state)
        {
            if((count-1) == GAME_WINDOW_BACKGROUND)
                DrawBitmap(bitmapIterator.bitmap, bitmapIterator.point);
            if (GameManager::GetInstance()->GetGameState() == GameState::MATCHING)
                return;

            switch (count-1)
            {
            case START_TILE_BACKGROUND:
            case DESERT_ISLAND_TILE_BACKGROUND:
            case OLYMPIC_TILE_BACKGROUND:
            case AIRPLAIN_TILE_BACKGROUND:
            case REVENUE_TILE_BACKGROUND:
            case CARD1_TILE_BACKGROUND:
            case CARD2_TILE_BACKGROUND:
            case CARD3_TILE_BACKGROUND:
            case CARD4_TILE_BACKGROUND:
                break;
            case PLAYER1_PIECE:
            case PLAYER2_PIECE:
            case PLAYER3_PIECE:
            case PLAYER4_PIECE:
                if ((int)(*GameManager::GetInstance()->GetBackruptcyVector()).size() == 0)
                    return;
                if ((*GameManager::GetInstance()->GetBackruptcyVector())[count - PLAYER2_PIECE] == false)
                    DrawBitmap(bitmapIterator.bitmap, bitmapIterator.point, true);
                break;
            case PLAYER1_TURN_UI:
            case PLAYER2_TURN_UI:
            case PLAYER3_TURN_UI:
            case PLAYER4_TURN_UI:
                if((count - PLAYER2_TURN_UI) == GameManager::GetInstance()->whosTurn &&
                    GameManager::GetInstance()->whosTurn != -1)
                    DrawBitmap(bitmapIterator.bitmap, bitmapIterator.point);
                break;
            case PLAYER1_ME_UI:
            case PLAYER2_ME_UI:
            case PLAYER3_ME_UI:
            case PLAYER4_ME_UI:
                if (count - PLAYER2_ME_UI != GameManager::GetInstance()->GetCharacterIndex() - 1)
                    break;
                else
                    DrawBitmap(bitmapIterator.bitmap, bitmapIterator.point);
                break;
            case BLACKBOARD_UI:
                break;
            default:
                DrawBitmap(bitmapIterator.bitmap, bitmapIterator.point);
                break;
            }
        }
        else
            DrawBitmap(bitmapIterator.bitmap, bitmapIterator.point, true);  
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

void RenderManager::DrawAnimationBitmap(const HBITMAP bitmap, const POINT printPoint, const SIZE printSize, int row, int col, int& count, bool isTransparentBlt)
{
    BITMAP bitmapSize{};
    SelectObject(backMemDC, bitmap);
    GetObject(bitmap, sizeof(bitmapSize), &bitmapSize);

    POINT bitmapSlicePOINT = {0,0};

    if (row == 0)
    {
        if(col <= count)
            count = 0;

        bitmapSlicePOINT.y = (count * printSize.cy);
    }
    else if (col == 0)
    {
        if (row <= count)
        {
            count = 0;
        }

        bitmapSlicePOINT.x = (count * printSize.cx);
    }else  
    {
        if ((row * col) <= count)
            count = 0;

        bitmapSlicePOINT.x = (count % row) * printSize.cx;
        bitmapSlicePOINT.y = (count / row) * printSize.cy;
    }

    if (isTransparentBlt)
    {
        TransparentBlt(memDC, printPoint.x, printPoint.y,
            printSize.cx, printSize.cy, backMemDC, bitmapSlicePOINT.x, bitmapSlicePOINT.y, printSize.cx, printSize.cy,RGB(255, 255, 255));
    }
    else
    {
        BitBlt(memDC, printPoint.x, printPoint.y, printSize.cx, printSize.cy, backMemDC, bitmapSlicePOINT.x, bitmapSlicePOINT.y, SRCCOPY);
    }
}

void RenderManager::DrawGameMessage(string message)
{
    DrawText(memDC, message.c_str(), -1, &messageRect, DT_NOCLIP | DT_CENTER);

    if (GameManager::GetInstance()->GetPlayerCount() == 0)
        return;

    for (int i = 0; i < (int)GameManager::GetInstance()->GetUserMoneyVector()->size(); i++)
    {
        if ((*GameManager::GetInstance()->GetBackruptcyVector())[i] == true)
        {
            DrawText(memDC, "파산했습니다.", -1, &moneyRect[i], DT_NOCLIP | DT_LEFT);
        }
        else
        {
            DrawText(memDC, ("돈 : " + to_string((*GameManager::GetInstance()->GetUserMoneyVector())[i])).c_str(), -1, &moneyRect[i], DT_NOCLIP | DT_LEFT);
        } 
    }

}

void RenderManager::DrawSelectMode()
{
    if (GameManager::GetInstance()->GetBoardData().mapSize == 0)
        return;
    if (isSelectMapMode == IDLE_MODE)
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
