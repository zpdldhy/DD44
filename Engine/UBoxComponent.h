#pragma once
#include "UShapeComponent.h"

class UBoxComponent : public UShapeComponent
{
protected:
	// 실제 연산
	Box m_Box;

public:
	UBoxComponent() { m_ShapeType = ShapeType::ST_BOX; }
	virtual ~UBoxComponent() = default;

public:
	void Init() override;
	void Tick() override;
	void Render() override;
	void Destroy() override;

	void UpdateBounds() override;

protected:
	void CreateCollisionRange() override;
	void UpdateCollisionRange() override;

public:
	const Box& const GetBounds() { return m_Box; }

public:
	const Vec3& const GetCenter() override { return m_Box.vCenter; }
};

