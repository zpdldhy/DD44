#pragma once
#include "EnemyScriptBase.h"
#include "HeadRollerStates.h"
#include "WalkerStates.h"

class TPlayer;

class HeadRollerMovement : public EnemyScriptBase
{
public:
	HeadRollerMovement() = default;
	virtual ~HeadRollerMovement() = default;
private:
	// States 
	shared_ptr<StateBase> empty;
	shared_ptr<StateBase> idle;
	shared_ptr<StateBase> look;
	shared_ptr<StateBase> walk;
	shared_ptr<StateBase> roll;
	shared_ptr<StateBase> hit;
	shared_ptr<StateBase> death;
	// distance
	Vec3 direction;
	float chasingOffset = 15.0f;
	// cooltime
	float attackCool = 2.0f;
	float currentACool = 0.0f;
	bool countAttackCool = false;
	bool canAttack = true;
	// movement
	float m_walkSpeed = 0.006;
	float posOffset = 3.0f;
	Vec3 m_vPos[2];
	Vec3 m_vCurrentTarget;
	int m_currentPosIdx;
	Vec3 m_vDistance;
	//// rotation
	Vec3 moveDir;
	bool m_rotate = false;

public:
	void Init() override;
	void Tick() override;
	virtual shared_ptr<UScriptComponent> Clone() override;
private:
	void InitPos();
	void Walk();
	bool CheckPlayer();
	void CheckCoolDown();
	void CheckHit();
	void AttackStart();

};

