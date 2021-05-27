#include "SocketTransfer.h"
#include <string>
#include <WS2tcpip.h>	// inet_pton()
#include <process.h>
#include "MainSystem.h"
#include "GameManager.h"
#include "GameWindow.h"
#include "UiDialog.h"
#include "resource1.h"

SocketTransfer* SocketTransfer::instance = nullptr;

SocketTransfer::SocketTransfer() {}
SocketTransfer::~SocketTransfer() {}

void SocketTransfer::RecvDataMethod(SOCKET clientSocket)
{
	char header = NULL;

	while (nullptr != recvThreadHandle)
	{
		char cBuffer[MAX_PACKET_SIZE] = {};
		header = NULL;

		if ((recv(clientSocket, cBuffer, MAX_PACKET_SIZE, 0)) == -1)
		{
			PrintErrorCode(State::GAME, RECV_ERROR);
			break;
		}

		if (nullptr != recvCBF)
		{
			recvCBF(cBuffer);
		}
		else
		{
			memcpy(&header, &cBuffer[0], sizeof(char));

			switch (header)	// 나중에 enum 값으로 변경하기
			{
			case GET_MAPDATA:
				GetMapDataMethod1(cBuffer);
				break;
			case READY:
				GetReadyMethod(cBuffer);
				break;
			case ROLL_DICE_SIGN:
				GetRollDiceSignMethod(cBuffer);
				break;
			case ROLL_DICE:
				GetRollDiceMethod(cBuffer);
				break;
			case BUY_LAND_SIGN:
				BuyLandSignMethod(cBuffer);
				break;
			case BUY_BUILDING_SIGN:
				BuyBuildingMethod(cBuffer);
				break;
			case PAY_TOLL_SIGN:
				PayTollSignMethod(cBuffer);
				break;
			case TAKE_OVER_SIGN:
				TakeOverSignMethod(cBuffer);
				break;
			case BUY_LANDMARK_SIGN:
				BuyLandMarkSignMethod(cBuffer);
				break;
			case BUY_LAND_SYNC:
				GetBuyLandSyncMethod(cBuffer);
				break;
			case BUY_BUILDING_SYNC:
				GetBuyBuildSyncMethod(cBuffer);
				break;
			case PAY_TOLL_SIGN_SYNC:
				GetPayTollSignSyncMethod(cBuffer);
				break;
			case TAKE_OVER_SYNC:
				GetTakeOverSyncMethod(cBuffer);
				break;
			case BUY_LANDMARK_SIGN_SYNC:
				GetBuyLandMarkSyncMethod(cBuffer);
				break;
			case FINISH_THIS_TURN_PROCESS:
				SendNextTurnSignMethod();
				break;
			default:
				break;
			}
		}
	}

	TerminateRecvDataThread();
}

UINT WINAPI SocketTransfer::RecvDataThread(void* arg)
{
	instance->RecvDataMethod(GetInstance()->clientSocket);
	return 0;
}

void SocketTransfer::GetMapDataMethod1(char* packet)
{
	instance->GetMapData1(packet);
}

void SocketTransfer::GetMapData1(char* packet)
{
	mapPacket1 _mapPacket1;
	int code = 0;

	memcpy(&_mapPacket1.mapSize, &packet[1], sizeof(unsigned int));	// get mapSize
	for (int i = 0; i < (int)_mapPacket1.mapSize * DIRECTION; i++)
	{
		memcpy(&code, &packet[1 + sizeof(unsigned int) + (i*sizeof(int))], sizeof(int));	// get mapCode
		_mapPacket1.code.emplace_back(code);
	}

	GameManager::GetInstance()->GetAddressBoardData()->mapSize = _mapPacket1.mapSize;
	GameManager::GetInstance()->GetAddressBoardData()->code = _mapPacket1.code;

	GameManager::GetInstance()->SetGameMessage("");	// 메시지 초기화
	recvCBF = GetMapDataMethod2;
}

void SocketTransfer::GetMapDataMethod2(char* packet)
{
	instance->GetMapData2(packet);
}

