#include "IPDebug.h"
#include <strsafe.h>

using namespace ip;

void IPDebug::GetDetailError(DWORD code, char* buf, size_t cbDest)
{
    LPVOID lpMsgBuffer;
    FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, code,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        reinterpret_cast<LPSTR>(&lpMsgBuffer), 0, NULL);
    StringCbPrintfA(buf, cbDest, "%s", reinterpret_cast<LPCSTR>(lpMsgBuffer));
    size_t len = strlen(buf);
    if (len >= 2)
        buf[len - 2] = '\0';
    LocalFree(lpMsgBuffer);
}
