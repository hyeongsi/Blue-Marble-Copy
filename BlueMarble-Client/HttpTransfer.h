#pragma once
#include <curl.h>
#include <string>

using namespace std;

class HttpTransfer
{
private:
	static HttpTransfer* instance;
	CURL* curl = nullptr;
	CURLcode rc;
	string readBuffer;

	HttpTransfer();
	~HttpTransfer();

	static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);
public:
	static HttpTransfer* GetInstance();
	static void ReleaseInstance();

	string GetRanking();
};

