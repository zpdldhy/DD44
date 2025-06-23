#pragma once
#include "USkinnedMeshComponent.h"
class UInstanceSkinnedMeshComponent : public USkinnedMeshComponent
{
public:
	void Init() override;
	void Tick() override;
	void PreRender() override;
	void PostRender() override;
	void Destroy() override;
public:
	void UpdateMeshResourceData();
private:
	INSTANCE_VERTEX m_trans;
	ANIM_VERTEX m_anim;
};

