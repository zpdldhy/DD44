#pragma once
#include "IExecute.h"
#include "ActorLoader.h"
#include "MeshLoader.h"

class TestYoooooon : public IExecute
{
public:
	shared_ptr<class ACameraActor> m_pCameraActor = nullptr;
	shared_ptr<class AActor> m_pPlayer = nullptr;
	shared_ptr<class AActor> m_pBox = nullptr;
	class MouseRay m_Cursor;

	bool m_bEnginCamera = true;

	// Asset loading
	ActorLoader actorLoader;
	MeshLoader meshLoader;

public:
	void Init() override;
	void Update() override;
	void Render() override;
	void Destroy();

protected:
	void SetupEngineCamera();
	void SetupSkybox();
	void SetupSunLight();

	void SetupEditorCallbacks();
	void SetupCharacterEditorCallback();
	void SetupMapEditorCallback();
	void SetupObjectEditorCallback();
	void SetupUIEditorCallback();

	void LoadAllPrefabs(const std::string& extension);

	// Collision
	void CreateCollisionObject();
	void ClickMouse();
	void CheckCollision();
};

