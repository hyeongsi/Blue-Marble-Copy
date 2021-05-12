#pragma once

#include "resource.h"
#include "MainSystem.h"
#include "MainWindow.h"
#include "GameWindow.h"

#define MAX_LOADSTRING 100

HWND mainWindowHwnd;
HWND gameWindowHwnd;

const WCHAR* MAIN_WINDOW_CLASSNAME = L"MainWindow";
const WCHAR* GAME_WINDOW_CLASSNAME = L"GameWindow";