void SocketTransfer::GetMapData2(char* packet)
{
	mapPacket2 _mapPacket2;
	char str[100];
	unsigned int stackMemorySize = 0;

	for (int i = 0; i < 8 * DIRECTION; i++)
	{
		memcpy(&_mapPacket2.charSize, &packet[stackMemorySize], sizeof(unsigned int));	// get charSize
		stackMemorySize += sizeof(unsigned int);

		strcpy_s(str, _mapPacket2.charSize, &packet[stackMemorySize]);
		stackMemorySize += _mapPacket2.charSize;

		_mapPacket2.name.emplace_back(str);
		GameManager::GetInstance()->GetAddressBoardData()->owner.emplace_back(0);	// 땅 소유자 추가
	}

	GameManager::GetInstance()->GetAddressBoardData()->name = _mapPacket2.name;
	RenderManager::GetInstance()->InitDrawBoardMap();

	recvCBF = nullptr;
}

void SocketTransfer::GetReadyMethod(char* packet)
{
	readyPacket rPacket;
	memcpy(&rPacket.header, &packet[0], sizeof(char));					// get ready sign
	memcpy(&rPacket.number, &packet[1], sizeof(int));		// get number
	memcpy(&rPacket.playerCount, &packet[1 + sizeof(int)], sizeof(int));		// get playerCount
	memcpy(&rPacket.initMoney, &packet[1 + sizeof(int) + sizeof(int)], sizeof(int));		// get initMoney

	for (int i = 0; i < rPacket.playerCount; i++)
	{
		GameManager::GetInstance()->GetUserPositionVector()->emplace_back(0);
		GameManager::GetInstance()->GetUserMoneyVector()->emplace_back(rPacket.initMoney);
	}
	GameManager::GetInstance()->SetCharacterIndex(rPacket.number);
	GameManager::GetInstance()->SetPlayerCount(rPacket.playerCount);

	MakePacket(READY);
	SendMessageToGameServer();
}

void SocketTransfer::GetRollDiceSignMethod(char* packet)
{
	instance->GetRollDiceSign(packet);
}

void SocketTransfer::GetRollDiceSign(char* packet)
{
	GameManager::GetInstance()->SetIsMyDiceTurn(true);
	GameManager::GetInstance()->SetGameState(GameState::ROLL_DICE);
	GameManager::GetInstance()->SetGameMessage("주사위를 돌려주세요");
	
	GameWindow::GetInstance()->ShowButton();
}

void SocketTransfer::GetRollDiceMethod(char* packet)
{
	instance->GetRollDice(packet);
	instance->recvCBF = nullptr;
}

void SocketTransfer::GetRollDice(char* packet)
{
	diceRollPacket dPacket;
	int accumDataSize = 1;

	memcpy(&dPacket.whosTurn, &packet[accumDataSize], sizeof(dPacket.whosTurn));	// get turn
	accumDataSize += sizeof(dPacket.whosTurn);
	memcpy(&dPacket.diceValue1, &packet[accumDataSize], sizeof(dPacket.diceValue1));	// get diceValue1
	accumDataSize += sizeof(dPacket.diceValue1);
	memcpy(&dPacket.diceValue2, &packet[accumDataSize], sizeof(dPacket.diceValue2));	// get diceValue2
	accumDataSize += sizeof(dPacket.diceValue2);
	memcpy(&dPacket.plusMoney, &packet[accumDataSize], sizeof(dPacket.plusMoney));		// get plusMoney
	accumDataSize += sizeof(dPacket.plusMoney);
	memcpy(&dPacket.isDesertIsland, &packet[accumDataSize], sizeof(dPacket.isDesertIsland));		// get isDesertIsland

	if (dPacket.diceValue1 == dPacket.diceValue2)
	{
		GameManager::GetInstance()->SetGameMessage("더블!! " + to_string(dPacket.diceValue1) + " , " + to_string(dPacket.diceValue2));	// 메시지 갱신
	}
	else if(dPacket.plusMoney != 0) // START 지점을 지나 추가 자금을 받았을 경우
	{
		GameManager::GetInstance()->SetGameMessage(to_string(dPacket.diceValue1) + " , " + to_string(dPacket.diceValue2) + "\n"
			+ " 출발지를 지나 "+ to_string(dPacket.plusMoney) + "을 획득했습니다.");	// 메시지 갱신

		(*GameManager::GetInstance()->GetUserMoneyVector())[dPacket.whosTurn] += dPacket.plusMoney;
	}
	else
	{
		GameManager::GetInstance()->SetGameMessage(to_string(dPacket.diceValue1) + " , " + to_string(dPacket.diceValue2));	// 메시지 갱신
	}
	GameWindow::GetInstance()->HideButton();

	if (!dPacket.isDesertIsland)	// 감옥이 아니라면 이동 처리
	{
		GameManager::GetInstance()->MoveUserPosition(dPacket.whosTurn, dPacket.diceValue1 + dPacket.diceValue2);
	}
}

