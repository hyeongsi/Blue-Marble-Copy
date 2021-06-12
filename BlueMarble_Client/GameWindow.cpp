#include "GameWindow.h"
#include "RenderManager.h"
#include "BitmapManager.h"
#include "GameManager.h"

bool GameWindow::isReset = false;
GameWindow* GameWindow::instance = nullptr;

GameWindow::GameWindow() {}
GameWindow::~GameWindow() {}

void GameWindow::WmSizeMethod(HWND hWnd)
{
    RECT g_clientRect{ 0,0, RenderManager::GetInstance()->GetClientSize()->cx, RenderManager::GetInstance()->GetClientSize()->cy }; // 클라이언트 크기
    SIZE clientSize;

    AdjustWindowRect(&g_clientRect, WS_OVERLAPPEDWINDOW, false);    // 메뉴창 크기 빼고 윈도우 크기 계산
    clientSize.cx = g_clientRect.right - g_clientRect.left;
    clientSize.cy = g_clientRect.bottom - g_clientRect.top;
    SetWindowPos(hWnd,
        GetNextWindow(hWnd, GW_HWNDPREV),
        0, 0,
        clientSize.cx, clientSize.cy,
        SWP_NOMOVE);
}

void GameWindow::InitClass(HWND hWnd)
{
    instance->g_hWnd = hWnd;

    ResizeWindow(RenderManager::GetInstance()->GetClientSize()->cx, RenderManager::GetInstance()->GetClientSize()->cy, POINT(300, 100), hWnd);

    BitmapManager::GetInstance()->LoadHwndData(State::GAME);
    BitmapManager::GetInstance()->LoadBitmapData(State::GAME);  // game bitmap loading
    BitmapManager::GetInstance()->LoadButtonBitmapData(State::GAME);

    instance->CreateButton(hWnd);
}

void GameWindow::ReInitGame(HWND hWnd)
{
    if (isReset)
    {
        RenderManager::GetInstance()->Init(hWnd);        // MainMenu <- -> Game Render
        MainSystem::GetInstance()->RegistUpdateCallbackFunction(GameUpdate);    // main menu update callback regist // MainMenu <- -> Game Render
        HideButton(ROLL_DICE_BTN);
        HideButton(SELECT_UI_BTN);
        if(!GameManager::GetInstance()->GetPlayerCount())
            ShowButton(EXIT_UI_BTN);
        isReset = false;
        GameManager::GetInstance()->SetGameState(GameState::MATCHING);
    }
}

void GameWindow::ResizeWindow(const LONG width, const LONG height, const POINT printPoint, HWND hWnd)
{
    RECT g_clientRect{ 0,0, width, height }; // 클라이언트 크기
    SIZE clientSize;

    AdjustWindowRect(&g_clientRect, WS_OVERLAPPEDWINDOW, false);    // 메뉴창 크기 빼고 윈도우 크기 계산
    clientSize.cx = g_clientRect.right - g_clientRect.left;
    clientSize.cy = g_clientRect.bottom - g_clientRect.top;
    MoveWindow(hWnd, printPoint.x, printPoint.y, clientSize.cx, clientSize.cy, true);   // printPoint 지점에 clientSize 크기로 출력
}

void GameWindow::CreateButton(HWND hWnd)
{
    vector<HwndInfo>* hwndInfo = BitmapManager::GetInstance()->GetHwnd(State::GAME);

    if (hwndInfo == nullptr)
        return;

    for (const auto& hwndIterator : *hwndInfo)
    {
        instance->hwndWindow.emplace_back(CreateWindow(hwndIterator.type.c_str(),
            hwndIterator.text.c_str(), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_BITMAP,
            hwndIterator.point.x, hwndIterator.point.y,
            hwndIterator.size.cx, hwndIterator.size.cy,
            hWnd, (HMENU)hwndIterator.id, (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), NULL));
    }

    for (int i = 0; i < (int)(*BitmapManager::GetInstance()->GetButtonBitmap(State::GAME)).size(); i++)
    {
        SendMessage(instance->hwndWindow[i], BM_SETIMAGE, 0, (LPARAM)(*BitmapManager::GetInstance()->GetButtonBitmap(State::GAME))[i].bitmap);
    }
}

void GameWindow::ShowButton(int kind)
{
    switch (kind)
    {
    case ROLL_DICE_BTN:
        ShowWindow(hwndWindow[ROLL_DICE_BTN_NUMBER], SW_SHOW);                // 버튼 출력
        break;
    case SELECT_UI_BTN:
        ShowWindow(hwndWindow[SELECT_MODE_OK_BTN_NUMBER], SW_SHOW);           // 버튼 출력
        ShowWindow(hwndWindow[SELECT_MODE_CANCEL_BTN_NUMBER], SW_SHOW);       // 버튼 출력
        break;
    case EXIT_UI_BTN:
        ShowWindow(hwndWindow[EXIT_MATCH_BTN_NUMBER], SW_SHOW);       // 버튼 출력
        break;
    }
}

