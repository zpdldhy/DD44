#pragma once
#include "UScriptComponent.h"
#include "MageStates.h"

class TEnemy;

class MageMovement : public UScriptComponent
{
public:
	MageMovement() = default;
	~MageMovement() = default;
private:
	weak_ptr<AActor> player;

	// States
	shared_ptr<StateBase> idle;
	shared_ptr<StateBase> appear;
	shared_ptr<StateBase> disappear;
	shared_ptr<StateBase> walk;
	shared_ptr<StateBase> attack;
	shared_ptr<StateBase> hit;
	shared_ptr<StateBase> death;
	shared_ptr<StateBase> currentState;
	UINT currentStateId;
	// attackOffset
	float findDistance = 10.0f;
	float hitElapsed = 0.0f;
	// FX
	float m_fHitFlashTimer = 0.0f;
	bool m_bIsFlashing = false;

public:
	virtual void Init() override;
	virtual void Tick() override;
	virtual shared_ptr<UScriptComponent> Clone() override;
public:
	void SetPlayer(const weak_ptr<AActor>& _player) { player = _player; }
	void ChangeState(shared_ptr<StateBase> _state);
	void Rotate();
public:
	void CheckHit();
public:
	//FX
	void Flashing();
	void ApplyHitFlashToAllMaterials(shared_ptr<UMeshComponent> comp, float value);

};