void SocketTransfer::BuyLandSignMethod(char* packet)
{
	instance->BuyLandSign(packet);
}

void SocketTransfer::BuyLandSign(char* packet)
{
	buyLandPacket bPacket;				
	memcpy(&bPacket.whosTurn, &packet[1], sizeof(int));						// get turn
	memcpy(&bPacket.passPrice, &packet[1 + sizeof(int)], sizeof(int));	// get passPrice

	UiDialog::GetInstance()->landPrice = bPacket.passPrice;
	DialogBox(MainSystem::GetInstance()->GetHinstance(), MAKEINTRESOURCE(IDD_BUY_MENU1),
		GameWindow::GetInstance()->g_hWnd, UiDialog::GetInstance()->BuyLandDlgProc);

	if (UiDialog::GetInstance()->GetBuyLandDlgState() == IDOK)
	{
		MakePacket(BUY_LAND_SIGN);
		AppendPacketData(bPacket.whosTurn, sizeof(int));	// 누가 지불하는지,
		AppendPacketData(true, sizeof(bool));	// 구매 유무
		SendMessageToGameServer();
	}
	else
	{
		MakePacket(BUY_LAND_SIGN);
		AppendPacketData(bPacket.whosTurn, sizeof(int));	// 누가 지불하는지,
		AppendPacketData(false, sizeof(bool));	// 구매 유무
		SendMessageToGameServer();
	}
}

void SocketTransfer::BuyBuildingMethod(char* packet)
{
	instance->BuyBuilding(packet);
}

void SocketTransfer::BuyBuilding(char* packet)
{
	buyBuildingPacket buyBuildingPkt;
	int accumDataSize = 1;
	memcpy(&buyBuildingPkt.whosTurn, &packet[accumDataSize], sizeof(buyBuildingPkt.whosTurn));			// get turn
	accumDataSize += sizeof(buyBuildingPkt.whosTurn);

	memcpy(&buyBuildingPkt.villaPrice, &packet[accumDataSize], sizeof(buyBuildingPkt.villaPrice));		// get villaPrice
	accumDataSize += sizeof(buyBuildingPkt.villaPrice);
	memcpy(&buyBuildingPkt.buildingPrice, &packet[accumDataSize], sizeof(buyBuildingPkt.buildingPrice));	// get buildingPrice
	accumDataSize += sizeof(buyBuildingPkt.buildingPrice);
	memcpy(&buyBuildingPkt.hotelPrice, &packet[accumDataSize], sizeof(buyBuildingPkt.hotelPrice));		// get hotelPrice
	accumDataSize += sizeof(buyBuildingPkt.hotelPrice);

	memcpy(&buyBuildingPkt.isBuildVilla, &packet[accumDataSize], sizeof(buyBuildingPkt.isBuildVilla));	// get isBuyVilla
	accumDataSize += sizeof(buyBuildingPkt.isBuildVilla);
	memcpy(&buyBuildingPkt.isBuildBuilding, &packet[accumDataSize], sizeof(buyBuildingPkt.isBuildBuilding));	// get isBuyBuilding
	accumDataSize += sizeof(buyBuildingPkt.isBuildBuilding);
	memcpy(&buyBuildingPkt.isBuildHotel, &packet[accumDataSize], sizeof(buyBuildingPkt.isBuildHotel));	// get isBuyHotel
	accumDataSize += sizeof(buyBuildingPkt.isBuildHotel);

	memcpy(&buyBuildingPkt.userMoney, &packet[accumDataSize], sizeof(buyBuildingPkt.userMoney));	// get userMoney
	(*GameManager::GetInstance()->GetUserMoneyVector())[buyBuildingPkt.whosTurn] = buyBuildingPkt.userMoney;

	UiDialog::GetInstance()->SettingBuildPrice(buyBuildingPkt.whosTurn,&buyBuildingPkt.villaPrice, &buyBuildingPkt.buildingPrice, &buyBuildingPkt.hotelPrice,
		&buyBuildingPkt.isBuildVilla, &buyBuildingPkt.isBuildBuilding, &buyBuildingPkt.isBuildHotel);
	DialogBox(MainSystem::GetInstance()->GetHinstance(), MAKEINTRESOURCE(IDD_BUY_MENU2),
		GameWindow::GetInstance()->g_hWnd, UiDialog::GetInstance()->BuyBuildDlgProc);

	if (UiDialog::GetInstance()->GetBuyLandDlgState() == IDOK)
	{
		MakePacket(BUY_BUILDING_SIGN);
		AppendPacketData(buyBuildingPkt.whosTurn, sizeof(int));	// 누가 지불하는지,
		AppendPacketData(true, sizeof(bool));	// 구매 유무
		AppendPacketData(UiDialog::GetInstance()->GetBuildInfoData().isBuyVilla, sizeof(bool));		// 빌라 구매 유무
		AppendPacketData(UiDialog::GetInstance()->GetBuildInfoData().isBuyBuilding, sizeof(bool));	// 빌딩 구매 유무
		AppendPacketData(UiDialog::GetInstance()->GetBuildInfoData().isBuyHotel, sizeof(bool));		// 호텔 구매 유무
		SendMessageToGameServer();
	}
	else
	{
		MakePacket(BUY_BUILDING_SIGN);
		AppendPacketData(buyBuildingPkt.whosTurn, sizeof(int));	// 누가 지불하는지,
		AppendPacketData(false, sizeof(bool));	// 구매 유무
		SendMessageToGameServer();
	}
}

