#include "Card.h"
#include "GameRoom.h"

void Card::UseCard(GameRoom* room)
{
	room->state = GameState::WAIT;
	owner = room->GetTakeControlPlayer();	// 카드 소유자 등록
	room->preCardId = cardId;

	printf("%s %d\n", "UseCard - ", cardId);

	switch (cardId)
	{
	case ESCAPE:
		room->GetPHoldCard()->emplace_back(*this);
		break;
	}

	(*room->GetPUserMoneyVector())[room->GetTakeControlPlayer()] += money;	// 돈 추가,감소

	if (room->MoveUserPosition(movePosition))	// 시작점 통과 시
	{
		if (!isPaySalary) // 월급 지불 하면 안 될 경우
			(*room->GetPUserMoneyVector())[room->GetTakeControlPlayer()] -= SALARY; // 월급 압수
	}

	if (moveIndex == -1)   // 이동할 좌표값이 없으면
	{
		room->SendCardSign(*this);
		return;
	}
	
	// 목표 좌표 가 될 때 까지 이동
	while (room->GetUserPositionVector()[room->GetTakeControlPlayer()] != moveIndex)
	{
		if (room->MoveUserPosition(1))	// 1칸씩 이동하면서, 지정 좌표 도달 하면 탈출, 시작점 통과 시
		{
			if (!isPaySalary) // 월급 지불 하면 안 될 경우
				(*room->GetPUserMoneyVector())[room->GetTakeControlPlayer()] -= SALARY; // 월급 압수
		}
	}

	room->SendCardSign(*this);
}

void Card::ActivatedMyTrapCard(GameRoom* room)
{
	// 카드 사용 처리 후
	room->isDouble = true;	// 더블로 만들어서 탈출,
	room->EndTurn();
}
