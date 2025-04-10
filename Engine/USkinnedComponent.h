#pragma once
#include "UMeshComponent.h"

class USkinnedComponent : public UMeshComponent
{
	void Init() override;
	void Tick() override;
	void PreRender() override;
	void Render() override;
	void PostRender() override;
	void Destroy() override;
};