void SocketTransfer::PayTollSignMethod(char* packet)
{
	instance->PayTollSign(packet);
}

void SocketTransfer::PayTollSign(char* packet)
{
	payTollPacket payTollPkt;

	int accumDataSize = 1;
	memcpy(&payTollPkt.whosTurn, &packet[accumDataSize], sizeof(payTollPkt.whosTurn));		// get turn
	accumDataSize += sizeof(payTollPkt.whosTurn);
	memcpy(&payTollPkt.passPrice, &packet[accumDataSize], sizeof(payTollPkt.passPrice));	// get passPrice

	UiDialog::GetInstance()->SetPriceText(payTollPkt.passPrice);

	DialogBox(MainSystem::GetInstance()->GetHinstance(), MAKEINTRESOURCE(IDD_TOLL_MENU),
		GameWindow::GetInstance()->g_hWnd, UiDialog::GetInstance()->PayTollDlgProc);

	if (UiDialog::GetInstance()->GetBuyLandDlgState() == IDOK)
	{
		MakePacket(PAY_TOLL_SIGN);
		AppendPacketData(payTollPkt.whosTurn, sizeof(int));	// 누가 지불하는지,
		SendMessageToGameServer();
	}
}

void SocketTransfer::TakeOverSignMethod(char* packet)
{
	instance->TakeOverSign(packet);
}

void SocketTransfer::TakeOverSign(char* packet)
{
	takeOverPacket takeOverPkt;

	int accumDataSize = 1;
	memcpy(&takeOverPkt.whosTurn, &packet[accumDataSize], sizeof(takeOverPkt.whosTurn));		// get turn
	accumDataSize += sizeof(takeOverPkt.whosTurn);
	memcpy(&takeOverPkt.takeOverPrice, &packet[accumDataSize], sizeof(takeOverPkt.takeOverPrice));// get takeOverPrice

	UiDialog::GetInstance()->SetTakeOverPriceText(takeOverPkt.takeOverPrice);

	DialogBox(MainSystem::GetInstance()->GetHinstance(), MAKEINTRESOURCE(IDD_TAKE_OVER_LAND),
		GameWindow::GetInstance()->g_hWnd, UiDialog::GetInstance()->TakeOverDlgProc);

	MakePacket(TAKE_OVER_SIGN);
	AppendPacketData(takeOverPkt.whosTurn, sizeof(int));	// 누가 지불하는지,
	if (UiDialog::GetInstance()->GetBuyLandDlgState() == IDOK)
		AppendPacketData(true , sizeof(bool));	// 인수 유무
	else
		AppendPacketData(false, sizeof(bool));	// 인수 유무
	SendMessageToGameServer();
}

void SocketTransfer::BuyLandMarkSignMethod(char* packet)
{
	instance->BuyLandMarkSign(packet);
}

