#include "pch.h"
#include "AUIActor.h"
#include "UMaterial.h"
#include "DxWrite.h"
#include "UStaticMeshComponent.h"

ComPtr<ID3D11Buffer> AUIActor::m_pUICB = nullptr;

void AUIActor::Init()
{
	AActor::Init();

	CreateUIData();

	if (m_bSliceActor == false)
	{
		CreateUVSlice();
	}

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

	if (m_vSlice.x < 0.5f || m_vSlice.y < 0.5f || m_vSlice.z < 0.5f || m_vSlice.w < 0.5f)
		m_bUseSliceActor = true;
	else
		m_bUseSliceActor = false;

	if (m_bUseSliceActor == true && m_bSliceActor == false)
		UpdateUVSlice();

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

	if (m_pUICB && m_bSliceActor == false)
	{
		DC->UpdateSubresource(m_pUICB.Get(), 0, nullptr, &m_tUISliceData, 0, 0);
		DC->PSSetConstantBuffers(3, 1, m_pUICB.GetAddressOf());
	}

	if (m_bUseSliceActor == true && m_bSliceActor == false)
		for (auto& pSlice : m_vSliceActor)
			pSlice->Render();

	if (m_bUseSliceActor == false || m_bSliceActor == true)
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
	// 9개의 Mesh를 생성
	m_vSliceActor.resize(9);

	float u[4] = { 0.f, 0.5f, 0.5f, 1.f };
	float v[4] = { 0.f, 0.5f, 0.5f, 1.f };

	for (int iRow = 0; iRow < 3; iRow++)
	{
		for (int iCol = 0; iCol < 3; iCol++) 
		{
			m_vSliceActor[3 * iRow + iCol] = make_shared<AUIActor>();

			// Mesh Set
			auto pMeshComponent = make_shared<UStaticMeshComponent>();
			{
				vector<PNCT_VERTEX> vVertexList;
				vector<DWORD> vIndexList;

				vVertexList.resize(4);
				vIndexList.resize(6);

				Vec3 vMin = Vec3(-0.5f, -0.5f, -0.f);
				Vec3 vMax = Vec3(+0.5f, +0.5f, +0.f);

				vVertexList[0] = PNCT_VERTEX(Vec3(vMin.x, vMin.y, vMin.z), Vec3(0, 0, -1), Vec4(1, 0, 0, 1), Vec2(u[iCol], v[iRow + 1]));
				vVertexList[1] = PNCT_VERTEX(Vec3(vMin.x, vMax.y, vMax.z), Vec3(0, 0, -1), Vec4(1, 0, 0, 1), Vec2(u[iCol], v[iRow]));
				vVertexList[2] = PNCT_VERTEX(Vec3(vMax.x, vMax.y, vMax.z), Vec3(0, 0, -1), Vec4(1, 0, 0, 1), Vec2(u[iCol + 1], v[iRow]));
				vVertexList[3] = PNCT_VERTEX(Vec3(vMax.x, vMin.y, vMin.z), Vec3(0, 0, -1), Vec4(1, 0, 0, 1), Vec2(u[iCol + 1], v[iRow + 1]));

				int iIndex = 0;
				vIndexList[iIndex++] = 0; vIndexList[iIndex++] = 1; vIndexList[iIndex++] = 3;
				vIndexList[iIndex++] = 1; vIndexList[iIndex++] = 2; vIndexList[iIndex++] = 3;

				// Mesh Setting
				shared_ptr<UStaticMeshResources> pMesh = nullptr;
				pMesh = make_shared<UStaticMeshResources>();
				pMesh->SetVertexList(vVertexList);
				pMesh->SetIndexList(vIndexList);
				pMesh->Create();

				pMeshComponent->SetMesh(pMesh);
			}
			m_vSliceActor[3 * iRow + iCol]->SetMeshComponent(pMeshComponent);

			auto mat = GetMeshComponent()->GetMaterial();
			pMeshComponent->SetMaterial(mat);

			m_vSliceActor[3 * iRow + iCol]->SetIdleTexture(m_pIdleTexture);
			m_vSliceActor[3 * iRow + iCol]->SetHoverTexture(m_pHoverTexture);
			m_vSliceActor[3 * iRow + iCol]->SetActiveTexture(m_pActiveTexture);
			m_vSliceActor[3 * iRow + iCol]->SetSelectTexture(m_pSelectTexture);

			m_vSliceActor[3 * iRow + iCol]->m_bSliceActor = true;
			m_vSliceActor[3 * iRow + iCol]->Init();
		}
	}
}

void AUIActor::UpdateUVSlice()
{
	if (m_bSliceActor == true)
		return;

	auto scale = GetScale();
	auto pos = GetPosition();

	float SliceScaleX[3];
	float SliceScaleY[3];

	SliceScaleX[0] = scale.x * m_vSlice.x;
	SliceScaleX[1] = scale.x * (1.f - (m_vSlice.x + m_vSlice.y));
	SliceScaleX[2] = scale.x * m_vSlice.y;

	SliceScaleY[0] = scale.y * m_vSlice.z;
	SliceScaleY[1] = scale.y * (1.f - (m_vSlice.z + m_vSlice.w));
	SliceScaleY[2] = scale.y * m_vSlice.w;

	float SlicePosX[3];
	float SlicePosY[3];

	SlicePosX[0] = pos.x - scale.x * (0.5f - (m_vSlice.x / 2.f));
	SlicePosX[1] = pos.x;
	SlicePosX[2] = pos.x + scale.x * (0.5f - (m_vSlice.y / 2.f));

	SlicePosY[0] = pos.y + scale.y * (0.5f - (m_vSlice.z / 2.f));
	SlicePosY[1] = pos.y;
	SlicePosY[2] = pos.y - scale.y * (0.5f - (m_vSlice.w / 2.f));

	for (int iRow = 0; iRow < 3; iRow++)
	{
		for (int iCol = 0; iCol < 3; iCol++)
		{
			m_vSliceActor[3 * iRow + iCol]->SetScale(Vec3(SliceScaleX[iCol], SliceScaleY[iRow], 1.f));
			m_vSliceActor[3 * iRow + iCol]->SetPosition(Vec3(SlicePosX[iCol], SlicePosY[iRow], pos.z));
			m_vSliceActor[3 * iRow + iCol]->SetStateType(m_eStateType);
		}
	}
	
	for (auto& pSlice : m_vSliceActor)
		pSlice->Tick();
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

