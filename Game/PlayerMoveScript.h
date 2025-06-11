#pragma once
#include "UScriptComponent.h"
#include "PlayerStates.h"

class APawn;
class AActor;
class UMeshComponent;

class PlayerMoveScript : public UScriptComponent
{
public:
	PlayerMoveScript() = default;
	virtual ~PlayerMoveScript() = default;
public:
	float m_fSpeed = 10.0f;
	float m_fRollSpeed  = 15.0f;
	float m_fCurrentSpeed = 0.f;
	float m_fRotationSpeed = 8.0f;

	bool m_bDamageCoolTime = false;
	float m_fDamageCoolTime = 1.0f;
	bool m_bCanBeHit = true;

	bool m_bRollCoolTime = false;
	float m_fRollCoolTime = 0.3f;
	bool m_bCanRoll = true;

	// Attack
	shared_ptr<AActor> attackRangeActor;
	Vec3 colOffset;

	Vec3 m_vLook;
	Vec3 m_vRight;
	Vec3 m_vLastMoveDir;
	Vec3 velocity;
	Vec3 m_vRollLook;

	// 상호작용
	//UINT m_vHP = 4;
	UINT m_vArrowCount = 4;

	// UI Actor
	vector<shared_ptr<class AUIActor>> m_vHPUI;
	vector<shared_ptr<class AUIActor>> m_vArrowUI;
	bool m_bHPUIChange = false;

	Color fullHP = { 0.055f, 0.247f, -0.324, 0.0f };

	// Child Mesh Components
	weak_ptr<UMeshComponent> backSword;
	weak_ptr<UMeshComponent> handSword;

	shared_ptr<StateBase> idle;
	shared_ptr<StateBase> walk;
	shared_ptr<StateBase> roll;
	shared_ptr<StateBase> attack;
	shared_ptr<StateBase> hit;
	shared_ptr<StateBase> shoot;
	shared_ptr<StateBase> die;
	shared_ptr<StateBase> currentState;

	// Slash 
	shared_ptr<UMaterial> m_pSlashMaterial = nullptr;
	float m_fSlashTime = 0.0f;
	bool m_bSlashPlaying = false;
	float m_fSlashDuration = 0.5f;
	 
	// Camera Offset
	Vec3 m_vCameraOffset = { 20.0f, 30.0f, -20.0f };
	// temp
	int animIndex = 0;

public:
	void Init() override;
	void Tick() override;
	virtual shared_ptr<UScriptComponent> Clone() override;
private:
	void SetUI();
	void UpdateHPUI();
	void UpdateArrowUI();
public:
	void ChangetState(shared_ptr<StateBase> _state);
public:
	void Slash();
	void PlayBloodBurst(const Vec3& _origin, const Vec3& _direction, float _speed, float _spreadAngleDeg, int _minCount = 5, int _maxCount = 10);
	void VisitAllMeshMaterials(shared_ptr<UMeshComponent> comp);
	float m_fHitFlashTimer = 0.0f;
	bool m_bIsFlashing = false;
	void ApplyHitFlashToAllMaterials(shared_ptr<UMeshComponent> comp, float value);
public:
	void Move();
	void CheckHit();
	void RollMove();
	bool CanAttack();
	void UpdateCollider();
};

