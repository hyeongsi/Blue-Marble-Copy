#include "MainMenu.h"
#include "Resource.h"
#include "MainSystem.h"
#include "HttpTransfer.h"
#include "RenderManager.h"
#include "BitmapManager.h"

MainMenu* MainMenu::instance = nullptr;

MainMenu::MainMenu() {}
MainMenu::~MainMenu() {}

void MainMenu::InitMainMenu(HWND hWnd)
{
    instance->hWnd = hWnd;
    instance->hInst = (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);

    instance->renderManager = RenderManager::GetInstance();
    instance->renderManager->Init(instance->hWnd);
    instance->bitmapManager = BitmapManager::GetInstance();

    ResizeWindow(instance->renderManager->GetClientSize()->cx, instance->renderManager->GetClientSize()->cy, POINT(300,100));

    instance->bitmapManager->LoadMainMenuHwnd();
    instance->bitmapManager->LoadMainMenuBitmap();  // main menu bitmap loading

    CreateButton();
    ShowButton();

    MainSystem::GetInstance()->RegistUpdateCallbackFunction(MainMenuUpdate);    // main menu update callback regist
}

void MainMenu::ResizeWindow(const LONG width, const LONG height, const POINT printPoint)
{
    RECT g_clientRect{ 0,0, width, height }; // 클라이언트 크기
    SIZE clientSize;

    AdjustWindowRect(&g_clientRect, WS_OVERLAPPEDWINDOW, false);    // 메뉴창 크기 빼고 윈도우 크기 계산
    clientSize.cx = g_clientRect.right - g_clientRect.left;
    clientSize.cy = g_clientRect.bottom - g_clientRect.top;
    MoveWindow(instance->hWnd, printPoint.x, printPoint.y, clientSize.cx, clientSize.cy, true);   // printPoint 지점에 clientSize 크기로 출력
}

void MainMenu::CreateButton()
{
    vector<HwndInfo>* hwndInfo = instance->bitmapManager->GetHwnd(State::MAIN_MENU);

    if (hwndInfo == nullptr)
        return;

    for (const auto& hwndIterator : *hwndInfo)
    {
        instance->hwndWindow.emplace_back(CreateWindow(hwndIterator.type.c_str(), hwndIterator.text.c_str(), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            hwndIterator.point.x, hwndIterator.point.y,
            hwndIterator.size.cx, hwndIterator.size.cy,
            instance->hWnd, (HMENU)hwndIterator.id, instance->hInst, NULL));
    }
}

void MainMenu::ShowButton()
{
    for (const auto& buttonHandleIterator : instance->hwndWindow)
    {
        ShowWindow(buttonHandleIterator, SW_SHOW);                // 버튼 출력
    }
}

void MainMenu::HideButton()
{
    for (const auto& buttonHandleIterator : instance->hwndWindow)
    {
        ShowWindow(buttonHandleIterator, SW_HIDE);                // 버튼 출력
    }
}

void MainMenu::StartGameMethod()
{
    instance->StartGame();
}

void MainMenu::StartGame()
{

}

void MainMenu::GetRankingDataMethod()
{
    instance->GetRankingData();
}

void MainMenu::GetRankingData()
{
    // 랭킹창으로 이동 후 랭킹을 보여줄 수 있도록
    // HttpTransfer::GetInstance()->GetRanking();
}

MainMenu* MainMenu::GetInstance()
{
	if (nullptr == instance)
	{
		instance = new MainMenu();
	}

	return nullptr;
}

void MainMenu::ReleaseInstance()
{
	delete instance;
	instance = nullptr;
}

LRESULT MainMenu::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
        GetInstance()->InitMainMenu(hWnd);
        break;
    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {
        case IDC_START:
            StartGameMethod();
            break;
        case IDC_RANKING:
            GetRankingDataMethod();
            break;
        }
    }
        break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
    }
        break;
    case WM_CLOSE:
        DestroyWindow(hWnd);
        break;
    case WM_DESTROY:
        MainSystem::ReleaseInstance();
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

void MainMenu::MainMenuUpdate()
{
    instance->renderManager->RenderInitSetting();
    instance->renderManager->DrawMainMenu();

    for (int i = 0; i < (int)instance->hwndWindow.size(); i++)
    {
        instance->renderManager->DrawHwnd(instance->hwndWindow[i],
            (*instance->bitmapManager->GetHwnd(State::MAIN_MENU))[i].point, 
            (*instance->bitmapManager->GetHwnd(State::MAIN_MENU))[i].size );
    }

    instance->renderManager->Render();
}
