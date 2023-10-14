#include "TestScene01.h"
#include "App\Assets\Object\ControllableRectangle\ControllableRectangle.h"
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

    // Player* pPlayer = new Player(ActiveFlag(true), L"플레이어", ObjectTag::Default);
    // pPlayer->GetTransform().SetPosition(D2D_VECTOR_2F{ 0.0f, 0.0f });
    // AddObject(pPlayer, LayerType::Player);

    ControllableRectangle* pControllableRect = new ControllableRectangle(D2D_RECT_F{ -35.0f, 55.0f, 35.0f, -55.0f });
    ip::math::Vector2 colliderVertices1[4];
    colliderVertices1[0] = ip::math::Vector2(-35.0f, -55.0f);
    colliderVertices1[1] = ip::math::Vector2(+35.0f, -55.0f);
    colliderVertices1[2] = ip::math::Vector2(+35.0f, +55.0f);
    colliderVertices1[3] = ip::math::Vector2(-35.0f, +55.0f);
    pControllableRect->CreateCollider(colliderVertices1, 4);
    pControllableRect->Collider()->m_position = ip::math::Vector2(-60.0f, 48.0f);
    AddObject(pControllableRect, LayerType::Interactable);

    // Square* pBox = new Square(D2D_RECT_F{ -25.0f, 25.0f, 25.0f, -25.0f });
    // ip::math::Vector2 colliderVertices2[4];
    // colliderVertices2[0] = ip::math::Vector2(-25.0f, -25.0f);
    // colliderVertices2[1] = ip::math::Vector2(+25.0f, -25.0f);
    // colliderVertices2[2] = ip::math::Vector2(+25.0f, +25.0f);
    // colliderVertices2[3] = ip::math::Vector2(-25.0f, +25.0f);
    // pBox->CreateCollider(colliderVertices2, 4);
    // pBox->Collider()->m_position = ip::math::Vector2(28.0f, -36.0f);
    // AddObject(pBox, LayerType::Interactable);

    
    Circle* pCircle = new Circle(25.0f);
    pCircle->CreateCollider(25.0f);
    pCircle->Collider()->m_position = ip::math::Vector2(69.52f, -37.721f);
    AddObject(pCircle, LayerType::Interactable);

    /*
    Circle* pCircle2 = new Circle(ActiveFlag(true), L"원2", ObjectTag::Default);
    pCircle2->GetRigidBody()->m_pos = ip::math::Vector2(20.0f, -96.0f);
    pCircle2->GetRigidBody()->m_invMass = 0.6;
    AddObject(pCircle2, LayerType::Interactable);
    // pCircle2->GetRigidBody()->gravity = false;

    Circle* pCircle3 = new Circle(ActiveFlag(true), L"원3", ObjectTag::Default);
    pCircle3->GetRigidBody()->m_pos = ip::math::Vector2(80.0f, -96.0f);
    pCircle3->GetRigidBody()->m_invMass = 0.3;
    AddObject(pCircle3, LayerType::Interactable);
    // pCircle3->GetRigidBody()->gravity = false;
    
    Circle* pCircle4 = new Circle(ActiveFlag(true), L"원4", ObjectTag::Default);
    pCircle4->GetRigidBody()->m_pos = ip::math::Vector2(140.0f, -96.0f);
    pCircle4->GetRigidBody()->m_invMass = 0.15;
    AddObject(pCircle4, LayerType::Interactable);
    // pCircle4->GetRigidBody()->gravity = false;
    
    Circle* pCircle5 = new Circle(ActiveFlag(true), L"원5", ObjectTag::Default);
    pCircle5->GetRigidBody()->m_pos = ip::math::Vector2(200.0f, -96.0f);
    pCircle5->GetRigidBody()->m_invMass = 0.8;
    AddObject(pCircle5, LayerType::Interactable);
    // pCircle5->GetRigidBody()->gravity = false;
    
    Circle* pCircle6 = new Circle(ActiveFlag(true), L"원6", ObjectTag::Default);
    pCircle6->GetRigidBody()->m_pos = ip::math::Vector2(-40.0f, -96.0f);
    pCircle6->GetRigidBody()->m_invMass = 2.5;
    AddObject(pCircle6, LayerType::Interactable);
    // pCircle6->GetRigidBody()->gravity = false;
    */
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
