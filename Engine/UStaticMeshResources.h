#pragma once
#include "UMeshResources.h"

// Binding ¼ø¼­
// 1. SetVertexList, SetIndexList
// 2. Bind

class UStaticMeshResources : public UMeshResources
{
public:
	UStaticMeshResources() = default;
	virtual ~UStaticMeshResources() = default;

public:
	void Create() override;
	void Bind() override;
};

