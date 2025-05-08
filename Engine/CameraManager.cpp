#include "pch.h"
#include "CameraManager.h"
#include "Device.h"
#include "ACameraActor.h"
#include "UCameraComponent.h"

void CameraManager::Init()
{	
	// No CameraActor!! 
	// Set in client
	if(m_pCurrentCameraActor == nullptr)
		assert(false);

	m_pUICameraActor = make_shared<ACameraActor>(ProjectionType::PT_ORTHOGRAPHIC);
	{
		m_pUICameraActor->SetPosition(Vec3(0.f, 0.f, -1.f));
		m_pUICameraActor->Init();
	}

	CreateCameraBuffer();
}

void CameraManager::Tick()
{
	m_pUICameraActor->Tick();
	m_pCurrentCameraActor->GetCameraComponent()->SetFrustumVisible(false);
}

void CameraManager::Render(CameraViewType _ViewType)
{
	if (_ViewType == CameraViewType::CVT_ACTOR)
		m_pCurrentComponent = m_pCurrentCameraActor->GetCameraComponent();
	else if (_ViewType == CameraViewType::CVT_UI)
		m_pCurrentComponent = m_pUICameraActor->GetCameraComponent();

	m_CameraData.matView = m_pCurrentComponent->GetView();
	m_CameraData.matProjection = m_pCurrentComponent->GetProjection();

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
	if (_cameraActor->GetCameraComponent() == nullptr)
		return;

	m_pCurrentCameraActor = _cameraActor;
}

shared_ptr<class AActor> CameraManager::GetCameraActor()
{
	return m_pCurrentCameraActor;
}

shared_ptr<class UCameraComponent> CameraManager::GetCurrentCameraComponent()
{
	return m_pCurrentComponent;
}

Matrix CameraManager::GetCurrentView()
{
	return m_pCurrentComponent->GetView();
}

Matrix CameraManager::GetCurrentProjection()
{
	return m_pCurrentComponent->GetProjection();
}
