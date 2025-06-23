#include "pch.h"
#include "UCameraComponent.h"
#include "UStaticMeshComponent.h"
#include "UMaterial.h"
#include "Device.h"
#include "DxState.h"

ComPtr<ID3D11Buffer> UCameraComponent::m_pFrustumCB = nullptr;

void UCameraComponent::Init()
{
	USceneComponent::Init();

	UpdateView();
	UpdateProjection();

#ifndef DEBUG
	if (m_ProjectionType == ProjectionType::PT_PERSPECTIVE&& g_bRangeVisibleMode)
		CreateFrustumBox();
#endif // DEBUG
}

void UCameraComponent::Tick()
{
	USceneComponent::Tick();

	UpdateView();
	UpdateProjection();

#ifndef DEBUG
	if (m_ProjectionType == ProjectionType::PT_PERSPECTIVE && g_bRangeVisibleMode)
		UpdateFrustumBox();
#endif // DEBUG
}

void UCameraComponent::Render()
{
#ifndef DEBUG
	if (m_ProjectionType == ProjectionType::PT_PERSPECTIVE && m_bVisibleFrustumBox == true && g_bRangeVisibleMode)
	{
		if (m_pCurrentRasterizer)
			m_pCurrentRasterizer.Reset();

		DC->RSGetState(m_pCurrentRasterizer.GetAddressOf());
		DC->RSSetState(STATE->m_pRSWireFrame.Get());

		UpdateFrustumBuffer();
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
	// 근단면
	vector<PNCT_VERTEX> vertexList;
	vertexList.resize(24);

	Vec3 vMin = Vec3(-1.f, -1.f, 0.f);
	Vec3 vMax = Vec3(1.f, 1.f, 1.f);

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
	pMaterial->Load(L"", L"../Resources/Shader/CameraFrustum.hlsl");
	pMaterial->SetUseEffect(false);
	pMesh->SetMaterial(pMaterial);

	m_pFrustumBox->SetMeshComponent(pMesh);
	m_pFrustumBox->SetPosition(m_vWorldPosition);
	m_pFrustumBox->SetRotation(m_vWorldRotation);		

	m_pFrustumBox->Init();	

	CreateFrustumBuffer();
}

void UCameraComponent::CreateFrustumBuffer()
{
	if (m_pFrustumCB != nullptr)
		return;

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.ByteWidth = sizeof(m_tFrustumData);
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	D3D11_SUBRESOURCE_DATA sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.pSysMem = &m_tFrustumData;

	HRESULT hr = DEVICE->CreateBuffer(&bd, &sd, m_pFrustumCB.GetAddressOf());

	if (FAILED(hr))
	{
		DX_CHECK(hr, _T("CreateUISliceBuffer Failed"));
	}
}

void UCameraComponent::UpdateFrustumBox()
{
	m_pFrustumBox->SetPosition(m_vWorldPosition);
	m_pFrustumBox->SetRotation(m_vWorldRotation);

	m_pFrustumBox->Tick();

	m_bVisibleFrustumBox = true;
}

void UCameraComponent::UpdateFrustumBuffer()
{
	if (m_pFrustumCB)
	{
		m_tFrustumData.matViewProjInvert = (m_matView * m_matProjection).Invert();
		DC->UpdateSubresource(m_pFrustumCB.Get(), 0, nullptr, &m_tFrustumData, 0, 0);
		DC->VSSetConstantBuffers(3, 1, m_pFrustumCB.GetAddressOf());
	}
}

void UCameraComponent::UpdateView()
{
	Vec3 vUp = Vec3(0.f, 1.f, 0.f);

	if (m_ViewType == ViewType::VT_LOOKAT)
	{
		m_matView = DirectX::XMMatrixLookAtLH(m_vWorldPosition, m_vLookAt, vUp);
		m_vLookTo = m_vLookAt - m_vWorldPosition;
	}
	else if(m_ViewType == ViewType::VT_LOOKTO)
		m_matView = DirectX::XMMatrixLookToLH(m_vWorldPosition, m_vLookTo, vUp);

	m_vLookTo.Normalize();
	Vec3 vRight = vUp.Cross(m_vLookTo);

	// look Vector가 변경됨에 따라 Rotation을 다시 구해야 한다.
	Quaternion qRotation = Quaternion::LookRotation(m_vLookTo, vUp);

	m_vWorldRotation.y = atan2f(m_vLookTo.x, m_vLookTo.z);	// Yaw : 좌우
	m_vWorldRotation.x = asinf(-m_vLookTo.y);				// pitch : 상하	

	// WorldRotation -> LocalRotation
	m_matWorldRotation = Matrix::CreateRotationZ(m_vWorldRotation.z);
	m_matWorldRotation *= Matrix::CreateRotationX(m_vWorldRotation.x);
	m_matWorldRotation *= Matrix::CreateRotationY(m_vWorldRotation.y);	

	auto matWorld = m_matWorldScale * m_matWorldRotation * m_matWorldTranslation;

	Matrix matLocal = Matrix::Identity;
	if (m_pParentTransform)
		matLocal = matWorld * m_matParent.Invert();

	Vec3 scale, pos;
	Quaternion qRot;
	bool success = matLocal.Decompose(scale, qRot, pos);

	if (success)
	{
		m_vLocalRotation = qRot.ToEuler();
	}
}

void UCameraComponent::UpdateProjection()
{
	if (m_ProjectionType == ProjectionType::PT_ORTHOGRAPHIC)
		m_matProjection = DirectX::XMMatrixOrthographicLH(m_fWidth, m_fHeight, m_fNear, m_fFar);
	else if (m_ProjectionType == ProjectionType::PT_PERSPECTIVE)
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