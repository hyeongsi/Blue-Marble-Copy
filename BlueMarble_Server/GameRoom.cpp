#pragma once
#include "GameRoom.h"
#include "GameServer.h"
#include <stdio.h>
#include "HttpTransfer.h"

GameRoom::GameRoom(vector<unsigned int> userSocketVector)
{
	for (int i = 0; i < (int)userSocketVector.size(); i++)
	{
		userVector.emplace_back(userSocketVector[i]);
		userPositionVector.emplace_back(0);
		isFinishTurnProcessVector.emplace_back(false);
		userMoneyVector.emplace_back(START_MONEY);
		bankruptcyVector.emplace_back(false);
		isDesertIsland.emplace_back(false);
		desertIslandCount.emplace_back(0);
	}

	isDouble = false;

	board = *MapManager::GetInstance()->GetBoardData(ORIGINAL);

	for (int i = 0; i < (int)(board.mapSize * DIRECTION); i++)
	{
		landBoardData.land[i] = -1;
		landBoardData.villa[i] = -1;
		landBoardData.building[i] = -1;
		landBoardData.hotel[i] = -1;
		landBoardData.landMark[i] = -1;
		landBoardData.olympic[i] = 0;
	}

	gameServer = GameServer::GetInstance();
}

vector<SOCKET> GameRoom::GetUserVector()
{
	return userVector;
}

vector<SOCKET>* GameRoom::GetPUserVector()
{
	return &userVector;
}

vector<int>* GameRoom::GetPUserMoneyVector()
{
	return &userMoneyVector;
}

vector<int> GameRoom::GetUserPositionVector()
{
	return userPositionVector;
}

vector<bool> GameRoom::GetBackruptcyVector()
{
	return bankruptcyVector;
}

vector<Card>* GameRoom::GetPHoldCard()
{
	return &holdCard;
}

