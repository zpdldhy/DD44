#pragma once
#include "Singleton.h"

struct CameraConstantData
{
	Matrix matView;
	Matrix matProjection;
	Vec3   g_vCameraPos;
	float  GameTime;
};

enum class CameraViewType
{
	CVT_ACTOR,
	CVT_UI
};

class CameraManager : public Singleton<CameraManager>
{
	ComPtr<ID3D11Buffer> m_pCameraCB;
	CameraConstantData m_CameraData;

	shared_ptr<class AActor> m_p3DCameraActor = nullptr;	// 3D Camera
	shared_ptr<class AActor> m_pUICameraActor = nullptr;	// Canvers
	shared_ptr<class UCameraComponent> m_p3DComponent = nullptr;
	shared_ptr<class UCameraComponent> m_pUIComponent = nullptr;

public:
	void Init();
	void Tick();
	void Render(CameraViewType _ViewType);
	void Destroy();

private:
	void CreateCameraBuffer();

public:
	void Set3DCameraActor(shared_ptr<class AActor> _cameraActor);	
	shared_ptr<class AActor> Get3DCameraActor();

	// Get
	shared_ptr<class UCameraComponent> Get3DCameraComponent();
	Matrix Get3DView();
	Matrix Get3DProjection();

	shared_ptr<class UCameraComponent> GetUICameraComponent();
	Matrix GetUIView();
	Matrix GetUIProjection();
};

