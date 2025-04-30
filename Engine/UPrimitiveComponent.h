#pragma once
#include "USceneComponent.h"

class UMaterial;

class UPrimitiveComponent : public USceneComponent
{
public:
	UPrimitiveComponent() = default;
	virtual ~UPrimitiveComponent() = default;

protected:
	// MATERIAL
	shared_ptr<UMaterial> m_pMaterial;

public:
	void SetMaterial(shared_ptr<UMaterial> _material){ m_pMaterial = _material; }
	shared_ptr<UMaterial> GetMaterial() { return m_pMaterial; }

public:
	virtual void PreRender() abstract;
	virtual void PostRender() abstract;
};

