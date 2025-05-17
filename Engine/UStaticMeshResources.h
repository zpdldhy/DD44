#pragma once
#include "UMeshResources.h"

// Binding ����
// 1. SetVertexList, SetIndexList
// 2. Bind

class UStaticMeshResources : public UMeshResources
{
public:
	UStaticMeshResources() { m_Type = MeshType::M_STATIC; }
	virtual ~UStaticMeshResources() = default;

public:
	void Create() override;
	void Bind() override;
};

