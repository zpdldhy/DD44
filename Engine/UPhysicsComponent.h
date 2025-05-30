#pragma once
#include "USceneComponent.h"

class UPhysicsComponent : public USceneComponent
{
public:
	UPhysicsComponent() = default;
	virtual ~UPhysicsComponent() = default;

public:
	void Init() override;
	void Tick() override;
	void Render() override;
	void Destroy() override;

public:
	void SetMaxGravity(const float& _fMaxGravity) { m_fMaxGravity = _fMaxGravity; }
	void SetMaxSpeed(const float& _fMaxSpeed) { m_fMaxSpeed = _fMaxSpeed; }
	void SetWeight(const float& _fWeight) { m_fWeight = _fWeight; }
	void SetMove(const Vec3& _vDir, const float& _fMaxSpeed, const float& _fAccle = 1.0f);

	float GetCurrentGravity() { return m_fCurrentGravity; }
	float GetCurrentSpeed() { return m_fCurrentGravity; }
	Vec3 GetVelocity() { return m_vVelocity; }

private:
	void CollisionCalculate(const Vec3& currentPos, const Vec3& targetPos, Vec3& velocity);

public:
	bool m_bFly = false;

private:
	bool m_bColGrounded = false;

	// Gravity
	float m_fMaxGravity = 0.5f;
	Vec3 m_fGravityDirection = { 0.f, -1.f, 0.f };
	float m_fCurrentGravity = 0.f;	// 연산에 사용되는 놈
	float m_fWeight = 0.f;

	// Speed
	float m_fAcceleration = 0.f;
	float m_fMaxSpeed = 0.5f;
	float m_fCurrentSpeed = 0.f;	// 연산에 사용되는 놈
	Vec3 m_vDirection = { 0.f, 0.f, 0.f };
	Vec3 m_vVelocity = { 0.f, 0.f, 0.f };

	Vec3 m_normal = { 0.f, 0.f, 0.f };
};

