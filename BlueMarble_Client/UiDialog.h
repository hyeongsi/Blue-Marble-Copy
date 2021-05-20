#pragma once
#include "SocketTransfer.h"

class UiDialog
{
private:
	static UiDialog* instance;

	RECT rect = { NULL };
	int BuyLandDlgState = 0;

	UiDialog();
	~UiDialog();
public:
	int landPrice = 0;

	static UiDialog* GetInstance();
	static void ReleaseInstance();

	static BOOL CALLBACK BuyLandDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam);
	int GetBuyLandDlgState();
};

