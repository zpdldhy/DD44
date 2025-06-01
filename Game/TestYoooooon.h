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
	vector<shared_ptr<class AActor>> m_vGround;
	class MouseRay m_Cursor;

	bool m_bEnginCamera = true;

public:
	void Init() override;
	void Tick() override;
	void Render() override;
	void Destroy() override;

protected:
	void SetupEngineCamera();
	void SetupSkybox();
	void SetupSunLight();

	void SetupEditorCallbacks();
	void SetupCharacterEditorCallback();
	void SetupMapEditorCallback();
	void SetupObjectEditorCallback();
	void SetupUIEditorCallback();

	// Collision
	void CreateCollisionObject();
	void ClickMouse();
	void CheckCollision();
};

