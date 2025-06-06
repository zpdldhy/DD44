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

	UpdateDirection();

	m_vVelocity = m_vCurrentDir * m_fCurrentSpeed;

	if (m_bColGrounded)
		int i = 0;

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

void UPhysicsComponent::UpdateDirection()
{
	auto shape = GetOwner()->GetShapeComponent();

	if (shape == nullptr)
		return;

	listNum = shape->GetCollisionList().size();

	for (auto& colShape : shape->GetCollisionList())
	{
		auto inter = colShape.second.Inter;
		auto normal = inter - shape->GetCenter();
		normal.Normalize();

		if (colShape.second.bColGround == true)
		{
			m_bColGrounded = true;

			m_vCurrentDir.y = 0.f;
			auto CurrentPos = GetOwner()->GetPosition();

			if (shape->GetShapeType() == ShapeType::ST_SPHERE)
			{
				auto sphere = dynamic_pointer_cast<USphereComponent>(shape);
				CurrentPos.y = inter.y + sphere->GetBounds().fRadius;
			}

			GetOwner()->SetPosition(CurrentPos);
		}

		m_vCurrentDir -= normal;
	}

	if (m_bColGrounded)
	{
	}
}
