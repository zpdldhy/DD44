#pragma once
#include "UScriptComponent.h"

class APawn;
class AActor;
class UAnimInstance;

class PlayerMoveScript : public UScriptComponent
{
public:
	PlayerMoveScript() = default;
	virtual ~PlayerMoveScript() = default;
public:
	float m_fSpeed  = 13.0f; 
	float m_fRotationSpeed = 8.0f;
	Vec3 m_vLook;
	Vec3 m_vRight;

	// ANIMATION
	shared_ptr<UAnimInstance> m_pAnimInstance;
	bool m_bAttack = false;

	// 상호작용
	bool m_bDamaged = false;
	float m_fDamageTime = 1.f;

	UINT m_vHP = 4;
	UINT m_vArrowCount = 4;

	// UI Actor
	vector<shared_ptr<class AUIActor>> m_vHPUI;
	vector<shared_ptr<class AUIActor>> m_vArrowUI;

	Color fullHP = { 0.055f, 0.247f, -0.324, 0.0f };

public:
	void Init() override;
	void Tick() override;

private:
	void SetUI();
	void UpdateHPUI();
	void UpdateArrowUI();
};

