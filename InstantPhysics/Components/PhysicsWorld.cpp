#include "PhysicsWorld.h"
#include "Common\Type.h"
#include "Common\IPParameter.h"
#include "Common\LayerCollisionMatrix.h"
#include "Components\IPDebug.h"
#include "Components\PhysicsObject.h"
#include "Components\CircleCollider.h"
#include "Components\CapsuleCollider.h"
#include "Components\HullCollider.h"
#include "Components\Manifold.h"
#include "Components\Simplex.h"
#include "Components\Query.h"
#include <process.h>
#include <iostream>
#include <fstream>
#include <strsafe.h>

using namespace ip;

VOID NTAPI DoIPMultiThreadJob(ULONG_PTR pJob)
{
    reinterpret_cast<MTJob*>(pJob)->Do();
}

#ifdef _DEBUG
std::list<PhysicsWorld::ContactPoint> PhysicsWorld::m_cpList;
#endif // _DEBUG

PhysicsWorld::CDF PhysicsWorld::cdft[COLLIDER_TYPE_COUNT][COLLIDER_TYPE_COUNT];

//              Circle      Capsule     Hull        Line
// 
// Circle       Cir_Cir     Cir_Cap     Cir_Hul     Cir_Lin
// 
// Capsule      Cap_Cir     Cap_Cap     Cap_Hul     Cap_Lin
// 
// Hull         Hul_Cir     Hul_Cap     Hul_Hul     Hul_Lin
// 
// Line         Lin_Cir     Lin_Cap     Lin_Hul     Lin_Lin

#define _cdft PhysicsWorld::cdft

#define Cir_Cir PhysicsWorld::CheckCirclevsCircleCollision
#define Cir_Cap PhysicsWorld::CheckCirclevsCapsuleCollision
#define Cir_Hul PhysicsWorld::CheckCirclevsHullCollision
#define Cir_Lin /*PhysicsWorld::CheckCirclevsLineCollision*/ nullptr
#define Cap_Cir PhysicsWorld::CheckCapsulevsCircleCollision
#define Cap_Cap PhysicsWorld::CheckCapsulevsCapsuleCollision
#define Cap_Hul PhysicsWorld::CheckCapsulevsHullCollision
#define Cap_Lin /*PhysicsWorld::CheckCapsulevsLineCollision*/ nullptr
#define Hul_Cir PhysicsWorld::CheckHullvsCircleCollision
#define Hul_Cap PhysicsWorld::CheckHullvsCapsuleCollision
#define Hul_Hul PhysicsWorld::CheckHullvsHullCollision
#define Hul_Lin /*PhysicsWorld::CheckHullvsLineCollision*/ nullptr
#define Lin_Cir /*PhysicsWorld::CheckLinevsCircleCollision*/ nullptr
#define Lin_Cap /*PhysicsWorld::CheckLinevsCapsuleCollision*/ nullptr
#define Lin_Hul /*PhysicsWorld::CheckLinevsHullCollision*/ nullptr
#define Lin_Lin /*PhysicsWorld::CheckLinevsLineCollision*/ nullptr

PhysicsWorld::PhysicsWorld()
    : m_gravity(real(0.0), real(-9.8 * 0.5))
    , m_threadCount(0)
    , m_fc(0)
    , m_terminateMTFlag(false)
    , m_mtMode(false)
    , m_MTUpdatePhysObjFinishEvent(nullptr)
{
    ZeroMemory(m_threads, sizeof(m_threads));
    _cdft[0][0] = Cir_Cir;  _cdft[0][1] = Cir_Cap;  _cdft[0][2] = Cir_Hul;  _cdft[0][3] = Cir_Lin;
    _cdft[1][0] = Cap_Cir;  _cdft[1][1] = Cap_Cap;  _cdft[1][2] = Cap_Hul;  _cdft[1][3] = Cap_Lin;
    _cdft[2][0] = Hul_Cir;  _cdft[2][1] = Hul_Cap;  _cdft[2][2] = Hul_Hul;  _cdft[2][3] = Hul_Lin;
    _cdft[3][0] = Lin_Cir;  _cdft[3][1] = Lin_Cap;  _cdft[3][2] = Lin_Hul;  _cdft[3][3] = Lin_Lin;
}

PhysicsWorld::~PhysicsWorld()
{
    m_AABBArray.Destroy();
    m_activeSet.Destroy();

    if (m_MTUpdatePhysObjFinishEvent)
    {
        CloseHandle(m_MTUpdatePhysObjFinishEvent);
        m_MTUpdatePhysObjFinishEvent = nullptr;
    }

    m_terminateMTFlag = true;
    // MemoryBarrier();

    MTDoNothing dummy;
    for (long i = 0; i < m_threadCount; i++) QueueUserAPC(DoIPMultiThreadJob, m_threads[i], reinterpret_cast<ULONG_PTR>(&dummy));
    WaitForMultipleObjects(m_threadCount, m_threads, TRUE, INFINITE);
    for (long i = 0; i < m_threadCount; i++) { CloseHandle(m_threads[i]); m_threads[i] = nullptr; }
}

bool PhysicsWorld::Initialize()
{
    char gleBuf[256];
    char buf[256];
    SYSTEM_INFO si;
    GetSystemInfo(&si);

    DWORD nop = si.dwNumberOfProcessors;
    DWORD ps = si.dwPageSize;
    DWORD ag = si.dwAllocationGranularity;

    std::ofstream fLog;
    Safe_ofstream safefLog(&fLog);
    fLog.open("IPSysInfo.txt", std::ios::out);
    if (fLog.fail())
        return false;

    StringCbPrintfA(buf, sizeof(buf), "Number of (Logical)Processors: %u\n", nop);
    fLog.write(buf, strlen(buf));
    StringCbPrintfA(buf, sizeof(buf), "VM Page Size: %uKB\n", ps >> 10);
    fLog.write(buf, strlen(buf));
    StringCbPrintfA(buf, sizeof(buf), "VM Allocation Granularity: %uKB\n", ag >> 10);
    fLog.write(buf, strlen(buf));

    m_threadCount = nop > parameter::MAX_THREADS ? parameter::MAX_THREADS : nop;

    if (m_threadCount <= 2)
    {
        m_mtMode = false;
        return true;
    }


    // 아래부분은 나중에 IOCP로 대체
    m_terminateMTFlag = false;
    m_mtMode = true;

    m_MTUpdatePhysObjFinishEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (m_MTUpdatePhysObjFinishEvent == NULL)
    {
        DWORD code = GetLastError();
        IPDebug::GetDetailError(code, gleBuf, sizeof(gleBuf));
        StringCbPrintfA(buf, sizeof(buf), "CreateEvent() failed. (%u) %s\n", code, gleBuf);
        fLog.write(buf, strlen(buf));
        return false;
    }

    void* threadHandleBuf[parameter::MAX_THREADS];
    ZeroMemory(threadHandleBuf, sizeof(threadHandleBuf));

    for (int i = 0; i < m_threadCount; i++)
    {
        void* handle = reinterpret_cast<void*>(_beginthreadex(NULL, 0, PhysicsWorld::WorkerThreadEntry, this, 0, nullptr));
        if (handle == nullptr)
            break;
        threadHandleBuf[i] = handle;
    }

    for (int i = 0; i < m_threadCount; i++)
    {
        if (threadHandleBuf[i] == NULL)
        {
            DWORD code = GetLastError();
            IPDebug::GetDetailError(code, gleBuf, sizeof(gleBuf));
            StringCbPrintfA(buf, sizeof(buf), "_beginthreadex() failed. (%u) %s\n", code, gleBuf);
            fLog.write(buf, strlen(buf));
            StringCbPrintfA(buf, sizeof(buf), "Multi-thread mode turned off.\n");
            fLog.write(buf, strlen(buf));
            
            m_mtMode = false;
            m_terminateMTFlag = true;
            // MemoryBarrier();

            for (int j = 0; j < i; j++)
            {
                QueueUserAPC(DoIPMultiThreadJob, threadHandleBuf[j], reinterpret_cast<ULONG_PTR>(&m_MTDoNothingJob));
                CloseHandle(threadHandleBuf[j]);
            }
            return true;
        }
    }

    for (int i = 0; i < m_threadCount; i++)
    {
        m_threads[i] = threadHandleBuf[i];
    }

    return true;
}

void PhysicsWorld::Update()
{
    // multiple position constraint solve (-> 충돌 콜백 여러번 호출 방지하기 전까지 보류)
    // for (int i = 0; i < parameter::SUB_STEP_COUNT; i++)
    // {
    //     UpdatePhysicsObject();
    //     BroadPhase();
    //     NarrowPhase();
    // }

    UpdatePhysicsObject();
    BroadPhase();
    NarrowPhase();
}

void PhysicsWorld::AddPhysicsObject(PhysicsObject* pPhysObj)
{
    pPhysObj->m_pPhysWorld = this;
    pPhysObj->m_pCollider->UpdateCollider();
    m_AABBArray.Add(pPhysObj);
    m_physObjs.push_back(pPhysObj);
}

void PhysicsWorld::RemovePhysicsObject(PhysicsObject* pPhysObj)
{
    // 시뮬레이션 리스트에서 제거
    // PhysicsObject가 스스로 할당한 자원은 여기서 해제 X
    // PhysicsObject 클래스에서 해제하게끔

    pPhysObj->m_isDead = true;
    // RemoveAABBNode(pCollider);       // 노드 리스트에서 제거 (메모리 해제 X)
    // RemoveFromActiveSet(pCollider);  // activeIndex -1이면 생략
    // RemoveFromColliderArray(pCollider);
    
    // RemoveCollider, AABBNode X <- PhysicsObject 자기 자신이 해제 (할당을 해당 클래스에서 하므로)
    pPhysObj->m_pPhysWorld = nullptr;
}

