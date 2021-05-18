#pragma once
#include "SocketTransfer.h"

class UiDialog
{
private:
	static UiDialog* instance;

	UiDialog();
	~UiDialog();
public:
	static UiDialog* GetInstance();
	static void ReleaseInstance();

	static BOOL CALLBACK BuyLandDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam);
};

