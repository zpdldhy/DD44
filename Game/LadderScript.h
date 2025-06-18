#pragma once
#include "TInteractScriptComponent.h" 
class LadderScript : public TInteractScriptComponent
{
public:
	LadderScript() = default;
	virtual ~LadderScript() = default;
public:
	void Init();
	void Tick();
	virtual shared_ptr<UScriptComponent> Clone() override;
public:
	virtual void Interact() override;
private:
	bool m_bAppear = false;
	float m_elapsed = 0.0f;
	int activeChildNum = 0;
};

