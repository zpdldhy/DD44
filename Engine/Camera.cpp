#include "pch.h"
#include "Camera.h"

void Camera::Tick()
{
	UpdateView();
}

void Camera::Render(ProjectionType _projectionType)
{
	if (_projectionType == ProjectionType::PT_ORTHOGRAPHIC)
		UpdateOrthographicProjection();
	else if (_projectionType == ProjectionType::PT_PERSPECTIVE)
		UpdatePersPectiveProjection();

	DC->UpdateSubresource(m_pCameraBuffer.Get(), 0, nullptr, &m_CameraData, 0, 0);
	DC->VSSetConstantBuffers(1, 1, m_pCameraBuffer.GetAddressOf());
}

void Camera::CreateCameraBuffer()
{
	D3D11_BUFFER_DESC pDesc;
	ZeroMemory(&pDesc, sizeof(pDesc));
	pDesc.ByteWidth = sizeof(CameraConstantData);
	pDesc.Usage = D3D11_USAGE_DEFAULT;
	pDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	HRESULT hr = DEVICE->CreateBuffer(&pDesc, nullptr, m_pCameraBuffer.GetAddressOf());

	if (FAILED(hr))
	{
		DX_CHECK(hr, _T(__FUNCTION__));
		assert(false);
	}
}

void Camera::UpdateOrthographicProjection()
{	
	m_CameraData.matProjection = DirectX::SimpleMath::Matrix::CreateOrthographic(m_fWidth, m_fHeight, 0.1f, 1000.0f);
}

void Camera::UpdatePersPectiveProjection()
{
	m_CameraData.matProjection = DirectX::SimpleMath::Matrix::CreatePerspectiveFieldOfView(DirectX::XMConvertToRadians(m_fFov), m_fAspect, 0.1f, 1000.0f);
}

void Camera::SetOrthographic(float _width, float _height)
{
	m_fWidth = _width;
	m_fHeight = _height;	
}

void Camera::SetPerspective(float _fov, float _aspect, float _near, float _far)
{
	m_fFov = _fov;
	m_fAspect = _aspect;
	m_fNear = _near;
	m_fFar = _far;
}

Camera::Camera()
{
	CreateCameraBuffer();
}
