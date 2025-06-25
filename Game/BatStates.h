#pragma once
#include "StateBase.h"
#include "EnemyState.h"

class AActor;

class BatIdleState : public StateBase
{
private:
	weak_ptr<AActor> m_pOwner;
public:
	BatIdleState(weak_ptr<AActor> _pOwner);
	~BatIdleState() {}
public:
	virtual void Enter() override;
	virtual void Tick() override;
	virtual void End() override;
};

class BatWalkState : public StateBase
{
private:
	weak_ptr<AActor> m_pOwner;
public:
	BatWalkState(weak_ptr<AActor> _pOwner);
	~BatWalkState() {}
public:
	virtual void Enter() override;
	virtual void Tick() override;
	virtual void End() override;
};

class BatAttackState : public StateBase
{
private:
	weak_ptr<AActor> m_pOwner;
public:
	BatAttackState(weak_ptr<AActor> _pOwner);
	~BatAttackState() {}
public:
	virtual void Enter() override;
	virtual void Tick() override;
	virtual void End() override;
};

class BatDieState : public StateBase
{
private:
	weak_ptr<AActor> m_pOwner;
	weak_ptr<AActor> m_pPlayer;
public:
	BatDieState(weak_ptr<AActor> _pOwner);
	~BatDieState() {}
	
public:
	virtual void Enter() override;
	virtual void Tick() override;
	void ApplyDissolveToAllMaterials(shared_ptr<class UMeshComponent> _comp, float _time);
	virtual void End() override;

	void SetPlayer(const weak_ptr<AActor>& _player) { m_pPlayer = _player; }
};