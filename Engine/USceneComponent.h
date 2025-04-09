#pragma once
#include "UActorComponent.h"

class USceneComponent : public UActorComponent
{
public:
	void Init() override;
	void Tick() override;
	void PreRender() override;
	void Render() override;
	void PostRender() override;
	void Destroy() override;
};

