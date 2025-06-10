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
	Vec3 start;
	Vec3 end;
	Vec3 prevPos;
	Vec3 velocity;
	float height;
	float gravity;
	float fall;
	float totalTime;
	float elapsed;
	float hangDuration;
	float hangElapsed = 0.0f;

	bool bDone = false;
	JumpPhase phase = JumpPhase::Ascending;
public:
	void Init(Vec3 _start, Vec3 _end, float _height, float _total, float _hangDuration, float _fall = 1.8f);
	Vec3 Update();
	bool IsDone() { return bDone; }
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

class BettyJumpAttack : public StateBase
{
private:
	weak_ptr<AActor> m_pOwner;
	
	// MOVEMENT
	//// JUMP
	Vec3 targetPos;
	float posOffset = 5.0f;
	JumpMotionHandler jumpHandler;
	//// Rotate
	float targetYaw;
	//// Anim
	bool bControlOnce = true;
	float originAnimSpeed;
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
	void SlowAnimSpeed();
	void NormalizeAnimSpeed();
};

class BettyTwoHandAttack : public StateBase
{
private:
	weak_ptr<AActor> m_pOwner;
public:
	BettyTwoHandAttack(weak_ptr<AActor> _pOwner);
	~BettyTwoHandAttack() {}
public:
	virtual void Enter() override;
	virtual void Tick() override;
	virtual void End() override;
};

class BettyOneHandBackAttack : public StateBase
{
private:
	weak_ptr<AActor> m_pOwner;
	// 
	bool bRight = true;
	//// Rotate
	float targetYaw;
public:
	BettyOneHandBackAttack(weak_ptr<AActor> _pOwner);
	~BettyOneHandBackAttack() {}
public:
	virtual void Enter() override;
	virtual void Tick() override;
	virtual void End() override;
public:
	bool CheckDirection(Vec3 _targetPos);
};

class BettyOneHandDownAttack : public StateBase
{
private:
	weak_ptr<AActor> m_pOwner;
	// 
	bool bRight = true;
	//// Rotate
	float targetYaw;
public:
	BettyOneHandDownAttack(weak_ptr<AActor> _pOwner);
	~BettyOneHandDownAttack() {}
public:
	virtual void Enter() override;
	virtual void Tick() override;
	virtual void End() override;
public:
	bool CheckDirection(Vec3 _targetPos);
};

class BettyRollAttack : public StateBase
{
private:
	weak_ptr<AActor> m_pOwner;
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
	float targetYaw;
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
	Vec3 dir;
	// jump motion
	float posOffset = 10.0f;
	JumpMotionHandler jumpHandler;
	float targetYaw;

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

class BettyDropAttack : public StateBase
{
private:
	weak_ptr<AActor> m_pOwner;
public:
	BettyDropAttack(weak_ptr<AActor> _pOwner);
	~BettyDropAttack() {}
public:
	virtual void Enter() override;
	virtual void Tick() override;
	virtual void End() override;
};

class BettyRoarAttack : public StateBase
{
private:
	weak_ptr<AActor> m_pOwner;
public:
	BettyRoarAttack(weak_ptr<AActor> _pOwner);
	~BettyRoarAttack() {}
public:
	virtual void Enter() override;
	virtual void Tick() override;
	virtual void End() override;
};

class BettyDeathState : public StateBase
{
private:
	weak_ptr<AActor> m_pOwner;
public:
	BettyDeathState(weak_ptr<AActor> _pOwner);
	~BettyDeathState() {}
public:
	virtual void Enter() override;
	virtual void Tick() override;
	virtual void End() override;
};