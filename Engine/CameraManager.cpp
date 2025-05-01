#include "pch.h"
#include "CameraManager.h"
#include "Device.h"
#include "ACameraActor.h"
#include "UCameraComponent.h"

void CameraManager::Init()
{	
	// Set Actor!! 
	// In Client!! 
	// Not in Engine!!
	if(m_pCurrentCameraActor == nullptr)
		assert(false);

	m_pUICameraActor = make_shared<ACameraActor>(ProjectionType::PT_ORTHOGRAPHIC);
	{
		m_pUICameraActor->SetPosition(Vec3(0.f, 0.f, 0.f));
		m_pUICameraActor->Init();
	}

	CreateCameraBuffer();
}

void CameraManager::Tick()
{
	m_pUICameraActor->Tick();
}

void CameraManager::Render(CameraViewType _ViewType)
{
	shared_ptr<UCameraComponent> pCameraComponent = nullptr;

	if (_ViewType == CameraViewType::CVT_ACTOR)
		pCameraComponent = m_pCurrentCameraActor->GetCameraComponent<UCameraComponent>();
	else if (_ViewType == CameraViewType::CVT_UI)
		pCameraComponent = m_pUICameraActor->GetCameraComponent<UCameraComponent>();

	m_CameraData.matView = pCameraComponent->GetView();
	m_CameraData.matProjection = pCameraComponent->GetProejction();

	DC->UpdateSubresource(m_pCameraCB.Get(), 0, nullptr, &m_CameraData, 0, 0);
	DC->VSSetConstantBuffers(1, 1, m_pCameraCB.GetAddressOf());
}

void CameraManager::Destroy()
{

}

void CameraManager::CreateCameraBuffer()
{
	D3D11_BUFFER_DESC pDesc;
	ZeroMemory(&pDesc, sizeof(pDesc));
	pDesc.ByteWidth = sizeof(CameraConstantData);
	pDesc.Usage = D3D11_USAGE_DEFAULT;
	pDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	HRESULT hr = DEVICE->CreateBuffer(&pDesc, nullptr, m_pCameraCB.GetAddressOf());

	if (FAILED(hr))
	{
		DX_CHECK(hr, _T("CreateCameraBuffer Failed"));
		assert(false);
	}
}

void CameraManager::SetCameraActor(shared_ptr<class AActor> _cameraActor)
{
	if (_cameraActor->GetCameraComponent<UCameraComponent>() == nullptr)
		return;

	m_pCurrentCameraActor = _cameraActor;
}

