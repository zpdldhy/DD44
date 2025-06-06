#pragma once
#include "UPrimitiveComponent.h"
#include <set>

struct CollisionData
{
	Box box;
	Vec3 Inter = { 0.f, 0.f, 0.f };
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

	// ���� ǥ��
	shared_ptr<class AActor> m_pCollisionRange = nullptr;
	shared_ptr<class AActor> m_pLookRange = nullptr;
	shared_ptr<class AActor> m_pDownRange = nullptr;
	ComPtr<ID3D11RasterizerState> m_pCurrentRasterizer = nullptr;

public:
	bool m_bVisible = true;

public:
	UShapeComponent() = default;
	virtual ~UShapeComponent() = default;

private:
	void PreRender() override {}
	void PostRender() override {}

protected:
	virtual void UpdateBounds() abstract;

	virtual void CreateCollisionRange() abstract;
	virtual void UpdateCollisionRange() abstract;

public:
	void SetShapeColor(DirectX::SimpleMath::Color _color) { m_pShapeColor = _color; }
	void SetCollisionEnabled(CollisionEnabled _type) { m_CollisionEnabled = _type; }
	virtual const Vec3& const GetCenter() abstract;

	const ShapeType& const GetShapeType() { return m_ShapeType; }
	const CollisionEnabled& const GetCollisionType() { return m_CollisionEnabled; }
};

