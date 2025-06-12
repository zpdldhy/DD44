#pragma once
#include "UActorComponent.h"
#include "AActor.h"

class UScriptComponent : public UActorComponent, enable_shared_from_this<UScriptComponent>
{
public:
	UScriptComponent() = default;
	virtual ~UScriptComponent() = default;

public:
	void Init() override;
	void Tick() override;
	void Render() override;
	void Destroy() override;
public:
	virtual shared_ptr<UScriptComponent> Clone();
};

