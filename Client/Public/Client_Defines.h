#pragma once

namespace Client
{
	const unsigned int	g_iWinCX = 1280;
	const unsigned int	g_iWinCY = 720;

	enum LEVEL { LEVEL_STATIC, LEVEL_LOADING, LEVEL_LOGO, LEVEL_GAMEPLAY, LEVEL_END };
}

#include <process.h>

#define		VK_MAX		0xff

extern HINSTANCE g_hInst;
extern HWND g_hWnd;

using namespace Client;
