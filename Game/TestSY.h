#pragma once
#include "IExecute.h"
#include "MapEditorUI.h"
#include "ObjectEditorUI.h"
#include "ActorLoader.h"
#include "PrefabLoader.h"
#include "QuadTree.h"

class TestSY : public IExecute
{
public:
	shared_ptr<class UStaticMeshComponent> m_pStaticMesh;
	shared_ptr<class APawn> m_pActor;

	shared_ptr<class ACameraActor> m_pCameraActor;

	shared_ptr<class UStaticMeshComponent> m_pSkyMesh;
	shared_ptr<class ASky> m_pSky;
	shared_ptr<class ALight> m_pLight;

	std::shared_ptr<QuadTree> m_pQuadTree;

public:
	void Init() override;
	void Tick() override;
	void Render() override;
	void Destroy() override;

private:
	void SetClickPos();
	void LoadAllPrefabs(const std::string& extension);
	void InitializeQuadTree();
	void InsertAllActorsIntoQuadTree();
	void UpdateQuadTreeActors();
	void QuadTreeCollision();
};

