#pragma once
#include "GameRoom.h"
#include "GameServer.h"
#include "MapManager.h"
#include <stdio.h>

GameRoom::GameRoom(SOCKET user1, SOCKET user2)
{
	userVector.emplace_back(user1);
	userVector.emplace_back(user2);

	gameServer = GameServer::GetInstance();
}

vector<SOCKET> GameRoom::GetUserVector()
{
	return userVector;
}

void GameRoom::NextTurn()
{
	takeControlPlayer++;
	if ((int)userVector.size() >= takeControlPlayer)
	{
		takeControlPlayer = 0;
	}
}

int GameRoom::GetTakeControlPlayer()
{
	return takeControlPlayer;
}

bool GameRoom::CheckSendDelay()
{
	double duration = (finishTime - startTime) / CLOCKS_PER_SEC;

	if (duration >= 1.0)
	{
		startTime = clock();
		return true;
	}

	finishTime = clock();
	return false;
}

void GameRoom::SendMapDataMethod(SOCKET& socekt)
{
	boardData* board = MapManager::GetInstance()->GetBoardData(ORIGINAL);	// 나중에 enum 값으로 변경하기

	if (nullptr != board)
	{
		gameServer->MakePacket(sendPacket, &packetLastIndex, GET_MAPDATA);
		gameServer->AppendPacketData(sendPacket, &packetLastIndex, board->mapSize, sizeof(board->mapSize));
		for (int i = 0; i < (int)board->code.size(); i++)
		{
			gameServer->AppendPacketData(sendPacket, &packetLastIndex, board->code[i], sizeof(board->code[i]));
		}
		gameServer->PacektSendMethod(sendPacket, socekt);
		printf("%s\n", "send MapData1");

		gameServer->MakePacket(sendPacket, &packetLastIndex, NULL);
		for (int i = 0; i < (int)board->code.size(); i++)
		{
			gameServer->AppendPacketData(sendPacket, &packetLastIndex, board->name[i].size() + 1, sizeof(unsigned int));
			gameServer->AppendPacketPointerData(sendPacket, &packetLastIndex, board->name[i].c_str(), board->name[i].size());
			gameServer->AppendPacketData(sendPacket, &packetLastIndex, '\0', sizeof(char));
		}
		gameServer->PacektSendMethod(sendPacket, socekt);
		printf("%s\n", "send MapData2");
	}
	else
	{
		gameServer->PrintErrorCode(NOT_FOUND_BOARDDATA_ERROR);
	}
}

void GameRoom::SendRollDiceSignMethod(SOCKET& socket)
{
	gameServer->MakePacket(sendPacket, &packetLastIndex, ROLL_DICE_SIGN);
	gameServer->PacektSendMethod(sendPacket, socket);
	printf("%s\n", "send Roll Dice Msg");

	state = GameState::WAIT;
}

void GameRoom::SendRollTheDice(int value)
{
	for (int i = 0; i < (int)userVector.size(); i++)
	{
		gameServer->MakePacket(sendPacket, &packetLastIndex, ROLL_DICE);
		gameServer->AppendPacketData(sendPacket, &packetLastIndex, takeControlPlayer, sizeof(int));
		gameServer->AppendPacketData(sendPacket, &packetLastIndex, value, sizeof(value));
		gameServer->PacektSendMethod(sendPacket, userVector[i]);
	}
}

void GameRoom::UpdateMapData(int diceValue)
{

}

