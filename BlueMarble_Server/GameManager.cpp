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
	GameServer::GetInstance()->AppendPacketData(sendPacket, &packetLastIndex, 200.0f, sizeof(float));
	GameServer::GetInstance()->PacektSendMethod(sendPacket, user1);

	GameServer::GetInstance()->MakePacket(sendPacket, &packetLastIndex, READY);
	GameServer::GetInstance()->AppendPacketData(sendPacket, &packetLastIndex, 2, sizeof(int));
	GameServer::GetInstance()->AppendPacketData(sendPacket, &packetLastIndex, 2, sizeof(int));
	GameServer::GetInstance()->AppendPacketData(sendPacket, &packetLastIndex, 200.0f, sizeof(float));
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
		room->SendBuyLandSign(false);
		break;
	case TOUR_TILE:
		room->SendBuyLandSign(true);
		break;
	//case CARD_TILE:
	//	room->state = GameState::CARD_SIGN;
	//	break;
	//case DESERT_ISLAND_TILE:
	//	room->state = GameState::DESERT_ISLAND_SIGN;
	//	break;
	//case OLYMPIC_TILE:
	//	room->state = GameState::OLYMPIC_SIGN;
	//	break;
	//case WORLD_TRABLE_TILE:
	//	room->state = GameState::WORLD_TRABLE_SIGN;
	//	break;
	//case REVENUE_TILE:
	//	room->state = GameState::REVENUE_SIGN;
	//	break;
	}

	if ((diceValue1 == diceValue2) && (!(room->GetDiceDoubleCount() >= 3)))
	{
		room->isDouble = true;
		room->SetDiceDoubleCount(room->GetDiceDoubleCount()+1);
	}
	else
	{
		room->isDouble = false;
		room->SetDiceDoubleCount(0);
	}
}
