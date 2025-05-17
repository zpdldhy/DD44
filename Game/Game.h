#pragma once
#include "IExecute.h"

class Game : public IExecute
{
public:
	shared_ptr<class ACameraActor> m_pCameraActor;

	shared_ptr<class UStaticMeshComponent> m_pSkyMesh;
	shared_ptr<class ASky> m_pSky;

public:
	void Init() override;
	void Update() override;
	void Render() override;
	void Destroy();

protected:
	void SetupEngineCamera();
	void SetupSkybox();

	void SetupEditorCallbacks();
	void SetupCharacterEditorCallback();
	void SetupMapEditorCallback();
	void SetupObjectEditorCallback();

	void LoadAllPrefabs(const std::string& extension);

};

