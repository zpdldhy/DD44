#include "pch.h"
#include "AActor.h"
#include "Device.h"
#include "UScriptComponent.h"

void AActor::Init()
{
	CreateConstantBuffer();

	for (auto& component : m_arrComponent)
	{
		if (component)
		{
			component->SetOwner(this);
			component->Init();
		}
	}

	for (auto& script : m_vScript)
	{
		script->Init();
		script->SetOwner(this);
	}
}

void AActor::Tick()
{
	UpdateWorldMatrix();

	for (auto& component : m_arrComponent)
	{
		if (component)
		{
			component->Tick();
		}
	}

	for (auto& script : m_vScript)
	{
		script->Tick();
	}
}

void AActor::Render()
{
	if (m_pWorldCB)
	{
		DC->UpdateSubresource(m_pWorldCB.Get(), 0, NULL, &m_cbData, 0, 0);
		DC->VSSetConstantBuffers(0, 1, m_pWorldCB.GetAddressOf());
	}

	for (auto& component : m_arrComponent)
	{
		if (component)
		{
			component->Render();
		}
	}

	for (auto& script : m_vScript)
	{
		script->Render();
	}
}

void AActor::Destroy()
{
	for (auto& component : m_arrComponent)
	{
		if (component)
		{
			component->Destroy();
		}
	}

	for (auto& script : m_vScript)
	{
		script->Destroy();
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
		DEVICE->CreateBuffer(&bd, &sd, m_pWorldCB.GetAddressOf());

	if (FAILED(hr))
	{
		DX_CHECK(hr, _T("CreateWorldBuffer Failed"));
		return false;
	}

	return true;
}

void AActor::UpdateWorldMatrix()
{
	m_matScale = Matrix::CreateScale(m_vScale);

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

	m_cbData.matWorld = m_matWorld;
}
