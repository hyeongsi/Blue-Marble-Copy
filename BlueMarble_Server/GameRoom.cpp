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

	userMoneyVector.emplace_back(200.0f);
	userMoneyVector.emplace_back(200.0f);

	isDouble = false;

	board = *MapManager::GetInstance()->GetBoardData(ORIGINAL);

	for (int i = 0; i < (int)(board.mapSize * DIRECTION); i++)
	{
		landBoardData.land[i] = NULL;
		landBoardData.villa[i] = NULL;
		landBoardData.building[i] = NULL;
		landBoardData.hotel[i] = NULL;
		landBoardData.landMark[i] = NULL;
	}

	gameServer = GameServer::GetInstance();
}

vector<SOCKET> GameRoom::GetUserVector()
{
	return userVector;
}

vector<float>* GameRoom::GetPUserMoneyVector()
{
	return &userMoneyVector;
}

vector<int> GameRoom::GetUserPositionVector()
{
	return userPositionVector;
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

landData GameRoom::GetLandBoardData()
{
	return landBoardData;
}

landData* GameRoom::GetPLandBoardData()
{
	return &landBoardData;
}

boardData GameRoom::GetMapData()
{
	return board;
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

	if (duration >= 2.0)
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
		printf("%s %d\n", "send MapData1 - ", socekt);

		gameServer->MakePacket(sendPacket, &packetLastIndex, NULL);
		for (int i = 0; i < (int)board->code.size(); i++)
		{
			gameServer->AppendPacketData(sendPacket, &packetLastIndex, board->name[i].size() + 1, sizeof(unsigned int));
			gameServer->AppendPacketPointerData(sendPacket, &packetLastIndex, board->name[i].c_str(), board->name[i].size());
			gameServer->AppendPacketData(sendPacket, &packetLastIndex, '\0', sizeof(char));
		}
		gameServer->PacektSendMethod(sendPacket, socekt);
		printf("%s %d\n", "send MapData2 - ", socekt);
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
	printf("%s %d\n", "send Roll Dice Msg - ", socket);

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
		userMoneyVector[takeControlPlayer] += 30.0f;	// START 지점 통과, 30만원 지급
	}
}

void GameRoom::SendPayTollSign()
{
	float toll = 0;
	//if (board.land[userPositionVector[takeControlPlayer]] != NULL)	// 땅
	//	toll += landBoardData.land[userPositionVector[takeControlPlayer]];
	//if (board.villa[userPositionVector[takeControlPlayer]] != NULL)	// 빌라
	//	toll += landBoardData.villa[userPositionVector[takeControlPlayer]];
	//if (board.building[userPositionVector[takeControlPlayer]] != NULL)	// 빌딜
	//	toll += landBoardData.building[userPositionVector[takeControlPlayer]];
	//if (board.hotel[userPositionVector[takeControlPlayer]] != NULL)	// 호텔
	//	toll += landBoardData.hotel[userPositionVector[takeControlPlayer]];
	//if (board.landMark[userPositionVector[takeControlPlayer]] != NULL)	// 랜드마크
	//	toll += landBoardData.landMark[userPositionVector[takeControlPlayer]];

	// 통행료 기준으로 계산하도록 변경하기, 위의 코드는 건설 비용 기준으로 계산하도록 되어있음

	gameServer->MakePacket(sendPacket, &packetLastIndex, PAY_TOLL_SIGN);
	gameServer->AppendPacketData(sendPacket, &packetLastIndex, toll, sizeof(float));	// 통행료 가격
	gameServer->PacektSendMethod(sendPacket, userVector[takeControlPlayer]);
	printf("%s %d\n", "send Pay_Toll_Sign - ", userVector[takeControlPlayer]);
}

