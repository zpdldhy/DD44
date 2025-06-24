#pragma once
#include "TCharacter.h"

class TInteractScriptComponent;

class TInteractable : public TCharacter
{
public:
	TInteractable() = default;
	virtual ~TInteractable() = default;
public:
	virtual void Init() override;
	virtual void Tick() override;
public:
	void Interact();
private:
	shared_ptr<TInteractScriptComponent> interactionScript;
};

