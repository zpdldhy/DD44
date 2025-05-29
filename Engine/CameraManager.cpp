#include "pch.h"
#include "CameraManager.h"
#include "Device.h"
#include "ACameraActor.h"
#include "UCameraComponent.h"
#include "Timer.h"

void CameraManager::Init()
{	
	// No CameraActor!! 
	// Set in client
	if(m_p3DCameraActor == nullptr)
		assert(false);

	m_pUICameraActor = make_shared<ACameraActor>(ProjectionType::PT_ORTHOGRAPHIC);
	{
		m_pUICameraActor->SetPosition(Vec3(0.f, 0.f, -1.f));
		m_pUICameraActor->Init();
		m_pUIComponent = m_pUICameraActor->GetCameraComponent();
	}

	CreateCameraBuffer();
}

void CameraManager::Tick()
{
	m_pUICameraActor->Tick();
	m_p3DCameraActor->GetCameraComponent()->SetFrustumVisible(false);
}

void CameraManager::Render(CameraViewType _ViewType)
{
	shared_ptr<UCameraComponent> pCurrentComponent = nullptr;

	if (_ViewType == CameraViewType::CVT_ACTOR)
		pCurrentComponent = m_p3DCameraActor->GetCameraComponent();
	else if (_ViewType == CameraViewType::CVT_UI)
		pCurrentComponent = m_pUICameraActor->GetCameraComponent();

	m_CameraData.matView = pCurrentComponent->GetView();
	m_CameraData.matProjection = pCurrentComponent->GetProjection();
	m_CameraData.g_vCameraPos = pCurrentComponent->GetWorldPosition();
	m_CameraData.GameTime = TIMER->GetGameTime();

	DC->UpdateSubresource(m_pCameraCB.Get(), 0, nullptr, &m_CameraData, 0, 0);
	DC->VSSetConstantBuffers(1, 1, m_pCameraCB.GetAddressOf());
	DC->PSSetConstantBuffers(1, 1, m_pCameraCB.GetAddressOf());
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

void CameraManager::Set3DCameraActor(shared_ptr<class AActor> _cameraActor)
{
	if (_cameraActor->GetCameraComponent() == nullptr)
		return;

	m_p3DCameraActor = _cameraActor;
	m_p3DComponent = m_p3DCameraActor->GetCameraComponent();
}

shared_ptr<class AActor> CameraManager::Get3DCameraActor()
{
	return m_p3DCameraActor;
}

shared_ptr<class UCameraComponent> CameraManager::Get3DCameraComponent()
{
	return m_p3DComponent;
}

Matrix CameraManager::Get3DView()
{
	return m_p3DComponent->GetView();
}

Matrix CameraManager::Get3DProjection()
{
	return m_p3DComponent->GetProjection();
}

shared_ptr<class UCameraComponent> CameraManager::GetUICameraComponent()
{
	return m_pUIComponent;
}

Matrix CameraManager::GetUIView()
{
	return m_pUIComponent->GetView();
}

Matrix CameraManager::GetUIProjection()
{
	return m_pUIComponent->GetProjection();
}
