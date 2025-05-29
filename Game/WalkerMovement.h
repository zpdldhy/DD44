#pragma once
#include "UScriptComponent.h"
#include "WalkerStates.h"

class AActor;

class WalkerMovement : public UScriptComponent
{
public:
	WalkerMovement() = default;
	virtual ~WalkerMovement() = default;
private:
	// States 
	shared_ptr<StateBase> idle;
	shared_ptr<StateBase> walk;
	shared_ptr<StateBase> hit;
	shared_ptr<StateBase> death;
	shared_ptr<StateBase> currentState;
private:
	weak_ptr<AActor> player;
private:
	// FX
	Vec3 m_vLastMoveDir;
	Vec3 velocity;
	float m_fHitFlashTimer = 0.0f;
	bool m_bIsFlashing = false;
	// Movement
	float m_wait;
	bool m_bWait;
	Vec3 m_vPos[4];
	Vec3 m_vCurrentTarget;
	int m_currentPosIdx;
	float m_half = 5;
	Vec3 m_vDistance;
	float m_walkSpeed = 0.008;
	float m_fRotationSpeed = 5.0f;
	bool m_rotate;
	Vec3 moveDir;
	// Health
	int m_hp = 3;
public:
	void Init() override;
	void Tick() override;
public:
	void ChangeState(shared_ptr<StateBase> _state);
public:
	void SetPlayer(const weak_ptr<AActor>& _player) { player = _player; }
public:
	// FX
	void PlayBloodBurst(const Vec3& _origin, const Vec3& _direction, float _speed, float _spreadAngleDeg, int _minCount = 5, int _maxCount = 10);
	void VisitAllMeshMaterials(shared_ptr<UMeshComponent> comp);
	void ApplyHitFlashToAllMaterials(shared_ptr<UMeshComponent> comp, float value);
	void Flashing();
	// Movement
	void LerpRotate();
};

