#include "pch.h"
#include "AUIActor.h"
#include "AInstance.h"
#include "UMaterial.h"
#include "DxWrite.h"
#include "UStaticMeshComponent.h"

void AUIActor::Init()
{
	AActor::Init();

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

	m_pSliceActor->Render();
}

void AUIActor::TextRender()
{
	Vec2 pos;
	pos.x = GetPosition().x + static_cast<float>(g_windowSize.x) / 2.0f;
	pos.y = -(GetPosition().y - static_cast<float>(g_windowSize.y) / 2.0f);
	m_pText->Draw(pos);
}

void AUIActor::CreateUVSlice()
{
	m_pSliceActor = make_shared<AInstance>();

	float u[4] = { 0.f, 0.5f, 0.5f, 1.f };
	float v[4] = { 0.f, 0.5f, 0.5f, 1.f };

	m_vSliceMesh.resize(9);

	// 9개의 Mesh에 Instance Data 입력
	for (int iRow = 0; iRow < 3; iRow++)
	{
		for (int iCol = 0; iCol < 3; iCol++)
		{
			m_vSliceMesh[3 * iRow + iCol] = UStaticMeshComponent::CreatePlane();
			m_vSliceMesh[3 * iRow + iCol]->SetOwner(shared_from_this());

			m_vSliceMesh[3 * iRow + iCol]->SetInstanceColor(m_Color);
			m_vSliceMesh[3 * iRow + iCol]->SetInstanceStartUV(Vec2(u[iCol], v[iRow]));
			m_vSliceMesh[3 * iRow + iCol]->SetInstanceEndUV(Vec2(u[iCol + 1], v[iRow + 1]));

			// 얘꺼 바뀌면 다 바뀜
			m_vSliceMesh[3 * iRow + iCol]->SetMaterial(GetMeshComponent()->GetMaterial());

			if (iRow == 0 && iCol == 0)
				m_pSliceActor->SetInstanceMesh(m_vSliceMesh[3 * iRow + iCol]);
			else
				m_pSliceActor->AddInstanceMesh(m_vSliceMesh[3 * iRow + iCol]);
		}
	}
}

void AUIActor::UpdateUVSlice()
{
	auto scale = GetScale();
	auto pos = GetPosition();

	// Scale
	float SliceScaleX[3];
	float SliceScaleY[3];

	SliceScaleX[0] = scale.x * m_vSlice.x;
	SliceScaleX[1] = scale.x * (1.f - (m_vSlice.x + m_vSlice.y));
	SliceScaleX[2] = scale.x * m_vSlice.y;

	SliceScaleY[0] = scale.y * m_vSlice.z;
	SliceScaleY[1] = scale.y * (1.f - (m_vSlice.z + m_vSlice.w));
	SliceScaleY[2] = scale.y * m_vSlice.w;

	// Position, Rotation이 적용된 위치를 받아야 한다...ㅅㅂ
	float SlicePosX[3];
	float SlicePosY[3];

	SlicePosX[0] = pos.x - scale.x * (0.5f - (m_vSlice.x / 2.f));
	SlicePosX[2] = pos.x + scale.x * (0.5f - (m_vSlice.y / 2.f));
	SlicePosX[1] = ((SlicePosX[0] + (SliceScaleX[0] / 2.f)) + (SlicePosX[2] - (SliceScaleX[2] / 2.f))) / 2.f;

	SlicePosY[0] = pos.y + scale.y * (0.5f - (m_vSlice.z / 2.f));
	SlicePosY[2] = pos.y - scale.y * (0.5f - (m_vSlice.w / 2.f));
	SlicePosY[1] = ((SlicePosY[0] - (SliceScaleY[0] / 2.f)) + (SlicePosY[2] + (SliceScaleY[2] / 2.f))) / 2.f;

	for (int iRow = 0; iRow < 3; iRow++)
	{
		for (int iCol = 0; iCol < 3; iCol++)
		{
			m_vSliceMesh[3 * iRow + iCol]->SetLocalScale(Vec3(SliceScaleX[iCol], SliceScaleY[iRow], 1.f));
			m_vSliceMesh[3 * iRow + iCol]->SetLocalPosition(Vec3(SlicePosX[iCol], SlicePosY[iRow], pos.z));
			m_vSliceMesh[3 * iRow + iCol]->SetInstanceColor(m_Color);
		}
	}
}

void AUIActor::CreateText()
{
	m_pText = make_unique<Text>();
	m_pFont = make_unique<Font>();
	m_pFont->Create(m_szFontPath, 30.0f, m_Color);
	UpdateText();
}

void AUIActor::UpdateText()
{
	if (!DXWRITE->m_pDxWrite)
		return;

	m_pFont->SetPath(m_szFontPath);
	m_pFont->SetSize(m_fFontSize);
	m_pFont->SetColor(m_Color);
	m_pFont->SetAlignment(m_TextAlignment, m_ParaAlignment);
	Vec2 scale(GetScale().x, GetScale().y);
	m_pText->Create(m_szText, scale, m_pFont.get());
}

