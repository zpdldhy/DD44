#include "pch.h"
#include "UCameraComponent.h"
#include "UStaticMeshComponent.h"
#include "UMaterial.h"
#include "Device.h"
#include "DxState.h"

void UCameraComponent::Init()
{
	UpdateView();

	if (m_ProjectionType == ProjectionType::PT_ORTHOGRAPHIC)
		UpdateOrthographicProjection();
	else if (m_ProjectionType == ProjectionType::PT_PERSPECTIVE)
		UpdatePersPectiveProjection();

#ifndef DEBUG
	if (m_ProjectionType == ProjectionType::PT_PERSPECTIVE)
		CreateFrustumBox();
#endif // DEBUG
}

void UCameraComponent::Tick()
{
	UpdateView();

	if (m_ProjectionType == ProjectionType::PT_ORTHOGRAPHIC)
		UpdateOrthographicProjection();
	else if (m_ProjectionType == ProjectionType::PT_PERSPECTIVE)
		UpdatePersPectiveProjection();

#ifndef DEBUG
	if (m_ProjectionType == ProjectionType::PT_PERSPECTIVE)
		UpdateFrustumBox();
#endif // DEBUG
}

void UCameraComponent::Render()
{
#ifndef DEBUG
	if (m_ProjectionType == ProjectionType::PT_PERSPECTIVE && m_bVisibleFrustumBox == true)
	{
		if (m_pCurrentRasterizer)
			m_pCurrentRasterizer.Reset();

		DC->RSGetState(m_pCurrentRasterizer.GetAddressOf());
		DC->RSSetState(STATE->m_pRSWireFrame.Get());

		m_pFrustumBox->Render();

		DC->RSSetState(m_pCurrentRasterizer.Get());

		m_pCurrentRasterizer.Reset();
	}
#endif // DEBUG
}

void UCameraComponent::Destroy()
{
}

