#pragma once
#include "UMeshComponent.h"
#include "UStaticMeshResources.h"

class UStaticMeshComponent : public UMeshComponent
{
public:
	UStaticMeshComponent() = default;
	virtual ~UStaticMeshComponent() = default;

protected:
	shared_ptr<UStaticMeshResources> m_pMesh = nullptr;

public:
	void Init() override;
	void Tick() override;
	void PreRender() override;
	void PostRender() override;
	void Destroy() override;

public:
	void SetMesh(shared_ptr<UStaticMeshResources> _mesh) { m_pMesh = _mesh; }
	shared_ptr<UStaticMeshResources> GetMesh() { return m_pMesh; }

public:
	static shared_ptr<UStaticMeshComponent> CreateTriangle();
	static shared_ptr<UStaticMeshComponent> CreatePlane();
	static shared_ptr<UStaticMeshComponent> CreateCube();
	static shared_ptr<UStaticMeshComponent> CreateSphere(int _sliceCount, int _stackCount);
};