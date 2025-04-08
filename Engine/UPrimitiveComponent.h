#pragma once
#include "USceneComponent.h"

class UMaterial;
class Meshdata;

class UPrimitiveComponent : public USceneComponent
{
protected:
	// MATERIAL
	shared_ptr<UMaterial> m_pMaterial;
	// 
	//shared_ptr<MeshData

public:
	void SetMaterial(shared_ptr<UMaterial> _material);
public:
	bool CreateVertexBuffer();
	bool CreateIndexBuffer();
public:
	void PreRender();
	virtual void Render() override;
	void PostRender();
};

