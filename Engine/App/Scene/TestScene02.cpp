#include "TestScene02.h"
#include "App\Assets\Object\Square\Square.h"
#include "App\Assets\Object\Circle\Circle.h"
#include "Core\Engine.h"

using namespace D2DEngine;

D2DEngine::Scene* CreateTestScene02()
{
    return new TestScene02;
}

TestScene02::TestScene02()
    : pCamera(nullptr)
{
    RECT rc;
    Engine::GetInstance().GetClientRect(&rc);

    pCamera = new Camera(static_cast<float>(rc.right - rc.left), static_cast<float>(rc.bottom - rc.top), 1.0f);
    SetRenderingCamera(pCamera);
}

TestScene02::~TestScene02()
{
    SafeDeleteScalar(pCamera);
}

void TestScene02::OnStart()
{
#ifdef _DEBUG
    Log::Print(Log::Type::LT_INFO, "TestScene02::OnStart");
#endif // _DEBUG

    Square* pPlayer = new Square(ActiveFlag(true), L"¹Ú½º1", ObjectTag::Default);
    pPlayer->GetTransform().SetPosition(D2D_VECTOR_2F{ -500.0f, 300.0f });
    AddObject(pPlayer, LayerType::Player);

    Circle* pCircle = new Circle(ActiveFlag(true), L"¿ø1", ObjectTag::Default);
    pCircle->GetTransform().SetPosition(D2D_VECTOR_2F{ 200.0f, 200.0f });
    AddObject(pCircle, LayerType::Interactable);
}

void TestScene02::CreateDeviceDependentResources()
{
#ifdef _DEBUG
    Log::Print(Log::Type::LT_SYSTEM, "TestScene02::CreateDeviceDependentResources");
#endif // _DEBUG
}

void TestScene02::ReleaseDeviceDependentResources()
{
#ifdef _DEBUG
    Log::Print(Log::Type::LT_SYSTEM, "TestScene02::ReleaseDeviceDependentResources");
#endif // _DEBUG
}

void TestScene02::OnDestroy()
{
#ifdef _DEBUG
    Log::Print(Log::Type::LT_INFO, "TestScene02::OnDestroy");
#endif // _DEBUG
}
