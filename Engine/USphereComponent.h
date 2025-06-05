#pragma once
#include "UShapeComponent.h"

class USphereComponent : public UShapeComponent
{
protected:
	// 실제 연산
	Sphere m_Sphere;
	Ray m_LookRay;
	Ray m_GroundRay;

public:
	USphereComponent() { m_ShapeType = ShapeType::ST_SPHERE; }
	virtual ~USphereComponent() = default;

public:
	void Init() override;
	void Tick() override;
	void Render() override;
	void Destroy() override;

protected:
	void UpdateBounds() override;

	void CreateCollisionRange() override;
	void UpdateCollisionRange() override;

public:
	const Sphere& const GetBounds() { return m_Sphere; }
	const Vec3& const GetCenter() override { return m_Sphere.vCenter; }
};

