#pragma once
#include "GameManager.h"
#include "GameServer.h"
#include <random>
#include "CardManager.h"

GameManager* GameManager::instance = nullptr;

GameManager::GameManager() {}
GameManager::~GameManager() {}

GameManager* GameManager::GetInstance()
{
	if (nullptr == instance)
	{
		instance = new GameManager();
	}

	return instance;
}

void GameManager::ReleaseInstance()
{
	delete instance;
	instance = nullptr;
}

void GameManager::CreateRoom(SOCKET& user1, SOCKET& user2)
{
	gameRoomVectorMutex.lock();
	roomVector.emplace_back(new GameRoom(user1, user2));
	gameRoomVectorMutex.unlock();

	char sendPacket[MAX_PACKET_SIZE] = {};
	unsigned int packetLastIndex = 0;
	GameServer::GetInstance()->MakePacket(sendPacket, &packetLastIndex, READY);
	GameServer::GetInstance()->AppendPacketData(sendPacket, &packetLastIndex, 1, sizeof(int));
	GameServer::GetInstance()->AppendPacketData(sendPacket, &packetLastIndex, 2, sizeof(int));
	GameServer::GetInstance()->AppendPacketData(sendPacket, &packetLastIndex, 2000, sizeof(int));
	GameServer::GetInstance()->PacektSendMethod(sendPacket, user1);

	GameServer::GetInstance()->MakePacket(sendPacket, &packetLastIndex, READY);
	GameServer::GetInstance()->AppendPacketData(sendPacket, &packetLastIndex, 2, sizeof(int));
	GameServer::GetInstance()->AppendPacketData(sendPacket, &packetLastIndex, 2, sizeof(int));
	GameServer::GetInstance()->AppendPacketData(sendPacket, &packetLastIndex, 2000, sizeof(int));
	GameServer::GetInstance()->PacektSendMethod(sendPacket, user2);
}

GameRoom* GameManager::GetRoom(int index)
{
	if (0 > index || index >= (int)roomVector.size())
		return nullptr;
	else
		return roomVector[index];
}

void GameManager::DeleteGameRoom(GameRoom* room)
{
	for (auto iterator = roomVector.begin(); iterator != roomVector.end(); iterator++)
	{
		if ((*iterator) != room)
			continue;

		for (int i = 0; i < (int)room->GetUserVector().size(); i++)
		{
			closesocket((*room->GetPUserVector())[i]);
		}

		roomVector.erase(iterator);
		delete room;		// 방 없애기
		room = nullptr;
		break;
	}
	
	printf("%s\n", "delete Game Room");
}

int GameManager::FindBelongRoom(SOCKET& socket)
{
	for (int i = 0; i < (int)roomVector.size(); i++)	// 게임방 속
	{
		for (int j = 0; j < (int)roomVector[i]->GetUserVector().size(); j++)	// 유저들
		{
			if (socket == roomVector[i]->GetUserVector()[j])
			{
				return i;
			}
		}
	}

	return -1;
}

void GameManager::ArriveLandTileMethod(GameRoom* room)
{
	room->state = GameState::WAIT;

	if (room->GetLandBoardData().land[room->GetUserPositionVector()[room->GetTakeControlPlayer()]] == -1)
	{
		room->SendBuyLandSign();	// 구입여부 확인 메시지 전송
	}
	else if(room->GetLandBoardData().land[room->GetUserPositionVector()[room->GetTakeControlPlayer()]] == room->GetTakeControlPlayer()) // 내 땅이면
	{
		if (room->GetLandBoardData().landMark[room->GetUserPositionVector()[room->GetTakeControlPlayer()]] == room->GetTakeControlPlayer()) // 랜드마크 있으면
		{
			room->EndTurn();
		}
		else if (room->GetLandBoardData().villa[room->GetUserPositionVector()[room->GetTakeControlPlayer()]] == room->GetTakeControlPlayer() &&
			room->GetLandBoardData().building[room->GetUserPositionVector()[room->GetTakeControlPlayer()]] == room->GetTakeControlPlayer() &&
			room->GetLandBoardData().hotel[room->GetUserPositionVector()[room->GetTakeControlPlayer()]] == room->GetTakeControlPlayer() )
		{
			room->SendBuyLandMarkSign();
		}
		else // 건물이 모두 건설되어 있지 않다면,
		{
			room->CheckLandKindNSendMessage();
		}
	}
	else  // 남의 땅이면
	{
		room->SendPayTollSign();	// 통행료 지불 요청
	}
}