void PhysicsWorld::UpdatePhysicsObject()
{
    size_t size = m_physObjs.size();

    if (m_mtMode && (size > m_threadCount * parameter::PHYSOBJ_MULTITHREAD_UPDATE_LOWER_LIMIT))
    {
        UpdatePhysicsObjectMultiThread(&m_physObjs[0], size);
    }
    else
    {
        UpdatePhysicsObjectSingleThread(&m_physObjs[0], size);
    }
}

void PhysicsWorld::BroadPhase()
{
    m_AABBArray.Sort();

    int listSize = m_AABBArray.GetCount();
    AABBNode** list = m_AABBArray.Get();
    Collider** pActiveList = m_activeSet.Get();

    for (int i = 0; i < listSize; i++)
    {
        AABBNode* cursor = list[i];
        Collider* cursorCollider = cursor->pCollider;

        if (cursor->type == Type::MIN)
        {
            int activeCount = m_activeSet.GetCount();

            for (int j = 0; j < activeCount; j++)
            {
                if (pActiveList[j]->m_AABB.minY > cursorCollider->m_AABB.maxY || pActiveList[j]->m_AABB.maxY < cursorCollider->m_AABB.minY)
                    continue;

                if (!LayerCollisionMatrix::s_LCM[pActiveList[j]->m_layer][cursorCollider->m_layer])
                    continue;

                // Static <-> Static, Static <-> Kinematic, Kinematic <-> Kinematic은 처리 X
                if (pActiveList[j]->m_pPhysObj->m_rigidBody.m_type != Dynamic && cursorCollider->m_pPhysObj->m_rigidBody.m_type != Dynamic)
                    continue;

                m_npList.emplace_back(pActiveList[j], cursorCollider);
            }
            m_activeSet.Add(cursorCollider);
        }
        else    // cursor->type == Type::MAX
        {
            m_activeSet.Remove(cursor->pCollider);
        }
    }
}

void PhysicsWorld::NarrowPhase()
{
    Manifold m;

    auto end = m_npList.end();
    for (auto i = m_npList.begin(); i != end; ++i)
    {
        Collider& colliderA = *(*i).first;
        Collider& colliderB = *(*i).second;

        const bool result = cdft[colliderA.m_type][colliderB.m_type](m, colliderA, colliderB);
        if (!result)
            continue;

#ifdef _DEBUG
        for (uint32_t i = 0; i < m.contactCount; i++)
        {
            m_cpList.push_back(ContactPoint(m.contact[i].x, m.contact[i].y));
        }
#endif // DEBUG

        ResolveCollision(m);
    } 

    m_npList.clear();
}

void PhysicsWorld::ResolveCollision(const Manifold& manifold)
{
    const Collider& colA = *manifold.pReference;
    const Collider& colB = *manifold.pIncident;
    RigidBody& rbA = colA.m_pPhysObj->m_rigidBody;
    RigidBody& rbB = colB.m_pPhysObj->m_rigidBody;

    real e_plus_1 = min(rbA.m_bounce, rbB.m_bounce) + real(1.0);
    math::Vector2 impulse[2];
    math::Vector2 (&frictionImpulse)[2] = impulse;
    real jBuffer[2];
    bool impulseValidFlag[2] = { true, true };
    bool(&frictionValidFlag)[2] = impulseValidFlag;
    math::Vector2 raBuffer[2];
    math::Vector2 rbBuffer[2];
    math::Vector2 ra;
    math::Vector2 rb;
    math::Vector2 raPerp;
    math::Vector2 rbPerp;
    math::Vector2 angularVelra;
    math::Vector2 angularVelrb;
    math::Vector2 relativeVel;

    uint32_t contactCount = manifold.contactCount;
    const math::Vector2 collisionNormal = manifold.collisionNormal;
    uint32_t i;
    for (i = 0; i < contactCount; i++)
    {
        raBuffer[i] = ra = manifold.contact[i] - colA.m_pPhysObj->m_position;
        rbBuffer[i] = rb = manifold.contact[i] - colB.m_pPhysObj->m_position;

        raPerp = math::Vector2(-ra.y, ra.x);
        rbPerp = math::Vector2(-rb.y, rb.x);

        angularVelra = raPerp * rbA.m_angularVelocity;
        angularVelrb = rbPerp * rbB.m_angularVelocity;

        relativeVel = (rbB.m_linearVelocity + angularVelrb) - (rbA.m_linearVelocity + angularVelra);

        // 접촉점에서의 상대 속도 크기
        real relativeVelMag = math::Vector2::Dot(relativeVel, collisionNormal);

        if (relativeVelMag > real(0.0))
        {
            impulseValidFlag[i] = false;
            continue;
        }

        real raPerpDotNormal = math::Vector2::Dot(raPerp, collisionNormal);
        real rbPerpDotNormal = math::Vector2::Dot(rbPerp, collisionNormal);

        real denominator = rbA.m_invMass + rbB.m_invMass +
            rbA.m_invInertia * (raPerpDotNormal * raPerpDotNormal) + rbB.m_invInertia * (rbPerpDotNormal * rbPerpDotNormal);

        real j = -e_plus_1 * relativeVelMag / denominator;
        jBuffer[i] = j;
        
        impulse[i] = j * collisionNormal;
    }

    for (i = 0; i < contactCount; i++)
    {
        if (impulseValidFlag[i] == true)
        {
            rbA.m_linearVelocity -= impulse[i] * rbA.m_invMass;
            rbA.m_angularVelocity -= math::Vector2::Cross(raBuffer[i], impulse[i]) * rbA.m_invInertia;
            rbB.m_linearVelocity += impulse[i] * rbB.m_invMass;
            rbB.m_angularVelocity += math::Vector2::Cross(rbBuffer[i], impulse[i]) * rbB.m_invInertia;
        }
    }

    // Friction
    real sf = (rbA.m_staticFriction + rbB.m_staticFriction) * real(0.5);
    real df = (rbA.m_dynamicFriction + rbB.m_dynamicFriction) * real(0.5);
    // real sf = std::sqrt(rbA.m_staticFriction * rbA.m_staticFriction + rbB.m_staticFriction * rbB.m_staticFriction);
    // real df = std::sqrt(rbA.m_dynamicFriction * rbA.m_dynamicFriction + rbB.m_dynamicFriction * rbB.m_dynamicFriction);
    for (i = 0; i < contactCount; i++)
    {
        if (impulseValidFlag[i] == false)
            continue;       // j 값이 유효하지 않으므로 계산 불가능
    
        raBuffer[i] = ra = manifold.contact[i] - colA.m_pPhysObj->m_position;
        rbBuffer[i] = rb = manifold.contact[i] - colB.m_pPhysObj->m_position;
    
        raPerp = math::Vector2(-ra.y, ra.x);
        rbPerp = math::Vector2(-rb.y, rb.x);
    
        angularVelra = raPerp * rbA.m_angularVelocity;
        angularVelrb = rbPerp * rbB.m_angularVelocity;
    
        relativeVel = (rbB.m_linearVelocity + angularVelrb) - (rbA.m_linearVelocity + angularVelra);
    
        math::Vector2 tangent = relativeVel - (math::Vector2::Dot(relativeVel, collisionNormal) * collisionNormal);
        real squaredLength = tangent.SquaredLength();
        if (squaredLength < std::numeric_limits<real>::epsilon())
        {
            frictionValidFlag[i] = false;
            continue;
        }
        tangent /= std::sqrt(squaredLength);

        real raPerpDotTangent = math::Vector2::Dot(raPerp, tangent);
        real rbPerpDotTangent = math::Vector2::Dot(rbPerp, tangent);
    
        real denominator = rbA.m_invMass + rbB.m_invMass +
            rbA.m_invInertia * (raPerpDotTangent * raPerpDotTangent) + rbB.m_invInertia * (rbPerpDotTangent * rbPerpDotTangent);
    
        real jt = -math::Vector2::Dot(relativeVel, tangent) / denominator;
    
        if (std::abs(jt) <= jBuffer[i] * sf)
            frictionImpulse[i] = jt * tangent;
        else
            frictionImpulse[i] = -jBuffer[i] * tangent * df;
    }
    
    for (i = 0; i < contactCount; i++)
    {
        if (frictionValidFlag[i] == true)
        {
            rbA.m_linearVelocity -= frictionImpulse[i] * rbA.m_invMass;
            rbA.m_angularVelocity -= math::Vector2::Cross(raBuffer[i], frictionImpulse[i]) * rbA.m_invInertia;
            rbB.m_linearVelocity += frictionImpulse[i] * rbB.m_invMass;
            rbB.m_angularVelocity += math::Vector2::Cross(rbBuffer[i], frictionImpulse[i]) * rbB.m_invInertia;
        }
    }

    // Corrects Floating Point Error 1
    // const real percent = real(0.8);     // usually 20% to 80%
    // const real slop = real(0.01);       // usually 0.01 to 0.1
    // math::Vector2 correctionDir = manifold.collisionNormal *
    //     (max(manifold.penetrationDepth - slop, real(0.0)) * percent / (rbA.m_invMass + rbB.m_invMass));
    // colA.m_pPhysObj->m_position -= rbA.m_invMass * correctionDir;
    // colB.m_pPhysObj->m_position += rbB.m_invMass * correctionDir;

    // Corrects Floating Point Error 2
    math::Vector2 correctionVector = manifold.collisionNormal * manifold.penetrationDepth;
    real totalMass = rbA.m_invMass + rbB.m_invMass;
    colA.m_pPhysObj->m_position -= correctionVector * (rbA.m_invMass / totalMass);
    colB.m_pPhysObj->m_position += correctionVector * (rbB.m_invMass / totalMass);
}

