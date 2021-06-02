#pragma once
#include <random>

using namespace std;

enum CardId
{
	PAY_HOSPITAL_BILLS = 0,	// ������ ����	-50
	WINNING_A_LOTTERY = 1,	// ���� ��÷		+200
	ESCAPE = 2,				// ���ε� Ż��	
	DESERT_ISLAND = 3,		// ���ε� �̵�	
	TOURISM = 4,			// ������ �̵�	
	SPEEDING_FINE = 5,		// ���� ����		-50
	STUDY_ABOARD = 6,		// �ؿ� ����		-100
	PENSION_BENEFITS = 7,	// ���� ����		+50
	MOVE = 8,				// �̻�			3ĭ �ڷ�
};

class GameRoom;
class Card
{
public:
	int cardId = -1;		// ī�� ID
	int money = 0;			// ����,���� �ݾ�
	int movePosition = 0;	// �̵��� �Ÿ�
	int moveIndex = -1;		// �̵��� ��ǥ
	bool isPaySalary = false;	// �������� ��� �� ���� ���� ����

	int owner = -1;

	void UseCard(GameRoom* room);
};

