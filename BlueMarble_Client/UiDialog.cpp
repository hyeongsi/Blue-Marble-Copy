#include "UiDialog.h"
#include "GameWindow.h"
#include "resource1.h"

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
		GetWindowRect(GameWindow::GetInstance()->g_hWnd, &instance->rect);
		SetWindowPos(hDlg, HWND_TOP, instance->rect.left + 500, instance->rect.top + 300, 0,0, SWP_NOSIZE);
		SetDlgItemText(hDlg, IDC_PRICE, string("АЁАн : " + to_string(instance->landPrice)).c_str());
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			instance->BuyLandDlgState = IDOK;
			EndDialog(hDlg, wParam);
			return true;
		case IDCANCEL:
			instance->BuyLandDlgState = IDCANCEL;
			EndDialog(hDlg, wParam);
			return true;
		}
		return false;
	case WM_CLOSE:
		PostQuitMessage(0);
		return true;
	}

	return false;
}

int UiDialog::GetBuyLandDlgState()
{
	return BuyLandDlgState;
}
