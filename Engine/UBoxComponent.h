#pragma once
#include "UShapeComponent.h"

class UBoxComponent : public UShapeComponent
{
protected:
	// 실제 연산
	Box m_Box;

	// 범위 표기
	shared_ptr<class AActor> m_pCollisionRange = nullptr;
	vector<PNCT_VERTEX> m_vVertexList;
	vector<DWORD> m_vIndexList;

	ComPtr<ID3D11RasterizerState> m_pCurrentRasterizer = nullptr;

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

public:
	void CreateCollisionRange();
	void UpdateCollisionRange();

public:
	const Vec3& GetBoxCenter()  { return m_Box.vCenter; }
};