UINT WINAPI GameManager::RoomLogicThread(void* arg)
{
	instance->RoomLogicThreadMethod((GameRoom*)arg);
	return 0;
}

void GameManager::RoomLogicThreadMethod(GameRoom* room)
{
	room->startTime = clock();
	for (auto& userSocket : room->GetUserVector())
	{
		room->SendMapDataMethod(userSocket);	// 맵 데이터 전송
	}
	room->finishTime = clock();
	
	while (true)
	{
		if (!room->CheckSendDelay())	// send delay check
		{
			continue;
		}

		switch (room->state)
		{
		case GameState::ROLL_DICE_SIGN:
			room->SendRollDiceSignMethod(room->GetUserVector()[room->GetTakeControlPlayer()]);	// 해당 차례 유저에게 주사위 굴리기 메시지 전송
			break;
		case GameState::LAND_TILE:
			ArriveLandTileMethod(room);
			break;
		case GameState::CARD_TILE:
			CardManager::GetInstance()->DrawCard().UseCard(room);	// 카드 뽑고 카드 사용
			break;
		case GameState::DESERT_ISLAND_TILE:
			room->DesertIslandMethod();
			break;
		case GameState::OLYMPIC_TILE:
			room->OlympicMethod();
			break;
		case GameState::WORLD_TRABLE_TILE:
			room->WorldTrableMethod();
			break;
		case GameState::REVENUE_TILE:
			room->SendRevenueSign();
			break;
		case GameState::NEXT_TURN:
			room->SendFinishTurnSign();
			break;
		case GameState::GAME_OVER:
			gameRoomVectorMutex.lock();
			if (room != nullptr)
			{
				DeleteGameRoom(room);
			}
			gameRoomVectorMutex.unlock();
			return;
		default:
			break;
		}
		
		room->finishTime = clock();
	}
}

void GameManager::RollTheDiceMethod(GameRoom* room)
{
	instance->RollTheDice(room);
}

void GameManager::RollTheDice(GameRoom* room)
{
	random_device rd;
	mt19937 gen(rd());		// random_device 를 통해 난수 생성 엔진을 초기화 한다.
	uniform_int_distribution<int> dis(1, 6);		// 1 부터 6 까지 균등하게 나타나는 난수열을 생성하기 위해 균등 분포 정의.
	
	int	diceValue1 = 10; //dis(gen);
	int	diceValue2 = 14; //dis(gen);

	if (room->IsDesertIsland() && diceValue1 == diceValue2)
	{
		room->SetIsDesertIsland(false);
		room->SetDesertIslandCount(0);
	}

	if (!room->IsDesertIsland() || (room->GetDesertIslandCount() >= ESCAPE_COUNT))
	{
		room->SendRollTheDice(diceValue1, diceValue2, false);
		room->MoveUserPosition(diceValue1 + diceValue2);		// 유저 위치 갱신
	}
	else
	{
		room->SendRollTheDice(diceValue1, diceValue2, true);
	}

	// 도착한 지역에서의 처리
	room->MoveTileProcess();

	if ((diceValue1 == diceValue2) && (!(room->GetDiceDoubleCount() >= 3)))
	{
		room->isDouble = true;
		room->SetDiceDoubleCount(room->GetDiceDoubleCount() + 1);
	}
	else
	{
		room->isDouble = false;
		room->SetDiceDoubleCount(0);
	}
}

void GameManager::BuyLandMethod(GameRoom* room, char* data)
{
	instance->BuyLand(room, data);
}

