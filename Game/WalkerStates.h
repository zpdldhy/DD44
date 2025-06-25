#pragma once
#include "StateBase.h"
#include "EnemyState.h"

class AActor;

class WalkerIdleState : public StateBase
{
private:
	weak_ptr<AActor> m_pOwner;
public:
	WalkerIdleState(weak_ptr<AActor> _pOwner);
	~WalkerIdleState() {}
public:
	virtual void Enter() override;
	virtual void Tick() override;
	virtual void End() override;
};


class WalkerWalkState : public StateBase
{
private:
	weak_ptr<AActor> m_pOwner;
public:
	WalkerWalkState(weak_ptr<AActor> _pOwner);
	~WalkerWalkState() {}
public:
	virtual void Enter() override;
	virtual void Tick() override;
	virtual void End() override;
};

class WalkerHitState : public StateBase
{
private:
	weak_ptr<AActor> m_pOwner;
public:
	WalkerHitState(weak_ptr<AActor> _pOwner);
	~WalkerHitState() {}
public:
	virtual void Enter() override;
	virtual void Tick() override;
	virtual void End() override;
};

class WalkerDieState : public StateBase
{
private:
	weak_ptr<AActor> m_pOwner;
	weak_ptr<AActor> player;
public:
	WalkerDieState(weak_ptr<AActor> _pOwner);
	~WalkerDieState() {}
	
public:
	virtual void Enter() override;
	virtual void Tick() override;
	void ApplyDissolveToAllMaterials(shared_ptr<class UMeshComponent> _comp, float _time);
	virtual void End() override;

};