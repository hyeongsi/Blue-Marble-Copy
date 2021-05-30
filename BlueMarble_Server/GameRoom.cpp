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

	userMoneyVector.emplace_back(2000);
	userMoneyVector.emplace_back(2000);

	isDesertIsland.emplace_back(false);
	isDesertIsland.emplace_back(false);

	desertIslandCount.emplace_back(0);
	desertIslandCount.emplace_back(0);

	isDouble = false;

	board = *MapManager::GetInstance()->GetBoardData(ORIGINAL);

	for (int i = 0; i < (int)(board.mapSize * DIRECTION); i++)
	{
		landBoardData.land[i] = -1;
		landBoardData.villa[i] = -1;
		landBoardData.building[i] = -1;
		landBoardData.hotel[i] = -1;
		landBoardData.landMark[i] = -1;
	}

	gameServer = GameServer::GetInstance();
}

vector<SOCKET> GameRoom::GetUserVector()
{
	return userVector;
}

vector<int>* GameRoom::GetPUserMoneyVector()
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

void GameRoom::DesertIslandMethod()
{
	if (!isDesertIsland[takeControlPlayer])	// 처음 무인도 진입 시
	{
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
	}
}

void GameRoom::MoveUserPosition(int diceValue)
{
	userPositionVector[takeControlPlayer] += diceValue;

	if (userPositionVector[takeControlPlayer] >= (int)board.mapSize * DIRECTION)
	{
		userPositionVector[takeControlPlayer] -= board.mapSize * DIRECTION;
		userMoneyVector[takeControlPlayer] += SALARY;	// START 지점 통과, 30만원 지급
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

	gameServer->MakePacket(sendPacket, &packetLastIndex, PAY_TOLL_SIGN);
	gameServer->AppendPacketData(sendPacket, &packetLastIndex, takeControlPlayer, sizeof(takeControlPlayer));	// 턴
	gameServer->AppendPacketData(sendPacket, &packetLastIndex, toll, sizeof(toll));	// 통행료 가격
	gameServer->PacektSendMethod(sendPacket, userVector[takeControlPlayer]);
	printf("%s %d\n", "send Pay_Toll_Sign - ", userVector[takeControlPlayer]);
}

void GameRoom::SendBuyLandSign()
{
	gameServer->MakePacket(sendPacket, &packetLastIndex, BUY_LAND_SIGN);
	gameServer->AppendPacketData(sendPacket, &packetLastIndex, takeControlPlayer, sizeof(int));	// 유저
	gameServer->AppendPacketData(sendPacket, &packetLastIndex,
		board.land[userPositionVector[takeControlPlayer]], sizeof(int));	// 땅 가격
	gameServer->PacektSendMethod(sendPacket, userVector[takeControlPlayer]);
	printf("%s %d\n", "send Buy_Land_Sign - ", userVector[takeControlPlayer]);
}

void GameRoom::SendLandSyncSign(int turn, bool isBuy)
{
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
}

void GameRoom::SendBuildingSyncSign(int turn, bool isBuy, bool isBuyVilla, bool isBuyBuilding, bool isBuyHotel, int accumPrice)
{
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
}

void GameRoom::SendPayTollSignSync(int turn, int tollPrice, bool isPass, int landOwner)
{
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
}

void GameRoom::CheckLandKindNSendMessage()
{
	bool isBuyVilla;
	bool isBuyBuilding;
	bool isBuyHotel;

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
}

void GameRoom::SendTakeOverSign(int landOwner)
{
	int takeOverPrice = GetBuildPrice(landOwner);

	takeOverPrice *= 2;	// 인수 비용은 구입 비용의 2배

	gameServer->MakePacket(sendPacket, &packetLastIndex, TAKE_OVER_SIGN);	// 인수 메시지
	gameServer->AppendPacketData(sendPacket, &packetLastIndex, takeControlPlayer, sizeof(takeControlPlayer));	// 턴
	gameServer->AppendPacketData(sendPacket, &packetLastIndex, takeOverPrice, sizeof(takeOverPrice));	// 인수비용
	gameServer->PacektSendMethod(sendPacket, userVector[takeControlPlayer]);
}

void GameRoom::SendTakeOverSignSync(int takeOverPrice, int owner)
{
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
}

void GameRoom::SendBuyLandMarkSign()
{
	int landMarkPrice = board.landMark[userPositionVector[takeControlPlayer]];
	
	gameServer->MakePacket(sendPacket, &packetLastIndex, BUY_LANDMARK_SIGN);	// 인수 메시지
	gameServer->AppendPacketData(sendPacket, &packetLastIndex, takeControlPlayer, sizeof(takeControlPlayer));	// 턴
	gameServer->AppendPacketData(sendPacket, &packetLastIndex, landMarkPrice, sizeof(landMarkPrice));	// 랜드마크 건설비용
	gameServer->PacektSendMethod(sendPacket, userVector[takeControlPlayer]);
}

void GameRoom::SendBuyLandMarkSignSync(int landMarkPrice)
{
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
}

void GameRoom::SendSellLandSign(int goalPrice, int state)
{
	beforeSellSign = state;

	gameServer->MakePacket(sendPacket, &packetLastIndex, SELL_LAND_SIGN);	// 매각 메시지
	gameServer->AppendPacketData(sendPacket, &packetLastIndex, takeControlPlayer, sizeof(takeControlPlayer));	// 턴
	gameServer->AppendPacketData(sendPacket, &packetLastIndex, goalPrice, sizeof(goalPrice));	// 필요 금액
	gameServer->AppendPacketData(sendPacket, &packetLastIndex,
		userMoneyVector[takeControlPlayer], sizeof(userMoneyVector[takeControlPlayer]));	// 유저 돈

	vector<int> landPosition;

	for (int i = 0; i < (int)board.land.size(); i++)
	{
		if (landBoardData.land[i] == takeControlPlayer)  // 땅이 지어져 있는 곳이면
		{
			landPosition.emplace_back(i);
		}
	}

	gameServer->PacektSendMethod(sendPacket, userVector[takeControlPlayer]);
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
			if (landBoardData.landMark[userPositionVector[takeControlPlayer]] == takeControlPlayer) // 랜드마크 지어져 있으면
				SendBuyLandMarkSign();
			else  // 인수 처리
				SendTakeOverSign(landOwner);
		}
	}
	else
	{
		if (DisposalPrice() + userMoneyVector[takeControlPlayer] >= tollPrice)  // 판매 시 통행료를 낼 수 있을 때
		{
			SendSellLandSign(tollPrice, TAKE_OVER_LAND); // 판매 메시지 보내기
		}
		else    // 판매금 + 소지금이 통행료보다 적을 경우
		{
			// 게임 오버 처리
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

void GameRoom::SendSelectLandIndex(int index)
{
	gameServer->MakePacket(sendPacket, &packetLastIndex, SEND_SELECT_MODE_INPUT_KEY);
	gameServer->AppendPacketData(sendPacket, &packetLastIndex, index, sizeof(index));	// 선택 지역 인덱스
	gameServer->PacektSendMethod(sendPacket, userVector[takeControlPlayer]);
	printf("%s\ %dn", "send Data", SEND_SELECT_MODE_INPUT_KEY);
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

int GameRoom::DisposalPrice()
{
	int disposalPrice = 0;

	for (int i = 0; i < (int)board.land.size(); i++)
	{
		if (landBoardData.land[i] == takeControlPlayer)  // 땅이 지어져 있는 곳이면
		{
			disposalPrice += board.land[i];
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
