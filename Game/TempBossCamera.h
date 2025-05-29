#pragma once
#include "UScriptComponent.h"

class TempBossCamera : public UScriptComponent
{
public:
	TempBossCamera() = default;
	virtual ~TempBossCamera() = default;
public:

public:
	void Init() override;
	void Tick() override;
};
