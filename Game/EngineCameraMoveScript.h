#pragma once
#include "UScriptComponent.h"

class EngineCameraMoveScript : public UScriptComponent
{
	POINT m_pPrevMousePos = { 0, 0 };

public:
	EngineCameraMoveScript() = default;
	virtual ~EngineCameraMoveScript() = default;

public:
	void Init() override;
	void Tick() override;
};

