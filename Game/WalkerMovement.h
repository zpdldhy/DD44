#pragma once
#include "UScriptComponent.h"
#include "WalkerStates.h"

class TPlayer;
class TEnemy;

class WalkerMovement : public UScriptComponent
{
public:
	WalkerMovement() = default;
	virtual ~WalkerMovement() = default;
private:
	shared_ptr<TEnemy> owner;
	weak_ptr<TPlayer> player;
	// States 
	shared_ptr<StateBase> idle;
	shared_ptr<StateBase> walk;
	shared_ptr<StateBase> hit;
	shared_ptr<StateBase> death;
	shared_ptr<StateBase> currentState;

private:
	// Movement
	float m_wait;
	bool m_bWait;
	Vec3 m_vPos[4];
	Vec3 m_vCurrentTarget;
	int m_currentPosIdx;
	float m_half = 5;
	Vec3 m_vDistance;
	float m_walkSpeed = 0.006;
	float m_fRotationSpeed = 5.0f;
	bool m_rotate;
	Vec3 moveDir;
public:
	void Init() override;
	void Tick() override;
	virtual shared_ptr<UScriptComponent> Clone() override;
public:
	void ChangeState(shared_ptr<StateBase> _state);
public:
	void SetPlayer(const weak_ptr<TPlayer>& _player) { player = _player; }

public:
	// Movement
	void LerpRotate();
	void CheckHit();
};