void SocketTransfer::BuyLandMarkSign(char* packet)
{
	buyLandMarkSignPacket buyLandMarkSignPkt;

	int accumDataSize = 1;
	memcpy(&buyLandMarkSignPkt.whosTurn, &packet[accumDataSize], sizeof(buyLandMarkSignPkt.whosTurn));		// get turn
	accumDataSize += sizeof(buyLandMarkSignPkt.whosTurn);
	memcpy(&buyLandMarkSignPkt.landMarkPrice, &packet[accumDataSize], sizeof(buyLandMarkSignPkt.landMarkPrice));// get landmarkPrice

	UiDialog::GetInstance()->SetLandMarkPriceText(buyLandMarkSignPkt.landMarkPrice);

	DialogBox(MainSystem::GetInstance()->GetHinstance(), MAKEINTRESOURCE(IDD_BUY_LANDMARK),
		GameWindow::GetInstance()->g_hWnd, UiDialog::GetInstance()->TakeOverDlgProc);

	MakePacket(BUY_LANDMARK_SIGN);
	AppendPacketData(buyLandMarkSignPkt.whosTurn, sizeof(int));	// 누가 지불하는지,
	if (UiDialog::GetInstance()->GetBuyLandDlgState() == IDOK)
		AppendPacketData(true, sizeof(bool));	// 인수 유무
	else
		AppendPacketData(false, sizeof(bool));	// 인수 유무
	SendMessageToGameServer();
}

void SocketTransfer::GetBuyLandSyncMethod(char* packet)
{
	instance->GetBuyLandSync(packet);
}

void SocketTransfer::GetBuyLandSync(char* packet)
{
	buyLandSyncPacket buyLandSyncPkt;
	int accumDataSize = 1;

	memcpy(&buyLandSyncPkt.isBuy, &packet[accumDataSize], sizeof(bool));			// get isbuy
	accumDataSize += sizeof(bool);
	memcpy(&buyLandSyncPkt.whosTurn, &packet[accumDataSize], sizeof(int));		    // get turn
	accumDataSize += sizeof(int);
	memcpy(&buyLandSyncPkt.landPrice, &packet[accumDataSize], sizeof(int));		// get landPrice
	accumDataSize += sizeof(int);
	memcpy(&buyLandSyncPkt.userMoney, &packet[accumDataSize], sizeof(int));		// get usermoney
	accumDataSize += sizeof(int);

	GameManager::GetInstance()->SetGameMessage("구입 - " + to_string(buyLandSyncPkt.landPrice));	// 메시지 갱신
	(*GameManager::GetInstance()->GetUserMoneyVector())[buyLandSyncPkt.whosTurn] = buyLandSyncPkt.userMoney;

	if (buyLandSyncPkt.isBuy)
	{
		GameManager::GetInstance()->GetAddressBoardData()->owner[
			(*GameManager::GetInstance()->GetUserPositionVector())[buyLandSyncPkt.whosTurn]] = buyLandSyncPkt.whosTurn+1;
	}
	
	if(buyLandSyncPkt.whosTurn == GameManager::GetInstance()->GetCharacterIndex()-1)
	{ 
		MakePacket(BUY_LAND_SYNC);
		SendMessageToGameServer();
	}
}

void SocketTransfer::GetBuyBuildSyncMethod(char* packet)
{
	instance->GetBuyBuildSync(packet);
}

