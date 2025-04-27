#pragma once
#include "UMeshComponent.h"

class USkinnedComponent : public UMeshComponent
{
	virtual void Init();
	virtual void Tick();
	virtual void Render();
	virtual void Destroy();
};

