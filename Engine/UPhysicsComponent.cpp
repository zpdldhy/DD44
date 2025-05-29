#include "pch.h"
#include "UPhysicsComponent.h"
#include "Timer.h"
#include "AActor.h"

void UPhysicsComponent::Init()
{
}

void UPhysicsComponent::Tick()
{
	// 중력 연산
	//m_fCurrentGravity += m_fWeight * TIMER->GetDeltaTime();

	if (m_fCurrentGravity > m_fMaxGravity)
		m_fCurrentGravity = m_fMaxGravity;

	// 속도 연산
	m_fCurrentSpeed -= m_fWeight * 2.f * TIMER->GetDeltaTime();

	if (m_fCurrentSpeed > m_fMaxSpeed)
		m_fCurrentSpeed = m_fMaxSpeed;

	if (m_fCurrentSpeed < 0.f)
		m_fCurrentSpeed = 0.f;

	// 방향 연산, 방향이기에 무조건 단위 벡터이다.
	m_vDirection.Normalize();

	m_vSpeed = m_vDirection * m_fCurrentSpeed + (m_fCurrentGravity * Vec3(0.f, -1.f, 0.f));

	// 충돌체 연산
	if (GetOwner()->GetShapeComponent() == nullptr)
		return;

	GetOwner()->AddPosition(m_vSpeed);
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
	m_vDirection = _vDir;
	m_fCurrentSpeed += _fAccle * TIMER->GetDeltaTime();
	m_fMaxSpeed = _fMaxSpeed;
}
