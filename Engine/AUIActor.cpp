#include "pch.h"
#include "AUIActor.h"
#include "UMaterial.h"

ComPtr<ID3D11Buffer> AUIActor::m_pUISliceCB = nullptr;

void AUIActor::Init()
{
	AActor::Init();
	CreateUISlice();
}

void AUIActor::Tick()
{
	m_pState->ProcessState(this);

	if (m_eStateType == UIStateType::ST_IDLE)
	{
		GetMeshComponent()->GetMaterial()->SetTexture(m_pIdleTexture);
	}
	else if (m_eStateType == UIStateType::ST_HOVER)
	{
		GetMeshComponent()->GetMaterial()->SetTexture(m_pHoverTexture);
	}
	else if (m_eStateType == UIStateType::ST_ACTIVE)
	{
		GetMeshComponent()->GetMaterial()->SetTexture(m_pActiveTexture);
	}
	else if (m_eStateType == UIStateType::ST_SELECT)
	{
		GetMeshComponent()->GetMaterial()->SetTexture(m_pSelectTexture);
	}

	AActor::Tick();
}

void AUIActor::Render()
{
	if (m_pUISliceCB)
	{
		DC->UpdateSubresource(m_pUISliceCB.Get(), 0, nullptr, &m_tUISliceData, 0, 0);
		DC->PSSetConstantBuffers(2, 1, m_pUISliceCB.GetAddressOf());
	}

	AActor::Render();
}

void AUIActor::CreateUISlice()
{
	if (m_pUISliceCB != nullptr)
		return;

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.ByteWidth = sizeof(UISliceData);
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	D3D11_SUBRESOURCE_DATA sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.pSysMem = &m_tUISliceData;

	HRESULT hr = DEVICE->CreateBuffer(&bd, &sd, m_pUISliceCB.GetAddressOf());

	if (FAILED(hr))
	{
		DX_CHECK(hr, _T("CreateUISliceBuffer Failed"));
	}
}