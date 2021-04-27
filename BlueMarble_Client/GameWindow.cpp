#include "GameWindow.h"
#include "RenderManager.h"
#include "BitmapManager.h"

GameWindow* GameWindow::instance = nullptr;

GameWindow::GameWindow() {}
GameWindow::~GameWindow() {}

void GameWindow::InitMainMenu(HWND hWnd)
{
    instance->hWnd = hWnd;
    instance->hInst = (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);

    instance->renderManager = RenderManager::GetInstance();
    instance->renderManager->Init(instance->hWnd);        // MainMenu <- -> Game Render
    instance->bitmapManager = BitmapManager::GetInstance();

    ResizeWindow(instance->renderManager->GetClientSize()->cx, instance->renderManager->GetClientSize()->cy, POINT(300, 100));

    instance->bitmapManager->LoadHwndData(State::GAME);
    instance->bitmapManager->LoadBitmapData(State::GAME);  // game bitmap loading

    MainSystem::GetInstance()->RegistUpdateCallbackFunction(GameUpdate);    // main menu update callback regist // MainMenu <- -> Game Render
}

void GameWindow::ResizeWindow(const LONG width, const LONG height, const POINT printPoint)
{
    RECT g_clientRect{ 0,0, width, height }; // Ŭ���̾�Ʈ ũ��
    SIZE clientSize;

    AdjustWindowRect(&g_clientRect, WS_OVERLAPPEDWINDOW, false);    // �޴�â ũ�� ���� ������ ũ�� ���
    clientSize.cx = g_clientRect.right - g_clientRect.left;
    clientSize.cy = g_clientRect.bottom - g_clientRect.top;
    MoveWindow(instance->hWnd, printPoint.x, printPoint.y, clientSize.cx, clientSize.cy, true);   // printPoint ������ clientSize ũ��� ���
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
        GetInstance()->InitMainMenu(hWnd);
        break;
    case WM_COMMAND:
        /*switch (LOWORD(wParam))
        {
        case IDC_START:
            StartGameMethod();
            break;
        case IDC_RANKING:
            GetRankingDataMethod();
            break;
        }*/
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

void GameWindow::GameUpdate()
{
    instance->renderManager->RenderInitSetting();

    // �׸���

    instance->renderManager->Render();
}
