#pragma once
#include "UPrimitiveComponent.h"

class UMeshComponent : public UPrimitiveComponent
{
public:
	void Init() override;
	void Tick() override;
	void PreRender() override;
	void Render() override;
	void PostRender() override;
	void Destroy() override;
};

