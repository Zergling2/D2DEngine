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
    ip::math::Vector2 colliderVerticesBufferCCW[4];
    colliderVerticesBufferCCW[0] = ip::math::Vector2(-35.0f, -55.0f);
    colliderVerticesBufferCCW[1] = ip::math::Vector2(+35.0f, -55.0f);
    colliderVerticesBufferCCW[2] = ip::math::Vector2(+35.0f, +55.0f);
    colliderVerticesBufferCCW[3] = ip::math::Vector2(-35.0f, +55.0f);
    pControllableRect->CreateCollider(colliderVerticesBufferCCW, 4);
    pControllableRect->Collider()->CreateRigidBody(real(1.0), real(0.5), real(0.6), false);
    pControllableRect->Collider()->m_position = ip::math::Vector2(0.0f, 300.0f);
    AddObject(pControllableRect, LayerType::Interactable);

    Circle* pCircle = new Circle(25.0f);
    pCircle->CreateCollider(25.0f);
    pCircle->Collider()->CreateRigidBody(real(0.5), real(0.5), real(0.8), false);
    pCircle->Collider()->m_position = ip::math::Vector2(0.0f, 300.0f);
    AddObject(pCircle, LayerType::Interactable);

    pCircle = new Circle(25.0f);
    pCircle->CreateCollider(25.0f);
    pCircle->Collider()->CreateRigidBody(real(1.2), real(0.5), real(0.8), false);
    pCircle->Collider()->m_position = ip::math::Vector2(0.0f, 300.0f);
    AddObject(pCircle, LayerType::Interactable);
    
    pCircle = new Circle(25.0f);
    pCircle->CreateCollider(25.0f);
    pCircle->Collider()->CreateRigidBody(real(1.2), real(0.5), real(0.8), false);
    pCircle->Collider()->m_position = ip::math::Vector2(0.0f, 310.0f);
    AddObject(pCircle, LayerType::Interactable);
    
    pCircle = new Circle(25.0f);
    pCircle->CreateCollider(25.0f);
    pCircle->Collider()->CreateRigidBody(real(1.2), real(0.5), real(0.8), false);
    pCircle->Collider()->m_position = ip::math::Vector2(0.0f, 290.0f);
    AddObject(pCircle, LayerType::Interactable);
    
    pCircle = new Circle(25.0f);
    pCircle->CreateCollider(25.0f);
    pCircle->Collider()->CreateRigidBody(real(0.9), real(0.5), real(0.8), false);
    pCircle->Collider()->m_position = ip::math::Vector2(0.0f, 300.0f);
    AddObject(pCircle, LayerType::Interactable);
    
    pCircle = new Circle(25.0f);
    pCircle->CreateCollider(25.0f);
    pCircle->Collider()->CreateRigidBody(real(0.75), real(0.5), real(0.8), false);
    pCircle->Collider()->m_position = ip::math::Vector2(-150.0f, 500.0f);
    AddObject(pCircle, LayerType::Interactable);
    
    pCircle = new Circle(25.0f);
    pCircle->CreateCollider(25.0f);
    pCircle->Collider()->CreateRigidBody(real(0.6), real(0.5), real(0.8), false);
    pCircle->Collider()->m_position = ip::math::Vector2(-100.0f, 500.0f);
    AddObject(pCircle, LayerType::Interactable);

    Square* pBox = new Square(D2D_RECT_F{ -500.0f, 10.0f, 500.0f, -10.0f });
    colliderVerticesBufferCCW[0] = ip::math::Vector2(-500.0f, -10.0f);
    colliderVerticesBufferCCW[1] = ip::math::Vector2(+500.0f, -10.0f);
    colliderVerticesBufferCCW[2] = ip::math::Vector2(+500.0f, +10.0f);
    colliderVerticesBufferCCW[3] = ip::math::Vector2(-500.0f, +10.0f);
    pBox->CreateCollider(colliderVerticesBufferCCW, 4);
    pBox->Collider()->CreateRigidBody(real(10.0), real(0.5), real(0.5), true);
    pBox->Collider()->m_orientation = 25.0f;
    pBox->Collider()->m_position = ip::math::Vector2(0.0f, -100.0f);
    AddObject(pBox, LayerType::Ground);

    pBox = new Square(D2D_RECT_F{ -250.0f, 10.0f, 250.0f, -10.0f });
    colliderVerticesBufferCCW[0] = ip::math::Vector2(-250.0f, -10.0f);
    colliderVerticesBufferCCW[1] = ip::math::Vector2(+250.0f, -10.0f);
    colliderVerticesBufferCCW[2] = ip::math::Vector2(+250.0f, +10.0f);
    colliderVerticesBufferCCW[3] = ip::math::Vector2(-250.0f, +10.0f);
    pBox->CreateCollider(colliderVerticesBufferCCW, 4);
    pBox->Collider()->CreateRigidBody(real(10.0), real(0.5), real(0.5), true);
    pBox->Collider()->m_orientation = -20.0f;
    pBox->Collider()->m_position = ip::math::Vector2(-100.0f, 200.0f);
    AddObject(pBox, LayerType::Ground);

    pBox = new Square(D2D_RECT_F{ -250.0f, 10.0f, 250.0f, -10.0f });
    colliderVerticesBufferCCW[0] = ip::math::Vector2(-250.0f, -10.0f);
    colliderVerticesBufferCCW[1] = ip::math::Vector2(+250.0f, -10.0f);
    colliderVerticesBufferCCW[2] = ip::math::Vector2(+250.0f, +10.0f);
    colliderVerticesBufferCCW[3] = ip::math::Vector2(-250.0f, +10.0f);
    pBox->CreateCollider(colliderVerticesBufferCCW, 4);
    pBox->Collider()->CreateRigidBody(real(10.0), real(0.5), real(0.5), true);
    pBox->Collider()->m_orientation = 90.0f;
    pBox->Collider()->m_position = ip::math::Vector2(-400.0f, -300.0f);
    AddObject(pBox, LayerType::Ground);
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
