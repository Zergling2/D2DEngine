#pragma once

#include <Windows.h>
#include <list>
#include <vector>
#include <set>
#include <unordered_map>
#include <string>
#include <tchar.h>
#include <strsafe.h>

#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>

#include "Headers\Macro.h"
#include "Mathf\Mathf.h"
#include "Core\Debug.h"

// ================== TYPEDEF ==================
typedef long long InstanceID;

// ================== CONFIG ==================
LPCWSTR CONST WNDCLASS_NAME = L"Main Class";
LPCWSTR CONST WINDOW_TITLE = L"제목 없음";

CONST ULONG RES_WIDTH = 1600UL;
CONST ULONG RES_HEIGHT = 900UL;

CONST UINT RENDER_FREQUENCY = 144U;
CONST FLOAT RENDER_INTERVAL_TIME = 1.0F / (FLOAT)RENDER_FREQUENCY;
CONST UINT FIXED_FREQUENCY = 60U;
CONST FLOAT FIXED_DELTA_TIME = 1.0F / (FLOAT)FIXED_FREQUENCY;
// ============================================