void GameManager::BuyLand(GameRoom* room, char* data)
{
	buyLandPacket bPacket;					
	memcpy(&bPacket.whosTurn, &data[1], sizeof(int));						// get turn
	memcpy(&bPacket.isBuy, &data[1 + sizeof(int)], sizeof(bool));			// get isBuy

	if (bPacket.isBuy)	// 구매 시
	{
		if ((*room->GetPUserMoneyVector())[bPacket.whosTurn] >= room->GetMapData().land[
				(room->GetUserPositionVector())[bPacket.whosTurn]
			])	// 돈 비교
		{
			(*room->GetPUserMoneyVector())[bPacket.whosTurn] -=
				room->GetMapData().land[(room->GetUserPositionVector())[bPacket.whosTurn]];		// 돈 차감

			room->GetPLandBoardData()->land[room->GetUserPositionVector()[bPacket.whosTurn]] = bPacket.whosTurn;	// 구매 처리
			room->SendLandSyncSign(bPacket.whosTurn, bPacket.isBuy);
		}
		else  // 땅 살 돈 없으면
		{
			room->EndTurn();
		}
	}
	else   // 미 구매 시
	{
		room->EndTurn();
	}
}

void GameManager::BuyBuildingMethod(GameRoom* room, char* data)
{
	instance->BuyBuilding(room, data);
}

void GameManager::BuyBuilding(GameRoom* room, char* data)
{
	buyBuildingPacket buyBuildingPkt;
	int accumDataSize = 1;
	int accumBuildPrice = 0;

	memcpy(&buyBuildingPkt.whosTurn, &data[accumDataSize], sizeof(buyBuildingPkt.whosTurn));	// get turn
	accumDataSize += sizeof(buyBuildingPkt.whosTurn);
	memcpy(&buyBuildingPkt.isBuy, &data[accumDataSize], sizeof(buyBuildingPkt.isBuy));		// get isBuy
	accumDataSize += sizeof(buyBuildingPkt.isBuy);

	if (buyBuildingPkt.isBuy)	// 구입 시
	{
		memcpy(&buyBuildingPkt.isBuyVilla, &data[accumDataSize], sizeof(buyBuildingPkt.isBuyVilla));		// get isBuyVilla
		accumDataSize += sizeof(buyBuildingPkt.isBuyVilla);
		memcpy(&buyBuildingPkt.isBuyBuilding, &data[accumDataSize], sizeof(buyBuildingPkt.isBuyBuilding));	// get isBuyBuilding
		accumDataSize += sizeof(buyBuildingPkt.isBuyBuilding);
		memcpy(&buyBuildingPkt.isBuyHotel, &data[accumDataSize], sizeof(buyBuildingPkt.isBuyHotel));		// get isBuyHotel

		if ((!buyBuildingPkt.isBuyVilla) && (!buyBuildingPkt.isBuyBuilding) && (!buyBuildingPkt.isBuyHotel))	// 구입 버튼 눌렀으나 아무것도 구매하지 않았을 경우
		{
			room->EndTurn();
		}
		else
		{
			// 구매하고자 하는 건축비용 계산
			if (buyBuildingPkt.isBuyVilla)
				accumBuildPrice += room->GetMapData().villa[(room->GetUserPositionVector())[buyBuildingPkt.whosTurn]];
			if (buyBuildingPkt.isBuyBuilding)
				accumBuildPrice += room->GetMapData().building[(room->GetUserPositionVector())[buyBuildingPkt.whosTurn]];
			if (buyBuildingPkt.isBuyHotel)
				accumBuildPrice += room->GetMapData().hotel[(room->GetUserPositionVector())[buyBuildingPkt.whosTurn]];

			// 건물들 금액이 소지한 금액보다 작다면 구매처리
			if (accumBuildPrice <= (*room->GetPUserMoneyVector())[buyBuildingPkt.whosTurn])	
			{
				if (buyBuildingPkt.isBuyVilla)
					room->GetPLandBoardData()->villa[room->GetUserPositionVector()[buyBuildingPkt.whosTurn]] = buyBuildingPkt.whosTurn;	// 구매 처리
				if (buyBuildingPkt.isBuyBuilding)
					room->GetPLandBoardData()->building[room->GetUserPositionVector()[buyBuildingPkt.whosTurn]] = buyBuildingPkt.whosTurn;	// 구매 처리
				if (buyBuildingPkt.isBuyHotel)
					room->GetPLandBoardData()->hotel[room->GetUserPositionVector()[buyBuildingPkt.whosTurn]] = buyBuildingPkt.whosTurn;	// 구매 처리

				(*room->GetPUserMoneyVector())[buyBuildingPkt.whosTurn] -= accumBuildPrice;

				room->SendBuildingSyncSign(buyBuildingPkt.whosTurn, buyBuildingPkt.isBuy,
					buyBuildingPkt.isBuyVilla, buyBuildingPkt.isBuyBuilding, buyBuildingPkt.isBuyHotel, accumBuildPrice);
			}
			else  // 건축 비용을 낼 돈이 없다면
			{
				room->CheckLandKindNSendMessage();	// 다시 건축메시지 보냄
			}
		}
	}
	else    // 미 구입 시
	{
		room->EndTurn();
	}
}

