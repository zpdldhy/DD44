#pragma once
#include "UScriptComponent.h"

class StateBase;
class TPlayer;

class EnemyScriptBase : public UScriptComponent
{
public:
	EnemyScriptBase() = default;
	virtual ~EnemyScriptBase() = default;
protected:
	weak_ptr<TPlayer> player;

	// State
	shared_ptr<StateBase> currentState;
	UINT currentStateId;

	// Common Movement
	float m_targetYaw;
	float m_fRotationSpeed = 5.0f;
public:
	virtual void ChangeState(shared_ptr<StateBase> _state);
	void SetPlayer(const weak_ptr<TPlayer>& _player) { player = _player; }
public:
	bool LerpRotate(float _targetYaw);
};

