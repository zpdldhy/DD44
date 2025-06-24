#pragma once
#include "StateBase.h"
#include "EnemyState.h"

class AActor;

class MageIdleState : public StateBase
{
private:
	weak_ptr<AActor> m_pOwner;

	// temp 
	int animIndex = 0;
public:
	MageIdleState(weak_ptr<AActor> _pOwner);
	~MageIdleState() {}
public:
	virtual void Enter() override;
	virtual void Tick() override;
	virtual void End() override;
};

class MageAppearState : public StateBase
{
private:
	weak_ptr<AActor> m_pOwner;
public:
	// Scaling
	Vec3 increment = { 0.02f, 0.01f, 0.02f };
	Vec3 maxV = { 1.0f, 1.0f, 1.0f };
	Vec3 minV = { 0.01f, 0.01f, 0.01f };
	// Rotation
	Vec3 targetPos;
	Vec3 dir;
	float targetYaw;

public:
	MageAppearState(weak_ptr<AActor> _pOwner);
	~MageAppearState() {}
public:
	virtual void Enter() override;
	virtual void Tick() override;
	virtual void End() override;
public:
	void SetDirection(Vec3 _targetPos);
private:
	void SetMeshVisible();
};

class MageDisappearState : public StateBase
{
private:
	weak_ptr<AActor> m_pOwner;
public:
	// Scaling
	float lowerElapsed = 0.0f;
	Vec3 increment = { 0.01f, 0.004f, 0.01f };
	Vec3 maxV = { 1.0f, 1.0f, 1.0f };
	Vec3 minV = { 0.01f, 0.01f, 0.01f };

public:
	MageDisappearState(weak_ptr<AActor> _pOwner);
	~MageDisappearState() {}
public:
	virtual void Enter() override;
	virtual void Tick() override;
	virtual void End() override;
private:
	void SetAllMeshInvisible(const shared_ptr<class UMeshComponent>& _mesh);
};

class MageHitState : public StateBase
{
private:
	weak_ptr<AActor> m_pOwner;
	Vec3 dir;
	float runElapsed = 0.0f;
	float rotateElapsed = 0.0f;
	float targetYaw;
	bool bMove;
public:
	MageHitState(weak_ptr<AActor> _pOwner);
	~MageHitState() {}
public:
	virtual void Enter() override;
	virtual void Tick() override;
	virtual void End() override;
public:
	void SetDirection(Vec3 _playerPos);
};

class MageAttackState : public StateBase
{
private:
	weak_ptr<AActor> m_pOwner;
	weak_ptr<AActor> m_pTarget;
	// 이동
	Vec3 dir;
	float targetYaw;
	// teleport 관련 변수
	Vec3 destination;
	float disElapsed = 0.0f;
	// sub-state
	enum AttackPhase { Attack, Runaway, Disappear, Wait, Appear, Done };
	AttackPhase currentPhase = AttackPhase::Attack;
	shared_ptr<class MageAttackStart> attack;
	shared_ptr<class MageRunaway> runaway;
	shared_ptr<class MageDisappearState> disappear;
	shared_ptr<class MageAppearState> appear;

public:
	MageAttackState(weak_ptr<AActor> _pOwner);
	~MageAttackState() {}
public:
	virtual void Enter() override;
	virtual void Tick() override;
	virtual void End() override;
public:
	void SetTarget(weak_ptr<AActor> _pTarget);
	void SetDirection();
};

class MageAttackStart : public StateBase
{
private:
	weak_ptr<AActor> m_pOwner;
	weak_ptr<AActor> m_pTarget;

	// 이동
	Vec3 dir;
	float targetYaw;

public:
	MageAttackStart(weak_ptr<AActor> _pOwner);
	~MageAttackStart() {}
public:
	virtual void Enter() override;
	virtual void Tick() override;
	virtual void End() override;
public:
	void Throw();
	void SetTarget(weak_ptr<AActor> _pTarget);
	void SetDirection();
};

class MageRunaway : public StateBase
{
private:
	weak_ptr<AActor> m_pOwner;
	// 이동
	Vec3 dir;
	float targetYaw;
	float elapsed = 0.0f;

public:
	MageRunaway(weak_ptr<AActor> _pOwner);
	~MageRunaway() {}
public:
	virtual void Enter() override;
	virtual void Tick() override;
	virtual void End() override;
public:
	void SetDirection(Vec3 _targetPos);
};



class MageDieState : public StateBase
{
private:
	weak_ptr<AActor> m_pOwner;
public:
	MageDieState(weak_ptr<AActor> _pOwner);
	~MageDieState() {}
public:
	virtual void Enter() override;
	virtual void Tick() override;
	void ApplyDissolveToAllMaterials(shared_ptr<class UMeshComponent> _comp, float _time);
	virtual void End() override;

};