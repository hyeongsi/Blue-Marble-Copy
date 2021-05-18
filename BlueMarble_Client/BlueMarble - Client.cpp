#include "framework.h"
#include "BlueMarble - Client.h"

HINSTANCE hInst;                                // 현재 인스턴스입니다.

ATOM                MyRegisterClass(HINSTANCE hInstance, State state);
BOOL                InitInstance(HINSTANCE, int);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    MyRegisterClass(hInstance, State::MAIN_MENU);
    MyRegisterClass(hInstance, State::GAME);

    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_BLUEMARBLECLIENT));

    MSG msg;
    MainSystem* mainSystem = MainSystem::GetInstance();

    while (true)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
                break;
            else
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }

        mainSystem->Update();
    }

    MainSystem::ReleaseInstance();
    return (int) msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance, State state)
{
    WNDCLASSEXW wcex;

    switch (state)
    {
    case State::MAIN_MENU:
        wcex.lpszClassName = MAIN_WINDOW_CLASSNAME;
        wcex.lpfnWndProc = MainWindow::GetInstance()->WndProc;
        break;
    case State::GAME:
        wcex.lpszClassName = GAME_WINDOW_CLASSNAME;
        wcex.lpfnWndProc = GameWindow::GetInstance()->WndProc;
        break;
    default:
        break;
    }

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_BLUEMARBLECLIENT));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName = NULL;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   mainWindowHwnd = CreateWindowW(MAIN_WINDOW_CLASSNAME, L"Main", WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   gameWindowHwnd = CreateWindowW(GAME_WINDOW_CLASSNAME, L"Game", WS_OVERLAPPEDWINDOW,
       CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   SetWindowText(mainWindowHwnd, _T("BlueMarble - Main")); // <- 이것 또한 앞 1글자만 인식
   // 왜 제목이 1글자밖에 인식을 못하는건지
   //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
   //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
   //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
   //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

   if (!mainWindowHwnd)
   {
      return FALSE;
   }

   if (!gameWindowHwnd)
   {
       return FALSE;
   }

   ShowWindow(mainWindowHwnd, nCmdShow);
   ShowWindow(gameWindowHwnd, SW_HIDE);

   MainSystem::GetInstance()->SetWindowHwnd(State::MAIN_MENU, mainWindowHwnd);
   MainSystem::GetInstance()->SetWindowHwnd(State::GAME, gameWindowHwnd);
   MainSystem::GetInstance()->SetHinstance(hInstance);

   return TRUE;
}