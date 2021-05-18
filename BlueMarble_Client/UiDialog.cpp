#include "UiDialog.h"

UiDialog* UiDialog::instance = nullptr;

UiDialog::UiDialog() {}
UiDialog::~UiDialog(){}

UiDialog* UiDialog::GetInstance()
{
	if (instance == nullptr)
	{
		instance = new UiDialog();
	}
	return instance;
}

void UiDialog::ReleaseInstance()
{
	delete instance;
	instance = nullptr;
}

BOOL UiDialog::BuyLandDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	switch (iMessage)
	{
	case WM_INITDIALOG:
		//SetWindowPos(hDlg, HWND_TOP, 100, 100, 0,0,SWP_NOSIZE);
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
		case IDCANCEL:
			//EndDialog(hDlgMain, 0);
			return true;
		}
		return false;
	case WM_CLOSE:
		PostQuitMessage(0);
		return true;
	}

	return false;
}
