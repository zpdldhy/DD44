#pragma once
#include "TInteractScriptComponent.h" 

class FenceScript : public TInteractScriptComponent
{
public:
	FenceScript() = default;
	virtual ~FenceScript() = default;
public:
	void Init();
	void Tick();
	virtual shared_ptr<UScriptComponent> Clone() override;
public:
	virtual void Interact() override;
private:
	bool m_bOpen = false;
	float m_elapsed = 0.0f;
	Vec3 originPos;
	Vec3 offset;
};

