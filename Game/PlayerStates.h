#pragma once
#include "StateBase.h"
class AActor;
class UMeshComponent;

enum PLAYER_STATE
{
	EMPTY_STATE = 0,
	PLAYER_S_IDLE,
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
class PlayerBaseState : public StateBase
{
public:
	PlayerBaseState(UINT _iStateId);
	virtual ~PlayerBaseState() = default;
public:
	virtual void Enter() override {};
	virtual void Tick() override {};
	virtual void End() override {};
public:
	// 구르기
	bool m_bCanRoll = true;
	float m_fRollElapsed = 0.0f;
	float m_fRollOffset = 0.5f;

	// 피격
	bool m_bCanBeHit = true;
	float m_fDamageElapsed = 0.0f;
	float m_fDamageOffset = 1.0f;
};
class PlayerEmptyState : public PlayerBaseState
{
public:
	PlayerEmptyState();
	~PlayerEmptyState() {}
public:
	virtual void Enter() override;
	virtual void Tick() override;
	virtual void End() override;

};

class PlayerIdleState : public PlayerBaseState
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

class PlayerWalkState : public PlayerBaseState
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

class PlayerRollState : public PlayerBaseState
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

class PlayerAttackState : public PlayerBaseState
{
private:
	weak_ptr<AActor> m_pOwner;
	weak_ptr<AActor> m_pAttackRange;

	enum AttackCombo { OnFirst = 0, OnSecond, OnThird, Done };
	AttackCombo currentPhase = OnFirst;
	bool m_bOnCombo = false;
	Vec3 dir;

	// FX
	// Slash 
	shared_ptr<class UMaterial> m_pSlashMaterial = nullptr;
	float m_fSlashTime = 0.0f;
	bool m_bSlashPlaying = false;
	float m_fSlashDuration = 0.3f;
	bool reverse = false;

public:
	PlayerAttackState(weak_ptr<AActor> _pOwner);
	~PlayerAttackState() {}
public:
	virtual void Enter() override;
	virtual void Tick() override;
	virtual void End() override;
public:
	void CheckAttackCombo(bool _onCombo) { m_bOnCombo = _onCombo; }
	void CheckMouse();
	void Rotate();
	void Move();
	void Slash();
public:
	void SetAttackRange(shared_ptr<AActor> _attack) { m_pAttackRange = _attack; }
};

class PlayerShootState : public PlayerBaseState
{
private:
	weak_ptr<AActor> m_pOwner;
	weak_ptr<AActor> m_pTarget;
	weak_ptr<AActor> m_pBow;
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
	void SetBow(shared_ptr<AActor> _bow) { m_pBow = _bow; }
};

#pragma region shoot-substate
class PlayerShootStart : public PlayerBaseState
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

class PlayerShoot : public PlayerBaseState
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

class PlayerHitState : public PlayerBaseState
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

class PlayerDieState : public PlayerBaseState
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

class PlayerClimbState : public PlayerBaseState
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
	int GetCurrentPhase() { return currentPhase; }
};

class PlayerClimbFinish : public PlayerBaseState
{
private:
	weak_ptr<AActor> m_pOwner;
public:
	PlayerClimbFinish(weak_ptr<AActor> _pOwner);
	~PlayerClimbFinish() {}
public:
	Vec3 ladderDir;
	float offset = 9.0f; 
public:
	virtual void Enter() override;
	virtual void Tick() override;
	virtual void End() override;
public:
	void SetLadderDir(Vec3 _dir);
};

