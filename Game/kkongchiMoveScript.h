#pragma once
#include "UScriptComponent.h"

class kkongchiMoveScript : public UScriptComponent
{
public:
	kkongchiMoveScript() = default;
	virtual ~kkongchiMoveScript() = default;

public:
	void Tick() override;
};

