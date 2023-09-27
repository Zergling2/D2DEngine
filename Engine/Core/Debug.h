#pragma once

namespace D2DEngine
{
	const char* const FMT_CH_SYSTEM = "[SYSTEM] %s\n";
	const wchar_t* const FMT_WCH_SYSTEM = L"[SYSTEM] %s\n";
	const char* const FMT_CH_INFO = "[INFO] %s\n";
	const wchar_t* const FMT_WCH_INFO = L"[INFO] %s\n";
	const char* const FMT_CH_CRITICAL = "[CRITICAL] %s\n";
	const wchar_t* const FMT_WCH_CRITICAL = L"[CRITICAL] %s\n";
	const char* const FMT_CH_ERROR = "[ERROR] %s\n";
	const wchar_t* const FMT_WCH_ERROR = L"[ERROR] %s\n";
	const char* const FMT_CH_WARNING = "[WARNING] %s\n";
	const wchar_t* const FMT_WCH_WARNING = L"[WARNING] %s\n";
	const char* const FMT_CH_FAIL = "[FAIL] %s\n";
	const wchar_t* const FMT_WCH_FAIL = L"[FAIL] %s\n";
	const char* const FMT_CH_LAST_ERROR = "[LAST_ERROR] %s\n";
	const wchar_t* const FMT_WCH_LAST_ERROR = L"[LAST_ERROR] %s\n";
	const char* const FMT_CH_NETWORK = "[NETWORK I/O] %s\n";
	const wchar_t* const FMT_WCH_NETWORK = L"[NETWORK I/O] %s\n";
	const char* const FMT_CH_NONE = "%s\n";
	const wchar_t* const FMT_WCH_NONE = L"%s\n";


	class Log
	{
	private:
		enum class Color
		{
			BLACK,  		//  0
			DARK_BLUE,		//  1
			DARK_GREEN,		//  2
			DARK_SKY_BLUE,  //  3
			DARK_RED,  		//  4
			DARK_PURPLE,	//  5
			DARK_YELLOW,	//  6
			GRAY,			//  7
			DARK_GRAY,		//  8
			BLUE,			//  9
			GREEN,			// 10
			SKY_BLUE,		// 11
			RED,			// 12
			PURPLE,			// 13
			YELLOW,			// 14
			WHITE			// 15
		};

	public:
		enum class Type
		{
			// Log Type
			LT_SYSTEM,
			LT_INFO,
			LT_CRITICAL,
			LT_ERROR,
			LT_WARNING,
			LT_FAIL,
			LT_LAST_ERROR,
			LT_NETWORK,
			LT_NO_PREFIX
		};
	public:
		static void Print(Type type, const wchar_t* str);
		static void Print(Type type, const char* str);
	private:
		static void SetColor(Color color);
	private:
		static Color s_currentColor;
	};
}
