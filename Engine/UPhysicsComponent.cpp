#include "pch.h"
#include "UPhysicsComponent.h"
#include "Timer.h"
#include "AActor.h"
#include "UBoxComponent.h"
#include "USphereComponent.h"
#include "ObjectManager.h"
#include "CollisionManager.h"

void UPhysicsComponent::Init()
{
}

void UPhysicsComponent::Tick()
{
	m_vCurrentDir.Normalize();

	// 지면 마찰
	m_fCurrentSpeed -= m_fFriction;

	if (m_fCurrentSpeed < 0.f)
		m_fCurrentSpeed = 0.f;

	if (m_fCurrentSpeed > m_fMaxSpeed)
		m_fCurrentSpeed = m_fMaxSpeed;	

	//UpdateDirection();

	// 중력
	if (m_bColGrounded == false && m_fWeight > 0.f)
		m_vCurrentGravity += 0.1f * m_fWeight;

	if (m_vCurrentGravity > m_vMaxGravity)
		m_vCurrentGravity = m_vMaxGravity;

	m_vCurrentDir.Normalize();
	m_vVelocity = m_vCurrentDir * m_fCurrentSpeed + Vec3(0.f, -m_vCurrentGravity, 0.f);

	GetOwner()->AddPosition(m_vVelocity);

	// reset
	m_bColGrounded = false;
}

void UPhysicsComponent::Render()
{
}

void UPhysicsComponent::Destroy()
{
}

// 기존 속도를 유지하고 싶으면 _fAccle에 1.f를 입력
void UPhysicsComponent::SetMove(const Vec3& _vDir, const float& _fMaxSpeed, const float& _fAccle)
{
	m_vCurrentDir += _vDir;
	m_fMaxSpeed = _fMaxSpeed;
	m_fCurrentSpeed += _fAccle;
}

//void UPhysicsComponent::UpdateDirection()
//{
//	auto shape = GetOwner()->GetShapeComponent();
//
//	if (shape == nullptr)
//		return;
//
//	// 이전 Frame의 결과
//	for (auto& colShape : GetOwner()->m_vCollisionList)
//	{
//		auto inter = colShape.second.Inter;
//		auto normal = inter - shape->GetCenter();
//		normal.Normalize();
//
//		auto D = normal.Dot(m_vCurrentDir);
//
//		if (colShape.second.bColGround == true)
//		{
//			m_bColGrounded = true;
//			m_vCurrentGravity = 0.f;			
//		}
//
//		//m_vCurrentDir -= D * normal;
//	}
//}