void SocketTransfer::GetBuyBuildSync(char* packet)
{
	buyBuildingSyncPacket buyBuildingSyncPkt;
	int accumDataSize = 1;
	string buyMessageString = "구입 - ";

	memcpy(&buyBuildingSyncPkt.isBuy, &packet[accumDataSize], sizeof(buyBuildingSyncPkt.isBuy));	// get isbuy
	accumDataSize += sizeof(buyBuildingSyncPkt.isBuy);
	memcpy(&buyBuildingSyncPkt.whosTurn, &packet[accumDataSize], sizeof(buyBuildingSyncPkt.whosTurn));  // get turn
	accumDataSize += sizeof(buyBuildingSyncPkt.whosTurn);

	memcpy(&buyBuildingSyncPkt.isBuyVilla, &packet[accumDataSize], sizeof(buyBuildingSyncPkt.isBuyVilla));	// get isBuyVilla
	accumDataSize += sizeof(buyBuildingSyncPkt.isBuyVilla);
	memcpy(&buyBuildingSyncPkt.isBuyBuilding, &packet[accumDataSize], sizeof(buyBuildingSyncPkt.isBuyBuilding));// get isBuyBuilding
	accumDataSize += sizeof(buyBuildingSyncPkt.isBuyBuilding);
	memcpy(&buyBuildingSyncPkt.isBuyHotel, &packet[accumDataSize], sizeof(buyBuildingSyncPkt.isBuyHotel));	// get isBuyHotel
	accumDataSize += sizeof(buyBuildingSyncPkt.isBuyHotel);

	memcpy(&buyBuildingSyncPkt.accumPrice, &packet[accumDataSize], sizeof(buyBuildingSyncPkt.accumPrice));	// get accumPrice
	accumDataSize += sizeof(buyBuildingSyncPkt.accumPrice);
	memcpy(&buyBuildingSyncPkt.userMoney, &packet[accumDataSize], sizeof(buyBuildingSyncPkt.userMoney));	// get usermoney

	if (buyBuildingSyncPkt.isBuy)
	{
		if (buyBuildingSyncPkt.isBuyVilla)
		{
			GameManager::GetInstance()->GetAddressBoardBuildData()->villa[
				(*GameManager::GetInstance()->GetUserPositionVector())[buyBuildingSyncPkt.whosTurn]
			] = buyBuildingSyncPkt.isBuyVilla;

			buyMessageString += "빌라 ";
		}
			
		if (buyBuildingSyncPkt.isBuyBuilding)
		{
			GameManager::GetInstance()->GetAddressBoardBuildData()->building[
				(*GameManager::GetInstance()->GetUserPositionVector())[buyBuildingSyncPkt.whosTurn]
			] = buyBuildingSyncPkt.isBuyBuilding;

			buyMessageString += " 빌딩 ";
		}
			
		if (buyBuildingSyncPkt.isBuyHotel)
		{
			GameManager::GetInstance()->GetAddressBoardBuildData()->hotel[
				(*GameManager::GetInstance()->GetUserPositionVector())[buyBuildingSyncPkt.whosTurn]
			] = buyBuildingSyncPkt.isBuyHotel;

			buyMessageString += " 호텔 ";
		}

		buyMessageString += (" " + to_string(buyBuildingSyncPkt.accumPrice));
		GameManager::GetInstance()->SetGameMessage(buyMessageString);	// 메시지 갱신
	}

	(*GameManager::GetInstance()->GetUserMoneyVector())[buyBuildingSyncPkt.whosTurn] = buyBuildingSyncPkt.userMoney;	// 돈 갱신

	if (buyBuildingSyncPkt.whosTurn == GameManager::GetInstance()->GetCharacterIndex() - 1)
	{
		MakePacket(BUY_BUILDING_SYNC);
		SendMessageToGameServer();
	}
}

void SocketTransfer::GetPayTollSignSyncMethod(char* packet)
{
	instance->GetPayTollSignSync(packet);
}

void SocketTransfer::GetPayTollSignSync(char* packet)
{
	payTollSyncPacket payTollSyncPkt;
	int accumDataSize = 1;

	memcpy(&payTollSyncPkt.isPass, &packet[accumDataSize], sizeof(payTollSyncPkt.isPass));	// get isPass
	accumDataSize += sizeof(payTollSyncPkt.isPass);
	memcpy(&payTollSyncPkt.whosTurn, &packet[accumDataSize], sizeof(payTollSyncPkt.whosTurn));  // get turn
	accumDataSize += sizeof(payTollSyncPkt.whosTurn);
	memcpy(&payTollSyncPkt.landOwner, &packet[accumDataSize], sizeof(payTollSyncPkt.landOwner));  // get landOwner
	accumDataSize += sizeof(payTollSyncPkt.landOwner);
	memcpy(&payTollSyncPkt.toll, &packet[accumDataSize], sizeof(payTollSyncPkt.toll));  // get toll
	accumDataSize += sizeof(payTollSyncPkt.toll);
	memcpy(&payTollSyncPkt.userMoney, &packet[accumDataSize], sizeof(payTollSyncPkt.userMoney));  // get userMoney
	accumDataSize += sizeof(payTollSyncPkt.userMoney);
	memcpy(&payTollSyncPkt.landOwnerMoney, &packet[accumDataSize], sizeof(payTollSyncPkt.landOwnerMoney));  // get landOwnerMoney

	if (payTollSyncPkt.isPass)
		GameManager::GetInstance()->SetGameMessage("통행료 지불 완료 - " + to_string(payTollSyncPkt.toll));	// 메시지 갱신
	else
		GameManager::GetInstance()->SetGameMessage("소지자금 부족, 통행료 미지급 - " + to_string(payTollSyncPkt.toll));	// 메시지 갱신

	(*GameManager::GetInstance()->GetUserMoneyVector())[payTollSyncPkt.whosTurn] = payTollSyncPkt.userMoney;	// 돈 갱신
	(*GameManager::GetInstance()->GetUserMoneyVector())[payTollSyncPkt.landOwner] = payTollSyncPkt.landOwnerMoney;	// 돈 갱신

	if (payTollSyncPkt.whosTurn == GameManager::GetInstance()->GetCharacterIndex() - 1)
	{
		MakePacket(PAY_TOLL_SIGN_SYNC);
		SendMessageToGameServer();
	}
}

