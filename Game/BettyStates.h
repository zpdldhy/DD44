#pragma once
#include "StateBase.h"

class AActor;

enum BETTY_STATE
{
	BETTY_S_IDLE = 0,
	BETTY_S_INTRO,
	BETTY_S_A_JUMPHIGH,
	BETTY_S_A_TWOHAND,
	BETTY_S_A_HANDBACK,
	BETTY_S_A_HANDDOWN,
	BETTY_S_A_ROLL,
	BETTY_S_A_THROWSNOW,
	BETTY_S_A_DROPSNOW,
	BETTY_S_A_ROAR,
	BETTY_S_DEATH,
	BETTY_S_COUNT
};

class JumpMotionHandler
{
public:
	enum class JumpPhase
	{
		Ascending, Hanging, Falling, Done
	};
	JumpMotionHandler() = default;
	~JumpMotionHandler(){}
private:
	Vec3 start = Vec3();
	Vec3 end = Vec3();
	Vec3 prevPos = Vec3();
	Vec3 velocity = Vec3();
	float height = 0.f;
	float gravity = 0.f;
	float fall = 0.f;
	float totalTime = 0.f;
	float elapsed = 0.f;
	float hangDuration = 0.f;
	float hangElapsed = 0.0f;

	bool bDone = false;
	JumpPhase phase = JumpPhase::Ascending;
public:
	void Init(Vec3 _start, Vec3 _end, float _height, float _total, float _hangDuration, float _fall = 1.8f);
	Vec3 Update();
	bool IsDone() { return bDone; }
};

class BettyStateBase : public StateBase
{
public:
	BettyStateBase(UINT _iStateId);
	~BettyStateBase() {}
protected:
	weak_ptr<AActor> m_pOwner;
public:
	// 공격 범위들
	weak_ptr<AActor> leftRange;
	weak_ptr<AActor> rightRange;
	bool bAnimStop = false;

	// 후딜
	float originAnimSpeed;
	float elapsed = 0.0f;
	float offset = 0.3f;
public:
	void SetRange(const shared_ptr<AActor>& left, const shared_ptr<AActor>& right);
	void EnableCollider();
};

class BettyIdleState : public BettyStateBase
{
public:
	BettyIdleState(weak_ptr<AActor> _pOwner);
	~BettyIdleState() {}
public:
	virtual void Enter() override;
	virtual void Tick() override;
	virtual void End() override;
};

class BettyIntroState : public BettyStateBase
{
	bool m_bNextAnim = true;
public:
	BettyIntroState(weak_ptr<AActor> _pOwner);
	~BettyIntroState() {}
public:
	virtual void Enter() override;
	virtual void Tick() override;
	virtual void End() override;
public:
	void PlayFx();
};

class BettyJumpAttack : public BettyStateBase
{	
	// MOVEMENT
	//// JUMP
	Vec3 targetPos = Vec3();
	float posOffset = 5.0f;
	JumpMotionHandler jumpHandler;
	//// Rotate
	float targetYaw = 0.f;
	//// Anim
	bool bControlOnce = true;
	float originAnimSpeed = 0.f;
public:
	BettyJumpAttack(weak_ptr<AActor> _pOwner);
	~BettyJumpAttack() {}
public:
	virtual void Enter() override;
	virtual void Tick() override;
	virtual void End() override;
public:
	void SetTargetPos(Vec3 _pos) { targetPos = _pos; }
public:
	void PlayHandFx();
	void PlayBodyFx();
};

class BettyTwoHandAttack : public BettyStateBase
{
public:
	BettyTwoHandAttack(weak_ptr<AActor> _pOwner);
	~BettyTwoHandAttack() {}
public:
	virtual void Enter() override;
	virtual void Tick() override;
	virtual void End() override;
private:
	void StartCol();
	void EndAnim();

};

