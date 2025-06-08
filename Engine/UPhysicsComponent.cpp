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

	//UpdateDirection();

	// �߷�
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

// ���� �ӵ��� �����ϰ� ������ _fAccle�� 1.f�� �Է�
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
//	// ���� Frame�� ���
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