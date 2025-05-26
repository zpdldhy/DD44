#pragma once
#include "UScriptComponent.h"

class BatMovement : public UScriptComponent
{
public:
	BatMovement() = default;
	virtual ~BatMovement() = default;
public:
	Vec3 m_vCenter ;
	float m_fRadius = 8.0f;
	float angle = 0.0f; 
	float angularSpeed = 1.0f;
	float rotateSpeed = 0.5f;
	float m_fRotationSpeed = 8.0f;

public:
	void Init() override;
	void Tick() override;
};