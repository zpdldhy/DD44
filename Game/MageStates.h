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
	Vec3 targetPos = Vec3();
	Vec3 dir = Vec3();
	float targetYaw = 0.f;

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
	bool bStaticMage = false;
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
	void CheckStatic(bool _b) { bStaticMage = _b; }
};

class MageHitState : public StateBase
{
private:
	weak_ptr<AActor> m_pOwner;
	Vec3 dir = Vec3();
	bool bStaticMage = false;
	float runElapsed = 0.0f;
	float rotateElapsed = 0.0f;
	float targetYaw = 0.f;
	bool bMove = true;		// 초기화 추가
public:
	MageHitState(weak_ptr<AActor> _pOwner);
	~MageHitState() {}
public:
	virtual void Enter() override;
	virtual void Tick() override;
	virtual void End() override;
public:
	void SetDirection(Vec3 _playerPos);
	void CheckStatic(bool _b) { bStaticMage = _b; }
};

class MageAttackState : public StateBase
{
private:
	weak_ptr<AActor> m_pOwner;
	weak_ptr<AActor> m_pTarget;
	// 이동
	bool bStaticMage = false;
	Vec3 dir = Vec3();
	float targetYaw = 0.f;
	// teleport 관련 변수
	Vec3 destination = Vec3();
	float disElapsed = 0.0f;
	float waitElapsed = 0.0f;
	// anim 관련 변수
	float originSpped = 0.f;
	// sub-state
	enum AttackPhase { Attack, Runaway, Disappear, Wait, StandStill, Appear, Done };
	AttackPhase currentPhase = AttackPhase::Attack;
	shared_ptr<class MageAttackStart> attack = nullptr;
	shared_ptr<class MageRunaway> runaway = nullptr;
	shared_ptr<class MageDisappearState> disappear = nullptr;
	shared_ptr<class MageAppearState> appear = nullptr;

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
	void CheckStatic(bool _s) { bStaticMage = _s; }
};

class MageAttackStart : public StateBase
{
private:
	weak_ptr<AActor> m_pOwner;
	weak_ptr<AActor> m_pTarget;

	// 이동
	Vec3 dir = Vec3();
	float targetYaw = 0.f;

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
	Vec3 dir = Vec3();
	float targetYaw = 0.f;
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
	weak_ptr<AActor> m_pPlayer;
public:
	MageDieState(weak_ptr<AActor> _pOwner);
	~MageDieState() {}
public:
	virtual void Enter() override;
	virtual void Tick() override;
	void ApplyDissolveToAllMaterials(shared_ptr<class UMeshComponent> _comp, float _time);
	virtual void End() override;

	void SetPlayer(const weak_ptr<AActor>& _player) { m_pPlayer = _player; }
};