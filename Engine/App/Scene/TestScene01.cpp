#include "TestScene01.h"
#include "App\Assets\Object\ControllableRectangle\ControllableRectangle.h"
#include "App\Assets\Object\Square\Square.h"
#include "App\Assets\Object\Pentagon\Pentagon.h"
#include "App\Assets\Object\Triangle\Triangle.h"
#include "App\Assets\Object\Circle\Circle.h"
#include "Core\Engine.h"

using namespace D2DEngine;

D2DEngine::Scene* CreateTestScene01()
{
    return new TestScene01;
}

TestScene01::TestScene01()
    : m_pCamera(nullptr)
{
    RECT rc;
    Engine::GetInstance().GetClientRect(&rc);
    
    m_pCamera = new Camera(static_cast<float>(rc.right - rc.left), static_cast<float>(rc.bottom - rc.top), 1.0f);
    SetRenderingCamera(m_pCamera);
}

TestScene01::~TestScene01()
{
    SafeDeleteScalar(m_pCamera);
}

void TestScene01::OnStart()
{
#ifdef _DEBUG
    Log::Print(Log::Type::LT_INFO, "TestScene01::OnStart");
#endif // _DEBUG

    // Player* pPlayer = new Player(ActiveFlag(true), L"플레이어", ObjectTag::Default);
    // pPlayer->GetTransform().SetPosition(D2D_VECTOR_2F{ 0.0f, 0.0f });
    // AddObject(pPlayer, LayerType::Player);

    ControllableRectangle* pControllableRect = new ControllableRectangle(D2D_RECT_F{ -37.5f, 50.0f, 37.5f, -50.0f });
    pControllableRect->CreateCollider(ip::math::Size2{ 75.0f, 100.0f });
    pControllableRect->Collider()->CreateRigidBody(real(5.0), real(2.0), real(0.6), real(0.15), real(0.1), false);
    pControllableRect->Collider()->SetPosition(0.0f, 400.0f);
    AddObject(pControllableRect, LayerType::Interactable);



    // D2D1_POINT_2F shapeBuffer[10];
    // float x = 0.0f;
    // float y = 0.0f;
    // shapeBuffer[0].x = -25.0f;
    // shapeBuffer[0].y = -25.0f;
    // shapeBuffer[1].x = +25.0f;
    // shapeBuffer[1].y = -25.0f;
    // shapeBuffer[2].x = +35.0f;
    // shapeBuffer[2].y = 0.0f;
    // shapeBuffer[3].x = 0.0f;
    // shapeBuffer[3].y = 35.0f;
    // shapeBuffer[4].x = -35.0f;
    // shapeBuffer[4].y = 0.0f;
    // x = 0.0f;
    // y = 0.0f;
    // for (int i = 0; i < 5; i++)
    // {
    //     x += shapeBuffer[i].x;
    //     y += shapeBuffer[i].y;
    // }
    // x /= 5.0f;
    // y /= 5.0f;
    // for (int i = 0; i < 5; i++)
    // {
    //     shapeBuffer[i].x -= x;
    //     shapeBuffer[i].y -= y;
    // }
    // Pentagon* pentagon = new Pentagon(shapeBuffer);
    // pentagon->CreateCollider(reinterpret_cast<ip::math::Vector2*>(shapeBuffer));
    // pentagon->Collider()->CreateRigidBody(real(1.8), real(0.5), real(0.5), real(0.3), real(0.1), false);
    // pentagon->Collider()->SetRotation(real(0.0));
    // pentagon->Collider()->SetPosition(0.0, 300.0);
    // AddObject(pentagon, LayerType::Interactable);






    // shapeBuffer[0].x = -25.0f;
    // shapeBuffer[0].y = -25.0f;
    // shapeBuffer[1].x = +25.0f;
    // shapeBuffer[1].y = -25.0f;
    // shapeBuffer[2].x = 0.0f;
    // shapeBuffer[2].y = +32.5f;
    // x = 0.0f;
    // y = 0.0f;
    // for (int i = 0; i < 3; i++)
    // {
    //     x += shapeBuffer[i].x;
    //     y += shapeBuffer[i].y;
    // }
    // x /= 3.0f;
    // y /= 3.0f;
    // for (int i = 0; i < 3; i++)
    // {
    //     shapeBuffer[i].x -= x;
    //     shapeBuffer[i].y -= y;
    // }
    // Triangle* tri = new Triangle(shapeBuffer);
    // tri->CreateCollider(reinterpret_cast<ip::math::Vector2*>(shapeBuffer));
    // tri->Collider()->CreateRigidBody(real(1.8), real(0.5), real(0.5), real(0.3), real(0.1), false);
    // tri->Collider()->SetRotation(real(0.0));
    // tri->Collider()->SetPosition(0.0, 300.0);
    // AddObject(tri, LayerType::Interactable);


    // Square* b = new Square(D2D_RECT_F{ -35.0f, 15.0f, 35.0f, -15.0f });
    // b->CreateCollider(ip::math::Size2{ 70.0f, 30.0f });
    // b->Collider()->CreateRigidBody(real(1.0), real(0.5), real(0.5), real(0.15), real(0.1), false);
    // b->Collider()->SetRotation(D2DEngine::Mathf::DegreeToRadian(0.0f));
    // b->Collider()->SetPosition(0.0, 700.0);
    // AddObject(b, LayerType::Interactable);
    // 
    // b = new Square(D2D_RECT_F{ -35.0f, 15.0f, 35.0f, -15.0f });
    // b->CreateCollider(ip::math::Size2{ 70.0f, 30.0f });
    // b->Collider()->CreateRigidBody(real(1.0), real(0.5), real(0.5), real(0.15), real(0.1), false);
    // b->Collider()->SetRotation(D2DEngine::Mathf::DegreeToRadian(0.0f));
    // b->Collider()->SetPosition(5.0f, 1000.0f);
    // AddObject(b, LayerType::Interactable);
    // 
    // b = new Square(D2D_RECT_F{ -35.0f, 15.0f, 35.0f, -15.0f });
    // b->CreateCollider(ip::math::Size2{ 70.0f, 30.0f });
    // b->Collider()->CreateRigidBody(real(1.0), real(0.5), real(0.5), real(0.15), real(0.1), false);
    // b->Collider()->SetRotation(D2DEngine::Mathf::DegreeToRadian(0.0f));
    // b->Collider()->SetPosition(15.0f, 1300.0f);
    // AddObject(b, LayerType::Interactable);
    // 
    // b = new Square(D2D_RECT_F{ -35.0f, 15.0f, 35.0f, -15.0f });
    // b->CreateCollider(ip::math::Size2{ 70.0f, 30.0f });
    // b->Collider()->CreateRigidBody(real(1.0), real(0.5), real(0.5), real(0.15), real(0.1), false);
    // b->Collider()->SetRotation(D2DEngine::Mathf::DegreeToRadian(0.0f));
    // b->Collider()->SetPosition(-10.0f, 1600.0f);
    // AddObject(b, LayerType::Interactable);
    // 
    // b = new Square(D2D_RECT_F{ -35.0f, 15.0f, 35.0f, -15.0f });
    // b->CreateCollider(ip::math::Size2{ 70.0f, 30.0f });
    // b->Collider()->CreateRigidBody(real(1.0), real(0.5), real(0.5), real(0.15), real(0.1), false);
    // b->Collider()->SetRotation(D2DEngine::Mathf::DegreeToRadian(0.0f));
    // b->Collider()->SetPosition(-20.0f, 1900.0f);
    // AddObject(b, LayerType::Interactable);

    Circle* pCircle = new Circle(25.0f);
    pCircle->CreateCollider(25.0f);
    pCircle->Collider()->CreateRigidBody(real(0.5), real(0.5), real(0.8), real(0.3), real(0.1), false);
    pCircle->Collider()->SetPosition(0.0f, 300.0f);
    AddObject(pCircle, LayerType::Interactable);
    
    pCircle = new Circle(25.0f);
    pCircle->CreateCollider(25.0f);
    pCircle->Collider()->CreateRigidBody(real(0.75), real(0.5), real(0.8), real(0.3), real(0.1), false);
    pCircle->Collider()->SetPosition(-150.0f, 500.0f);
    AddObject(pCircle, LayerType::Interactable);
    
    pCircle = new Circle(25.0f);
    pCircle->CreateCollider(25.0f);
    pCircle->Collider()->CreateRigidBody(real(0.6), real(0.5), real(0.8), real(0.3), real(0.1), false);
    pCircle->Collider()->SetPosition(-100.0f, 500.0f);
    AddObject(pCircle, LayerType::Interactable);

    Square* pBox = new Square(D2D_RECT_F{ -500.0f, 10.0f, 500.0f, -10.0f });
    pBox->CreateCollider(ip::math::Size2{ 1000.0f, 20.0f });
    pBox->Collider()->CreateRigidBody(real(1000.0), real(0.5), real(0.5), real(0.2), real(0.1), true);
    pBox->Collider()->SetRotation(D2DEngine::Mathf::DegreeToRadian(15.0f));
    pBox->Collider()->SetPosition(0.0f, -100.0f);
    AddObject(pBox, LayerType::Ground);
    
    pBox = new Square(D2D_RECT_F{ -250.0f, 10.0f, 250.0f, -10.0f });
    pBox->CreateCollider(ip::math::Size2{ 500.0f, 20.0f });
    pBox->Collider()->CreateRigidBody(real(1000.0), real(0.5), real(0.5), real(0.2), real(0.1), true);
    pBox->Collider()->SetRotation(D2DEngine::Mathf::DegreeToRadian(-20.0f));
    pBox->Collider()->SetPosition(-100.0f, 200.0f);
    AddObject(pBox, LayerType::Ground);
    
    pBox = new Square(D2D_RECT_F{ -250.0f, 10.0f, 250.0f, -10.0f });
    pBox->CreateCollider(ip::math::Size2{ 500.0f, 20.0f });
    pBox->Collider()->CreateRigidBody(real(1000.0), real(0.5), real(0.5), real(0.2), real(0.1), true);
    pBox->Collider()->SetRotation(D2DEngine::Mathf::DegreeToRadian(90.0f));
    pBox->Collider()->SetPosition(-400, -300);
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
