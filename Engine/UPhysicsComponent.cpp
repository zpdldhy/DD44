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

	// ���� ����
	m_fCurrentSpeed -= m_fFriction;

	if (m_fCurrentSpeed < 0.f)
		m_fCurrentSpeed = 0.f;

	if (m_fCurrentSpeed > m_fMaxSpeed)
		m_fCurrentSpeed = m_fMaxSpeed;	

	// �߷�
	if (m_bColGrounded == false && m_fWeight > 0.f)
		m_vCurrentGravity += 0.1f * m_fWeight;
	else if (m_bColGrounded == true)
		m_vCurrentGravity = 0.f;

	if (m_vCurrentGravity > m_vMaxGravity)
		m_vCurrentGravity = m_vMaxGravity;

	m_vCurrentDir.Normalize();
	m_vVelocity = m_vCurrentDir * m_fCurrentSpeed + Vec3(0.f, -m_vCurrentGravity, 0.f);

	//auto tempTime = TIMER->GetDeltaTime() / 0.016667f;
	//m_vVelocity *= tempTime;

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

// ���� �ӵ��� �����ϰ� ������ _fAccle�� 1.f�� �Է�
void UPhysicsComponent::SetMove(const Vec3& _vDir, const float& _fMaxSpeed, const float& _fAccle)
{
	m_vCurrentDir += _vDir;
	m_fMaxSpeed = _fMaxSpeed;
	m_fCurrentSpeed += _fAccle;
}