#include "pch.h"
#include "Camera.h"

void Camera::Render()
{

}

void Camera::CreateCameraBuffer()
{
	D3D11_BUFFER_DESC pDesc;
	ZeroMemory(&pDesc, sizeof(pDesc));
	pDesc.ByteWidth = sizeof(Matrix);
	pDesc.Usage = D3D11_USAGE_DEFAULT;
	pDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	HRESULT hr = DEVICE->CreateBuffer(&pDesc, nullptr, m_pCameraBuffer.GetAddressOf());

	if (FAILED(hr))
	{
		DX_CHECK(hr, _T(__FUNCTION__));
		assert(false);
	}
}
