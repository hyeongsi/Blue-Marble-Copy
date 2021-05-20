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
	if (room->GetLandBoardData().land[room->GetUserPositionVector()[room->GetTakeControlPlayer()]] == -1)
	{
		room->SendBuyLandSign();	// 구입여부 확인 메시지 전송
	}
	else if(room->GetLandBoardData().land[room->GetUserPositionVector()[room->GetTakeControlPlayer()]] == room->GetTakeControlPlayer()) // 내 땅이면
	{
		//room->SendBuyLand(isTour, true);
	}
	else  // 남의 땅이면
	{
		//room->SendPayTollSign();	// 통행료 지불 요청
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

	int diceValue1 = dis(gen);
	int diceValue2 = dis(gen);

	room->SendRollTheDice(diceValue1, diceValue2);
	room->MoveUserPosition(diceValue1 + diceValue2);		// 유저 위치 갱신

	// 도착한 지역에서의 처리
	switch (room->GetMapData().code[room->GetUserPositionVector()[room->GetTakeControlPlayer()]])
	{
	case LAND_TILE:
	case TOUR_TILE:
		ArriveLandTileMethod(room);
		break;
	case CARD_TILE:
		room->state = GameState::NEXT_TURN;
		break;
	case DESERT_ISLAND_TILE:
		room->state = GameState::NEXT_TURN;
		break;
	case OLYMPIC_TILE:
		room->state = GameState::NEXT_TURN;
		break;
	case WORLD_TRABLE_TILE:
		room->state = GameState::NEXT_TURN;
		break;
	case REVENUE_TILE:
		room->state = GameState::NEXT_TURN;
		break;
	}

	if ((diceValue1 == diceValue2) && (!(room->GetDiceDoubleCount() >= 3)))
	{
		room->state = GameState::ROLL_DICE_SIGN;
		room->isDouble = true;
		room->SetDiceDoubleCount(room->GetDiceDoubleCount()+1);
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
			//room->SendBuyLand(isTour, true);	// 구입 여부 전송
		}
		else   // 땅 팔거나, 땅 다 팔아도 파산이면 게임오버 처리
		{
			//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
			//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
		}
	}
	else   // 미 구매 시
	{
		//room->SendBuyLand(isTour, false);	// 구입 여부 전송
		room->state = GameState::NEXT_TURN;	// 다음턴으로 넘기기
	}
}
