#pragma once
#include "UMeshComponent.h"

class UStaticMeshComponent : public UMeshComponent
{
public:
	void CreateSphere(int iSliceCount, int iStackCount);
	void CreateCube();
	void CreateTriangle();
	void CreatePlane();
};

