#include "pch.h"
#include "UPhysicsComponent.h"
#include "Timer.h"
#include "AActor.h"

void UPhysicsComponent::Init()
{
}

void UPhysicsComponent::Tick()
{
	// �߷� ����
	//m_fCurrentGravity += m_fWeight * TIMER->GetDeltaTime();

	if (m_fCurrentGravity > m_fMaxGravity)
		m_fCurrentGravity = m_fMaxGravity;

	// �ӵ� ����
	m_fCurrentSpeed -= m_fWeight * 2.f * TIMER->GetDeltaTime();

	if (m_fCurrentSpeed > m_fMaxSpeed)
		m_fCurrentSpeed = m_fMaxSpeed;

	if (m_fCurrentSpeed < 0.f)
		m_fCurrentSpeed = 0.f;

	// ���� ����, �����̱⿡ ������ ���� �����̴�.
	m_vDirection.Normalize();

	m_vSpeed = m_vDirection * m_fCurrentSpeed + (m_fCurrentGravity * Vec3(0.f, -1.f, 0.f));

	// �浹ü ����
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

// ���� �ӵ��� �����ϰ� ������ _fAccle�� 1.f�� �Է�
void UPhysicsComponent::SetMove(const Vec3& _vDir, const float& _fMaxSpeed, const float& _fAccle)
{
	m_vDirection = _vDir;
	m_fCurrentSpeed += _fAccle * TIMER->GetDeltaTime();
	m_fMaxSpeed = _fMaxSpeed;
}