void PhysicsWorld::Integrate(PhysicsObject* pPhysObj)
{
    RigidBody& rb = pPhysObj->m_rigidBody;
    rb.m_force += m_gravity * rb.m_mass * pPhysObj->m_rigidBody.m_gravityScale;
    rb.m_linearVelocity += rb.m_force * rb.m_invMass * parameter::PHYSICS_UPDATE_INTERVAL / parameter::SUB_STEP_COUNT;
    rb.m_angularVelocity += rb.m_invInertia * rb.m_torque * parameter::PHYSICS_UPDATE_INTERVAL / parameter::SUB_STEP_COUNT;
    pPhysObj->m_position += rb.m_linearVelocity * parameter::PHYSICS_UPDATE_INTERVAL / parameter::SUB_STEP_COUNT;
    pPhysObj->Rotate(rb.m_angularVelocity * parameter::PHYSICS_UPDATE_INTERVAL / parameter::SUB_STEP_COUNT);

    rb.m_force = math::Vector2::zero;
    rb.m_torque = real(0.0);
}

void PhysicsWorld::KinematicUpdate(PhysicsObject* pPhysObj)
{
    RigidBody& rb = pPhysObj->m_rigidBody;
    pPhysObj->m_position += rb.m_linearVelocity * parameter::PHYSICS_UPDATE_INTERVAL / parameter::SUB_STEP_COUNT;
    pPhysObj->Rotate(rb.m_angularVelocity * parameter::PHYSICS_UPDATE_INTERVAL / parameter::SUB_STEP_COUNT);
}

void PhysicsWorld::UpdatePhysicsObjectMultiThread(PhysicsObject** ppBeginAddr, size_t size)
{
    // ######!!
    m_fc = 0;
    // ######!!

    int i;
    size_t stride = size / m_threadCount;
    size_t remainder = size % m_threadCount;
    // Setting table
    PhysicsObject** ppBaseAddr = ppBeginAddr;
    for (i = 0; i < m_threadCount; i++)
    {
        m_MTUpdatePhysObjJobArr[i].m_pPhysWorld = this;
        m_MTUpdatePhysObjJobArr[i].m_pBeginAddr = ppBaseAddr;
        m_MTUpdatePhysObjJobArr[i].m_objCount = stride;
        ppBaseAddr += stride;
    }
    m_MTUpdatePhysObjJobArr[m_threadCount - 1].m_objCount += remainder;

    for (i = m_threadCount - 1; i >= 0; i--)
    {
        QueueUserAPC(DoIPMultiThreadJob, m_threads[i], reinterpret_cast<ULONG_PTR>(&m_MTUpdatePhysObjJobArr[i]));
    }

    WaitForSingleObject(m_MTUpdatePhysObjFinishEvent, INFINITE);
}

void PhysicsWorld::UpdatePhysicsObjectSingleThread(PhysicsObject** ppBeginAddr, size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        PhysicsObject* pPhysObj = *(ppBeginAddr + i);
        switch (pPhysObj->m_rigidBody.m_type)
        {
        case Unknown:
        case Static:
            continue;
        case Kinematic:
            KinematicUpdate(pPhysObj);
            break;
        case Dynamic:
            Integrate(pPhysObj);
            break;
        }
        pPhysObj->m_pCollider->UpdateCollider();
    }
}

bool PhysicsWorld::CheckCirclevsCircleCollision(Manifold& manifold, const Collider& circleColliderA, const Collider& circleColliderB)
{
    const CircleCollider& circleA = reinterpret_cast<const CircleCollider&>(circleColliderA);
    const CircleCollider& circleB = reinterpret_cast<const CircleCollider&>(circleColliderB);


    math::Vector2 v = circleColliderB.m_pPhysObj->m_position - circleColliderA.m_pPhysObj->m_position;
    real squaredLength = v.SquaredLength();
    real radiusSum = circleA.m_radius + circleB.m_radius;

    if (squaredLength > radiusSum * radiusSum)
        return false;

    if (squaredLength == real(0.0))
    {
        manifold.pReference = const_cast<Collider*>(&circleColliderA);
        manifold.pIncident = const_cast<Collider*>(&circleColliderB);
        manifold.collisionNormal = math::Vector2(real(0.0), real(1.0));
        manifold.penetrationDepth = radiusSum;
        manifold.contact[0] = circleColliderA.m_pPhysObj->m_position + (math::Vector2(real(0.0), real(1.0)) * (circleA.m_radius - (radiusSum * real(0.5))));
        manifold.contactCount = 1;
        return true;
    }
    else
    {
        real distance = std::sqrt(squaredLength);
        v /= distance;      // normalize vector v

        manifold.pReference = const_cast<Collider*>(&circleColliderA);
        manifold.pIncident = const_cast<Collider*>(&circleColliderB);
        manifold.collisionNormal = v;
        manifold.penetrationDepth = radiusSum - distance;
        manifold.contact[0] = circleColliderA.m_pPhysObj->m_position + (v * (circleA.m_radius - (manifold.penetrationDepth * real(0.5))));
        manifold.contactCount = 1;
        return true;
    }
}

void PhysicsWorld::FaceQueryHullvsCircle(Query& query, const HullCollider& ref, const CircleCollider& inc)
{
    math::Vector2 edge;
    math::Vector2 faceNormal;
    real bestDistance = std::numeric_limits<real>::lowest();
    math::Vector2 bestFaceNormal;

    for (uint16_t faceIndex = 0; faceIndex < ref.m_vertexCount; faceIndex++)
    {
        uint16_t faceIndex2 = faceIndex + 1;
        if (faceIndex2 >= ref.m_vertexCount)
            faceIndex2 = 0;

        math::Vector2 v2(ref.m_pWorldX[faceIndex2], ref.m_pWorldY[faceIndex2]);
        math::Vector2 v1(ref.m_pWorldX[faceIndex], ref.m_pWorldY[faceIndex]);

        edge = v2 - v1;
        faceNormal = math::Vector2::Normalized(math::Vector2(edge.y, -edge.x));        // right normal

        // 원의 중심 - 엣지 한 점까지 내적 + 원 반지름 (SP 거리(음수)) (원의 중심이 무조건 Hull 내부 or 경계에 있으므로 d가 최대 0.0이다.)
        real d = math::Vector2::Dot(inc.m_pPhysObj->m_position - v1, faceNormal);

        if (d > bestDistance)
        {
            bestDistance = d;
            bestFaceNormal = faceNormal;
        }
    }
    bestDistance -= inc.m_radius;   // d 구할 때마다 안뺐으므로

    // =======================================================================================
    // query.axisExist = false;        // FaceQueryHullvsCircle 호출되는 경우는 무조건 false이다.
    // query.refFaceIndex = ;           // 클리핑 안하므로 불필요
    // query.incSupportIndex = ;        // 클리핑 안하므로 불필요
    query.bestDistance = bestDistance;
    query.refFaceNormal = bestFaceNormal;
}

bool PhysicsWorld::CheckCapsulevsCircleCollision(Manifold& manifold, const Collider& capsuleCollider, const Collider& circleCollider)
{
    return CheckCirclevsCapsuleCollision(manifold, circleCollider, capsuleCollider);
}

bool PhysicsWorld::CheckCirclevsCapsuleCollision(Manifold& manifold, const Collider& circleCollider, const Collider& capsuleCollider)
{
    math::Vector2 capsuleP0 = reinterpret_cast<const CapsuleCollider&>(capsuleCollider).m_worldVertices[0] - circleCollider.m_pPhysObj->m_position;
    math::Vector2 capsuleP1 = reinterpret_cast<const CapsuleCollider&>(capsuleCollider).m_worldVertices[1] - circleCollider.m_pPhysObj->m_position;

    math::Vector2 v = GetClosestPointOnLineSegementFromOrigin(capsuleP0, capsuleP1);
    real squaredLength = v.SquaredLength();

    real radiusSum = reinterpret_cast<const CircleCollider&>(circleCollider).m_radius
        + reinterpret_cast<const CapsuleCollider&>(capsuleCollider).m_radius;

    if (squaredLength > radiusSum * radiusSum)
        return false;

    real distance = std::sqrt(squaredLength);
    v /= distance;      // normalize vector v

    manifold.pReference = const_cast<Collider*>(&circleCollider);
    manifold.pIncident = const_cast<Collider*>(&capsuleCollider);

    if (distance != real(0.0))
        manifold.collisionNormal = v;
    else
        manifold.collisionNormal = math::Vector2(real(0.0), real(1.0));       // collision resolution constant vector

    manifold.penetrationDepth = radiusSum - distance;       // = -(distance - radiusSum)
    manifold.contact[0] = circleCollider.m_pPhysObj->m_position + (v * (reinterpret_cast<const CircleCollider&>(circleCollider).m_radius - (manifold.penetrationDepth * real(0.5))));
    manifold.contactCount = 1;

    return true;
}

bool PhysicsWorld::CheckCapsulevsCapsuleCollision(Manifold& manifold, const Collider& capsuleColliderA, const Collider& capsuleColliderB)
{
    return false;
}

bool PhysicsWorld::CheckCapsulevsHullCollision(Manifold& manifold, const Collider& capsuleCollider, const Collider& hullCollider)
{
    return CheckHullvsCapsuleCollision(manifold, hullCollider, capsuleCollider);
}

bool PhysicsWorld::CheckHullvsCapsuleCollision(Manifold& manifold, const Collider& hullCollider, const Collider& capsuleCollider)
{
    return false;
}