void SocketTransfer::GetTakeOverSyncMethod(char* packet)
{
	instance->GetTakeOverSync(packet);
}

void SocketTransfer::GetTakeOverSync(char* packet)
{
	takeOverSyncPacket takeOverSyncPkt;
	int accumDataSize = 1;

	memcpy(&takeOverSyncPkt.whosTurn, &packet[accumDataSize], sizeof(takeOverSyncPkt.whosTurn));  // get turn
	accumDataSize += sizeof(takeOverSyncPkt.whosTurn);
	memcpy(&takeOverSyncPkt.takeOverPrice, &packet[accumDataSize], sizeof(takeOverSyncPkt.takeOverPrice));	// get price
	accumDataSize += sizeof(takeOverSyncPkt.takeOverPrice);
	memcpy(&takeOverSyncPkt.owner, &packet[accumDataSize], sizeof(takeOverSyncPkt.owner));	// get owner
	accumDataSize += sizeof(takeOverSyncPkt.owner);
	memcpy(&takeOverSyncPkt.userMoney, &packet[accumDataSize], sizeof(takeOverSyncPkt.userMoney));	// get usermoney
	accumDataSize += sizeof(takeOverSyncPkt.userMoney);
	memcpy(&takeOverSyncPkt.ownerMoney, &packet[accumDataSize], sizeof(takeOverSyncPkt.ownerMoney));	// get ownerMoney

	GameManager::GetInstance()->SetGameMessage("인수 완료 - " + to_string(takeOverSyncPkt.takeOverPrice) + " 지불");	// 메시지 갱신

	(*GameManager::GetInstance()->GetUserMoneyVector())[takeOverSyncPkt.whosTurn] = takeOverSyncPkt.userMoney;	// 돈 갱신
	(*GameManager::GetInstance()->GetUserMoneyVector())[takeOverSyncPkt.owner] = takeOverSyncPkt.ownerMoney;	// 돈 갱신
	
	// 땅 인수 처리
	GameManager::GetInstance()->GetAddressBoardData()->owner[
		(*GameManager::GetInstance()->GetUserPositionVector())[takeOverSyncPkt.whosTurn]] = takeOverSyncPkt.whosTurn + 1;

	if (takeOverSyncPkt.whosTurn == GameManager::GetInstance()->GetCharacterIndex() - 1)
	{
		MakePacket(TAKE_OVER_SYNC);
		SendMessageToGameServer();
	}
}

void SocketTransfer::GetBuyLandMarkSyncMethod(char* packet)
{
	instance->GetBuyLandMarkSync(packet);
}

void SocketTransfer::GetBuyLandMarkSync(char* packet)
{
	buyLandMarkSyncPacket buyLandMarkSyncPkt;
	int accumDataSize = 1;

	memcpy(&buyLandMarkSyncPkt.whosTurn, &packet[accumDataSize], sizeof(buyLandMarkSyncPkt.whosTurn));  // get turn
	accumDataSize += sizeof(buyLandMarkSyncPkt.whosTurn);
	memcpy(&buyLandMarkSyncPkt.landMarkPrice, &packet[accumDataSize], sizeof(buyLandMarkSyncPkt.landMarkPrice));	// get price
	accumDataSize += sizeof(buyLandMarkSyncPkt.landMarkPrice);
	memcpy(&buyLandMarkSyncPkt.userMoney, &packet[accumDataSize], sizeof(buyLandMarkSyncPkt.userMoney));	// get userMoney

	GameManager::GetInstance()->SetGameMessage("랜드마크 구매 - " + to_string(buyLandMarkSyncPkt.landMarkPrice) + " 지불");	// 메시지 갱신

	(*GameManager::GetInstance()->GetUserMoneyVector())[buyLandMarkSyncPkt.whosTurn] = buyLandMarkSyncPkt.userMoney;	// 돈 갱신
	GameManager::GetInstance()->GetAddressBoardBuildData()->landMark
		[(*GameManager::GetInstance()->GetUserPositionVector())[buyLandMarkSyncPkt.whosTurn]] = true;	// 랜드마크 구매 처리

	if (buyLandMarkSyncPkt.whosTurn == GameManager::GetInstance()->GetCharacterIndex() - 1)
	{
		MakePacket(BUY_LANDMARK_SIGN_SYNC);
		SendMessageToGameServer();
	}
}

