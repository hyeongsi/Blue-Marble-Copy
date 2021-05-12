#include "GameWindow.h"
#include "RenderManager.h"
#include "BitmapManager.h"
#include "GameManager.h"

GameWindow* GameWindow::instance = nullptr;

GameWindow::GameWindow() {}
GameWindow::~GameWindow() {}

void GameWindow::InitClass(HWND hWnd)
{
    instance->renderManager = RenderManager::GetInstance();
    instance->bitmapManager = BitmapManager::GetInstance();

    ResizeWindow(instance->renderManager->GetClientSize()->cx, instance->renderManager->GetClientSize()->cy, POINT(300, 100), hWnd);

    instance->bitmapManager->LoadHwndData(State::GAME);
    instance->bitmapManager->LoadBitmapData(State::GAME);  // game bitmap loading

    instance->CreateButton(hWnd);
}

void GameWindow::ReInitGame(HWND hWnd)
{
    instance->renderManager->Init(hWnd);        // MainMenu <- -> Game Render
    MainSystem::GetInstance()->RegistUpdateCallbackFunction(GameUpdate);    // main menu update callback regist // MainMenu <- -> Game Render
    ShowButton();
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
    vector<HwndInfo>* hwndInfo = instance->bitmapManager->GetHwnd(State::GAME);

    if (hwndInfo == nullptr)
        return;

    for (const auto& hwndIterator : *hwndInfo)
    {
        instance->hwndWindow.emplace_back(CreateWindow(hwndIterator.type.c_str(), hwndIterator.text.c_str(), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            hwndIterator.point.x, hwndIterator.point.y,
            hwndIterator.size.cx, hwndIterator.size.cy,
            hWnd, (HMENU)hwndIterator.id, (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), NULL));
    }
}

void GameWindow::ShowButton()
{
    for (const auto& buttonHandleIterator : instance->hwndWindow)
    {
        ShowWindow(buttonHandleIterator, SW_SHOW);                // 버튼 출력
    }
}

void GameWindow::HideButton()
{
    for (const auto& buttonHandleIterator : instance->hwndWindow)
    {
        ShowWindow(buttonHandleIterator, SW_HIDE);                // 버튼 출력
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
        switch (LOWORD(wParam))
        {
        case IDC_ROLL_THE_DICE:
            SendDiceTriggerMsgMethod();
            break;
        }
        break;
       
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

void GameWindow::GameUpdate()
{
    instance->renderManager->RenderInitSetting();
    instance->renderManager->DrawBoardMap();
    instance->renderManager->DrawWindow(State::GAME);

    if (GameManager::GetInstance()->GetIsMyTurn() || GameManager::GetInstance()->GetGameState() == GameState::ROLL_DICE)
    {
        for (int i = 0; i < (int)instance->hwndWindow.size(); i++)
        {
            instance->renderManager->DrawHwnd(instance->hwndWindow[i],
                (*instance->bitmapManager->GetHwnd(State::GAME))[i].point,
                (*instance->bitmapManager->GetHwnd(State::GAME))[i].size);
        }
    }

    instance->renderManager->Render();
}
