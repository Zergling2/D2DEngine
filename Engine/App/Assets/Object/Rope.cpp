#include "Rope.h"
#include "Core\Engine.h"
#include "App\Assets\Object\RopeSegment.h"

using namespace D2DEngine;

Rope::Rope(float segmentRadius, int segmentCount, float segmentLength)
	: GameObject()
	, m_ppRopeSegment(nullptr)
	, m_segmentRadius(segmentRadius)
	, m_segmentCount(segmentCount)
	, m_segmentLength(segmentLength)
{
}

Rope::~Rope()
{
	for (int i = 0; i < m_segmentCount; i++)
	{
		Engine::GetInstance().PhysWorld().RemovePhysicsObject(m_ppRopeSegment[i]);
		D2DESafeDelete(m_ppRopeSegment[i]);
	}
	D2DESafeDeleteArray(m_ppRopeSegment);
}

void Rope::CreateRopeSegment()
{
	float y = GetPosition().y;
	m_ppRopeSegment = new RopeSegment * [m_segmentCount];
	ZeroMemory(m_ppRopeSegment, sizeof(RopeSegment*) * m_segmentCount);
	RopeSegment* pSegment = m_ppRopeSegment[0] = new RopeSegment(m_segmentRadius);
	pSegment->CreateCircleCollider(ip::Default, m_segmentRadius);
	pSegment->CreateRigidBody(ip::Static, 0.2, 0.2 * m_segmentRadius * m_segmentRadius, 0.5, 0.00, 0.00, 0.0);
	pSegment->SetPosition(GetPosition().x, y);
	pSegment->SetPrevPosition(pSegment->GetPosition());
	y -= m_segmentLength;
	Engine::GetInstance().PhysWorld().AddPhysicsObject(pSegment);
	SceneManager::GetInstance().GetCurrentScene().AddObject(pSegment);

	for (int i = 1; i < m_segmentCount; i++)
	{
		pSegment = m_ppRopeSegment[i] = new RopeSegment(m_segmentRadius);

		pSegment->CreateCircleCollider(ip::Default, m_segmentRadius);
		pSegment->CreateRigidBody(ip::Dynamic, 0.2, 0.2 * m_segmentRadius * m_segmentRadius, 0.5, 0.00, 0.00, 0.0);
		pSegment->SetPosition(GetPosition().x, y);
		pSegment->SetPrevPosition(pSegment->GetPosition());
		y -= m_segmentLength;
		Engine::GetInstance().PhysWorld().AddPhysicsObject(pSegment);
		SceneManager::GetInstance().GetCurrentScene().AddObject(pSegment);
	}
}

void Rope::UpdateSegment()
{
	for (int i = 0; i < m_segmentCount; i++)
	{
		m_ppRopeSegment[i]->SetVelocity(m_ppRopeSegment[i]->GetPosition() - m_ppRopeSegment[i]->GetPrevPosition());
		m_ppRopeSegment[i]->SetPrevPosition(m_ppRopeSegment[i]->GetPosition());
		m_ppRopeSegment[i]->AddForce(0.2 * ip::math::Vector2(0.0, -9.81));
		m_ppRopeSegment[i]->Translate(ip::math::Vector2(0.0, -9.81) * Time::GetFixedDeltaTime() * Time::GetFixedDeltaTime());
		m_ppRopeSegment[i]->Translate(m_ppRopeSegment[i]->GetVelocity());
	}
}

void Rope::ApplyConstraint()
{
	if (m_segmentCount < 2)
	{
		return;
	}

	m_ppRopeSegment[0]->SetPosition(GetPosition());
	for (int i = 0; i < m_segmentCount - 1; i++)
	{
		float distance = (m_ppRopeSegment[i + 1]->GetPosition() - m_ppRopeSegment[i]->GetPosition()).Length();
		float diff = m_segmentLength - distance;
		ip::math::Vector2 dir = ip::math::Vector2::Normalized(m_ppRopeSegment[i + 1]->GetPosition() - m_ppRopeSegment[i]->GetPosition());

		ip::math::Vector2 movement = dir * diff;
		if (i == 0)
		{
			m_ppRopeSegment[i + 1]->Translate(movement, ip::World);
		}
		else
		{
			m_ppRopeSegment[i]->Translate(-movement * 0.5f, ip::World);
			m_ppRopeSegment[i + 1]->Translate(movement * 0.5f, ip::World);
		}
	}
	
}

void Rope::Start()
{
	Log::Print(Log::Type::LT_INFO, ToString());
#ifdef _DEBUG
	Log::Print(Log::Type::LT_SYSTEM, L"Rope::Start()");
#endif // _DEBUG
}

void Rope::Awake()
{
#ifdef _DEBUG
	Log::Print(Log::Type::LT_SYSTEM, L"Rope::Awake()");
#endif // _DEBUG
}

void Rope::FixedUpdate()
{
	UpdateSegment();
	ApplyConstraint();
}

void Rope::Update()
{
}

void Rope::OnDestroy()
{
#ifdef _DEBUG
	Log::Print(Log::Type::LT_SYSTEM, L"Rope::OnDestroy()");
#endif // _DEBUG
}

void Rope::OnRender()
{
	for (int i = 0; i < m_segmentCount; i++)
	{
		m_ppRopeSegment[i]->OnRender();
	}
}