void GameRoom::NextTurn()
{
	takeControlPlayer++;
	
	while (true)	// 파산 당하지 않은 유저 차례면 탈출
	{
		if ((int)userVector.size() <= takeControlPlayer)
		{
			takeControlPlayer = 0;
		}

		if (!bankruptcyVector[takeControlPlayer])	// 파산 당하지 않았으면 탈출
			break;

		takeControlPlayer++;
	}

	isCheckTrapCard = false;
	checkIsUsingTrapCardId = -1;

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

bool GameRoom::IsDesertIsland()
{
	return isDesertIsland[takeControlPlayer];
}

int GameRoom::GetDesertIslandCount()
{
	return desertIslandCount[takeControlPlayer];
}

void GameRoom::SetIsDesertIsland(bool isDesert)
{
	isDesertIsland[takeControlPlayer] = isDesert;
}

void GameRoom::SetDesertIslandCount(int count)
{
	desertIslandCount[takeControlPlayer] = count;
}

vector<int>* GameRoom::GetSelectLandIndex()
{
	return &selectLandIndex;
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

	packetLastIndexMutex.lock();
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
	packetLastIndexMutex.unlock();
}

void GameRoom::SendRollDiceSignMethod(SOCKET& socket)
{
	packetLastIndexMutex.lock();
	gameServer->MakePacket(sendPacket, &packetLastIndex, ROLL_DICE_SIGN);
	gameServer->PacektSendMethod(sendPacket, socket);
	printf("%s %d\n", "send Roll Dice Msg - ", socket);

	for (int i = 0; i < (int)userVector.size(); i++)
	{
		if (takeControlPlayer == i)
			continue;

		gameServer->MakePacket(sendPacket, &packetLastIndex, SYNC_TURN);
		gameServer->AppendPacketData(sendPacket, &packetLastIndex, takeControlPlayer, sizeof(takeControlPlayer));	// 턴
		gameServer->PacektSendMethod(sendPacket, userVector[i]);
	}

	state = GameState::WAIT;
	packetLastIndexMutex.unlock();
}

void GameRoom::DesertIslandMethod()
{
	if (!isDesertIsland[takeControlPlayer])	// 처음 무인도 진입 시
	{
		if (CheckTrapCard(ESCAPE) && isCheckTrapCard == false)	// 사용할 탈출 카드가 있고, 카드 사용 여부 안물어봤으면
		{
			state = GameState::WAIT;
			SendIsUseCardSign(ESCAPE);
			return;
		}
		isDesertIsland[takeControlPlayer] = true;	// 무인도 집어넣고
	}
	else
	{
		if (isDouble)
		{
			isDesertIsland[takeControlPlayer] = false;
			desertIslandCount[takeControlPlayer] = 0;
		}
		else
		{
			desertIslandCount[takeControlPlayer]++;	// 처음이 아니면 카운트 ++
		}
	}

	state = GameState::NEXT_TURN;
}

void GameRoom::WorldTrableMethod()
{
	packetLastIndexMutex.lock();
	state = GameState::WAIT;
	gameServer->MakePacket(sendPacket, &packetLastIndex, WORLD_TRABLE_SIGN);
	gameServer->AppendPacketData(sendPacket, &packetLastIndex, userPositionVector[takeControlPlayer], sizeof(userPositionVector[takeControlPlayer]));	// 위치
	gameServer->PacektSendMethod(sendPacket, userVector[takeControlPlayer]);
	packetLastIndexMutex.unlock();
}

void GameRoom::WorldTrableSignSyncMethod()
{
	packetLastIndexMutex.lock();
	gameServer->MakePacket(sendPacket, &packetLastIndex, WORLD_TRABLE_SIGN_SYNC);
	gameServer->AppendPacketData(sendPacket, &packetLastIndex, takeControlPlayer, sizeof(int));	// 차례
	gameServer->AppendPacketData(sendPacket, &packetLastIndex, userPositionVector[takeControlPlayer], 
		sizeof(userPositionVector[takeControlPlayer])); // 위치
	gameServer->AppendPacketData(sendPacket, &packetLastIndex, userMoneyVector[takeControlPlayer],
		sizeof(userMoneyVector[takeControlPlayer])); // 돈

	for (auto& socketIterator : userVector)
	{
		gameServer->PacektSendMethod(sendPacket, socketIterator);
		printf("%s %d\n", "send WorldTrableSignSync - ", socketIterator);
	}
	packetLastIndexMutex.unlock();
}

void GameRoom::OlympicMethod()
{
	packetLastIndexMutex.lock();
	state = GameState::WAIT;
	gameServer->MakePacket(sendPacket, &packetLastIndex, OLYMPIC_SIGN);
	gameServer->AppendPacketData(sendPacket, &packetLastIndex, userPositionVector[takeControlPlayer], sizeof(userPositionVector[takeControlPlayer]));	// 위치
	gameServer->PacektSendMethod(sendPacket, userVector[takeControlPlayer]);
	packetLastIndexMutex.unlock();
}

void GameRoom::OlympicSyncMethod(int selectIndex)
{
	packetLastIndexMutex.lock();
	landBoardData.olympic[selectIndex] += 1;	// 올림픽 개최 여부 및 중첩 변경

	gameServer->MakePacket(sendPacket, &packetLastIndex, OLYMPIC);
	gameServer->AppendPacketData(sendPacket, &packetLastIndex, takeControlPlayer, sizeof(int));	// 차례
	gameServer->AppendPacketData(sendPacket, &packetLastIndex, selectIndex,
		sizeof(selectIndex)); // 위치
	gameServer->AppendPacketData(sendPacket, &packetLastIndex, landBoardData.olympic[selectIndex],
		sizeof(landBoardData.olympic[selectIndex])); // 위치

	for (auto& socketIterator : userVector)
	{
		gameServer->PacektSendMethod(sendPacket, socketIterator);
		printf("%s %d\n", "send OLYMPIC SYNC - ", socketIterator);
	}
	packetLastIndexMutex.unlock();
}

void GameRoom::SendRollTheDice(int value1, int value2, bool isDesertIsland)
{
	int salary = SALARY;
	bool isPassStartTile = false;	// 월급 처리 유무
	if ((userPositionVector[takeControlPlayer] + value1 + value2) >= (int)board.mapSize * DIRECTION)
	{
		isPassStartTile = true;
	}

	for (int i = 0; i < (int)userVector.size(); i++)
	{
		gameServer->MakePacket(sendPacket, &packetLastIndex, ROLL_DICE);
		gameServer->AppendPacketData(sendPacket, &packetLastIndex, takeControlPlayer, sizeof(int));
		gameServer->AppendPacketData(sendPacket, &packetLastIndex, value1, sizeof(value1));
		gameServer->AppendPacketData(sendPacket, &packetLastIndex, value2, sizeof(value2));
		if (isPassStartTile)
			gameServer->AppendPacketData(sendPacket, &packetLastIndex, salary, sizeof(int));
		else
			gameServer->AppendPacketData(sendPacket, &packetLastIndex, 0, sizeof(int));

		gameServer->AppendPacketData(sendPacket, &packetLastIndex, isDesertIsland, sizeof(isDesertIsland));
		gameServer->PacektSendMethod(sendPacket, userVector[i]);
		printf("%s %d\n", "send ROLL_DICE - ", userVector[i]);
	}
}

int GameRoom::MoveUserPosition(int diceValue)
{
	bool isPassStartTile = false;

	userPositionVector[takeControlPlayer] += diceValue;

	if (userPositionVector[takeControlPlayer] >= (int)board.mapSize * DIRECTION)
	{
		userPositionVector[takeControlPlayer] -= board.mapSize * DIRECTION;
		userMoneyVector[takeControlPlayer] += SALARY;	// START 지점 통과, 30만원 지급
		isPassStartTile = true;
	}
	else if (userPositionVector[takeControlPlayer] < 0)
	{
		userPositionVector[takeControlPlayer] += board.mapSize * DIRECTION;
	}

	return isPassStartTile;
}

void GameRoom::MoveTileProcess()
{
	switch(board.code[userPositionVector[takeControlPlayer]])
	{
	case START_TILE:
		state = GameState::NEXT_TURN;
		break;
	case LAND_TILE:
	case TOUR_TILE:
		state = GameState::LAND_TILE;
		break;
	case CARD_TILE:
		state = GameState::CARD_TILE;
		break;
	case DESERT_ISLAND_TILE:
		state = GameState::DESERT_ISLAND_TILE;
		break;
	case OLYMPIC_TILE:
		state = GameState::OLYMPIC_TILE;
		break;
	case WORLD_TRABLE_TILE:
		state = GameState::WORLD_TRABLE_TILE;
		break;
	case REVENUE_TILE:
		state = GameState::REVENUE_TILE;
		break;
	}
}

void GameRoom::SendPayTollSign()
{
	int toll = 0;

	if (landBoardData.landMark[userPositionVector[takeControlPlayer]] != -1)	// 랜드마크 소유자라면
	{
		toll += board.tollLandMark[userPositionVector[takeControlPlayer]];
	}
	else
	{
		toll += board.tollLand[userPositionVector[takeControlPlayer]];	// 땅

		if (landBoardData.villa[userPositionVector[takeControlPlayer]] != -1)	// 빌라
			toll += board.tollVilla[userPositionVector[takeControlPlayer]];
		if (landBoardData.building[userPositionVector[takeControlPlayer]] != -1)	// 빌딜
			toll += board.tollBuilding[userPositionVector[takeControlPlayer]];
		if (landBoardData.hotel[userPositionVector[takeControlPlayer]] != -1)	// 호텔
			toll += board.tollHotel[userPositionVector[takeControlPlayer]];
	}

	if (landBoardData.olympic[userPositionVector[takeControlPlayer]] != 0)
		toll *= (int)pow(2, landBoardData.olympic[userPositionVector[takeControlPlayer]]);	// 올림픽 가격 적용

	packetLastIndexMutex.lock();
	gameServer->MakePacket(sendPacket, &packetLastIndex, PAY_TOLL_SIGN);
	gameServer->AppendPacketData(sendPacket, &packetLastIndex, takeControlPlayer, sizeof(takeControlPlayer));	// 턴
	gameServer->AppendPacketData(sendPacket, &packetLastIndex, toll, sizeof(toll));	// 통행료 가격
	gameServer->PacektSendMethod(sendPacket, userVector[takeControlPlayer]);
	printf("%s %d\n", "send Pay_Toll_Sign - ", userVector[takeControlPlayer]);
	packetLastIndexMutex.unlock();
}

void GameRoom::SendBuyLandSign()
{
	packetLastIndexMutex.lock();
	gameServer->MakePacket(sendPacket, &packetLastIndex, BUY_LAND_SIGN);
	gameServer->AppendPacketData(sendPacket, &packetLastIndex, takeControlPlayer, sizeof(int));	// 유저
	gameServer->AppendPacketData(sendPacket, &packetLastIndex,
		board.land[userPositionVector[takeControlPlayer]], sizeof(int));	// 땅 가격
	gameServer->PacektSendMethod(sendPacket, userVector[takeControlPlayer]);
	printf("%s %d\n", "send Buy_Land_Sign - ", userVector[takeControlPlayer]);
	packetLastIndexMutex.unlock();
}

void GameRoom::SendLandSyncSign(int turn, bool isBuy)
{
	packetLastIndexMutex.lock();
	gameServer->MakePacket(sendPacket, &packetLastIndex, BUY_LAND_SYNC);
	gameServer->AppendPacketData(sendPacket, &packetLastIndex, isBuy, sizeof(bool));		// 구매 유무
	gameServer->AppendPacketData(sendPacket, &packetLastIndex, turn, sizeof(int));		// 유저
	gameServer->AppendPacketData(sendPacket, &packetLastIndex, board.land[userPositionVector[turn]], sizeof(int));	// 땅 가격
	gameServer->AppendPacketData(sendPacket, &packetLastIndex, userMoneyVector[turn], sizeof(int));	// 해당 유저 돈
	
	for (auto& socketIterator : userVector)
	{
		gameServer->PacektSendMethod(sendPacket, socketIterator);
		printf("%s %d\n", "send Buy_Land_Sync - ", socketIterator);
	}
	packetLastIndexMutex.unlock();
}

void GameRoom::SendBuildingSyncSign(int turn, bool isBuy, bool isBuyVilla, bool isBuyBuilding, bool isBuyHotel, int accumPrice)
{
	packetLastIndexMutex.lock();
	gameServer->MakePacket(sendPacket, &packetLastIndex, BUY_BUILDING_SYNC);
	gameServer->AppendPacketData(sendPacket, &packetLastIndex, isBuy, sizeof(isBuy));	// 구매 유무
	gameServer->AppendPacketData(sendPacket, &packetLastIndex, turn, sizeof(turn));		// 유저

	gameServer->AppendPacketData(sendPacket, &packetLastIndex, isBuyVilla, sizeof(isBuyVilla));	// 빌라 구매유무
	gameServer->AppendPacketData(sendPacket, &packetLastIndex, isBuyBuilding, sizeof(isBuyBuilding));	// 빌딩 구매유무
	gameServer->AppendPacketData(sendPacket, &packetLastIndex, isBuyHotel, sizeof(isBuyHotel));	// 호텔 구매유무

	gameServer->AppendPacketData(sendPacket, &packetLastIndex, accumPrice, sizeof(accumPrice));	// 총 가격
	gameServer->AppendPacketData(sendPacket, &packetLastIndex, userMoneyVector[turn], sizeof(userMoneyVector[turn]));	// 해당 유저 돈

	for (auto& socketIterator : userVector)
	{
		gameServer->PacektSendMethod(sendPacket, socketIterator);
		printf("%s %d\n", "send Buy_Build_Sync - ", socketIterator);
	}
	packetLastIndexMutex.unlock();
}

void GameRoom::SendPayTollSignSync(int turn, int tollPrice, bool isPass, int landOwner)
{
	packetLastIndexMutex.lock();
	int userMoney = userMoneyVector[turn] - tollPrice;
	int ownerMoney = userMoneyVector[landOwner] + tollPrice;

	gameServer->MakePacket(sendPacket, &packetLastIndex, PAY_TOLL_SIGN_SYNC);
	gameServer->AppendPacketData(sendPacket, &packetLastIndex, isPass, sizeof(isPass));	// 통행료 지불 유무
	gameServer->AppendPacketData(sendPacket, &packetLastIndex, turn, sizeof(turn));		// 유저
	gameServer->AppendPacketData(sendPacket, &packetLastIndex, landOwner, sizeof(landOwner));	// 땅 주인 번호
	gameServer->AppendPacketData(sendPacket, &packetLastIndex, tollPrice, sizeof(tollPrice));	// 통행료

	if (isPass)
	{
		gameServer->AppendPacketData(sendPacket, &packetLastIndex, userMoney, sizeof(userMoneyVector[turn]));	// 해당 유저 돈
		gameServer->AppendPacketData(sendPacket, &packetLastIndex, ownerMoney, sizeof(userMoneyVector[landOwner]));	// 땅 주인 돈
	}
	else
	{
		gameServer->AppendPacketData(sendPacket, &packetLastIndex, userMoneyVector[turn], sizeof(userMoneyVector[turn]));	// 해당 유저 돈
		gameServer->AppendPacketData(sendPacket, &packetLastIndex, userMoneyVector[landOwner], sizeof(userMoneyVector[landOwner]));	// 땅 주인 돈
	}
	
	for (auto& socketIterator : userVector)
	{
		gameServer->PacektSendMethod(sendPacket, socketIterator);
		printf("%s %d\n", "send PayTollSignSync - ", socketIterator);
	}
	packetLastIndexMutex.unlock();
}

void GameRoom::CheckLandKindNSendMessage()
{
	bool isBuyVilla;
	bool isBuyBuilding;
	bool isBuyHotel;

	packetLastIndexMutex.lock();
	if (board.code[userPositionVector[takeControlPlayer]] == LAND_TILE) // Land
	{
		gameServer->MakePacket(sendPacket, &packetLastIndex, BUY_BUILDING_SIGN);
		gameServer->AppendPacketData(sendPacket, &packetLastIndex, takeControlPlayer, sizeof(int));		// 유저
		gameServer->AppendPacketData(sendPacket, &packetLastIndex, board.villa[userPositionVector[takeControlPlayer]], sizeof(int));	// 빌라 가격
		gameServer->AppendPacketData(sendPacket, &packetLastIndex, board.building[userPositionVector[takeControlPlayer]], sizeof(int));	// 빌딩 가격
		gameServer->AppendPacketData(sendPacket, &packetLastIndex, board.hotel[userPositionVector[takeControlPlayer]], sizeof(int));	// 호텔 가격

		isBuyVilla = (landBoardData.villa[userPositionVector[takeControlPlayer]] == takeControlPlayer);
		gameServer->AppendPacketData(sendPacket, &packetLastIndex, isBuyVilla, sizeof(bool));				// 빌라 구매유무
		isBuyBuilding = (landBoardData.building[userPositionVector[takeControlPlayer]] == takeControlPlayer);
		gameServer->AppendPacketData(sendPacket, &packetLastIndex, isBuyBuilding, sizeof(bool));			// 빌딩 구매유무
		isBuyHotel = (landBoardData.hotel[userPositionVector[takeControlPlayer]] == takeControlPlayer);
		gameServer->AppendPacketData(sendPacket, &packetLastIndex, isBuyHotel, sizeof(bool));				// 호텔 구매유무

		gameServer->AppendPacketData(sendPacket, &packetLastIndex, userMoneyVector[takeControlPlayer], sizeof(int));	// 유저 돈
		gameServer->PacektSendMethod(sendPacket, userVector[takeControlPlayer]);
		printf("%s %d\n", "send Buy_Building_Sign - ", userVector[takeControlPlayer]);
	}
	else  // Tour
	{
		EndTurn();
	}
	packetLastIndexMutex.unlock();
}

void GameRoom::SendTakeOverSign(int landOwner)
{
	int takeOverPrice = GetBuildPrice(landOwner);

	takeOverPrice *= 2;	// 인수 비용은 구입 비용의 2배

	packetLastIndexMutex.lock();
	gameServer->MakePacket(sendPacket, &packetLastIndex, TAKE_OVER_SIGN);	// 인수 메시지
	gameServer->AppendPacketData(sendPacket, &packetLastIndex, takeControlPlayer, sizeof(takeControlPlayer));	// 턴
	gameServer->AppendPacketData(sendPacket, &packetLastIndex, takeOverPrice, sizeof(takeOverPrice));	// 인수비용
	gameServer->PacektSendMethod(sendPacket, userVector[takeControlPlayer]);
	packetLastIndexMutex.unlock();
}

void GameRoom::SendTakeOverSignSync(int takeOverPrice, int owner)
{
	packetLastIndexMutex.lock();
	gameServer->MakePacket(sendPacket, &packetLastIndex, TAKE_OVER_SYNC);
	gameServer->AppendPacketData(sendPacket, &packetLastIndex, takeControlPlayer, sizeof(takeControlPlayer));	// 유저
	gameServer->AppendPacketData(sendPacket, &packetLastIndex, takeOverPrice, sizeof(takeOverPrice));	// 인수비용
	gameServer->AppendPacketData(sendPacket, &packetLastIndex, owner, sizeof(owner));	// 원래 땅주인
	gameServer->AppendPacketData(sendPacket, &packetLastIndex, 
		userMoneyVector[takeControlPlayer], sizeof(userMoneyVector[takeControlPlayer]));	// 돈
	gameServer->AppendPacketData(sendPacket, &packetLastIndex, 
		userMoneyVector[owner], sizeof(userMoneyVector[owner]));	// 원래 땅주인 돈

	for (auto& socketIterator : userVector)
	{
		gameServer->PacektSendMethod(sendPacket, socketIterator);
		printf("%s %d\n", "send PayTakeOverSync - ", socketIterator);
	}
	packetLastIndexMutex.unlock();
}

void GameRoom::SendCardSignSync(char* packet)
{
	cardSignProcessPacket cardSignProcessPkt;
	int accumDataSize = 1;
	memcpy(&cardSignProcessPkt.isTrapCard, &packet[accumDataSize], sizeof(cardSignProcessPkt.isTrapCard));	// get isTrapCard
	accumDataSize += sizeof(cardSignProcessPkt.isTrapCard);
	
	if (cardSignProcessPkt.isTrapCard)	// 발동카드 사용 유무 물어본거 였으면
	{
		memcpy(&cardSignProcessPkt.isOk, &packet[accumDataSize], sizeof(cardSignProcessPkt.isOk));	// get isOk
		isCheckTrapCard = true; // 카드 사용유무 메시지 전송 했으니 설정

		if (cardSignProcessPkt.isOk)  // 카드 사용한다고 했으면
		{
			packetLastIndexMutex.lock();
			DeleteUseTrapCard();	// 사용 카드 삭제
			gameServer->MakePacket(sendPacket, &packetLastIndex, TRAP_CARD_SYNC);
			gameServer->AppendPacketData(sendPacket, &packetLastIndex, takeControlPlayer, sizeof(takeControlPlayer));	// 유저
			gameServer->AppendPacketData(sendPacket, &packetLastIndex, checkIsUsingTrapCardId, sizeof(checkIsUsingTrapCardId));	// 카드 id

			for (auto& socketIterator : userVector)
			{
				gameServer->PacektSendMethod(sendPacket, socketIterator);
				printf("%s %d\n", "send TrapCardSync - ", socketIterator);
			}
			packetLastIndexMutex.unlock();
		}
		else  // 카드 사용 안하면
		{
			switch (checkIsUsingTrapCardId)
			{
			case ESCAPE:
				DesertIslandMethod();	// 다시 감옥
				break;
			}
		}
	}
	else    // 일반 카드 사용 메시지일 때
	{
		switch (preCardId)	// 나중에 발동하는 카드라면, 턴 넘김
		{
		case ESCAPE:
			EndTurn();
			return;
		}

		packetLastIndexMutex.lock();
		gameServer->MakePacket(sendPacket, &packetLastIndex, CARD_SIGN_SYNC);
		gameServer->AppendPacketData(sendPacket, &packetLastIndex, takeControlPlayer, sizeof(takeControlPlayer));	// 유저
		gameServer->AppendPacketData(sendPacket, &packetLastIndex, preCardId, sizeof(preCardId));	// 카드 id
		gameServer->AppendPacketData(sendPacket, &packetLastIndex,
			userMoneyVector[takeControlPlayer], sizeof(userMoneyVector[takeControlPlayer]));		// 유저 돈
		gameServer->AppendPacketData(sendPacket, &packetLastIndex,
			userPositionVector[takeControlPlayer], sizeof(userPositionVector[takeControlPlayer]));	// 유저 위치

		for (auto& socketIterator : userVector)
		{
			gameServer->PacektSendMethod(sendPacket, socketIterator);
			printf("%s %d\n", "send CardSignSync - ", socketIterator);
		}
		packetLastIndexMutex.unlock();
	}
}

bool GameRoom::CheckTrapCard(int cardId)
{
	for (const auto& it : holdCard)
	{
		if (it.cardId == cardId && it.owner == takeControlPlayer)
		{
			return true;
		}
	}

	return false;
}

void GameRoom::SendRevenueSign()
{
	packetLastIndexMutex.lock();
	state = GameState::WAIT;
	gameServer->MakePacket(sendPacket, &packetLastIndex, REVENUE_SIGN);
	gameServer->AppendPacketData(sendPacket, &packetLastIndex, takeControlPlayer, sizeof(takeControlPlayer));	// 유저
	gameServer->AppendPacketData(sendPacket, &packetLastIndex, TAX, sizeof(TAX));	// 세금
	gameServer->PacektSendMethod(sendPacket, userVector[takeControlPlayer]);
	packetLastIndexMutex.unlock();
}

void GameRoom::SendRevenueSignSync()
{
	packetLastIndexMutex.lock();
	gameServer->MakePacket(sendPacket, &packetLastIndex, REVENUE_SIGN_SYNC);
	gameServer->AppendPacketData(sendPacket, &packetLastIndex, takeControlPlayer, sizeof(takeControlPlayer));	// 유저
	gameServer->AppendPacketData(sendPacket, &packetLastIndex, TAX, sizeof(TAX));	// 세금
	gameServer->AppendPacketData(sendPacket, &packetLastIndex, userMoneyVector[takeControlPlayer], sizeof(userMoneyVector[takeControlPlayer]));	// 유저 돈
	for (auto& socketIterator : userVector)
	{
		gameServer->PacektSendMethod(sendPacket, socketIterator);
		printf("%s %d\n", "send RevenueSignSync - ", socketIterator);
	}
	packetLastIndexMutex.unlock();
}

void GameRoom::SendBuyLandMarkSign()
{
	int landMarkPrice = board.landMark[userPositionVector[takeControlPlayer]];
	
	packetLastIndexMutex.lock();
	gameServer->MakePacket(sendPacket, &packetLastIndex, BUY_LANDMARK_SIGN);	// 인수 메시지
	gameServer->AppendPacketData(sendPacket, &packetLastIndex, takeControlPlayer, sizeof(takeControlPlayer));	// 턴
	gameServer->AppendPacketData(sendPacket, &packetLastIndex, landMarkPrice, sizeof(landMarkPrice));	// 랜드마크 건설비용
	gameServer->PacektSendMethod(sendPacket, userVector[takeControlPlayer]);
	packetLastIndexMutex.unlock();
}

void GameRoom::SendBuyLandMarkSignSync(int landMarkPrice)
{
	packetLastIndexMutex.lock();
	gameServer->MakePacket(sendPacket, &packetLastIndex, BUY_LANDMARK_SIGN_SYNC);	// 인수 메시지
	gameServer->AppendPacketData(sendPacket, &packetLastIndex, takeControlPlayer, sizeof(takeControlPlayer));	// 턴
	gameServer->AppendPacketData(sendPacket, &packetLastIndex, landMarkPrice, sizeof(landMarkPrice));	// 구입가격
	gameServer->AppendPacketData(sendPacket, &packetLastIndex, 
		userMoneyVector[takeControlPlayer], sizeof(userMoneyVector[takeControlPlayer]));	// 유저 돈

	for (auto& socketIterator : userVector)
	{
		gameServer->PacektSendMethod(sendPacket, socketIterator);
		printf("%s %d\n", "send BuyLandMarkSync - ", socketIterator);
	}
	packetLastIndexMutex.unlock();
}

void GameRoom::SendSellLandSign(int goalPrice, int state)
{
	beforeSellSign = state;
	this->goalPrice = goalPrice;

	packetLastIndexMutex.lock();
	gameServer->MakePacket(sendPacket, &packetLastIndex, SELL_LAND_SIGN);	// 매각 메시지
	gameServer->AppendPacketData(sendPacket, &packetLastIndex, takeControlPlayer, sizeof(takeControlPlayer));	// 턴
	gameServer->AppendPacketData(sendPacket, &packetLastIndex, goalPrice, sizeof(goalPrice));	// 필요 금액
	gameServer->AppendPacketData(sendPacket, &packetLastIndex,
		userMoneyVector[takeControlPlayer], sizeof(userMoneyVector[takeControlPlayer]));	// 유저 돈

	selectLandIndex.clear();

	gameServer->PacektSendMethod(sendPacket, userVector[takeControlPlayer]);
	packetLastIndexMutex.unlock();
}

void GameRoom::SendCardSign(Card card)
{
	packetLastIndexMutex.lock();
	gameServer->MakePacket(sendPacket, &packetLastIndex, CARD_SIGN);	// 매각 메시지
	gameServer->AppendPacketData(sendPacket, &packetLastIndex, takeControlPlayer, sizeof(takeControlPlayer));	// 턴
	gameServer->AppendPacketData(sendPacket, &packetLastIndex, card.cardId, sizeof(card.cardId));	// 카드 id
	gameServer->AppendPacketData(sendPacket, &packetLastIndex, false, sizeof(false));	// 사용 여부 물어보는 카드 인지
	gameServer->PacektSendMethod(sendPacket, userVector[takeControlPlayer]);
	packetLastIndexMutex.unlock();
}

void GameRoom::SendIsUseCardSign(int cardId)
{
	switch (cardId)	// 지정한 카드가 아니면 호출 X
	{
	case ESCAPE:
		checkIsUsingTrapCardId = ESCAPE;
		break;
	default:
		return;
	}
	
	packetLastIndexMutex.lock();
	gameServer->MakePacket(sendPacket, &packetLastIndex, CARD_SIGN);	// 매각 메시지
	gameServer->AppendPacketData(sendPacket, &packetLastIndex, takeControlPlayer, sizeof(takeControlPlayer));	// 턴
	gameServer->AppendPacketData(sendPacket, &packetLastIndex, cardId, sizeof(cardId));	// 카드 id
	gameServer->AppendPacketData(sendPacket, &packetLastIndex, true, sizeof(true));	// 사용 여부 물어보는 카드 인지
	gameServer->PacektSendMethod(sendPacket, userVector[takeControlPlayer]);
	packetLastIndexMutex.unlock();
}

void GameRoom::DeleteUseTrapCard()
{
	for (auto it = holdCard.begin(); it != holdCard.end(); it++)
	{
		if ((*it).cardId == checkIsUsingTrapCardId && (*it).owner == takeControlPlayer)
		{
			holdCard.erase(it);
			return;
		}
	}
}

void GameRoom::SendSellLandSignSync()
{
	int sellLandSize = (int)selectLandIndex.size();

	packetLastIndexMutex.lock();
	gameServer->MakePacket(sendPacket, &packetLastIndex, SELL_LAND_SIGN_SYNC);	// 매각 동기화 메시지
	gameServer->AppendPacketData(sendPacket, &packetLastIndex, takeControlPlayer, sizeof(takeControlPlayer));	// 턴
	gameServer->AppendPacketData(sendPacket, &packetLastIndex,
		userMoneyVector[takeControlPlayer], sizeof(userMoneyVector[takeControlPlayer]));	// 유저 돈
	gameServer->AppendPacketData(sendPacket, &packetLastIndex, goalPrice, sizeof(goalPrice));	// 목표금액
	gameServer->AppendPacketData(sendPacket, &packetLastIndex, sellLandSize, sizeof(sellLandSize));	// 판매하는 땅 개수
	for (int i = 0; i < sellLandSize; i++)
	{
		gameServer->AppendPacketData(sendPacket, &packetLastIndex, selectLandIndex[i], sizeof(selectLandIndex[i]));	// 판매하는 땅 인덱스
	}

	for (auto& socketIterator : userVector)
	{
		gameServer->PacektSendMethod(sendPacket, socketIterator);
		printf("%s %d\n", "send SellLandSignSync - ", socketIterator);
	}
	packetLastIndexMutex.unlock();
}

void GameRoom::CheckPassNSellMessage()
{
	int tollPrice = 0;
	int landOwner = landBoardData.land[userPositionVector[takeControlPlayer]];

	if (landBoardData.landMark[userPositionVector[takeControlPlayer]] == landOwner)	// 랜드마크이면
	{
		tollPrice += board.tollLandMark[userPositionVector[takeControlPlayer]];
	}
	else   // 랜드마크 아니면
	{
		tollPrice += board.tollLand[userPositionVector[takeControlPlayer]];
		if (landBoardData.villa[userPositionVector[takeControlPlayer]] == landOwner)
		{
			tollPrice += board.tollVilla[userPositionVector[takeControlPlayer]];
		}
		if (landBoardData.building[userPositionVector[takeControlPlayer]] == landOwner)
		{
			tollPrice += board.tollBuilding[userPositionVector[takeControlPlayer]];
		}
		if (landBoardData.hotel[userPositionVector[takeControlPlayer]] == landOwner)
		{
			tollPrice += board.tollHotel[userPositionVector[takeControlPlayer]];
		}
	}

	if (landBoardData.olympic[userPositionVector[takeControlPlayer]] != 0)
		tollPrice *= (int)pow(2, landBoardData.olympic[userPositionVector[takeControlPlayer]]);	// 올림픽 가격 적용

	if (tollPrice <= userMoneyVector[takeControlPlayer])
	{
		userMoneyVector[takeControlPlayer] -= tollPrice;
		userMoneyVector[landOwner] += tollPrice;

		if (board.code[userPositionVector[takeControlPlayer]] == TOUR_TILE)  // 휴양지라면, 인수 못하니 다음턴
		{
			EndTurn();
		}
		else
		{
			if (landBoardData.landMark[userPositionVector[takeControlPlayer]] != -1) // 랜드마크 지어져 있으면
				EndTurn();	// 인수 못하니까 다음턴으로 넘기기
			else  // 인수 처리
			{
				if ((GetBuildPrice(landOwner) * 2) > (userMoneyVector[takeControlPlayer] + TotalDisposalPrice())) // 인수 비용이 땅 매각가격 + 소지금 보다 많다면 인수 불가
					EndTurn();
				else   // 땅 팔아서 인수 비용 충당이 가능하다면 땅 매각
					SendTakeOverSign(landOwner);
			}	
		}
	}
	else
	{
		if (TotalDisposalPrice() + userMoneyVector[takeControlPlayer] >= tollPrice)  // 판매 시 통행료를 낼 수 있을 때
		{
			SendSellLandSign(tollPrice, PAY_TOLL); // 판매 메시지 보내기
		}
		else    // 판매금 + 소지금이 통행료보다 적을 경우
		{
			Bankruptcy(takeControlPlayer, true);	// 파산 처리
			state = GameState::NEXT_TURN;
		}
	}
}

void GameRoom::CheckCanBuild()
{
	if (landBoardData.villa[userPositionVector[takeControlPlayer]] == takeControlPlayer &&
		landBoardData.building[userPositionVector[takeControlPlayer]] == takeControlPlayer &&
		landBoardData.hotel[userPositionVector[takeControlPlayer]] == takeControlPlayer)
	{
		SendBuyLandMarkSign();
	}
	else // 건물 3개가 지어있지 않을 경우
	{
		CheckLandKindNSendMessage();	// 건물 짓기 신호 보내기
	}
}

void GameRoom::SendFinishTurnSign()
{
	packetLastIndexMutex.lock();
	for (int i = 0; i < (int)userVector.size(); i++)
	{
		if (bankruptcyVector[i])
			continue;

		gameServer->MakePacket(sendPacket, &packetLastIndex, FINISH_THIS_TURN_PROCESS);
		gameServer->PacektSendMethod(sendPacket, userVector[i]);
	}
	packetLastIndexMutex.unlock();
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

	int bankruptcyCount = 0;
	for (int i = 0; i < (int)bankruptcyVector.size(); i++)
	{
		if (bankruptcyVector[i])
			bankruptcyCount++;
	}

	if (boolCount == (int)isFinishTurnProcessVector.size() - bankruptcyCount)
	{
		NextTurn();
	}
}

void GameRoom::SendSelectLandIndex(int index, bool isSpaceBar)
{
	packetLastIndexMutex.lock();
	gameServer->MakePacket(sendPacket, &packetLastIndex, SEND_SELECT_MODE_INPUT_KEY);
	gameServer->AppendPacketData(sendPacket, &packetLastIndex, index, sizeof(index));	// 선택 지역 인덱스
	gameServer->AppendPacketData(sendPacket, &packetLastIndex, isSpaceBar, sizeof(isSpaceBar));	// 스페이스바 동작인지

	bool isErase = false;
	int sellLandPrice = 0;

	if (isSpaceBar)
	{
		for (auto it = selectLandIndex.begin(); it != selectLandIndex.end(); it++)
		{
			if ((*it) == index)   // 한번 선택된 장소라면, 해당 장소 제거
			{
				selectLandIndex.erase(it);
				isErase = true;
				break;
			}
		}

		if (!isErase)
			selectLandIndex.emplace_back(index);	// 선택 안된 장소면 장소 추가
	}

	gameServer->AppendPacketData(sendPacket, &packetLastIndex, isErase, sizeof(isErase));	// 삭제 유무

	sellLandPrice = DisposalPrice(index);

	gameServer->AppendPacketData(sendPacket, &packetLastIndex, sellLandPrice, sizeof(sellLandPrice));	// 판매 금액
	gameServer->PacektSendMethod(sendPacket, userVector[takeControlPlayer]);
	printf("%s %d\n", "send Data", SEND_SELECT_MODE_INPUT_KEY);
	packetLastIndexMutex.unlock();
}

void GameRoom::SendGameOverSign()
{
	packetLastIndexMutex.lock();
	gameServer->MakePacket(sendPacket, &packetLastIndex, GAMEOVER_SIGN);
	for (int i = 0; i < (int)bankruptcyVector.size(); i++)
	{
		if (!bankruptcyVector[i])
		{
			gameServer->AppendPacketData(sendPacket, &packetLastIndex, i, sizeof(i));	// 우승한 유저 인덱스
			HttpTransfer::GetInstance()->SetRanking(userVector[i], userMoneyVector[i]);
			break;
		}
	}

	for (auto& socketIterator : userVector)
	{
		gameServer->PacektSendMethod(sendPacket, socketIterator);
		printf("%s %d\n", "send GameOverSign - ", socketIterator);
	}

	state = GameState::GAME_OVER;
	packetLastIndexMutex.unlock();
}

int GameRoom::GetBuildPrice(int turn)
{
	int buildPrice = 0;

	buildPrice += board.land[userPositionVector[takeControlPlayer]];
	if (landBoardData.villa[userPositionVector[takeControlPlayer]] == turn)
	{
		buildPrice += board.villa[userPositionVector[takeControlPlayer]];
	}
	if (landBoardData.building[userPositionVector[takeControlPlayer]] == turn)
	{
		buildPrice += board.building[userPositionVector[takeControlPlayer]];
	}
	if (landBoardData.hotel[userPositionVector[takeControlPlayer]] == turn)
	{
		buildPrice += board.hotel[userPositionVector[takeControlPlayer]];
	}

	return buildPrice;
}

int GameRoom::TakeOverLand(int turn, int takeOverPrice)
{
	int owner = landBoardData.land[userPositionVector[takeControlPlayer]];

	userMoneyVector[turn] -= takeOverPrice;
	userMoneyVector[owner] += takeOverPrice;

	landBoardData.land[userPositionVector[takeControlPlayer]] = turn;	// 땅 이전
	if(landBoardData.villa[userPositionVector[takeControlPlayer]] == owner)	// 빌라 있다면
		landBoardData.villa[userPositionVector[takeControlPlayer]] = turn;	// 빌라 명의 이전
	if (landBoardData.building[userPositionVector[takeControlPlayer]] == owner)// 빌딩 있다면
		landBoardData.building[userPositionVector[takeControlPlayer]] = turn;	// 빌딩 명의 이전
	if (landBoardData.hotel[userPositionVector[takeControlPlayer]] == owner)// 호텔 있다면
		landBoardData.hotel[userPositionVector[takeControlPlayer]] = turn;	// 호텔 명의 이전

	return owner;
}

void GameRoom::BuyLandMark(int price)
{
	userMoneyVector[takeControlPlayer] -= price;		// 돈 제거
	landBoardData.landMark[userPositionVector[takeControlPlayer]] = takeControlPlayer;	// 랜드마크 건설 처리
}

int GameRoom::TotalDisposalPrice()
{
	int disposalPrice = 0;

	for (int i = 0; i < (int)board.land.size(); i++)
	{
		if (landBoardData.land[i] == takeControlPlayer)  // 땅이 지어져 있는 곳이면
		{
			disposalPrice += board.land[i];
			if (board.code[i] == TOUR_TILE)
				continue;
		}
		if (landBoardData.villa[i] == takeControlPlayer)  // 빌라 있으면
		{
			disposalPrice += board.villa[i];
		}
		if (landBoardData.building[i] == takeControlPlayer)  // 빌딩 있으면
		{
			disposalPrice += board.building[i];
		}
		if (landBoardData.hotel[i] == takeControlPlayer)  // 호텔 있으면
		{
			disposalPrice += board.hotel[i];
		}
		if (landBoardData.landMark[i] == takeControlPlayer)  // 랜드마크 있으면
		{
			disposalPrice += board.landMark[i];
		}
	}

	return disposalPrice / 2;	// 매각 비용은 건설비용의 반토막 ( x / 2 ) 값
}

int GameRoom::DisposalPrice(int index)
{
	int disposalPrice = 0;

	if (landBoardData.land[index] == takeControlPlayer)  // 땅이 지어져 있는 곳이면
	{
		disposalPrice += board.land[index];
		if (board.code[index] == TOUR_TILE)
			return disposalPrice / 2;	// 매각 비용은 건설비용의 반토막 ( x / 2 ) 값
	}
	if (landBoardData.villa[index] == takeControlPlayer)  // 빌라 있으면
	{
		disposalPrice += board.villa[index];
	}
	if (landBoardData.building[index] == takeControlPlayer)  // 빌딩 있으면
	{
		disposalPrice += board.building[index];
	}
	if (landBoardData.hotel[index] == takeControlPlayer)  // 호텔 있으면
	{
		disposalPrice += board.hotel[index];
	}
	if (landBoardData.landMark[index] == takeControlPlayer)  // 랜드마크 있으면
	{
		disposalPrice += board.landMark[index];
	}

	return disposalPrice / 2;	// 매각 비용은 건설비용의 반토막 ( x / 2 ) 값
}

int GameRoom::SellLand()
{
	int accumPrice = 0;

	for (auto it : selectLandIndex)
	{
		accumPrice += DisposalPrice(it);
	}

	for (auto it : selectLandIndex)  // 처분 처리
	{
		landBoardData.land[it] = -1;
		landBoardData.villa[it] = -1;
		landBoardData.hotel[it] = -1;
		landBoardData.building[it] = -1;
		landBoardData.landMark[it] = -1;
		landBoardData.olympic[it] = 0;
	}

	userMoneyVector[takeControlPlayer] += accumPrice;	// 돈 증가

	return accumPrice;
}

int GameRoom::FindNextLand(int selectValue, bool isLeft)
{
	list<int> myLand;

	for (int i = 0; i < (int)landBoardData.land.size(); i++)
	{
		if (landBoardData.land[i] == takeControlPlayer) // 땅을 소유하고 있다면 
		{
			myLand.emplace_back(i);
		}
	}

	if (isLeft)	// 왼쪽의 지역 찾기 , 순방향 참조
	{
		if (myLand.size() == 1 || myLand.size() == 0)	// 소지한 땅이 0,1개면
			return selectValue;

		if (myLand.back() == selectValue)	// 선택 지역이 맨끝이면
			return myLand.front();

		for (auto it = myLand.begin(); it != myLand.end(); it++)
		{
			if ((*it) == selectValue)
			{
				it++;
				return (*it);
			}
		}

		return selectValue;
	}
	else   // 오른쪽의 지역 찾기 , 역방향 참조
	{
		if (myLand.size() == 1 || myLand.size() == 0)	// 소지한 땅이 0,1개면
			return selectValue;

		if (myLand.front() == selectValue)	// 선택 지역이 맨처음이면
			return myLand.back();

		reverse(myLand.begin(), myLand.end());

		for (auto it = myLand.begin(); it != myLand.end(); it++)
		{
			if ((*it) == selectValue)
			{
				it++;
				return (*it);
			}
		}

		return selectValue;
	}
}

void GameRoom::Bankruptcy(int index, bool isToll)
{
	bankruptcyVector[index] = true;	// 파산 여부 변경

	packetLastIndexMutex.lock();
	gameServer->MakePacket(sendPacket, &packetLastIndex, BANKRUPTCY_SIGN);
	gameServer->AppendPacketData(sendPacket, &packetLastIndex, index, sizeof(index));	// 파산 유저 번호

	int landOwner = -1;
	if (isToll)
	{
		landOwner = landBoardData.land[userPositionVector[takeControlPlayer]];
		userMoneyVector[landOwner] += TotalDisposalPrice() + userMoneyVector[takeControlPlayer]; // 땅 주인에게 파산한 사람 돈 주기
	}

	gameServer->AppendPacketData(sendPacket, &packetLastIndex, landOwner, sizeof(landOwner));	// 땅 주인 번호
	if (landOwner != -1)
	{
		gameServer->AppendPacketData(sendPacket, &packetLastIndex, userMoneyVector[landOwner], sizeof(userMoneyVector[landOwner]));	// 파산 유저 전재산
	}
	
	for (auto& socketIterator : userVector)
	{
		gameServer->PacektSendMethod(sendPacket, socketIterator);
		printf("%s %d\n", "send Bankruptcy Sign - ", socketIterator);
	}

	userMoneyVector[index] = 0;		// 돈 없애기

	for (int i = 0; i < (int)board.code.size(); i++)
	{
		if (landBoardData.land[i] == index)	// 땅 없애기
		{
			if (landBoardData.villa[i] == index)
				landBoardData.villa[i] = -1;
			if (landBoardData.building[i] == index)
				landBoardData.building[i] = -1;
			if (landBoardData.hotel[i] == index)
				landBoardData.hotel[i] = -1;
			if (landBoardData.olympic[i] == index)
				landBoardData.olympic[i] = -1;
		}
	}

	if (index == takeControlPlayer)	// 탈주한 인원 차례였던 경우,
	{
		EndTurn();	// 해당 턴 종료
	}
	packetLastIndexMutex.unlock();
}

void GameRoom::EndTurn()
{
	if (isDouble)
	{
		isDouble = false;
		state = GameState::ROLL_DICE_SIGN;
	}
	else
		state = GameState::NEXT_TURN;
}
