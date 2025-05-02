#pragma once
#include "UPrimitiveComponent.h"

enum class CollisionEnabled
{
	CE_NOCOLLISION,
	CE_QUERYONLY,
	CE_QUERYANDPHYSICS,
};

class UShapeComponent : public UPrimitiveComponent
{
protected:
	CollisionEnabled m_CollisionEnabled = CollisionEnabled::CE_NOCOLLISION;
	DirectX::SimpleMath::Color m_pShapeColor = { 0.f, 0.f, 0.f, 0.f };

public:
	UShapeComponent() = default;
	virtual ~UShapeComponent() = default;

public:
	virtual void UpdateBounds() abstract;

public:
	void SetShapeColor(DirectX::SimpleMath::Color _color) { m_pShapeColor = _color; }
	void SetCollisionEnabled(CollisionEnabled _type) { m_CollisionEnabled = _type; }
};

