#include "pch.h"
#include "USkinnedComponent.h"
#include "Device.h"
#include "UMaterial.h"

void USkinnedComponent::Init()
{
}

void USkinnedComponent::Tick()
{
}

void USkinnedComponent::Destroy()
{

}

void USkinnedComponent::PreRender()
{
	if (m_pAnim) m_pAnim->Tick();

	if (m_pAnim) m_pAnim->Render(0);

	UINT Strides[2] = { sizeof(PNCT_VERTEX), sizeof(IW_VERTEX) };
	UINT Offsets[2] = { 0, 0 };
	ID3D11Buffer* pVB[2] = { m_pVertexBuffer.Get(), m_pIwBuffer.Get() };

	DC->IASetVertexBuffers(0, 2, pVB, Strides, Offsets);
	DC->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	if (m_pMaterial)
	{
		m_pMaterial->Bind();
	}
}

void USkinnedComponent::PostRender()
{
	if (m_vIndexList.size() <= 0)
	{
		DC->Draw(m_vVertexList.size(), 0);
	}
	else
	{
		DC->DrawIndexed(m_vIndexList.size(), 0, 0);
	}

	for (auto& child : m_vChild)
	{
		child->PreRender();
		child->PostRender();
	}
}

void USkinnedComponent::CreateIwBuffer()
{
	D3D11_BUFFER_DESC pDesc;
	ZeroMemory(&pDesc, sizeof(pDesc));
	pDesc.ByteWidth = sizeof(IW_VERTEX) * m_vIwList.size();
	pDesc.Usage = D3D11_USAGE_DEFAULT;
	pDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA pInitialData;
	ZeroMemory(&pInitialData, sizeof(pInitialData));
	pInitialData.pSysMem = &m_vIwList.at(0);
	HRESULT hr = DEVICE->CreateBuffer(&pDesc, &pInitialData, m_pIwBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		assert(false);
	}
}

