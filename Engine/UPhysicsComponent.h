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

private:
	void UpdateDirection();

public:
	bool m_bFly = false;

private:
	bool m_bColGrounded = false;

	// Gravity
	float m_fWeight = 0.f;

	// Move	
	Vec3 m_vCurrentDir = { 0.f, 0.f, 0.f };	// ���� ����(normalize)
	float m_fFriction = 0.1f;				// ���� ������
	float m_fCurrentSpeed = 0.f;			// ���� Speed
	float m_fMaxSpeed = 0.f;				// ���� Speed

	Vec3 m_vVelocity = { 0.f, 0.f, 0.f };	// ���� ���

	// Temp
	size_t listNum = 0;
};

