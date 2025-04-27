#include "pch.h"
#include "AActor.h"
#include "Device.h"

void AActor::Init()
{
	CreateConstantBuffer();
}

void AActor::Tick()
{
	UpdateWorldMatrix();

	if (m_pMesh)
	{
		m_pMesh->Tick();
	}

}

void AActor::PreRender()
{
	m_cbData.matWorld = m_matWorld;
	Matrix view = Matrix::CreateLookAt(
		Vec3(0, 0, -5), Vec3(0, 0, 0), Vec3(0, 1, 0));
	Matrix proj = Matrix::CreatePerspectiveFieldOfView(
		3.141592f / 4.0f, 800.0f / 600.0f, 0.1f, 100.0f);

	m_cbData.matView = view;
	m_cbData.matProj = proj;
}

void AActor::Render()
{
	PreRender();

	if (m_pMesh)
	{
		m_pMesh->Render();
	}

	PostRender();
}

void AActor::PostRender()
{
	DC->UpdateSubresource(m_pConstantBuffer.Get(), 0, NULL, &m_cbData, 0, 0);
	DC->VSSetConstantBuffers(0, 1, m_pConstantBuffer.GetAddressOf());
}

void AActor::Destroy()
{
	if (m_pMesh)
	{
		m_pMesh->Destroy();
	}
}

bool AActor::CreateConstantBuffer()
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
		DEVICE->CreateBuffer(&bd, &sd, m_pConstantBuffer.GetAddressOf());

	if (FAILED(hr))
	{
		DX_CHECK(hr, _T("CreateConstantBuffer Failed"));
		return false;
	}

	return true;
}

void AActor::UpdateWorldMatrix()
{
	m_matScale.CreateScale(m_vScale);

	m_matRotation  = Matrix::CreateRotationZ(m_vRotation.z);
	m_matRotation *= Matrix::CreateRotationX(m_vRotation.x);
	m_matRotation *= Matrix::CreateRotationY(m_vRotation.y);

	m_matTrans = Matrix::CreateTranslation(m_vPosition);

	m_matWorld = m_matOffset * m_matScale * m_matRotation * m_matTrans * m_matParent;

	m_vLook.x  = m_matWorld._31; m_vLook.y  = m_matWorld._32; m_vLook.z  = m_matWorld._33;
	m_vRight.x = m_matWorld._11; m_vRight.y = m_matWorld._12; m_vRight.z = m_matWorld._13;
	m_vUp.x    = m_matWorld._21; m_vUp.y    = m_matWorld._22; m_vUp.z    = m_matWorld._23;

	m_vLook.Normalize();
	m_vRight.Normalize();
	m_vUp.Normalize();
}
