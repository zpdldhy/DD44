#pragma once
#include "UScriptComponent.h"
#include "PlayerStates.h"

class APawn;
class AActor;
class UAnimInstance;
class UMeshComponent;

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


	// Child Mesh Components
	// 어떻게 관리해야 좋을지 모르겟음
	weak_ptr<UMeshComponent> sword;
	weak_ptr<UMeshComponent> effect;
	weak_ptr<UMeshComponent> handSocket;
	weak_ptr<UMeshComponent> backSocket;

	shared_ptr<StateBase> idle;
	shared_ptr<StateBase> walk;
	shared_ptr<StateBase> attack;
	shared_ptr<StateBase> currentState;

	// Slash 
	shared_ptr<UMaterial> m_pSlashMaterial = nullptr;
	float m_fSlashTime = 0.0f;
	bool m_bSlashPlaying = false;
	float m_fSlashDuration = 0.5f;

public:
	void Init() override;
	void Tick() override;

private:
	void SetUI();
	void UpdateHPUI();
	void UpdateArrowUI();
public:
	void ChangetState(shared_ptr<StateBase> _state);
public:
	void Slash();
	void PlayBloodBurst(const Vec3& _origin, const Vec3& _direction, float _speed, float _spreadAngleDeg, int _minCount = 5, int _maxCount = 10);
};

