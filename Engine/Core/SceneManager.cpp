#include "SceneManager.h"
#include "Core\Scene.h"
#include <stdexcept>
#include <algorithm>

using namespace D2DEngine;

SceneManager SceneManager::s_instance;

SceneManager::SceneManager()
    : m_pCurrentScene(nullptr)
    , m_currentSceneId(-1)
    , m_nextSceneId(-1)
    , m_sceneChangeFlag(true)
{
}

SceneManager::~SceneManager()
{
}

void SceneManager::RegisterSceneTable(SceneTable table[], unsigned int count)
{
    for (unsigned int i = 0; i < count; i++)
    {
        m_sceneTable.push_back(table[i]);
    }

    std::sort(m_sceneTable.begin(), m_sceneTable.end(), SceneTable::Compare);
}

bool SceneManager::CreateNextScene()
{
    m_sceneChangeFlag = false;

    Scene* pOldScene = m_pCurrentScene;
    if (pOldScene)
    {
        pOldScene->OnDestroy();
        delete pOldScene;
        m_pCurrentScene = nullptr;
    }

    for (size_t i = 0; i < m_sceneTable.size(); i++)
    {
        if (m_sceneTable[i].id == m_nextSceneId)
        {
            m_pCurrentScene = m_sceneTable[i].SceneFactoryFucntion();       // Create Scene
            break;
        }
    }

    if (!m_pCurrentScene)
    {
#ifdef _DEBUG
        char buffer[128];
        StringCbPrintfA(buffer, sizeof(buffer), "Invalid scene id(%d). Can't load scene.", m_nextSceneId);
        D2DEngine::Log::Print(D2DEngine::Log::Type::LT_CRITICAL, buffer);
#endif // _DEBUG
        return false;
    }
    else
    {
#ifdef _DEBUG
        char buffer[128];
        StringCbPrintfA(buffer, sizeof(buffer), "Scene id(%d) created.", m_nextSceneId);
        D2DEngine::Log::Print(D2DEngine::Log::Type::LT_SYSTEM, buffer);
#endif // _DEBUG
        m_currentSceneId = m_nextSceneId;
        m_nextSceneId = -1;
        m_pCurrentScene->Initialize();
        m_pCurrentScene->CreateDeviceDependentResources();
        m_pCurrentScene->OnStart();
        return true;
    }
}