#pragma once
#include "UScriptComponent.h"
#include "BatMovement.h"
#include "BatStates.h"

class UMeshComponent;
class AActor;

class BatMovement : public UScriptComponent
{
public:
	BatMovement() = default;
	virtual ~BatMovement() = default;
public:
	Vec3 m_vCenter;
	Vec3 m_vTargetPos;
	Vec3 m_vPrevPos;
	float m_fLerp = -0.05;
	float m_fLerpOffset = 0.005f;
	float m_fRadius = 5.0f;
	float angle = 0.0f; 
	float angularSpeed = 1.0f;
	float rotateSpeed = 0.35f;
	float originRotateSpeed = 0.35f;
	float m_fRotationSpeed = 8.0f;
	bool m_bCanStartAttack = false;
	//
	bool m_bClockWise = false;
	//
	Vec3 m_vLastMoveDir;
	Vec3 velocity;
	// 
	bool m_bReturn;
	Vec3 m_vReturnPos;
	bool m_bStopAllMove = false;
	//
	shared_ptr<AActor> attackRangeActor;
	Vec3 colOffset;

	// TEMP
	bool m_bMove = false;

	// States 
	shared_ptr<StateBase> idle;
	shared_ptr<StateBase> walk;
	shared_ptr<StateBase> attack;
	shared_ptr<StateBase> death;
	shared_ptr<StateBase> currentState;

	weak_ptr<AActor> player;
public:
	void Init() override;
	void Tick() override;
public:
	void ChangetState(shared_ptr<StateBase> _state);
	void ReturningToPos();
	void PlayBloodBurst(const Vec3& _origin, const Vec3& _direction, float _speed, float _spreadAngleDeg, int _minCount = 5, int _maxCount = 10);
	void VisitAllMeshMaterials(shared_ptr<UMeshComponent> comp);
	float m_fHitFlashTimer = 0.0f;
	bool m_bIsFlashing = false;
	void ApplyHitFlashToAllMaterials(shared_ptr<UMeshComponent> comp, float value);
	void Flashing();
	void Attack();
	void SetPlayer(const weak_ptr<AActor>& _player) { player = _player; }
	void CheckHit();
};