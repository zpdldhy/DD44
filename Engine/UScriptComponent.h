#pragma once
#include "UActorComponent.h"
#include "AActor.h"

class UScriptComponent : public UActorComponent
{
public:
	UScriptComponent() = default;
	virtual ~UScriptComponent() = default;

public:
	void Init() override;
	void Tick() override;
	void Render() override;
	void Destroy() override;
};

