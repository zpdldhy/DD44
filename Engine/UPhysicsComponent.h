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
	void SetWeight(const float& _fWeight) { m_fWeight = _fWeight; }

	void SetFriction(const float& _fFriction) { m_fFriction = _fFriction; }
	void SetMove(const Vec3& _vDir, const float& _fMaxSpeed, const float& _fAccle = 1.0f);

public:
	bool m_bFly = false;
	bool m_bFall = false;
	bool m_bColGrounded = false;
	float m_fNoGroundTime =0.f;

	// Gravity
	float m_fWeight = 0.f;
	float m_vCurrentGravity = 0.f;
	float m_vMaxGravity = 0.5f;

	// Move	
	Vec3 m_vCurrentDir = { 0.f, 0.f, 0.f };	// 진행 방향(normalize)
	float m_fFriction = 0.1f;				// 지면 마찰력
	float m_fCurrentSpeed = 0.f;			// 현재 Speed
	float m_fMaxSpeed = 0.f;				// 제한 Speed

	Vec3 m_vVelocity = { 0.f, 0.f, 0.f };	// 최종 결과
};

