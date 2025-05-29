#include "pch.h"
#include "UPhysicsComponent.h"
#include "Timer.h"
#include "AActor.h"
#include "UBoxComponent.h"
#include "ObjectManager.h"

void UPhysicsComponent::Init()
{
}

void UPhysicsComponent::Tick()
{
	if (GetOwner()->m_szName == L"Object")
		return;

	m_fGravityDirection = { 0.f, -1.f, 0.f };

	m_fCurrentGravity += m_fGravityAcceleration * m_fWeight * TIMER->GetDeltaTime();

	if (m_fCurrentGravity > m_fMaxGravity)
		m_fCurrentGravity = m_fMaxGravity;

	// �ӵ� ����
	m_fCurrentSpeed -= 10.f * TIMER->GetDeltaTime();

	if (m_fCurrentSpeed > m_fMaxSpeed)
		m_fCurrentSpeed = m_fMaxSpeed;

	if (m_fCurrentSpeed < 0.f)
		m_fCurrentSpeed = 0.f;

	// ���� ����, �����̱⿡ ������ ���� �����̴�.
	m_vDirection.Normalize();

	// �浹ü ����
	CollisionCalculate();

	m_vSpeed = m_vDirection * m_fCurrentSpeed + (m_fCurrentGravity * m_fGravityDirection);

	auto pos = GetOwner()->GetPosition();

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
	m_vDirection.x = _vDir.x;
	m_vDirection.z = _vDir.z;
	m_fCurrentSpeed += _fAccle * TIMER->GetDeltaTime();
	m_fMaxSpeed = _fMaxSpeed;
}

void UPhysicsComponent::CollisionCalculate()
{
	auto pShape = GetOwner()->GetShapeComponent();

	if (pShape == nullptr)
		return;

	for (auto& colData : pShape->GetCollisionList())
	{
		auto normal = colData.second.ColNormal;
		normal.Normalize();
		
		if (normal.y > 0)
			m_fGravityDirection.y += normal.y;

		m_vDirection.x += normal.x;
		m_vDirection.z += normal.z;
	}
}