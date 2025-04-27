#pragma once
#include "AActor.h"
#include "UCameraComponent.h"

class ACameraActor : public AActor
{
	shared_ptr<UCameraComponent> m_pCameraComponent;

public:
	void Init() override;
	void Tick() override;
	void Render() override;

public:
	ACameraActor() : m_pCameraComponent(make_shared<UCameraComponent>(ProjectionType::PT_PERSPECTIVE)) {}
	~ACameraActor() {}
};

