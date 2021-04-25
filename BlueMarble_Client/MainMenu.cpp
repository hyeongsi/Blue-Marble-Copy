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

    instance->renderManager = RenderManager::GetInstance();
    instance->renderManager->Init(instance->hWnd);

    ResizeWindow(instance->renderManager->GetClientSize()->cx, instance->renderManager->GetClientSize()->cy, POINT(300,100));

    CreateButton();
    ShowButton();

    BitmapManager::GetInstance()->LoadMainMenuBitmap(instance->hInst);  // main menu bitmap loading
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
        }
    }
        break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        HDC hMemDC = CreateCompatibleDC(hdc); // 메모리 DC를 만든다
        SelectObject(hMemDC, (HBITMAP)LoadImageA(NULL, "./sprites/blueMarbleLogo.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION));
        BitBlt(hdc, 0, 0, 500, 500, hMemDC, 0, 0, SRCCOPY);
        EndPaint(hWnd, &ps);
    }
        break;
    case WM_CLOSE:
        if (hWnd)
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
   /* instance->renderManager->RenderInitSetting();
    instance->renderManager->DrawMainMenu();
    instance->renderManager->Render();*/
}
