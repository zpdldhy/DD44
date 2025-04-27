#pragma once
#include "UMeshComponent.h"

class UStaticMeshComponent : public UMeshComponent
{
public:
	void CreateCube();
	void CreateTriangle();
	void CreatePlane();
};