void USkinnedComponent::CreateCube()
{
	m_vVertexList.resize(24);
	m_vIndexList.resize(36);

	// Transform 넣기 전 Render를 위해 임시로 값 변경
	Vec3 vMin = Vec3(-0.5f, -0.5f, 0.1f);
	Vec3 vMax = Vec3(+0.5f, +0.5f, 0.6f);

	// Front
	m_vVertexList[0] = PNCT_VERTEX(Vec3(vMin.x, vMin.y, vMin.z), Vec3(0, 0, -1), Vec4(1, 0, 0, 1), Vec2(0, 1));
	m_vVertexList[1] = PNCT_VERTEX(Vec3(vMin.x, vMax.y, vMin.z), Vec3(0, 0, -1), Vec4(1, 0, 0, 1), Vec2(0, 0));
	m_vVertexList[2] = PNCT_VERTEX(Vec3(vMax.x, vMax.y, vMin.z), Vec3(0, 0, -1), Vec4(1, 0, 0, 1), Vec2(1, 0));
	m_vVertexList[3] = PNCT_VERTEX(Vec3(vMax.x, vMin.y, vMin.z), Vec3(0, 0, -1), Vec4(1, 0, 0, 1), Vec2(1, 1));
	// Back
	m_vVertexList[4] = PNCT_VERTEX(Vec3(vMin.x, vMin.y, vMax.z), Vec3(0, 0, 1), Vec4(1, 0, 0, 1), Vec2(1, 1));
	m_vVertexList[5] = PNCT_VERTEX(Vec3(vMin.x, vMax.y, vMax.z), Vec3(0, 0, 1), Vec4(1, 0, 0, 1), Vec2(1, 0));
	m_vVertexList[6] = PNCT_VERTEX(Vec3(vMax.x, vMax.y, vMax.z), Vec3(0, 0, 1), Vec4(1, 0, 0, 1), Vec2(0, 0));
	m_vVertexList[7] = PNCT_VERTEX(Vec3(vMax.x, vMin.y, vMax.z), Vec3(0, 0, 1), Vec4(1, 0, 0, 1), Vec2(0, 1));
	// Left
	m_vVertexList[8] = PNCT_VERTEX(Vec3(vMin.x, vMin.y, vMax.z), Vec3(0, 0, 1), Vec4(1, 0, 0, 1), Vec2(0, 1));
	m_vVertexList[9] = PNCT_VERTEX(Vec3(vMin.x, vMax.y, vMax.z), Vec3(0, 0, 1), Vec4(1, 0, 0, 1), Vec2(0, 0));
	m_vVertexList[10] = PNCT_VERTEX(Vec3(vMin.x, vMax.y, vMin.z), Vec3(0, 0, -1), Vec4(1, 0, 0, 1), Vec2(1, 0));
	m_vVertexList[11] = PNCT_VERTEX(Vec3(vMin.x, vMin.y, vMin.z), Vec3(0, 0, -1), Vec4(1, 0, 0, 1), Vec2(1, 1));
	// Right
	m_vVertexList[12] = PNCT_VERTEX(Vec3(vMax.x, vMin.y, vMin.z), Vec3(0, 0, -1), Vec4(1, 0, 0, 1), Vec2(0, 1));
	m_vVertexList[13] = PNCT_VERTEX(Vec3(vMax.x, vMax.y, vMin.z), Vec3(0, 0, -1), Vec4(1, 0, 0, 1), Vec2(0, 0));
	m_vVertexList[14] = PNCT_VERTEX(Vec3(vMax.x, vMax.y, vMax.z), Vec3(0, 0, 1), Vec4(1, 0, 0, 1), Vec2(1, 0));
	m_vVertexList[15] = PNCT_VERTEX(Vec3(vMax.x, vMin.y, vMax.z), Vec3(0, 0, 1), Vec4(1, 0, 0, 1), Vec2(1, 1));
	// UP
	m_vVertexList[16] = PNCT_VERTEX(Vec3(vMin.x, vMax.y, vMin.z), Vec3(0, 0, -1), Vec4(1, 0, 0, 1), Vec2(0, 1));
	m_vVertexList[17] = PNCT_VERTEX(Vec3(vMin.x, vMax.y, vMax.z), Vec3(0, 0, 1), Vec4(1, 0, 0, 1), Vec2(0, 0));
	m_vVertexList[18] = PNCT_VERTEX(Vec3(vMax.x, vMax.y, vMax.z), Vec3(0, 0, 1), Vec4(1, 0, 0, 1), Vec2(1, 0));
	m_vVertexList[19] = PNCT_VERTEX(Vec3(vMax.x, vMax.y, vMin.z), Vec3(0, 0, -1), Vec4(1, 0, 0, 1), Vec2(1, 1));
	// Bottom
	m_vVertexList[20] = PNCT_VERTEX(Vec3(vMin.x, vMin.y, vMax.z), Vec3(0, 0, 1), Vec4(1, 0, 0, 1), Vec2(0, 1));
	m_vVertexList[21] = PNCT_VERTEX(Vec3(vMin.x, vMin.y, vMin.z), Vec3(0, 0, -1), Vec4(1, 0, 0, 1), Vec2(0, 0));
	m_vVertexList[22] = PNCT_VERTEX(Vec3(vMax.x, vMin.y, vMin.z), Vec3(0, 0, -1), Vec4(1, 0, 0, 1), Vec2(1, 0));
	m_vVertexList[23] = PNCT_VERTEX(Vec3(vMax.x, vMin.y, vMax.z), Vec3(0, 0, 1), Vec4(1, 0, 0, 1), Vec2(1, 1));


	CreateVertexBuffer();
	CreateIndexBuffer();

	m_vIwList.resize(24);
	for (int i = 0; i < 24; i++)
	{
		m_vIwList[i].i1[0] = 1.0f;
		m_vIwList[i].w1[0] = 1.0f;
	}

	CreateIwBuffer();
}
