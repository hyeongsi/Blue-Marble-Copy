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
	GameServer::GetInstance()->AppendPacketData(sendPacket, &packetLastIndex, roomVector.size() - 1, sizeof(int));
	GameServer::GetInstance()->AppendPacketData(sendPacket, &packetLastIndex, 1, sizeof(int));
	GameServer::GetInstance()->AppendPacketData(sendPacket, &packetLastIndex, 2, sizeof(int));
	GameServer::GetInstance()->PacektSendMethod(sendPacket, user1);

	GameServer::GetInstance()->MakePacket(sendPacket, &packetLastIndex, READY);
	GameServer::GetInstance()->AppendPacketData(sendPacket, &packetLastIndex, roomVector.size() - 1, sizeof(int));
	GameServer::GetInstance()->AppendPacketData(sendPacket, &packetLastIndex, 2, sizeof(int));
	GameServer::GetInstance()->AppendPacketData(sendPacket, &packetLastIndex, 2, sizeof(int));
	GameServer::GetInstance()->PacektSendMethod(sendPacket, user2);
}

GameRoom* GameManager::GetRoom(int index)
{
	if (0 > index || index >= (int)roomVector.size())
		return nullptr;
	else
		return roomVector[index];
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
		case GameState::ROLL_DICE:
			//RollTheDiceMethod(room);
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
	uniform_int_distribution<int> dis(2, 12);		// 2 부터 12 까지 균등하게 나타나는 난수열을 생성하기 위해 균등 분포 정의.

	int diceValue = dis(gen);

	room->SendRollTheDice(diceValue);
	room->UpdateMapData(diceValue);		// 맵데이터 업데이트 하도록
}
