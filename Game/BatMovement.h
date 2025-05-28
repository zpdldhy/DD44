#pragma once
#include "UScriptComponent.h"
#include "BatMovement.h"
#include "BatStates.h"

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
	float m_fRadius = 8.0f;
	float angle = 0.0f; 
	float angularSpeed = 1.0f;
	float rotateSpeed = 0.5f;
	float m_fRotationSpeed = 8.0f;

	// 
	bool m_bReturn;
	Vec3 m_vReturnPos;
	// States 
	shared_ptr<StateBase> idle;
	shared_ptr<StateBase> walk;
	shared_ptr<StateBase> attack;
	shared_ptr<StateBase> death;
	shared_ptr<StateBase> currentState;


public:
	void Init() override;
	void Tick() override;
public:
	void ChangetState(shared_ptr<StateBase> _state);
	void ReturningToPos();
	void PlayBloodBurst(const Vec3& _origin, const Vec3& _direction, float _speed, float _spreadAngleDeg, int _minCount = 5, int _maxCount = 10);
};