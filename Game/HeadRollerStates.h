#pragma once
#include "StateBase.h"
#include "EnemyState.h"

class AActor;

class Enemy1IdleState : public StateBase
{
private:
	weak_ptr<AActor> m_pOwner;
public:
	Enemy1IdleState(weak_ptr<AActor> _pOwner);
	~Enemy1IdleState() {}
public:
	virtual void Enter() override;
	virtual void Tick() override;
	virtual void End() override;
};

class Enemy1WalkState : public StateBase
{
private:
	weak_ptr<AActor> m_pOwner;
public:
	Enemy1WalkState(weak_ptr<AActor> _pOwner);
	~Enemy1WalkState() {}
public:
	virtual void Enter() override;
	virtual void Tick() override;
	virtual void End() override;
};

class Enemy1AttackStartState : public StateBase
{
private:
	weak_ptr<AActor> m_pOwner;

	// script에서 돌리고 있는 current state
	shared_ptr<StateBase>* m_pCurrentState = nullptr;
	
	// 사용할 state
	shared_ptr<StateBase> m_pIdleState;
	shared_ptr<StateBase> m_pRollState;
	shared_ptr<StateBase> m_pStunState;
public:
	Enemy1AttackStartState(weak_ptr<AActor> _pOwner);
	~Enemy1AttackStartState() {}
public:
	virtual void Enter() override;
	virtual void Tick() override;
	virtual void End() override;
public:
	void SetIdleState(const shared_ptr<StateBase>& _roll);
	// 1. Roll 2. Stun
	void SetRelateState(const shared_ptr<StateBase>& _roll, const shared_ptr<StateBase>& _stun);
};

class Enemy1RollState : public StateBase
{
private:
	weak_ptr<AActor> m_pOwner;
	shared_ptr<StateBase> m_pIdleState;
public:
	Enemy1RollState(weak_ptr<AActor> _pOwner);
	~Enemy1RollState() {}
public:
	virtual void Enter() override;
	virtual void Tick() override;
	virtual void End() override;
};

class Enemy1StunState : public StateBase
{
private:
	weak_ptr<AActor> m_pOwner;

public:
	Enemy1StunState(weak_ptr<AActor> _pOwner);
	~Enemy1StunState() {}
public:
	virtual void Enter() override;
	virtual void Tick() override;
	virtual void End() override;
};