#include "GameWindow.h"

GameWindow* GameWindow::instance = nullptr;

GameWindow::GameWindow() {}
GameWindow::~GameWindow() {}

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
        break;
    case WM_COMMAND:
    {
        /*switch (LOWORD(wParam))
        {
        case IDC_START:
            StartGameMethod();
            break;
        case IDC_RANKING:
            GetRankingDataMethod();
            break;
        }*/
    }
        break;
    case WM_CLOSE:
        DestroyWindow(hWnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
