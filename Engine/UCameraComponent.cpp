#include "pch.h"
#include "UCameraComponent.h"

void UCameraComponent::Init()
{
	CreateCameraBuffer();
}

void UCameraComponent::Tick()
{
	UpdateView();

	if (m_ProjectionType == ProjectionType::PT_ORTHOGRAPHIC)
		UpdateOrthographicProjection();
	else if (m_ProjectionType == ProjectionType::PT_PERSPECTIVE)
		UpdatePersPectiveProjection();
}

void UCameraComponent::Render()
{
	DC->UpdateSubresource(m_pCameraCB.Get(), 0, nullptr, &m_CameraData, 0, 0);
	DC->VSSetConstantBuffers(1, 1, m_pCameraCB.GetAddressOf());
}

void UCameraComponent::Destroy()
{
}

void UCameraComponent::CreateCameraBuffer()
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

void UCameraComponent::UpdateView()
{
	auto vEye = m_pOwner->m_vPosition;
	auto vLook = m_pOwner->m_vLook;
	auto vUp = m_pOwner->m_vUp;

	m_CameraData.matView = DirectX::XMMatrixLookAtLH(vEye, vEye + vLook, vUp);
}

void UCameraComponent::UpdateOrthographicProjection()
{	
	m_CameraData.matProjection = DirectX::SimpleMath::Matrix::CreateOrthographic(m_fWidth, m_fHeight, 0.1f, 1000.0f);
}

void UCameraComponent::UpdatePersPectiveProjection()
{
	m_CameraData.matProjection = DirectX::XMMatrixPerspectiveFovLH(m_fFov, m_fAspect, m_fNear, m_fFar);
}

void UCameraComponent::SetOrthographic(float _width, float _height)
{
	m_fWidth = _width;
	m_fHeight = _height;	
}

void UCameraComponent::SetPerspective(float _fov, float _aspect, float _near, float _far)
{
	m_fFov = _fov;
	m_fAspect = _aspect;
	m_fNear = _near;
	m_fFar = _far;
}