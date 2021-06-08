#pragma once
#include "Card.h"
#include <vector>

class CardManager
{
private:
	static CardManager* instance;
	std::vector<Card> cardData;

	CardManager();
	~CardManager();
public:
	static CardManager* GetInstance();
	static void ReleaseInstance();

	void LoadCardData();
	std::vector<Card> GetCardDataVector();

	Card DrawCard();	// 카드 뽑기
};

