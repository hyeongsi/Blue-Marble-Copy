#pragma warning(disable: 26812)	// 열거형 찾을 수 없습니다. - CURLcode 에서 계속 뜸 - 외부라이브러리라 바꾸기 꺼림직함
#include "HttpTransfer.h"
#include <process.h>

HttpTransfer* HttpTransfer::instance = nullptr;

HttpTransfer::HttpTransfer() 
{
	rc = CURLcode::CURL_LAST;
}
HttpTransfer::~HttpTransfer(){}

size_t HttpTransfer::WriteCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
	((string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

HttpTransfer* HttpTransfer::GetInstance()
{
	if (nullptr == instance)
	{
		instance = new HttpTransfer();
		curl_global_init(CURL_GLOBAL_ALL);
		instance->headerlist = curl_slist_append(instance->headerlist, "Content-Type: application/json");
		instance->curl = curl_easy_init();
	}

	return instance;
}

void HttpTransfer::ReleaseInstance()
{
	if (instance->curl != nullptr)
		curl_easy_cleanup(instance->curl);

	curl_global_cleanup();

	delete instance;
	instance = nullptr;
}

void HttpTransfer::SetRanking(int id, int score)
{
	string sendData;
	sendData = "name=" + to_string(id) + "&" + "score=" + to_string(score);

	if (curl != nullptr)
	{
		curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:3000/addRank");

		curl_easy_setopt(curl, CURLOPT_POST, 1L);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, sendData.c_str());

		curl_easy_perform(curl);
	}
}
