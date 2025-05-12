#pragma once
#include "UShapeComponent.h"

class UBoxComponent : public UShapeComponent
{
protected:
	Vec3 m_vCenter = Vec3::Zero;

	shared_ptr<class AActor> m_pCollisionRange = nullptr;
	vector<PNCT_VERTEX> m_vVertexList;
	vector<DWORD> m_vIndexList;

	ComPtr<ID3D11RasterizerState> m_pCurrentRasterizer = nullptr;

public:
	UBoxComponent() = default;
	virtual ~UBoxComponent() = default;

public:
	void Init() override;
	void Tick() override;
	void PreRender() override;
	void Render() override;
	void PostRender() override;
	void Destroy() override;

public:
	void CreateCollisionBox();
	void UpdateBounds() override;

public:
	const Vec3& GetBoxCenter()  { return m_vCenter; }
};