void GameWindow::HideButton(int kind)
{
    switch (kind)
    {
    case ROLL_DICE_BTN:
        ShowWindow(hwndWindow[ROLL_DICE_BTN_NUMBER], SW_HIDE);               // 버튼 출력
        break;
    case SELECT_UI_BTN:
        ShowWindow(hwndWindow[SELECT_MODE_OK_BTN_NUMBER], SW_HIDE);          // 버튼 출력
        ShowWindow(hwndWindow[SELECT_MODE_CANCEL_BTN_NUMBER], SW_HIDE);      // 버튼 출력
        break;
    case EXIT_UI_BTN:
        ShowWindow(hwndWindow[EXIT_MATCH_BTN_NUMBER], SW_HIDE);              // 버튼 출력
        break;
    }
}

void GameWindow::SendDiceTriggerMsgMethod()
{
    instance->SendDiceTriggerMsg();
}

void GameWindow::SendDiceTriggerMsg()
{
    SocketTransfer::GetInstance()->SendRollDiceSign();
    GameManager::GetInstance()->SetGameState(GameState::WAIT);
    HideButton(ROLL_DICE_BTN);
}

void GameWindow::SendSelectBtnMsgMethod(bool isOK)
{
    instance->SendSelectBtnMsg(isOK);
}

void GameWindow::SendSelectBtnMsg(bool isOK)
{
    SocketTransfer::GetInstance()->GetSelectBtnMsg(isOK);
    GameManager::GetInstance()->SetGameState(GameState::WAIT);
    HideButton(SELECT_UI_BTN);

    RenderManager::GetInstance()->isSelectMapMode = IDLE_MODE;
}

GameWindow* GameWindow::GetInstance()
{
	if (nullptr == instance)
	{
		instance = new GameWindow();
	}

	return instance;
}

void GameWindow::ReleaseInstance()
{
	delete instance;
	instance = nullptr;
}

LRESULT GameWindow::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
        GetInstance()->InitClass(hWnd);
        break;
    case WM_SHOWWINDOW:
        GetInstance()->ReInitGame(hWnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_COMMAND:
        switch (HIWORD(wParam))
        {
            case BN_CLICKED:
            {
                switch (LOWORD(wParam))
                {
                   case IDC_ROLL_THE_DICE:
                       SendDiceTriggerMsgMethod();
                       break;
                   case IDC_SELECT_OK:
                       SendSelectBtnMsgMethod(true);
                       break;
                   case IDC_SELECT_CANCEL:
                       SendSelectBtnMsgMethod(false);
                       break;
                   case IDC_EXIT_MATCHING:
                       SocketTransfer::GetInstance()->TerminateRecvDataThread();
                       break;
                }
            }
        }
        break;
    case WM_SIZE:
        GetInstance()->WmSizeMethod(hWnd);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

void GameWindow::GameUpdate()
{
    RenderManager::GetInstance()->RenderInitSetting();
    RenderManager::GetInstance()->DrawBoardMap();
    RenderManager::GetInstance()->DrawWindow(State::GAME);  // 이미지 출력

    switch (GameManager::GetInstance()->GetGameState())
    {
    case GameState::ROLL_DICE:
        if (GameManager::GetInstance()->GetIsMyDiceTurn())
        {
            RenderManager::GetInstance()->DrawHwnd(instance->hwndWindow[ROLL_DICE_BTN_NUMBER],
                (*BitmapManager::GetInstance()->GetHwnd(State::GAME))[ROLL_DICE_BTN_NUMBER].point,
                (*BitmapManager::GetInstance()->GetHwnd(State::GAME))[ROLL_DICE_BTN_NUMBER].size);
        }
        break;
    case GameState::SELECT_MODE:
        for (int i = SELECT_MODE_OK_BTN_NUMBER; i <= SELECT_MODE_CANCEL_BTN_NUMBER; i++)
        {
            RenderManager::GetInstance()->DrawHwnd(instance->hwndWindow[i],
                (*BitmapManager::GetInstance()->GetHwnd(State::GAME))[i].point,
                (*BitmapManager::GetInstance()->GetHwnd(State::GAME))[i].size);
        }
        break;
    case GameState::MATCHING:
        RenderManager::GetInstance()->DrawHwnd(instance->hwndWindow[EXIT_MATCH_BTN_NUMBER],
            (*BitmapManager::GetInstance()->GetHwnd(State::GAME))[EXIT_MATCH_BTN_NUMBER].point,
            (*BitmapManager::GetInstance()->GetHwnd(State::GAME))[EXIT_MATCH_BTN_NUMBER].size);
        break;
    }

    RenderManager::GetInstance()->DrawGameMessage(GameManager::GetInstance()->GetGameMessage());

    SocketTransfer::GetInstance()->SendSelectModeInput(GameManager::GetInstance()->SelectModeInputKey());     // 선택모드일 때 선택받은 키값 전달

    RenderManager::GetInstance()->Render();
}
