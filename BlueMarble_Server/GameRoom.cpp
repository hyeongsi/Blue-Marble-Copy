#pragma once
#include "GameRoom.h"
#include "GameServer.h"
#include <stdio.h>

GameRoom::GameRoom(SOCKET user1, SOCKET user2)
{
	userVector.emplace_back(user1);
	userVector.emplace_back(user2);

	userPositionVector.emplace_back(0);
	userPositionVector.emplace_back(0);

	isFinishTurnProcessVector.emplace_back(false);
	isFinishTurnProcessVector.emplace_back(false);

	board = *MapManager::GetInstance()->GetBoardData(ORIGINAL);

	gameServer = GameServer::GetInstance();
}

vector<SOCKET> GameRoom::GetUserVector()
{
	return userVector;
}

void GameRoom::NextTurn()
{
	takeControlPlayer++;
	if ((int)userVector.size() <= takeControlPlayer)
	{
		takeControlPlayer = 0;
	}

	state = GameState::ROLL_DICE_SIGN;

	for (int i = 0; i < (int)isFinishTurnProcessVector.size(); i++)	// 초기화
	{
		isFinishTurnProcessVector[i] = false;
	}
}

int GameRoom::GetTakeControlPlayer()
{
	return takeControlPlayer;
}

int GameRoom::GetDiceDoubleCount()
{
	return diceDoubleCount;
}

void GameRoom::SetDiceDoubleCount(int count)
{
	diceDoubleCount = count;
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
	boardData* board = MapManager::GetInstance()->GetBoardData(ORIGINAL);

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

void GameRoom::SendRollTheDice(int value1, int value2)
{
	for (int i = 0; i < (int)userVector.size(); i++)
	{
		gameServer->MakePacket(sendPacket, &packetLastIndex, ROLL_DICE);
		gameServer->AppendPacketData(sendPacket, &packetLastIndex, takeControlPlayer, sizeof(int));
		gameServer->AppendPacketData(sendPacket, &packetLastIndex, value1, sizeof(value1));
		gameServer->AppendPacketData(sendPacket, &packetLastIndex, value2, sizeof(value2));
		gameServer->PacektSendMethod(sendPacket, userVector[i]);
	}
}

void GameRoom::MoveUserPosition(int diceValue)
{
	userPositionVector[takeControlPlayer] += diceValue;

	if (userPositionVector[takeControlPlayer] >= (int)board.mapSize * DIRECTION)
	{
		userPositionVector[takeControlPlayer] -= board.mapSize * DIRECTION;
	}
}

void GameRoom::SendFinishTurnSign()
{
	for (int i = 0; i < (int)userVector.size(); i++)
	{
		gameServer->MakePacket(sendPacket, &packetLastIndex, FINISH_THIS_TURN_PROCESS);
		gameServer->PacektSendMethod(sendPacket, userVector[i]);
	}
}

void GameRoom::CheckEndProcess(SOCKET clientSocket)
{
	int boolCount = 0;
	for (int i = 0; i < (int)userVector.size(); i++)
	{
		if (userVector[i] == clientSocket)
		{
			isFinishTurnProcessVector[i] = true;
		}

		if (true == isFinishTurnProcessVector[i])
		{
			boolCount++;
		}
	}

	if (boolCount == (int)isFinishTurnProcessVector.size())
	{
		NextTurn();
	}
}