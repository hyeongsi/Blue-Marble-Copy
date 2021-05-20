#pragma once
#include "SocketTransfer.h"

typedef struct BuildInfo
{
	int* villaPrice = nullptr;
	int* buildingPrice = nullptr;
	int* hotelPrice = nullptr;

	bool* villa = nullptr;
	bool* building = nullptr;
	bool* hotel = nullptr;
}buildInfo;

class UiDialog
{
private:
	static UiDialog* instance;

	RECT rect = { NULL };
	int BuyLandDlgState = 0;
	buildInfo buildInfoData;
	int accumBuildPrice = 0;

	UiDialog();
	~UiDialog();

	void AccumNSetText(HWND hDlg, int state);
public:
	int landPrice = 0;

	static UiDialog* GetInstance();
	static void ReleaseInstance();

	static BOOL CALLBACK BuyLandDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam);
	int GetBuyLandDlgState();

	static BOOL CALLBACK BuyBuildDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam);
	void SettingBuildPrice(int* villaPrice, int* buildingPrice, int* hotelPrice, bool* villa, bool* building, bool* hotel);
};

