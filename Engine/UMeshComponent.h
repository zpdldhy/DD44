#pragma once
#include "UPrimitiveComponent.h"

class UMeshComponent : public UPrimitiveComponent
{
public:
	UMeshComponent() = default;
	virtual ~UMeshComponent() = default;

protected:
	vector<shared_ptr<UMeshComponent>>	m_vChild;

public:
	void AddChild(shared_ptr<UMeshComponent> _child) { m_vChild.push_back(_child); }
};

