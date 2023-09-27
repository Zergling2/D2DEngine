//
// 브러시나 렌더타겟은 그래픽카드 메모리에 저장됨 = 장치 의존적 자원(Device Dependent Resource)
// 도형 데이터는 메인 메모리에 저장됨 = 장치 독립적 자원(Device Independent Resource)
//

#include "Core\Engine.h"

#include "App\Scene\SceneId.h"
#include "App\Scene\TestScene01.h"
#include "App\Scene\TestScene02.h"

#ifdef _DEBUG
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#endif


int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int)
{
    // Use HeapSetInformation to specify that the process should
    // terminate if the heap manager detects an error in any heap used
    // by the process.
    // The return value is ignored, because we want to continue running in the
    // unlikely event that HeapSetInformation fails.
    // HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);

    if (FAILED(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED)))         // https://imcyber.tistory.Component/115
    {
        return -1;
    }

    if (!D2DEngine::Engine::GetInstance().Initialize())
    {
        wchar_t buf[32];
        wsprintfW(buf, L"CODE: %u", GetLastError());
        MessageBoxW(NULL, buf, L"FATAL ERROR", MB_OK);
        return -1;
    }

    D2DEngine::SceneTable table[2];
    table[0].id = static_cast<int>(SceneId::TestScene01);
    table[0].SceneFactoryFucntion = CreateTestScene01;
    table[1].id = static_cast<int>(SceneId::TestScene02);
    table[1].SceneFactoryFucntion = CreateTestScene02;

    D2DEngine::SceneManager::GetInstance().RegisterSceneTable(table, _countof(table));
    D2DEngine::SceneManager::GetInstance().SetFirstScene(static_cast<int>(SceneId::TestScene01));

    int ret = D2DEngine::Engine::GetInstance().Run();
    CoUninitialize();

    return ret;
}