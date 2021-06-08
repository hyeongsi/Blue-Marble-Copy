#pragma once
#include <random>

using namespace std;

enum CardId
{
	PAY_HOSPITAL_BILLS = 0,	// 병원비 지출	-50
	WINNING_A_LOTTERY = 1,	// 복권 당첨		+200
	ESCAPE = 2,				// 무인도 탈출	
	DESERT_ISLAND = 3,		// 무인도 이동	
	TOURISM = 4,			// 독도로 이동	
	SPEEDING_FINE = 5,		// 과속 벌금		-50
	STUDY_ABOARD = 6,		// 해외 유학		-100
	PENSION_BENEFITS = 7,	// 연금 혜택		+50
	MOVE = 8,				// 이사			3칸 뒤로
};

class GameRoom;
class Card
{
public:
	int cardId = -1;		// 카드 ID
	int money = 0;			// 지급,지불 금액
	int movePosition = 0;	// 이동할 거리
	int moveIndex = -1;		// 이동할 좌표
	bool isPaySalary = false;	// 시작지점 통과 시 월급 지급 여부

	int owner = -1;

	void UseCard(GameRoom* room);
};

