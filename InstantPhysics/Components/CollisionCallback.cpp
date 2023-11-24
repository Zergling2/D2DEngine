#include "CollisionCallback.h"
#include "Components\IPDebug.h"
#include <processthreadsapi.h>
#include <iostream>
#include <fstream>
#include <strsafe.h>

using namespace ip;

DWORD TLSCollisionNodePool::s_tlsIndex;

bool TLSCollisionNodePool::Initialize()
{
    DWORD tlsIndex = TlsAlloc();
    if (tlsIndex == TLS_OUT_OF_INDEXES)
    {
        DWORD code = GetLastError();

        std::ofstream fLog;
        fLog.open("IPErrorLog.txt", std::ios::out);
        if (fLog.fail())
            return false;

        char gleBuf[256];
        IPDebug::GetDetailError(code, gleBuf, sizeof(gleBuf));
        char buf[256];
        StringCbPrintfA(buf, sizeof(buf), "TlsAlloc() failed. (%u)%s\n", code, gleBuf);
        fLog.write(buf, strlen(buf));

        fLog.close();
        return false;
    }

    TLSCollisionNodePool::s_tlsIndex = tlsIndex;
    return false;
}
