#pragma once
#include "IExecute.h"
#include "MapEditorUI.h"

class TestSY : public IExecute
{
public:
	shared_ptr<class UStaticMeshComponent> m_pStaticMesh;
	shared_ptr<class APawn> m_pActor;

	shared_ptr<class ACameraActor> m_pCameraActor;

	shared_ptr<class UStaticMeshComponent> m_pSkyMesh;
	shared_ptr<class ASky> m_pSky;

	std::unique_ptr<MapEditorUI> m_pMapEditorUI;
	std::vector<std::shared_ptr<class ATerrainTileActor>> m_vTiles;

	bool m_bEditorWireframe = false;

	// ViewPort Texture
	shared_ptr<class APawn> m_pPlane = nullptr;
	shared_ptr<class ViewPortTexture> m_pTexture = nullptr;

public:
	void Init() override;
	void Update() override;
	void Render() override;
	void Destroy();
};

