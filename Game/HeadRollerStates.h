#pragma once
#include "StateBase.h"
#include "EnemyState.h"

class AActor;

class HeadRollerIdleState : public StateBase
{
private:
	weak_ptr<AActor> m_pOwner;
public:
	HeadRollerIdleState(weak_ptr<AActor> _pOwner);
	~HeadRollerIdleState() {}
public:
	virtual void Enter() override;
	virtual void Tick() override;
	virtual void End() override;
};

class HeadRollerAttackState : public StateBase
{
private:
	weak_ptr<AActor> m_pOwner;

	// sub-state
	enum RollPhase { Find, Start, Middle, Final, Done };
	RollPhase currentPhase = RollPhase::Find;
	
	shared_ptr<class HeadRollerRollFind> find;
	shared_ptr<class HeadRollerRollStart> start;
	shared_ptr<class HeadRollerRollMiddle> middle;
	shared_ptr<class HeadRollerRollEnd> end;

	// 
	Vec3 dir;
	float rollElapsed = 0.0f;
	bool bHitWall = false;
public:
	HeadRollerAttackState(weak_ptr<AActor> _pOwner);
	~HeadRollerAttackState() {}
public:
	virtual void Enter() override;
	virtual void Tick() override;
	virtual void End() override;
public:
	void SetDirection(Vec3 _dir) { dir = _dir; }
	bool CheckWallCollision();
};

#pragma region RollSubState
class HeadRollerRollFind : public StateBase
{
private:
	weak_ptr<AActor> m_pOwner;
	Vec3 dir;
	float targetYaw;
public:
	HeadRollerRollFind(weak_ptr<AActor> _pOwner);
	~HeadRollerRollFind() {}
public:
	virtual void Enter() override;
	virtual void Tick() override;
	virtual void End() override;
public:
	void SetDirection(Vec3 _dir) { dir = _dir; }

};
class HeadRollerRollStart : public StateBase
{
private:
	weak_ptr<AActor> m_pOwner;
	Vec3 dir;
public:
	HeadRollerRollStart(weak_ptr<AActor> _pOwner);
	~HeadRollerRollStart() {}
public:
	virtual void Enter() override;
	virtual void Tick() override;
	virtual void End() override;
public:
	void SetDirection(Vec3 _dir) { dir = _dir; }
};

class HeadRollerRollMiddle : public StateBase
{
private:
	weak_ptr<AActor> m_pOwner;
	Vec3 dir;
public:
	HeadRollerRollMiddle(weak_ptr<AActor> _pOwner);
	~HeadRollerRollMiddle() {}
public:
	virtual void Enter() override;
	virtual void Tick() override;
	virtual void End() override;
public:
	void SetDirection(Vec3 _dir) { dir = _dir; }
};

class HeadRollerRollEnd : public StateBase
{
private:
	weak_ptr<AActor> m_pOwner;
	Vec3 dir;
public:
	HeadRollerRollEnd(weak_ptr<AActor> _pOwner);
	~HeadRollerRollEnd() {}
public:
	virtual void Enter() override;
	virtual void Tick() override;
	virtual void End() override;
public:
	void SetDirection(Vec3 _dir) { dir = _dir; }
};
#pragma endregion

class HeadRollerLookState : public StateBase
{
private:
	weak_ptr<AActor> m_pOwner;
public:
	HeadRollerLookState(weak_ptr<AActor> _pOwner);
	~HeadRollerLookState() {}
public:
	virtual void Enter() override;
	virtual void Tick() override;
	virtual void End() override;
};

class HeadRollerDieState : public StateBase
{
private:
	weak_ptr<AActor> m_pOwner;
public:
	HeadRollerDieState(weak_ptr<AActor> _pOwner);
	~HeadRollerDieState() {}

public:
	virtual void Enter() override;
	virtual void Tick() override;
	virtual void End() override;
	
	void ApplyDissolveToAllMaterials(shared_ptr<class UMeshComponent> _comp, float _time);
	void SetMeshInvisible();
};