void UCameraComponent::CreateFrustumBox()
{
	m_pFrustumBox = make_shared<AActor>();	

	auto pMesh = make_shared<UStaticMeshComponent>();	
	auto pMeshData = make_shared<UStaticMeshResources>();

	// Set VertexList
	// ±Ù´Ü¸é
	vector<PNCT_VERTEX> vertexList;
	vertexList.resize(24);

	Vec3 vMin = Vec3(-1.f, -1.f, 0.f);
	Vec3 vMax = Vec3(1.f, 1.f, 1.f);

	Matrix matViewProj;
	(m_matView * m_matProjection).Invert(matViewProj);

	// Front
	vertexList[0] = PNCT_VERTEX(Vec3(vMin.x, vMin.y, vMin.z), Vec3(0.f, 0.f, +1.f), Vec4(0.f, 0.f, 1.f, 1.f), Vec2(0.f, 0.f));
	vertexList[1] = PNCT_VERTEX(Vec3(vMin.x, vMax.y, vMin.z), Vec3(0.f, 0.f, +1.f), Vec4(0.f, 0.f, 1.f, 1.f), Vec2(0.f, 0.f));
	vertexList[2] = PNCT_VERTEX(Vec3(vMax.x, vMax.y, vMin.z), Vec3(0.f, 0.f, +1.f), Vec4(0.f, 0.f, 1.f, 1.f), Vec2(0.f, 0.f));
	vertexList[3] = PNCT_VERTEX(Vec3(vMax.x, vMin.y, vMin.z), Vec3(0.f, 0.f, +1.f), Vec4(0.f, 0.f, 1.f, 1.f), Vec2(0.f, 0.f));
	// Back
	vertexList[4] = PNCT_VERTEX(Vec3(vMin.x, vMin.y, vMax.z), Vec3(0.f, 0.f, -1.f), Vec4(0.f, 0.f, 1.f, 1.f), Vec2(0.f, 0.f));
	vertexList[5] = PNCT_VERTEX(Vec3(vMin.x, vMax.y, vMax.z), Vec3(0.f, 0.f, -1.f), Vec4(0.f, 0.f, 1.f, 1.f), Vec2(0.f, 0.f));
	vertexList[6] = PNCT_VERTEX(Vec3(vMax.x, vMax.y, vMax.z), Vec3(0.f, 0.f, -1.f), Vec4(0.f, 0.f, 1.f, 1.f), Vec2(0.f, 0.f));
	vertexList[7] = PNCT_VERTEX(Vec3(vMax.x, vMin.y, vMax.z), Vec3(0.f, 0.f, -1.f), Vec4(0.f, 0.f, 1.f, 1.f), Vec2(0.f, 0.f));
	// Left
	vertexList[8] = PNCT_VERTEX(Vec3(vMin.x, vMin.y, vMax.z), Vec3(+1.f, 0.f, 0.f), Vec4(0.f, 0.f, 1.f, 1.f), Vec2(0.f, 0.f));
	vertexList[9] = PNCT_VERTEX(Vec3(vMin.x, vMax.y, vMax.z), Vec3(+1.f, 0.f, 0.f), Vec4(0.f, 0.f, 1.f, 1.f), Vec2(0.f, 0.f));
	vertexList[10] = PNCT_VERTEX(Vec3(vMin.x, vMax.y, vMin.z), Vec3(+1.f, 0.f, 0.f), Vec4(0.f, 0.f, 1.f, 1.f), Vec2(0.f, 0.f));
	vertexList[11] = PNCT_VERTEX(Vec3(vMin.x, vMin.y, vMin.z), Vec3(+1.f, 0.f, 0.f), Vec4(0.f, 0.f, 1.f, 1.f), Vec2(0.f, 0.f));
	// Right																							 
	vertexList[12] = PNCT_VERTEX(Vec3(vMax.x, vMin.y, vMin.z), Vec3(-1.f, 0.f, 0.f), Vec4(0.f, 0.f, 1.f, 1.f), Vec2(0.f, 0.f));
	vertexList[13] = PNCT_VERTEX(Vec3(vMax.x, vMax.y, vMin.z), Vec3(-1.f, 0.f, 0.f), Vec4(0.f, 0.f, 1.f, 1.f), Vec2(0.f, 0.f));
	vertexList[14] = PNCT_VERTEX(Vec3(vMax.x, vMax.y, vMax.z), Vec3(-1.f, 0.f, 0.f), Vec4(0.f, 0.f, 1.f, 1.f), Vec2(0.f, 0.f));
	vertexList[15] = PNCT_VERTEX(Vec3(vMax.x, vMin.y, vMax.z), Vec3(-1.f, 0.f, 0.f), Vec4(0.f, 0.f, 1.f, 1.f), Vec2(0.f, 0.f));
	// UP																								 
	vertexList[16] = PNCT_VERTEX(Vec3(vMin.x, vMax.y, vMin.z), Vec3(0.f, -1.f, 0.f), Vec4(0.f, 0.f, 1.f, 1.f), Vec2(0.f, 0.f));
	vertexList[17] = PNCT_VERTEX(Vec3(vMin.x, vMax.y, vMax.z), Vec3(0.f, -1.f, 0.f), Vec4(0.f, 0.f, 1.f, 1.f), Vec2(0.f, 0.f));
	vertexList[18] = PNCT_VERTEX(Vec3(vMax.x, vMax.y, vMax.z), Vec3(0.f, -1.f, 0.f), Vec4(0.f, 0.f, 1.f, 1.f), Vec2(0.f, 0.f));
	vertexList[19] = PNCT_VERTEX(Vec3(vMax.x, vMax.y, vMin.z), Vec3(0.f, -1.f, 0.f), Vec4(0.f, 0.f, 1.f, 1.f), Vec2(0.f, 0.f));
	// Bottom																							
	vertexList[20] = PNCT_VERTEX(Vec3(vMin.x, vMin.y, vMax.z), Vec3(0.f, +1.f, 0.f), Vec4(0.f, 0.f, 1.f, 1.f), Vec2(0.f, 0.f));
	vertexList[21] = PNCT_VERTEX(Vec3(vMin.x, vMin.y, vMin.z), Vec3(0.f, +1.f, 0.f), Vec4(0.f, 0.f, 1.f, 1.f), Vec2(0.f, 0.f));
	vertexList[22] = PNCT_VERTEX(Vec3(vMax.x, vMin.y, vMin.z), Vec3(0.f, +1.f, 0.f), Vec4(0.f, 0.f, 1.f, 1.f), Vec2(0.f, 0.f));
	vertexList[23] = PNCT_VERTEX(Vec3(vMax.x, vMin.y, vMax.z), Vec3(0.f, +1.f, 0.f), Vec4(0.f, 0.f, 1.f, 1.f), Vec2(0.f, 0.f));

	for (int i = 0; i < 24; i++)
	{		
		vertexList[i].pos = Vec3::Transform(vertexList[i].pos, matViewProj);
	}

	pMeshData->SetVertexList(vertexList);

	// Set IndexList
	vector<DWORD> indexList;
	indexList.resize(36);

	UINT iIndex = 0;
	auto& I = indexList;
	// Front
	I[iIndex++] = 0; I[iIndex++] = 1; I[iIndex++] = 2;
	I[iIndex++] = 0; I[iIndex++] = 2; I[iIndex++] = 3;
	// Back
	I[iIndex++] = 6; I[iIndex++] = 5; I[iIndex++] = 4;
	I[iIndex++] = 7; I[iIndex++] = 6; I[iIndex++] = 4;
	// Left
	I[iIndex++] = 8; I[iIndex++] = 9; I[iIndex++] = 10;
	I[iIndex++] = 8; I[iIndex++] = 10; I[iIndex++] = 11;
	// Right
	I[iIndex++] = 12; I[iIndex++] = 13; I[iIndex++] = 14;
	I[iIndex++] = 12; I[iIndex++] = 14; I[iIndex++] = 15;
	// Top
	I[iIndex++] = 16; I[iIndex++] = 17; I[iIndex++] = 18;
	I[iIndex++] = 16; I[iIndex++] = 18; I[iIndex++] = 19;
	// Bottom
	I[iIndex++] = 20; I[iIndex++] = 21; I[iIndex++] = 22;
	I[iIndex++] = 20; I[iIndex++] = 22; I[iIndex++] = 23;

	pMeshData->SetIndexList(indexList);
	pMeshData->Create();
	pMesh->SetMesh(pMeshData);

	// Set Material
	auto pMaterial = make_shared<UMaterial>();
	pMaterial->Load(L"", L"../Resources/Shader/DefaultColor.hlsl");
	pMesh->SetMaterial(pMaterial);

	m_pFrustumBox->SetMeshComponent(pMesh);
	m_pFrustumBox->SetScale(m_vScale);

	m_pFrustumBox->Init();	
}