void GameManager::PayTollMethod(GameRoom* room, char* data)
{
	instance->PayToll(room, data);
}

void GameManager::PayToll(GameRoom* room, char* data)
{
	payTollSignPacket payTollSignPkt;
	int accumDataSize = 1;
	int tollPrice = 0;

	memcpy(&payTollSignPkt.whosTurn, &data[accumDataSize], sizeof(payTollSignPkt.whosTurn));	// get turn
	accumDataSize += sizeof(payTollSignPkt.whosTurn);

	int landOwner = room->GetLandBoardData().land[room->GetUserPositionVector()[payTollSignPkt.whosTurn]];

	if (room->GetLandBoardData().landMark[room->GetUserPositionVector()[payTollSignPkt.whosTurn]] == landOwner)	// 랜드마크이면
	{
		tollPrice += room->GetMapData().tollLandMark[room->GetUserPositionVector()[payTollSignPkt.whosTurn]];
	}
	else   // 랜드마크 아니면
	{
		tollPrice += room->GetMapData().tollLand[room->GetUserPositionVector()[payTollSignPkt.whosTurn]];
		if (room->GetLandBoardData().villa[room->GetUserPositionVector()[payTollSignPkt.whosTurn]] == landOwner)
		{
			tollPrice += room->GetMapData().tollVilla[room->GetUserPositionVector()[payTollSignPkt.whosTurn]];
		}
		if (room->GetLandBoardData().building[room->GetUserPositionVector()[payTollSignPkt.whosTurn]] == landOwner)
		{
			tollPrice += room->GetMapData().tollBuilding[room->GetUserPositionVector()[payTollSignPkt.whosTurn]];
		}
		if (room->GetLandBoardData().hotel[room->GetUserPositionVector()[payTollSignPkt.whosTurn]] == landOwner)
		{
			tollPrice += room->GetMapData().tollHotel[room->GetUserPositionVector()[payTollSignPkt.whosTurn]];
		}
	}

	if (room->GetLandBoardData().olympic[room->GetUserPositionVector()[room->GetTakeControlPlayer()]] != 0)
		tollPrice *= (int)pow(2, room->GetLandBoardData().olympic[room->GetUserPositionVector()[room->GetTakeControlPlayer()]]); // 올림픽 적용

	if (tollPrice <= (*room->GetPUserMoneyVector())[payTollSignPkt.whosTurn])
	{
		room->SendPayTollSignSync(payTollSignPkt.whosTurn, tollPrice, true, landOwner);
	}
	else
	{
		room->SendPayTollSignSync(payTollSignPkt.whosTurn, tollPrice, false, landOwner);
	}
		
}

void GameManager::TakeOverMethod(GameRoom* room, char* data)
{
	instance->TakeOver(room, data);
}

