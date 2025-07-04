#pragma once
#include "UPrimitiveComponent.h"
#include <set>

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
	ShapeType m_ShapeType = ShapeType::ST_COUNT;
	DirectX::SimpleMath::Color m_pShapeColor = { 0.f, 0.f, 0.f, 0.f };

	// ���� ǥ��
	shared_ptr<class AActor> m_pCollisionRange = nullptr;
	static vector<shared_ptr<class AInstance>> m_vInstanceList;
	static bool isRendered;

	Ray m_LookRay;
	Ray m_GroundRay;
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
	void Render() override;
	void PostRender() override {}

public:
	virtual void UpdateBounds() abstract;

protected:
	virtual void CreateCollisionRange() abstract;
	virtual void UpdateCollisionRange() abstract;

public:
	void SetShapeColor(DirectX::SimpleMath::Color _color) { m_pShapeColor = _color; }
	void SetCollisionEnabled(CollisionEnabled _type) { m_CollisionEnabled = _type; }
	virtual const Vec3 GetCenter() abstract;

	const ShapeType GetShapeType() { return m_ShapeType; }
	const CollisionEnabled GetCollisionType() { return m_CollisionEnabled; }

	const Ray GetLookRay() { return m_LookRay; }
	const Ray GetGroundRay() { return m_GroundRay; }

protected:
	void SetInstance(shared_ptr<AActor> _pActor);
	void MakeInstance(shared_ptr<AActor> _pActor);
};

