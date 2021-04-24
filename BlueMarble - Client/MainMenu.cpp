#include "MainMenu.h"
#include "Resource.h"
#include "MainSystem.h"
#include "HttpTransfer.h"
#include "BitmapManager.h"

MainMenu* MainMenu::instance = nullptr;

MainMenu::MainMenu() {}
MainMenu::~MainMenu() {}

void MainMenu::InitMainMenu(HWND hWnd)
{
    instance->hWnd = hWnd;
    instance->hInst = (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);

    CreateButton();
    ShowButton();

    BitmapManager::GetInstance()->LoadMainMenuBitmap(instance->hInst);  // main menu bitmap loading
    MainSystem::GetInstance()->RegistUpdateCallbackFunction(MainMenuUpdate);    // main menu update callback regist
}

void MainMenu::CreateButton()
{
    instance->hStartButton = CreateWindow("button", "START", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        instance->StartButtonPoint.x, instance->StartButtonPoint.y,
        instance->StartButtonSize.cx, instance->StartButtonSize.cy,
        instance->hWnd, (HMENU)IDC_START, instance->hInst, NULL);    // 메인화면의 시작 버튼 생성

    instance->hStartButton = CreateWindow("button", "RANKING", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        instance->RankingButtonPoint.x, instance->RankingButtonPoint.y,
        instance->RankingButtonSize.cx, instance->RankingButtonSize.cy,
        instance->hWnd, (HMENU)IDC_RANKING, instance->hInst, NULL);    // 메인화면의 시작 버튼 생성
}

void MainMenu::ShowButton()
{
    ShowWindow(instance->hStartButton, SW_SHOW);                // 버튼 출력
}

void MainMenu::HideButton()
{
    ShowWindow(instance->hStartButton, SW_HIDE);                // 버튼 숨기기
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
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        }
    }
        break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        /*SelectObject(hdc, BitmapManager::GetInstance()->GetBitmap(State::MAIN_MENU, BACKGROUND));
        BitBlt(hdc, 0, 0, 100, 100, null, 0, 0, SRCCOPY);*/
        EndPaint(hWnd, &ps);
    }
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

}
