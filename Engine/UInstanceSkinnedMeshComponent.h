#pragma once
#include "USkinnedMeshComponent.h"
class UInstanceSkinnedMeshComponent : public USkinnedMeshComponent
{
public:
	void Init() override;
	void Tick() override;
	void Render() override;
	void PreRender() override;
	void PostRender() override;
	void Destroy() override;
public:
	void UpdateMeshResourceData();
public:
	virtual void RenderShadow();
public:
	virtual void SetVisible(bool _visible) override;
	void SetShadowVisible(bool _b) { bRenderShadow = _b; }

private:
	INSTANCE_VERTEX m_trans;
	ANIM_VERTEX m_anim;

	// shadow
	bool bRenderShadow = true;
};

