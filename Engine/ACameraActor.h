#pragma once
#include "AActor.h"
#include "CameraComponent.h"

class ACameraActor : public AActor
{
	shared_ptr<CameraComponent> m_pCameraComponent;

public:
	void Tick() override;

public:
	shared_ptr<CameraComponent> GetCameraComponent() { return m_pCameraComponent; }
};