class BettyOneHandBackAttack : public BettyStateBase
{
	// 
	bool bRight = true;
	//// Rotate
	float targetYaw = 0.f;
public:
	BettyOneHandBackAttack(weak_ptr<AActor> _pOwner);
	~BettyOneHandBackAttack() {}
public:
	virtual void Enter() override;
	virtual void Tick() override;
	virtual void End() override;
public:
	bool CheckDirection(Vec3 _targetPos);
	void StartCol();
};

class BettyOneHandDownAttack : public BettyStateBase
{
	// 
	bool bRight = true;
	//// Rotate
	float targetYaw = 0.f;
public:
	BettyOneHandDownAttack(weak_ptr<AActor> _pOwner);
	~BettyOneHandDownAttack() {}
public:
	virtual void Enter() override;
	virtual void Tick() override;
	virtual void End() override;
public:
	bool CheckDirection(Vec3 _targetPos);
	void StartCol();
};

class BettyRollAttack : public BettyStateBase
{
private:
	weak_ptr<AActor> m_pTarget;
	// sub-state
	enum RollPhase { Start, Middle, Final, Done };
	RollPhase currentPhase = RollPhase::Start;
	shared_ptr<class BettyRollStart> start;
	shared_ptr<class BettyRollMiddle> middle;
	shared_ptr<class BettyRollEnd> end;
	// 
	int rollCount = 3;
	float rollElapsed = 0.0f;
	bool bHitWall = false;
	Vec3 dir;
public:
	BettyRollAttack(weak_ptr<AActor> _pOwner);
	~BettyRollAttack() {}
public:
	virtual void Enter() override;
	virtual void Tick() override;
	virtual void End() override;
public:
	void SetTarget(weak_ptr<AActor> _target);
	void PlayFx();
};
#pragma region RollSubState
class BettyRollStart : public StateBase
{
private:
	weak_ptr<AActor> m_pOwner;
	Vec3 dir;
	// jump motion
	float posOffset = 15.0f;
	JumpMotionHandler jumpHandler;
	float targetYaw = 0.f;
public:
	BettyRollStart(weak_ptr<AActor> _pOwner);
	~BettyRollStart() {}
public:
	virtual void Enter() override;
	virtual void Tick() override;
	virtual void End() override;
public:
	void SetDirection(Vec3 _dir) { dir = _dir; }
};
class BettyRollMiddle : public StateBase
{
private:
	weak_ptr<AActor> m_pOwner;
	Vec3 dir;
public:
	BettyRollMiddle(weak_ptr<AActor> _pOwner);
	~BettyRollMiddle() {}
public:
	virtual void Enter() override;
	virtual void Tick() override;
	virtual void End() override;
public:
	void SetDirection(Vec3 _dir) { dir = _dir; }
	bool CheckWallCollision();

};
class BettyRollEnd : public StateBase
{
private:
	weak_ptr<AActor> m_pOwner;
	Vec3 dir = Vec3();
	// jump motion
	float posOffset = 10.0f;
	JumpMotionHandler jumpHandler = JumpMotionHandler();
	float targetYaw = 0.f;

public:
	BettyRollEnd(weak_ptr<AActor> _pOwner);
	~BettyRollEnd() {}
public:
	virtual void Enter() override;
	virtual void Tick() override;
	virtual void End() override;
public:
	void SetDirection(Vec3 _dir) { dir = _dir; }

};
#pragma endregion

class BettyDropAttack : public BettyStateBase
{
public:
	BettyDropAttack(weak_ptr<AActor> _pOwner);
	~BettyDropAttack() {}
public:
	virtual void Enter() override;
	virtual void Tick() override;
	virtual void End() override;
};

class BettyRoarAttack : public BettyStateBase
{
public:
	BettyRoarAttack(weak_ptr<AActor> _pOwner);
	~BettyRoarAttack() {}
public:
	virtual void Enter() override;
	virtual void Tick() override;
	virtual void End() override;
};

class BettyDeathState : public BettyStateBase
{
public:
	BettyDeathState(weak_ptr<AActor> _pOwner);
	~BettyDeathState() {}
public:
	virtual void Enter() override;
	virtual void Tick() override;
	virtual void End() override;
};