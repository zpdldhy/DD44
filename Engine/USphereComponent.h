#pragma once
#include "UShapeComponent.h"

class USphereComponent : public UShapeComponent
{
protected:
	// 실제 연산
	Sphere m_Sphere;

public:
	USphereComponent() { m_ShapeType = ShapeType::ST_SPHERE; }
	virtual ~USphereComponent() = default;

public:
	void Init() override;
	void Tick() override;
	void Destroy() override;

	void UpdateBounds() override;

protected:
	void CreateCollisionRange() override;
	void UpdateCollisionRange() override;

public:
	const Sphere GetBounds() { return m_Sphere; }
	const Vec3 GetCenter() override { return m_Sphere.vCenter; }
};

