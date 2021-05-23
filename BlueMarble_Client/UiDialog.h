#pragma once
#include "SocketTransfer.h"

typedef struct BuildInfo
{
	int whosTurn = 0;

	int* villaPrice = nullptr;
	int* buildingPrice = nullptr;
	int* hotelPrice = nullptr;

	bool* villa = nullptr;
	bool* building = nullptr;
	bool* hotel = nullptr;

	bool isBuyVilla = false;
	bool isBuyBuilding = false;
	bool isBuyHotel = false;
}buildInfo;

class UiDialog
{
private:
	static UiDialog* instance;

	RECT rect = { NULL };
	int BuyLandDlgState = 0;
	buildInfo buildInfoData;
	int accumBuildPrice = 0;
	int payToll = 0;

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
	void SettingBuildPrice(int whosTurn, int* villaPrice, int* buildingPrice, int* hotelPrice, bool* villa, bool* building, bool* hotel);
	buildInfo GetBuildInfoData();

	static BOOL CALLBACK PayTollDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam);
	void SetPriceText(int price);
};

