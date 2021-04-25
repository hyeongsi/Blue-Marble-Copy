#pragma warning(disable: 26812)	// 열거형 찾을 수 없습니다. - CURLcode 에서 계속 뜸 - 외부라이브러리라 바꾸기 꺼림직함
#include "HttpTransfer.h"

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
	}

	return nullptr;
}

void HttpTransfer::ReleaseInstance()
{
	delete instance;
	instance = nullptr;
}

string HttpTransfer::GetRanking()
{
	curl_global_init(CURL_GLOBAL_ALL);
	curl = curl_easy_init();

	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
	curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:3000/ranking");	// ip, port 설정

	rc = curl_easy_perform(curl);	// 데이터 긇어오는 역할
	if (CURLE_OK != rc)
	{
		readBuffer = "";
	}

	if (curl)
		curl_easy_cleanup(curl);

	curl_global_cleanup();

	return readBuffer;
}
