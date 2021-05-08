#include "main.h"

int main()
{
	cout << "Game Server" << endl << endl;
	GameServer::GetInstance()->StartServer();
}