void GameManager::TakeOver(GameRoom* room, char* data)
{
	takeOverSignPacket takeOverSignPkt;
	int accumDataSize = 1;
	int takeOverPrice = 0;

	memcpy(&takeOverSignPkt.whosTurn, &data[accumDataSize], sizeof(takeOverSignPkt.whosTurn));	// get turn
	accumDataSize += sizeof(takeOverSignPkt.whosTurn);
	memcpy(&takeOverSignPkt.isTakeOver, &data[accumDataSize], sizeof(takeOverSignPkt.isTakeOver));	// get isTakeOver

	int owner = room->GetLandBoardData().land[room->GetUserPositionVector()[room->GetTakeControlPlayer()]];
	takeOverPrice = room->GetBuildPrice(owner) * 2;

	if (takeOverSignPkt.isTakeOver)
	{
		if ((*room->GetPUserMoneyVector())[room->GetTakeControlPlayer()] >= takeOverPrice) // 인수 비용 충분하면
		{
			owner = room->TakeOverLand(takeOverSignPkt.whosTurn, takeOverPrice);
			room->SendTakeOverSignSync(takeOverPrice, owner);
		} 
		else  // 인수 비용 없을 경우
		{
			bool isHaveLand = false;
			for (int i = 0; i < (int)room->GetMapData().land.size(); i++)
			{
				if (room->GetTakeControlPlayer() == room->GetLandBoardData().land[i])
				{
					isHaveLand = true;
					break;
				}
			}

			if (isHaveLand)   // 땅을 하나라도 가지고 있다면
			{
				room->SendSellLandSign(takeOverPrice, TAKE_OVER_LAND);	// 땅 팔기
			}
			else    // 땅을 하나라도 가지고 있지 않으면
			{
				room->EndTurn();   // 인수 취소
			}
		}
	}
	else
	{
		room->EndTurn();
	}
}

void GameManager::BuyLandMarkMethod(GameRoom* room, char* data)
{
	instance->BuyLandMark(room, data);
}

void GameManager::BuyLandMark(GameRoom* room, char* data)
{
	buyLandMarkSignPacket buyLandMarkSignPkt;
	int accumDataSize = 1;

	memcpy(&buyLandMarkSignPkt.whosTurn, &data[accumDataSize], sizeof(buyLandMarkSignPkt.whosTurn));	// get turn
	accumDataSize += sizeof(buyLandMarkSignPkt.whosTurn);
	memcpy(&buyLandMarkSignPkt.isBuy, &data[accumDataSize], sizeof(buyLandMarkSignPkt.isBuy));	// get isBuy

	int landMarkPrice = room->GetMapData().landMark[room->GetUserPositionVector()[room->GetTakeControlPlayer()]];

	if (buyLandMarkSignPkt.isBuy)  // 구입 의사 있다면
	{
		if ((*room->GetPUserMoneyVector())[room->GetTakeControlPlayer()] >= landMarkPrice) // 구입 비용 충분하면
		{
			room->BuyLandMark(landMarkPrice);	// 구입처리
			room->SendBuyLandMarkSignSync(landMarkPrice);	// 싱크 메시지 전송
		}
		else  // 랜드마크 구입 비용 없을 경우
		{
			if ((*room->GetPUserMoneyVector())[room->GetTakeControlPlayer()] + room->TotalDisposalPrice() >= landMarkPrice)
			{
				room->SendSellLandSign(landMarkPrice, BUILD_LANDMARK);	// 땅 팔기
			}
			else
			{
				room->EndTurn();   // 인수 취소
			}	
		}
	}
	else
	{
		room->EndTurn();
	}
}

void GameManager::GetCardSignSyncMethod(GameRoom* room)
{
	instance->GetCardSignSync(room);
}

