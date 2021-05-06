#pragma once

typedef void(*CALLBACK_FUNC)(void);
typedef void(*CALLBACK_FUNC_PACKET)(char*);

enum class State
{
	MAIN_MENU = 0,
	RANK_MENU = 1,
	GAME = 2,
};

constexpr const int MAX_PACKET_SIZE = 1024;