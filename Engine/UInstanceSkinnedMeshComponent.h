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
	virtual void SetVisible(bool _visible) override;

private:
	INSTANCE_VERTEX m_trans;
	ANIM_VERTEX m_anim;
};

