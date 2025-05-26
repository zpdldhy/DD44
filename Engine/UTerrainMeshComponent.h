#pragma once
#include "UMeshComponent.h"
#include "UStaticMeshResources.h"

class UTerrainMeshComponent : public UMeshComponent
{
public:
	UTerrainMeshComponent() = default;
	virtual ~UTerrainMeshComponent() = default;

protected:
	shared_ptr<UStaticMeshResources> m_pMesh = nullptr;

public:
	void Init() override;
	void Tick() override;
	void Render() override;
	void PreRender() override;
	void PostRender() override;
	void Destroy() override;

public:
	shared_ptr<UStaticMeshResources> GetMesh() { return m_pMesh; }

public:
	void CreateGrid(int _sizeX, int _sizeZ, float _cellSize);
	void MeshCreate() { m_pMesh->Create(); }
};

