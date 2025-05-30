#pragma once
#include "UPrimitiveComponent.h"
#include <set>

struct CollisionData
{
	Box box;
	Vec3 ColNormal = { 0.f, 0.f, 0.f };
	bool bColGround = false;
};

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
	ShapeType m_ShapeType = ShapeType::ST_NONE;
	DirectX::SimpleMath::Color m_pShapeColor = { 0.f, 0.f, 0.f, 0.f };

	// temp 
	map<UINT, CollisionData> m_vCollisionList;

public:
	UShapeComponent() = default;
	virtual ~UShapeComponent() = default;

public:
	virtual void UpdateBounds() abstract;

public:
	void SetShapeColor(DirectX::SimpleMath::Color _color) { m_pShapeColor = _color; }
	void SetCollisionEnabled(CollisionEnabled _type) { m_CollisionEnabled = _type; }

	const ShapeType& const GetShapeType() { return m_ShapeType; }
	const CollisionEnabled& const GetCollisionType() { return m_CollisionEnabled; }
	void AddCollision(UINT _colActor, CollisionData _colData) { m_vCollisionList.insert(make_pair(_colActor, _colData)); }
	map<UINT, CollisionData>& GetCollisionList() { return m_vCollisionList; }
	size_t GetCollisionCount() { return m_vCollisionList.size(); }
	void ResetCollisionList() { m_vCollisionList.clear(); }
};

