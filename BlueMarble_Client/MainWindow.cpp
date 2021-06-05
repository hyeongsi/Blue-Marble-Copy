#include "MainWindow.h"
#include "Resource1.h"
#include "MainSystem.h"
#include "RenderManager.h"
#include "BitmapManager.h"
#include "HttpTransfer.h"
#include <process.h>
#include "GameWindow.h"

MainWindow* MainWindow::instance = nullptr;
bool MainWindow::isRunRankingThread = false;
bool MainWindow::isRunStartThread = false;
bool MainWindow::isReset = true;

MainWindow::MainWindow() {}
MainWindow::~MainWindow() {}

void MainWindow::InitMainMenu(HWND hWnd)
{
    instance->renderManager = RenderManager::GetInstance();
    instance->bitmapManager = BitmapManager::GetInstance();

    ResizeWindow(instance->renderManager->GetClientSize()->cx, instance->renderManager->GetClientSize()->cy, POINT(300,100), hWnd);

    instance->bitmapManager->LoadHwndData(State::MAIN_MENU);
    instance->bitmapManager->LoadBitmapData(State::MAIN_MENU);  // main menu bitmap loading 

    CreateButton(hWnd);
}

void MainWindow::ReInitMainMenu(HWND hWnd)
{
    if (isReset)
    {
        instance->renderManager->Init(hWnd);  // MainMenu <- -> Game Render
        MainSystem::GetInstance()->RegistUpdateCallbackFunction(MainMenuUpdate);    // main menu update callback regist // MainMenu <- -> Game Render
        ShowButton();
        isReset = false;
    }
}

void MainWindow::ResizeWindow(const LONG width, const LONG height, const POINT printPoint, HWND hWnd)
{
    RECT g_clientRect{ 0,0, width, height }; // 클라이언트 크기
    SIZE clientSize;

    AdjustWindowRect(&g_clientRect, WS_OVERLAPPEDWINDOW, false);    // 메뉴창 크기 빼고 윈도우 크기 계산
    clientSize.cx = g_clientRect.right - g_clientRect.left;
    clientSize.cy = g_clientRect.bottom - g_clientRect.top;
    MoveWindow(hWnd, printPoint.x, printPoint.y, clientSize.cx, clientSize.cy, true);   // printPoint 지점에 clientSize 크기로 출력
}

void MainWindow::CreateButton(HWND hWnd)
{
    vector<HwndInfo>* hwndInfo = instance->bitmapManager->GetHwnd(State::MAIN_MENU);

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

void MainWindow::ShowButton()
{
    for (const auto& buttonHandleIterator : instance->hwndWindow)
    {
        ShowWindow(buttonHandleIterator, SW_SHOW);                // 버튼 출력
    }
}

void MainWindow::HideButton()
{
    for (const auto& buttonHandleIterator : instance->hwndWindow)
    {
        ShowWindow(buttonHandleIterator, SW_HIDE);                // 버튼 출력
    }
}

void MainWindow::StartGameMethod()
{
    if (!isRunStartThread)
    {
        _beginthreadex(NULL, 0, StartGame, NULL, 0, NULL);
    }
}

UINT WINAPI MainWindow::StartGame(void* arg)
{
    isRunStartThread = true;
    SocketTransfer::ReleaseInstance();
    GameManager::ReleaseInstance();
    if (SocketTransfer::GetInstance()->ConnectServer())
    {
        SocketTransfer::GetInstance()->StartRecvDataThread();

        GameWindow::GetInstance()->isReset = true;
        ShowWindow(MainSystem::GetInstance()->GetWindowHwnd(State::MAIN_MENU), SW_HIDE);
        ShowWindow(MainSystem::GetInstance()->GetWindowHwnd(State::GAME), SW_SHOW);
    }
    isRunStartThread = false;

    return 0;
}

void MainWindow::GetRankingDataMethod()
{
    if (!isRunRankingThread)
    {
        _beginthreadex(NULL, 0, GetRankingData, NULL, 0, NULL);
    }
}

UINT WINAPI MainWindow::GetRankingData(void* arg)
{
    isRunRankingThread = true;

    string rankingData = HttpTransfer::GetInstance()->GetRanking();
    if (rankingData == "")
    {
        SocketTransfer::GetInstance()->PrintErrorCode(State::RANK_MENU, 0);
    }
    else
    {
        MessageBox(MainSystem::GetInstance()->GetWindowHwnd(State::MAIN_MENU),
            rankingData.c_str(), "Ranking", MB_OK);

        rankingData = "";
    }

    isRunRankingThread = false;
    return 0;
}

MainWindow* MainWindow::GetInstance()
{
	if (nullptr == instance)
	{
		instance = new MainWindow();
	}

	return nullptr;
}

void MainWindow::ReleaseInstance()
{
	delete instance;
	instance = nullptr;
}

LRESULT MainWindow::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
        GetInstance()->InitMainMenu(hWnd);
        break;
    case WM_SHOWWINDOW:
        GetInstance()->ReInitMainMenu(hWnd);
        break;
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDC_START:
            StartGameMethod();
            break;
        case IDC_RANKING:
            GetRankingDataMethod();
            break;
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

void MainWindow::MainMenuUpdate()
{
    instance->renderManager->RenderInitSetting();
    instance->renderManager->DrawWindow(State::MAIN_MENU);

    for (int i = 0; i < (int)instance->hwndWindow.size(); i++)
    {
        instance->renderManager->DrawHwnd(instance->hwndWindow[i],
            (*instance->bitmapManager->GetHwnd(State::MAIN_MENU))[i].point, 
            (*instance->bitmapManager->GetHwnd(State::MAIN_MENU))[i].size);
    }

    instance->renderManager->Render();
}
