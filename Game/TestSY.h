#pragma once
#include "IExecute.h"
#include "MapEditorUI.h"
#include "ObjectEditorUI.h"

class TestSY : public IExecute
{
public:
	shared_ptr<class UStaticMeshComponent> m_pStaticMesh;
	shared_ptr<class APawn> m_pActor;

	shared_ptr<class ACameraActor> m_pCameraActor;

	shared_ptr<class UStaticMeshComponent> m_pSkyMesh;
	shared_ptr<class ASky> m_pSky;

	bool m_bEditorWireframe = false;

	// Mouse Picking
	Vec3 m_vMouseLay;
	Vec3 m_vCameraPos;


public:
	void Init() override;
	void Update() override;
	void Render() override;
	void Destroy();

private:
	void SetClickPos();
};

