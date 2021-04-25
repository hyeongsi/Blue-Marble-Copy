#pragma warning(disable: 26812)	// ������ ã�� �� �����ϴ�. - CURLcode ���� ��� �� - �ܺζ��̺귯���� �ٲٱ� ��������
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
	curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:3000/ranking");	// ip, port ����

	rc = curl_easy_perform(curl);	// ������ �J����� ����
	if (CURLE_OK != rc)
	{
		readBuffer = "";
	}

	if (curl)
		curl_easy_cleanup(curl);

	curl_global_cleanup();

	return readBuffer;
}
