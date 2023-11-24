#include "TestScene01.h"
#include "App\Assets\Object\Car.h"
#include "App\Assets\Object\Wall.h"
#include "App\Assets\Object\AlYac.h"
#include "App\Assets\Object\Stone.h"
#include "App\Assets\Object\Triangle.h"
#include "App\Assets\Object\Circle.h"
#include "App\Assets\Object\Oscillation.h"
#include "App\Assets\Object\Ground.h"
#include "App\Assets\Object\Rope.h"
#include "Core\Engine.h"

Scene* CreateTestScene01()
{
    return new TestScene01;
}

TestScene01::TestScene01()
    : m_pCamera(nullptr)
{
    RECT rc;
    Engine::GetInstance().GetClientRect(&rc);
    
    m_pCamera = new Camera(static_cast<float>(rc.right - rc.left), static_cast<float>(rc.bottom - rc.top));
    SetRenderingCamera(m_pCamera);
}

TestScene01::~TestScene01()
{
    D2DESafeDelete(m_pCamera);
}

void TestScene01::OnStart()
{
    m_pCamera->SetScale(100.0f, 100.0f);

#ifdef _DEBUG
    Log::Print(Log::Type::LT_INFO, "TestScene01::OnStart");
#endif // _DEBUG

    Rope* pRope = new Rope(0.1f, 20, 0.25f);
    pRope->SetPosition(-6.0f, 3.0f);
    pRope->CreateRopeSegment();         // SetPosition 하고나서 호출!!
    AddObject(pRope, LayerType::Default);

    Oscillation* o = new Oscillation(0.25f);
    o->CreateRigidBody(ip::Kinematic, 0.0, 0.0, 0.2, 0.2, 0.1, 1.0);
    o->CreateCircleCollider(ip::Default, 0.25f);
    o->SetPosition(1.0, 0.0);
    o->SetAngularVelocity(2);
    o->SetVelocity(0.0, 0.0);
    Engine::GetInstance().PhysWorld().AddPhysicsObject(o);
    AddObject(o, LayerType::Default);
    
    D2D1_POINT_2F shapeBuffer[10];
    float x = 0.0f;
    float y = 0.0f;
    shapeBuffer[0].x = -0.2f;
    shapeBuffer[0].y = -0.25f;
    shapeBuffer[1].x = +0.25f;
    shapeBuffer[1].y = -0.25f;
    shapeBuffer[2].x = +0.28f;
    shapeBuffer[2].y = 0.0f;
    shapeBuffer[3].x = 0.0f;
    shapeBuffer[3].y = 0.35f;
    shapeBuffer[4].x = -0.42f;
    shapeBuffer[4].y = 0.12f;
    x = 0.0f;
    y = 0.0f;
    for (int i = 0; i < 5; i++)
    {
        x += shapeBuffer[i].x;
        y += shapeBuffer[i].y;
    }
    x /= 5;
    y /= 5;
    for (int i = 0; i < 5; i++)
    {
        shapeBuffer[i].x -= x;
        shapeBuffer[i].y -= y;
    }

    Stone* pStone = new Stone(shapeBuffer);
    pStone->CreateHullCollider(ip::Default, reinterpret_cast<ip::math::Vector2*>(shapeBuffer), 5);
    pStone->CreateRigidBody(ip::Dynamic, real(2), real(0.2), real(0.3), real(0.5), real(0.4), real(1.0));
    pStone->SetRotation(Mathf::DegreeToRadian(-24.0f));
    pStone->SetPosition(0.0f, 2.5f);
    Engine::GetInstance().PhysWorld().AddPhysicsObject(pStone);
    AddObject(pStone, LayerType::Interactable);

    pStone = new Stone(shapeBuffer);
    pStone->CreateHullCollider(ip::Default, reinterpret_cast<ip::math::Vector2*>(shapeBuffer), 5);
    pStone->CreateRigidBody(ip::Static, real(2), real(0.2), real(0.3), real(0.0), real(0.0), real(1.0));
    pStone->SetRotation(Mathf::DegreeToRadian(-90.0f));
    pStone->SetPosition(5.0f, 0.5f);
    Engine::GetInstance().PhysWorld().AddPhysicsObject(pStone);
    AddObject(pStone, LayerType::Interactable);

    pStone = new Stone(shapeBuffer);
    pStone->CreateHullCollider(ip::Default, reinterpret_cast<ip::math::Vector2*>(shapeBuffer), 5);
    pStone->CreateRigidBody(ip::Static, real(2), real(0.2), real(0.3), real(0.0), real(0.0), real(1.0));
    pStone->SetRotation(Mathf::DegreeToRadian(-46.0f));
    pStone->SetPosition(-6.5f, 0.75f);
    Engine::GetInstance().PhysWorld().AddPhysicsObject(pStone);
    AddObject(pStone, LayerType::Interactable);
    
    shapeBuffer[0].x = -0.15f;
    shapeBuffer[0].y = -0.35f;
    shapeBuffer[1].x = +0.55f;
    shapeBuffer[1].y = -0.35f;
    shapeBuffer[2].x = 0.0f;
    shapeBuffer[2].y = +0.185f;
    x = 0.0f;
    y = 0.0f;
    for (int i = 0; i < 3; i++)
    {
        x += shapeBuffer[i].x;
        y += shapeBuffer[i].y;
    }
    x /= 3;
    y /= 3;
    for (int i = 0; i < 3; i++)
    {
        shapeBuffer[i].x -= x;
        shapeBuffer[i].y -= y;
    }
    Triangle* tri = new Triangle(shapeBuffer);
    tri->CreateHullCollider(ip::Default, reinterpret_cast<ip::math::Vector2*>(shapeBuffer), 3);
    tri->CreateRigidBody(ip::Dynamic, real(0.2), real(0.02), real(0.03), real(0.3), real(0.15), real(1.0));
    tri->SetRotation(real(0.0));
    tri->SetPosition(0.0, 3.5);
    Engine::GetInstance().PhysWorld().AddPhysicsObject(tri);
    AddObject(tri, LayerType::Interactable);
    
    
    AlYac* alyac = new AlYac(0.2, 2.0);
    alyac->CreateCapsuleCollider(ip::Default, 0.2, 2.0);
    alyac->CreateRigidBody(ip::Kinematic, real(0.2), real(0.02), real(0.03), real(0.3), real(0.15), real(1.0));
    alyac->SetRotation(D2DEngine::Mathf::DegreeToRadian(0.0f));
    alyac->SetPosition(6.0, 5.0);
    Engine::GetInstance().PhysWorld().AddPhysicsObject(alyac);
    AddObject(alyac, LayerType::Interactable);

    alyac = new AlYac(0.15, 1.5);
    alyac->CreateCapsuleCollider(ip::Default, 0.15, 1.5);
    alyac->CreateRigidBody(ip::Kinematic, real(0.2), real(0.02), real(0.03), real(0.0), real(0.0), real(1.0));
    alyac->SetRotation(D2DEngine::Mathf::DegreeToRadian(0.0f));
    alyac->SetPosition(-5.5, 0.0);
    Engine::GetInstance().PhysWorld().AddPhysicsObject(alyac);
    AddObject(alyac, LayerType::Interactable);
    
    Wall* pWall;

    pWall = new Wall(D2D_RECT_F{ -3, -0.1, 3, 0.1 }, 1.0f, 3.0f, ip::math::Vector2::right);
    pWall->CreateBoxCollider(ip::Wall, 6.0, 0.2);
    pWall->CreateRigidBody(ip::Kinematic, real(0), real(0), real(0.3), real(0.4), real(0.4), real(1.0));
    pWall->SetRotation(0);
    pWall->SetPosition(-2, 2.0);
    pWall->StaticReady();
    Engine::GetInstance().PhysWorld().AddPhysicsObject(pWall);
    AddObject(pWall, LayerType::Interactable);

    pWall = new Wall(D2D_RECT_F{ -1, -0.1, 1, 0.1 }, 3.0f, 5.0f, ip::math::Vector2::up);
    pWall->CreateBoxCollider(ip::Wall, 2.0, 0.2);
    pWall->CreateRigidBody(ip::Kinematic, real(0), real(0), real(0.3), real(0.4), real(0.4), real(1.0));
    pWall->SetRotation(0);
    pWall->SetPosition(-1.5, 1.5);
    pWall->StaticReady();
    Engine::GetInstance().PhysWorld().AddPhysicsObject(pWall);
    AddObject(pWall, LayerType::Interactable);
    
    
    Car* pCar = new Car();
    pCar->CreateBoxCollider(ip::CarBody, real(1.0), real(0.5));
    pCar->CreateRigidBody(ip::Kinematic, real(5), real(0.5), real(0.4), real(0.2), real(0.1), real(1.0));
    pCar->SetRotation(0);
    pCar->SetPosition(0.0f, 5.0f);
    Engine::GetInstance().PhysWorld().AddPhysicsObject(pCar);
    AddObject(pCar, LayerType::Interactable);
    
    Ground* pGround;

#define GROUND_Y -4.0f
    
    shapeBuffer[0] = { -0.5, -1.0 };
    shapeBuffer[1] = { 0.5, -1.0 };
    shapeBuffer[2] = { 0.5, 1.4 };
    shapeBuffer[3] = { -0.5, 1.6 };
    pGround = new Ground(shapeBuffer);
    pGround->CreateHullCollider(ip::Ground, reinterpret_cast<ip::math::Vector2*>(shapeBuffer), 4);
    pGround->CreateRigidBody(ip::Static, 0, 0, real(0.35), real(0.2), real(0.1), real(1.0));
    pGround->SetPosition(-10.0f, GROUND_Y);
    pGround->SetRotation(0.0);
    Engine::GetInstance().PhysWorld().AddPhysicsObject(pGround);
    AddObject(pGround, LayerType::Interactable);

    shapeBuffer[0] = { -0.5, -1.0 };
    shapeBuffer[1] = { 0.5, -1.0 };
    shapeBuffer[2] = { 0.5, 0.8 };
    shapeBuffer[3] = { -0.5, 1.4 };
    pGround = new Ground(shapeBuffer);
    pGround->CreateHullCollider(ip::Ground, reinterpret_cast<ip::math::Vector2*>(shapeBuffer), 4);
    pGround->CreateRigidBody(ip::Static, 0, 0, real(0.35), real(0.2), real(0.1), real(1.0));
    pGround->SetPosition(-9.0f, GROUND_Y);
    pGround->SetRotation(0.0);
    Engine::GetInstance().PhysWorld().AddPhysicsObject(pGround);
    AddObject(pGround, LayerType::Interactable);

    shapeBuffer[0] = { -0.5, -1.0 };
    shapeBuffer[1] = { 0.5, -1.0 };
    shapeBuffer[2] = { 0.5, 0.2 };
    shapeBuffer[3] = { -0.5, 0.8 };
    pGround = new Ground(shapeBuffer);
    pGround->CreateHullCollider(ip::Ground, reinterpret_cast<ip::math::Vector2*>(shapeBuffer), 4);
    pGround->CreateRigidBody(ip::Static, 0, 0, real(0.35), real(0.2), real(0.1), real(1.0));
    pGround->SetPosition(-8.0f, GROUND_Y);
    pGround->SetRotation(0.0);
    Engine::GetInstance().PhysWorld().AddPhysicsObject(pGround);
    AddObject(pGround, LayerType::Interactable);
    
    
    shapeBuffer[0] = { -0.5, -1.0 };
    shapeBuffer[1] = { 0.5, -1.0 };
    shapeBuffer[2] = { 0.5, 0.4 };
    shapeBuffer[3] = { -0.5, 0.2 };
    pGround = new Ground(shapeBuffer);
    pGround->CreateHullCollider(ip::Ground, reinterpret_cast<ip::math::Vector2*>(shapeBuffer), 4);
    pGround->CreateRigidBody(ip::Static, 0, 0, real(0.35), real(0.2), real(0.1), real(1.0));
    pGround->SetPosition(-7.0f, GROUND_Y);
    Engine::GetInstance().PhysWorld().AddPhysicsObject(pGround);
    AddObject(pGround, LayerType::Interactable);
    
    shapeBuffer[0] = { -0.5, -1.0 };
    shapeBuffer[1] = { 0.5, -1.0 };
    shapeBuffer[2] = { 0.5, -0.1 };
    shapeBuffer[3] = { -0.5, 0.4 };
    pGround = new Ground(shapeBuffer);
    pGround->CreateHullCollider(ip::Ground, reinterpret_cast<ip::math::Vector2*>(shapeBuffer), 4);
    pGround->CreateRigidBody(ip::Static, 0, 0, real(0.35), real(0.2), real(0.1), real(1.0));
    pGround->SetPosition(-6.0f, GROUND_Y);
    Engine::GetInstance().PhysWorld().AddPhysicsObject(pGround);
    AddObject(pGround, LayerType::Interactable);
    
    shapeBuffer[0] = { -0.5, -1.0 };
    shapeBuffer[1] = { 0.5, -1.0 };
    shapeBuffer[2] = { 0.5, 0.5 };
    shapeBuffer[3] = { -0.5, -0.1 };
    pGround = new Ground(shapeBuffer);
    pGround->CreateHullCollider(ip::Ground, reinterpret_cast<ip::math::Vector2*>(shapeBuffer), 4);
    pGround->CreateRigidBody(ip::Static, 0, 0, real(0.35), real(0.2), real(0.1), real(1.0));
    pGround->SetPosition(-5.0f, GROUND_Y);
    Engine::GetInstance().PhysWorld().AddPhysicsObject(pGround);
    AddObject(pGround, LayerType::Interactable);
    
    shapeBuffer[0] = { -0.5, -1.0 };
    shapeBuffer[1] = { 0.5, -1.0 };
    shapeBuffer[2] = { 0.5, 0.25 };
    shapeBuffer[3] = { -0.5, 0.5 };
    pGround = new Ground(shapeBuffer);
    pGround->CreateHullCollider(ip::Ground, reinterpret_cast<ip::math::Vector2*>(shapeBuffer), 4);
    pGround->CreateRigidBody(ip::Static, 0, 0, real(0.35), real(0.2), real(0.1), real(1.0));
    pGround->SetPosition(-4.0f, GROUND_Y);
    Engine::GetInstance().PhysWorld().AddPhysicsObject(pGround);
    AddObject(pGround, LayerType::Interactable);
    
    shapeBuffer[0] = { -0.5, -1.0 };
    shapeBuffer[1] = { 0.5, -1.0 };
    shapeBuffer[2] = { 0.5, 0.6 };
    shapeBuffer[3] = { -0.5, 0.25 };
    pGround = new Ground(shapeBuffer);
    pGround->CreateHullCollider(ip::Ground, reinterpret_cast<ip::math::Vector2*>(shapeBuffer), 4);
    pGround->CreateRigidBody(ip::Static, 0, 0, real(0.35), real(0.2), real(0.1), real(1.0));
    pGround->SetPosition(-3.0f, GROUND_Y);
    Engine::GetInstance().PhysWorld().AddPhysicsObject(pGround);
    AddObject(pGround, LayerType::Interactable);
    
    shapeBuffer[0] = { -0.5, -1.0 };
    shapeBuffer[1] = { 0.5, -1.0 };
    shapeBuffer[2] = { 0.5, 0.1 };
    shapeBuffer[3] = { -0.5, 0.6 };
    pGround = new Ground(shapeBuffer);
    pGround->CreateHullCollider(ip::Ground, reinterpret_cast<ip::math::Vector2*>(shapeBuffer), 4);
    pGround->CreateRigidBody(ip::Static, 0, 0, real(0.35), real(0.2), real(0.1), real(1.0));
    pGround->SetPosition(-2.0f, GROUND_Y);
    Engine::GetInstance().PhysWorld().AddPhysicsObject(pGround);
    AddObject(pGround, LayerType::Interactable);
    
    shapeBuffer[0] = { -0.5, -1.0 };
    shapeBuffer[1] = { 0.5, -1.0 };
    shapeBuffer[2] = { 0.5, -0.2 };
    shapeBuffer[3] = { -0.5, 0.1 };
    pGround = new Ground(shapeBuffer);
    pGround->CreateHullCollider(ip::Ground, reinterpret_cast<ip::math::Vector2*>(shapeBuffer), 4);
    pGround->CreateRigidBody(ip::Static, 0, 0, real(0.35), real(0.2), real(0.1), real(1.0));
    pGround->SetPosition(-1.0f, GROUND_Y);
    Engine::GetInstance().PhysWorld().AddPhysicsObject(pGround);
    AddObject(pGround, LayerType::Interactable);
    
    shapeBuffer[0] = { -0.5, -1.0 };
    shapeBuffer[1] = { 0.5, -1.0 };
    shapeBuffer[2] = { 0.5, 0.0 };
    shapeBuffer[3] = { -0.5, -0.2 };
    pGround = new Ground(shapeBuffer);
    pGround->CreateHullCollider(ip::Ground, reinterpret_cast<ip::math::Vector2*>(shapeBuffer), 4);
    pGround->CreateRigidBody(ip::Static, 0, 0, real(0.35), real(0.2), real(0.1), real(1.0));
    pGround->SetPosition(0.0f, GROUND_Y);
    Engine::GetInstance().PhysWorld().AddPhysicsObject(pGround);
    AddObject(pGround, LayerType::Interactable);
    
    shapeBuffer[0] = { -0.5, -1.0 };
    shapeBuffer[1] = { 0.5, -1.0 };
    shapeBuffer[2] = { 0.5, 0.25 };
    shapeBuffer[3] = { -0.5, 0.0 };
    pGround = new Ground(shapeBuffer);
    pGround->CreateHullCollider(ip::Ground, reinterpret_cast<ip::math::Vector2*>(shapeBuffer), 4);
    pGround->CreateRigidBody(ip::Static, 0, 0, real(0.35), real(0.2), real(0.1), real(1.0));
    pGround->SetPosition(1.0f, GROUND_Y);
    Engine::GetInstance().PhysWorld().AddPhysicsObject(pGround);
    AddObject(pGround, LayerType::Interactable);
    
    shapeBuffer[0] = { -0.5, -1.0 };
    shapeBuffer[1] = { 0.5, -1.0 };
    shapeBuffer[2] = { 0.5, 0.1 };
    shapeBuffer[3] = { -0.5, 0.25 };
    pGround = new Ground(shapeBuffer);
    pGround->CreateHullCollider(ip::Ground, reinterpret_cast<ip::math::Vector2*>(shapeBuffer), 4);
    pGround->CreateRigidBody(ip::Static, 0, 0, real(0.35), real(0.2), real(0.1), real(1.0));
    pGround->SetPosition(2.0f, GROUND_Y);
    Engine::GetInstance().PhysWorld().AddPhysicsObject(pGround);
    AddObject(pGround, LayerType::Interactable);
    
    shapeBuffer[0] = { -0.5, -1.0 };
    shapeBuffer[1] = { 0.5, -1.0 };
    shapeBuffer[2] = { 0.5, 0.2 };
    shapeBuffer[3] = { -0.5, 0.1 };
    pGround = new Ground(shapeBuffer);
    pGround->CreateHullCollider(ip::Ground, reinterpret_cast<ip::math::Vector2*>(shapeBuffer), 4);
    pGround->CreateRigidBody(ip::Static, 0, 0, real(0.35), real(0.2), real(0.1), real(1.0));
    pGround->SetPosition(3.0f, GROUND_Y);
    Engine::GetInstance().PhysWorld().AddPhysicsObject(pGround);
    AddObject(pGround, LayerType::Interactable);
    
    shapeBuffer[0] = { -0.5, -1.0 };
    shapeBuffer[1] = { 0.5, -1.0 };
    shapeBuffer[2] = { 0.5, 0.35 };
    shapeBuffer[3] = { -0.5, 0.2 };
    pGround = new Ground(shapeBuffer);
    pGround->CreateHullCollider(ip::Ground, reinterpret_cast<ip::math::Vector2*>(shapeBuffer), 4);
    pGround->CreateRigidBody(ip::Static, 0, 0, real(0.35), real(0.2), real(0.1), real(1.0));
    pGround->SetPosition(4.0f, GROUND_Y);
    Engine::GetInstance().PhysWorld().AddPhysicsObject(pGround);
    AddObject(pGround, LayerType::Interactable);
    
    shapeBuffer[0] = { -0.5, -1.0 };
    shapeBuffer[1] = { 0.5, -1.0 };
    shapeBuffer[2] = { 0.5, 0.55 };
    shapeBuffer[3] = { -0.5, 0.35 };
    pGround = new Ground(shapeBuffer);
    pGround->CreateHullCollider(ip::Ground, reinterpret_cast<ip::math::Vector2*>(shapeBuffer), 4);
    pGround->CreateRigidBody(ip::Static, 0, 0, real(0.35), real(0.2), real(0.1), real(1.0));
    pGround->SetPosition(5.0f, GROUND_Y);
    Engine::GetInstance().PhysWorld().AddPhysicsObject(pGround);
    AddObject(pGround, LayerType::Interactable);
    
    shapeBuffer[0] = { -0.5, -1.0 };
    shapeBuffer[1] = { 0.5, -1.0 };
    shapeBuffer[2] = { 0.5, 0.7 };
    shapeBuffer[3] = { -0.5, 0.55 };
    pGround = new Ground(shapeBuffer);
    pGround->CreateHullCollider(ip::Ground, reinterpret_cast<ip::math::Vector2*>(shapeBuffer), 4);
    pGround->CreateRigidBody(ip::Static, 0, 0, real(0.35), real(0.2), real(0.1), real(1.0));
    pGround->SetPosition(6.0f, GROUND_Y);
    Engine::GetInstance().PhysWorld().AddPhysicsObject(pGround);
    AddObject(pGround, LayerType::Interactable);
    
    shapeBuffer[0] = { -0.5, -1.0 };
    shapeBuffer[1] = { 0.5, -1.0 };
    shapeBuffer[2] = { 0.5, 0.7 };
    shapeBuffer[3] = { -0.5, 0.7 };
    pGround = new Ground(shapeBuffer);
    pGround->CreateHullCollider(ip::Ground, reinterpret_cast<ip::math::Vector2*>(shapeBuffer), 4);
    pGround->CreateRigidBody(ip::Static, 0, 0, real(0.35), real(0.2), real(0.1), real(1.0));
    pGround->SetPosition(7.0f, GROUND_Y);
    Engine::GetInstance().PhysWorld().AddPhysicsObject(pGround);
    AddObject(pGround, LayerType::Interactable);
    
    shapeBuffer[0] = { -0.5, -1.0 };
    shapeBuffer[1] = { 0.5, -1.0 };
    shapeBuffer[2] = { 0.5, 1.5 };
    shapeBuffer[3] = { -0.5, 0.7 };
    pGround = new Ground(shapeBuffer);
    pGround->CreateHullCollider(ip::Ground, reinterpret_cast<ip::math::Vector2*>(shapeBuffer), 4);
    pGround->CreateRigidBody(ip::Static, 0, 0, real(0.35), real(0.2), real(0.1), real(1.0));
    pGround->SetPosition(8.0f, GROUND_Y);
    Engine::GetInstance().PhysWorld().AddPhysicsObject(pGround);
    AddObject(pGround, LayerType::Interactable);

    shapeBuffer[0] = { -0.5, -1.0 };
    shapeBuffer[1] = { 0.5, -1.0 };
    shapeBuffer[2] = { 0.5, 3.0 };
    shapeBuffer[3] = { -0.5, 1.5 };
    pGround = new Ground(shapeBuffer);
    pGround->CreateHullCollider(ip::Ground, reinterpret_cast<ip::math::Vector2*>(shapeBuffer), 4);
    pGround->CreateRigidBody(ip::Static, 0, 0, real(0.35), real(0.2), real(0.1), real(1.0));
    pGround->SetPosition(9.0f, GROUND_Y);
    Engine::GetInstance().PhysWorld().AddPhysicsObject(pGround);
    AddObject(pGround, LayerType::Interactable);

    shapeBuffer[0] = { -0.5, -1.0 };
    shapeBuffer[1] = { 0.5, -1.0 };
    shapeBuffer[2] = { 0.5, 4.0 };
    shapeBuffer[3] = { -0.5, 3.0 };
    pGround = new Ground(shapeBuffer);
    pGround->CreateHullCollider(ip::Ground, reinterpret_cast<ip::math::Vector2*>(shapeBuffer), 4);
    pGround->CreateRigidBody(ip::Static, 0, 0, real(0.35), real(0.2), real(0.1), real(1.0));
    pGround->SetPosition(10.0f, GROUND_Y);
    Engine::GetInstance().PhysWorld().AddPhysicsObject(pGround);
    AddObject(pGround, LayerType::Interactable);
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
