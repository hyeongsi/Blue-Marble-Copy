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
			readFile >> card.cardId;		// ī�� ID
			readFile >> card.money;			// ����,���� �ݾ�
			readFile >> card.movePosition;	// �̵��� �Ÿ�
			readFile >> card.moveIndex;		// �̵��� ��ǥ
			readFile >> card.isPaySalary;	// �������� ��� �� ���� ���� ����

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
	mt19937 gen(rd());		// random_device �� ���� ���� ���� ������ �ʱ�ȭ �Ѵ�.
	uniform_int_distribution<int> dis(0, (int)cardData.size()-1);		// 0 ���� ī�尳�� ���� �յ��ϰ� ��Ÿ���� �������� �����ϱ� ���� �յ� ���� ����.

	return cardData[dis(gen)];
}
