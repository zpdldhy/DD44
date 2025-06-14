#pragma once
#include "StateBase.h"
class AActor;
class UMeshComponent;

enum PLAYER_STATE
{
	PLAYER_S_IDLE = 0,
	PLAYER_S_WALK,
	PLAYER_S_CLIMB,
	PLAYER_S_ROLL,
	PLAYER_S_ATTACK,
	PLAYER_S_SHOOT,
	PLAYER_S_HIT,
	PLAYER_S_DEATH,
	PLAYER_S_GETITEM,
	PLAYER_S_COUNT
};

class PlayerIdleState : public StateBase
{
private:
	weak_ptr<AActor> m_pOwner;
public:
	PlayerIdleState(weak_ptr<AActor> _pOwner);
	~PlayerIdleState() {}
public:
	virtual void Enter() override;
	virtual void Tick() override;
	virtual void End() override;
public:

};

class PlayerWalkState : public StateBase
{
private:
	weak_ptr<AActor> m_pOwner;
public:
	PlayerWalkState(weak_ptr<AActor> _pOwner);
	~PlayerWalkState() {}
public:
	virtual void Enter() override;
	virtual void Tick() override;
	virtual void End() override;
public:
	float m_fDustTimer = 0.0f;

};

class PlayerRollState : public StateBase
{
private:
	weak_ptr<AActor> m_pOwner;
public:
	PlayerRollState(weak_ptr<AActor> _pOwner);
	~PlayerRollState() {}
public:
	virtual void Enter() override;
	virtual void Tick() override;
	virtual void End() override;
public:
	int m_iDustSpawnCount = 0;
	float m_fDustSpawnTimer = 0.0f;
};

class PlayerAttackState : public StateBase
{
private:
	weak_ptr<AActor> m_pOwner;

	shared_ptr<StateBase>* m_pCurrentState = nullptr;
	shared_ptr<StateBase> m_pPrevState;

	shared_ptr<UMeshComponent> m_pSword;
	shared_ptr<UMeshComponent> m_pBackSocket;
	shared_ptr<UMeshComponent> m_pHandSocket;

	Vec3 handSwordRot = Vec3(0.0f, 0.0f, DD_PI / 2);
	Vec3 handSwordPos = Vec3(1.0f, 0.0f, 0.0f);

	Vec3 backSwordRot = Vec3(0.0f, 0.3f, -DD_PI / 2);
	Vec3 backSwordPos = Vec3(-0.5f, 0.0f, 0.0f);

public:
	PlayerAttackState(weak_ptr<AActor> _pOwner);
	~PlayerAttackState() {}
public:
	virtual void Enter() override;
	virtual void Tick() override;
	virtual void End() override;
public:
	void SetPrevState(const shared_ptr<StateBase>& _prevState) { m_pPrevState = _prevState; }
	void SetCurrentState(shared_ptr<StateBase>* _currentState) { m_pCurrentState = _currentState; }
	// sword, hand, back 순서
	void SetComponent(shared_ptr<UMeshComponent> _sword, shared_ptr<UMeshComponent> _hand, shared_ptr<UMeshComponent> _back);
};

class PlayerShootState : public StateBase
{
private:
	weak_ptr<AActor> m_pOwner;
	weak_ptr<AActor> m_pTarget;
	//
	Vec3 dir;
	// sub-state
	bool bEnd = false;
	bool bCanShoot = true;
	enum ShootPhase { Start, Aiming, Shoot, Done };
	ShootPhase currentPhase = ShootPhase::Start;
	shared_ptr<class PlayerShootStart> start;
	shared_ptr<class PlayerShoot> shoot;
public:
	PlayerShootState(weak_ptr<AActor> _pOwner);
	~PlayerShootState() {}
public:
	virtual void Enter() override;
	virtual void Tick() override;
	virtual void End() override;
public:
	void SetTarget(weak_ptr<AActor> _pTarget) { m_pTarget = _pTarget; }
	void Rotate();
	void CheckMouse();
	void CheckEnd(bool _end) { bEnd = _end; }
	void CheckShootCount(bool _able);
};

#pragma region shoot-substate
class PlayerShootStart : public StateBase
{
private:
	weak_ptr<AActor> m_pOwner;
public:
	float elapsed = 0.0f;
public:
	PlayerShootStart(weak_ptr<AActor> _pOwner);
	~PlayerShootStart() {}
public:
	virtual void Enter() override;
	virtual void Tick() override;
	virtual void End() override;
};

class PlayerShoot : public StateBase
{
private:
	weak_ptr<AActor> m_pOwner;
	bool bCanShoot; 
public:
	PlayerShoot(weak_ptr<AActor> _pOwner);
	~PlayerShoot() {}
public:
	virtual void Enter() override;
	virtual void Tick() override;
	virtual void End() override;
public:
	void CheckShootCount(bool _able);
	void CanShoot();
};

#pragma endregion

class PlayerHitState : public StateBase
{
private:
	weak_ptr<AActor> m_pOwner;
	bool m_bPlayNext = true;

public:
	PlayerHitState(weak_ptr<AActor> _pOwner);
	~PlayerHitState() {}

public:
	virtual void Enter() override;
	virtual void Tick() override;
	virtual void End() override;
};

class PlayerDieState : public StateBase
{
private:
	weak_ptr<AActor> m_pOwner;
	bool m_bPlayNext = true;
	int animIndex;
public:
	PlayerDieState(weak_ptr<AActor> _pOwner);
	~PlayerDieState() {}

public:
	virtual void Enter() override;
	virtual void Tick() override;
	virtual void End() override;
};

class PlayerClimbState : public StateBase
{
private:
	weak_ptr<AActor> m_pOwner;
	enum ClimbPhase { Playing, Finish, Done };
	ClimbPhase currentPhase = ClimbPhase::Playing;
	// move랑 stop은 phase 안에서만 해도 조절 가능
	shared_ptr<class PlayerClimbFinish> finish;
	bool isFinish = false;
	bool isMoving = true;
	Vec3 ladderDir;
public:
	PlayerClimbState(weak_ptr<AActor> _pOwner);
	~PlayerClimbState() {}

public:
	virtual void Enter() override;
	virtual void Tick() override;
	virtual void End() override;
public:
	void CheckClimbFinish(bool _finish) { isFinish = _finish; }
	void CheckMove(bool _isMoving);
	void SetLadderDir(Vec3 _dir);
	void CheckIsMoving(bool _moving) { isMoving = _moving; }
};

class PlayerClimbFinish : public StateBase
{
private:
	weak_ptr<AActor> m_pOwner;
public:
	PlayerClimbFinish(weak_ptr<AActor> _pOwner);
	~PlayerClimbFinish() {}
public:
	Vec3 ladderDir;
public:
	virtual void Enter() override;
	virtual void Tick() override;
	virtual void End() override;
public:
	void SetLadderDir(Vec3 _dir);
};

