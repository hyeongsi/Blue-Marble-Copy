#include "UiDialog.h"
#include "GameWindow.h"
#include "resource1.h"

UiDialog* UiDialog::instance = nullptr;

UiDialog::UiDialog() {}
UiDialog::~UiDialog(){}

void UiDialog::AccumNSetText(HWND hDlg, int state)
{
	UINT check;
	int userMoney = 0;

	userMoney = (*GameManager::GetInstance()->GetUserMoneyVector())[buildInfoData.whosTurn];
	switch (state)
	{
	case IDC_CHECK_VILLA:
		check = SendDlgItemMessage(hDlg, IDC_CHECK_VILLA, BM_GETCHECK, 0, 0);
		if (check == BST_UNCHECKED)
		{
			instance->accumBuildPrice -= *instance->buildInfoData.villaPrice;
			buildInfoData.isBuyVilla = false;
		}
		else if (check == BST_CHECKED)
		{
			instance->accumBuildPrice += *instance->buildInfoData.villaPrice;
			buildInfoData.isBuyVilla = true;

			if ((userMoney - accumBuildPrice) < 0)
			{
				SendDlgItemMessage(hDlg, IDC_CHECK_VILLA, BM_SETCHECK, BST_UNCHECKED, 0);
				instance->accumBuildPrice -= *instance->buildInfoData.villaPrice;
				buildInfoData.isBuyVilla = false;
			}
		}
		break;
	case IDC_CHECK_BUILDING:
		check = SendDlgItemMessage(hDlg, IDC_CHECK_BUILDING, BM_GETCHECK, 0, 0);
		if (check == BST_UNCHECKED)
		{
			instance->accumBuildPrice -= *instance->buildInfoData.buildingPrice;
			buildInfoData.isBuyBuilding = false;
		}
		else if (check == BST_CHECKED)
		{
			instance->accumBuildPrice += *instance->buildInfoData.buildingPrice;
			buildInfoData.isBuyBuilding = true;

			if ((userMoney - accumBuildPrice) < 0)
			{
				SendDlgItemMessage(hDlg, IDC_CHECK_BUILDING, BM_SETCHECK, BST_UNCHECKED, 0);
				instance->accumBuildPrice -= *instance->buildInfoData.villaPrice;
				buildInfoData.isBuyVilla = false;
			}
		}
		break;
	case IDC_CHECK_HOTEL:
		check = SendDlgItemMessage(hDlg, IDC_CHECK_HOTEL, BM_GETCHECK, 0, 0);
		if (check == BST_UNCHECKED)
		{
			instance->accumBuildPrice -= *instance->buildInfoData.hotelPrice;
			buildInfoData.isBuyHotel = false;
		}
		else if (check == BST_CHECKED)
		{
			instance->accumBuildPrice += *instance->buildInfoData.hotelPrice;
			buildInfoData.isBuyHotel = true;

			if ((userMoney - accumBuildPrice) < 0)
			{
				SendDlgItemMessage(hDlg, IDC_CHECK_HOTEL, BM_SETCHECK, BST_UNCHECKED, 0);
				instance->accumBuildPrice -= *instance->buildInfoData.villaPrice;
				buildInfoData.isBuyVilla = false;
			}
		}
		break;
	default:
		break;
	}

	SetDlgItemText(hDlg, IDC_STATIC_BUILD_PRICE, string("가격 : " + to_string(instance->accumBuildPrice)).c_str());
}

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
		SetDlgItemText(hDlg, IDC_PRICE, string("가격 : " + to_string(instance->landPrice)).c_str());
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

BOOL UiDialog::BuyBuildDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	switch (iMessage)
	{
	case WM_INITDIALOG:
		GetWindowRect(GameWindow::GetInstance()->g_hWnd, &instance->rect);
		SetWindowPos(hDlg, HWND_TOP, instance->rect.left + 500, instance->rect.top + 300, 0, 0, SWP_NOSIZE);

		if(*instance->buildInfoData.villa)
			EnableWindow(GetDlgItem(hDlg, IDC_CHECK_VILLA), false);
		if (*instance->buildInfoData.building)
			EnableWindow(GetDlgItem(hDlg, IDC_CHECK_BUILDING), false);
		if (*instance->buildInfoData.hotel)
			EnableWindow(GetDlgItem(hDlg, IDC_CHECK_HOTEL), false);
		instance->accumBuildPrice = 0;
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_CHECK_VILLA:
			switch (HIWORD(wParam))
			{
			case BN_CLICKED:
				instance->AccumNSetText(hDlg, IDC_CHECK_VILLA);
				break;
			}
			break;
		case IDC_CHECK_BUILDING:
			switch (HIWORD(wParam))
			{
			case BN_CLICKED:
				instance->AccumNSetText(hDlg, IDC_CHECK_BUILDING);
				break;
			}
			break;
		case IDC_CHECK_HOTEL:
			switch (HIWORD(wParam))
			{
			case BN_CLICKED:
				instance->AccumNSetText(hDlg, IDC_CHECK_HOTEL);
				break;
			}
			break;
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

void UiDialog::SettingBuildPrice(int whosTurn, int* villaPrice, int* buildingPrice, int* hotelPrice, bool* villa, bool* building, bool* hotel)
{
	buildInfoData.whosTurn = whosTurn;
	
	buildInfoData.villaPrice = villaPrice;
	buildInfoData.buildingPrice = buildingPrice;
	buildInfoData.hotelPrice = hotelPrice;

	buildInfoData.villa = villa;
	buildInfoData.building = building;
	buildInfoData.hotel = hotel;

	buildInfoData.isBuyVilla = false;
	buildInfoData.isBuyBuilding = false;
	buildInfoData.isBuyHotel = false;
}

buildInfo UiDialog::GetBuildInfoData()
{
	return buildInfoData;
}

BOOL UiDialog::PayTollDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	switch (iMessage)
	{
	case WM_INITDIALOG:
		GetWindowRect(GameWindow::GetInstance()->g_hWnd, &instance->rect);
		SetWindowPos(hDlg, HWND_TOP, instance->rect.left + 500, instance->rect.top + 300, 0, 0, SWP_NOSIZE);
		SetDlgItemText(hDlg, IDC_STATIC_PAYTOLL, string("통행료 : " + to_string(instance->payToll)).c_str());
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			instance->BuyLandDlgState = IDOK;
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

void UiDialog::SetPriceText(int price)
{
	payToll = price;
}

BOOL UiDialog::TakeOverDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	switch (iMessage)
	{
	case WM_INITDIALOG:
		GetWindowRect(GameWindow::GetInstance()->g_hWnd, &instance->rect);
		SetWindowPos(hDlg, HWND_TOP, instance->rect.left + 500, instance->rect.top + 300, 0, 0, SWP_NOSIZE);
		SetDlgItemText(hDlg, IDC_STATIC_PAYTOLL, string("비용 : " + to_string(instance->takeOverPrice)).c_str());
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

void UiDialog::SetTakeOverPriceText(int price)
{
	takeOverPrice = price;
}

BOOL UiDialog::BuyLandMarkDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	switch (iMessage)
	{
	case WM_INITDIALOG:
		GetWindowRect(GameWindow::GetInstance()->g_hWnd, &instance->rect);
		SetWindowPos(hDlg, HWND_TOP, instance->rect.left + 500, instance->rect.top + 300, 0, 0, SWP_NOSIZE);
		SetDlgItemText(hDlg, IDC_STATIC_PRICE, string("가격 : " + to_string(instance->landMarkPrice)).c_str());
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

void UiDialog::SetLandMarkPriceText(int price)
{
	landMarkPrice = price;
}