void GameManager::GetCardSignSync(GameRoom* room)
{
	if ((*room->GetPUserMoneyVector())[room->GetTakeControlPlayer()] <= 0)	// 카드 사용 후 돈 없으면
	{
		room->Bankruptcy();		// 파산 처리
		return;
	}

	Card preCard = CardManager::GetInstance()->GetCardDataVector()[room->preCardId];

	if (preCard.movePosition != 0 || preCard.moveIndex != -1)	  // 이동 하는 카드라면
	{
		room->MoveTileProcess();	// 도착 타일 처리
	}
	else    // 이동 안하는 카드라면
	{
		room->EndTurn();
	}
}

void GameManager::GetTrapCardSyncMethod(GameRoom* room)
{
	instance->GetTrapCardSync(room);
}

void GameManager::GetTrapCardSync(GameRoom* room)
{
	switch (room->checkIsUsingTrapCardId)
	{
	case ESCAPE:
		room->EndTurn();
		break;
	}
}

void GameManager::GetSelectIndexMethod(GameRoom* room, char* data, char header)
{
	instance->GetSelectIndex(room, data, header);
}

void GameManager::GetSelectIndex(GameRoom* room, char* data, char header)
{
	getSelectIndexPacket getSelectIndexPkt;
	int accumDataSize = 1;

	memcpy(&getSelectIndexPkt.selectIndex, &data[accumDataSize], sizeof(getSelectIndexPkt.selectIndex));	// get selectIndex

	switch (header)
	{
	case OLYMPIC_SIGN:
		if (room->GetMapData().code[getSelectIndexPkt.selectIndex] == LAND_TILE ||
			room->GetMapData().code[getSelectIndexPkt.selectIndex] == TOUR_TILE)	// 구입가능 지역이라면
		{
			room->OlympicSyncMethod(getSelectIndexPkt.selectIndex); // 올림픽 처리
		}
		else  // 올림픽 개최 불가 장소라면,
		{
			room->OlympicMethod();  // 다시 선택
		}
		break;
	case WORLD_TRABLE_SIGN:
		if (room->GetUserPositionVector()[room->GetTakeControlPlayer()] <= getSelectIndexPkt.selectIndex)
			room->MoveUserPosition(getSelectIndexPkt.selectIndex - room->GetUserPositionVector()[room->GetTakeControlPlayer()]); // 맵인덱스 - 내위치 -> 이동거리
		else
			room->MoveUserPosition(room->GetMapData().code.size() - room->GetUserPositionVector()[room->GetTakeControlPlayer()] +
				getSelectIndexPkt.selectIndex); // 맵사이즈 - 내위치 + 맵인덱스 ==> 이동거리

		room->WorldTrableSignSyncMethod();
		break;
	}
}

void GameManager::RevenueSignMethod(GameRoom* room)
{
	instance->RevenueSign(room);
}

void GameManager::RevenueSign(GameRoom* room)
{
	if ((*room->GetPUserMoneyVector())[room->GetTakeControlPlayer()] >= TAX)	// 세금을 바로 낼 수 있다면
	{
		(*room->GetPUserMoneyVector())[room->GetTakeControlPlayer()] -= TAX;
		room->SendRevenueSignSync();
	}
	else if(room->TotalDisposalPrice() + (*room->GetPUserMoneyVector())[room->GetTakeControlPlayer()] >= TAX) // 땅 매각 후 세금 내기
	{
		room->SendSellLandSign(TAX, PAY_TAX);
	}
	else  // 세금 낼 돈이 아예 없으면
	{
		room->Bankruptcy();	// 파산 처리
	}
}

void GameManager::SelectInputKeyProcessMethod(GameRoom* room, char* data)
{
	instance->SelectInputKeyProcess(room, data);
}

void GameManager::SelectInputKeyProcess(GameRoom* room, char* data)
{
	selectInputKeyPacket selectInputKeyPkt;
	int accumDataSize = 1;

	memcpy(&selectInputKeyPkt.inputKey, &data[accumDataSize], sizeof(selectInputKeyPkt.inputKey));	// get key
	accumDataSize += sizeof(selectInputKeyPkt.inputKey);
	memcpy(&selectInputKeyPkt.currentSelectValue, &data[accumDataSize], sizeof(selectInputKeyPkt.currentSelectValue));	// get currentSelectValue

	switch (selectInputKeyPkt.inputKey)
	{
	case INPUT_SPACE:
		room->SendSelectLandIndex(selectInputKeyPkt.currentSelectValue, true);
		break;
	case INPUT_LEFT:
		room->SendSelectLandIndex(room->FindNextLand(selectInputKeyPkt.currentSelectValue, true), false);
		break;
	case INPUT_RIGHT:
		room->SendSelectLandIndex(room->FindNextLand(selectInputKeyPkt.currentSelectValue, false), false);
		break;
	}
}