void GameRoom::SendBuyLandSign(bool isTour)
{
	if (isTour)
	{
		gameServer->MakePacket(sendPacket, &packetLastIndex, BUY_TOUR_SIGN);
		gameServer->AppendPacketData(sendPacket, &packetLastIndex, takeControlPlayer, sizeof(int));	// 유저
		gameServer->AppendPacketData(sendPacket, &packetLastIndex, 
			board.landMark[userPositionVector[takeControlPlayer]], sizeof(float));	// 땅 가격
		gameServer->PacektSendMethod(sendPacket, userVector[takeControlPlayer]);
		printf("%s %d\n", "send Buy_Tour_Sign - ", userVector[takeControlPlayer]);
	}
	else
	{
		gameServer->MakePacket(sendPacket, &packetLastIndex, BUY_LAND_SIGN);
		gameServer->AppendPacketData(sendPacket, &packetLastIndex, takeControlPlayer, sizeof(int));	// 유저
		gameServer->AppendPacketData(sendPacket, &packetLastIndex, 
			board.landMark[userPositionVector[takeControlPlayer]], sizeof(float));	// 땅 가격
		gameServer->PacektSendMethod(sendPacket, userVector[takeControlPlayer]);
		printf("%s %d\n", "send Buy_Land_Sign - ", userVector[takeControlPlayer]);
	}
}

void GameRoom::SendBuyLand(bool isTour, bool isBuy)
{
	// 해당 클라 말고도 구매 정보를 보내서 동기화 할 수 있도록 하기,
	if (isTour)
	{
		gameServer->MakePacket(sendPacket, &packetLastIndex, BUY_TOUR);
		gameServer->AppendPacketData(sendPacket, &packetLastIndex, isBuy, sizeof(bool));				// 구매 유무
		gameServer->AppendPacketData(sendPacket, &packetLastIndex, takeControlPlayer, sizeof(int));		// 유저
		gameServer->AppendPacketData(sendPacket, &packetLastIndex, userMoneyVector[takeControlPlayer], sizeof(float));	// 해당 유저 돈
		gameServer->PacektSendMethod(sendPacket, userVector[takeControlPlayer]);
		printf("%s %d\n", "send Buy_Tour - ", userVector[takeControlPlayer]);
	}
	else
	{
		if (landBoardData.landMark[userPositionVector[takeControlPlayer]] != takeControlPlayer)	// 랜드마크가 건설되어 있다면
		{
			state = GameState::NEXT_TURN;
			return;
		}

		gameServer->MakePacket(sendPacket, &packetLastIndex, BUY_LAND);
		gameServer->AppendPacketData(sendPacket, &packetLastIndex, isBuy, sizeof(bool));				// 구매 유무
		gameServer->AppendPacketData(sendPacket, &packetLastIndex, takeControlPlayer, sizeof(int));		// 유저
		gameServer->AppendPacketData(sendPacket, &packetLastIndex, userMoneyVector[takeControlPlayer], sizeof(float));	// 해당 유저 돈

		gameServer->AppendPacketData(sendPacket, &packetLastIndex, board.villa[userPositionVector[takeControlPlayer]], sizeof(float));	// 빌라 가격
		gameServer->AppendPacketData(sendPacket, &packetLastIndex, board.building[userPositionVector[takeControlPlayer]], sizeof(float));	// 빌딩 가격
		gameServer->AppendPacketData(sendPacket, &packetLastIndex, board.hotel[userPositionVector[takeControlPlayer]], sizeof(float));	// 호텔 가격
		gameServer->AppendPacketData(sendPacket, &packetLastIndex, board.landMark[userPositionVector[takeControlPlayer]], sizeof(float));	// 랜드마크 가격

		gameServer->AppendPacketData(sendPacket, &packetLastIndex, landBoardData.villa[userPositionVector[takeControlPlayer]], sizeof(bool));	// 빌라 건축 유무
		gameServer->AppendPacketData(sendPacket, &packetLastIndex, landBoardData.building[userPositionVector[takeControlPlayer]], sizeof(bool));	// 빌딩 건축 유무
		gameServer->AppendPacketData(sendPacket, &packetLastIndex, landBoardData.hotel[userPositionVector[takeControlPlayer]], sizeof(bool));	// 호텔 건축 유무

		gameServer->PacektSendMethod(sendPacket, userVector[takeControlPlayer]);
		printf("%s %d\n", "send Buy_Land - ", userVector[takeControlPlayer]);
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