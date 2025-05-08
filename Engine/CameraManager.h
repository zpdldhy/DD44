#pragma once
#include "Singleton.h"

struct CameraConstantData
{
	Matrix matView;
	Matrix matProjection;
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

	shared_ptr<class AActor> m_pCurrentCameraActor = nullptr;
	shared_ptr<class AActor> m_pUICameraActor = nullptr;	// Canvers
	shared_ptr<class UCameraComponent> m_pCurrentComponent = nullptr;

public:
	void Init();
	void Tick();
	void Render(CameraViewType _ViewType);
	void Destroy();

private:
	void CreateCameraBuffer();

public:
	void SetCameraActor(shared_ptr<class AActor> _cameraActor);	
	shared_ptr<class AActor> GetCameraActor();
	shared_ptr<class UCameraComponent> GetCurrentCameraComponent();
	Matrix GetCurrentView();
	Matrix GetCurrentProjection();
};

