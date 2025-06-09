#include "pch.h"
#include "AUIActor.h"
#include "UMaterial.h"
#include "DxWrite.h"

ComPtr<ID3D11Buffer> AUIActor::m_pUICB = nullptr;

void AUIActor::Init()
{
	//if (m_bSliceActor == false)
		//m_bRender = false;

	AActor::Init();
	CreateUIData();
	CreateUVSlice();
	CreateText();
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

	if (m_bTextUI)
		UpdateText();

	AActor::Tick();
}

void AUIActor::Render()
{
	if (m_bTextUI)
	{
		TextRender();
		return;
	}

	if (m_pUICB)
	{
		DC->UpdateSubresource(m_pUICB.Get(), 0, nullptr, &m_tUISliceData, 0, 0);
		DC->PSSetConstantBuffers(3, 1, m_pUICB.GetAddressOf());
	}

	AActor::Render();
}

void AUIActor::TextRender()
{
	Vec2 pos;
	pos.x = GetPosition().x + static_cast<float>(g_windowSize.x) / 2.0f;
	pos.y = -(GetPosition().y - static_cast<float>(g_windowSize.y) / 2.0f);
	m_pText->Draw(pos);
}

void AUIActor::CreateUIData()
{
	if (m_pUICB != nullptr)
		return;

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.ByteWidth = sizeof(UIData);
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	D3D11_SUBRESOURCE_DATA sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.pSysMem = &m_tUISliceData;

	HRESULT hr = DEVICE->CreateBuffer(&bd, &sd, m_pUICB.GetAddressOf());

	if (FAILED(hr))
	{
		DX_CHECK(hr, _T("CreateUISliceBuffer Failed"));
	}
}

void AUIActor::CreateUVSlice()
{
	// SliceActor면 생성하지 말것
	if (m_bSliceActor == true)
		return;

	// 9개의 Mesh를 생성
	m_vSliceActor.resize(9);

	for (int i = 0; i < 9; i++)
	{
		m_vSliceActor[i] = make_shared<AUIActor>();
		m_vSliceActor[i]->m_bSliceActor = true;
	}
}

void AUIActor::CreateText()
{
	m_pText = make_unique<Text>();
	m_pFont = make_unique<Font>();
	m_pFont->Create(m_szFontPath, 30.0f, m_tUISliceData.vColor);
	UpdateText();
}

void AUIActor::UpdateText()
{	
	if (!DXWRITE->m_pDxWrite)
		return;

	m_pFont->SetPath(m_szFontPath);
	m_pFont->SetSize(m_fFontSize);
	m_pFont->SetColor(m_tUISliceData.vColor);
	m_pFont->SetAlignment(DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	Vec2 scale(GetScale().x, GetScale().y);
	m_pText->Create(m_szText, scale, m_pFont.get());
}

