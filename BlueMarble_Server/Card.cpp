#include "Card.h"
#include "GameRoom.h"

void Card::UseCard(GameRoom* room)
{
	room->state = GameState::WAIT;
	owner = room->GetTakeControlPlayer();	// ī�� ������ ���
	room->preCardId = cardId;

	switch (cardId)
	{
	case ESCAPE:
		room->GetPHoldCard()->emplace_back(*this);
		break;
	}

	(*room->GetPUserMoneyVector())[room->GetTakeControlPlayer()] += money;	// �� �߰�,����

	if (room->MoveUserPosition(movePosition))	// ������ ��� ��
	{
		if (!isPaySalary) // ���� ���� �ϸ� �� �� ���
			(*room->GetPUserMoneyVector())[room->GetTakeControlPlayer()] -= SALARY; // ���� �м�
	}

	if (moveIndex == -1)   // �̵��� ��ǥ���� ������
	{
		room->SendCardSign(*this);
		return;
	}
	
	// ��ǥ ��ǥ �� �� �� ���� �̵�
	while (room->GetUserPositionVector()[room->GetTakeControlPlayer()] != moveIndex)
	{
		if (room->MoveUserPosition(movePosition))	// ������ ��� ��
		{
			if (!isPaySalary) // ���� ���� �ϸ� �� �� ���
				(*room->GetPUserMoneyVector())[room->GetTakeControlPlayer()] -= SALARY; // ���� �м�
		}
	}

	room->SendCardSign(*this);
}

void Card::ActivatedMyTrapCard(GameRoom* room)
{
	// ī�� ��� ó�� ��
	room->isDouble = true;	// ����� ���� Ż��,
	room->EndTurn();
}
