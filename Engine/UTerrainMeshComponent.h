#pragma once
#include "UMeshComponent.h"

class UTerrainMeshComponent : public UMeshComponent
{
public:
	void CreateTriangle();
	void CreatePlane();
	void CreateCube();
	void CreateSphere(int _sliceCount, int _stackCount);
	void CreateGrid(int _col, int _row, float _cellSize);
};

