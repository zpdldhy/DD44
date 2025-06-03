#include "pch.h"
#include "UPhysicsComponent.h"
#include "Timer.h"
#include "AActor.h"
#include "UBoxComponent.h"
#include "ObjectManager.h"
#include "CollisionManager.h"

template<typename T>
T Clamp(T value, T minVal, T maxVal)
{
	if (value < minVal)
		return minVal;
	if (value > maxVal)
		return maxVal;
	return value;
}

void UPhysicsComponent::Init()
{
}

void UPhysicsComponent::Tick()
{
	float deltaTime = TIMER->GetDeltaTime();

	// �߷� ó��
	const float GravityAcceleration = 9.8f; // �������� �߷°�
	m_fCurrentGravity += GravityAcceleration * m_fWeight * deltaTime;
	m_fCurrentGravity = Clamp(m_fCurrentGravity, 0.f, m_fMaxGravity);

	// ���� �ӵ� ó��
	m_fCurrentSpeed -= 3.f * deltaTime;
	m_fCurrentSpeed = Clamp(m_fCurrentSpeed, 0.f, m_fMaxSpeed);

	// ���� ����ȭ
	m_vDirection.Normalize();

	// �ӵ� ����
	Vec3 horizontalVelocity = m_vDirection * m_fCurrentSpeed;
	Vec3 verticalVelocity = m_fGravityDirection * m_fCurrentGravity;
	Vec3 totalVelocity = horizontalVelocity + verticalVelocity;

	// ���� �̵�
	Vec3 currentPosition = GetOwner()->GetPosition();
	Vec3 expectedPosition = currentPosition + totalVelocity;

	// �浹 �˻� �� ħ�� ����
	Vec3 adjustedVelocity = totalVelocity;
	CollisionCalculate(currentPosition, expectedPosition, adjustedVelocity);

	// ���� �̵�
	GetOwner()->AddPosition(adjustedVelocity);
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

//void UPhysicsComponent::CollisionCalculate(const Vec3& currentPos, const Vec3& targetPos, Vec3& velocity)
//{
//	auto pShape = GetOwner()->GetShapeComponent();
//	if (!pShape)
//		return;
//
//	m_bColGrounded = false;
//
//	for (const auto& colData : pShape->GetCollisionList())
//	{
//		Vec3 normal = colData.second.ColNormal;
//		normal.Normalize();
//
//		float dot = velocity.Dot(normal);
//
//		if (dot < 0.f)
//		{
//			// ħ�� ����: �̵� ���⿡�� �浹�� ���� ����
//			velocity -= dot * normal;
//
//			if (colData.second.bColGround || normal.y > 0.5f)
//			{
//				m_bColGrounded = true;
//				m_fCurrentGravity = 0.f;
//			}
//		}
//	}
//}

void UPhysicsComponent::CollisionCalculate(const Vec3& currentPos, const Vec3& targetPos, Vec3& velocity)
{
	auto pShape = GetOwner()->GetShapeComponent();
	if (!pShape)
		return;

	m_bColGrounded = false;
	static float currentVelocity = 0.f;
	static size_t preColSize = 0;
	static float preVelocity = 0.f;

	auto list = pShape->GetCollisionList();

	for (const auto& colData : pShape->GetCollisionList())
	{
		auto p = OBJECT->GetActor(colData.first);
		
		if (p->m_szName == L"")
			int i = 0;

		//Vec3 normal = colData.second.ColNormal;
		//normal.Normalize();

		//float dot = velocity.Dot(normal);

		//if (dot < 0.f)
		//{
		//	// ħ�� ����: �̵� ���⿡�� �浹�� ���� ����
		//	velocity -= dot * normal;

		//	if (colData.second.bColGround)
		//	{
		//		m_bColGrounded = true;
		//		m_fCurrentGravity = 0.f;
		//	}
		//	else if (normal.y > 0.5f)
		//	{
		//		m_bColGrounded = true;
		//		m_fCurrentGravity = 0.f;
		//	}
		//}
	}

	velocity.y = 0.f;

	if (GetOwner()->m_szName == L"MyCharacter")
	{
		currentVelocity = velocity.Length();

		if (preColSize == 1 && pShape->GetCollisionList().size() == 2)
			int i = 0;

		if (pShape->GetCollisionList().size() == 1 || pShape->GetCollisionList().size() == 0)
			int i = 0;

		if (preColSize == 2 && preVelocity <= 0.001f && currentVelocity > preVelocity)
			int i = 0;

		preColSize = pShape->GetCollisionList().size();
		preVelocity = currentVelocity;
	}

}