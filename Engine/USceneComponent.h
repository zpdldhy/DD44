#pragma once
#include "UActorComponent.h"

class USceneComponent : public UActorComponent
{
public:
	USceneComponent() = default;
	virtual ~USceneComponent() = default;

public:
	virtual void Init();
	virtual void Tick();
	virtual void Render();
	virtual void Destroy();
};

