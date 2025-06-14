#pragma once
#include "UScriptComponent.h"
#include "BatMovement.h"
#include "BatStates.h"

class UMeshComponent;
class AActor;
class TEnemy;

class BatMovement : public UScriptComponent
{
public:
	BatMovement() = default;
	virtual ~BatMovement() = default;
private:
	weak_ptr<AActor> player;
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

public:
	void Init() override;
	void Tick() override;
	virtual shared_ptr<UScriptComponent> Clone() override;
	void SetPlayer(const weak_ptr<AActor>& _player) { player = _player; }
public:
	void ChangetState(shared_ptr<StateBase> _state);
	void ReturningToPos();
	void Attack();
	void CheckHit();
};