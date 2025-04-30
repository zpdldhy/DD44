#pragma once
#include "UMeshResources.h"

// Binding ����
// 1. SetVertexList, SetIndexList
// 2. Bind

class UStaticMeshResources : public UMeshResources
{
public:
	UStaticMeshResources() = default;
	virtual ~UStaticMeshResources() = default;

public:
	void Bind() override;
};

