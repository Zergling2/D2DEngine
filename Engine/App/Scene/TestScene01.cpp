#include "TestScene01.h"
#include "App\Assets\Object\Square\Square.h"
#include "App\Assets\Object\Circle\Circle.h"
#include "Core\Engine.h"

using namespace D2DEngine;

D2DEngine::Scene* CreateTestScene01()
{
    return new TestScene01;
}

TestScene01::TestScene01()
    : pCamera(nullptr)
{
    RECT rc;
    Engine::GetInstance().GetClientRect(&rc);
    
    pCamera = new Camera(static_cast<float>(rc.right - rc.left), static_cast<float>(rc.bottom - rc.top), 1.0f);
    SetRenderingCamera(pCamera);
}

TestScene01::~TestScene01()
{
    SafeDeleteScalar(pCamera);
}

void TestScene01::OnStart()
{
#ifdef _DEBUG
    Log::Print(Log::Type::LT_INFO, "TestScene01::OnStart");
#endif // _DEBUG

    Square* pPlayer = new Square(ActiveFlag(true), L"¹Ú½º1", ObjectTag::Default);
    pPlayer->GetTransform().SetPosition(D2D_VECTOR_2F{ 0.0f, 0.0f });
    AddObject(pPlayer, LayerType::Player);

    Circle* pCircle = new Circle(ActiveFlag(true), L"¿ø1", ObjectTag::Default);
    pCircle->GetTransform().SetPosition(D2D_VECTOR_2F{ 50.0f, 100.0f });
    AddObject(pCircle, LayerType::Interactable);
}

void TestScene01::CreateDeviceDependentResources()
{
#ifdef _DEBUG
    Log::Print(Log::Type::LT_SYSTEM, "TestScene01::CreateDeviceDependentResources");
#endif // _DEBUG
}

void TestScene01::ReleaseDeviceDependentResources()
{
#ifdef _DEBUG
    Log::Print(Log::Type::LT_SYSTEM, "TestScene01::ReleaseDeviceDependentResources");
#endif // _DEBUG
}

void TestScene01::OnDestroy()
{
#ifdef _DEBUG
    Log::Print(Log::Type::LT_INFO, "TestScene01::OnDestroy");
#endif // _DEBUG
}
