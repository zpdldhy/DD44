#pragma once
#include "EnemyScriptBase.h"
#include "MageStates.h"

class TEnemy;

class MageMovement : public EnemyScriptBase
{
public:
	MageMovement() = default;
	~MageMovement() = default;
private:
	bool bStaticMage = false;

	// States
	shared_ptr<StateBase> idle;
	shared_ptr<StateBase> appear;
	shared_ptr<StateBase> disappear;
	shared_ptr<StateBase> runaway;
	shared_ptr<StateBase> walk;
	shared_ptr<StateBase> attack;
	shared_ptr<StateBase> hit;
	shared_ptr<StateBase> death;

	// attackOffset
	float findDistance = 30.0f;
	float hitElapsed = 0.0f;
	// FX
	float m_fHitFlashTimer = 0.0f;
	bool m_bIsFlashing = false;

public:
	virtual void Init() override;
	virtual void Tick() override;
	virtual shared_ptr<UScriptComponent> Clone() override;
public:
	void Rotate();
public:
	void CheckHit();
	void CheckAttack();
};

