#pragma once
#include "UPrimitiveComponent.h"
#include "CollisionScript.h"

enum class CollisionEnabled
{
	CE_NOCOLLISION,
	CE_QUERYONLY,
	CE_QUERYANDPHYSICS,
};

class UShapeComponent : public UPrimitiveComponent
{
protected:
	shared_ptr<CollisionScript> m_CollisionScript = nullptr;
	CollisionEnabled m_CollisionEnabled = CollisionEnabled::CE_NOCOLLISION;
	ShapeType m_ShapeType = ShapeType::ST_NONE;
	DirectX::SimpleMath::Color m_pShapeColor = { 0.f, 0.f, 0.f, 0.f };

public:
	UShapeComponent() = default;
	virtual ~UShapeComponent() = default;

public:
	virtual void UpdateBounds() abstract;
	void CollisionAction();

public:
	void SetShapeColor(DirectX::SimpleMath::Color _color) { m_pShapeColor = _color; }
	void SetCollisionEnabled(CollisionEnabled _type) { m_CollisionEnabled = _type; }

	const ShapeType& const GetShapeType() { return m_ShapeType; }
	const CollisionEnabled& const GetCollisionType() { return m_CollisionEnabled; }
};

