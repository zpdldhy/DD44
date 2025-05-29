#pragma once
#include "StateBase.h"

class AActor;

enum BETTY_STATE
{
	BETTY_S_IDLE = 0,
	BETTY_S_INTRO,
	BETTY_S_WALK,
	BETTY_S_CHASE,
	BETTY_S_ATTACK,
	BETTY_S_HIT,
	BETTY_S_DEATH,
	BETTY_S_COUNT
};

class BettyIdleState : public StateBase
{
private:
	weak_ptr<AActor> m_pOwner;
public:
	BettyIdleState(weak_ptr<AActor> _pOwner);
	~BettyIdleState() {}
public:
	virtual void Enter() override;
	virtual void Tick() override;
	virtual void End() override;
};

class BettyIntroState : public StateBase
{
private:
	weak_ptr<AActor> m_pOwner;
	bool m_bNextAnim = true;
public:
	BettyIntroState(weak_ptr<AActor> _pOwner);
	~BettyIntroState() {}
public:
	virtual void Enter() override;
	virtual void Tick() override;
	virtual void End() override;
};