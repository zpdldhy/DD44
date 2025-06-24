#pragma once
#include "TInteractScriptComponent.h" 

class AActor;

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
	void Open();
private:
	shared_ptr<AActor> bodyCollider;
	bool m_bOpen = false;
	bool m_bClose = false;
	float m_elapsed = 0.0f;
	Vec3 originPos;
	Vec3 offset;
};

