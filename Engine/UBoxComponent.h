#pragma once
#include "UShapeComponent.h"

class UBoxComponent : public UShapeComponent
{
protected:
	// 실제 연산
	Ray m_LookRay;
	Ray m_GroundRay;
	Box m_Box;

	// 범위 표기
	shared_ptr<class AActor> m_pCollisionRange = nullptr;
	ComPtr<ID3D11RasterizerState> m_pCurrentRasterizer = nullptr;

public:
	bool m_bVisible = true;

public:
	UBoxComponent() { m_ShapeType = ShapeType::ST_BOX; }
	virtual ~UBoxComponent() = default;

public:
	void Init() override;
	void Tick() override;
	void PreRender() override;
	void Render() override;
	void PostRender() override;
	void Destroy() override;

public:
	virtual void UpdateBounds() override;
	const Box& const GetBounds() { return m_Box; }
	const Ray& const GetLookRay() { return m_LookRay; }
	const Ray& const GetGroundRay() { return m_GroundRay; }

public:
	void CreateCollisionRange();
	void UpdateCollisionRange();

public:
	const Vec3& GetBoxCenter()  { return m_Box.vCenter; }
};

