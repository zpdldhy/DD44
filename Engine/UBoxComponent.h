#pragma once
#include "UShapeComponent.h"

class UBoxComponent : public UShapeComponent
{
protected:
	// 실제 연산
	Box m_Box;
	Ray m_LookRay;
	Ray m_GroundRay;

public:
	UBoxComponent() { m_ShapeType = ShapeType::ST_BOX; }
	virtual ~UBoxComponent() = default;

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
	const Box& const GetBounds() { return m_Box; }
	const Ray& const GetLookRay() { return m_LookRay; }
	const Ray& const GetGroundRay() { return m_GroundRay; }

public:
	const Vec3& GetBoxCenter()  { return m_Box.vCenter; }
};

