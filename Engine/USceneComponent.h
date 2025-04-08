#pragma once
#include "UActorComponent.h"

class USceneComponent : public UActorComponent
{
public:
	virtual void Init() {};
	virtual void Tick() {};
	virtual void Render() {};
	virtual void Destroy() {};
};

