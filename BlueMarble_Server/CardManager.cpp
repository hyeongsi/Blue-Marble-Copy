#include "CardManager.h"
#include <fstream>
#include <string>
#include <random>

CardManager* CardManager::instance = nullptr;

CardManager::CardManager() {}
CardManager::~CardManager() {}

CardManager* CardManager::GetInstance()
{
	if (nullptr == instance)
	{
		instance = new CardManager();
	}

	return instance;
}

void CardManager::ReleaseInstance()
{
	delete instance;
	instance = nullptr;
}

void CardManager::LoadCardData()
{
	const char* cardDataPath = "cardData/original.txt";

	ifstream readFile;
	Card card;

	readFile.open(cardDataPath);
	if (readFile.is_open())
	{
		while (!readFile.eof())
		{
			readFile >> card.cardId;		// 카드 ID
			readFile >> card.money;			// 지급,지불 금액
			readFile >> card.movePosition;	// 이동할 거리
			readFile >> card.moveIndex;		// 이동할 좌표
			readFile >> card.isPaySalary;	// 시작지점 통과 시 월급 지급 여부

			cardData.emplace_back(card);
		}
	}
	
	readFile.close();
}

std::vector<Card> CardManager::GetCardDataVector()
{
	return cardData;
}

Card CardManager::DrawCard()
{
	random_device rd;
	mt19937 gen(rd());		// random_device 를 통해 난수 생성 엔진을 초기화 한다.
	uniform_int_distribution<int> dis(0, (int)cardData.size()-1);		// 0 부터 카드개수 까지 균등하게 나타나는 난수열을 생성하기 위해 균등 분포 정의.

	return cardData[dis(gen)];
}