void GameManager::SellLandProcessMethod(GameRoom* room, char* data)
{
	instance->SellLandProcess(room, data);
}

void GameManager::SellLandProcess(GameRoom* room, char* data)
{
	sellLandProcessPacket sellLandProcessPkt;
	int accumDataSize = 1;
	int sellPrice = 0;

	memcpy(&sellLandProcessPkt.isOK, &data[accumDataSize], sizeof(sellLandProcessPkt.isOK));	// get isOK

	if (sellLandProcessPkt.isOK)
	{
		sellPrice = room->SellLand();	// 땅 팔기 처리
		room->SendSellLandSignSync();
		(*room->GetSelectLandIndex()).clear();	// 땅 팔았으니 선택한 땅들 초기화
	}
	else   // 취소를 눌렀을 때
	{
		switch (room->beforeSellSign)
		{
		case BUILD_LANDMARK:
			if (room->GetLandBoardData().land[room->GetTakeControlPlayer()] != room->GetTakeControlPlayer()) // 랜드마크 구입하고자 하는 땅까지 팔아버렸으면
				room->EndTurn();
			else
				room->SendBuyLandMarkSign();
			break;
		case PAY_TOLL:
			room->SendPayTollSign();
			break;
		case TAKE_OVER_LAND:
			room->SendTakeOverSign(room->GetLandBoardData().land[room->GetUserPositionVector()[room->GetTakeControlPlayer()]]);
			break;
		case PAY_TAX:
			room->SendRevenueSign();
			break;
		}
	}
}

void GameManager::GetBankruptcySignMethod(GameRoom* room)
{
	instance->GetBankruptcySign(room);
}

void GameManager::GetBankruptcySign(GameRoom* room)
{
	int alivePlayerCount = 0;

	for (int i = 0; i < (int)room->GetBackruptcyVector().size(); i++)
	{
		if (room->GetBackruptcyVector()[i] == false)
		{
			alivePlayerCount++;
		}
	}
	
	if (alivePlayerCount < 2)	// 게임 진행에 필요한 최소 인원, 2명보다 작으면 게임 종료 처리
	{
		room->SendGameOverSign();
		return;
	}

	room->EndTurn();
}

void GameManager::AfterSellLandSyncMethod(GameRoom * room)
{
	instance->AfterSellLandSync(room);
}

void GameManager::AfterSellLandSync(GameRoom* room)
{
	if (room->goalPrice > (*room->GetPUserMoneyVector())[room->GetTakeControlPlayer()])  // 팔았는데도 돈이 안채워지면
	{
		room->SendSellLandSign(room->goalPrice, room->beforeSellSign);	// 다시 땅 팔기
	}
	else  // 팔고 돈 채워지면
	{
		switch (room->beforeSellSign)
		{
		case BUILD_LANDMARK:
			if (room->GetLandBoardData().land[room->GetUserPositionVector()[room->GetTakeControlPlayer()]] != room->GetTakeControlPlayer()) // 랜드마크 지을려는 땅까지 팔아버렸으면 다음턴으로
				room->NextTurn();
			else
				room->SendBuyLandMarkSign();
			break;
		case PAY_TOLL:
			room->SendPayTollSign();
			break;
		case TAKE_OVER_LAND:
			room->SendTakeOverSign(room->GetLandBoardData().land[room->GetUserPositionVector()[room->GetTakeControlPlayer()]]);
			break;
		case PAY_TAX:
			room->SendRevenueSign();
			break;
		}
	}
}