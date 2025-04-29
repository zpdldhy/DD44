#pragma once
#include "UMeshComponent.h"

class UStaticMeshComponent : public UMeshComponent
{
public:
	void CreateTriangle();
	void CreatePlane();
	void CreateCube();
	void CreateSphere(int _sliceCount, int _stackCount);
};

