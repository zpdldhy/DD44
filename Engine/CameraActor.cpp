#include "pch.h"
#include "CameraActor.h"

void CameraActor::Tick()
{
	AActor::Tick();

	// Update the camera data
	UpdateViewMatrix();
	UpdateOrthographicProjection();
	UpdatePersPectiveProjection();
}

void CameraActor::PreRender()
{
	m_cameraData.matView = m_matView;
	if (m_projectionType == ProjectionType::PT_ORTHOGRAPHIC)
		m_cameraData.matProjection = m_matOrthographicProjection;
	else
		m_cameraData.matProjection = m_matPerspectiveProjection;
}

void CameraActor::PostRender()
{
	DC->UpdateSubresource(m_pConstantBuffer.Get(), 0, NULL, &m_cameraData, 0, 0);
	DC->VSSetConstantBuffers(1, 1, m_pConstantBuffer.GetAddressOf());
}

bool CameraActor::CreateConstantBuffer()
{
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.ByteWidth = sizeof(CameraConstantData);
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	D3D11_SUBRESOURCE_DATA sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.pSysMem = &m_cameraData;

	HRESULT hr =
		DEVICE->CreateBuffer(&bd, &sd, m_pConstantBuffer.GetAddressOf());

	if (FAILED(hr))
	{
		DX_CHECK(hr, _T("CreateConstantBuffer Failed"));
		return false;
	}

	return true;
}
void CameraActor::UpdateViewMatrix()
{
	// Look Vector를 정면으로 Update한다.
	m_matView =
		DirectX::XMMatrixLookAtLH(m_vPosition, m_vPosition + m_vLook, m_vUp);
}

void CameraActor::UpdateOrthographicProjection()
{	
	m_matOrthographicProjection =
		DirectX::XMMatrixOrthographicLH(m_fWidth, m_fHeight, m_fNear, m_fFar);
}

void CameraActor::UpdatePersPectiveProjection()
{
	m_matPerspectiveProjection = 
		DirectX::XMMatrixPerspectiveFovLH(m_fFov, m_fAspect, m_fNear, m_fFar);
}

void CameraActor::SetOrthographic(float _width, float _height)
{
	m_fWidth = _width;
	m_fHeight = _height;	
}

void CameraActor::SetPerspective(float _fov, float _aspect, float _near, float _far)
{
	m_fFov = _fov;
	m_fAspect = _aspect;
	m_fNear = _near;
	m_fFar = _far;
}