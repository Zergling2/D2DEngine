#include "Debug.h"
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <strsafe.h>

using namespace D2DEngine;

Log::Color Log::s_currentColor = Log::Color::DARK_GRAY;

void Log::Print(Type type, const wchar_t* str)
{
	switch (type)
	{
	case Type::LT_SYSTEM:
		SetColor(Color::GRAY);
		wprintf(FMT_WCH_SYSTEM, str);
		break;
	case Type::LT_INFO:
		SetColor(Color::GREEN);
		wprintf(FMT_WCH_INFO, str);
		break;
	case Type::LT_CRITICAL:
		SetColor(Color::DARK_RED);
		wprintf(FMT_WCH_CRITICAL, str);
		break;
	case Type::LT_ERROR:
		SetColor(Color::DARK_YELLOW);
		wprintf(FMT_WCH_ERROR, str);
		break;
	case Type::LT_WARNING:
		SetColor(Color::RED);
		wprintf(FMT_WCH_WARNING, str);
		break;
	case Type::LT_FAIL:
		SetColor(Color::PURPLE);
		wprintf(FMT_WCH_FAIL, str);
		break;
	case Type::LT_LAST_ERROR:
		SetColor(Color::DARK_BLUE);
		wprintf(FMT_WCH_LAST_ERROR, str);
		break;
	case Type::LT_NETWORK:
		SetColor(Color::SKY_BLUE);
		wprintf(FMT_WCH_NETWORK, str);
		break;
	default:
		SetColor(Color::WHITE);
		wprintf(FMT_WCH_NONE, str);
		break;
	}
}

void Log::Print(Type type, const char* str)
{
	switch (type)
	{
	case Type::LT_SYSTEM:
		SetColor(Color::GRAY);
		printf(FMT_CH_SYSTEM, str);
		break;
	case Type::LT_INFO:
		SetColor(Color::GREEN);
		printf(FMT_CH_INFO, str);
		break;
	case Type::LT_CRITICAL:
		SetColor(Color::DARK_RED);
		printf(FMT_CH_CRITICAL, str);
		break;
	case Type::LT_ERROR:
		SetColor(Color::DARK_YELLOW);
		printf(FMT_CH_ERROR, str);
		break;
	case Type::LT_WARNING:
		SetColor(Color::RED);
		printf(FMT_CH_WARNING, str);
		break;
	case Type::LT_FAIL:
		SetColor(Color::PURPLE);
		printf(FMT_CH_FAIL, str);
		break;
	case Type::LT_LAST_ERROR:
		SetColor(Color::DARK_BLUE);
		printf(FMT_CH_LAST_ERROR, str);
		break;
	case Type::LT_NETWORK:
		SetColor(Color::SKY_BLUE);
		printf(FMT_CH_NETWORK, str);
		break;
	default:
		SetColor(Color::WHITE);
		printf(FMT_CH_NONE, str);
		break;
	}
}

void Log::SetColor(Color color)
{
	if (Log::s_currentColor != color)
	{
		Log::s_currentColor = color;
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), static_cast<WORD>(color));
	}
}
