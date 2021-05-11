#pragma once
#include "GameRoom.h"
#include "GameServer.h"
#include "MapManager.h"
#include <iostream>

GameRoom::GameRoom(SOCKET user1, SOCKET user2)
{
	userList.emplace_back(user1);
	userList.emplace_back(user2);
}

list<SOCKET> GameRoom::GetUserList()
{
	return userList;
}

void GameRoom::NextTurn()
{
	takeControlPlayer++;
	if ((unsigned int)userList.size() >= takeControlPlayer)
	{
		takeControlPlayer = 0;
	}
}

int GameRoom::GetTakeControlPlayer()
{
	return takeControlPlayer;
}

void GameRoom::SendMapDataMethod(SOCKET& socekt)
{
	boardData* board = MapManager::GetInstance()->GetBoardData(ORIGINAL);	// 나중에 enum 값으로 변경하기
	char sendPacket[MAX_PACKET_SIZE] = {};
	unsigned int packetLastIndex = 0;

	if (nullptr != board)
	{
		GameServer::GetInstance()->MakePacket(sendPacket, &packetLastIndex, GET_MAPDATA);
		GameServer::GetInstance()->AppendPacketData(sendPacket, &packetLastIndex, board->mapSize, sizeof(board->mapSize));
		for (int i = 0; i < (int)board->code.size(); i++)
		{
			GameServer::GetInstance()->AppendPacketData(sendPacket, &packetLastIndex, board->code[i], sizeof(board->code[i]));
		}
		GameServer::GetInstance()->PacektSendMethod(sendPacket, socekt);
		cout << "send MapData1" << endl;

		GameServer::GetInstance()->MakePacket(sendPacket, &packetLastIndex, NULL);
		for (int i = 0; i < (int)board->code.size(); i++)
		{
			GameServer::GetInstance()->AppendPacketData(sendPacket, &packetLastIndex, board->name[i].size() + 1, sizeof(unsigned int));
			GameServer::GetInstance()->AppendPacketPointerData(sendPacket, &packetLastIndex, board->name[i].c_str(), board->name[i].size());
			GameServer::GetInstance()->AppendPacketData(sendPacket, &packetLastIndex, '\0', sizeof(char));
		}
		GameServer::GetInstance()->PacektSendMethod(sendPacket, socekt);
		cout << "send MapData2" << endl;
	}
	else
	{
		GameServer::GetInstance()->PrintErrorCode(NOT_FOUND_BOARDDATA_ERROR);
	}
}
