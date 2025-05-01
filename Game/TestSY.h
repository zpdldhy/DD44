#pragma once
#include "IExecute.h"

class TestSY : public IExecute
{
public:
	shared_ptr<class UStaticMeshComponent> m_pStaticMesh;
	shared_ptr<class APawn> m_pActor;

	shared_ptr<class ATerrainTileActor> m_pTerrain;

	shared_ptr<class ACameraActor> m_pCameraActor;

	shared_ptr<class UStaticMeshComponent> m_pSkyMesh;
	shared_ptr<class ASky> m_pSky;

	vector<shared_ptr<APawn>> m_vObjects;
    int m_gridWidth = 20;
    int m_gridHeight = 20;
    float m_cellSize = 10.0f;

public:
	void Init() override;
	void Update() override;
	void Render() override;
	void Destroy();

	
};

