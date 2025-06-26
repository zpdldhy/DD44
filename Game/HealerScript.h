#pragma once
#include "TInteractScriptComponent.h" 

class HealerScript : public TInteractScriptComponent
{
public:
	HealerScript() = default;
	virtual ~HealerScript() = default;
public:
	void Init();
	void Tick();
	virtual shared_ptr<UScriptComponent> Clone() override;
public:
	virtual void Interact() override;

private:
};

