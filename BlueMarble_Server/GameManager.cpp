#pragma once
#include "GameManager.h"
#include "GameServer.h"
#include <random>

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
	roomVector.emplace_back(new GameRoom(user1, user2));

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
			room->state = GameState::NEXT_TURN;	// 다음턴으로 넘기기
		}
		else if (room->GetLandBoardData().villa[room->GetUserPositionVector()[room->GetTakeControlPlayer()]] == room->GetTakeControlPlayer() &&
			room->GetLandBoardData().building[room->GetUserPositionVector()[room->GetTakeControlPlayer()]] == room->GetTakeControlPlayer() &&
			room->GetLandBoardData().hotel[room->GetUserPositionVector()[room->GetTakeControlPlayer()]] == room->GetTakeControlPlayer() )
		{
			// 랜드마크 구매 메시지
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
			room->TempCheckNextTurn();
			break;
		case GameState::DESERT_ISLAND_TILE:
			room->DesertIslandMethod();
			break;
		case GameState::OLYMPIC_TILE:
			room->TempCheckNextTurn();
			break;
		case GameState::WORLD_TRABLE_TILE:
			room->TempCheckNextTurn();
			break;
		case GameState::REVENUE_TILE:
			room->TempCheckNextTurn();
			break;
		case GameState::NEXT_TURN:
			room->SendFinishTurnSign();
			break;
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
	
	int	diceValue1 = dis(gen);
	int	diceValue2 = dis(gen);

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
	switch (room->GetMapData().code[room->GetUserPositionVector()[room->GetTakeControlPlayer()]])
	{
	case START_TILE:
		room->state = GameState::NEXT_TURN;
		break;
	case LAND_TILE:
	case TOUR_TILE:
		room->state = GameState::LAND_TILE;
		break;
	case CARD_TILE:
		room->state = GameState::CARD_TILE;
		break;
	case DESERT_ISLAND_TILE:
		room->state = GameState::DESERT_ISLAND_TILE;
		break;
	case OLYMPIC_TILE:
		room->state = GameState::OLYMPIC_TILE;
		break;
	case WORLD_TRABLE_TILE:
		room->state = GameState::WORLD_TRABLE_TILE;
		break;
	case REVENUE_TILE:
		room->state = GameState::REVENUE_TILE;
		break;
	}

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
		else   // 땅 팔거나, 땅 다 팔아도 파산이면 게임오버 처리
		{
			//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
			//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
		}
	}
	else   // 미 구매 시
	{
		if (room->isDouble)
			room->state = GameState::ROLL_DICE_SIGN;
		else
			room->state = GameState::NEXT_TURN;	// 다음턴으로 넘기기
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
		memcpy(&buyBuildingPkt.isBuyVilla, &data[accumDataSize], sizeof(buyBuildingPkt.isBuyVilla));		// get usBuyVilla
		accumDataSize += sizeof(buyBuildingPkt.isBuyVilla);
		memcpy(&buyBuildingPkt.isBuyBuilding, &data[accumDataSize], sizeof(buyBuildingPkt.isBuyBuilding));	// get isBuyBuilding
		accumDataSize += sizeof(buyBuildingPkt.isBuyBuilding);
		memcpy(&buyBuildingPkt.isBuyHotel, &data[accumDataSize], sizeof(buyBuildingPkt.isBuyHotel));		// get isBuyHotel

		if ((!buyBuildingPkt.isBuyVilla) && (!buyBuildingPkt.isBuyBuilding) && (!buyBuildingPkt.isBuyHotel))	// 구입 버튼 눌렀으나 아무것도 구매하지 않았을 경우
		{
			if (room->isDouble)
				room->state = GameState::ROLL_DICE_SIGN;
			else
				room->state = GameState::NEXT_TURN;	// 다음턴으로 넘기기
		}
		else
		{
			if (buyBuildingPkt.isBuyVilla)
			{
				(*room->GetPUserMoneyVector())[buyBuildingPkt.whosTurn] -=
					room->GetMapData().villa[(room->GetUserPositionVector())[buyBuildingPkt.whosTurn]];		// 돈 차감

				room->GetPLandBoardData()->villa[room->GetUserPositionVector()[buyBuildingPkt.whosTurn]] = buyBuildingPkt.whosTurn;	// 구매 처리
				accumBuildPrice += room->GetMapData().villa[(room->GetUserPositionVector())[buyBuildingPkt.whosTurn]];
			}
			if (buyBuildingPkt.isBuyBuilding)
			{
				(*room->GetPUserMoneyVector())[buyBuildingPkt.whosTurn] -=
					room->GetMapData().building[(room->GetUserPositionVector())[buyBuildingPkt.whosTurn]];		// 돈 차감

				room->GetPLandBoardData()->building[room->GetUserPositionVector()[buyBuildingPkt.whosTurn]] = buyBuildingPkt.whosTurn;	// 구매 처리
				accumBuildPrice += room->GetMapData().building[(room->GetUserPositionVector())[buyBuildingPkt.whosTurn]];
			}
			if (buyBuildingPkt.isBuyHotel)
			{
				(*room->GetPUserMoneyVector())[buyBuildingPkt.whosTurn] -=
					room->GetMapData().hotel[(room->GetUserPositionVector())[buyBuildingPkt.whosTurn]];		// 돈 차감

				room->GetPLandBoardData()->hotel[room->GetUserPositionVector()[buyBuildingPkt.whosTurn]] = buyBuildingPkt.whosTurn;	// 구매 처리
				accumBuildPrice += room->GetMapData().hotel[(room->GetUserPositionVector())[buyBuildingPkt.whosTurn]];
			}

			room->SendBuildingSyncSign(buyBuildingPkt.whosTurn, buyBuildingPkt.isBuy,
				buyBuildingPkt.isBuyVilla, buyBuildingPkt.isBuyBuilding, buyBuildingPkt.isBuyHotel, accumBuildPrice);
		}
	}
	else    // 미 구입 시
	{
		if (room->isDouble)
			room->state = GameState::ROLL_DICE_SIGN;
		else
			room->state = GameState::NEXT_TURN;	// 다음턴으로 넘기기
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

	if (tollPrice <= (*room->GetPUserMoneyVector())[payTollSignPkt.whosTurn])
	{
		room->SendPayTollSyncSign(payTollSignPkt.whosTurn, tollPrice, true, landOwner);
	}
	else
	{
		room->SendPayTollSyncSign(payTollSignPkt.whosTurn, tollPrice, false, landOwner);
	}
		
}