bool PhysicsWorld::CheckHullvsHullCollision(Manifold& manifold, const Collider& hullColliderA, const Collider& hullColliderB)
{
    Query queryA_B;     // HullA->HullB Query
    Query queryB_A;     // HullB->HullA Query

    // -------------------------------------------
    FaceQueryHullvsHull(queryA_B, reinterpret_cast<const HullCollider&>(hullColliderA), reinterpret_cast<const HullCollider&>(hullColliderB));
    if (queryA_B.axisExist)                     //
        return false;                           //
    // -------------------------------------------

    // -------------------------------------------
    FaceQueryHullvsHull(queryB_A, reinterpret_cast<const HullCollider&>(hullColliderB), reinterpret_cast<const HullCollider&>(hullColliderA));
    if (queryB_A.axisExist)                     //
        return false;                           //
    // -------------------------------------------

    if (queryA_B.bestDistance > queryB_A.bestDistance)
    {
        // ref = boxA
        manifold.pReference = const_cast<Collider*>(&hullColliderA);
        manifold.pIncident = const_cast<Collider*>(&hullColliderB);
        manifold.collisionNormal = queryA_B.refFaceNormal;
        manifold.penetrationDepth = -queryA_B.bestDistance;
        ComputeHullvsHullContactPoint(manifold, queryA_B.refFaceIndex, queryA_B.incSupportIndex);
    }
    else
    {
        // ref = boxB
        manifold.pReference = const_cast<Collider*>(&hullColliderB);
        manifold.pIncident = const_cast<Collider*>(&hullColliderA);
        manifold.collisionNormal = queryB_A.refFaceNormal;
        manifold.penetrationDepth = -queryB_A.bestDistance;
        ComputeHullvsHullContactPoint(manifold, queryB_A.refFaceIndex, queryB_A.incSupportIndex);
    }

    return true;
}

bool PhysicsWorld::CheckCirclevsHullCollision(Manifold& manifold, const Collider& circleCollider, const Collider& hullCollider)
{
    return CheckHullvsCircleCollision(manifold, hullCollider, circleCollider);
}

const math::Vector2 PhysicsWorld::GetClosestPointOnLineSegementFromOrigin(const math::Vector2& begin, const math::Vector2& end)
{
    real l = (end - begin).Length();
    math::Vector2 n = (end - begin) / l;
    real v = math::Vector2::Dot(-begin, n) / l;
    real u = math::Vector2::Dot(end, n) / l;

    if (u <= real(0.0))
    {
        return end;
    }
    else if (v <= real(0.0))
    {
        return begin;
    }
    else
    {
        return math::Vector2(u * begin + v * end);
    }
}

void PhysicsWorld::GetClosestVectorBetweenCircleCenterAndHull(const math::Vector2 circleCenter, const HullCollider& hull, math::Vector2& v, real& dist)
{
    const math::Vector2 dir = circleCenter - hull.m_pPhysObj->m_position;
    const uint16_t hullSupportIndex = hull.SupportIndex(dir);
    const uint16_t prevIndex = hull.GetPrevVertexIndex(hullSupportIndex);
    const uint16_t nextIndex = hull.GetNextVertexIndex(hullSupportIndex);

    math::Vector2 hullSupportPoint(hull.m_pWorldX[hullSupportIndex], hull.m_pWorldY[hullSupportIndex]);
    math::Vector2 hullPrevVertex(hull.m_pWorldX[prevIndex], hull.m_pWorldY[prevIndex]);
    math::Vector2 hullNextVertex(hull.m_pWorldX[nextIndex], hull.m_pWorldY[nextIndex]);

    math::Vector2 v1 = GetClosestPointOnLineSegementFromOrigin(hullSupportPoint - circleCenter, hullPrevVertex - circleCenter);
    math::Vector2 v2 = GetClosestPointOnLineSegementFromOrigin(hullNextVertex - circleCenter, hullSupportPoint - circleCenter);

    real v1LengthSquared = v1.SquaredLength();
    real v2LengthSquared = v2.SquaredLength();
    if (v1LengthSquared < v2LengthSquared)
    {
        v = v1;
        dist = std::sqrt(v1LengthSquared);
    }
    else
    {
        v = v2;
        dist = std::sqrt(v2LengthSquared);
    }
}

