#include "MainMenu.h"
#include "Resource.h"
#include "MainSystem.h"
#include "HttpTransfer.h"

MainMenu* MainMenu::instance = nullptr;

MainMenu::MainMenu() {}
MainMenu::~MainMenu() {}

void MainMenu::InitMainMenu(HWND hWnd)
{
    instance->hWnd = hWnd;
    instance->hInst = (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);
    instance->hStartButton = CreateWindow("button", "START", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        instance->StartButtonPoint.x, instance->StartButtonPoint.y, 
        instance->StartButtonSize.cx, instance->StartButtonSize.cy,
        hWnd, (HMENU)IDC_START, instance->hInst, NULL);    // ����ȭ���� ���� ��ư ����

    instance->hStartButton = CreateWindow("button", "RANKING", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        instance->RankingButtonPoint.x, instance->RankingButtonPoint.y,
        instance->RankingButtonSize.cx, instance->RankingButtonSize.cy,
        hWnd, (HMENU)IDC_RANKING, instance->hInst, NULL);    // ����ȭ���� ���� ��ư ����
    ShowButton();
}

void MainMenu::ShowButton()
{
    ShowWindow(instance->hStartButton, SW_SHOW);                // ��ư ���
}

void MainMenu::HideButton()
{
    ShowWindow(instance->hStartButton, SW_HIDE);                // ��ư �����
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
    // ��ŷâ���� �̵� �� ��ŷ�� ������ �� �ֵ���
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
