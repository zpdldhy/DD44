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

	// 중력 처리
	const float GravityAcceleration = 9.8f; // 현실적인 중력값
	m_fCurrentGravity += GravityAcceleration * m_fWeight * deltaTime;
	m_fCurrentGravity = Clamp(m_fCurrentGravity, 0.f, m_fMaxGravity);

	// 수평 속도 처리
	m_fCurrentSpeed -= 3.f * deltaTime;
	m_fCurrentSpeed = Clamp(m_fCurrentSpeed, 0.f, m_fMaxSpeed);

	// 방향 정규화
	m_vDirection.Normalize();

	// 속도 구성
	Vec3 horizontalVelocity = m_vDirection * m_fCurrentSpeed;
	Vec3 verticalVelocity = m_fGravityDirection * m_fCurrentGravity;
	Vec3 totalVelocity = horizontalVelocity + verticalVelocity;

	// 예상 이동
	Vec3 currentPosition = GetOwner()->GetPosition();
	Vec3 expectedPosition = currentPosition + totalVelocity;

	// 충돌 검사 및 침투 보정
	Vec3 adjustedVelocity = totalVelocity;
	CollisionCalculate(currentPosition, expectedPosition, adjustedVelocity);

	// 실제 이동
	GetOwner()->AddPosition(adjustedVelocity);
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
//			// 침투 보정: 이동 방향에서 충돌면 법선 제거
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
		//	// 침투 보정: 이동 방향에서 충돌면 법선 제거
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