bool PhysicsWorld::CheckHullvsCircleCollision(Manifold& manifold, const Collider& hullCollider, const Collider& circleCollider)
{
    Simplex simplex;

    simplex.Initialize(&hullCollider, &circleCollider);
    Simplex::State state;
    
    while ((state = simplex.EvolveSimplex()) == Simplex::State::Evolving);
    
    if (state == Simplex::State::NotIntersecting)
    {
        math::Vector2 cpv;      // closest point vector in the simplex
        real distance;
        GetClosestVectorBetweenCircleCenterAndHull(circleCollider.m_pPhysObj->m_position, reinterpret_cast<const HullCollider&>(hullCollider), cpv, distance);
    
        real r = reinterpret_cast<const CircleCollider&>(circleCollider).m_radius;
    
        if (distance <= r)
        {
            manifold.pReference = const_cast<Collider*>(&hullCollider);
            manifold.pIncident = const_cast<Collider*>(&circleCollider);
            manifold.collisionNormal = -cpv / distance;
            manifold.penetrationDepth = r - distance;
    
            // CirclevsHull 충돌은 클리핑 필요 없으므로 Contact point 바로 계산
            manifold.contact[0] = circleCollider.m_pPhysObj->m_position + cpv;
            manifold.contactCount = 1;
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        // state == Simplex::State::Intersecting
    
        Query query;
        FaceQueryHullvsCircle(query, reinterpret_cast<const HullCollider&>(hullCollider), reinterpret_cast<const CircleCollider&>(circleCollider));
    
        manifold.pReference = const_cast<Collider*>(&hullCollider);
        manifold.pIncident = const_cast<Collider*>(&circleCollider);
        manifold.collisionNormal = query.refFaceNormal;
        manifold.penetrationDepth = -query.bestDistance;
    
        // CirclevsHull 충돌은 클리핑 필요 없으므로 Contact point 바로 계산
        manifold.contact[0] = circleCollider.m_pPhysObj->m_position - (query.refFaceNormal * (query.bestDistance + reinterpret_cast<const CircleCollider&>(circleCollider).m_radius));   // 반대 방향으로 가야하므로 빼야함
        manifold.contactCount = 1;
    
        return true;
    }

    return false;
}

void PhysicsWorld::FaceQueryHullvsHull(Query& query, const HullCollider& ref, const HullCollider& inc)
{
    math::Vector2 bestFaceNormal;
    uint16_t bestFaceIndex;                    // face [i] = vertex[(i + 1) % 4] - vertex[i]
    uint16_t incBestSupportIndex;

    real bestDistance = std::numeric_limits<real>::lowest();

    uint16_t hullVertexCount = ref.m_vertexCount;
    for (uint16_t i = 0; i < hullVertexCount; i++)
    {
        math::Vector2 refNormal = ref.GetFaceNormal(i);
        uint16_t incSupportIndex = inc.SupportIndex(-refNormal);

        math::Vector2 incSupport(inc.m_pWorldX[incSupportIndex], inc.m_pWorldY[incSupportIndex]);

        real d = math::Vector2::Dot(incSupport - math::Vector2(ref.m_pWorldX[i], ref.m_pWorldY[i]), refNormal);
        if (d > real(0.0))
        {
            query.axisExist = true;
            return;
        }

        if (d > bestDistance)
        {
            bestFaceNormal = refNormal;
            bestDistance = d;
            bestFaceIndex = i;
            incBestSupportIndex = incSupportIndex;
        }
    }

    query.axisExist = false;
    query.refFaceIndex = bestFaceIndex;
    query.incSupportIndex = incBestSupportIndex;
    query.bestDistance = bestDistance;
    query.refFaceNormal = bestFaceNormal;
}

void PhysicsWorld::ComputeHullvsHullContactPoint(Manifold& manifold, uint16_t refFaceIndex, uint16_t incVertexIndex)
{
    const HullCollider& ref = reinterpret_cast<const HullCollider&>(*manifold.pReference);
    const HullCollider& inc = reinterpret_cast<const HullCollider&>(*manifold.pIncident);

    // n1, n2 (incVertex를 포함하는 두 면 각각의 법선 벡터)

    uint16_t nPrevIndex = inc.GetPrevVertexIndex(incVertexIndex);
    const math::Vector2 nPrev = inc.GetFaceNormal(nPrevIndex);       // vertex index는 face index로도 쓰일 수 있음
    const math::Vector2 nNext = inc.GetFaceNormal(incVertexIndex);

    uint16_t incVertexIndex2;
    // ref의 충돌 법선이랑 내적하기 때문에더 작다는건 그만큼 평행하다는 것
    if (math::Vector2::Dot(nPrev, manifold.collisionNormal) < math::Vector2::Dot(nNext, manifold.collisionNormal))
        incVertexIndex2 = nPrevIndex;
    else
        incVertexIndex2 = inc.GetNextVertexIndex(incVertexIndex);

    // 최초 contact point가정
    math::Vector2 contactPoint[2] = {
        math::Vector2(inc.m_pWorldX[incVertexIndex2], inc.m_pWorldY[incVertexIndex2]),
        math::Vector2(inc.m_pWorldX[incVertexIndex], inc.m_pWorldY[incVertexIndex])
    };
    uint32_t contactCount = 2;

    // ===============
    // ======
    // 영역 너머 체크 파트
    // CP 제거 시 반대쪽 버텍스 살아있는 1, 2단계(ref면 왼쪽, 오른쪽면의 법선 테스트)의 경우는 클리핑 해야함. (지터 방지 투포인트 컨택 매니폴드 생성)
    // 단, 3단계(ref면 법선 테스트) 클리핑 시에는 반대쪽 버텍스 살아있어도 아예 제거. (=> 원포인트 컨택 매니폴드 완성)
    math::Vector2 invalidAreaDir;         // 클리핑 면 기준으로 이 방향에 있으면 클리핑 대상)

    // ========================
    // 클리핑 계산용 변수들 버퍼
    real ratio;
    math::Vector2 dir;      // 길이 성분도 유효 (ratio 곱해서 사용)
    // ========================

    // 1. ref 충돌 면의 왼쪽(반시계방향) 면부터 검사
    // 면 기준으로 invalidAreaDir 방향 너머에 있으면 클리핑
    uint16_t faceQueryIndex = ref.GetNextFaceIndex(refFaceIndex);
    invalidAreaDir = ref.GetFaceNormal(faceQueryIndex);
    math::Vector2 faceQueryIndexVertex = math::Vector2(ref.m_pWorldX[faceQueryIndex], ref.m_pWorldY[faceQueryIndex]);
    // reference vertex 중 현재 클리핑 면을 이루는 면의 버텍스 둘 중 하나 선택
    // ref face가 바라보는 기준 왼쪽 영역 클리핑이니깐 refFaceIndex + 1 또는 +2번 버텍스 까지도 사용은 가능
    // 여기서는 refFaceIndex + 1번 버텍스 사용

    real d;
    
    d = math::Vector2::Dot(contactPoint[0] - faceQueryIndexVertex, invalidAreaDir);
    if (d > real(0.0))
    {
        // cp0이 클리핑 영역 너머에 있음
        // cp0 clipping code
        dir = contactPoint[1] - contactPoint[0];        // cp0 ------> cp1
        // cp1은 살아있다는 소리니깐 (invalidAreaDir 반대 방향에 있음)
        ratio = -d / math::Vector2::Dot(dir, invalidAreaDir);
        contactPoint[0] = contactPoint[0] + dir * ratio;            // NEW CONTACT POINT
    }
    else
    {
        d = math::Vector2::Dot(contactPoint[1] - faceQueryIndexVertex, invalidAreaDir);
        if (d > real(0.0))
        {
            // cp1이 클리핑 영역 너머에 있음
            // cp1 clipping code
            dir = contactPoint[0] - contactPoint[1];        // cp1 ------> cp0
            ratio = -d / math::Vector2::Dot(dir, invalidAreaDir);
            contactPoint[1] = contactPoint[1] + dir * ratio;        // NEW CONTACT POINT
        }
    }

    // 2. Clip right area
    faceQueryIndex = ref.GetPrevFaceIndex(refFaceIndex);
    invalidAreaDir = ref.GetFaceNormal(faceQueryIndex);
    faceQueryIndexVertex = math::Vector2(ref.m_pWorldX[faceQueryIndex], ref.m_pWorldY[faceQueryIndex]);

    d = math::Vector2::Dot(contactPoint[0] - faceQueryIndexVertex, invalidAreaDir);
    if (d > real(0.0))
    {
        // cp0이 클리핑 영역 너머에 있음
        // cp0 clipping code
        dir = contactPoint[1] - contactPoint[0];        // cp0 ------> cp1
        ratio = -d / math::Vector2::Dot(dir, invalidAreaDir);
        contactPoint[0] = contactPoint[0] + dir * ratio;            // NEW CONTACT POINT
    }
    else
    {
        d = math::Vector2::Dot(contactPoint[1] - faceQueryIndexVertex, invalidAreaDir);
        if (d > real(0.0))
        {
            // cp1이 클리핑 영역 너머에 있음
            // cp1 clipping code
            dir = contactPoint[0] - contactPoint[1];        // cp1 ------> cp0
            ratio = -d / math::Vector2::Dot(dir, invalidAreaDir);
            contactPoint[1] = contactPoint[1] + dir * ratio;        // NEW CONTACT POINT
        }
    }

    // 3. Clip collision normal face area
    // 충돌 법선 그대로 사용
    faceQueryIndex = refFaceIndex;
    invalidAreaDir = manifold.collisionNormal;
    faceQueryIndexVertex = math::Vector2(ref.m_pWorldX[faceQueryIndex], ref.m_pWorldY[faceQueryIndex]);

    d = math::Vector2::Dot(contactPoint[0] - faceQueryIndexVertex, invalidAreaDir);
    if (d > real(0.0))
    {
        // cp0이 클리핑 영역 너머에 있음
        // cp0 제거
        contactPoint[0] = contactPoint[1];
        contactCount = 1;
    }
    else
    {
        d = math::Vector2::Dot(contactPoint[1] - faceQueryIndexVertex, invalidAreaDir);
        if (d > real(0.0))
        {
            // cp1이 클리핑 영역 너머에 있음
            // cp1 제거
            // contact[0] = contact[0];
            contactCount = 1;
        }
    }

    manifold.contact[0] = contactPoint[0];
    manifold.contact[1] = contactPoint[1];
    manifold.contactCount = contactCount;
}

unsigned int __stdcall PhysicsWorld::WorkerThreadEntry(void* param)
{
    PhysicsWorld* pPhysWorld = reinterpret_cast<PhysicsWorld*>(param);
    const long threadCount = pPhysWorld->m_threadCount;
    volatile bool* const flag = &pPhysWorld->m_terminateMTFlag;

    while (!*flag)
    {
        SleepEx(INFINITE, TRUE);
    }

    return 0;
}


/*
bool PhysicsWorld::CheckCirclevsBoxCollision(Manifold& manifold, const Collider& circleCollider, const Collider& boxCollider)
{
    return CheckBoxvsCircleCollision(manifold, boxCollider, circleCollider);
}

bool PhysicsWorld::CheckBoxvsCircleCollision(Manifold& manifold, const Collider& boxCollider, const Collider& circleCollider)
{
    const BoxCollider& box = reinterpret_cast<const BoxCollider&>(boxCollider);
    const CircleCollider& circle = reinterpret_cast<const CircleCollider&>(circleCollider);

    // 원의 중심이 박스 내에 있는지 외부에 있는지 검사

    math::Vector2 boxNormal[4];
    boxNormal[0].x = std::sin(box.m_radian);
    boxNormal[0].y = -std::cos(box.m_radian);
    boxNormal[1].x = -boxNormal[0].y;
    boxNormal[1].y = boxNormal[0].x;
    boxNormal[2].x = -boxNormal[1].y;
    boxNormal[2].y = boxNormal[1].x;
    boxNormal[3].x = -boxNormal[2].y;
    boxNormal[3].y = boxNormal[2].x;

    real bestDistance = std::numeric_limits<real>::lowest();
    uint16_t faceIndex;
    for (uint16_t i = 0; i < 4; i++)
    {
        math::Vector2 circleSP = circle.m_position + boxNormal[(i + uint16_t(2)) & uint16_t(0x0003)] * circle.m_radius;
        real d = math::Vector2::Dot(circleSP - box.m_pWorldVertices[i], boxNormal[i]);

        if (d > real(0.0))
            return false;

        if (d > bestDistance)
        {
            bestDistance = d;
            faceIndex = i;
        }
    }

    manifold.pReference = const_cast<Collider*>(&boxCollider);
    manifold.pIncident = const_cast<Collider*>(&circleCollider);

    // 보로노이 영역 테스트
    // 직사각형이라서 Normal 재활용 가능
    math::Vector2 vertexToCenter[2];
    unsigned int closestVertexIndex;        // 원의 중심과 가장 가까운 정점

    vertexToCenter[0] = circle.m_position - box.m_pWorldVertices[(faceIndex + uint16_t(1)) & uint16_t(0x0003)];
    real test = math::Vector2::Dot(vertexToCenter[0], boxNormal[(faceIndex - uint16_t(1)) & uint16_t(0x0003)]);
    if (test >= real(0.0))
    {
        vertexToCenter[1] = circle.m_position - box.m_pWorldVertices[faceIndex];
        test = math::Vector2::Dot(vertexToCenter[1], boxNormal[(faceIndex + uint16_t(1)) & uint16_t(0x0003)]);
        if (test >= real(0.0))
        {
            // face region
            manifold.collisionNormal = boxNormal[faceIndex];
            manifold.penetrationDepth = -bestDistance;


            manifold.contact[0] = circle.m_position - manifold.collisionNormal * (circle.m_radius + bestDistance);
            manifold.contactCount = 1;
            return true;
        }
        else
        {
            closestVertexIndex = 1;
        }
    }
    else
    {
        closestVertexIndex = 0;
    }

    real distanceSquared = vertexToCenter[closestVertexIndex].SquaredLength();
    if (distanceSquared > circle.m_radius * circle.m_radius)
        return false;

    real distance = std::sqrt(distanceSquared);
    manifold.collisionNormal = vertexToCenter[closestVertexIndex] / distance;
    manifold.penetrationDepth = circle.m_radius - distance;
    manifold.contact[0] = circle.m_position - manifold.collisionNormal * circle.m_radius;
    manifold.contactCount = 1;

    return true;
}

void PhysicsWorld::FaceQueryBoxvsBox(Query& query, const BoxCollider& ref, const BoxCollider& inc)
{
    math::Vector2 refNormal[4];
    refNormal[0].x = std::sin(ref.m_radian);        // face[0](vertex[1]-vertex[0]) normal.x
    refNormal[0].y = -std::cos(ref.m_radian);       // face[0](vertex[1]-vertex[0]) normal.y
    refNormal[1].x = -refNormal[0].y;
    refNormal[1].y = refNormal[0].x;
    refNormal[2].x = -refNormal[1].y;
    refNormal[2].y = refNormal[1].x;
    refNormal[3].x = -refNormal[2].y;
    refNormal[3].y = refNormal[2].x;

    uint16_t incSupportIdx[4]; // [i] = ref normal i의 반대방향으로 검색한 other의 Support point
    // left normal (x, y) => (-y, x)

    incSupportIdx[0] = inc.SupportIndex(refNormal[2]);                  // normal[0]의 반대 방향 벡터인 normal[2] 사용
    incSupportIdx[2] = (incSupportIdx[0] + uint16_t(2)) & uint16_t(0x0003);       // Box To Box + SP 검색 비교 순서 CCW로 동일하므로 이렇게 계산 가능
    incSupportIdx[1] = (incSupportIdx[0] + uint16_t(1)) & uint16_t(0x0003);       // 위와 동일
    incSupportIdx[3] = (incSupportIdx[0] + uint16_t(3)) & uint16_t(0x0003);       // 위와 동일

    real bestDistanceFromRefToInc = std::numeric_limits<real>::lowest();
    uint16_t faceIndex;                    // face [i] = vertex[(i + 1) % 4] - vertex[i]
    uint16_t bestSupportIndexOfInc;
    for (uint16_t i = 0; i < 4; i++)
    {
        real d = math::Vector2::Dot(inc.m_pWorldVertices[incSupportIdx[i]] - ref.m_pWorldVertices[i], refNormal[i]);
        if (d > real(0.0))
        {
            query.axisExist = true;
            return;
        }

        if (d > bestDistanceFromRefToInc)
        {
            bestDistanceFromRefToInc = d;
            faceIndex = i;
            bestSupportIndexOfInc = incSupportIdx[i];
        }
    }

    query.axisExist = false;
    query.refFaceIndex = faceIndex;
    query.incSupportIndex = bestSupportIndexOfInc;
    query.bestDistance = bestDistanceFromRefToInc;
    query.refFaceNormal = refNormal[faceIndex];
}

void PhysicsWorld::ComputeBoxvsBoxContactPoint(Manifold& manifold, uint16_t refFaceIndex, uint16_t incVertexIndex)
{
    const BoxCollider& ref = reinterpret_cast<const BoxCollider&>(*manifold.pReference);
    const BoxCollider& inc = reinterpret_cast<const BoxCollider&>(*manifold.pIncident);

    // n1, n2 (incVertex를 포함하는 두 면 각각의 법선 벡터)
    const math::Vector2 nPrev = inc.GetFaceNormal((incVertexIndex - uint16_t(1)) & uint16_t(0x0003));       // vertex index는 face index로도 쓰일 수 있음
    const math::Vector2 nNext = math::Vector2(-nPrev.y, nPrev.x);

    uint16_t incVertexIndex2;
    // ref의 충돌 법선이랑 내적하기 때문에더 작다는건 그만큼 평행하다는 것
    if (math::Vector2::Dot(nPrev, manifold.collisionNormal) < math::Vector2::Dot(nNext, manifold.collisionNormal))
        incVertexIndex2 = (incVertexIndex - uint16_t(1)) & uint16_t(0x0003);
    else
        incVertexIndex2 = (incVertexIndex + uint16_t(1)) & uint16_t(0x0003);

    // 최초 contact point가정
    math::Vector2 contactPoint[2] = { inc.m_pWorldVertices[incVertexIndex2], inc.m_pWorldVertices[incVertexIndex] };
    uint32_t contactCount = 2;

    // ===============
    // ======
    // 영역 너머 체크 파트
    // CP 제거 시 반대쪽 버텍스 살아있는 1, 2단계(ref면 왼쪽, 오른쪽면의 법선 테스트)의 경우는 클리핑 해야함. (지터 방지 투포인트 컨택 매니폴드 생성)
    // 단, 3단계(ref면 법선 테스트) 클리핑 시에는 반대쪽 버텍스 살아있어도 아예 제거. (=> 원포인트 컨택 매니폴드 완성)
    math::Vector2 invalidAreaDir;         // 클리핑 면 기준으로 이 방향에 있으면 클리핑 대상)

    // ========================
    // 클리핑 계산용 변수들 버퍼
    real ratio;
    math::Vector2 dir;      // 길이 성분도 유효 (ratio 곱해서 사용)
    // ========================

    // 1. ref 충돌 면의 왼쪽(반시계방향) 면부터 검사
    // 직사각형이므로 좌우측 클리핑에 필요한 노말들을 Ref 노말의 left, right normal을 써서 빠르게 계산할 수 있음
    // 면 기준으로 invalidAreaDir 방향 너머에 있으면 클리핑
    invalidAreaDir.x = -manifold.collisionNormal.y;
    invalidAreaDir.y = manifold.collisionNormal.x;

    // reference vertex 중 현재 클리핑 면을 이루는 면의 버텍스 둘 중 하나 선택
    // ref face가 바라보는 기준 왼쪽 영역 클리핑이니깐 refFaceIndex + 1 또는 +2번 버텍스 까지도 사용은 가능
    // 여기서는 refFaceIndex + 1번 버텍스 사용

    real d;
    d = math::Vector2::Dot(contactPoint[0] - ref.m_pWorldVertices[(refFaceIndex + uint16_t(1)) & uint16_t(0x0003)], invalidAreaDir);
    if (d > real(0.0))
    {
        // cp0이 클리핑 영역 너머에 있음
        // cp0 clipping code
        dir = contactPoint[1] - contactPoint[0];        // cp0 ------> cp1
        // cp1은 살아있다는 소리니깐 (invalidAreaDir 반대 방향에 있음)
        ratio = -d / math::Vector2::Dot(dir, invalidAreaDir);
        contactPoint[0] = contactPoint[0] + dir * ratio;            // NEW CONTACT POINT
    }
    else
    {
        d = math::Vector2::Dot(contactPoint[1] - ref.m_pWorldVertices[(refFaceIndex + uint16_t(1)) & uint16_t(0x0003)], invalidAreaDir);
        if (d > real(0.0))
        {
            // cp1이 클리핑 영역 너머에 있음
            // cp1 clipping code
            dir = contactPoint[0] - contactPoint[1];        // cp1 ------> cp0
            ratio = -d / math::Vector2::Dot(dir, invalidAreaDir);
            contactPoint[1] = contactPoint[1] + dir * ratio;        // NEW CONTACT POINT
        }
    }

    // 2. Clip right area
    // 충돌 법선의 right normal (y, -x) 사용
    invalidAreaDir.x = manifold.collisionNormal.y;
    invalidAreaDir.y = -manifold.collisionNormal.x;

    d = math::Vector2::Dot(contactPoint[0] - ref.m_pWorldVertices[refFaceIndex], invalidAreaDir);
    if (d > real(0.0))
    {
        // cp0이 클리핑 영역 너머에 있음
        // cp0 clipping code
        dir = contactPoint[1] - contactPoint[0];        // cp0 ------> cp1
        ratio = -d / math::Vector2::Dot(dir, invalidAreaDir);
        contactPoint[0] = contactPoint[0] + dir * ratio;            // NEW CONTACT POINT
    }
    else
    {
        d = math::Vector2::Dot(contactPoint[1] - ref.m_pWorldVertices[refFaceIndex], invalidAreaDir);
        if (d > real(0.0))
        {
            // cp1이 클리핑 영역 너머에 있음
            // cp1 clipping code
            dir = contactPoint[0] - contactPoint[1];        // cp1 ------> cp0
            ratio = -d / math::Vector2::Dot(dir, invalidAreaDir);
            contactPoint[1] = contactPoint[1] + dir * ratio;        // NEW CONTACT POINT
        }
    }

    // 3. Clip collision normal face area
    // 충돌 법선 그대로 사용
    invalidAreaDir = manifold.collisionNormal;

    d = math::Vector2::Dot(contactPoint[0] - ref.m_pWorldVertices[refFaceIndex], invalidAreaDir);
    if (d > real(0.0))
    {
        // cp0이 클리핑 영역 너머에 있음
        // cp0 제거
        contactPoint[0] = contactPoint[1];
        contactCount = 1;
    }
    else
    {
        d = math::Vector2::Dot(contactPoint[1] - ref.m_pWorldVertices[refFaceIndex], invalidAreaDir);
        if (d > real(0.0))
        {
            // cp1이 클리핑 영역 너머에 있음
            // cp1 제거
            // contact[0] = contact[0];
            contactCount = 1;
        }
    }

    manifold.contact[0] = contactPoint[0];
    manifold.contact[1] = contactPoint[1];
    manifold.contactCount = contactCount;
}

void PhysicsWorld::ComputeBoxvsHullContactPoint(Manifold& manifold, uint16_t refFaceIndex, uint16_t incVertexIndex)
{
    const BoxCollider& ref = reinterpret_cast<const BoxCollider&>(*manifold.pReference);
    const HullCollider& inc = reinterpret_cast<const HullCollider&>(*manifold.pIncident);

    // n1, n2 (incVertex를 포함하는 두 면 각각의 법선 벡터)
    uint16_t nPrevIndex = inc.GetPrevVertexIndex(incVertexIndex);
    const math::Vector2 nPrev = inc.GetFaceNormal(nPrevIndex);       // vertex index는 face index로도 쓰일 수 있음
    const math::Vector2 nNext = inc.GetFaceNormal(incVertexIndex);

    uint16_t incVertexIndex2;
    // ref의 충돌 법선이랑 내적하기 때문에더 작다는건 그만큼 평행하다는 것
    if (math::Vector2::Dot(nPrev, manifold.collisionNormal) < math::Vector2::Dot(nNext, manifold.collisionNormal))
        incVertexIndex2 = nPrevIndex;
    else
    {
        incVertexIndex2 = inc.GetNextVertexIndex(incVertexIndex);
    }

    // 최초 contact point가정
    math::Vector2 contactPoint[2] = { inc.m_pWorldVertices[incVertexIndex2], inc.m_pWorldVertices[incVertexIndex] };
    uint32_t contactCount = 2;

    // ===============
    // ======
    // 영역 너머 체크 파트
    // CP 제거 시 반대쪽 버텍스 살아있는 1, 2단계(ref면 왼쪽, 오른쪽면의 법선 테스트)의 경우는 클리핑 해야함. (지터 방지 투포인트 컨택 매니폴드 생성)
    // 단, 3단계(ref면 법선 테스트) 클리핑 시에는 반대쪽 버텍스 살아있어도 아예 제거. (=> 원포인트 컨택 매니폴드 완성)
    math::Vector2 invalidAreaDir;         // 클리핑 면 기준으로 이 방향에 있으면 클리핑 대상)

    // ========================
    // 클리핑 계산용 변수들 버퍼
    real ratio;
    math::Vector2 dir;      // 길이 성분도 유효 (ratio 곱해서 사용)
    // ========================

    // 1. ref 충돌 면의 왼쪽(반시계방향) 면부터 검사
    // ref가 직사각형이므로 충돌 벡터 회전만으로 왼쪽, 오른쪽 면 노말 구할 수 있음
    // 면 기준으로 invalidAreaDir 방향 너머에 있으면 클리핑
    invalidAreaDir.x = -manifold.collisionNormal.y;     // 충돌 면 왼쪽 면의 노말(left normal로 구해짐)
    invalidAreaDir.y = manifold.collisionNormal.x;

    // reference vertex 중 현재 클리핑 면을 이루는 면의 버텍스 둘 중 하나 선택
    // ref face가 바라보는 기준 왼쪽 영역 클리핑이니깐 refFaceIndex + 1 또는 +2번 버텍스 까지도 사용은 가능
    // 여기서는 refFaceIndex + 1번 버텍스 사용

    real d;
    d = math::Vector2::Dot(contactPoint[0] - ref.m_pWorldVertices[(refFaceIndex + uint16_t(1)) & uint16_t(0x0003)], invalidAreaDir);
    if (d > real(0.0))
    {
        // cp0이 클리핑 영역 너머에 있음
        // cp0 clipping code
        dir = contactPoint[1] - contactPoint[0];        // cp0 ------> cp1
        // cp1은 살아있다는 소리니깐 (invalidAreaDir 반대 방향에 있음)
        ratio = -d / math::Vector2::Dot(dir, invalidAreaDir);
        contactPoint[0] = contactPoint[0] + dir * ratio;            // NEW CONTACT POINT
    }
    else
    {
        d = math::Vector2::Dot(contactPoint[1] - ref.m_pWorldVertices[(refFaceIndex + uint16_t(1)) & uint16_t(0x0003)], invalidAreaDir);
        if (d > real(0.0))
        {
            // cp1이 클리핑 영역 너머에 있음
            // cp1 clipping code
            dir = contactPoint[0] - contactPoint[1];        // cp1 ------> cp0
            ratio = -d / math::Vector2::Dot(dir, invalidAreaDir);
            contactPoint[1] = contactPoint[1] + dir * ratio;        // NEW CONTACT POINT
        }
    }

    // 2. Clip right area
    // 충돌 법선의 right normal (y, -x) 사용
    invalidAreaDir.x = manifold.collisionNormal.y;
    invalidAreaDir.y = -manifold.collisionNormal.x;

    d = math::Vector2::Dot(contactPoint[0] - ref.m_pWorldVertices[refFaceIndex], invalidAreaDir);
    if (d > real(0.0))
    {
        // cp0이 클리핑 영역 너머에 있음
        // cp0 clipping code
        dir = contactPoint[1] - contactPoint[0];        // cp0 ------> cp1
        ratio = -d / math::Vector2::Dot(dir, invalidAreaDir);
        contactPoint[0] = contactPoint[0] + dir * ratio;            // NEW CONTACT POINT
    }
    else
    {
        d = math::Vector2::Dot(contactPoint[1] - ref.m_pWorldVertices[refFaceIndex], invalidAreaDir);
        if (d > real(0.0))
        {
            // cp1이 클리핑 영역 너머에 있음
            // cp1 clipping code
            dir = contactPoint[0] - contactPoint[1];        // cp1 ------> cp0
            ratio = -d / math::Vector2::Dot(dir, invalidAreaDir);
            contactPoint[1] = contactPoint[1] + dir * ratio;        // NEW CONTACT POINT
        }
    }

    // 3. Clip collision normal face area
    // 충돌 법선 그대로 사용
    invalidAreaDir.x = manifold.collisionNormal.x;
    invalidAreaDir.y = manifold.collisionNormal.y;

    d = math::Vector2::Dot(contactPoint[0] - ref.m_pWorldVertices[refFaceIndex], invalidAreaDir);
    if (d > real(0.0))
    {
        // cp0이 클리핑 영역 너머에 있음
        // cp0 제거
        contactPoint[0] = contactPoint[1];
        contactCount = 1;
    }
    else
    {
        d = math::Vector2::Dot(contactPoint[1] - ref.m_pWorldVertices[refFaceIndex], invalidAreaDir);
        if (d > real(0.0))
        {
            // cp1이 클리핑 영역 너머에 있음
            // cp1 제거
            // contact[0] = contact[0];
            contactCount = 1;
        }
    }

    manifold.contact[0] = contactPoint[0];
    manifold.contact[1] = contactPoint[1];
    manifold.contactCount = contactCount;
}

void PhysicsWorld::ComputeHullvsBoxContactPoint(Manifold& manifold, uint16_t refFaceIndex, uint16_t incVertexIndex)
{
    const HullCollider& ref = reinterpret_cast<const HullCollider&>(*manifold.pReference);
    const BoxCollider& inc = reinterpret_cast<const BoxCollider&>(*manifold.pIncident);

    // n1, n2 (incVertex를 포함하는 두 면 각각의 법선 벡터)
    const math::Vector2 nPrev = inc.GetFaceNormal((incVertexIndex - uint16_t(1)) & uint16_t(0x0003));       // vertex index는 face index로도 쓰일 수 있음
    const math::Vector2 nNext = inc.GetFaceNormal(incVertexIndex);

    uint16_t incVertexIndex2;
    // ref의 충돌 법선이랑 내적하기 때문에더 작다는건 그만큼 평행하다는 것
    if (math::Vector2::Dot(nPrev, manifold.collisionNormal) < math::Vector2::Dot(nNext, manifold.collisionNormal))
        incVertexIndex2 = (incVertexIndex - uint16_t(1)) & uint16_t(0x0003);
    else
        incVertexIndex2 = (incVertexIndex + uint16_t(1)) & uint16_t(0x0003);

    // 최초 contact point가정
    math::Vector2 contactPoint[2] = { inc.m_pWorldVertices[incVertexIndex2], inc.m_pWorldVertices[incVertexIndex] };
    uint32_t contactCount = 2;

    // ===============
    // ======
    // 영역 너머 체크 파트
    // CP 제거 시 반대쪽 버텍스 살아있는 1, 2단계(ref면 왼쪽, 오른쪽면의 법선 테스트)의 경우는 클리핑 해야함. (지터 방지 투포인트 컨택 매니폴드 생성)
    // 단, 3단계(ref면 법선 테스트) 클리핑 시에는 반대쪽 버텍스 살아있어도 아예 제거. (=> 원포인트 컨택 매니폴드 완성)
    math::Vector2 invalidAreaDir;         // 클리핑 면 기준으로 이 방향에 있으면 클리핑 대상)

    // ========================
    // 클리핑 계산용 변수들 버퍼
    real ratio;
    math::Vector2 dir;      // 길이 성분도 유효 (ratio 곱해서 사용)
    // ========================

    // 1. ref 충돌 면의 왼쪽(반시계방향) 면부터 검사
    // 면 기준으로 invalidAreaDir 방향 너머에 있으면 클리핑
    uint16_t faceQueryIndex = ref.GetNextFaceIndex(refFaceIndex);
    invalidAreaDir = ref.GetFaceNormal(faceQueryIndex);

    // reference vertex 중 현재 클리핑 면을 이루는 면의 버텍스 둘 중 하나 선택
    // ref face가 바라보는 기준 왼쪽 영역 클리핑이니깐 refFaceIndex + 1 또는 +2번 버텍스 까지도 사용은 가능
    // 여기서는 refFaceIndex + 1번 버텍스 사용

    real d;
    d = math::Vector2::Dot(contactPoint[0] - ref.m_pWorldVertices[faceQueryIndex], invalidAreaDir);
    if (d > real(0.0))
    {
        // cp0이 클리핑 영역 너머에 있음
        // cp0 clipping code
        dir = contactPoint[1] - contactPoint[0];        // cp0 ------> cp1
        // cp1은 살아있다는 소리니깐 (invalidAreaDir 반대 방향에 있음)
        ratio = -d / math::Vector2::Dot(dir, invalidAreaDir);
        contactPoint[0] = contactPoint[0] + dir * ratio;            // NEW CONTACT POINT
    }
    else
    {
        d = math::Vector2::Dot(contactPoint[1] - ref.m_pWorldVertices[faceQueryIndex], invalidAreaDir);
        if (d > real(0.0))
        {
            // cp1이 클리핑 영역 너머에 있음
            // cp1 clipping code
            dir = contactPoint[0] - contactPoint[1];        // cp1 ------> cp0
            ratio = -d / math::Vector2::Dot(dir, invalidAreaDir);
            contactPoint[1] = contactPoint[1] + dir * ratio;        // NEW CONTACT POINT
        }
    }

    // 2. Clip right area
    faceQueryIndex = ref.GetPrevFaceIndex(refFaceIndex);
    invalidAreaDir = ref.GetFaceNormal(faceQueryIndex);

    d = math::Vector2::Dot(contactPoint[0] - ref.m_pWorldVertices[refFaceIndex], invalidAreaDir);
    if (d > real(0.0))
    {
        // cp0이 클리핑 영역 너머에 있음
        // cp0 clipping code
        dir = contactPoint[1] - contactPoint[0];        // cp0 ------> cp1
        ratio = -d / math::Vector2::Dot(dir, invalidAreaDir);
        contactPoint[0] = contactPoint[0] + dir * ratio;            // NEW CONTACT POINT
    }
    else
    {
        d = math::Vector2::Dot(contactPoint[1] - ref.m_pWorldVertices[refFaceIndex], invalidAreaDir);
        if (d > real(0.0))
        {
            // cp1이 클리핑 영역 너머에 있음
            // cp1 clipping code
            dir = contactPoint[0] - contactPoint[1];        // cp1 ------> cp0
            ratio = -d / math::Vector2::Dot(dir, invalidAreaDir);
            contactPoint[1] = contactPoint[1] + dir * ratio;        // NEW CONTACT POINT
        }
    }

    // 3. Clip collision normal face area
    // 충돌 법선 그대로 사용
    invalidAreaDir.x = manifold.collisionNormal.x;
    invalidAreaDir.y = manifold.collisionNormal.y;

    d = math::Vector2::Dot(contactPoint[0] - ref.m_pWorldVertices[refFaceIndex], invalidAreaDir);
    if (d > real(0.0))
    {
        // cp0이 클리핑 영역 너머에 있음
        // cp0 제거
        contactPoint[0] = contactPoint[1];
        contactCount = 1;
    }
    else
    {
        d = math::Vector2::Dot(contactPoint[1] - ref.m_pWorldVertices[refFaceIndex], invalidAreaDir);
        if (d > real(0.0))
        {
            // cp1이 클리핑 영역 너머에 있음
            // cp1 제거
            // contact[0] = contact[0];
            contactCount = 1;
        }
    }

    manifold.contact[0] = contactPoint[0];
    manifold.contact[1] = contactPoint[1];
    manifold.contactCount = contactCount;
}

void PhysicsWorld::FaceQueryBoxvsHull(Query& query, const BoxCollider& ref, const HullCollider& inc)
{
    math::Vector2 refNormal[4];
    refNormal[0].x = std::sin(ref.m_radian);        // face[0](vertex[1]-vertex[0]) normal.x
    refNormal[0].y = -std::cos(ref.m_radian);       // face[0](vertex[1]-vertex[0]) normal.y
    refNormal[1].x = -refNormal[0].y;
    refNormal[1].y = refNormal[0].x;
    refNormal[2].x = -refNormal[1].y;
    refNormal[2].y = refNormal[1].x;
    refNormal[3].x = -refNormal[2].y;
    refNormal[3].y = refNormal[2].x;

    uint16_t incSupportIndex[4]; // [i] = ref normal i의 반대방향으로 검색한 other의 Support point index
    // left normal (x, y) => (-y, x)

    incSupportIndex[0] = inc.SupportIndex(refNormal[2]);       // ref 0번 면에 대한 inc의 support point검색은 ref 2번 노말 사용하면 됨
    incSupportIndex[1] = inc.SupportIndex(refNormal[3]);
    incSupportIndex[2] = inc.SupportIndex(refNormal[0]);
    incSupportIndex[3] = inc.SupportIndex(refNormal[1]);

    real bestDistance = std::numeric_limits<real>::lowest();
    uint16_t bestFaceIndex;                    // face [i] = vertex[(i + 1) % 4] - vertex[i]
    uint16_t incBestSupportIndex;
    for (uint16_t i = 0; i < 4; i++)
    {
        real d = math::Vector2::Dot(inc.m_pWorldVertices[incSupportIndex[i]] - ref.m_pWorldVertices[i], refNormal[i]);
        if (d > real(0.0))
        {
            query.axisExist = true;
            return;
        }

        if (d > bestDistance)
        {
            bestDistance = d;
            bestFaceIndex = i;
            incBestSupportIndex = incSupportIndex[i];
        }
    }

    query.axisExist = false;
    query.refFaceIndex = bestFaceIndex;
    query.incSupportIndex = incBestSupportIndex;
    query.bestDistance = bestDistance;
    query.refFaceNormal = refNormal[bestFaceIndex];
}

void PhysicsWorld::FaceQueryHullvsBox(Query& query, const HullCollider& ref, const BoxCollider& inc)
{
    real bestDistance = std::numeric_limits<real>::lowest();
    math::Vector2 bestFaceNormal;
    uint16_t bestFaceIndex;                    // face [i] = vertex[(i + 1) % 4] - vertex[i]
    uint16_t incBestSupportIndex;

    uint16_t hullVertexCount = ref.m_vertexCount;
    for (uint16_t i = 0; i < hullVertexCount; i++)
    {
        math::Vector2 refNormal = ref.GetFaceNormal(i);
        uint16_t incSupportIndex = inc.SupportIndex(-refNormal);
        math::Vector2 incSupport = inc.m_pWorldVertices[incSupportIndex];

        real d = math::Vector2::Dot(incSupport - ref.m_pWorldVertices[i], refNormal);
        if (d > real(0.0))
        {
            query.axisExist = true;
            return;
        }

        if (d > bestDistance)
        {
            bestFaceNormal = refNormal;
            bestDistance = d;
            bestFaceIndex = i;
            incBestSupportIndex = incSupportIndex;
        }
    }

    query.axisExist = false;
    query.refFaceIndex = bestFaceIndex;
    query.incSupportIndex = incBestSupportIndex;
    query.bestDistance = bestDistance;
    query.refFaceNormal = bestFaceNormal;
}
 
bool PhysicsWorld::CheckBoxvsBoxCollision(Manifold& manifold, const Collider& boxColliderA, const Collider& boxColliderB)
{
    Query queryA_B;     // BoxA->BoxB Query
    Query queryB_A;     // BoxB->BoxA Query

    // -------------------------------------------
    // Test A->B                                //
    FaceQueryBoxvsBox(queryA_B, reinterpret_cast<const BoxCollider&>(boxColliderA), reinterpret_cast<const BoxCollider&>(boxColliderB));    //
    if (queryA_B.axisExist)                     //
        return false;                           //
    // -------------------------------------------

    // -------------------------------------------
    // Test B->A                                //
    FaceQueryBoxvsBox(queryB_A, reinterpret_cast<const BoxCollider&>(boxColliderB), reinterpret_cast<const BoxCollider&>(boxColliderA));    //
    if (queryB_A.axisExist)                     //
        return false;                           //
    // -------------------------------------------

    if (queryA_B.bestDistance > queryB_A.bestDistance)
    {
        // ref = boxA
        manifold.pReference = const_cast<Collider*>(&boxColliderA);
        manifold.pIncident = const_cast<Collider*>(&boxColliderB);
        manifold.collisionNormal = queryA_B.refFaceNormal;
        manifold.penetrationDepth = -queryA_B.bestDistance;
        ComputeBoxvsBoxContactPoint(manifold, queryA_B.refFaceIndex, queryA_B.incSupportIndex);
    }
    else
    {
        // ref = boxB
        manifold.pReference = const_cast<Collider*>(&boxColliderB);
        manifold.pIncident = const_cast<Collider*>(&boxColliderA);
        manifold.collisionNormal = queryB_A.refFaceNormal;
        manifold.penetrationDepth = -queryB_A.bestDistance;
        ComputeBoxvsBoxContactPoint(manifold, queryB_A.refFaceIndex, queryB_A.incSupportIndex);
    }

    return true;
}

bool PhysicsWorld::CheckBoxvsHullCollision(Manifold& manifold, const Collider& boxCollider, const Collider& hullCollider)
{
    Query queryB_H;     // Box->Hull Query
    Query queryH_B;     // Hull->Box Query

    // -------------------------------------------
    // Test Box->Hull                           //
    FaceQueryBoxvsHull(queryB_H, reinterpret_cast<const BoxCollider&>(boxCollider), reinterpret_cast<const HullCollider&>(hullCollider));    //
    if (queryB_H.axisExist)                     //
        return false;                           //
    // -------------------------------------------

    // -------------------------------------------
    // Test Hull->Box                           //
    FaceQueryHullvsBox(queryH_B, reinterpret_cast<const HullCollider&>(hullCollider), reinterpret_cast<const BoxCollider&>(boxCollider));    //
    if (queryH_B.axisExist)                     //
        return false;                           //
    // -------------------------------------------

    if (queryB_H.bestDistance > queryH_B.bestDistance)
    {
        // ref = boxA
        manifold.pReference = const_cast<Collider*>(&boxCollider);
        manifold.pIncident = const_cast<Collider*>(&hullCollider);
        manifold.collisionNormal = queryB_H.refFaceNormal;
        manifold.penetrationDepth = -queryB_H.bestDistance;
        ComputeBoxvsHullContactPoint(manifold, queryB_H.refFaceIndex, queryB_H.incSupportIndex);
    }
    else
    {
        // ref = boxB
        manifold.pReference = const_cast<Collider*>(&hullCollider);
        manifold.pIncident = const_cast<Collider*>(&boxCollider);
        manifold.collisionNormal = queryH_B.refFaceNormal;
        manifold.penetrationDepth = -queryH_B.bestDistance;
        ComputeHullvsBoxContactPoint(manifold, queryH_B.refFaceIndex, queryH_B.incSupportIndex);
    }

    return true;
}

bool PhysicsWorld::CheckHullvsBoxCollision(Manifold& manifold, const Collider& hullCollider, const Collider& boxCollider)
{
    return CheckBoxvsHullCollision(manifold, boxCollider, hullCollider);
}
*/