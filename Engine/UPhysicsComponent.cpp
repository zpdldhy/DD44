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

	m_vVelocity = m_vCurrentDir * m_fCurrentSpeed;

	GetOwner()->AddPosition(m_vVelocity);
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