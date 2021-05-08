#include "MatchMakingServer.h"

int main()
{
	cout << "MatchMakingServer" << endl << endl;
	MatchMakingServer::GetInstance()->StartServer();
}