void UCameraComponent::UpdateFrustumBox()
{
	m_pFrustumBox->SetPosition(m_vEye);
	m_pFrustumBox->SetRotation(m_pOwner.lock()->GetRotation());

	m_pFrustumBox->Tick();

	m_bVisibleFrustumBox = true;
}

void UCameraComponent::UpdateView()
{
	m_vEye = m_pOwner.lock()->GetPosition() + m_vPosition;

	if (Vec3::Distance(Vec3(0.f, 0.f, 0.f), m_vPosition) < 0.1f)
		m_vLook = m_pOwner.lock()->GetLook();
	else
		m_vLook = -m_vPosition;

	Vec3 vUp = Vec3(0.f, 1.f, 0.f);

	m_matView = DirectX::XMMatrixLookAtLH(m_vEye, m_vEye + m_vLook, vUp);
}

void UCameraComponent::UpdateOrthographicProjection()
{	
	m_matProjection = DirectX::XMMatrixOrthographicLH(m_fWidth, m_fHeight, m_fNear, m_fFar);
}

void UCameraComponent::UpdatePersPectiveProjection()
{
	m_matProjection = DirectX::XMMatrixPerspectiveFovLH(m_fFov, m_fAspect, m_fNear, m_fFar);
}

void UCameraComponent::SetOrthographic(float _fWidth, float _fHeight)
{
	m_fWidth = _fWidth;
	m_fHeight = _fHeight;
}

void UCameraComponent::SetPerspective(float _fFov, float _fAspect, float _fNear, float _fFar)
{
	m_fFov = _fFov;
	m_fAspect = _fAspect;
	m_fNear = _fNear;
	m_fFar = _fFar;
}