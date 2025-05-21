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
	ShapeType m_ShapeType = ShapeType::ST_NONE;
	DirectX::SimpleMath::Color m_pShapeColor = { 0.f, 0.f, 0.f, 0.f };

	shared_ptr<CollisionScript> m_CollisionScript = nullptr;
	CollisionEnabled m_CollisionEnabled = CollisionEnabled::CE_NOCOLLISION;

	std::vector<std::shared_ptr<UShapeComponent>> m_vChildren;

public:
	UShapeComponent() = default;
	virtual ~UShapeComponent() = default;

	void Init() override;
	void Tick() override;
	void Render() override;
	void PreRender() override;
	void PostRender() override;

public:
	//virtual void UpdateBounds() abstract;
	void CollisionAction();
	void AddChild(std::shared_ptr<UShapeComponent> child);
	bool HasChildren() const;

public:
	void SetShapeColor(DirectX::SimpleMath::Color _color) { m_pShapeColor = _color; }
	void SetCollisionEnabled(CollisionEnabled _type) { m_CollisionEnabled = _type; }

	const ShapeType& const GetShapeType() { return m_ShapeType; }
	const CollisionEnabled& const GetCollisionType() { return m_CollisionEnabled; }
	const std::vector<std::shared_ptr<UShapeComponent>>& GetChildren() const { return m_vChildren; }
};

