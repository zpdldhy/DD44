#pragma once
#include "UScriptComponent.h"
#include "BettyStates.h"

class BettyMovement : public UScriptComponent
{
public:
	BettyMovement() = default;
	virtual ~BettyMovement() = default;
private:
	// States 
	shared_ptr<StateBase> idle;
	shared_ptr<StateBase> intro;
	shared_ptr<StateBase> currentState;
	
	weak_ptr<AActor> player;

	bool b = true;
public:
	void Init();
	void Tick();
public:
	void SetPlayer(const weak_ptr<AActor>& _player) { player = _player; }
	void ChangetState(shared_ptr<StateBase> _state);

};

