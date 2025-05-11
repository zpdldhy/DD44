#pragma once
#include "IExecute.h"
#include "MapEditorUI.h"
#include "ObjectEditorUI.h"
#include "ActorLoader.h"
#include "PrefabLoader.h"

class TestSY : public IExecute
{
public:
	shared_ptr<class UStaticMeshComponent> m_pStaticMesh;
	shared_ptr<class APawn> m_pActor;

	shared_ptr<class ACameraActor> m_pCameraActor;

	shared_ptr<class UStaticMeshComponent> m_pSkyMesh;
	shared_ptr<class ASky> m_pSky;

public:
	void Init() override;
	void Update() override;
	void Render() override;
	void Destroy();

	void LoadAllPrefabs(const std::string& extension);
};

