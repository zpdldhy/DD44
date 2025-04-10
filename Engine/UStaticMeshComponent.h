#pragma once
#include "UMeshComponent.h"

class UStaticMeshComponent : public UMeshComponent
{
public:
	void Init() override;
	void Tick() override;
	void PreRender() override;
	void Render() override;
	void PostRender() override;
	void Destroy() override;

public:
	void CreateCube();
	void CreateTriangle();
	void CreatePlane();
};