void SocketTransfer::SendNextTurnSignMethod()
{
	instance->SendNextTurnSign();
}

void SocketTransfer::SendNextTurnSign()
{
	GameManager::GetInstance()->SetIsMyDiceTurn(false);
	MakePacket(FINISH_THIS_TURN_PROCESS);
	SendMessageToGameServer();
}

void SocketTransfer::PrintErrorCode(State state, const int errorCode)
{
	switch (state)
	{
	case State::MAIN_MENU:
	case State::RANK_MENU:
		MessageBox(MainSystem::GetInstance()->GetWindowHwnd(State::MAIN_MENU),
			("error code : " + to_string(errorCode)).c_str(), "error 발생", MB_OK);
		break;
	case State::GAME:
		MessageBox(MainSystem::GetInstance()->GetWindowHwnd(State::GAME),
			("error code : " + to_string(errorCode)).c_str(), "error 발생", MB_OK);
		break;
	default:
		break;
	}
	
}

SocketTransfer* SocketTransfer::GetInstance()
{
	if (nullptr == instance)
	{
		instance = new SocketTransfer();

		if (0 != WSAStartup(MAKEWORD(2, 2), &instance->wsaData))
		{
			instance->PrintErrorCode(State::GAME, WSASTARTUP_ERROR);
		}
	}

	return instance;
}

void SocketTransfer::ReleaseInstance()
{
	delete instance;
	instance = nullptr;
	WSACleanup();
}

bool SocketTransfer::ConnectServer()
{
	clientSocket = socket(PF_INET, SOCK_STREAM, 0);

	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(PORT);
	inet_pton(AF_INET, SERVER_IP, &serverAddress.sin_addr);

	int connectResult = connect(clientSocket, (SOCKADDR*)&serverAddress, sizeof(serverAddress));
	if (connectResult != 0)
	{
		PrintErrorCode(State::GAME, CONNECT_ERROR);
		return false;
	}

	return true;
}

void SocketTransfer::StartRecvDataThread()
{
	if (nullptr != recvThreadHandle)
	{
		TerminateRecvDataThread();
	}

	recvThreadHandle = (HANDLE)_beginthreadex(NULL, 0, GetInstance()->RecvDataThread, nullptr, 0, NULL);   // recv thread start
}

void SocketTransfer::TerminateRecvDataThread()
{
	recvThreadMutex.lock();
	recvThreadHandle = nullptr;
	recvThreadMutex.unlock();
}

void SocketTransfer::MakePacket(char header)
{
	if(NULL != header)
	{
		memset(sendPacket, 0, MAX_PACKET_SIZE);		// 패킷 초기화
		sendPacket[0] = header;	// header setting
		packetLastIndex = 1;
	}
	else
	{
		memset(sendPacket, 0, MAX_PACKET_SIZE);		// 패킷 초기화
		packetLastIndex = 0;
	}
}

void SocketTransfer::AppendPacketPointerData(char* data, unsigned int dataSize)
{
	memcpy(&sendPacket[packetLastIndex], data, dataSize);
	packetLastIndex += dataSize;
}

void SocketTransfer::SendMessageToGameServer()
{
	if (send(clientSocket, sendPacket, MAX_PACKET_SIZE, 0) == -1)
	{
		PrintErrorCode(State::GAME, SEND_ERROR);
	}

	switch (sendPacket[0])	// header check
	{
	case GET_MAPDATA:
		recvCBF = GetMapDataMethod1;
		break;
	}
}

void SocketTransfer::RegistRecvCallbackFunction(CALLBACK_FUNC_PACKET cbf)
{
	recvCBF = cbf;
}

void SocketTransfer::SendRollDiceSign()
{
	recvCBF = GetRollDiceMethod;

	MakePacket(ROLL_DICE_SIGN);
	SendMessageToGameServer();
}
