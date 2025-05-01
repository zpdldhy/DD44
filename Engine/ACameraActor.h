#pragma once
#include "AActor.h"
#include "UCameraComponent.h"

class ACameraActor : public AActor
{

public:
	void Init() override;
	void Tick() override;
	void Render() override;

public:
	ACameraActor(ProjectionType _type = ProjectionType::PT_PERSPECTIVE) { m_arrComponent[static_cast<size_t>(ComponentType::CT_CAMERA)] = static_pointer_cast<USceneComponent>(make_shared<UCameraComponent>(_type)); }
	~ACameraActor() {}
};

