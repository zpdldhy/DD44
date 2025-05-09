#include "pch.h"
#include "USceneComponent.h"
#include "Device.h"

void USceneComponent::Init()
{
	CreateConstantBuffer();
}

void USceneComponent::Tick()
{
	UpdateWorldMatrix();
}

void USceneComponent::Render()
{
	if (m_pWorldCB)
	{
		DC->UpdateSubresource(m_pWorldCB.Get(), 0, NULL, &m_cbData, 0, 0);
		DC->VSSetConstantBuffers(0, 1, m_pWorldCB.GetAddressOf());
	}
}

void USceneComponent::Destroy()
{
}


bool USceneComponent::CreateConstantBuffer()
{
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.ByteWidth = sizeof(cbData);
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	D3D11_SUBRESOURCE_DATA sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.pSysMem = &m_cbData;

	HRESULT hr =
		DEVICE->CreateBuffer(&bd, &sd, m_pWorldCB.GetAddressOf());

	if (FAILED(hr))
	{
		DX_CHECK(hr, _T("CreateWorldBuffer Failed"));
		return false;
	}

	return true;
}

void USceneComponent::UpdateWorldMatrix()
{
	// Set Local
	m_matScale = Matrix::CreateScale(m_vLocalScale);

	m_matRotation = Matrix::CreateRotationZ(m_vLocalRight.z);
	m_matRotation *= Matrix::CreateRotationX(m_vLocalRight.x);
	m_matRotation *= Matrix::CreateRotationY(m_vLocalRight.y);

	m_matTrans = Matrix::CreateTranslation(m_vLocalPosition);

	// Set Matrix
	m_matLocal = m_matScale * m_matRotation * m_matTrans;
	m_matWorld = m_matLocal * m_matParent;

	// Set Local
	m_vLocalLook.x = m_matWorld._31; m_vLocalLook.y = m_matWorld._32; m_vLocalLook.z = m_matWorld._33;
	m_vLocalRight.x = m_matWorld._11; m_vLocalRight.y = m_matWorld._12; m_vLocalRight.z = m_matWorld._13;
	m_vLocalUp.x = m_matWorld._21; m_vLocalUp.y = m_matWorld._22; m_vLocalUp.z = m_matWorld._23;

	m_vLocalLook.Normalize();
	m_vLocalRight.Normalize();
	m_vLocalUp.Normalize();

	// Set World
	m_vWorldLook.x = m_matWorld._31; m_vWorldLook.y = m_matWorld._32; m_vWorldLook.z = m_matWorld._33;
	m_vWorldRight.x = m_matWorld._11; m_vWorldRight.y = m_matWorld._12; m_vWorldRight.z = m_matWorld._13;
	m_vWorldUp.x = m_matWorld._21; m_vWorldUp.y = m_matWorld._22; m_vWorldUp.z = m_matWorld._23;

	m_vWorldLook.Normalize();
	m_vWorldRight.Normalize();
	m_vWorldUp.Normalize();

	m_vWorldScale = m_vLocalScale * m_vParentScale;
	m_vWorldRotation = m_vLocalRotation + m_vParentRotation;
	m_vWorldPosition = m_vLocalPosition + m_vParentPosition;

	m_cbData.matWorld = m_matWorld;
}
